# 统一 CombatComponent：MCP 与游戏回归（2026-10-01）

> 2026-10-06 状态同步：历史记录：保留当时基线、方案和结果，不代表当前组件/资产或当前测试通过。 当前项目事实见 [本轮更新](../KnowledgeBase/26-update-2026-10-06.md)。

本轮使用用户常规编译并重新打开的 UE 5.5.4 编辑器，通过项目原生 MCP（127.0.0.1:3016/mcp）执行蓝图、原生自动化、单人 PIE 和双玩家 Listen Server 验证。未执行 UBT、独立进程客户端、Cook 或打包。

## 实际结果

- Hodge.Combo：DefinitionGrant、GraphValidation、MontageDuration、SessionRules，4 项全部通过。
- Hodge.Combat：DetectionRouting、HitGeometry、HitProfileValidation、MeleeHitHistory、MeleeSpecContext，5 项全部通过。此前 MeleeSpecContext 的测试配置缺口已在本轮编译版本中复跑通过。
- `/Game/CodexText/CombatHitWindows/Fixture/BP_MeleeTestCombatComponent` 和 `BP_MeleeTestHero` 均通过 MCP compile，结果 compiled=true、compilerStatus=UpToDate、saved=true。
- 实际 Pawn 只有一个复制 CombatComponent；PlayerState 没有 CombatComponent；旧 HodgeComboComponent 类无法加载。模拟代理、拥有者和服务器实例均核实组件数量。
- 单人连招套件：13 个断言通过，覆盖首段、自然结束、输入缓存过期、完整连招、移动取消、新 Pawn 的组件初始化和继续攻击，以及 PlayerState ASC 保持复用。
- 客户端网络连招套件：17 个断言通过，额外核实服务器激活授权、连招收尾、Pawn 组件上的移动取消 RPC、服务器重生和拥有端恢复攻击。
- 当前数据表的实际路线为 01→02→03→05→04；测试遵循该路线，没有把用户配置改成 01→02→03→04→05。
- 服务器 Point 事件使 01→03，自然结束事件使 03→04；拥有端与服务器最终回到 Entry，3 个检查通过。事件边和 Point 仅临时改内存，结束后按原始数据恢复。
- 攻击中在服务器移除 CombatComponent：两端组件消失且攻击标签清零，3 个检查通过。这验证组件移除，不代表已经验证整个 GameFeature 插件卸载/重新激活。
- 本体接触与同窗口重复接触、范围外、窗口后进入、窗口前取消、命中后取消、再次激活、窗口前销毁 Pawn、重生后攻击、武器 Socket 和命名碰撞盒，10 项伤害用例全部通过。接触用例为 100→70；未命中或窗口前取消为 100。
- Listen Server 主机攻击通过：两端目标血量均为 70。客户端首次攻击失败：两端均为 98.5，服务器来源 BaseDamage=0、拥有端=20；与合并前的已知属性初始化问题一致。本轮没有重新应用属性 GE 来改写正式验收结果。
- 主机攻击时模拟代理收到 Status.Attack，结束后清除；观察者状态由 ObserverTags 表达，不要求模拟代理的 CurrentComboTag 跟随复制。

伤害用例共 12 项，11 项通过、1 项失败；上述连招、事件及移除套件各自通过。不能把本轮描述成所有联机验收通过。

## 命令与证据

通过 `python Saved/Tests/UnifiedCombat/mcp_run.py` 连接 MCP，按 describe→execute 调用 `manage_blueprint.compile`、`system_control.run_tests`、`system_control.execute_python`、`system_control.set_project_setting` 和 `control_editor.play/stop`。原生测试命令实际为 `automation RunTests Hodge.Combo` 与 `automation RunTests Hodge.Combat`；开始请求不是完成证据，最终以日志中的 9 条 Test Completed 成功记录为准。

执行 `probe_combo.py`、`run_melee.py`、`run_modes.py`、`run_net.py`、`probe_net_combo.py`、`remove_component.py` 和现有 `Tools/BasicAttack/verify_definition_events.py`。采用真实 EnhancedInput 输入、GA、Timeline、Task、检测组件与既有伤害 GE；生命复位仅用于独立用例准备，不代替伤害攻击。

证据位于 `Saved/Tests/UnifiedCombat/summary.json`、`native_tests.txt`、`combo_runtime.json`、`combo_network.json`、`authority_events.json`、`component_removal_network.json`、`U_*.json` 和 `ListenServer_*_Unified.json`，包含逐帧命中及网络属性记录。Saved 文件作为运行证据，不纳入源码提交。

旧回归脚本有固定窗口时间和连招顺序假设，不能直接用于当前资产。前期准备阶段还遇到 Python 受保护属性、未暴露枚举、相对路径和 GameplayTag Python 对象比较问题；纠正测试脚本后才记录最终结果，这些准备失败不作为游戏实现失败。重生测试脚本已修正为重新获取新 Pawn 的 CombatComponent，Python 语法及 git diff --check 通过。

## 恢复与后续

已停止 PIE，恢复正式 Experience 的 Actions/PawnData、临时数据表和 Timeline 事件、玩家数量、单进程与附加选项、Standalone 模式、后台节流，并返回 `/Game/CodexText/L_MainMenu`。临时 DefaultEditorPerProjectUserSettings.ini 恢复为原先不存在的状态。测试蓝图的 compile/save 是本轮保留的资产操作；未保存正式 Experience 或临时事件配置，未提交或回退已有改动。

下一步应单独追踪后加入玩家服务器 CombatSet.BaseDamage 的初始化与属性 GE 授予/重设时序，修复后重跑客户端首次攻击。尚未覆盖完整 GameFeature 卸载/重激活、武器中途卸下、真实死亡动画、队伍友伤矩阵、延迟/丢包、独立进程与打包。现有 CommonUI GameViewport 配置错误仍出现，本轮没有修改 UI 配置。
