"""Use actual Melee damage/control to interrupt an active root-motion Pivot."""
import unreal
from pathlib import Path

source = Path(unreal.Paths.project_dir()).joinpath('Tools/DashSprint/run_hit_exit.py').read_text(encoding='utf-8')
source = source.replace("'DashTimingRefinement'", "'PivotFootSlide'")
source = source.replace("'hit-exit-'", "'pivot-hit-exit-'")
source = source.replace("'saw_sprint':False", "'saw_sprint':False,'saw_pivot':False")
source = source.replace('if age<1.8:', 'if age<3.0:')
source = source.replace('move,unreal.Vector(0,1,0)', 'move,unreal.Vector(0,-1 if age>=1.25 else 1,0)')
source = source.replace("        if ctx['saw_sprint'] and age>.8:",
                        "        root=policy(owner).get_profile().get_editor_property('SprintPivotMontage')\n        pivot=lib.inspect_montage_state(owner,root)\n        ctx['saw_pivot'] |= pivot.playing and pivot.position>.25\n        if ctx['saw_pivot']:")
source = source.replace('if age>2.5:', 'if age>3.5:')
source = source.replace("assert ctx['saw_sprint'] and ctx['cast']", "assert ctx['saw_sprint'] and ctx['saw_pivot'] and ctx['cast']")
source = source.replace("            assert not ctx['samples'][-1]['sprinting']", "            assert not lib.inspect_montage_state(owner,root).playing,'accepted hit must stop own root-motion montage'\n            assert not ctx['samples'][-1]['sprinting']")
exec(compile(source, 'pivot_hit_exit.py', 'exec'))
