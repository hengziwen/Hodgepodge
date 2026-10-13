"""Generate the HUD's simple vector-like ring icon; does not alter the reference image."""
from pathlib import Path
from PIL import Image, ImageDraw

folder = Path(__file__).resolve().parent / 'Art'
scale = 4
canvas = Image.new('RGBA', (128 * scale, 128 * scale))
draw = ImageDraw.Draw(canvas)
def line(points, width=4):
    draw.line([(x*scale, y*scale) for x,y in points], fill='white', width=width*scale, joint='curve')
for inset, width in [(9,3),(18,2)]:
    draw.ellipse((inset*scale,inset*scale,(128-inset)*scale,(128-inset)*scale),outline='white',width=width*scale)
line([(49,90),(49,37),(71,37),(82,48),(82,58),(71,67),(49,67)])
line([(67,67),(83,90)])
canvas.resize((128,128), Image.Resampling.LANCZOS).save(folder/'end-icon.png')
print(folder/'end-icon.png')
