"""Local stdio adapter for ChiR24's native MCP gateway (no token in config)."""
import asyncio
import argparse
from contextlib import asynccontextmanager
from datetime import timedelta
from pathlib import Path

from mcp import ClientSession, types
from mcp.client.streamable_http import streamablehttp_client
from mcp.server.lowlevel import Server
from mcp.server.stdio import stdio_server

parser = argparse.ArgumentParser()
parser.add_argument('--project', type=Path, required=True)
parser.add_argument('--port', type=int, required=True)
args = parser.parse_args()
project = args.project.resolve()
if not project.is_file() or project.suffix != '.uproject':
    parser.error('--project must point to an existing .uproject')
if not 1024 <= args.port <= 65535:
    parser.error('invalid port')

@asynccontextmanager
async def native_session():
    token_file = project.parent / 'Saved/MCP/capability-token'
    if not token_file.is_file():
        raise RuntimeError('Open the configured UE project with McpAutomationBridge enabled first.')
    token = token_file.read_text(encoding='utf-8').strip()
    if not token:
        raise RuntimeError('The project MCP token file is empty.')
    async with streamablehttp_client(
        f'http://127.0.0.1:{args.port}/mcp',
        headers={'X-MCP-Capability-Token': token},
        timeout=timedelta(seconds=150),
    ) as (read, write, _):
        async with ClientSession(read, write) as session:
            await session.initialize()
            yield session

server = Server('hodge_unreal', version='0.5.30-ue55-local', instructions=(
    'ChiR24 native Unreal gateway for the configured local project. '
    'Use unreal search, then describe, then execute with the returned tool/action and schema. '
    'Preserve native consent requirements. Inspect before editing. '
    'After a batch of Blueprint changes, compile with saveAfterCompile=true; '
    'individual mutation saves may be throttled. Report compile, PIE and persistence separately.'
))

@server.list_tools()
async def list_tools():
    async with native_session() as session:
        return (await session.list_tools()).tools

@server.call_tool()
async def call_tool(name: str, arguments: dict) -> types.CallToolResult:
    if name != 'unreal':
        return types.CallToolResult(isError=True, content=[types.TextContent(type='text', text='Unknown tool')])
    async with native_session() as session:
        return await session.call_tool(name, arguments)

async def main():
    async with stdio_server() as (read, write):
        await server.run(read, write, server.create_initialization_options())

if __name__ == '__main__':
    asyncio.run(main())
