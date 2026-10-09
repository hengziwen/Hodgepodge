# 旋转模式验证与覆盖率

适用 UE 5.5.4。运行需要当前项目编辑器及本地 MCP 网关；令牌由现有 `Tools/HitReaction/mcp_execute.py` 内部读取，不打印凭据。

作者工具 `configure_animation.py` 只修改 Main 的主图和固定层，保留方向资源，编译成功后保存两份资产。后续人工改图后不要无检查地重跑作者工具。

单人顺序：`start_single.py` → 等待 PIE 生成 → `setup_single.py` → `setup_actors.py` → `run_e2e.py`。使用一个本地玩家，避免无窗口测试创建无 Slate 用户的第二个本地玩家。例如：

```powershell
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CharacterFacing/start_single.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CharacterFacing/setup_single.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CharacterFacing/setup_actors.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CharacterFacing/run_e2e.py
```

网络顺序：`start_network.py` → 等待两客户端和 PIE 服务器 → `Tools/HitReaction/setup_two_clients.py` → `Tools/HitReaction/setup_actors.py` → `run_e2e.py`。可先执行 `Tools/HitReaction/set_network_lag.py` 设置每个 NetDriver 100ms 包延迟，再复跑。不是独立服务器可执行文件或真实 RTT=100ms。

每组结束检查 `Saved/FacingImplementation/facing-single-0ms.json` 或 `facing-network-0ms/100ms.json`：error 必须为空且 cases 数量为 8。Started 只代表已启动，测试进程退出码也不能代替 JSON 结果。

测试覆盖真实移动输入、方向资源、自由镜头、预留侧移／后退、请求释放、真实 Definition 动作覆盖和服务器拒绝。测试上下文不包含锁定目标或锁定 CameraMode。

结束必须执行 `Tools/HitReaction/stop_pie.py`，清理监听和 NetEmulation；不要保存全部 Dirty Package 或运行时测试配置。启动脚本只加载现有战斗地图，不修改项目默认地图。

原生测试组为 `Hodge.Facing`，并回归 `Hodge.Rotation+Hodge.Combo+Hodge.Combat+Hodge.HitReaction`。报告使用 Automation JSON 的 failed／notRun 数量判定。

行覆盖率使用 OpenCppCoverage 0.9.9.0 与 MSVC PDB；工具位于忽略的 Saved 目录。它报告编译行覆盖率，不提供分支覆盖率。原始 XML、二进制覆盖数据、构建日志、蓝图诊断和逐帧结果保留在 Saved/FacingImplementation；提交的验证文档说明实际范围与限制。

`summarize_results.py --base <实施前提交>` 检查实际结果并计算差异行覆盖。实施前提交记录在报告 JSON 的 coverage.diff_base；提交后复算须使用该值，不能把干净工作区的空差异当作本功能覆盖率。
