"""迁入已审计的 Lyra 跳字资源；保留虚拟路径，不覆盖不同内容的已有资产。"""
import hashlib
import json
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[2]
SOURCE = Path('E:/Project/UProject/study/Epic/LyraStarterGame')
ASSETS = [
    'Effects/Particles/Impacts/NS_DamageNumbers',
    'Effects/Materials/DamageNumbers/Instances/MI_numba',
    'Effects/Materials/DamageNumbers/Instances/MI_numberSingleCard',
    'Effects/Materials/DamageNumbers/M_numba',
    'Effects/NiagaraModules/NM_BPSystemEvent',
    'Effects/Textures/Flipbooks/numbers_color',
]


def migrate():
    """按字节校验源与目标，差异需要人工处理，禁止静默覆盖。"""
    manifest = []
    for asset in ASSETS:
        relative = Path('Content') / (asset + '.uasset')
        source, target = SOURCE / relative, ROOT / relative
        digest = hashlib.sha256(source.read_bytes()).hexdigest()
        if target.exists():
            assert hashlib.sha256(target.read_bytes()).hexdigest() == digest, str(target)
        else:
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)
        manifest.append({'package': '/Game/' + asset, 'sha256': digest, 'bytes': target.stat().st_size})
    output = ROOT / 'Saved/DamageNumbers'
    output.mkdir(parents=True, exist_ok=True)
    (output / 'lyra-migration.json').write_text(json.dumps(manifest, indent=2), encoding='utf-8')
    print(json.dumps(manifest, indent=2))


if __name__ == '__main__':
    migrate()
