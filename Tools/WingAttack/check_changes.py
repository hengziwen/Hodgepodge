"""核对源码修改范围、原编码和新增中文注释的行数约束。"""
import json,hashlib,subprocess
from pathlib import Path
ROOT=Path('E:/Project/Git/Hodgepodge');WORK=ROOT/'Saved/WingAttack'
before=WORK/'Before/Source'
changed=[];added=[];violations=[]
for path in (ROOT/'Source').rglob('*'):
    if not path.is_file():continue
    old=before/path.relative_to(ROOT/'Source')
    if not old.exists():added.append(path.relative_to(ROOT).as_posix())
    elif old.read_bytes()!=path.read_bytes():changed.append(path.relative_to(ROOT).as_posix())
    else:continue
    raw=path.read_bytes()
    encoding='utf-16' if raw.startswith(b'\xff\xfe') else 'utf-8-sig'
    if old.exists():
        old_raw=old.read_bytes()
        assert raw.startswith(b'\xff\xfe')==old_raw.startswith(b'\xff\xfe'),str(path)
    count=0
    for number,line in enumerate(raw.decode(encoding).splitlines(),1):
        if line.lstrip().startswith('//'):count+=1
        else:count=0
        if count>3:violations.append({'file':str(path),'line':number})
result={'changed_source':changed,'added_source':added,'comment_block_violations':violations}
(WORK/'change-review.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
assert not violations,violations
print(json.dumps(result,ensure_ascii=False))
