"""把原生 PIE 逐帧截图编码为明确标注的离线对照，不补帧或冒充实时录屏。"""
from pathlib import Path
import json
import subprocess
import sys

root = Path(__file__).resolve().parents[2] / 'Saved' / 'HitPresentation'
sys.path.insert(0, str(root / 'MediaDeps'))
import imageio_ffmpeg

captures = []
for name in ['ACT-Before-Complete', 'ACT-After']:
    folder = root / name
    data = json.loads((folder / 'capture.json').read_text(encoding='utf-8'))
    assert data['error'] is None and data['fixed_simulation_step'] == 1 / 60, data['error']
    assert len(data['rows']) == len(data['frames']) == 420
    assert all(Path(row['path']).is_file() for row in data['frames'])
    captures.append(folder)

output = root.parent / 'ACTCombat'
reports = []
for suffix, rate in [('normal', 60), ('quarter', 15)]:
    target = output / ('act-comparison-' + suffix + '.mp4')
    font = "fontfile='C\\:/Windows/Fonts/segoeui.ttf'"
    # 明确旧／新配置采集和离线边界，具体运行时哈希由验证记录关联。
    graph = ('[0:v][1:v]hstack=inputs=2,pad=iw:ih+76:0:76:color=black,'
             'drawtext=' + font + ':text=Baseline capture - original assets:x=16:y=10:fontsize=23:fontcolor=white,'
             'drawtext=' + font + ':text=Final capture - ACT prototype:x=976:y=10:fontsize=23:fontcolor=white,'
             'drawtext=' + font + ':text=Offline 60 Hz simulation - no recorded audio - not real-time FPS evidence:x=16:y=44:fontsize=19:fontcolor=white')
    args = [imageio_ffmpeg.get_ffmpeg_exe(), '-y', '-v', 'error']
    for folder in captures:
        args += ['-framerate', str(rate), '-i', str(folder / 'frame_%04d.png')]
    args += ['-filter_complex', graph, '-an', '-c:v', 'libx264', '-crf', '20', '-pix_fmt', 'yuv420p',
             '-movflags', '+faststart', str(target)]
    subprocess.run(args, check=True)
    reports.append({'path': str(target), 'frames': 420, 'fps': rate, 'seconds': 420 / rate, 'real_time_capture': False})
(output / 'comparison-media.json').write_text(json.dumps(reports, indent=2), encoding='utf-8')
print(json.dumps(reports))
