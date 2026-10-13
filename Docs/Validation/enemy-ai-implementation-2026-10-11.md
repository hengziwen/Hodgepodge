# 敌人 AI v0.1 实施与验证报告

日期：2026-10-11。引擎：UE 5.5.4。实现前 HEAD：06cd38f。范围为 [设计文档](../Design/enemy-ai-system.md) 的 P0-A／P0-B／P0-C／P1；GameFeature 必需扩展全面改造与 P2 不在本轮范围。配置和接口操作见 [手册](../Guides/enemy-ai-configuration.md)。未自动提交 Git。

## 1. 完成内容与用户指定修正

- PawnExtension 已绑定其他 ASC 时，EnemyInit 返回 Failed 并输出 ASC identity conflict；不抢占、不返回成功。缺少必需 StatProfile 同样终止，等待 Experience／复制／组件与非法配置分开处理。
- EnemyInit 成为唯一怪物初始化协调入口；EnemyCharacter 旧入口只委托，不再存在第二套属性／能力授予和固定重试 Timer。玩家保留 PlayerState 属性协调，敌人默认装备通过通用入口准备。
- AbilitySet 增加共享受检授予结果、批量预校验与精确句柄清理，旧 void 入口保留。装备 AddEntry 实际调用受检 API，空 Set、授予或 Actor 创建失败不提交候选 Entry。不可逆副作用明确报告并停止。
- EnemyCharacter 原生默认使用 HodgeEnemyAIController；正式两份敌人 BP 显式指向 AIC_Guardian。Controller 等待 EnemyInit，按 Possess 周期启动／解绑，未成功 RunBehaviorTree 不标记 Running。
- 最小感知／导航／BT／BB 与 Standalone GAS 攻击闭环完成。Task 先绑定结束委托再激活，处理同步结束、清空 ExecutionId 的正常结束、旧请求回调、取消与 Abort。
- Guardian 徒手和装备两版、独立骨架／网格／AnimBP、攻击／受击／死亡、属性和 PawnData 已配置。Default Experience 增加敌人 EquipmentManager，保留现有玩家和 Combat 条目。

## 2. 代码与测试文件

主要 Runtime 模块：

- [EnemyInit 接口](../../Source/Hodgepodge/Public/Component/HodgeEnemyInitComponent.h)与[实现](../../Source/Hodgepodge/Private/Component/HodgeEnemyInitComponent.cpp)。
- [EnemyCharacter](../../Source/Hodgepodge/Private/Character/HodgeEnemyCharacter.cpp)、[CombatCharacter](../../Source/Hodgepodge/Private/Character/HodgeCombatCharacter.cpp)、[PawnExtension](../../Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp)及 CombatComponent 晚注册推进入口。
- [AbilitySet](../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)、[EquipmentManager](../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp)、[EquipmentInstance](../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp)。
- [AIController](../../Source/Hodgepodge/Private/AI/HodgeEnemyAIController.cpp)、[BTTask](../../Source/Hodgepodge/Private/AI/HodgeBTTask_ExecuteAbility.cpp)。
- [EnemyAnimInstance](../../Source/Hodgepodge/Private/Animation/HodgeEnemyAnimInstance.cpp)、[EnemyDeath GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_EnemyDeath.cpp)。

测试与作者工具：

- [15 项敌人原生测试](../../Source/Hodgepodge/Private/Tests/HodgeEnemyAITests.cpp)；TestTypes／TestSupport 提供具体配置、失败 Actor 与初始化测试 World。既有 HitReaction 的 EnemyPawnDataInitialization 测试迁移到真实组件状态流程。
- [PIE／联机端到端检查](../../Tools/EnemyAI/run_e2e.py)、[蓝图编译检查](../../Tools/EnemyAI/validate_blueprints.py)及同目录的资产／地图／启动／结束／恢复脚本。
- HodgeAbilityEditor 的 HodgeAnimationAuthoringLibrary 配置受限 Guardian 图、骨架与导航；HodgeCombatValidationLibrary 提供随 Tick 提交动作和只读 Brain／Montage 检查。编辑器依赖仅在既有 Editor 模块增加，不引入新 Runtime 模块。

## 3. 实际结果

- HodgepodgeEditor / Win64 / Development：常规 UBT 构建通过，最终有效日志 EditorBuild15.log。
- Hodgepodge / Win64 / Development：常规 Game 构建通过，日志 GameBuild3.log。之后仅改编辑器工具／Python／资产和文档，Runtime 未再变更。
- 原生自动化：57 项通过，0 失败，0 未运行；其中 15 项为新 Enemy 测试，其余 42 项为属性、装备、连段、Combat、HitReaction、旋转、移动与关系映射回归。汇总含 29 项带警告的成功测试，保留日志中的调试／预期失败诊断，不宣称零警告。
- Guardian 蓝图：7 份，均 ERRORS=0、WARNINGS=0，包含动画、两份敌人、控制器、装备及死亡能力。
- 单人 PIE：14/14，通过，error=null，约 10.52 秒检查周期。
- Listen Server，两个玩家、同进程、至少一个远端客户端：18/18，通过，error=null，约 10.78 秒检查周期。
- 最终编辑器状态：PIE 已结束，无未保存内容包／地图；测试临时授予已撤销，后台 CPU／Slate／Idle 节流参数已恢复。

机器可读摘要见 [验证结果](enemy-ai-implementation-results-2026-10-11.json)。完整日志和原生 HTML／JSON 报告保留在项目 Saved/EnemyAIImplementation，不纳入源码。

## 4. 覆盖的正常、边界与异常场景

原生 Enemy 组：

