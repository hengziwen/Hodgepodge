"""从本次实际构建、31 项回归和八组 PIE 报告导出证据；不伪造代码覆盖率。"""
import json,hashlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
WORK=ROOT/'Saved/LockOnCamera'

def load(name):return json.loads((WORK/name).read_text(encoding='utf-8-sig'))
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()

automation=load('Automation/index.json')
assert len(automation['tests'])==31 and all(t['state']=='Success' for t in automation['tests'])
reports={}
for mode in ['single','listen','dedicated','listen-lag']:
    for kind,count in [('camera',24),('lock',20)]:
        filename=mode+('-camera-e2e.json' if kind=='camera' else '-e2e.json')
        r=load(filename)
        assert r['passed'] and len(r['checks'])==count and all(c['passed'] for c in r['checks']),filename
        reports[kind+'/'+mode]={'checks':r['checks'],'passed':True,'raw_path':'Saved/LockOnCamera/'+filename,'sha256':sha(WORK/filename)}
builds={}
for target in ['editor','game']:
    code=int((WORK/(target+'-build-exit.txt')).read_text().strip());assert code==0
    builds[target]={'exit_code':code,'log':'Saved/LockOnCamera/'+target+'-build.log','sha256':sha(WORK/(target+'-build.log'))}
paths=load('source-files.json')
flat={'Source/Hodgepodge/Public/Component/HodgeHeroComponent.h':'HodgeHeroComponent.h',
      'Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.cpp':'HodgeUIAuthoringLibrary.cpp',
      'Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp':'HodgeGameplayAbility.cpp'}
paths+=list(flat)
changed=[]
for p in paths:
    old=WORK/'Before'/flat.get(p,p)
    if (ROOT/p).read_bytes()!=old.read_bytes():changed.append(p)
added=['Source/Hodgepodge/Private/Tests/HodgeLockOnTestTypes.h','Source/Hodgepodge/Private/Tests/HodgeLockOnTestTypes.cpp']
assets=['Content/Main/Character/Hero/Targeting/DA_HeroLockOn.uasset','Content/Main/Character/Hero/BP_Hero_Pover.uasset','Content/Main/Camera/CM_ThirdPerson.uasset']
restored=load('editor-restored.json');assert not restored['pie'] and not restored['dirty_maps'] and not restored['dirty_content']
result={'date':'2026-10-13','engine':'5.5.4','design_version':'lock-on v0.4',
        'builds':builds,'automation':{'passed':31,'total':31,'tests':[{'name':t['fullTestPath'],'state':t['state']} for t in automation['tests']],
                                   'sha256':sha(WORK/'Automation/index.json')},
        'pie':reports,'asset_checks':load('asset-validation.json'),'config':load('soft-camera-config.json'),
        'blueprints':load('blueprints.json'),'editor_restored':restored,
        'source_changed':changed,'source_added':added,'source_sha256':{p:sha(ROOT/p) for p in changed+added},
        'asset_sha256':{p:sha(ROOT/p) for p in assets},
        'coverage':{'line_percent':None,'branch_percent':None,'note':'没有插桩，只提供实际需求断言与结果。'},
        'limits':['InputKey/EnhancedInput 注入不是物理鼠标、DPI 或主观手感验收','CameraOverride 验证真实 Hero 相机接口，不是一份完整正式演出 GA',
                  '未验证全部狭窄地形、高差、群体构图、Boss多点、换Pawn、热卸载、丢包或打包','专服PIE不是独立ServerTarget'],
        'fixed_failures':['过渡中输入被混合权重削弱','旧输入在临时镜头返回时重复应用风险','回归夹具在退出混合期间强设控制角度导致选错目标',
                          '单元夹具实例化Abstract基类','测试直接调用非导出的GAS句柄生成方法','Python无法读取受保护Blueprint.Status']}
out=ROOT/'Docs/Validation/soft-lock-camera-2026-10-13-results.json'
out.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'native':31,'camera_checks':96,'lock_checks':80,'source_changed':len(changed),'source_added':len(added)},ensure_ascii=False))
