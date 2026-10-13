"""将最终验证与实际代码版本关联到设计需求。"""
import json,hashlib,subprocess
from pathlib import Path
ROOT=Path('E:/Project/Git/Hodgepodge');WORK=ROOT/'Saved/WingAttack'
unit=json.loads((WORK/'Automation-final/index.json').read_text(encoding='utf-8-sig'))
e2e=[json.loads((WORK/(m+'-e2e.json')).read_text(encoding='utf-8')) for m in ['single','listen','dedicated']]
assets=json.loads((WORK/'asset-validation.json').read_text(encoding='utf-8'))
assert assets['passed'] and all(r['passed'] for r in e2e)
assert all(t['state'] in ['Success','成功'] for t in unit['tests'])
trace={
 'Hodge.Wing.AnimationMapping':['WING-02','WING-03','WING-04','WING-07','WING-08'],
 'Hodge.Wing.PresentationSession':['WING-06'],
 'Hodge.Wing.AttackPresentationIntegration':['WING-01','WING-03','WING-04','WING-05','WING-06','WING-07','WING-08','WING-10','WING-11','WING-12'],
 'Hodge.Wing.Envelope':['WING-07'],
 'Hodge.Wing.CancellationAndClock':['WING-07'],
 'Hodge.Wing.AssetContract':['WING-07']}
files=['Source/Hodgepodge/Public/Animation/HodgeWingProfile.h','Source/Hodgepodge/Private/Animation/HodgeWingProfile.cpp',
 'Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h','Source/Hodgepodge/Private/Animation/HodgeAnimNotifyState_WingAction.cpp',
 'Source/Hodgepodge/Public/Component/HodgeWingComponent.h','Source/Hodgepodge/Private/Component/HodgeWingComponent.cpp',
 'Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp',
 'Source/Hodgepodge/Private/Tests/HodgeWingAttackPresentationTests.cpp',
 'Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h','Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.cpp']
result={'design':'Docs/Design/attack-wing-presentation.md','native_tests':[{'test':t['fullTestPath'],'state':t['state'],'requirements':trace[t['fullTestPath']]} for t in unit['tests']],
 'assets':assets,'end_to_end':e2e,'source_sha256':{f:hashlib.sha256((ROOT/f).read_bytes()).hexdigest() for f in files}}
directory=ROOT/'Docs/Validation';directory.mkdir(exist_ok=True)
(directory/'attack-wing-presentation-2026-10-12-results.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
lines=['# 攻击限定翅膀实现与验证（2026-10-12）','',
 '设计：[攻击限定翅膀表现系统](../Design/attack-wing-presentation.md)。','',
 '## 实现与资产','',
 '- 新动画模式按角色源时间求值，通知仅定义半开显示窗口；先求值再显现，空闲和暂停不重复求值。',
 '- 会话句柄防止重复开始及旧结束回调；GAS 结束、死亡及 Avatar 更换清理表现。',
 '- 保留 WingText 的原插槽及 0.8 缩放；正式 Hero 使用等价 WingCase 骨骼与偏移。',
 '- 新正式配置：`/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu`、`BP_PoverWing_Anzhu`。',
 '- 初始配置只映射已校准的 AM_Attack04/B_Attack04；通知窗口为源秒 0.05 至动画时长减 0.05，用户可手动裁剪。',
 '- 旧程序化组件保持禁用，不新增翅膀伤害、吸附或锁定。','',
 '## 实际验证','',
 '- Editor 与 Game 最终常规构建均退出 0；日志为 Saved/WingAttack/EditorBuild-final.log、GameBuild-final.log。',
 f'- 原生自动化 {len(unit["tests"])} 项通过，包含旧模式回归及新增单元/真实世界集成。报告为 Saved/WingAttack/Automation-final/index.json。',
 f'- 资产集成 {len(assets["checks"])} 项通过；新组件各端唯一注入，旧条目保持关闭，翼骨架及源窗口有效。']
for r in e2e:lines.append(f'- {r["mode"]} 真实输入端到端：{len(r["checks"])} 项通过；Saved/WingAttack/{r["mode"]}-e2e.json 保存需求编号和逐帧证据。')
lines += ['','## 需求追溯','']
for t in result['native_tests']:lines.append('- `'+t['test']+'`：'+', '.join(t['requirements'])+'，通过。')
for req in sorted({c['requirement'] for r in e2e for c in r['checks']}):
    names=[r['mode']+':'+c['check'] for r in e2e for c in r['checks'] if c['requirement']==req]
    lines.append('- '+req+'：'+ '；'.join(names)+'。')
lines += ['','## 执行命令','', '```powershell',
 "& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2",
 "& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2",
 "& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -NoShaderWorker -nosplash '-ExecCmds=Automation RunTests Hodge.Wing' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/WingAttack/Automation-final' '-abslog=E:/Project/Git/Hodgepodge/Saved/WingAttack/Automation-final.log'",
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingAttack/configure_assets.py',
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingAttack/validate_assets.py',
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingAttack/start_test.py',
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WingAttack/start-listen.py',
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WingAttack/start-dedicated.py',
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingAttack/stop_test.py',
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingAttack/restore_editor.py','```','',
 '## 发现与修正','',
 '- 初次编译修正了 ASC getter 的非 const 调用和自动化 TObjectPtr 参数；最终构建通过。',
 '- 配置工具修正 DataAssetFactory 的编辑器属性访问；只重跑本功能资产，保留其他通知。',
 '- 测试启动阶段提前隔离敌人，避免玩家在脚本接入前死亡；这是测试夹具修正，不修改敌人 AI。',
 '- EPlayNetMode 不能由 Python 直接导出，恢复采用未修改的持久网络配置与原生配置入口。',
 '- 补充无效配置下复用网格也必须隐藏的保护，以及暂停时复用已求值姿势。','',
 '## 验证边界','',
 '- 专服是双客户端 PIE，不是独立 Dedicated Server 可执行文件。',
 '- 没有执行 Cook、打包、固定硬件帧率基准或网络丢包压力测试。',
 '- 死亡与 Avatar 更换覆盖在原生世界集成测试；端到端验证正常输入、收招和 Dash 取消，不把这些等同于完整重生系统验收。',
 '- 保留所有既有未提交改动；本轮没有自动提交。备份及日志位于 Saved/WingAttack。','']
(directory/'attack-wing-presentation-2026-10-12.md').write_text('\n'.join(lines),encoding='utf-8')
(WORK/'status-after.txt').write_bytes(subprocess.check_output(['git','status','--short'],cwd=ROOT))
print(json.dumps({'native_tests':len(unit['tests']),'asset_checks':len(assets['checks']),'e2e_checks':sum(len(r['checks']) for r in e2e)},ensure_ascii=False))