1. 幂等初始化、ASC Owner／Avatar、重复授予计数、已损生命不被恢复。
2. 外部 ASC 冲突、缺 StatProfile、非法属性等级均不 Ready，明确失败。
3. Combat／Equipment 晚到事件推进，徒手和装备准备，不重做已成功属性步骤。
4. ASC 解绑、授予回调中改绑定、组件移除的生命周期与句柄清理。
5. 空 AbilitySet、OnSpawn 初始化限制、授予预校验。
6. Instant GE 已执行后失败，报告不可逆，清理句柄但不伪装为属性回滚。
7. 装备空 Set、生成失败、Actor 构造中销毁，均无未完成 Entry／授予／Actor 残留。
8. Possess 周期、未配置树的失败、重复就绪、控制与死亡、旧 Task 回调失效。
9. Definition 清空 ExecutionId 后的合法结束回调，不误丢弃而等待超时。

实际 PIE：

- 预放置两种怪物，经真实 Experience 和组件注入完成就绪；装备版持有一个实例及一个表现 Actor。
- 重复初始化不重授予／不重开 Brain；真实感知发现 Hero，真实 NavMesh 追击距离从 850 减至约 649，随后进入攻击。
- 不同蒙太奇实例连续执行两次，BTTask 正常结束并允许下一次攻击；实际生命下降证明动画通知／GE 命中链路工作。未在行为树直接扣血。
- 玩家通过独立测试攻击 Definition 命中怪物，怪物生命从 350 减至约 340.70，真实 HitReaction 状态出现。受控时 CanExecuteAIAction 为假，解除后 Brain 仍 Running 且可行动。
- 已受伤时重复初始化不恢复生命；GameplayEvent.Death 触发既有死亡能力，停止正常 AI，DeathMontage 正常结束并销毁敌人。
- Listen Server 额外验证远端 EnemyInit／AnimInstance 就绪、攻击蒙太奇权重、复制受击阶段及死亡蒙太奇权重。运行决策和伤害仍在服务器。

这些是功能场景覆盖，不是行／分支覆盖率百分比；本轮未接入覆盖率插桩，不能宣称 100% 代码覆盖。

## 5. 实际运行命令

在关闭编辑器后执行常规构建（未使用 Live Coding）：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Enemy+Hodge.Attributes+Hodge.WeaponPresentation+Hodge.Combo+Hodge.Combat+Hodge.HitReaction+Hodge.Rotation+Hodge.Facing+Hodge.Movement+Hodge.Relationship' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/EnemyAIImplementation/Native4' '-abslog=E:/Project/Git/Hodgepodge/Saved/EnemyAIImplementation/Native4.log'
```

编辑器启动后，通过既有本地鉴权 gateway 执行以下脚本；输出成功只表示脚本执行，端到端必须等待最终报告：

```powershell
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/configure_guardian.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/configure_validation_grant.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/configure_validation_map.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/start_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/run_e2e.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/stop_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/start_network_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/run_e2e.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/stop_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/restore_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/validate_blueprints.py
git diff --check
```

## 6. 验证中发现并修复的问题

- 原 Enemy 测试夹具未完整注册 Init State／Controller，导致初始化卡住；既有失败路径随后直接解引用空 HealthSet。改为实际生命周期测试 World，并在断言失败后安全退出。修复后原生回归通过。
- Definition 在 ASC.OnAbilityEnded 前清空 ExecutionId；原 Task 身份比较会忽略合法结束。现在以 Spec／实例／Attempt／Possess 联合校验，只有有效新 ExecutionId 才表示另一轮。
- UE 5.5 MoveTo 的 AcceptableRadius／布尔配置为 ValueOrBBKey 结构，作者工具旧反射类型造成崩溃。改用实际结构类型赋值，常规重编译后生成成功。
- 创建行为树图时 AddSubNode 同步 UpdateAsset 会破坏未完成的运行树。添加图更新锁，完成后统一重建；重跑真实追击和重复攻击通过。
- 导航体积无普通碰撞，原 bounds 成功检查忽略非碰撞组件。改为包含非碰撞组件，实际 NavMesh 路径和追击通过。
- Python 的源资产结构访问可引用原字段；测试包配置曾使 Hero Definition 在内存中变脏。未保存该原资产，从磁盘重新加载恢复，改用 export／import 独立复制；最终 Git 无 Hero 资产改动。
- Python 端受击 API 名称修正为 GetReactionState；失败报告保留，最终 14／18 项报告均无异常。

## 7. 实现假设与未验证范围

- 必需 StatProfile、固定自身 PawnData、基础／装备能力满足首版需要；未依赖 GameFeature 必需攻击技能。因此没有在本轮全面重构 AddAbilities Actor 级记录或软资源授予确认。
- 首版初始化包禁止 OnSpawn。Instant GE 或任意用户回调的外部副作用不是通用可回滚事务；失败停止并报告，不整包恢复重试。
- 目标仅有效玩家 Hero，经 DamageRules 过滤；没有完整队伍／仇恨／EQS。
- 真正 Dedicated Server、独立进程网络、延迟／丢包压力、Cook／打包、对象池、热切换和大规模敌人未执行。Listen Server 通过不替代这些测试。
- 第 14.3 节的不可达导航、目标销毁、死亡与受击同帧等组合压力仍待扩展；本轮未宣称所有组合穷尽验证。
- 本轮以状态、伤害、位置和蒙太奇权重验证闭环；动画品质、武器握持位置及轻反馈视觉仍需团队在测试地图逐帧调校。
- 旧 Hero/GA_Attack 的 PlayTimeline 遗留问题未处理，不作为本轮敌人能力资源，也未声称整个项目所有蓝图都通过。
