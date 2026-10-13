"""Capture the native editor preview window using the documented screenshot API."""
import contextlib,io,json,sys
sys.path.insert(0,'E:/Project/Git/Hodgepodge/Tools/HitReaction')
args=sys.argv[:]
sys.argv=sys.argv[:1]
with contextlib.redirect_stdout(io.StringIO()):import mcp_execute as m
sys.argv=args
m.call({'operation':'describe','tool':'control_editor','action':'screenshot'})
result=m.call({'operation':'execute','tool':'control_editor','action':'screenshot','params':{'mode':'full_editor_window','filename':sys.argv[1] if len(sys.argv)>1 else 'wing-editor-preview','returnBase64':False}})
print(json.dumps(result,ensure_ascii=False))
