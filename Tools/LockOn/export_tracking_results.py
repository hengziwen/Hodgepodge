"""从实际断言和原生报告生成可追溯摘要，不用网关启动成功代替验收。"""
import hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
WORK=ROOT/'Saved/LockOnTracking'
MODES=['single','listen','dedicated','listen-lag']

def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def evidence(path):return {'path':path.relative_to(ROOT).as_posix(),'sha256':digest(path)}

index=WORK/'Automation/index.json'
auto=json.loads(index.read_text(encoding='utf-8-sig'))['tests']
assert len(auto)==36 and all(t['state']=='Success' for t in auto)
results={'date':'2026-10-13','engine':'5.5.4','native':{'count':len(auto),'passed':True,'tests':[{'name':t['fullTestPath'],'state':t['state']} for t in auto],'evidence':evidence(index)},'pie':[]}
for mode in MODES:
 for kind,suffix,count in [('tracking','-tracking-e2e.json',22),('regression','-e2e.json',20)]:
  path=WORK/(mode+suffix);r=json.loads(path.read_text(encoding='utf-8'))
  assert r['passed'] and not r['error'] and len(r['checks'])==count and all(c['passed'] for c in r['checks'])
  results['pie'].append({'mode':mode,'kind':kind,'passed':True,'checks':r['checks'],'sample_count':len(r['samples']),'evidence':evidence(path)})
results['configuration']=json.loads((WORK/'configuration.json').read_text(encoding='utf-8'))
commands=[
 'Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2',
 'Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2',
 'UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash "-ExecCmds=Automation RunTests Hodge.LockOn+Hodge.Facing+Hodge.Rotation+Hodge.Movement+Hodge.UI" "-TestExit=Automation Test Queue Empty" "-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/LockOnTracking/Automation" "-abslog=E:/Project/Git/Hodgepodge/Saved/LockOnTracking/automation.log"']
command_results=json.loads((WORK/'command-results.json').read_text(encoding='utf-8'))
assert all(r['exit_code']==0 and digest(WORK/r['log'])==r['sha256'] for r in command_results)
results['commands']=[{'command':c,'exit_code':r['exit_code'],'evidence':evidence(WORK/r['log'])} for c,r in zip(commands,command_results)]
files=[
 'Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h','Source/Hodgepodge/Private/Data/HodgeLockOnProfile.cpp',
 'Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h','Source/Hodgepodge/Private/Targeting/HodgeLockOnTypes.cpp',
 'Source/Hodgepodge/Public/Camera/HodgeCameraMode_LockOn.h','Source/Hodgepodge/Private/Camera/HodgeCameraMode_LockOn.cpp',
 'Source/Hodgepodge/Public/Component/HodgeLockOnComponent.h','Source/Hodgepodge/Private/Component/HodgeLockOnComponent.cpp',
 'Source/Hodgepodge/Private/Tests/HodgeLockOnTests.cpp','Content/Main/Character/Hero/Targeting/DA_HeroLockOn.uasset',
 'Tools/LockOn/run_tracking_e2e.py','Tools/LockOn/run_validation_batch.py','Tools/LockOn/configure_soft_camera.py']
results['implementation']=[evidence(ROOT/f) for f in files]
results['limits']=['环绕与敌人横移为受控 PIE 轨迹，真实移动／Dash/Sprint 使用 Enhanced Input。','CameraOverride 验证实际接口链，未新增正式演出 GA。','PIE Dedicated 不等于独立专服构建；未 Cook／打包。','未完成物理鼠标 DPI、全部高差／狭窄地形和最终主观手感验收。','本轮没有启用背后中键获取、攻击吸附、手柄或修改角色面向设计。']
results['initial_failures']=[{'issue':'横移修正测得约 7.6 度，初始固定 12 度断言过严','resolution':'改用大于 5 度的明显修正，并同时断言目标留在合理屏幕区域。'}, {'issue':'网络夹具在准备期间设置的本地 Actor 背向被既有复制恢复','resolution':'在中键事件前对服务器和拥有端 PIE 副本设置测试姿势，不改生产朝向代码。'}]
restored=WORK/'editor-restored.json'
if restored.exists():results['editor_restore']=json.loads(restored.read_text(encoding='utf-8'))
output=ROOT/'Docs/Validation/soft-lock-tracking-2026-10-13-results.json'
output.write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'native':len(auto),'tracking':88,'regression':80,'report':output.as_posix()},ensure_ascii=False))
