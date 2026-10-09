import unreal,builtins
s=builtins.HODGE_REACTION_TEST
for p in [s['host'],s['server_remote']]:
    for handle in p.get_editor_property('PlayerState').get_hodge_ability_system_component().get_all_abilities():
        ga=s['lib'].call_method('GetGameplayAbilityFromSpecHandle',(p.get_editor_property('PlayerState').get_hodge_ability_system_component(),handle))[0]
        if ga and s['lib'].call_method('IsGameplayAbilityActive',(ga,)):
            unreal.HodgeCombatValidationLibrary.queue_ability_action(p.get_editor_property('PlayerState').get_hodge_ability_system_component(),handle,True)
s.update({'source':s['server_remote'],'source_owner':s['remote'],'target':s['host'],
          'owned_target':s['host'],'observer_target':s['client_host'],'output_name':'network_simulated_'+str(s['lag_ms'])+'ms'})
print('Switched to client-origin attack and simulated-proxy target observation')
