import unreal,builtins
s=builtins.HODGE_REACTION_TEST
for w in [s['world']] + s.get('client_worlds', [s['client_world']]):unreal.SystemLibrary.execute_console_command(w,'NetEmulation.PktLag 100')
s['lag_ms']=100
s['output_name']='hero_two_clients_100ms' if s.get('use_formal_profile') else 'network_simulated_100ms'
print('Enabled 100ms packet lag in all validation PIE worlds')
