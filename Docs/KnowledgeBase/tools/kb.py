#!/usr/bin/env python3
"""本地知识库索引、漂移检查与全文搜索。仅使用 Python 标准库。"""
from __future__ import annotations

import argparse
from collections import defaultdict
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
from urllib.parse import quote, unquote

KB = Path(__file__).resolve().parents[1]
ROOT = KB.parents[1]
REF = KB / 'Reference'
SNAPSHOT = REF / 'snapshot.json'

PURPOSE = {
    'HodgeAssetManager': '资产入口、GameData 缓存、启动任务、同步加载和加载进度。Cue 初始化钩子仍需接通。',
    'HodgeAssetManagerStartupJob': '封装启动任务与进度权重，供 AssetManager 执行启动工作。',
    'HodgeGameData': '全局伤害、治疗、动态 Tag GE 的软类引用配置；需编辑器核对实际赋值。',
    'HodgePawnData': 'PawnClass、AbilitySets、TagRelationshipMapping、InputConfig、DefaultCameraMode 配置。',
    'HodgeAbilitySet': '权威端批量授予属性集、技能和 GE，并用句柄集合撤销。',
    'HodgeExperienceDefinition': '声明玩法所需插件、默认 PawnData、直接 Actions 和组合 ActionSets。',
    'HodgeExperienceActionSet': '复用 GameFeature 插件和动作配置的数据资产。',
    'HodgeExperienceManager': '管理编辑器等场景的 GameFeature 使用/停用协调，不是挂载在 GameState 的组件。',
    'HodgeExperienceManagerComponent': 'GameState 上的 Experience 复制、资源加载、插件激活、Action 执行与 Loaded 委托。',
    'HodgeGameModeBase': '服务器玩法选择、等待 Experience、取得 PawnData、默认 Pawn 生成与重生。数据注入仍注释。',
    'HodgeGameInstanceBase': '注册 Init State 顺序、主控制器访问和全局生命周期扩展。',
    'HodgeGameStateBase': 'GameState 基础扩展生命周期。',
    'HodgeGameState': '创建 ExperienceManager 和世界状态 ASC，处理游戏状态复制/扩展。',
    'HodgePlayerStateBase': 'PlayerState ModularGameplay Receiver 注册、注销及组件 Reset/CopyProperties。',
    'HodgePlayerState': '玩家 ASC、HealthSet、PawnData、阵营/标签栈等持有者。基础能力授予循环仍停用。',
    'HodgePlayerControllerBase': '把控制器和 Pawn 生命周期桥接到 LocalPlayer 委托。技能输入每帧消费尚未接入。',
    'HodgeLocalPlayerBase': '本地玩家对象及控制器、PlayerState、Pawn 就绪事件桥。',
    'HodgeHUDBase': '项目 HUD 基类扩展入口，不代表 CommonUI 已完成。',
    'HodgeCharacterBase': '原生 Character 基础、替换移动组件、Receiver 生命周期。EndPlay 事件配对需修正。',
    'HodgeCombatCharacter': 'PawnExtension、相机、ASC 查询、移动标签、复制与死亡占位逻辑。',
    'HodgeHeroCharacter': '玩家角色，当前保留 PossessedBy / OnRep_PlayerState 直接初始化 ASC 的旧入口。',
    'HodgeEnemyCharacter': '敌人移动与自动 AI 控制配置，尚未完成敌人 ASC 初始化。',
    'HodgePawnExtensionComponent': 'Pawn 数据复制和 Init State 协调、ASC 关联与解除、就绪委托。',
    'HodgeHeroComponent': '玩家初始化、输入与相机的迁移草稿；当前文件全部注释，不是有效反射类。',
    'HodgeCharacterMovementComponent': 'CharacterMovement 扩展、地面距离、加速度和能力系统相关移动入口。',
    'HodgeActorComponentBase': '通用 ActorComponent 基础访问与扩展。',
    'HodgeCombatComponentBase': '战斗组件占位，当前构造关闭 Tick，尚无完整命中或连招实现。',
    'HodgeInteractionComponentBase': '交互组件基础占位；完整扫描、交互规则与 UI 需另行实现。',
    'HodgeMovementComponentBase': '通用移动组件基础占位，与 CharacterMovement 派生类需区分。',
    'HodgeAbilitySystemComponent': 'Tag 输入缓存、激活组、关系映射、全局注册、失败通知与动态 Tag GE。',
    'HodgeGameplayAbility': '项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。',
    'HodgeAbilityCost': '自定义额外能力消耗的扩展契约。',
    'HodgeAbilityTagRelationshipMapping': '数据驱动的能力阻断、取消与激活条件关系。',
    'HodgeGameplayEffectContext': '项目 GE 上下文与序列化扩展；需要 Globals 分配入口配套。',
    'HodgeGameplayCueManager': '项目 Cue 资源加载/预加载管理实现；默认管理器配置尚未接通。',
    'HodgeGlobalAbilitySystem': '世界级全局能力/效果授予及 ASC 注册表。',
    'HodgeHealthSet': 'Health、MaxHealth、BaseDamage、BaseHeal；复制与夹取有效，GE 结算主体注释。',
    'HodgeAttributeSet': '项目 AttributeSet 基础和 ASC 访问。',
    'HodgeGameplayTags': '原生 GameplayTag 注册和移动状态标签映射。标签存在不等于对应玩法实现。',
    'GameplayTagStack': '带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。',
    'HodgeInputComponent': '基于 Tag 的 Native/Ability Action 绑定和句柄移除；映射辅助函数仍占位。',
    'HodgeInputConfig': 'NativeInputActions / AbilityInputActions 的 IA 与 Tag 数据配置及查询。',
    'HodgeInputModifiers': '输入数值处理扩展；具体启用情况由 IA/IMC 资产决定。',
    'HodgeInputUserSettings': 'Enhanced Input 用户设置派生入口；须核对实际设置类配置。',
    'HodgePlayerMappableKeyProfile': '玩家键位 Profile 扩展。',
    'HodgeAimSensitivityData': '瞄准灵敏度数据映射。',
    'HodgeCameraComponent': '相机模式栈宿主与最终视图输出，依赖默认模式委托。',
    'HodgeCameraMode': '相机视图、模式实例、混合和模式栈。',
    'HodgeCameraMode_ThirdPerson': '第三人称偏移与防穿透逻辑。',
    'HodgePenetrationAvoidanceFeeler': '相机防穿透探测参数结构。',
    'HodgePlayerCameraManager': '项目相机管理器；实际控制器类选择需验证。',
    'HodgeUICameraManagerComponent': 'UI 相机管理扩展，不代表 UI 系统已接入。',
    'HodgeCameraAssistInterface': '相机辅助接口契约。',
    'HodgeAnimInstance': 'ASC GameplayTag 属性映射和 GroundDistance 更新。',
    'HodgeActorBase': '项目 Actor 基类扩展。',
    'HodgeAbilitySourceInterface': '技能来源与相关计算契约。',
    'LoadingProcessInterface': '加载状态/原因查询契约；不是独立加载界面。',
    'GameFeatureAction_WorldActionBase': '按游戏世界和激活上下文组织 Action 生命周期。',
    'GameFeatureAction_AddAbilities': '面向配置 Actor 授予能力、属性与 AbilitySet，维护撤销句柄。',
    'GameFeatureAction_AddInputBinding': '额外 InputConfig 扩展；依赖 Hero 的主体和添加分支仍注释。',
    'GameFeatureAction_AddInputContextMapping': 'IMC 与本地玩家/设置集成，Controller 扩展添加分支仍注释。',
    'GameFeatureAction_AddGameplayCuePath': '声明和校验 Cue 路径配置；Policy 内实际增删路径仍注释。',
    'GameFeatureAction_AddWidget': 'Widget 注入迁移草稿，当前实现停用。',
    'GameFeatureAction_SplitscreenConfig': 'GameFeature 激活期间的分屏策略调整。',
    'HodgeGameFeaturePolicy': 'GameFeature 项目策略和资源加载观察扩展。',
}


