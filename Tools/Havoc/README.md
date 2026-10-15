# 共鸣回路／暗涌制作与验证工具

首版正式资产已接入，包含灭音、强化五段、破命、燃料Buff及Designer暗流条。设计见Docs/Design/havoc-dark-surge.md，最终记录见Docs/Validation/havoc-dark-surge-2026-10-15.md。

## 配置

`/Game/Main/Character/Hero/Ability/Havoc/GA_HavocRules`配置每秒消费、长按阈值、有界等待及GE引用。GE_DarkFlow_Init配置容量，GE_DarkSurge_State配置20%增伤，GE_DarkFlow_Drain配置周期。收益在各Definition.DarkFlowGainPerPhase／DarkFlowGainOnCast中修改。

命中与恢复窗口直接在七条AM_Ex_*_Montage通知轨编辑；灭音专用提交通知的Burst包含Box尺寸。保留源动画根运动；Box仅在提交时取位置，不持续跟随。首版新动作未开启朝向Warp，暂不接攻击吸附。

AS_Havoc已由玩家PawnData授予。E共享现有8秒CD；不要另建破命独立CD。WBP_DarkFlow经EAS注入血条上方扩展点；E新图标、重击和变奏暂缓。

编辑器开启、PIE停止且没有无关未保存包时，可通过既有认证网关运行：

```powershell
$env:PYTHONIOENCODING='utf-8'
E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Havoc/configure_ui.py
E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Havoc/configure_assets.py
E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Havoc/validate_assets.py
E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Havoc/validate_ui_assets.py
```

作者工具先备份旧资产，标记已经配置的新资产，重复执行不重置动作窗口。audit_assets.py只读导出源动画和既有资产信息。修改模板后若需要重新初始化，先审查metadata及用户修改，不直接删除标记批量覆盖。

## 开发控制台直接回满暗流

运行中的游戏窗口按`~`，输入`HodgeFillDarkFlow`并回车。命令只把当前玩家暗流补到实际容量上限，默认100；不自动灭音、不清E／R冷却、不补R能量。暗涌中使用后仍继续按正常速率消耗，不是无限能量开关。

客户端复用Controller.ServerCheat精确白名单，由服务器通过正式增量GE执行并复制HUD；死亡或容量未初始化时拒绝，Shipping／Test构建不执行实际效果。

联机回归复用本目录的调度工具：`$env:HODGE_HAVOC_SUITE='debug'`后执行`E:/Python/python.exe Tools/Havoc/run_batch.py single listen dedicated`。正式命令用例是`Hodge.Havoc.DebugFill`；完整记录见Docs/Validation/havoc-debug-fill-2026-10-15.md。

## 原生与PIE验证

按照Docs/AI_DEVELOPMENT.md保存关闭编辑器，完成Editor及Game常规构建。原生自动化命令见验证报告；查询JSON中的失败／未运行数，不能只看UE进程退出码。

编辑器冷启动就绪后执行真实输入核心与命中测试：

```powershell
$env:PYTHONIOENCODING='utf-8'
$env:HODGE_HAVOC_SUITE='core'
E:/Python/python.exe Tools/Havoc/run_batch.py single listen dedicated listen-lag
$env:HODGE_HAVOC_SUITE='hits'
E:/Python/python.exe Tools/Havoc/run_batch.py single listen dedicated
E:/Python/python.exe Tools/Havoc/export_test_results.py --report Saved/Havoc/AutomationGEFinal/index.json
```

run_batch.py复用既有PIE启动／停止工具；只在本轮地图中隔离敌人，不修改正式地图。核心套件用真实InputAction发动技能，补满和驱散通过明确标注的权威GE夹具；命中套件使用真实敌人HP变化验证收益、20%增伤与多目标／换段去重。

导出器同时检查原生、正式资产及8份运行时报告（核心4、命中3、单人E/R HUD回归1），保存用例到可入库JSON。原始采样及截图留在Saved/Havoc，不提交日志缓存。PIE Dedicated不等同于独立专服或打包验收。
