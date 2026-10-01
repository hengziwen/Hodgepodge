# Experience 判定组件与既有伤害 GE 复跑记录

日期：2026-10-01。项目：E:/Project/Git/Hodgepodge/Hodgepodge.uproject，UE 5.5.4。用户完成常规编译后，通过本机原生 MCP 执行资产迁移、真实 PIE 与双玩家 Listen Server。本轮没有执行 C++ 构建或打包。

## 结果

15 项正式玩法用例中 14 项通过、1 项失败；额外的客户端数值初始化诊断通过。Hodge.Combat 原生自动化共 5 项，4 项通过、MeleeSpecContext 失败。不能宣称完整联机或所有自动化验收通过。

- 本体重复接触：100→70，同窗口只结算一次。
- 范围外、窗口结束后进入：100，不造成伤害。
- 窗口前取消：100；窗口内命中后取消：70，停止后续结算。
- 取消后重新攻击：100→70。
- 目标伤害免疫、经核实碰撞的墙体遮挡：100。
- Pawn 在窗口前销毁：100；GameMode 重生后重新攻击：100→70。
- 武器 Socket 与角色命名 Box 两种来源：各为 100→70。
- 判定组件在窗口前移除：100，没有崩溃或继续结算。此用例验证组件移除，并非完整 GameFeature 插件卸载流程。
- Listen Server 主机攻击：服务器与客户端目标血量均为 70。
- Listen Server 客户端首次攻击失败：服务器来源 BaseDamage=0，拥有者客户端 BaseDamage=20；目标实际 100→98.5，两端血量一致，但伤害不符合预期。

诊断仅在运行时向后加入玩家的服务器 ASC 重新应用 GE_MeleeTestAttributes，确认 BaseDamage 0→20，随后客户端攻击为 100→70，两端一致。没有删除其他初始化效果、没有保存运行时补丁，也没有修复 PlayerState 初始化代码。伤害 GE 仍保留原配置；不能把它简化为任何情况下都只计算 BaseDamage×倍率，也不能仅据配置中的计算修饰推断实际数值。

## 组件与资产

执行 `python Tools/Combat/configure_melee_fixture_experience.py`，创建并配置 BP_MeleeTestCombatComponent。首次 PIE 发现组件蓝图虽有 CDO 参数，生成实例仍为空；编译该组件蓝图并保存默认值后，后续生成实例正确继承两条 HitSources，迁移脚本已加入该步骤。

运行时确认每个测试 Pawn 恰有一个 BP_MeleeTestCombatComponent，来源为 Combat.Source.Body.Origin 和 Combat.Source.Hitbox.Chest；角色 CDO 没有原生默认判定组件。武器仍使用独立装备实例中的来源。

三个 DA_Test 的 DamageEffect 显式引用 /Game/GameplayEffects/Damage/GameplayEffectParent_Damage_Basic。原伤害 GE 未被修改，未新增伤害 GE 类。窗口检测由 Task 调用组件，过滤、去重与效果应用仍属于 GA。

项目只扫描 /Game/Main/Experiences，实际 GameMode 的 WorldSettings Experience 入口还被注释。已新增独立测试体验 /Game/Main/Experiences/CodexText/Exp_MeleeValidation；原先位于 Fixture 目录的 Exp_MeleeTestExperience 不在扫描范围，不能直接作为已识别体验使用。桥接 Standalone play 不传 URL，PlaySettings 的附加服务器选项在该入口也未生效；本轮采用默认 Experience 的 Actions/PawnData 临时内存覆盖，仍经过 ExperienceManager→AddComponents→Receiver 真实添加链路，结束后恢复，未保存正式 Experience。

测试地图为 /Game/ThirdPerson/Maps/ThirdPersonMap。来源分别使用 Fixture 下 DA_MeleeTestPawn_Body、DA_MeleeTestPawn_Weapon、DA_MeleeTestPawn_HitBox；基础伤害 20、倍率 1.5、窗口 0.30～0.42 秒。输入使用真实 PIE 鼠标键或 EnhancedInput 本地玩家注入，GA、Timeline、服务器检测与 GE 不被替换为手工伤害。

## 自动化与源文件

执行 MCP console_command：Automation RunTests Hodge.Combat。DetectionRouting、HitGeometry、HitProfileValidation、MeleeHitHistory 通过。MeleeSpecContext 在 Both hits build an independent spec 断言失败，原因是测试的 Binding.DamageEffect 仍为空，而新实现正确禁止默认替代 GE。

已修改 Source/Hodgepodge/Private/Tests/HodgeMeleeRoutingTests.cpp：先断言缺少显式 GE 时不会构建 Spec，再显式选择 UGameplayEffect 基类验证上下文隔离。该用例不施加伤害，所以不需要新 GE 类或硬编码项目伤害资产。此测试源码修改发生在本轮运行之后，没有重新编译或复跑，状态仍为待验证。

## 测试准备失败与限制

最初按旧推断使用 68.5 作为预期，本体实际为 70；已修正文档并以 70 完成正式复跑。旧脚本创建墙体时没有可靠建立本轮运行时碰撞；重新在完成 Spawn 前配置 Movable、网格与 BlockAll，再核实实际边界和 Visibility 阻挡，复跑通过。首次组件移除脚本漏传 Python destroy_component 的 object 参数，记录属于测试脚本失败；纠正并确认组件数量为 0 后，正式移除用例通过。保留这些初始记录，不计入正式 15 项。

未覆盖完整 GameFeature 卸载/重新激活、实际死亡动画中途清理、武器中途卸下、友伤双方队伍矩阵、完整刀身覆盖、独立进程客户端或打包。既有 CommonUI GameViewport 配置错误仍存在，本轮未修改。

## 证据与恢复

逐帧结果与摘要在 Saved/Tests/MeleePIE/summary_v2.json、V2_*.json、ListenServer_*_V2*.json；原生自动化结果在 Saved/Logs/Hodgepodge.log。运行时数值诊断不计入正式通过数。

结束时停止 PIE，恢复正式 Experience 的 Actions 与 DefaultPawnData、原有后台节流、玩家数量、单进程设置、附加选项及 Standalone 模式。临时 DefaultEditorPerProjectUserSettings.ini 按启动前备份恢复；编辑器回到 /Game/CodexText/L_MainMenu。未自动提交、未回退用户改动。
