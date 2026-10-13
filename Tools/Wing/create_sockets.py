"""Use the native socket capability; UE exposes socket fields as read-only to Python."""
import contextlib
import io
import json
import shutil
import sys
from pathlib import Path

ROOT = Path('E:/Project/Git/Hodgepodge')
sys.path.insert(0, str(ROOT / 'Tools/HitReaction'))
with contextlib.redirect_stdout(io.StringIO()):
    import mcp_execute as gateway

package = 'Content/Wuwa/PoverEquipment/残象之翼_Skeleton.uasset'
backup = ROOT / 'Saved/WingImplementation/BeforeAssets' / package
if not backup.exists():
    backup.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(ROOT / package, backup)
code = "import unreal,json\nm=unreal.load_asset('/Game/Wuwa/PoverEquipment/残象之翼')\nprint(json.dumps([str(m.get_socket_by_index(i).socket_name) for i in range(m.num_sockets())]))"
state = gateway.call({'operation': 'execute', 'tool': 'system_control', 'action': 'execute_python', 'params': {'code': code}})
existing = json.loads(state['data']['output'].strip())
gateway.call({'operation': 'describe', 'tool': 'animation_physics', 'action': 'create_socket'})
for name, bone in [('WingHitRoot', 'Wing000'), ('WingHitMid', 'Wing003'), ('WingHitTip', 'Wing006')]:
    if name in existing:
        continue
    result = gateway.call({'operation': 'execute', 'tool': 'animation_physics', 'action': 'create_socket',
                           'params': {'skeletonPath': '/Game/Wuwa/PoverEquipment/残象之翼_Skeleton',
                                      'socketName': name, 'attachBoneName': bone, 'save': True}})
    assert result['success'], result
    print(name, bone, 'created and saved')
