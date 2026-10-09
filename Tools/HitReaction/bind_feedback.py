import builtins, sys
sys.path.insert(0, 'E:/Project/Git/Hodgepodge/Tools/HitReaction')
import feedback_listener
s = builtins.HODGE_REACTION_TEST
for pawn in [s['host'], s['server_remote'], s['remote'], s['client_host']]: feedback_listener.bind(pawn, s)
print('Bound retained UObject feedback listeners for all network roles')
