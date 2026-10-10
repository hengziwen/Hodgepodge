"""Validate v1.3 reports and measure new Runtime lines against this task's saved baseline."""
import difflib, hashlib, json, subprocess, xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SAVED = ROOT / 'Saved/DashTimingRefinement'
native = json.loads((SAVED / 'Native3/index.json').read_text(encoding='utf-8-sig'))
assert native['failed'] == 0 and native['notRun'] == 0
expected = {'single-0ms': 10, 'network-0ms': 10, 'network-100ms': 10,
            'combat-0ms': 3, 'combat-100ms': 3, 'hit-exit-0ms': 2, 'hit-exit-100ms': 2}
suites = {}
for name, count in expected.items():
    report = json.loads((SAVED / (name + '.json')).read_text(encoding='utf-8'))
    assert not report.get('error') and len(report['cases']) == count, name
    assert all(case.get('passed') for case in report['cases']), name
    details = {}
    if name.startswith(('single-', 'network-')):
        details['sprint_start_seconds'] = {
            case: round(next(row['t'] for row in report['samples'] if row['case'] == case and row['owner']['sprinting']), 4)
            for case in ['held_sprint', 'delayed_hold', 'delayed_handoff']}
        for role in (['owner'] if name.startswith('single-') else ['owner', 'server', 'observer']):
            frames = [(row['t'], x['time']) for row in report['samples'] if row['case'] == 'pivot_turn'
                      for x in row[role]['pivot'] if x['sequence'] == 'A_Hero_Sprint_Turn']
            assert len(frames) >= 3, (name, role, 'Pivot must actually play')
            rates = [(b[1] - a[1]) / (b[0] - a[0]) for a, b in zip(frames, frames[1:])
                     if b[0] > a[0] and b[1] >= a[1]]
            assert any(.65 < rate < 1.35 for rate in rates), (name, role, rates)
            assert not any(row[role]['locomotion'] == 'Pivot' for row in report['samples']
                           if row['case'] == 'pivot_turn' and 3.3 < row['t'] < 3.45), (name, role, 'Pivot must exit before release')
            details[role + '_pivot'] = {'frames': len(frames), 'last_sample_time': round(frames[-1][0], 4),
                                      'last_animation_time': round(frames[-1][1], 4)}
    suites[name] = {'passed': count, 'samples': len(report['samples']), 'details': details}

measured = {}
for name in ['native-coverage.xml', 'e2e-single-coverage.xml']:
    for cls in ET.parse(SAVED / name).findall('.//class'):
        filename = cls.get('filename', '').replace('\\', '/').lower()
        offset = filename.find('source/')
        if offset < 0:
            continue
        filename = filename[offset:]
        rows = measured.setdefault(filename, {})
        for line in cls.findall('./lines/line'):
            number = int(line.get('number'))
            rows[number] = rows.get(number, False) or int(line.get('hits', '0')) > 0

baseline = {row['path'].lower(): row for row in json.loads((SAVED / 'baseline.json').read_text(encoding='utf-8'))}
tracked = {p.lower(): p for p in subprocess.check_output(['git', 'ls-files', '--', 'Source/Hodgepodge'], cwd=ROOT, text=True).splitlines()}
def decode(data):
    return data.decode('utf-16' if data.startswith((b'\xff\xfe', b'\xfe\xff')) else 'utf-8-sig').splitlines()
details = []
covered = total = 0
for filename, lines in measured.items():
    if not filename.startswith('source/hodgepodge/') or '/tests/' in filename:
        continue
    path = ROOT / filename
    if not path.exists():
        continue
    current = decode(path.read_bytes())
    backup = SAVED / 'baseline' / filename
    if backup.exists():
        previous = decode(backup.read_bytes())
    elif filename in tracked:
        previous = decode(subprocess.check_output(['git', 'show', 'HEAD:' + tracked[filename]], cwd=ROOT))
    else:
        previous = []
    changed = set()
    for op, _, _, first, last in difflib.SequenceMatcher(None, previous, current, autojunk=False).get_opcodes():
        if op in ['replace', 'insert']:
            changed.update(range(first + 1, last + 1))
    selected = {n: hit for n, hit in lines.items() if n in changed}
    if not selected:
        continue
    hit = sum(selected.values())
    covered += hit
    total += len(selected)
    details.append({'file': filename, 'covered': hit, 'measurable': len(selected),
                    'uncovered': sorted(n for n, value in selected.items() if not value)})

preserved = ['Content/Main/Character/Hero/Ability/BasicAttack/GE_MeleeDamage_Instant.uasset',
             'Content/Main/Character/Hero/BP_Hero_Pover.uasset'] + [
             f'Content/Main/Character/Hero/Anim/Montages/AM_Attack0{i}_Montage.uasset' for i in range(1, 6)]
for filename in preserved:
    assert hashlib.sha256((ROOT / filename).read_bytes()).hexdigest() == baseline[filename.lower()]['sha256'], filename

result = {'version': '1.3', 'native': {k: native[k] for k in ['succeeded', 'succeededWithWarnings', 'failed', 'notRun']},
          'e2e': suites, 'e2e_total': sum(suite['passed'] for suite in suites.values()),
          'coverage': {'tool': 'OpenCppCoverage 0.9.9.0',
                       'scope': 'Runtime added/modified PDB measurable lines relative to task baseline; native plus single-player E2E',
                       'covered': covered, 'measurable': total, 'percent': round(100 * covered / total, 2) if total else None,
                       'branch_coverage': None, 'blueprint_coverage_percent': None, 'files': details},
          'user_assets_preserved': preserved}
target = ROOT / 'Docs/Validation/dash-sprint-timing-results-2026-10-10.json'
target.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print(json.dumps({k: result[k] for k in ['native', 'e2e_total']} | {'coverage': {k: result['coverage'][k] for k in ['covered', 'measurable', 'percent']}}, ensure_ascii=False))
