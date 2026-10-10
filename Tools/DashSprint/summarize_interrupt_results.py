"""Verify complete reports and preservation of pre-existing Main asset edits."""
import hashlib, json
from pathlib import Path

root = Path(__file__).resolve().parents[2]
saved = root / 'Saved/DashInterrupt'
native = json.loads((saved / 'Native2/index.json').read_text(encoding='utf-8-sig'))
assert native['failed'] == 0 and native['notRun'] == 0
assert native['succeeded'] + native['succeededWithWarnings'] == 34
reports = {}
for name, count in [('single-0ms.json', 10), ('network-0ms.json', 10), ('network-100ms.json', 10),
                    ('single-combat-0ms.json', 3), ('combat-0ms.json', 3), ('combat-100ms.json', 3),
                    ('single-0ms-interrupt.json', 10), ('network-0ms-interrupt.json', 10), ('network-100ms-interrupt.json', 10)]:
    report = json.loads((saved / name).read_text(encoding='utf-8-sig'))
    assert not report['error'], (name, report['error'])
    assert len(report['cases']) == count and all(case['passed'] for case in report['cases']), name
    if name.endswith('-interrupt.json'):
        assert all(case['outside_old_window'] for case in report['cases'] if case['case'].startswith('early_attack_')), name
        for case in report['cases']:
            samples = [r for r in report['samples'] if r['case'] == case['case']]
            if case['case'].startswith('death_'):
                assert not any(r['server']['dash'] for r in samples), (name, case['case'])
                if case['case'] == 'death_ability':
                    assert any(r['owner']['death'] for r in samples), name
                    assert not any(r['owner']['dash'] and r['owner']['death'] for r in samples), name
                    assert not samples[-1]['owner']['dash'], name
                else: assert not any(r['owner']['dash'] for r in samples), (name, case['case'])
            elif name.startswith('network-'):
                assert any(r['observer']['dash_weight'] > .1 for r in samples), (name, case['case'])
    reports[name] = {'passed': count, 'cases': report['cases']}
baseline = json.loads((saved / 'baseline.json').read_text(encoding='utf-8-sig'))
preserved = []
for entry in baseline:
    if not entry['path'].startswith('Content/Main/') or entry['path'] == 'Content/Main/Data/PawnData/DA_Dafult_PawnData.uasset': continue
    assert hashlib.sha256((root / entry['path']).read_bytes()).hexdigest() == entry['sha256'], entry['path']
    preserved.append(entry['path'])
assets = json.loads((saved / 'asset-verification.json').read_text(encoding='utf-8'))
assert len(assets['checks']) == 5 and assets['death_protected'] and assets['utility_untargeted']
result = {'date': '2026-10-10', 'engine': '5.5.4', 'native': {key: native[key] for key in ['succeeded', 'succeededWithWarnings', 'failed', 'notRun']},
    'pie_reports': reports, 'pie_passed': sum(report['passed'] for report in reports.values()), 'asset_checks': assets,
    'preserved_existing_main_assets': preserved, 'coverage': {'instrumented': False, 'scope': 'Native assertions and PIE scenario validation; no code/Blueprint percentage collected'},
    'not_verified': ['Cook/package', 'separate-process networking', 'packet loss and latency above 100ms', 'branch and Blueprint coverage']}
target = root / 'Docs/Validation/dash-interrupt-results-2026-10-10.json'
target.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'native_passed': 34, 'pie_passed': result['pie_passed'], 'preserved_assets': len(preserved)}))
