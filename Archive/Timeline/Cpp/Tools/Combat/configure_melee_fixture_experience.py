"""重新编译并打开编辑器后，通过 MCP 迁移测试判定来源，不启动 PIE。"""
import json
from pathlib import Path
import urllib.request


ROOT = Path(__file__).resolve().parents[2]
URL = 'http://127.0.0.1:3016/mcp'
FIXTURE = '/Game/CodexText/CombatHitWindows/Fixture'


def main():
    headers = {
        'X-MCP-Capability-Token': (ROOT / 'Saved/MCP/capability-token').read_text().strip(),
        'Content-Type': 'application/json',
        'Accept': 'application/json, text/event-stream',
    }

    def rpc(method, params, ident=2):
        body = {'jsonrpc': '2.0', 'method': method, 'params': params}
        if ident is not None:
            body['id'] = ident
        request = urllib.request.Request(URL, json.dumps(body).encode(), headers)
        with urllib.request.urlopen(request, timeout=60) as response:
            session = response.headers.get('Mcp-Session-Id')
            if session:
                headers['Mcp-Session-Id'] = session
            raw = response.read().decode()
        if not raw:
            return {}
        if raw.startswith(('event:', 'data:')):
            messages = [json.loads(line[5:].strip()) for line in raw.splitlines() if line.startswith('data:')]
            result = next(message for message in messages if message.get('id') == ident)
        else:
            result = json.loads(raw)
        if 'error' in result:
            raise RuntimeError(result['error'])
        return result.get('result', {})

    def execute(tool, action, params):
        # 使用当前桥接契约，再执行具体能力，失败时停止后续资产修改。
        rpc('tools/call', {'name': 'unreal', 'arguments': {'operation': 'describe', 'tool': tool, 'action': action}})
        result = rpc('tools/call', {'name': 'unreal', 'arguments': {
            'operation': 'execute', 'tool': tool, 'action': action, 'params': params}})
        data = result.get('structuredContent', result)
        if 'data' in data:
            data = data['data']
        if not data.get('success'):
            raise RuntimeError(data)
        return data

    rpc('initialize', {'protocolVersion': '2025-03-26', 'capabilities': {},
                       'clientInfo': {'name': 'hodge-fixture-migration', 'version': '1'}}, 1)
    rpc('notifications/initialized', {}, None)
    try:
        sources = json.loads((ROOT / 'Tools/Combat/melee_fixture_hit_sources.json').read_text(encoding='utf-8'))
        code = '''import unreal
assert unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world() is None, 'Stop PIE before migration'
assert unreal.get_default_object(unreal.HodgeCombatCharacter).get_component_by_class(unreal.HodgeCombatComponentBase) is None, 'Rebuild and reopen the editor first'
actual = unreal.Paths.convert_relative_path_to_full(unreal.Paths.get_project_file_path())
assert actual.replace('\\\\', '/').casefold() == PROJECT.casefold()
component_path = FIXTURE + '/BP_MeleeTestCombatComponent'
if not unreal.EditorAssetLibrary.does_asset_exist(component_path):
    factory = unreal.BlueprintFactory()
    factory.set_editor_property('parent_class', unreal.HodgeCombatComponentBase)
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset('BP_MeleeTestCombatComponent', FIXTURE, None, factory)
    assert asset, 'CombatComponent must be Blueprintable in the loaded DLL'
entries = []
for text in SOURCES:
    entry = unreal.HodgeHitSource()
    entry.import_text(text)
    entries.append(entry)
unreal.BlueprintEditorLibrary.compile_blueprint(unreal.load_asset(component_path))
default = unreal.get_default_object(unreal.EditorAssetLibrary.load_blueprint_class(component_path))
default.set_editor_property('hit_sources', entries)
assert unreal.EditorAssetLibrary.save_asset(component_path, False)
experience_path = '/Game/Main/Experiences/CodexText/Exp_MeleeValidation'
if not unreal.EditorAssetLibrary.does_asset_exist(experience_path):
    asset = unreal.EditorAssetLibrary.duplicate_asset('/Game/Main/Experiences/Exp_HodgeDefaultExperience', experience_path)
    assert asset
    default = unreal.get_default_object(unreal.EditorAssetLibrary.load_blueprint_class(experience_path))
    default.set_editor_property('default_pawn_data', unreal.load_asset(FIXTURE + '/DA_MeleeTestPawn_Body'))
assert unreal.EditorAssetLibrary.save_asset(experience_path, False)
experience = unreal.get_default_object(unreal.EditorAssetLibrary.load_blueprint_class(experience_path))
actions = [action for action in experience.get_editor_property('actions') if action.get_class().get_path_name() == '/Script/GameFeatures.GameFeatureAction_AddComponents']
assert len(actions) == 1
print('ACTION_PATH=' + actions[0].get_path_name())
'''
        prefix = 'PROJECT=' + repr(str(ROOT / 'Hodgepodge.uproject').replace('\\', '/'))
        prefix += '\nFIXTURE=' + repr(FIXTURE) + '\nSOURCES=' + repr(sources) + '\n'
        result = execute('system_control', 'execute_python', {'code': prefix + code})
        action_path = next(line.split('=', 1)[1] for line in result['output'].splitlines() if line.startswith('ACTION_PATH='))
        params = {'objectPath': action_path, 'propertyName': 'ComponentList'}
        entries = execute('inspect', 'get_property', params)['value']
        # 只替换测试 Experience 的判定组件，保留装备、输入及正式体验配置。
        old_class = '/Script/Hodgepodge.HodgeCombatComponentBase'
        new_class = FIXTURE + '/BP_MeleeTestCombatComponent.BP_MeleeTestCombatComponent_C'
        assert sum(old_class in entry or new_class in entry for entry in entries) == 1
        entries = [entry.replace(old_class, new_class) for entry in entries]
        execute('inspect', 'set_property', dict(params, value=entries))
        assert execute('inspect', 'get_property', params)['value'] == entries
        print('Migrated: /Game/Main/Experiences/CodexText/Exp_MeleeValidation')
    finally:
        request = urllib.request.Request(URL, headers=headers, method='DELETE')
        with urllib.request.urlopen(request, timeout=10):
            pass


if __name__ == '__main__':
    main()
