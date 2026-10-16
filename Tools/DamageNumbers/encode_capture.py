"""把真实 PIE 固定步长截图编码为演示；不下载媒体、不把离线帧率当作性能。"""
import json
from pathlib import Path
import shutil
import subprocess
import time

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'Saved/DamageNumbers'
report = json.loads((WORK / 'single-numbers-e2e.json').read_text(encoding='utf-8'))
assert report['passed'] and report['capture']['enabled']
frames = sorted((WORK / 'Frames').glob('frame-*.png'))
assert len(frames) == report['capture']['frames'] and len(frames) > 60
assert all(p.name == 'frame-%04d.png' % i for i, p in enumerate(frames))
ffmpeg = shutil.which('ffmpeg')
if not ffmpeg:
    ffmpeg = str(next((ROOT / 'Saved/HitPresentation/MediaDeps').rglob('ffmpeg*.exe')))
output = WORK / 'lyra-formal-combo-60fps-offline.mp4'
if output.exists():
    output.rename(output.with_name(output.stem + '-history-' + str(time.time_ns()) + output.suffix))
subprocess.run([ffmpeg, '-hide_banner', '-loglevel', 'warning', '-framerate', '60', '-i', str(WORK / 'Frames/frame-%04d.png'),
                '-c:v', 'libx264', '-crf', '18', '-pix_fmt', 'yuv420p', '-movflags', '+faststart', str(output)], check=True)
print(json.dumps({'video': str(output), 'frames': len(frames), 'fps': 60, 'kind': 'offline fixed simulation step'}, ensure_ascii=False))
