"""Project-scoped UE 5.5 Blueprint MCP. No arbitrary Python/console tool exposed."""
import json
import os
import socket
import threading
from pathlib import Path
from mcp.server.fastmcp import FastMCP

EXPECTED = os.environ.get('HODGE_PROJECT', 'D:/Hodgepodge/Hodgepodge.uproject')
LOCK = threading.RLock()
mcp = FastMCP('Hodgepodge Blueprints', instructions=(
    'UE 5.5 Blueprint bridge. Call bridge_status first. Work only on user-requested assets. '
    'Before editing an existing asset, inspect it and duplicate it to an explicit backup path. '
    'Read/write node tools currently target ordinary EventGraphs only; do not claim support '
    'for function, animation, material, or UMG graphs. Use full /Game paths. '
    'Read node pins before connecting. Compile and inspect the result before saving. '
    'No automatic retry for writes after timeout: read back first. Compilation is not a PIE test.'
))

def raw(command, params):
    with socket.create_connection(('127.0.0.1', 55557), timeout=5) as sock:
        sock.settimeout(40)
        sock.sendall((json.dumps({'type': command, 'params': params})+'\n').encode('utf-8'))
        data = b''
        while b'\n' not in data:
            chunk = sock.recv(262144)
            if not chunk:
                raise RuntimeError('UE closed the connection; inspect state before retrying a write.')
            data += chunk
            if len(data)>32*1024*1024:
                raise RuntimeError('Response too large; narrow the query.')
    response=json.loads(data.split(b'\n',1)[0])
    result=response.get('result', response)
    if response.get('status')=='error' or result.get('success') is False or result.get('error'):
        raise RuntimeError(json.dumps(response, ensure_ascii=False))
    return result

def identity():
    result=raw('run_python_in_unreal', {'code':'import unreal; print(unreal.Paths.convert_relative_path_to_full(unreal.Paths.get_project_file_path()))'})
    actual=result.get('output','').strip().replace('\\','/')
    if actual.casefold()!=EXPECTED.replace('\\','/').casefold():
        raise RuntimeError(f'Connected to {actual!r}; expected {EXPECTED!r}. Open the correct project and close other UE bridge instances.')
    return actual

def call(command, **params):
    with LOCK:
        identity()
        return raw(command, params)

@mcp.tool()
def bridge_status() -> dict:
    """Check that the running UE editor belongs to this project."""
    with LOCK:
        return {'connected':True,'project':identity(),'node_edit_scope':'ordinary EventGraph only'}

@mcp.tool()
def list_assets(path: str='/Game', asset_type: str='Blueprint') -> dict:
    """Find asset paths. Type is a class-name substring; empty means all types."""
    return call('list_assets',path=path,type=asset_type,recursive=True)

@mcp.tool()
def get_blueprint_graph(blueprint_path: str) -> dict:
    """Read ordinary EventGraph node IDs, pins and connections. Not animation/function graphs."""
    return call('get_blueprint_graph',blueprint_name=blueprint_path)

@mcp.tool()
def get_blueprint_defaults(blueprint_path: str) -> dict:
    """Read Blueprint class default properties."""
    return call('get_blueprint_defaults',blueprint_name=blueprint_path)

@mcp.tool()
def duplicate_asset(source_path: str, destination_path: str) -> dict:
    """Create an explicit backup or working copy before editing an existing asset."""
    return call('duplicate_asset',source_path=source_path,destination_path=destination_path)

@mcp.tool()
def create_actor_blueprint(name: str) -> dict:
    """Create an Actor Blueprint under /Game/Blueprints. Name must be a simple asset name."""
    if not name.startswith('BP_') or not name.replace('_','').isalnum():
        raise ValueError('Use a simple BP_Name asset name.')
    return call('create_blueprint',name=name,parent_class='Actor')

@mcp.tool()
def add_begin_play(blueprint_path: str) -> dict:
    """Find or add the BeginPlay event and return its node ID."""
    return call('add_blueprint_event_node',blueprint_name=blueprint_path,event_name='ReceiveBeginPlay')

@mcp.tool()
def add_branch(blueprint_path: str, x: int=300, y: int=0) -> dict:
    """Add an ordinary K2 Branch node and return its node ID."""
    return call('add_blueprint_branch',blueprint_name=blueprint_path,node_position=[x,y])

@mcp.tool()
def add_function_call(blueprint_path: str, target: str, function_name: str, x: int=400, y: int=0) -> dict:
    """Add a supported function call on self or a component. Inspect returned pins before wiring."""
    return call('add_blueprint_function_node',blueprint_name=blueprint_path,target=target,function_name=function_name,node_position=[x,y])

@mcp.tool()
def connect_pins(blueprint_path: str, source_node_id: str, source_pin: str, target_node_id: str, target_pin: str) -> dict:
    """Connect two EventGraph pins by their exact names and node GUIDs."""
    return call('connect_blueprint_nodes',blueprint_name=blueprint_path,source_node_id=source_node_id,source_pin=source_pin,target_node_id=target_node_id,target_pin=target_pin)

@mcp.tool()
def set_pin_default(blueprint_path: str, node_id: str, pin_name: str, default_value: str) -> dict:
    """Set an unconnected pin default; read back and compile to validate the value."""
    return call('set_blueprint_node_pin_default',blueprint_name=blueprint_path,node_id=node_id,pin_name=pin_name,default_value=default_value)

@mcp.tool()
def set_blueprint_default(blueprint_path: str, property_name: str, property_value: object) -> dict:
    """Set a supported class default property; inspect the property type first."""
    return call('set_blueprint_property',blueprint_name=blueprint_path,property_name=property_name,property_value=property_value)

@mcp.tool()
def compile_blueprint(blueprint_path: str) -> dict:
    """Compile and return the actual Blueprint compilation status. Does not save."""
    return call('compile_blueprint',blueprint_name=blueprint_path)

@mcp.tool()
def save_blueprint(blueprint_path: str) -> dict:
    """Compile, then save only this Blueprint if compilation succeeds."""
    with LOCK:
        result=call('compile_blueprint',blueprint_name=blueprint_path)
        if not result.get('compiled'):
            raise RuntimeError(f'Not saving a failed Blueprint: {result}')
        return call('save_asset',asset_path=blueprint_path)

@mcp.tool()
def read_editor_log(lines: int=100) -> dict:
    """Read the newest project editor log for compiler errors; bounded to 500 lines."""
    logs=Path(EXPECTED).parent/'Saved'/'Logs'
    files=list(logs.glob('*.log'))
    if not files: return {'message':'No project editor logs found.'}
    latest=max(files,key=lambda p:p.stat().st_mtime)
    return {'path':str(latest),'text':'\n'.join(latest.read_text(encoding='utf-8',errors='replace').splitlines()[-max(1,min(lines,500)):])}

if __name__=='__main__':
    mcp.run(transport='stdio')
