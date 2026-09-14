"""Run a project editor script through the installed native MCP gateway."""
import asyncio,json,sys
from pathlib import Path
from mcp import ClientSession
from mcp.client.streamable_http import streamablehttp_client

async def main():
    code=Path(sys.argv[1]).read_text(encoding='utf-8-sig')
    token=Path('D:/Hodgepodge/Saved/MCP/capability-token').read_text().strip()
    async with streamablehttp_client('http://127.0.0.1:3016/mcp',headers={'X-MCP-Capability-Token':token}) as (r,w,_):
        async with ClientSession(r,w) as session:
            await session.initialize()
            found=(await session.call_tool('unreal',{'operation':'search','query':'execute python','limit':10})).structuredContent
            call=next(x['nextCall'] for x in found['results'] if x['nextCall']['action']=='execute_python')
            desc=(await session.call_tool('unreal',call)).structuredContent
            result=await session.call_tool('unreal',{'operation':'execute','tool':desc['tool'],'action':desc['action'],'params':{'code':code}})
            data=result.structuredContent
            print(json.dumps(data.get('data',data),ensure_ascii=False))
            if result.isError or not data.get('success') or not data.get('data',{}).get('success'):raise RuntimeError('Editor script failed')
asyncio.run(main())
