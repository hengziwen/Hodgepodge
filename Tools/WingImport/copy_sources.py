"""Preserve the source hierarchy and verify copies before importing."""
import hashlib, json, shutil, subprocess
from pathlib import Path
PROJECT=Path('E:/Project/Git/Hodgepodge')
SOURCE=Path('D:/下载/BaiduwangpanDownloads/Anzhu')
DEST=PROJECT/'Content/Wuwa/Wing'
REPORT=PROJECT/'Saved/WingImport'
REPORT.mkdir(parents=True,exist_ok=True)
status=subprocess.check_output(['git','status','--short'],cwd=PROJECT)
(REPORT/'status-before.txt').write_bytes(status)
rows=[]
for path in sorted(SOURCE.rglob('*')):
    if not path.is_file():continue
    relative=path.relative_to(SOURCE)
    target=DEST/relative
    digest=hashlib.sha256(path.read_bytes()).hexdigest()
    if target.exists():
        assert hashlib.sha256(target.read_bytes()).hexdigest()==digest,str(target)
    else:
        target.parent.mkdir(parents=True,exist_ok=True)
        shutil.copy2(path,target)
    rows.append({'relative':relative.as_posix(),'sha256':digest,'bytes':path.stat().st_size})
(REPORT/'source-files.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'copied':len(rows),'bytes':sum(r['bytes'] for r in rows)}))
