"""Report authored animation channels, including clips that deliberately hide the wing."""
exec((__import__('pathlib').Path(__file__).parent/'inspect_sources.py').read_text(encoding='utf-8').split('rows = []')[0])
rows=[]
for path in sorted(SOURCE.rglob('*.fbx')):
    root,version=parse_fbx.parse(str(path));objects=child(root,b'Objects')
    index={e.props[0]:e for e in objects.elems};connections=child(root,b'Connections')
    links={}
    for link in connections.elems:
        if len(link.props)>=4:
            links.setdefault(link.props[1],[]).append((link.props[2],decode(link.props[3])))
    channels=[]
    for curve in objects.elems:
        if curve.id!=b'AnimationCurve':continue
        values=list(child(curve,b'KeyValueFloat').props[0])
        for node_id,axis in links.get(curve.props[0],[]):
            for model_id,property_name in links.get(node_id,[]):
                model=index.get(model_id)
                if model and model.id==b'Model':
                    channels.append({'bone':decode(model.props[1]).split('::')[0],'property':property_name,'axis':axis,
                        'min':min(values),'max':max(values),'first':values[0],'last':values[-1]})
    rows.append({'source':path.relative_to(SOURCE).as_posix(),'channels':channels})
(REPORT/'source-channels.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2),encoding='utf-8')
for row in rows:
    zero=[c['bone'] for c in row['channels'] if c['property']=='Lcl Scaling' and abs(c['max'])<1e-7]
    constant=sum(abs(c['max']-c['min'])<1e-7 for c in row['channels'])
    print(row['source'],'constant',constant,'/',len(row['channels']),'zero-scale',len(set(zero)), 'root',[c for c in row['channels'] if c['bone'] in ['Root','Bone_Wing000'] and c['property']=='Lcl Scaling'])
