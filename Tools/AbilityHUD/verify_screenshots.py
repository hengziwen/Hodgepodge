"""检查实际GPU截图中的充能方向与冷却上下明暗，不只验证CPU进度值。"""
import json
import os
from pathlib import Path
from PIL import Image

ROOT=Path(__file__).resolve().parents[2]
WORK=Path(os.environ.get('HODGE_ABILITY_HUD_WORK',str(ROOT/'Saved/AbilityHUD')))
report=json.loads((WORK/'single-e2e.json').read_text(encoding='utf-8'))
layout=report['checks'][0]['detail']
full=Image.open(WORK/'energy-100.png').convert('RGB')
width,height=full.size
checks=[]


def gold(pixel):
    """限定金色环像素，排除白色纹样、倒计时和灰色测试场地。"""
    r,g,b=pixel
    return r>180 and g>155 and b<175 and r>b+30


points={(x,y) for x in range(int(width*.96),width) for y in range(int(height*.85),height) if gold(full.getpixel((x,y)))}
assert len(points)>50,'没有检测到完整金环，可能材质编译或绑定失败'
cx=(min(x for x,y in points)+max(x for x,y in points))*.5
cy=(min(y for x,y in points)+max(y for x,y in points))*.5
quadrants=[[(x,y) for x,y in points if sx*(x-cx)>2 and sy*(y-cy)>2] for sx,sy in [(1,-1),(1,1),(-1,1),(-1,-1)]]
for percent,name in [(0,'01-empty'),(25,'energy-25'),(50,'energy-50'),(75,'energy-75'),(100,'energy-100')]:
    frame=Image.open(WORK/(name+'.png')).convert('RGB')
    ratios=[sum(gold(frame.getpixel(point)) for point in quadrant)/len(quadrant) for quadrant in quadrants]
    expected=int(percent/25)
    passed=all(ratio>.85 if index<expected else ratio<.15 for index,ratio in enumerate(ratios))
    checks.append({'case':str(percent)+'%实际金环从顶部顺时针','passed':passed,'quadrants':ratios})

# 使用正常白色图标定位纹样，排除中央数字及下方键帽再比较上下亮度。
normal=Image.open(WORK/'01-empty.png').convert('RGB')
half=Image.open(WORK/'03-skill-half.png').convert('RGB')
ex=layout['skill_center_x']*width
ey=cy
top=[]; bottom=[]
for x in range(int(ex-14),int(ex+15)):
    for y in range(int(ey-14),int(ey+15)):
        if abs(x-ex)<5 or abs(y-ey)<3:continue
        if min(normal.getpixel((x,y)))<205:continue
        (top if y<ey else bottom).append(sum(half.getpixel((x,y)))/3.)
assert top and bottom,'实际纹样样本不足'
top_mean=sum(top)/len(top); bottom_mean=sum(bottom)/len(bottom)
checks.append({'case':'半冷却时上方变暗、下方恢复亮色','passed':top_mean+60<bottom_mean,'top':top_mean,'bottom':bottom_mean})
result={'passed':all(row['passed'] for row in checks),'checks':checks,'scope':'1516x450实际单人Slate截图；不声称覆盖所有画幅与GPU'}
(WORK/'visual-results.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result,ensure_ascii=False))
assert result['passed'],result
