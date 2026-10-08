import unreal,builtins,json
s=builtins.HODGE_SKILL_TEST
s['source']=s['pc'].get_controlled_pawn();s['target']=s['target_pc'].get_controlled_pawn();assert s['source'] and s['target']
for i,h in enumerate([s['source'],s['target']]):
    h.character_movement.set_movement_mode(unreal.MovementMode.MOVE_NONE)
    h.capsule_component.set_collision_response_to_channel(unreal.CollisionChannel.ECC_PAWN,unreal.CollisionResponseType.ECR_OVERLAP)
    h.set_actor_location(unreal.Vector(1000+i*200,1000,300),False,False);h.set_actor_rotation(unreal.Rotator(0,0,0),False)
    hc=h.get_component_by_class(unreal.HodgeHealthComponent);assert hc.get_health()>0
s['source_asc']=s['source'].get_editor_property('player_state').get_hodge_ability_system_component()
s['target_asc']=s['target'].get_editor_property('player_state').get_hodge_ability_system_component()
s['health']=s['target'].get_component_by_class(unreal.HodgeHealthComponent)
s['manager']=s['source'].get_component_by_class(unreal.HodgeEquipmentManagerComponent)
s['ga_class']=unreal.load_asset('/Game/CodexText/SkillHitVolumes/GA_VolumeExample').generated_class()
s['grant_class']=unreal.load_asset('/Game/CodexText/SkillHitVolumes/BP_VolumeLabGrant').generated_class()
s['ability_set']=unreal.load_asset('/Game/CodexText/SkillHitVolumes/AS_VolumeLab')
lib=unreal.get_default_object(unreal.load_class(None,'/Script/GameplayAbilities.AbilitySystemBlueprintLibrary'))
attributes=s['target_asc'].get_all_attributes()
s['health_attribute']=next(a for a in attributes if a.export_text().endswith('AttributeName="Health")') or 'AttributeName="Health"' in a.export_text())
print(json.dumps({'source':s['source'].get_path_name(),'target':s['target'].get_path_name(),'health':s['health'].get_health(),'health_attribute':s['health_attribute'].export_text(),'manager':s['manager'].get_path_name(),'ability_api':[n for n in dir(s['source_asc']) if 'activate' in n or 'attribute' in n or 'abilit' in n]},ensure_ascii=False))
