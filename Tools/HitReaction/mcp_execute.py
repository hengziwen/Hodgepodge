"""Execute a project validation script through the authenticated local UE gateway."""
import json
import sys
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOKEN = (ROOT / 'Saved/MCP/capability-token').read_text(encoding='utf-8').strip()
SESSION = None
COUNTER = 0

def rpc(method, params, notification=False):
    global SESSION, COUNTER
    COUNTER += 1
    payload = {'jsonrpc': '2.0', 'method': method, 'params': params}
    if not notification:
        payload['id'] = COUNTER
    headers = {'Content-Type': 'application/json', 'Accept': 'application/json, text/event-stream',
               'X-MCP-Capability-Token': TOKEN}
    if SESSION:
        headers['Mcp-Session-Id'] = SESSION
    req = urllib.request.Request('http://127.0.0.1:3016/mcp', json.dumps(payload).encode(), headers=headers)
    with urllib.request.urlopen(req, timeout=90) as response:
        SESSION = response.headers.get('Mcp-Session-Id', SESSION)
        raw = response.read().decode('utf-8')
    if not raw:
        return {}
    if raw.lstrip().startswith('{'):
        result = json.loads(raw)
    else:
        messages = [json.loads(line[5:].strip()) for line in raw.splitlines() if line.startswith('data:')]
        if notification:
            return {}
        result = next(message for message in messages if message.get('id') == COUNTER)
    if 'error' in result:
        raise RuntimeError(result['error'])
    return result.get('result', result)

def call(args):
    result = rpc('tools/call', {'name': 'unreal', 'arguments': args})
    data = result.get('structuredContent')
    if data is None:
        data = json.loads(next(row['text'] for row in result['content'] if row.get('type') == 'text'))
    if result.get('isError'):
        detail = data.get('typedError', {}).get('unrealDetail', {})
        raise RuntimeError(detail.get('error') or data.get('error') or data)
    return data

rpc('initialize', {'protocolVersion': '2024-11-05', 'capabilities': {},
                   'clientInfo': {'name': 'hodge-hit-reaction-validation', 'version': '1'}})
rpc('notifications/initialized', {}, notification=True)
found = call({'operation': 'search', 'query': 'execute python', 'limit': 10})
next_call = next(row['nextCall'] for row in found['results'] if row['nextCall']['action'] == 'execute_python')
desc = call(next_call)
if len(sys.argv) < 2:
    print(json.dumps(desc, ensure_ascii=False))
else:
    code = Path(sys.argv[1]).read_text(encoding='utf-8-sig')
    result = call({'operation': 'execute', 'tool': desc['tool'], 'action': desc['action'], 'params': {'code': code}})
    data = result.get('data', result)
    print(json.dumps(data, ensure_ascii=False))
    if not result.get('success') or data.get('success') is False:
        raise RuntimeError('Editor validation script failed')
