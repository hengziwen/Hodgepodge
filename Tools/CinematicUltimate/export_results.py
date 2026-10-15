"""汇总真实原生与PIE报告，保留需求映射，不虚构行覆盖率或美术完成状态。"""
import json
import re
import shutil
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
WORK=ROOT/'Saved/CinematicUltimate'
DEST=ROOT/'Docs/Validation/resonance-liberation-2026-10-15-results.json'


def load(path):
    """只读取已经落盘的结果，缺报告即失败，不用空列表伪装通过。"""
    return json.loads(path.read_text(encoding='utf-8-sig'))


native=load(WORK/'FinalAutomationFixed/index.json')
assets=load(WORK/'assets-results.json')
assert native['failed']==0 and native['notRun']==0 and assets['passed']
e2e=[]
for mode in ['single','listen','dedicated','listen-lag','capture']:
    report=load(WORK/(mode+'-e2e.json'))
    assert report['passed'] and not report['error'],mode
    e2e.append({'mode':mode,'passed':True,'checks':report['checks'],'raw_report':(WORK/(mode+'-e2e.json')).relative_to(ROOT).as_posix()})

# 截图是验证产物，留在Saved，不把缓存图片装成新的生产资产。
screens=WORK/'Screenshots'
screens.mkdir(exist_ok=True)
captures=load(WORK/'capture-e2e.json')['screenshots']
assert len(captures)==3,'三个构图必须都有实际渲染文件'
image_paths=[]
for capture in captures:
    name=re.sub(r'[^a-z0-9-]','-',capture['shot'].lower())
    destination=screens/(name+'.png')
    shutil.copy2(ROOT/capture['file'],destination)
    image_paths.append(destination.relative_to(ROOT).as_posix())
shutil.copy2(ROOT/'Saved/UIFoundation/UI-Slate.png',screens/'hud.png')

result={
    'date':'2026-10-15','timezone':'Asia/Shanghai','engine':'UE 5.5.4',
    'builds':[{'target':target,'platform':'Win64','configuration':'Development','exit_code':0,'log':'Saved/CinematicUltimate/'+log}
              for target,log in [('HodgepodgeEditor','editor-lifecycle-final.log'),('Hodgepodge','game-lifecycle-final.log')]],
    'native':{'passed':native['succeeded']+native['succeededWithWarnings'],'failed':native['failed'],'with_warnings':native['succeededWithWarnings'],
              'tests':[{'name':test['fullTestPath'],'state':test['state'],'entries':test['entries']} for test in native['tests']],
              'raw_report':'Saved/CinematicUltimate/FinalAutomationFixed/index.json'},
    'asset_checks':assets,'e2e':e2e,
    'coverage':{'kind':'需求与用例映射；没有Clang/MSVC行或分支覆盖率采集','partial':['CIN-06：本地PP和音效/震屏事件接口已实现，正式Niagara、音效、震屏素材未制作',
        'CIN-03/04：旧句柄、重放、Dash取消、死亡已测；换Pawn和服务器拒绝后的实例恢复未逐项E2E注入',
        'CIN-08：81项相关原生回归，未重新跑完整Dash/Sprint/普攻Warp的全部实机矩阵']},
    'issues_fixed':[
        {'issue':'远端晚加入玩家拥有技能但充能容量为0','cause':'PreInitializeComponents中已加载Experience立即回调，早于ASC发现原生AttributeSet',
         'fix':'按Lyra将ASC绑定和Experience注册迁移到PostInitializeComponents','proof':'Hodge.Resonance.PlayerStateBeforePawn和Listen/Dedicated真实容量、回能、R输入'},
        {'issue':'资产重试重复添加大招输入Tag','cause':'Python反射GameplayTag对象比较未按标签名称匹配','fix':'按TagName去重本技能入口','proof':'大招输入Tag只绑定一次'},
        {'issue':'WidgetBlueprintFactory空Canvas可能不生成默认充能控件','cause':'只有RootWidget为空的判断不能识别空Canvas','fix':'RebuildWidget在Slate前处理空容器；真实子树与显示断言','proof':'RES-04各模式0/100、按钮禁用、位置'},
        {'issue':'Game构建中ATTRIBUTE_ACCESSORS依赖不完整ASC类型','fix':'公共ResonanceSet头显式包含ASC，Game常规构建通过'},
        {'issue':'PIE脚本死亡后访问已解绑ASC、锁定夹具未等待位置复制、Python受保护字段读取','fix':'从持久PlayerState读资源、先等目标位置复制、使用限定原生只读UI诊断','proof':'死亡与100ms PktLag模式、UI断言复跑通过'}],
    'remaining':[
        'T_Hit=0.68T、Box尺寸、伤害倍率2、5/20回能与三分镜偏移仍为资产中的原型初值，需按实际动画和战斗手感调参',
        'Impacts留空，未批准擅自加入控制或无敌；金刚体不自动禁止Dash',
        '独立Server Target/专服可执行程序、Cook、打包未执行；PIE Dedicated已执行',
        '正式地图灯光、近墙、多画幅、UI相机覆盖、其他CameraModifier叠加及全部网络拒绝场景需后续实机矩阵',
        '主蒙太奇同帧Branching Point重叠产生引擎警告；现有Definition源位置扫描保障本次状态窗口，已通过翼/后半/取消断言；后续统一通知轨警告处理',
        '没有逐帧读取原作视频，不声称逐帧复刻《鸣潮》'],
    'screenshots':image_paths+['Saved/CinematicUltimate/Screenshots/hud.png'],
    'committed':False,
}
DEST.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'native':result['native']['passed'],'asset_checks':len(assets['checks']),
                  'e2e':{entry['mode']:len(entry['checks']) for entry in e2e},'report':DEST.as_posix()},ensure_ascii=False))
