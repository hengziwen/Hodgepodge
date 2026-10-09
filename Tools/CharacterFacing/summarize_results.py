"""Validate actual reports and summarize instrumentable lines, including changed-line coverage."""
import argparse, json, re, subprocess, xml.etree.ElementTree as ET
from pathlib import Path

root = Path(__file__).resolve().parents[2]
saved = root / 'Saved/FacingImplementation'
parser = argparse.ArgumentParser()
parser.add_argument('--base', default='HEAD', help='Revision before the feature; use the report diff_base after commit')
args = parser.parse_args()
diff_base = subprocess.run(['git','rev-parse',args.base],cwd=root,text=True,capture_output=True,check=True).stdout.strip()
native = json.loads((saved / 'Native2/index.json').read_text(encoding='utf-8-sig'))
assert native['failed'] == 0 and native['notRun'] == 0, 'Native tests did not pass'
e2e = []
for name in ['facing-single-0ms', 'facing-network-0ms', 'facing-network-100ms']:
    report = json.loads((saved / (name + '.json')).read_text(encoding='utf-8'))
    assert not report['error'] and len(report['cases']) == 8, (name, report['error'])
    assert all(case['passed'] for case in report['cases']), name
    e2e.append({'name': name, 'passed': len(report['cases']), 'samples': len(report['samples'])})

coverage = ET.parse(saved / 'combined-coverage.xml').getroot()
regressions = []
for lag in [0, 100]:
    report = json.loads((root / 'Saved/AnimationNetworkReview' / f'facing-regression-{lag}ms.json').read_text(encoding='utf-8'))
    assert not report['error'] and len(report['cases']) == 3
    combo = next(case for case in report['cases'] if case['case'] == 'combo')
    assert [item['montage'] for item in combo['owner_transitions']] == ['AM_Attack01_Montage','AM_Attack02_Montage','AM_Attack03_Montage','AM_Attack05_Montage','AM_Attack04_Montage']
    assert any(row['owner']['controlled'] for row in report['samples'] if row['case'] == 'reaction')
    move = [row for row in report['samples'] if row['case'] == 'move_cancel']
    assert not move[-1]['owner_abilities'] and not move[-1]['server_abilities']
    regressions.append({'lag_ms': lag, 'passed_groups': 3})
files = []
for cls in coverage.findall('.//class'):
    filename = cls.attrib['filename'].replace('\\', '/')
    index = filename.find('Source/')
    if index < 0:
        continue
    relative = filename[index:]
    lines = {int(line.attrib['number']): int(line.attrib['hits']) for line in cls.findall('./lines/line')}
    diff = subprocess.run(['git', 'diff', diff_base, '--unified=0', '--', relative], cwd=root, text=True, encoding='utf-8', capture_output=True, check=True).stdout
    tracked = subprocess.run(['git', 'ls-files', '--error-unmatch', relative], cwd=root, capture_output=True).returncode == 0
    changed = set()
    if not tracked:
        changed = set(lines)
    else:
        cursor = None
        for row in diff.splitlines():
            match = re.match(r'@@ -\d+(?:,\d+)? \+(\d+)(?:,\d+)? @@', row)
            if match:
                cursor = int(match.group(1))
            elif cursor is not None and row.startswith('+') and not row.startswith('+++'):
                changed.add(cursor); cursor += 1
            elif cursor is not None and row.startswith(' '):
                cursor += 1
    measured_changed = changed & set(lines)
    files.append({'file': relative, 'covered_lines': sum(hit > 0 for hit in lines.values()), 'measured_lines': len(lines),
                  'changed_covered_lines': sum(lines[number] > 0 for number in measured_changed),
                  'changed_measured_lines': len(measured_changed),
                  'uncovered_changed_lines': sorted(number for number in measured_changed if lines[number] == 0)})

covered = sum(row['covered_lines'] for row in files); total = sum(row['measured_lines'] for row in files)
changed_covered = sum(row['changed_covered_lines'] for row in files); changed_total = sum(row['changed_measured_lines'] for row in files)
runtime = [row for row in files if row['file'].startswith('Source/Hodgepodge/')]
runtime_changed_covered = sum(row['changed_covered_lines'] for row in runtime)
runtime_changed_total = sum(row['changed_measured_lines'] for row in runtime)
summary = {'native': {key: native[key] for key in ['succeeded', 'succeededWithWarnings', 'failed', 'notRun']},
           'e2e': e2e, 'combat_regression': regressions, 'coverage': {'tool': 'OpenCppCoverage 0.9.9.0', 'configuration': 'Win64 Development, optimized MSVC PDB',
             'covered_lines': covered, 'measured_lines': total, 'line_percent': round(100 * covered / total, 2),
             'changed_covered_lines': changed_covered, 'changed_measured_lines': changed_total,
             'diff_base': diff_base,
             'changed_line_percent': round(100 * changed_covered / changed_total, 2) if changed_total else None,
             'runtime_changed_covered_lines': runtime_changed_covered, 'runtime_changed_measured_lines': runtime_changed_total,
             'runtime_changed_line_percent': round(100 * runtime_changed_covered / runtime_changed_total, 2) if runtime_changed_total else None,
             'branch_coverage_available': False, 'blueprint_line_coverage_available': False, 'files': files}}
(saved / 'test-summary.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding='utf-8')
print(json.dumps(summary, ensure_ascii=False, indent=2))
