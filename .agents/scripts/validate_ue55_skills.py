"""只读校验 UE5.5 skill 的发现入口、原始内容、链接和本机源码依据。"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import sys
from urllib.parse import unquote, urlparse

import yaml


def validate(agents: Path, engine: Path | None = None, lyra: Path | None = None) -> dict:
    """返回可检查的安装结果；内容不完整或版本不匹配时明确失败。"""
    lock = json.loads((agents / "ue55-skills.lock.json").read_text(encoding="utf-8"))
    engine = engine or Path(lock["engine_root"])
    lyra = lyra or Path(lock["lyra_root"])
    errors: list[str] = []
    version = json.loads((engine / "Build/Build.version").read_text(encoding="utf-8-sig"))
    actual_version = ".".join(str(version[key]) for key in ("MajorVersion", "MinorVersion", "PatchVersion"))
    if actual_version != lock["engine"]:
        errors.append(f"引擎版本不匹配：{actual_version} != {lock['engine']}")
    if not (lyra / "LyraStarterGame.uproject").is_file():
        errors.append(f"Lyra 不可用：{lyra}")

    entries = lock["skills"]
    names = [entry["name"] for entry in entries]
    if len(set(names)) != len(names):
        errors.append("发现重名的可加载入口")
    actual = {p.parent.name for p in (agents / "skills").glob("*/SKILL.md")}
    if actual != set(names):
        errors.append(f"入口与清单不一致：缺少={set(names) - actual}，额外={actual - set(names)}")

    # 仅检查本地执行文档的链接，上游原始文档作为固定版本的完整快照单独校验。
    documents = [agents / "UE55_SKILLS.md", agents / "ue55-policy.md", agents / "ue-project-context.md"]
    for entry in entries:
        path = agents / "skills" / entry["name"] / "SKILL.md"
        if not path.is_file():
            continue
        documents.append(path)
        text = path.read_text(encoding="utf-8")
        match = re.match(r"\A---\n(.*?)\n---\n", text, re.S)
        if not match:
            errors.append(f"缺失 frontmatter：{path}")
            continue
        metadata = yaml.safe_load(match.group(1))
        name = metadata.get("name", "")
        if name != entry["name"] or not re.fullmatch(r"[a-z0-9]+(?:-[a-z0-9]+)*", name) or len(name) > 64:
            errors.append(f"名称非法或与目录不匹配：{path}")
        description = metadata.get("description", "")
        if not isinstance(description, str) or not description.strip() or len(description) > 1024:
            errors.append(f"描述非法：{path}")
        if metadata.get("metadata", {}).get("engine") != lock["engine"]:
            errors.append(f"入口目标版本不匹配：{path}")
        if len(text.splitlines()) > 500:
            errors.append(f"入口超过 500 行：{path}")
        if "../../ue55-policy.md" not in text or f"../../ue55-reference/{entry['domain']}.md" not in text:
            errors.append(f"未接入本地执行流程：{path}")

    anchor_count = 0
    for domain, definition in lock["domains"].items():
        documents.append(agents / "ue55-reference" / f"{domain}.md")
        for anchor in definition["anchors"]:
            path = engine / anchor["path"]
            if not path.is_file():
                errors.append(f"源码文件不存在：{path}")
                continue
            lines = path.read_text(encoding="utf-8-sig", errors="replace").splitlines()
            number = anchor["line"]
            if number > len(lines) or anchor["symbol"] not in lines[number - 1]:
                errors.append(f"源码锚点已变化：{path}:{number} {anchor['symbol']}")
            anchor_count += 1

    for document in documents:
        if not document.is_file():
            errors.append(f"本地参考不存在：{document}")
            continue
        text = document.read_text(encoding="utf-8")
        for target in re.findall(r"(?<!!)\[[^\]\n]+\]\(([^)\n]+)\)", text):
            target = target.strip("<>")
            if target.startswith("#") or urlparse(target).scheme:
                continue
            relative = unquote(target.split("#", 1)[0])
            if not (document.parent / relative).resolve().is_file():
                errors.append(f"链接失效：{document} -> {target}")

    source_count = 0
    for source in lock["sources"]:
        root = agents / "skill-sources" / source["vendor"]
        expected = source["files"]
        present = {p.relative_to(root).as_posix() for p in root.rglob("*") if p.is_file()}
        if present != set(expected):
            errors.append(f"上游文件清单变化：{source['vendor']}")
        for relative, expected_hash in expected.items():
            path = root / relative
            if not path.is_file() or hashlib.sha256(path.read_bytes()).hexdigest() != expected_hash:
                errors.append(f"原始文件内容变化：{path}")
            source_count += 1
        actual_count = sum(entry["vendor"] == source["vendor"] for entry in entries)
        if actual_count != source["count"]:
            errors.append(f"上游条目数量不符：{source['vendor']}")

    if errors:
        raise ValueError("\n".join(errors))
    return dict(skills=len(entries), domains=len(lock["domains"]), engine=actual_version,
                source_files=source_count, source_anchors=anchor_count, links="passed",
                validation_scope=lock["validation_scope"])


def main() -> int:
    """支持换机器时显式指定引擎和 Lyra 路径，不修改锁定清单。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--engine-root", type=Path)
    parser.add_argument("--lyra-root", type=Path)
    args = parser.parse_args()
    try:
        print(json.dumps(validate(Path(__file__).resolve().parents[1], args.engine_root, args.lyra_root),
                         ensure_ascii=False, indent=2))
        return 0
    except (OSError, ValueError, KeyError, yaml.YAMLError) as error:
        print(f"skill 校验失败：{error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
