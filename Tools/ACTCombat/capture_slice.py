"""复用真实五段采集器，对照本轮备份与新样例；只在内存切换并配对恢复。"""
from pathlib import Path
import unreal

baseline = globals().get('ACT_CAPTURE_BASELINE', False)
stage = 'ACT-Before-Complete' if baseline else 'ACT-After'
# 高分辨率逐帧落盘不是实时采集，给 420 帧留出有界的渲染时间。
scope = {'CAPTURE_STAGE': stage, 'CAPTURE_FIXED_STEP': True, 'CAPTURE_BASELINE_ASSETS': False, 'CAPTURE_WALL_TIMEOUT': 600}
assert not unreal.EditorLevelLibrary.get_pie_worlds(True)
exec(Path(unreal.Paths.project_dir(), 'Tools/HitReaction/capture_presentation.py').read_text(encoding='utf-8'), scope)
if baseline:
    # 基线恢复资产但不回退代码，采集时 DLL 身份另存，不能标成原封存代码录像。
    for path in ['/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction',
                 '/Game/CodexText/HitReactionValidation/Dummy/DA_DummyHitReaction']:
        asset = unreal.load_asset(path)
        backup = unreal.load_asset('/Game/Tests/Combat/ACTPrototype/Backups/' + asset.get_name() + '_BeforeACT')
        assert backup
        for key in ['Animations', 'bAllowNaturalRecoveryTail', 'bEnableGroundRecovery', 'KnockbackStrengthCurve']:
            stored = asset.get_editor_property(key)
            scope['state']['restore_assets'].append((asset, key, stored))
            asset.set_editor_property(key, backup.get_editor_property(key))
    scope['BASELINE'] = True
