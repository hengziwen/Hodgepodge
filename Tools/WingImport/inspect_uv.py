"""Inspect effect-section UV layout and source texture channels."""
exec((__import__('pathlib').Path(__file__).parent/'inspect_sources.py').read_text(encoding='utf-8').split('rows = []')[0])
path=SOURCE/'EDarkNanzhuWingMd10011/Model/EDarkNanzhuWingMd10011.fbx'
root,version=parse_fbx.parse(str(path))
geo=next(e for e in child(root,b'Objects').elems if e.id==b'Geometry')
uv=list(child(child(geo,b'LayerElementUV'),b'UV').props[0])
uv_indices=list(child(child(geo,b'LayerElementUV'),b'UVIndex').props[0])
materials=list(child(child(geo,b'LayerElementMaterial'),b'Materials').props[0])
poly=list(child(geo,b'PolygonVertexIndex').props[0]);mat=0;data={}
for i,vertex in enumerate(poly):
    idx=uv_indices[i]*2
    data.setdefault(materials[mat],[]).append(uv[idx:idx+2])
    if vertex<0:mat+=1
for index,points in data.items():
    print(index,'range',[(min(p[a] for p in points),max(p[a] for p in points)) for a in range(2)],'first',points[:12])
