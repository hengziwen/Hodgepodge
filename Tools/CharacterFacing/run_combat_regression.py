"""Reuse the existing real montage/notify/GE regression without changing formal assets."""
import unreal, builtins
from pathlib import Path

s = builtins.HODGE_REACTION_TEST
s['review_name'] = 'facing-regression'
s['review_cases'] = ['move_cancel', 'combo', 'reaction']
root = Path(unreal.Paths.project_dir())
exec(compile((root / 'Tools/HitReaction/review_animation_network.py').read_text(encoding='utf-8'), 'review_animation_network.py', 'exec'))
