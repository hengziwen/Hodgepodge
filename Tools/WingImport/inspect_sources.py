"""Read FBX metadata without changing or converting the source assets."""
import sys, json, hashlib
from pathlib import Path
sys.path.insert(0, 'E:/Project/Git/Hodgepodge/Saved/PoverEquipmentImport/Runtime/python')
import bpy
from io_scene_fbx import parse_fbx

SOURCE = Path('D:/下载/BaiduwangpanDownloads/Anzhu')
REPORT = Path('E:/Project/Git/Hodgepodge/Saved/WingImport')
REPORT.mkdir(parents=True, exist_ok=True)

def child(element, key):
    return next((e for e in element.elems if e.id == key), None)

def decode(value):
    if isinstance(value, bytes):
        return value.decode('utf-8', errors='replace').replace('\x00\x01', '::')
    if hasattr(value, 'tolist'):
        return value.tolist()
    return value

def properties(element):
    table = child(element, b'Properties70')
    return {decode(p.props[0]): [decode(v) for v in p.props[4:]] for p in table.elems} if table else {}

rows = []
for path in sorted(SOURCE.rglob('*.fbx')):
    root, version = parse_fbx.parse(str(path))
    objects = child(root, b'Objects')
    models = [e for e in objects.elems if e.id == b'Model']
    materials = [e for e in objects.elems if e.id == b'Material']
    geometry = [e for e in objects.elems if e.id == b'Geometry']
    connections = child(root, b'Connections')
    row = {'source': path.relative_to(SOURCE).as_posix(), 'sha256': hashlib.sha256(path.read_bytes()).hexdigest(), 'version': version,
           'global': properties(child(root, b'GlobalSettings')),
           'models': [{'id': e.props[0], 'name': decode(e.props[1]), 'type': decode(e.props[2]), 'properties': properties(e)} for e in models],
           'materials': [{'id': e.props[0], 'name': decode(e.props[1]), 'properties': properties(e)} for e in materials],
           'textures': [{'name': decode(e.props[1]), 'filename': [decode(v) for v in (child(e,b'FileName').props if child(e,b'FileName') else [])]} for e in objects.elems if e.id == b'Texture'],
           'stacks': [{'name': decode(e.props[1]), 'properties': properties(e)} for e in objects.elems if e.id == b'AnimationStack'],
           'connections': [[decode(v) for v in c.props] for c in connections.elems], 'geometry': []}
    for geo in geometry:
        vertices = child(geo, b'Vertices')
        points = list(vertices.props[0]) if vertices else []
        mat = child(geo, b'LayerElementMaterial')
        indices = child(mat, b'Materials') if mat else None
        row['geometry'].append({'id': geo.props[0], 'name': decode(geo.props[1]), 'vertices': len(points)//3,
            'min': [min(points[a::3]) for a in range(3)] if points else [], 'max': [max(points[a::3]) for a in range(3)] if points else [],
            'material_polygon_counts': {str(i): list(indices.props[0]).count(i) for i in set(indices.props[0])} if indices else {}})
    rows.append(row)
(REPORT/'sources.json').write_text(json.dumps(rows, ensure_ascii=False, indent=2), encoding='utf-8')
for row in rows:
    print(json.dumps({k: row[k] for k in ['source', 'global', 'geometry', 'materials', 'textures', 'stacks']}, ensure_ascii=False))
