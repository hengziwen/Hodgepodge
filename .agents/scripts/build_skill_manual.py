"""从共用目录生成完整使用手册，并只读校验条目覆盖、来源、链接和组合引用。"""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import json
import os
from pathlib import Path
import re
import sys
from urllib.parse import unquote, urlparse

PROJECT = Path(__file__).resolve().parents[2]
AGENTS = PROJECT / ".agents"
CATALOG = AGENTS / "skill-catalog.json"
ROUTING = AGENTS / "skill-routing.md"
MANUAL = PROJECT / "Docs/Guides/skills-usage-manual.md"
SCOPE_LABELS = {"system": "系统", "personal": "个人", "plugin": "插件", "repository": "项目 UE5.5.4"}


def anchor(identifier: str) -> str:
    """使用显式 ASCII 锚点，避免插件冒号和大小写造成跳转歧义。"""
    return "skill-" + identifier.lower().replace(":", "-")


def move_links(text: str, original: Path, destination: Path) -> str:
    """嵌入短规则时按原文位置解析链接，避免相对路径指向错误目录。"""
    def replace(match: re.Match) -> str:
        target = match.group(1)
        if urlparse(target).scheme or target.startswith("#"):
            return match.group(0)
        filename, separator, fragment = target.partition("#")
        relative = Path(os.path.relpath((original.parent / filename).resolve(), destination.parent)).as_posix()
        return "](" + relative + (separator + fragment if separator else "") + ")"
    return re.sub(r"\]\(([^)]+)\)", replace, text)


