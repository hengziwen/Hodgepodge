"""将盒体校准、最终构建和实际命中测试归档为可追溯结果。"""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/WingBox'
OUT = ROOT / 'Docs/Validation'
cal = json.loads((WORK / 'configuration.json').read_text(encoding='utf-8'))
assets = json.loads((WORK / 'asset-validation.json').read_text(encoding='utf-8'))
native = []
for group in ['Automation-final', 'CombatAutomation-final']:
    data = json.loads((WORK / group / 'index.json').read_text(encoding='utf-8-sig'))
    native.extend({'name': t['fullTestPath'], 'state': t['state']} for t in data['tests'])
assert all(t['state'] == 'Success' for t in native)
damage = {m: json.loads((WORK / (m + '-damage.json')).read_text(encoding='utf-8')) for m in ['single', 'listen', 'dedicated']}
presentation = {m: json.loads((WORK / (m + '-e2e.json')).read_text(encoding='utf-8')) for m in damage}
assert assets['passed'] and all(r['passed'] for r in [*damage.values(), *presentation.values()])
paths = [
    'Source/Hodgepodge/Public/Animation/HodgeWingProfile.h',
    'Source/Hodgepodge/Public/Component/HodgeWingComponent.h',
    'Source/Hodgepodge/Private/Component/HodgeWingComponent.cpp',
    'Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h',
    'Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.cpp',
    'Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h',
    'Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.cpp',
    'Source/Hodgepodge/Private/Tests/HodgeWingAttackPresentationTests.cpp',
    'Source/Hodgepodge/Private/Tests/HodgeWingBoxHitTests.cpp',
]
def compact(result, suffix):
    return {'passed': result['passed'], 'checks': result['checks'], 'frames': len(result['samples']),
            'evidence': 'Saved/WingBox/' + result['mode'] + suffix}
summary = {'requirements': ['WING-HIT-01', 'WING-HIT-02', 'WING-HIT-03', 'WING-HIT-04'], 'calibration': cal,
           'build': {'Editor': {'exit': 0, 'log': 'Saved/WingBox/EditorBuild-complete.log'},
                     'Game': {'exit': 0, 'log': 'Saved/WingBox/GameBuild-complete.log'}},
           'native': native, 'assets': assets,
           'damage': {m: compact(r, '-damage.json') for m, r in damage.items()},
           'presentation': {m: compact(r, '-e2e.json') for m, r in presentation.items()},
           'source_sha256': {p: hashlib.sha256((ROOT / p).read_bytes()).hexdigest() for p in paths},
           'not_verified': ['独立Server可执行文件', 'Cook与打包', '弱网压力', '不同动画或缩放的盒体覆盖', '逐三角形碰撞'],
           'git_committed': False}
