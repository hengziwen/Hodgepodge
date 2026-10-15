"""按参考的白色纹样重绘透明图标，生成可替换PNG与矢量源，不处理参考截图像素。"""
from pathlib import Path
import math
from PIL import Image,ImageDraw

OUT=Path(__file__).resolve().parent/'Art'
OUT.mkdir(exist_ok=True)
SCALE=4


def bezier(points):
    """细分三次贝塞尔曲线，保证小尺寸纹样边缘连续。"""
    a,b,c,d=points
    return [tuple((1-t)**3*a[k]+3*(1-t)**2*t*b[k]+3*(1-t)*t*t*c[k]+t**3*d[k] for k in [0,1]) for t in [i/64 for i in range(65)]]


def build(name,curves,lines,circles):
    """同一份几何输出SVG和抗锯齿透明PNG，供后续美术替换。"""
    canvas=Image.new('RGBA',(128*SCALE,128*SCALE))
    draw=ImageDraw.Draw(canvas)
    svg=['<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 128 128"><g fill="none" stroke="white" stroke-linecap="round" stroke-linejoin="round">']
    for curve,width in curves:
        draw.line([(x*SCALE,y*SCALE) for x,y in bezier(curve)],fill='white',width=round(width*SCALE),joint='curve')
        svg.append('<path stroke-width="'+str(width)+'" d="M'+','.join(map(str,curve[0]))+' C'+' '.join(','.join(map(str,p)) for p in curve[1:])+'"/>')
    for points,width in lines:
        draw.line([(x*SCALE,y*SCALE) for x,y in points],fill='white',width=round(width*SCALE),joint='curve')
        svg.append('<polyline stroke-width="'+str(width)+'" points="'+' '.join(','.join(map(str,p)) for p in points)+'"/>')
    for x,y,r,width in circles:
        draw.ellipse(((x-r)*SCALE,(y-r)*SCALE,(x+r)*SCALE,(y+r)*SCALE),outline='white',width=round(width*SCALE))
        svg.append(f'<circle cx="{x}" cy="{y}" r="{r}" stroke-width="{width}"/>')
    svg.append('</g></svg>')
    (OUT/(name+'.svg')).write_text('\n'.join(svg),encoding='utf-8')
    canvas.resize((256,256),Image.Resampling.LANCZOS).save(OUT/(name+'.png'))


wing_curves=[]
for mirror in [False,True]:
    for points,width in [([(61,64),(18,60),(10,27),(25,28)],4), ([(25,28),(44,28),(43,53),(61,64)],4),
                         ([(60,68),(23,64),(13,78),(31,83)],3), ([(31,83),(45,87),(50,74),(60,68)],3),
                         ([(57,70),(34,85),(41,100),(58,89)],3), ([(58,89),(62,84),(62,78),(62,74)],3)]:
        wing_curves.append(([(128-x if mirror else x,y) for x,y in points],width))
build('resonance-skill',wing_curves,[([(64,20),(64,105)],4), ([(58,98),(64,108),(70,98)],3)],[(64,62,5,2)])

burst_curves=[]
burst_lines=[]
for angle in [0,90,180,270]:
    radians=math.radians(angle)
    def rotate(point):
        x,y=point[0]-64,point[1]-64
        return (64+x*math.cos(radians)-y*math.sin(radians),64+x*math.sin(radians)+y*math.cos(radians))
    burst_curves.append(([rotate(p) for p in [(62,51),(36,38),(47,19),(62,30)]],2.5))
    burst_curves.append(([rotate(p) for p in [(62,30),(72,37),(66,47),(62,51)]],2.5))
    burst_lines.append(([rotate(p) for p in [(64,24),(64,42),(57,48)]],2.5))
build('resonance-liberation',burst_curves,burst_lines+ [([(64,52),(76,64),(64,76),(52,64),(64,52)],2.5)],[(64,64,41,2),(64,64,7,2)])
print('生成两枚透明技能纹样与SVG源文件')