def render(catalog: dict) -> str:
    """生成面向用户与助手的同一份手册，正文判定规则只维护在目录和短规则中。"""
    entries = catalog["skills"]
    lines = ["# 全部可用 Skill 使用手册", "", f"快照日期：{catalog['snapshot_date']}；适用：Hodgepodge 当前 Codex 会话。", "",
             f"本手册覆盖 **{catalog['total']} 个可用 skill**：项目 UE skill {catalog['repository_count']} 个，系统/个人/插件 skill {catalog['other_count']} 个。每项说明功能、适用场景、全部必要触发条件、识别信号、排除条件、前置能力、组合方式、调用示例和验收边界。", "",
             "这里的“可用”是本次会话清单提供且已读取到真实 SKILL.md；不是“所有外部工具已经连接”，也不是“所有上游示例已经移植”。磁盘上的未公开内部技能不纳入，例如仅存在于系统目录、未在本次清单公开的 review-agent；`.agents/skill-sources/` 中的上游副本不重复计数。", "",
             "## 1 阅读与调用方法", "",
             "- **默认只说目标。** 例如“角色重生后生命条不刷新，修一下”，助手先识别 UE HUD、重生、ASC 绑定，再选择 UI 主 skill 与必要的 GAS/委托辅助，不要求你提供 skill 名称。",
             "- **需要指定时用自然语言。** 每项都给出“请使用 完整标识 完成：具体任务”的例子。普通 skill 在支持的界面也可用 `$skill-name`；插件以当前选择器中的完整条目为准。不要假定冒号命名可以直接拼成工具名或所有界面都接受同一种快捷语法。",
             "- **名称是选择器，不是执行函数。** 助手读选中项的 SKILL.md 后调用当前真正存在的工具；例如 `unreal-mcp` 在本项目走实际桥接，不是把字符串当作接口执行。",
             "- **全部必要条件必须同时满足。** “识别信号”只是线索，不是看到一个词就触发；不要求逐字命中，可以根据代码路径、附件、上一轮对象与请求含义判断。",
             "- **显式指定仍需范围适用。** 指定 skill 不能凭空增加工具、权限或商业资产。若指定项与目标明显不符，解释冲突并按真实目标完成可独立的工作；只有影响结果的缺失信息才澄清。",
             "- **调用前重读必要正文。** 本手册负责选对工作流；具体工具顺序、脚本、渲染、配对清理和产物验证以真实 skill、项目规则和更高层指令为准。", "",
             "### 1.1 维护入口", "",
             "- [短版自动选择规则](../../.agents/skill-routing.md)：任务开始时读取，避免全量装入 143 项细节。",
             "- [目录数据](../../.agents/skill-catalog.json)：逐项规则、来源路径/摘要和组合关系的维护源。",
             "- [UE 安装与版本适配](../../.agents/UE55_SKILLS.md)：108 项来源、重名和 UE5.5.4 限制。",
             "- [仓库 AGENTS.md](../../AGENTS.md)：自动选择的项目入口与长期工程约定。", "",
             "## 2 自动选择与组合规则", ""]
    routing = ROUTING.read_text(encoding="utf-8")
    body = "\n".join(routing.splitlines()[4:])
    body = re.sub(r"^## ", "### ", body, flags=re.M)
    lines += [move_links(body, ROUTING, MANUAL), "", "## 3 常见请求判定示例", "",
              "这些是后续行为检查和人工复核的正反场景，不表示本次执行了全部技能，也不是自然语言分类准确率测试。", ""]
    cases = [
        ("给角色增加火球技能和冷却", "ue-gameplay-abilities 主导；不选择 Agent Skill 创建。若还明确需要图标，才加 imagegen。"),
        ("把团队代码审查流程保存成一个 skill", "skill-creator 主导；需要长期强制约束才补 create-rule；不选择 GAS。"),
        ("继续刚才的工作", "继承未完成目标和上一轮执行面；不因句子没有领域词重新开始其他任务。"),
        ("只是解释示例 /shell git status，不要执行", "不调用 shell、不执行命令；引用和否定语义优先。"),
        ("当前 Excel 的这个选区加一列公式", "excel-live-control；先验证同一工作簿与连接，不静默导出 XLSX。"),
        ("生成一份可下载的 .xlsx", "文件型 Spreadsheets；不要求用户打开 Excel 桌面程序。"),
        ("在仓库写 Markdown 文档", "普通文件工具；可按文档内容用领域 skill，但不调用 Word 渲染，也不创建无关 Canvas。"),
        ("根据模板做一份 Word", "documents；不是 template-creator。只有要求以后复用时才创建模板 skill。"),
        ("给我一个网站，只在本地保存", "Sites local-only；不注册或部署，不额外要求发布批准。"),
        ("在聊天里演示延迟对技能预测的影响", "visualize；需要 UE/GAS 事实时用其专项核实，不因此建立托管站点。"),
        ("制作正式论文的 SVG 统计图", "标准绘图工具；不把内嵌互动展示或图片生成当成可导出科学绘图。"),
        ("给 UE 游戏加一个宠物跟随 AI", "UE AI/移动；不调用 ChatGPT Pets 插件。"),
        ("把 ChatGPT 现有伙伴改名", "update-pet 元数据路线；不生成图集、不新建替代 ID、不自动激活。"),
        ("把 ChatGPT 伙伴隐藏起来", "pets 的取消选择路线；不解释成删除自定义宠物。"),
        ("在 Notion 更新一条已知任务状态", "直接已连接工具；不新建任务库、不启动全量跨来源研究。"),
        ("根据 Notion 的多个方案比较本周变化", "research-documentation 或 weekly-project-digest 按研究/报告期目标择主；不是单页摘要。"),
        ("创建一个以后复用的代码审查子代理", "create-subagent 配置；先尊重宿主显式加载限制，不等同当场运行代理。"),
        ("请派三个代理分别审查这三个模块", "原生协作工具属于明确的本轮分工；不为此自动保存持久子代理类型。"),
        ("每周一提醒我跑测试", "原生自动化工具；不是 UE FTimerManager，也不是 afterFileEdit Hook。"),
        ("安装一个 .uplugin", "UE 插件/模块；不是 Agent Skill 安装或 Codex 外部应用连接。"),
        ("5.8 文档中的这个接口在 5.5 能用吗", "ue-navigating-engine-source，按实际功能补最窄领域；不升级引擎来满足示例。"),
        ("已有 UDS 的天空时间不同步", "ue-uds-time 先验证商业资产/版本；复制专项处理权威时间。没有资产则只给具体依赖缺口。"),
        ("解释这份 PR 的 CI 状态", "只读原生 GitHub 工具；不把一次状态查询扩大为 babysit 持续维护。"),
        ("拆成三个 PR，先给我具体方案", "split-to-prs 先给可审查方案；未获得所需授权前不建分支、提交、推送或开 PR。"),
    ]
    for number, (request, selection) in enumerate(cases, 1):
        lines += [f"{number}. **请求：**“{request}”。**判定：**{selection}"]
    lines += ["", "## 4 查阅索引", ""]
    for index, group in enumerate(catalog["groups"], 1):
        members = [entry for entry in entries if entry["group"] == group]
        lines += [f"- [5.{index} {group}（{len(members)}）](#group-{index})"]
    lines += ["", "### 按名称检索", ""]
    for entry in sorted(entries, key=lambda item: item["id"].lower()):
        lines.append(f"- [{entry['id']}](#{anchor(entry['id'])}) — {entry['title']}。")

    lines += ["", "## 5 逐项使用说明", ""]
    for index, group in enumerate(catalog["groups"], 1):
        members = [entry for entry in entries if entry["group"] == group]
        lines += ["", f'<a id="group-{index}"></a>', "", f"### 5.{index} {group}", ""]
        for entry in members:
            identifier = entry["id"]
            lines += [f'<a id="{anchor(identifier)}"></a>', "", f"#### {identifier}", "",
                      f"**功能：**{entry['function'].rstrip('。')}。", "",
                      f"**适用场景：**{entry['scenario']}", "",
                      "**自动触发条件（全部满足）：**", ""]
            lines += ["- " + condition.rstrip("。") + "。" for condition in entry["conditions"]]
            lines += ["", "**识别信号（可按语义匹配）：**" + "、".join(f"`{signal}`" for signal in entry["signals"]) + "。", "",
                      "**不触发与边界：**", ""]
            lines += ["- " + value.rstrip("。") + "。" for value in entry["exclude"]]
            lines += ["", f"**前置条件与实际可用性：**{entry['prerequisites']} {entry['availability']}", "",
                      f"**重叠选择：**{entry['preference']}", "", "**组合规则：**", ""]
            if entry["relations"]:
                lines += [f"- [{relation['skill']}](#{anchor(relation['skill'])})：{relation['when']}" for relation in entry["relations"]]
            else:
                lines += ["- 本条可独立执行；不默认叠加其他 skill。只有后续子任务产生明确的新交付或职责时按总规则重新选择。"]
            lines += ["", "**调用方式与正反例：**", "",
                      f"- 自动选择示例：“{entry['example']}” → 识别本条，不要求手动命名。",
                      f"- 可选的显式调用：“{entry['invocation']}”",
                      f"- 不触发示例：“{entry['counterexample']}” → 按真实对象、动作和排除条件转交或直接处理。"]
            if entry["source_policy"]["disable_model_invocation"]:
                lines += ["- 加载例外：原文件含 `disable-model-invocation=true`。自动识别意图不保证宿主允许隐式加载；如被限制，使用允许的显式入口，不暗改用户配置。"]
            lines += ["", f"**验收：**{entry['verification']}", "",
                      f"**真实入口与来源：**[{SCOPE_LABELS[entry['scope']]} SKILL.md]({entry['entry']})。"]
            if entry.get("reference"):
                relative = Path(os.path.relpath(PROJECT / entry["reference"], MANUAL.parent)).as_posix()
                lines += [f"本地适配细节见 [领域参考]({relative})；完整上游代码示例未整体移植/编译认证。"]
            lines += [""]
    lines += ["## 6 能力缺口和清单维护", "",
              "- 这份清单只描述本次会话的 143 项。新安装、移除或更新插件后重新盘点当前会话清单；不因磁盘上新增一个同名目录就断言其可调用。",
              "- 插件路径和个人路径属于本机快照。迁移机器先重新定位真实入口，保留完整标识、来源和边界；项目的 UE skill 可随仓库迁移，外部缓存不因此一起安装。",
              "- `disable-model-invocation` 和 `allow_implicit_invocation` 是不同来源/宿主的字段；是否生效必须按实际宿主核实。手册和规则都不是权限系统或自动执行 Hook。",
              "- 不可用的原生 UI、尚未连接的外部服务、没有安装的商业资产、缺失的新版引擎 API 均单独诊断。不要以换工具、绕过权限或伪造操作掩盖缺口。",
              "- 新的相近 skill 应补充正例、反例、首选关系与组合条件；不要只追加关键词。新自然语言说法无需逐字登记，助手仍按实际意图判断。当前会话新增的条目即使尚未收录，仍可按真实 description/SKILL.md 选择；记录目录待同步，不因此中断任务或顺手改写无关手册。",
              "- 修改 `.agents/skill-catalog.json` 或短规则后，在项目根目录执行 `python -X utf8 .agents/scripts/build_skill_manual.py` 更新本文件；执行同命令加 `--check` 只读检查生成一致性、143 项覆盖、来源摘要、锚点和组合引用。",
              "- 外部 SKILL.md 更新导致摘要变化时，先重读变更并同步逐项规则/来源摘要，再生成；不能直接刷新摘要掩盖未审查的触发或权限变更。",
              "- 本次文档验证不执行所有 skill，不产生平台自动选择准确率结论，也不代表 C++、蓝图、PIE、联机或打包测试通过。", ""]
    return "\n".join(lines)


