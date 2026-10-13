"""Start a requested suite only after PIE replicas and combat components are ready."""
import unreal,time,json
from pathlib import Path
kind=globals().get('SUITE','standalone')
assert kind in ('standalone','shared')
state={'start':time.monotonic()}
def ready_tick(_):
 worlds=unreal.EditorLevelLibrary.get_pie_worlds(False)
 if time.monotonic()-state['start']>30:
  unreal.unregister_slate_post_tick_callback(state['callback']);raise RuntimeError('PIE readiness timeout')
 if not worlds or time.monotonic()-state['start']<1.5:return
 for w in worlds:
  heroes=unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeHeroCharacter)
  enemies=unreal.GameplayStatics.get_all_actors_of_class(w,unreal.HodgeEnemyCharacter)
  if not heroes or len(enemies)<2:return
  if any(not e.get_enemy_init_component().is_gameplay_ready() for e in enemies):return
  if any(not h.get_component_by_class(unreal.HodgeEquipmentManagerComponent) or not h.get_hodge_ability_system_component() for h in heroes):return
  if not unreal.SystemLibrary.is_server(w) and not any(h.get_local_role()==unreal.NetRole.ROLE_AUTONOMOUS_PROXY for h in heroes):return
 unreal.unregister_slate_post_tick_callback(state['callback'])
 name='run_standalone_e2e.py' if kind=='standalone' else 'run_shared_e2e.py'
 source=Path('E:/Project/Git/Hodgepodge/Tools/CombatActionRequests')/name
 exec(compile(source.read_text(encoding='utf-8'),str(source),'exec'),{})
state['callback']=unreal.register_slate_post_tick_callback(ready_tick)
print('Waiting for PIE readiness: '+kind)
