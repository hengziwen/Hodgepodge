"""Publish final verification summaries, keeping frame captures and build logs in Saved."""
import json
from pathlib import Path

PROJECT = Path('E:/Project/Git/Hodgepodge')
WORK = PROJECT / 'Saved/WingImplementation'
OUT = PROJECT / 'Docs/Validation'


def read(name):
    return json.loads((WORK / name).read_text(encoding='utf-8-sig'))


assets = read('assets.json')
automation = read('AutomationFinal/index.json')
assert automation['failed'] == 0
results = {}
for mode in ['single', 'listen', 'dedicated-pie']:
    item = read(mode + '-e2e.json')
    assert not item['error'] and all(c['passed'] for c in item['checks']), mode
    results[mode] = {'checks': item['checks'], 'sample_count': len(item['samples'])}
damage = read('damage.json')
assert not damage['error'] and all(c['passed'] for c in damage['checks'])
rejection = read('client-cancel-rejection.json')
assert rejection['passed']
payload = {'assets': assets, 'automation': {'succeeded': automation['succeeded'],
    'succeededWithWarnings': automation['succeededWithWarnings'], 'failed': automation['failed'],
    'tests': [{'name': t['fullTestPath'], 'state': t['state']} for t in automation['tests']]},
    'e2e': results, 'damage': {'checks': damage['checks'], 'sample_count': len(damage['samples'])},
    'client_cancel_rejection': rejection, 'builds': {'Editor': 0, 'Game': 0}}
OUT.mkdir(exist_ok=True)
(OUT / 'pover-wing-results-2026-10-11.json').write_text(json.dumps(payload, ensure_ascii=False, indent=2), encoding='utf-8')
lines = ['# 残象之翼实现与验证记录', '', '日期：2026-10-11；引擎：UE 5.5.4。', '',
    '## 已完成', '',
    '- 保留并保存用户已创建的 WingAttach，未覆盖它的位置和旋转。SKM_Pover_LyraLab 的既有用户改动保留。',
    '- 新增 HodgeWingProfile、HodgeWingComponent、HodgeWingAnimInstance，业务代码位于现有 Hodgepodge Runtime 模块。',
    '- 创建 ABP_PoverWing、DA_PoverWing、BP_PoverWingComponent，由现有 Experience 在服务器和客户端注入正式 Pover。',
    '- 独立翅膀从参考姿势开始，程序驱动 Wing000/Wing003/Wing006，不需要动画序列。',
    '- 只在实际第五段挥翼。连段顺序 01→02→03→05→04，绑定 DA_Attack_4 / AM_Attack04_Montage。',
    '- 翅膀新增三处检测插槽；末段翼尖持续命中与原首个命中通知共用 Group/Phase 与伤害/Reaction。',
    '- 禁用翅膀物理阻挡，明确角色姿势→驱动参数→翼骨骼→PostPhysics 采样顺序。',
    '- 以实际蒙太奇时间驱动；暂停保持，取消从当前姿势回收。',
    '- 修复连段 GA 客户端不接受服务器终止的问题；服务器仍不接受未经协调器授权的客户端结束。', '',
    '## 核心文件', '',
    '- [Profile 与姿势插值](../../Source/Hodgepodge/Private/Animation/HodgeWingProfile.cpp)。',
    '- [组件生命周期与蒙太奇时钟](../../Source/Hodgepodge/Private/Component/HodgeWingComponent.cpp)。',
    '- [独立翅膀 AnimInstance](../../Source/Hodgepodge/Private/Animation/HodgeWingAnimInstance.cpp)。',
    '- [自动化测试](../../Source/Hodgepodge/Private/Tests/HodgeWingTests.cpp)。',
    '- [配置手册](../Guides/pover-wing-configuration.md)。', '',
    '## 实际验证结果', '',
    '- 常规 HodgepodgeEditor / Win64 / Development 构建通过，退出 0。',
    '- 常规 Hodgepodge / Win64 / Development 构建通过，退出 0。',
    '- 三项自动化测试全部 Success，失败 0；AssetContract 包含预期的错误配置诊断，因此报告计为带警告成功。',
    '- ABP 与组件蓝图编译均 ERRORS=0；最终重新打开编辑器后，使用修正后的 C++ 制作入口重新生成/编译 ABP，ERRORS=0 WARNINGS=0。']
for mode, item in results.items():
    lines.append(f'- {mode} PIE：{len(item["checks"])} 项检查通过，采样 {item["sample_count"]} 帧。')
lines += ['- Dedicated Server PIE 包含服务器、拥有者客户端及另一客户端；验证了实际翼骨骼位移和各端取消回收。',
    '- 实际翼尖服务器伤害：目标生命 350→319，持续重叠只出现一次扣血；暂停源蒙太奇后翼时钟保持。',
    '- 客户端结束自身连段后，服务器实例继续运行，反向终止权限检查通过。',
    '- Group/Phase 由正式资产配置共用；同组去重沿用现有 Melee GA，不另写翅膀扣血。', '',
    '## 实际命令', '', '```powershell',
    "& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1",
    "& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1",
    "& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Wing' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/WingImplementation/AutomationFinal' '-abslog=E:/Project/Git/Hodgepodge/Saved/WingImplementation/AutomationFinal.log'",
    '& E:/Python/python.exe Tools/Wing/create_sockets.py',
    '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Wing/configure_assets.py',
    '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Wing/start_validation.py',
    '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Wing/run_e2e.py',
    '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Wing/test_damage.py',
    '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Wing/test_client_cancel_rejection.py',
    '```', '',
    '多人启动脚本在执行 start_validation.py 前设置 WING_PLAYERS=2；PIE 专服增加 WING_DEDICATED=True。', '',
    '日志、原始逐帧结果、资产备份位于 Saved/WingImplementation。最终构建日志为 EditorBuild-delivery.log、GameBuild-delivery.log。', '',
    '## 发现并处理的问题', '',
    '- UE 5.5 的 Socket 字段无法通过 Python 赋值，改用项目网关原生 create_socket，再显式保存实际被修改的 Mesh。',
    '- Local To Component 的输出名为 ComponentPose；已修正图连接。变量读取改为 Self Member，避免无效目标。',
    '- DataTable 的 GameplayTag/Transition 导出为属性文本，制作脚本按实际格式解析连段顺序。',
    '- 连段远端终止权限必须按端区分，不能同时拒绝服务器对客户端的结束。',
    '- 编辑器脚本直接停服务器蒙太奇会影响 RPC 调用空间；最终网络取消通过原生世界 Tick 的 QueueAbilityAction 验证。', '',
    '## 限制与后续', '',
    '- 程序挥翼属于可调首版动作；角度、翼尖半径和时间都已可配置，后续仍可按镜头继续打磨。',
    '- 没有羽毛物理、独立飞行翼、Control Rig 或动画烘焙。',
    '- 没有执行 Cook、打包或独立 Dedicated Server 可执行文件测试；PIE 专服通过不等于专服构建通过。',
    '- 未采集代码行/分支覆盖率；报告提供功能场景与实际执行结果。',
    '- 保留已有未提交工作，没有自动提交 Git。', '',
    '[机器可读结果](pover-wing-results-2026-10-11.json)；[本机挂接预览](../../Saved/Screenshots/WindowsEditor/PoverWing-PIE.png)。', '']
(OUT / 'pover-wing-implementation-2026-10-11.md').write_text('\n'.join(lines), encoding='utf-8')
print('Published wing implementation verification report')
