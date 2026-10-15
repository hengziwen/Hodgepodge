# 大招能量回满命令验证

日期：2026-10-15；UE 5.5.4；未提交Git。

新增开发控制台命令`HodgeFillResonance`，在当前玩家控制器执行。实际容量为准，首版回满100；只补能量，不清除12秒CD、不自动放大招，死亡或资源未准备完成时拒绝。Shipping／Test构建通过编译宏关闭实际效果，未执行这两种构建。

Lyra参考：`LyraCheatManager::God`客户端转发到PlayerController.ServerCheat；`ApplySetByCallerHeal`通过ASC应用GE。Hodge完整CheatManager尚未实现，因此复用现有ServerCheat RPC，只开放精确白名单；ServerCheatAll保持未启用，不将任意远端字符串交给ConsoleCommand。

代码：`Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h`和对应Private主cpp；新增Exec及权威处理，复用HodgeResonanceEnergyEffect与PlayerState ASC。编辑器QueueUIAction的ResonanceFill只在后续游戏Tick执行正式控制台命令，避免Python脚本调用空间影响RPC；没有直接改测试属性来冒充联机命令成功。

本轮执行结果：

- Editor／Game Win64 Development常规构建退出0；反射修改后已保存并关闭编辑器构建，再冷启动。命令沿用Build.bat的`-WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1`；日志在Saved/ResonanceDebug/editor-build.log及game-build.log。
- `Automation RunTests Hodge.Resonance`：4项通过，0失败、0警告；新DebugFill测试覆盖缺容量、部分充能、重复回满、容量150、死亡拒绝和非白名单文本拒绝。其余3项是资源／成本、阶段去重与PlayerState初始化回归。完整报告Saved/ResonanceDebug/Automation/index.json。
- `E:/Python/python.exe Tools/CinematicUltimate/run_batch.py --debug single listen dedicated`：单人、Listen远端客户端、PIE Dedicated各8项通过。实际控制台使服务器／拥有者回满，其他玩家仍0；R键随后正常消耗100并产生CD；CD中再次回满与重复命令保持原CD，HUD满条显示冷却且按钮禁用。
- Python工具语法检查通过，目标差异`git diff --check`通过；没有Cook／打包或独立专服测试。

当前正常获取方式为实际伤害阶段回能：普攻资产初值5，共鸣技能初值20，大招0；同一执行同一HitGroup＋AttackPhase多个目标只回一次，打空不回。可在相应DA的ResonanceEnergyGainPerPhase调整。

使用步骤及资产配置见[共鸣解放配置指南](../Guides/resonance-liberation-configuration.md#开发时直接充满)。本次报告不覆盖此前大招所有美术和镜头测试。