def read(path: Path) -> str:
    return path.read_text(encoding='utf-8-sig', errors='replace')


def code_only(text: str) -> str:
    # 保留字符串与换行，去除 // 和 /* */ 注释，避免把草稿作为调用者。
    pattern = r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*[\s\S]*?\*/'
    def replace(m):
        s = m.group(0)
        return re.sub(r'[^\n]', ' ', s) if s.startswith('//') or s.startswith('/*') else s
    return re.sub(pattern, replace, text)


def link(path: Path, origin: Path = REF) -> str:
    return quote(os.path.relpath(path, origin).replace('\\', '/'), safe='/.-_')


def files() -> list[Path]:
    result = list((ROOT / 'Source').rglob('*')) + list((ROOT / 'Config').rglob('*'))
    result += list((ROOT / 'Content/Main').rglob('*'))
    result += [ROOT / 'Hodgepodge.uproject', ROOT / 'README.md', ROOT / 'LYRA_RUNTIME_FLOW.md',
               ROOT / 'LYRA_LEARNING_GUIDE.md', ROOT / 'UE5 开放世界动作 RPG 架构方案 V2.md']
    return sorted({p for p in result if p.is_file()})


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def git(*args: str) -> str:
    p = subprocess.run(['git', '-C', str(ROOT), *args], capture_output=True, encoding='utf-8', errors='replace')
    return p.stdout.strip() if p.returncode == 0 else '(不可用)'


