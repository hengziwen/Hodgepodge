"""按项目实际配置只读检查 MCP：握手、工具列表及蓝图桥项目身份。"""
from __future__ import annotations

import asyncio
import json
import os
from pathlib import Path
import sys
import tomllib

from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client


async def inspect(project: Path, name: str, config: dict) -> dict:
    """不执行资产写入，只验证真实服务响应；令牌由原有网关在内存中读取。"""
    server = StdioServerParameters(command=config["command"], args=config.get("args", []),
                                  cwd=config.get("cwd"), env={**os.environ, **config.get("env", {})})
    async with stdio_client(server) as (read, write):
        async with ClientSession(read, write) as session:
            await session.initialize()
            catalog = await session.list_tools()
            result = dict(server=name, tool_count=len(catalog.tools), tools=[tool.name for tool in catalog.tools])
            if name == "hodge_blueprints":
                status = await session.call_tool("bridge_status", {})
                if status.isError:
                    raise RuntimeError(f"{name} 项目身份检查失败")
                payload = status.structuredContent
                if not payload:
                    payload = json.loads(next(item.text for item in status.content if item.type == "text"))
                expected = (project / "Hodgepodge.uproject").as_posix().casefold()
                actual = payload.get("project", "").replace("\\", "/").casefold()
                if actual != expected:
                    raise RuntimeError(f"{name} 返回的项目身份不匹配")
                result["project"] = payload["project"]
                result["node_edit_scope"] = payload.get("node_edit_scope")
            elif name == "hodge_unreal":
                tool = next(tool for tool in catalog.tools if tool.name == "unreal")
                result["operations"] = tool.inputSchema["properties"]["operation"]["enum"]
                discovery = await session.call_tool("unreal", {"operation": "search", "query": "editor project", "limit": 3, "maxBytes": 4096})
                if discovery.isError:
                    raise RuntimeError("原生网关只读能力检索失败")
                payload = discovery.structuredContent or json.loads(
                    next(item.text for item in discovery.content if item.type == "text"))
                if payload.get("success") is False:
                    raise RuntimeError("原生网关能力检索返回失败")
                rows = payload.get("results", [])
                result["search"] = dict(success=True, returned=len(rows), total=payload.get("total"))
                candidate = next(row for row in rows if row.get("effect") == "read")
                contract = await session.call_tool("unreal", candidate["nextCall"])
                if contract.isError:
                    raise RuntimeError("原生网关只读能力描述失败")
                result["described_read_capability"] = candidate["capability"]
            return result


async def main() -> None:
    """顺序检查两个本地服务，避免并发占用同一编辑器。"""
    project = Path(__file__).resolve().parents[2]
    config = tomllib.loads((project / ".codex/config.toml").read_text(encoding="utf-8"))
    for name in ("hodge_blueprints", "hodge_unreal"):
        result = await asyncio.wait_for(inspect(project, name, config["mcp_servers"][name]), timeout=60)
        print(json.dumps(result, ensure_ascii=False, indent=2), flush=True)


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except Exception as error:
        # 展开聚合异常便于定位连接问题，输出前去除可能出现的本地令牌。
        token_path = Path(__file__).resolve().parents[2] / "Saved/MCP/capability-token"
        token = token_path.read_text(encoding="utf-8").strip() if token_path.is_file() else ""
        pending = [error]
        while pending:
            item = pending.pop()
            if isinstance(item, BaseExceptionGroup):
                pending.extend(item.exceptions)
                continue
            message = str(item).replace(token, "[redacted]") if token else str(item)
            print(f"MCP 只读检查失败：{type(item).__name__}：{message[:600]}", file=sys.stderr)
        raise SystemExit(1)
