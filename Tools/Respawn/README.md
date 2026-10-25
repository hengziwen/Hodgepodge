# Hero 自动重生制作与验证

设计见 `Docs/Design/hero-auto-respawn-2026-10-24.md`，实际结果见 `Docs/Validation/hero-auto-respawn-2026-10-24.md`。运行环境为本机 UE 5.5.4 和现有项目网关，不安装新依赖。

## 配置

完成 Editor/Game 常规构建并冷启动本项目编辑器后，通过现有网关运行：

```powershell
E:/Python/python.exe -X utf8 Tools/HitReaction/mcp_execute.py Tools/Respawn/configure_assets.py
E:/Python/python.exe -X utf8 Tools/HitReaction/mcp_execute.py Tools/Respawn/validate_assets.py
```

配置工具仅备份/保存正式 `DA_Pover` 与 `DA_Dafult_PawnData`，追加原生能力并启用已批准参数。重复运行不重复追加；若启用后的参数已被手调，则停止而不覆盖。资产与备份记录在 `Saved/PlayerRespawn_20261024/`。

## 运行集成验证

确保没有运行 PIE，也没有未保存的用户地图/资产；现有启动器会检查这些条件。顺序执行：

```powershell
E:/Python/python.exe -X utf8 Tools/Respawn/run_batch.py single listen0 listen100 dedicated0 dedicated100 dedicated-parallel100
E:/Python/python.exe -X utf8 Tools/Respawn/run_batch.py failures
```

每项使用真实 Damage/Death GA 自动链，结果按模式写入 JSON，失败历史自动改名保留。`run_parallel.py` 验证双玩家和新身体输入，`run_failures.py` 使用原生日志核对实际次数及间隔。

故障专项要求该编辑器日志路径为 `Saved/PlayerRespawn_20261024/RuntimeEditor.log`；启动编辑器时传入相应 `-abslog=<完整路径>`。该专项临时修改 Hero CDO 的出生碰撞策略，恢复后蓝图包仍可能被编辑器标为脏，因此放在最后，**不要保存测试产生的蓝图脏包**，正常关闭本次验证编辑器并重新打开再运行后续套件。脚本异常时也会恢复覆盖并销毁阻挡盒。

单人故障注入不等于真实网络故障；所有 PIE 模式都不等于独立专服或打包验收。报告必须按实际执行面描述。