def write(name: str, title: str, body: str) -> None:
    (REF / name).write_text(f'# {title}\n\n[知识库首页](../README.md) · [参考入口](README.md)\n\n'
                          '> 自动生成的静态导航；行为结论以人工章节和源码为准。\n\n' + body.rstrip() + '\n', encoding='utf-8')


def refresh() -> int:
    REF.mkdir(parents=True, exist_ok=True)
    source = sorted(p for p in (ROOT / 'Source').rglob('*') if p.suffix in {'.h', '.cpp', '.cs'})
    groups = defaultdict(list)
    index = ['索引排除第三方插件和构建产物；行号为生成时位置。注释已排除，但没有求值预处理条件。\n']
    for path in source:
        rel = path.relative_to(ROOT).as_posix()
        parts = path.relative_to(ROOT / 'Source').parts
        group = parts[2] if len(parts) > 3 and parts[1] in ('Public', 'Private') else 'Module'
        groups[group].append(path)
        active = code_only(read(path))
        functions = [(m.group(1), active.count('\n', 0, m.start()) + 1) for m in re.finditer(
            r'(?m)^(?:[\w:<>,*&]+[ \t]+)*([AUFI]?\w+::[~\w]+)\s*\(', active)]
        index.append(f'## {path.name}\n\n[{rel}]({link(path)})\n')
        index.append(PURPOSE.get(path.stem, '模块定义或基础代码；请查看对应文件。') + '\n')
        if not active.strip():
            index.append('状态：文件无有效非注释内容。\n')
        if functions:
            index += [f'- `{name}` — L{line}' for name, line in functions]
            index.append('')
    write('source-index.md', '源码文件与有效定义索引', '\n'.join(index))
    ref_index = ['## 按系统打开\n']
    for group, paths in sorted(groups.items()):
        name = 'source-' + group.lower() + '.md'
        ref_index.append(f'- [{group}：{len(paths)} 个文件]({name})')
        body = ['每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。\n']
        for path in paths:
            text = read(path)
            active = code_only(text)
            body += [f'## {path.name}\n', PURPOSE.get(path.stem, '模块或基础类型入口。') + '\n',
                     f'源码：[{path.relative_to(ROOT).as_posix()}]({link(path)})\n']
            includes = re.findall(r'^\s*#include\s+"([^"]+)"', active, re.M)
            local = []
            for inc in includes:
                candidate = ROOT / 'Source/Hodgepodge/Public' / inc
                if candidate.is_file():
                    local.append(f'[{inc}]({link(candidate)})')
            if local:
                body.append('项目内直接 include（不是运行调用关系）：' + '、'.join(local) + '\n')
            lines = [(i, line.rstrip()) for i, line in enumerate(active.splitlines(), 1) if line.strip()]
            if not lines:
                body.append('**全部为注释或空白；无有效声明/实现。**\n')
            elif path.suffix == '.h':
                body += ['有效头文件声明摘录（未展开宏，未求值预处理分支）：\n', '```cpp',
                         *[f'{i:4}: {line}' for i, line in lines], '```\n']
            else:
                symbols = [(i, line.strip()) for i, line in lines if re.match(
                    r'^(?:[\w:<>,*&]+[ \t]+)*[AUFI]?\w+::[~\w]+\s*\(', line)]
                body += ['定义候选（多行签名仅展示首行）：\n', *[f'- L{i}: `{s}`' for i, s in symbols], '']
        write(name, f'{group} 源码参考', '\n'.join(body))
    ref_index += ['\n## 数据与配置\n', '- [源码总索引](source-index.md)', '- [GameplayTag](gameplay-tags.md)',
                  '- [资产文件清单](assets.md)', '- [配置索引](config.md)', '- [扫描快照](snapshot.md)']
    write('README.md', '自动参考索引', '\n'.join(ref_index))

    tags = defaultdict(list)
    for path in source:
        if path.suffix not in ('.h', '.cpp'):
            continue
        active = code_only(read(path))
        for m in re.finditer(r'UE_DEFINE_GAMEPLAY_TAG(?:_STATIC|_COMMENT)?\s*\(\s*(\w+)\s*,\s*"([^"]+)"', active):
            tags[m.group(2).split('.')[0]].append((m.group(2), m.group(1), path, active.count('\n', 0, m.start()) + 1))
    body = ['此表提取有效原生注册宏；不解析资产或引擎标签。标签名称不是对应技能/系统已实现的证明。\n']
    for group, entries in sorted(tags.items()):
        body.append(f'## {group}\n')
        for tag, symbol, path, line in sorted(entries):
            body.append(f'- `{tag}` — `{symbol}`；[{path.name}]({link(path)}) L{line}')
        body.append('')
    write('gameplay-tags.md', 'GameplayTag 原生注册索引', '\n'.join(body))

    assets = sorted(p for p in (ROOT / 'Content/Main').rglob('*') if p.is_file())
    body = ['仅确认磁盘路径与大小，未解析二进制资产。IA 的轴类型、蓝图父类、DA 引用等请在编辑器验证。\n']
    last = None
    for path in assets:
        parent = path.parent.relative_to(ROOT / 'Content/Main').as_posix()
        if last != parent:
            body.append(f'\n## {parent}\n')
            last = parent
        game_path = '/Game/' + path.relative_to(ROOT / 'Content').with_suffix('').as_posix()
        body.append(f'- [{path.name}]({link(path)}) — `{game_path}`；{path.stat().st_size:,} 字节')
    body.append('\n## Content 顶层资源分布\n')
    counts = defaultdict(int)
    for p in (ROOT / 'Content').rglob('*'):
        if p.is_file():
            counts[p.relative_to(ROOT / 'Content').parts[0]] += 1
    body += [f'- `{key}`：{value:,} 个文件（包含源资源，不等于 UE 资产数）。' for key, value in sorted(counts.items())]
    write('assets.md', '资产文件清单', '\n'.join(body))

    body = ['这是项目默认配置摘录，带行号；凭据类字段值已隐去。不包含用户 Saved/Config、引擎配置或地图蓝图覆盖。\n']
    for path in sorted((ROOT / 'Config').glob('*.ini')):
        body += [f'## {path.name}\n', f'[打开源配置]({link(path)})\n', '```ini']
        for i, line in enumerate(read(path).splitlines(), 1):
            if not line.strip():
                continue
            if '=' in line and re.search(r'token|password|secret|apikey|api_key|privatekey', line.split('=', 1)[0], re.I):
                line = line.split('=', 1)[0] + '=[REDACTED]'
            body.append(f'{i:4}: {line}')
        body.append('```\n')
    write('config.md', '配置逐节索引', '\n'.join(body))

    tracked = files()
    data = {'schema': 1, 'generated_at_utc': datetime.now(timezone.utc).isoformat(),
            'git_head': git('rev-parse', 'HEAD'),
            'scope': 'Source/**, Config/**, Content/Main/**, uproject and four root documents; excludes third-party plugins',
            'files': {p.relative_to(ROOT).as_posix(): digest(p) for p in tracked}}
    SNAPSHOT.write_text(json.dumps(data, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    status = git('status', '--short', '--untracked-files=no')
    write('snapshot.md', '扫描快照', f'''## 生成信息

- UTC：{data['generated_at_utc']}
- Git HEAD：`{data['git_head']}`
- 源文件（h/cpp/cs）：{len(source)}
- Content/Main 文件：{len(assets)}
- 原生标签注册条目：{sum(map(len, tags.values()))}
- 漂移跟踪文件：{len(tracked)}

## 生成时已有的受 Git 跟踪修改

```text
{status or '(无)'}
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

没有运行 Unreal 编译、PIE、打包或蓝图数据解析。Content/Main 之外只统计顶层文件数量，不做引用结论。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
''')
    print(f'已刷新 {len(list(REF.glob("*.md")))} 份参考文档；{len(source)} 个源文件，{len(assets)} 个 Main 文件。')
    return 0


def check() -> int:
    errors = []
    docs = sorted(KB.rglob('*.md'))
    for path in docs:
        text = re.sub(r'```[\s\S]*?```', '', read(path))
        for m in re.finditer(r'\[[^\]\n]*\]\((<[^>]+>|[^)\s]+)\)', text):
            target = m.group(1).strip('<>')
            if re.match(r'^[a-zA-Z][\w+.-]*:', target) or target.startswith('#'):
                continue
            target = unquote(target.split('#')[0])
            if target and not (path.parent / target).resolve().exists():
                errors.append(f'{path.relative_to(KB)}: 链接不存在 {target}')
    drift = []
    if SNAPSHOT.exists():
        old = json.loads(read(SNAPSHOT))['files']
        new = {p.relative_to(ROOT).as_posix(): digest(p) for p in files()}
        for name in sorted(old.keys() | new.keys()):
            if old.get(name) != new.get(name):
                kind = '新增' if name not in old else '删除' if name not in new else '修改'
                drift.append(f'{kind}: {name}')
    else:
        errors.append('缺少 snapshot.json，请先 refresh。')
    for entry in errors + drift:
        print(entry)
    print(f'检查 {len(docs)} 份 Markdown：{len(errors)} 个链接/快照错误，{len(drift)} 个源码/配置/资产漂移。')
    return 1 if errors or drift else 0


def search(query: str, limit: int) -> int:
    matches = 0
    ordered = sorted(KB.rglob('*.md'), key=lambda p: ('Reference' in p.parts, str(p)))
    for path in ordered:
        for i, line in enumerate(read(path).splitlines(), 1):
            if query.casefold() in line.casefold():
                print(f'{path.relative_to(ROOT).as_posix()}:{i}: {line[:280]}')
                matches += 1
                if matches >= limit:
                    print(f'达到显示上限 {limit}，用 --limit 扩大。')
                    return 0
    print(f'共 {matches} 处匹配。')
    return 0


def main() -> int:
    if not sys.stdout.isatty():
        sys.stdout.reconfigure(encoding='utf-8')
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='command', required=True)
    sub.add_parser('refresh', help='只刷新自动参考、清单和快照')
    sub.add_parser('check', help='检查本地 Markdown 链接与扫描文件漂移')
    s = sub.add_parser('search', help='本地全文搜索，优先人工章节')
    s.add_argument('query')
    s.add_argument('--limit', type=int, default=30)
    args = parser.parse_args()
    if args.command == 'refresh':
        return refresh()
    if args.command == 'check':
        return check()
    if args.limit < 1:
        parser.error('--limit 必须为正数')
    return search(args.query, args.limit)


if __name__ == '__main__':
    sys.exit(main())