def validate(catalog: dict, text: str) -> dict:
    """检查真实入口与文档完整性，不执行技能脚本或外部操作。"""
    errors = []
    entries = catalog["skills"]
    identifiers = {entry["id"] for entry in entries}
    if len(entries) != catalog["total"] or len(identifiers) != len(entries):
        errors.append("目录数量或标识唯一性不符")
    groups = Counter(entry["group"] for entry in entries)
    if set(groups) != set(catalog["groups"]):
        errors.append("分类缺失或有未登记分类")
    actual_repository = {path.parent.name for path in (AGENTS / "skills").glob("*/SKILL.md")}
    registered_repository = {entry["id"] for entry in entries if entry["scope"] == "repository"}
    if actual_repository != registered_repository:
        errors.append("项目发现目录与手册覆盖不一致")
    if len(registered_repository) != catalog["repository_count"]:
        errors.append("项目 skill 数量不符")
    required = ("function", "scenario", "conditions", "signals", "exclude", "prerequisites", "example",
                "counterexample", "invocation", "verification", "availability", "preference")
    relation_count = 0
    for entry in entries:
        for field in required:
            if not entry.get(field):
                errors.append(f"{entry['id']} 缺少 {field}")
        path = Path(entry["entry"])
        if not path.is_file() or hashlib.sha256(path.read_bytes()).hexdigest() != entry["source_sha256"]:
            errors.append(f"入口不存在或来源已变化：{entry['id']} {path}")
        if entry["example"] == entry["counterexample"]:
            errors.append(f"正反例相同：{entry['id']}")
        for relation in entry["relations"]:
            relation_count += 1
            if relation["skill"] not in identifiers or not relation["when"]:
                errors.append(f"组合引用不完整：{entry['id']}")
    anchors = set(re.findall(r'<a id="([^"]+)"></a>', text))
    if any(anchor(identifier) not in anchors for identifier in identifiers):
        errors.append("条目锚点覆盖不完整")
    for target in re.findall(r"(?<!!)\[[^\]\n]+\]\(([^)\n]+)\)", text):
        if re.match(r"^[A-Za-z]:/", target):
            if not Path(unquote(target)).is_file():
                errors.append(f"本机链接失效：{target}")
            continue
        if urlparse(target).scheme:
            continue
        filename, separator, fragment = target.partition("#")
        if not filename:
            if separator and fragment not in anchors:
                errors.append(f"内部锚点失效：{target}")
        elif not (MANUAL.parent / unquote(filename)).resolve().is_file():
            errors.append(f"文件链接失效：{target}")
    if errors:
        raise ValueError("\n".join(errors))
    return dict(total=len(entries), repository=len(registered_repository), other=len(entries) - len(registered_repository),
                groups=len(groups), examples=2 * len(entries), conditional_relations=relation_count,
                source_files="passed", links="passed", note="Document and source consistency only; not a model routing accuracy test.")


def main() -> int:
    """默认生成手册；--check 只读检查，便于文档变更验收。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    try:
        catalog = json.loads(CATALOG.read_text(encoding="utf-8"))
        expected = render(catalog)
        if args.check:
            if not MANUAL.is_file() or MANUAL.read_text(encoding="utf-8") != expected:
                raise ValueError("手册与维护源不同步，请先核实目录/短规则后重新生成")
        else:
            MANUAL.parent.mkdir(parents=True, exist_ok=True)
            MANUAL.write_text(expected, encoding="utf-8", newline="\n")
        print(json.dumps(validate(catalog, expected), ensure_ascii=False, indent=2))
        return 0
    except (OSError, ValueError, KeyError) as error:
        print(f"手册校验失败：{error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
