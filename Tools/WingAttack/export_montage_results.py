"""归档通知蒙太奇最终构建、资产和实际测试结果，保留需求追溯。"""
import hashlib
import json
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
WORK=ROOT/'Saved/WingMontage'
OUT=ROOT/'Docs/Validation'
native=json.loads((WORK/'Automation-final/index.json').read_text(encoding='utf-8-sig'))
assets=json.loads((WORK/'asset-validation.json').read_text(encoding='utf-8'))
blueprints=json.loads((WORK/'blueprint-status.json').read_text(encoding='utf-8'))
results={}
for scenario,suffix in [('presentation','-e2e.json'),('damage','-damage.json')]:
    results[scenario]={}
    for mode in ['single','listen','dedicated']:
        report=json.loads((WORK/scenario/(mode+suffix)).read_text(encoding='utf-8'))
        assert report['passed'] and all(row['passed'] for row in report['checks']), (scenario,mode)
        results[scenario][mode]={'passed':report['passed'],'checks':report['checks'],'samples':len(report['samples']),
                                 'evidence':(WORK/scenario/(mode+suffix)).relative_to(ROOT).as_posix()}
assert native['failed']==0 and native['notRun']==0 and assets['passed']
assert all(row['data']['compileStatus']=='BS_UpToDate' and not row['data']['hasCompileErrors'] for row in blueprints)
paths=['Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h',
       'Source/Hodgepodge/Private/Animation/HodgeAnimNotifyState_WingAction.cpp',
       'Source/Hodgepodge/Public/Animation/HodgeWingProfile.h',
       'Source/Hodgepodge/Private/Animation/HodgeWingProfile.cpp',
       'Source/Hodgepodge/Public/Component/HodgeWingComponent.h',
       'Source/Hodgepodge/Private/Component/HodgeWingComponent.cpp',
       'Source/Hodgepodge/Private/Tests/HodgeWingAttackPresentationTests.cpp',
       'Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h',
       'Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.cpp',
       'Content/Main/Character/Hero/Wing/DA_PoverWing_Anzhu.uasset',
       'Content/Main/Character/Hero/Wing/ABP_PoverWing_Anzhu.uasset',
       'Content/Main/Character/Hero/Wing/Montages/AM_Wing_Attack04.uasset',
       'Content/Main/Character/Hero/Wing/Montages/AM_Wing_Attack05.uasset',
       'Content/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage.uasset',
       'Content/Main/Character/Hero/Anim/Montages/AM_Attack05_Montage.uasset',
       'Content/Wuwa/Anim/AM_Attack04.uasset','Content/Main/WingText.uasset']
report={'date':'2026-10-14','builds':{'Editor':{'exit_code':0,'log':'Saved/WingMontage/EditorBuild.log'},
                                 'Game':{'exit_code':0,'log':'Saved/WingMontage/GameBuild.log'}},
        'native':{'passed':native['succeeded']+native['succeededWithWarnings'],
                  'with_warnings':native['succeededWithWarnings'],
                  'tests':[{'name':row['fullTestPath'],'state':row['state'],'warnings':row['warnings']} for row in native['tests']]},
        'assets':assets,'blueprints':[row['data'] for row in blueprints], 'pie':results,
        'hashes':{path:hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
        'restored':json.loads((WORK/'editor-restored.json').read_text(encoding='utf-8')),
        'limitations':['没有独立专服进程、Cook/打包或手动物理输入手感验证',
                       '翼蒙太奇只驱动表现和姿势，技能与伤害事件保留在角色蒙太奇',
                       '旧字段仅保留序列化兼容，未删除旧程序化资产',
                       '隔离 Box 场景最初发 Dash 输入未结束攻击，未单独定位激活失败；Box 清理改用服务器原生技能取消队列，Dash 另由表现 E2E 验证']}
(OUT/'wing-notify-montage-2026-10-14-results.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
total=sum(len(mode['checks']) for scenario in results.values() for mode in scenario.values())
print(json.dumps({'native':report['native']['passed'],'assets':len(assets['checks']),'pie':total,'blueprints':len(blueprints)},ensure_ascii=False))
