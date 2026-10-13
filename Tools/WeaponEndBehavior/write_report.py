"""归档本轮通知结束方式的最终构建、原生测试和多端真实输入结果。"""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/WeaponEndBehavior'
OUT = ROOT / 'Docs/Validation'
native = json.loads((WORK / 'Automation/index.json').read_text(encoding='utf-8-sig'))
tests = [{'name': t['fullTestPath'], 'state': t['state']} for t in native['tests']]
assert all(t['state'] == 'Success' for t in tests)
assets = json.loads((WORK / 'asset-validation.json').read_text(encoding='utf-8'))
e2e = {m: json.loads((WORK / (m + '-e2e.json')).read_text(encoding='utf-8')) for m in ['single', 'listen', 'dedicated']}
assert assets['passed'] and all(r['passed'] for r in e2e.values())
files = [
    'Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h',
    'Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h',
    'Source/Hodgepodge/Private/Animation/HodgeCombatAnimNotifies.cpp',
    'Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h',
    'Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp',
    'Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h',
    'Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp',
    'Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationActor.cpp',
    'Source/Hodgepodge/Private/Tests/HodgeWeaponPresentationTests.cpp',
]
summary = {'build': {'Editor': {'exit': 0, 'log': 'Saved/WeaponEndBehavior/EditorBuild.log'},
                     'Game': {'exit': 0, 'log': 'Saved/WeaponEndBehavior/GameBuild.log'}},
           'native': tests, 'assets': assets,
           'e2e': {m: {'passed': r['passed'], 'checks': r['checks'], 'frames': len(r['samples']),
                       'evidence': 'Saved/WeaponEndBehavior/' + m + '-e2e.json'} for m, r in e2e.items()},
           'source_sha256': {f: hashlib.sha256((ROOT / f).read_bytes()).hexdigest() for f in files},
           'not_verified': ['Cook与打包', '独立Server可执行文件', '弱网压力或人工预测拒绝', '死亡重生专项PIE'],
           'git_committed': False}
(OUT / 'weapon-notify-end-behavior-2026-10-12-results.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding='utf-8')
text = '''# 武器手持通知结束方式验证（2026-10-12）

对应[武器表现设计修订](../Design/weapon-presentation.md#手持通知结束方式2026-10-12)和[攻击配置手册](../Guides/attack-ability-configuration.md)。按用户确认，“直接消失”指在手部立即开始现有材质淡出，不是瞬间隐藏。

## 配置与实现

- Hodge 武器手持区间新增 EndBehavior，Details 中显示“结束方式”；ReturnToBack 为“回背部后延迟淡出”，FadeAtHand 为“在手部直接淡出”。现有通知默认保留 ReturnToBack。
- 直接方式跳过 ReturnGraceSeconds、ReturnSeconds 和 HoverSeconds，使用武器 Profile 的 FadeSeconds（正式配置 0.35 秒）。可见 Mesh 保持手部挂接，检测 Mesh 不移动。
- 释放方式在 Begin 时随执行和区间句柄保存，正常 End、GA 结束及取消沿用同一配置；还有其他请求时不收回。批量结束同一执行按申请顺序清理，使用最新区间的配置，避免 TMap 顺序不确定。
- Fading/Hidden 的挂接方式通过既有复制快照 EndBehavior 区分，不新增 RPC。正在手部淡出时接新攻击，从当前透明度重新显现；旧句柄和旧阶段计时器不能收回新攻击。
- 死亡和卸装继续立即隐藏；旧路径及原资产默认行为保留。没有自动把普攻通知改成直接模式。

## 实际验证

- Editor、Game Win64 Development 常规构建均退出 0，未用 Live Coding 替代。
- Hodge.WeaponPresentation 三项原生测试均 Success：RequestsAndLifecycle、GeometrySeparation、新增 NotifyEndBehavior。新增用例覆盖默认策略、即刻进入 Fading、手部挂接、中间透明度、新攻击从当前透明度衔接、旧计时器、旧句柄、重叠请求、批量取消顺序和非法枚举。
'''
for mode, r in e2e.items():
    text += '- ' + mode + ' 真实输入 ' + str(len(r['checks'])) + ' 项通过，' + str(len(r['samples'])) + ' 帧记录；验证回背悬浮、直接淡出、淡出中接新攻击、真实 Shift/Dash 取消，各端挂接、实际 MID 参数与结束方式一致。\n'
text += '''- 验证四条现有武器通知恢复默认 ReturnToBack。当前 AM_Attack04_Montage 没有武器手持通知，本任务未擅自补建。
- PIE 测试临时改了已加载通知的模式；结束后恢复原值，并从原磁盘重新加载被 UE 标 dirty 的测试包，没有保存测试模式或通过 Git 回退资产。相关武器 Actor 与实例蓝图执行 Compile/Save，没有编辑器编译错误。
- 停止 PIE，恢复本轮开始时的地图、网络和性能选项，确认无未保存包。已有用户改动保留，未提交 Git。

## 执行命令与证据

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -NoShaderWorker -nosplash '-ExecCmds=Automation RunTests Hodge.WeaponPresentation' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/WeaponEndBehavior/Automation' '-abslog=E:/Project/Git/Hodgepodge/Saved/WeaponEndBehavior/Automation.log'
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WeaponEndBehavior/start_single.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WeaponEndBehavior/start_listen.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WeaponEndBehavior/start_dedicated.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WeaponEndBehavior/reload_test_assets.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WeaponEndBehavior/restore.py
```

每个网络模式结束先执行 Tools/WingAttack/stop_test.py，再重载测试包。原生报告和三模式逐帧数据位于 Saved/WeaponEndBehavior；代码 SHA、检查明细见[机器可读结果](weapon-notify-end-behavior-2026-10-12-results.json)。

## 问题与边界

测试临时设置通知属性时 UE 会标记 Montage dirty；首次切换网络模式被启动检查正确拒绝。新增校验与包重载步骤后复跑通过，避免测试配置被保存。已有其他攻击蒙太奇的重叠 BranchingPoint 警告保持原样。

本轮没有 Cook/打包、独立专服构建、弱网压力、人工预测拒绝或死亡重生的专项 PIE；死亡、卸装和旧请求清理由本轮原生生命周期回归覆盖。当前测试均在项目 UE 5.5.4 与实际正式装备配置下执行。
'''
(OUT / 'weapon-notify-end-behavior-2026-10-12.md').write_text(text, encoding='utf-8')
print(json.dumps({'native_tests': len(tests), 'e2e_checks': sum(len(r['checks']) for r in e2e.values()), 'default_notifications': len(assets['checks'])}, ensure_ascii=False))
