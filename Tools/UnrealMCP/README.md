# Hodgepodge Blueprint MCP

Project configuration: `../../.codex/config.toml`.
Start the Hodgepodge UE 5.5 editor, then open this project in Codex and call `bridge_status`.
The bridge rejects connections to other projects. Close other UnrealMCP-enabled editors if port 55557 is occupied.

Runtime: `.venv/Scripts/python.exe`; server: `server.py`. Use `python -m pip` rather than pip.exe when maintaining this copied environment.

Verified 2026-09-10: MCP initialization, listing 15 tools, project identity D:/Hodgepodge/Hodgepodge.uproject, and reading the project's asset list.
Previously verified in an isolated UE 5.5 project: create Actor Blueprint, read EventGraph, add Branch, connect BeginPlay, compile, read connections back, save asset.
No gameplay/PIE test or edits to existing Hodgepodge Blueprint assets were performed by this installation.

## Limits

Node tools target ordinary EventGraphs. Animation state machines, function graphs, UMG graphs and material graphs are not supported by this server.
Back up an existing asset using duplicate_asset before editing. Compile before saving; a successful compile is not a gameplay test.
Tools do not promise transactional rollback. After a timeout, inspect the asset before retrying a write.

## Source and local changes

UnrealMCP source: https://github.com/voodoofox/unreal-mcp
Pinned revision: e6f2ffa63f6a463adf96b6425b7b1deb54ff2162. MIT license is included in the plugin directory.
Local changes: UE 5.5 FStringOutputDevice include; Niagara authoring handler disabled due to incompatible APIs; Blueprint compile response reflects actual status; PythonScriptPlugin and Metasound plugin dependencies declared; EnabledByDefault=true.
The plugin is Editor-only. These changes were built against local UE 5.5.4. Preserve this patched source instead of replacing it with upstream without compatibility testing.

To disable, disable UnrealMCP in the UE plugin browser and restart the editor; set enabled=false for mcp_servers.hodge_blueprints in the project Codex config.
