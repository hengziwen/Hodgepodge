import unreal, builtins
from pathlib import Path

worlds = unreal.EditorLevelLibrary.get_pie_worlds(False)
assert len(worlds) == 1
pc = unreal.GameplayStatics.get_player_controller(worlds[0], 0)
assert pc and pc.get_controlled_pawn()
builtins.HODGE_REACTION_TEST = {'world': worlds[0], 'source_pc': pc, 'target_pc': pc,
    'use_formal_profile': True, 'grant_path': '/Game/CodexText/HitReactionValidation/BP_HeroPresentationProbe'}
print('Configured one local player; no synthetic second Slate user')
