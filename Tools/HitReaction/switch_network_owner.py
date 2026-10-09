import builtins
s = builtins.HODGE_REACTION_TEST
s.update({'source': s['host'], 'source_owner': s['host'], 'target': s['server_remote'],
          'owned_target': s['remote'], 'observer_target': s['server_remote'],
          'output_name': 'network_owner_' + str(s['lag_ms']) + 'ms'})
print('Switched to remote owning-client target observation')
