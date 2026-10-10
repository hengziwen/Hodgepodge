"""Summarize historical v1.2 reports only; v1.3 uses summarize_timing_results.py."""
import json,re,subprocess,xml.etree.ElementTree as ET
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]; SAVED=ROOT/'Saved/DashSprintImplementation'
native=json.loads((SAVED/'Native6/index.json').read_text(encoding='utf-8-sig'))
assert native['failed']==0 and native['notRun']==0,native
expected={'single-0ms':7,'network-0ms':7,'network-100ms':7,'combat-0ms':3,'combat-100ms':3,'hit-exit-0ms':2,'hit-exit-100ms':2}
suites={}
for name,count in expected.items():
    report=json.loads((SAVED/(name+'.json')).read_text(encoding='utf-8'))
    assert not report.get('error') and len(report['cases'])==count,(name,report.get('error'))
    assert all(case.get('passed') for case in report['cases']),name
    suites[name]={'passed':count,'samples':len(report['samples'])}

files={}
for name in ['native-coverage.xml','e2e-single-coverage.xml']:
    path=SAVED/name
    assert path.exists(),path
    tree=ET.parse(path)
    for cls in tree.findall('.//class'):
        filename=cls.get('filename','').replace('\\','/').lower()
        source_index=filename.find('source/')
        if source_index<0:continue
        filename=str(ROOT/filename[source_index:]).replace('\\','/').lower()
        lines=files.setdefault(filename,{})
        for line in cls.findall('./lines/line'):
            number=int(line.get('number'));hit=int(line.get('hits','0'))>0
            lines[number]=lines.get(number,False) or hit

changed={}
diff=subprocess.check_output(['git','diff','--unified=0','--','Source/Hodgepodge'],cwd=ROOT,text=True,encoding='utf-8')
current=None
for line in diff.splitlines():
    if line.startswith('+++ b/'):
        current=str(ROOT/line[6:]).replace('\\','/').lower();changed.setdefault(current,set())
    elif current and line.startswith('@@'):
        match=re.search(r'\+(\d+)(?:,(\d+))?',line)
        start=int(match[1]);count=int(match[2] or 1);changed[current].update(range(start,start+count))
for filename in subprocess.check_output(['git','ls-files','--others','--exclude-standard','--','Source/Hodgepodge'],cwd=ROOT,text=True).splitlines():
    path=ROOT/filename
    if path.suffix in ['.cpp','.h']:
        changed[str(path).replace('\\','/').lower()]=set(range(1,len(path.read_text(encoding='utf-8-sig').splitlines())+1))

totals={'covered':0,'measurable':0};details=[]
for filename,numbers in changed.items():
    if '/private/tests/' in filename:continue
    measured=files.get(filename,{})
    selected={number:hit for number,hit in measured.items() if number in numbers}
    if not selected:continue
    covered=sum(selected.values());total=len(selected)
    totals['covered']+=covered;totals['measurable']+=total
    details.append({'file':filename,'covered':covered,'measurable':total,'uncovered':sorted(n for n,h in selected.items() if not h)})
totals['percent']=round(100*totals['covered']/totals['measurable'],2) if totals['measurable'] else None
result={'native':{k:native[k] for k in ['succeeded','succeededWithWarnings','failed','notRun']},
    'e2e':suites,'coverage':{'tool':'OpenCppCoverage 0.9.9.0','scope':'Runtime changed MSVC/PDB measurable lines; native suite plus single-player E2E',
    'branch_coverage':None,'blueprint_coverage_percent':None,'runtime_changed_lines':totals,'files':details}}
target=ROOT/'Docs/Validation/dash-sprint-results-2026-10-10.json'
target.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'native':result['native'],'e2e_total':sum(v['passed'] for v in suites.values()),'coverage':totals},ensure_ascii=False))
