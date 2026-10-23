"""将实际逐帧截图编成离线对照视频；不补帧、不伪称实时录屏。"""
import json
import subprocess
import sys
from pathlib import Path

import cv2
import numpy as np

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'Saved' / 'HitPresentation'
sys.path.insert(0, str(OUT / 'MediaDeps'))
import imageio_ffmpeg


def read_capture(stage):
    """输入必须有完整截图和均匀的实际仿真时间，不允许用重复帧补齐。"""
    folder = OUT / stage
    data = json.loads((folder / 'capture.json').read_text(encoding='utf-8'))
    assert (data['error'] is None or data.get('cleanup_recovered')) and data['fixed_simulation_step'] == 1 / 60
    assert len(data['frames']) == len(data['rows']) == 420
    for index, frame in enumerate(data['frames']):
        assert Path(frame['path']).is_file()
        assert abs(frame['elapsed'] - index / 60) < .001
    return data


before = read_capture(sys.argv[1] if len(sys.argv) > 1 else 'BeforeReplay60')
after = read_capture(sys.argv[2] if len(sys.argv) > 2 else 'After60')
assert before['baseline_assets_latest_runtime'] and not after['baseline_assets_latest_runtime']
frames = []
for index in range(420):
    # 两边采用同一新运行时；左侧仅恢复原资产配置，避免误导为旧代码版本。
    canvas = np.full((720, 1920, 3), 24, dtype=np.uint8)
    for side, (capture, title) in enumerate([(before, 'A: Original asset configuration'), (after, 'B: Revised prototype assets')]):
        shot = cv2.imread(capture['frames'][index]['path'])
        assert shot is not None and shot.shape[:2] == (540, 960)
        x = side * 960
        canvas[70:610, x:x + 960] = shot
        row = capture['rows'][index]
        lines = [title, 'Same latest runtime | offline 60 Hz | no recorded audio',
                 't=%.3f  HP=%.0f  contacts=%d' % (row['elapsed'], row['health'], row['presentation_count']),
                 str(row['source_montage']) + ' -> ' + str(row['target_montage']),
                 str(row['phase']) + ' / ' + str(row['outcome']),
                 'Attack/Body=%s/%s  pos=%.3f weight=%.2f strength=%.2f' % (row.get('attack_rank', '?'), row.get('body_rank', '?'), row.get('target_position', 0), row.get('target_weight', 0), row.get('strength', 0))]
        for text, y, scale in zip(lines, [25, 52, 632, 654, 678, 705], [.64, .48, .55, .49, .43, .49]):
            cv2.putText(canvas, text, (x + 14, y), cv2.FONT_HERSHEY_SIMPLEX, scale, (225, 225, 225), 1, cv2.LINE_AA)
    frames.append(canvas)

results = []
for speed, fps in [('normal', 60), ('quarter', 15)]:
    target = OUT / ('comparison-' + speed + '.mp4')
    # 无音轨是明确的证据边界，不能将导出的接触 WAV 冒充实况收音。
    command = [imageio_ffmpeg.get_ffmpeg_exe(), '-y', '-v', 'error', '-f', 'rawvideo',
               '-pixel_format', 'bgr24', '-video_size', '1920x720', '-framerate', str(fps),
               '-i', '-', '-an', '-c:v', 'libx264', '-crf', '20', '-pix_fmt', 'yuv420p',
               '-movflags', '+faststart', str(target)]
    process = subprocess.Popen(command, stdin=subprocess.PIPE)
    try:
        for frame in frames:
            process.stdin.write(frame.tobytes())
        process.stdin.close()
        assert process.wait() == 0
    finally:
        if process.poll() is None:
            process.kill()
    results.append({'path': str(target), 'frames': len(frames), 'fps': fps, 'seconds': len(frames) / fps})

cv2.imwrite(str(OUT / 'comparison-contact.png'), frames[130])
(OUT / 'comparison-media.json').write_text(json.dumps(results, indent=2), encoding='utf-8')
print(json.dumps(results))