(OUT / 'wing-box-hit-2026-10-12-results.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding='utf-8')
text = '''# 新 Anzhu 翅膀 Box 攻击判定验证（2026-10-12）

对应[攻击限定翅膀设计 v1.2 第 12 节](../Design/attack-wing-presentation.md#12-翼面-box-攻击判定2026-10-12)，需求 WING-HIT-01～04。实际编辑入口和调试命令见[攻击配置手册第 9 节](../Guides/attack-ability-configuration.md#9-新-anzhu-翅膀的-box-配置)。

## 最终配置与行为

- 正式第 5 段 AM_Attack04_Montage 使用原有 NamedComponent/WingMesh 持续命中，Shape 改为 Box，锚点为实际骨骼 Bone_Wing000，Follow 跟随翼根。
- BoxHalfExtent=(67.642195, 33.136577, 77.690944) cm；LocalTransform.Location=(61.441557, 5.547495, -35.116657) cm，旋转零、缩放一。这是世界厘米半尺寸，在 0.8 网格缩放下按实际实体翼面测量，各轴已加 4 cm 余量。
- 沿原有 0.566667～0.866667 秒窗口采样 19 个姿势、97603 次实体顶点，排除第一个特效材质槽。保留当前 6 条通知，不修改显示窗口、动画映射、模型、骨架、材质、缩放或其他状态通知。
- 与手部共享 PoverWingFinisher/Strike，重复命中间隔零；伤害和受击继承原 Definition。模型本身仍 NoCollision，采用既有虚拟盒体 Sweep，而不是额外常驻碰撞组件。
- DA_PoverWing_Anzhu 开启 EnableAuthorityHitSampling。专服保留隐藏网格，不创建 MID、不自动推进翼动画；命中任务验证当前 GAS 执行及主蒙太奇后准备骨骼，在角色更新之后查询，首次与暂停恢复先重置几何历史。隐藏角色按需补刷新挂点，每帧最多一次。
- 取消立即停止命中；0.35 秒淡出继续作为纯表现，新攻击可以覆盖尾迹。客户端不上传可信命中结果。

## 本轮最终验证结果

'''
text += '- Editor/Game 常规构建退出 0；原生 ' + str(len(native)) + ' 项全部 Success（Hodge.Wing 8 项、Hodge.Combat 5 项）。新增 BoxHitGeometry 验证真实骨骼、偏移、盒内/盒外、关闭会话及缺失锚点；原集成增加非法执行和未启用权威采样的拒绝用例。\n'
text += '- 资产配置 ' + str(len(assets['checks'])) + ' 项通过，检查当前几何、真实锚点、同刀去重、默认伤害、通知数量和淡出保留。\n'
for mode, result in damage.items():
    detail = next(c['detail'] for c in result['checks'] if c['check'] == '翼盒单独造成伤害')
    distance = next(c['detail'] for c in result['checks'] if c['check'] == '目标避开手部球判定')
    text += '- ' + mode + ' 真实伤害：' + str(len(result['checks'])) + ' 项通过；HP ' + str(detail[0]) + ' → ' + str(detail[1]) + '，本刀仅一次下降；目标在伤害窗口与手部的最小距离约 ' + f'{distance:.2f}' + ' cm，避开手部球。范围外与取消/淡出阶段无伤害。\n'
text += '- Listen Server 检查远端客户端生命值复制；专服 PIE 检查隐藏采样网格与两个客户端生命值复制。\n'
for mode, result in presentation.items():
    text += '- ' + mode + ' 表现回归：' + str(len(result['checks'])) + ' 项通过；五段连击、实际翼骨运动、三槽材质渐变、自然收招、Dash 取消及空闲无额外翼求值。\n'
text += '''- 测试目标只在 PIE 内临时缩小到半径 5 cm、半高 6 cm，以隔离翼盒与手部；生产怪物资产没有修改。
- 执行组件蓝图和 WingText 编译，无编辑器编译错误；停止 PIE、恢复初始加载地图与性能设置，PIE 设为单人 Standalone，最终无未保存资产或地图。

## 命令与证据

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -NoShaderWorker -nosplash '-ExecCmds=Automation RunTests Hodge.Wing' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/WingBox/Automation-final' '-abslog=E:/Project/Git/Hodgepodge/Saved/WingBox/Automation-final.log'
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -NoShaderWorker -nosplash '-ExecCmds=Automation RunTests Hodge.Combat' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/WingBox/CombatAutomation-final' '-abslog=E:/Project/Git/Hodgepodge/Saved/WingBox/CombatAutomation-final.log'
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingAttack/configure_box.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingAttack/validate_box.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WingBox/start_damage_single.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WingBox/start_damage_listen.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WingBox/start_damage_dedicated.py
& E:/Python/python.exe Saved/WingBox/run_presentations.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WingBox/restore.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WingBox/restore_single.py
```

各伤害测试结束先执行 Tools/WingAttack/stop_test.py。三模式入口选择相同 start_test.py 与 run_box_damage.py；表现回归选择原 run_e2e.py。逐帧数据、资产备份和校准明细位于 Saved/WingBox；汇总及代码 SHA 见[机器可读结果](wing-box-hit-2026-10-12-results.json)。

## 排查和未验证项

初轮构建发现 UE 5.5 的 GetAnimatingAbility 非 const 接口，修正调用后通过。旧测量入口 GetCPUSkinnedVertices 对缺少 MeshObject 的临时网格触发断言；测量尚未提交任何资产，现已改用 GetCurrentRefToLocalMatrices/ComputeSkinnedPositions，不依赖渲染对象，复跑校准成功。修复后有一次 DLL 占用链接失败，在本轮未加载项目的失败启动进程退出后常规链接通过；未强制关闭包含未保存工作的用户编辑器。

初轮 Python 测试使用了不可反射的角色原生 getter，改为 GetComponentByClass 后复跑通过。已有其他攻击的重叠 BranchingPoint 警告，以及专服 Dash 取消时一次 ApplyRootMotionConstantForce 的空 MovementComponent 警告保留在日志；本任务没有重构这些通知或移动技能。最终本功能检查全部通过。

没有执行独立 Server Target、Cook/打包、弱网压力、其他翼动画/角色缩放的覆盖验证或逐三角形碰撞。当前 Box 是保守实体包围，修改动作、伤害区间或缩放需重新校准。不自动提交 Git，也没有改动 Guardian 行走、攻击吸附或锁定系统。
'''
(OUT / 'wing-box-hit-2026-10-12.md').write_text(text, encoding='utf-8')
print(json.dumps({'native': len(native), 'asset_checks': len(assets['checks']),
                  'damage_checks': sum(len(r['checks']) for r in damage.values()),
                  'presentation_checks': sum(len(r['checks']) for r in presentation.values())}, ensure_ascii=False))
