"""Read-only smoke test using the exact project Codex MCP configuration."""
import asyncio
import json
import tomllib
from pathlib import Path

from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client

PROJECT = Path(__file__).resolve().parents[2]

async def main():
    config = tomllib.loads((PROJECT / '.codex/config.toml').read_text(encoding='utf-8-sig'))
    entry = config['mcp_servers']['hodge_unreal']
    params = StdioServerParameters(command=entry['command'], args=entry['args'], cwd=entry['cwd'])
    async with stdio_client(params) as (read, write):
        async with ClientSession(read, write) as session:
            await session.initialize()
            assert [tool.name for tool in (await session.list_tools()).tools] == ['unreal']
            found = await session.call_tool('unreal', {'operation': 'search', 'query': 'get widget info', 'limit': 10})
            row = next(x for x in found.structuredContent['results'] if x['nextCall']['action'] == 'get_widget_info')
            described = await session.call_tool('unreal', row['nextCall'])
            info = described.structuredContent
            result = await session.call_tool('unreal', {
                'operation': 'execute', 'tool': info['tool'], 'action': info['action'],
                'params': {'widgetPath': '/Game/CodexText/WBP_Character'},
            })
            assert not result.isError, result
            data = result.structuredContent['data']
            assert data['success'], data
            slots = data['widgetInfo']['slots']
            assert 'CharacterCanvas' in slots and 'ReturnButton' in slots, slots
            print(json.dumps({'status': 'PASS', 'transport': 'Codex config -> stdio -> authenticated native HTTP',
                              'asset': data['assetPath'], 'widget_count': len(slots)}, ensure_ascii=False))

if __name__ == '__main__':
    asyncio.run(main())
