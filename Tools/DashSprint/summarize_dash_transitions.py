"""Evaluate Dash height continuity, handoff velocity and completed regression reports."""
import json, math, statistics, hashlib
from pathlib import Path

root = Path(__file__).resolve().parents[2]
saved = root / 'Saved/DashTransitions'
native = json.loads((saved / 'Native8/index.json').read_text(encoding='utf-8-sig'))
assert native['failed'] == 0 and native['notRun'] == 0
assert native['succeeded'] + native['succeededWithWarnings'] == 35
files = [('fixed-single-0ms.json', 7), ('fixed-network-0ms.json', 7), ('fixed-network-100ms.json', 7),
         ('single-0ms.json', 10), ('network-0ms.json', 10), ('network-100ms.json', 10),
         ('single-combat-0ms.json', 3), ('combat-0ms.json', 3), ('combat-100ms.json', 3)]
reports = {}; metrics = {}
state = json.loads((saved / 'final-state.json').read_text(encoding='utf-8'))
assert not state['pie_worlds'] and not state['dirty_packages']
for filename, count in files:
    report = json.loads((saved / filename).read_text(encoding='utf-8-sig'))
    assert report['error'] is None and len(report['cases']) == count and all(c['passed'] for c in report['cases']), (filename, report['error'])
    reports[filename] = {'passed': count, 'cases': [c['case'] for c in report['cases']]}
    if filename.startswith('fixed-'):
        metrics[filename] = {}
        for role in ['owner']:
            values = [r[role] for r in report['samples'] if r['case'] == 'idle_tap']
            tail = [v['pelvis_cs_z'] for v in values if v['montage_position'] >= 1.2 and v['dash_weight'] > .2]
            idle = [r[role]['pelvis_cs_z'] for r in report['samples'] if r['case'] == 'idle_tap' and 2 < r['t'] < 3.4]
            gap = statistics.mean(tail) - statistics.mean(idle)
            assert abs(gap) < .75, (filename, role, gap)
            sprint = [r for r in report['samples'] if r['case'] == 'held_sprint']
            travel = next(r['t'] for r in sprint if r[role]['dash_committed'])
            speed = min(math.hypot(*r[role]['velocity']) for r in sprint if travel + .2 < r['t'] < travel + .8)
            assert speed > 150, (filename, role, speed)
            metrics[filename][role] = {'dash_tail_to_idle_pelvis_delta_cm': gap, 'minimum_handoff_speed_cm_s': speed}
baseline = json.loads((saved / 'baseline-single-0ms.json').read_text(encoding='utf-8'))
idle_rows = [r for r in baseline['samples'] if r['case'] == 'idle_tap']
old_gap = statistics.mean(r['owner']['pelvis_cs_z'] for r in idle_rows if r['owner']['montage_position'] >= 1.2 and r['owner']['dash_weight'] > .2) - statistics.mean(r['owner']['pelvis_cs_z'] for r in idle_rows if 2 < r['t'] < 3.4)
expected_changes = {'Content/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase.uasset', 'Content/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility.uasset'}
assets = json.loads((saved / 'baseline.json').read_text(encoding='utf-8'))
preserved = []
for entry in assets:
    if not entry['path'].startswith('Content/Main/') or entry['path'] in expected_changes: continue
    assert hashlib.sha256((root / entry['path']).read_bytes()).hexdigest() == entry['sha256'], entry['path']
    preserved.append(entry['path'])
result = {'date': '2026-10-10', 'engine': '5.5.4', 'native_passed': 35, 'pie_passed': sum(r['passed'] for r in reports.values()),
          'native_succeeded_with_warnings': native['succeededWithWarnings'], 'native_failed': native['failed'],
          'native_report': 'Saved/DashTransitions/Native8/index.json',
          'builds': {'editor': {'exit_code': 0, 'log': 'Saved/DashTransitions/EditorBuild11.log'},
                     'game': {'exit_code': 0, 'log': 'Saved/DashTransitions/GameBuild7.log'}},
          'reports': reports, 'baseline_pelvis_drop_cm': old_gap, 'metrics': metrics,
          'final_editor_state': state,
          'preserved_main_assets': preserved, 'expected_asset_changes': sorted(expected_changes),
          'not_verified': ['Cook/package', 'separate-process networking', 'packet loss and delay above 100ms', 'instrumented code/Blueprint coverage', 'all terrain and camera angles']}
(root / 'Docs/Validation/dash-transitions-results-2026-10-10.json').write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'native_passed': 35, 'pie_passed': result['pie_passed'], 'metrics': metrics, 'preserved_assets': len(preserved)}))
