# R1 命中检测独立与 R3 角色收益隔离

日期：2026-10-16（Asia/Shanghai）。UE 5.5.4。开发基线包含上轮尚未提交的 R9／R5 修复；本轮不提交、不覆盖已有改动。原有地图外部 Actor 删除项保留。

## 范围与职责

R1：新增 `UHodgeHitDetectionComponent`，唯一保存检测会话、来源解析、几何历史、采样序号和姿势占用。原 `HodgeHitDetection` 几何策略继续复用。组件没有独立 Tick 或复制，不构成第二条采样链。原 `WaitHitResults` Task 继续拥有 GA 窗口、暂停／恢复、最终采样和 PostPhysics 节拍，直接调用独立检测服务；骨骼、武器／翼来源的更新依赖及权威翼姿势准备由检测服务处理。

Combat 保留请求、连段、输入、网络协调，以及旧检测 API 的薄代理。旧 `HitSources` 资产字段保留读取兼容，在建立会话前传给检测服务；会话快照和运行状态只在服务中持有。服务由 Combat 按 Actor 查找或创建一份，Shutdown／EndPlay／OnUnregister 配对清理；最后一个姿势租用退出恢复原骨骼策略，旧句柄不能释放新占用。

R3：公共 Melee 保留目标资格、伤害提交、SettlementId 关联的实际生命扣除观察和受击事务，发布 `FHodgeAttackSettlement` 值载荷。载荷包含执行、Spec、结算、HitGroup／AttackPhase、目标 ASC 弱引用和 AcceptedDamage，不保留临时 Spec 指针。公共 Combat／Melee 不再 include Havoc／Resonance 类型或处理其收益。

Definition 引用可选、内嵌的 `AttackRules` 配置；每个服务器执行创建独立规则对象，并在结束时释放。共鸣规则拥有自己的阶段账本；暗流规则拥有阶段与施放账本，调用现有 HavocRules 执行角色资源收益，角色级拢音限流、模式会话及消耗继续属于原 HavocRules。普通 NPC 没有配置或资源时不会创建玩家属性集。

同步分发强持有当前能力与规则快照，每次处理重新核对执行／Spec／ASC／Avatar。GE 回调撤销能力、触发 GC 或切换执行后停止旧规则；已经发生的数值变化不假称回滚。共鸣先于暗流的既有收益顺序保留。Combat 只观察通用 `State.Combat.Mode` 子标签，排除父级重复通知，不再依赖暗涌具体类型或标签。

## 配置迁移与兼容边界

盘点 `/Game` 下 34 个 Definition，13 个有专属收益配置：普通五段、强化五段、灭音、破命、常态共鸣技能。仅这 13 个 Main 资产被保存；迁移前二进制备份及字段快照在 `Saved/R1R3/AssetBefore`、`assets-before.json`。

- `HodgeResonanceAttackRuleConfig`：`GainPerPhase`／`GainEffect`。
- `HodgeHavocAttackRuleConfig`：`DarkFlowGainPerPhase`／`DarkFlowGainOnCast`／`bEnhancedNormalAttack`。
- 每个字段逐一对照原值，含 ScalableFloat 和 GE 引用；旧收益值迁移后归零。Montage、伤害、通知时刻、GA、技能 Tag 和授予配置不在迁移脚本的修改范围。
- `HodgeLegacyAttackRuleAdapter` 仅为旧资产／原生夹具把旧字段转换成同一规则实现；显式新配置优先，同类新旧路径不会同时执行。适配层的删除条件是归档、外部资产与所有旧调用者均不再依赖旧字段，不能只因本次 13 个资产已迁移就删除反射字段。
- 作者和校验工具已改用 `Tools/AttackRules/rule_config.py`，不继续写回旧 Definition 收益字段。迁移 helper 不保存资产；保存范围由调用者明确控制。

配置制作见 [攻击手册](../Guides/attack-ability-configuration.md)。本轮没有引入新 UBT 模块、消息总线、通用资源平台，也没有改写现有 GAS／Experience／Init State 链。

## Lyra 核查与项目差异

本机参考根目录：`E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame`。

- `Weapons/LyraGameplayAbility_RangedWeapon.cpp::StartRangedWeaponTargeting/OnTargetDataReadyCallback/WeaponTrace`：沿用检测与效果提交分离、目标数据身份和服务器提交边界；Lyra 这条实现是射击 TargetData，不是本项目的 Montage 近战采样，不能直接替换现有 Task／几何策略。
- `AbilitySystem/Attributes/LyraHealthSet.cpp::PostGameplayEffectExecute`：核查伤害处理及通知的职责边界。Lyra 的伤害消息在生命夹取前广播，不能替代本项目 SettlementId 关联的实际生命扣除，因此继续复用 HodgeHealthSet 的 OnDamageAccepted。
- `Character/LyraPawnExtensionComponent.cpp`、`AbilitySystem/LyraAbilitySystemComponent.cpp`：保留 Owner／Avatar 与执行解绑边界。本机 Lyra 未提供本项目所需的近战通知会话、翼姿势占用或 Havoc／Resonance 收益实现；本轮只扩展这些缺少的职责，不移植无关系统。

## 已执行验证

- 修改前原生基线：143 项，78 Success、65 带警告、0 失败。
- 修改后完整 `Hodge.*`：149 项，84 Success、65 带警告、0 失败。新增 6 项覆盖检测注销／姿势恢复、阶段去重与身份、新旧配置优先级、同步重入、施放一次性提交，以及收益回调撤销能力并触发 GC 后阻止后续旧规则。
- Editor／Game 常规 Development Win64 构建均退出 0。先确认编辑器无 PIE／脏包后正常关闭，构建后冷启动；未使用 Live Coding 作为验证。
- 13 个资产迁移逐字段断言通过，30 项既有 Havoc 资产校验通过；结束迁移时无脏内容包／地图。
- 单人暗涌核心 PIE：17 项通过，覆盖施放回能、真实输入长按、正常／强化动作、共享 E 冷却、燃料及驱散清理。
- 单人／Listen 拥有者客户端／PIE Dedicated 命中收益各 6 项，合计 18 项全部通过：普通命中回 5 暗流、多目标共享阶段收益与拢音、强化伤害 20% 增幅、换段不足一秒不重复拢音、收益只属于施法者 ASC。Dedicated 用例实际使用未渲染服务器骨骼与翼来源，不能扩大为独立专服构建或全部骨骼表现通过。
- 12 个受影响正式蓝图执行编译，生成类有效，日志无蓝图编译错误；未保存蓝图。编辑器最终无 PIE、无脏内容包／地图，原 ThirdPersonMap 及本轮记录的 PIE／性能设置已恢复。

完整原始报告在 `Saved/R1R3` 和 `Saved/Havoc`；[可追溯摘要 JSON](combat-detection-role-rules-2026-10-16-results.json) 记录原生、资产、PIE、蓝图及最终状态。已执行 `git diff --check`、知识库链接／漂移检查和公共层专属依赖扫描；没有自动提交。

## 实际命令

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/R1R3/Final' '-abslog=E:/Project/Git/Hodgepodge/Saved/R1R3/final.log'
$env:NO_PROXY='127.0.0.1,localhost,::1'; $env:no_proxy=$env:NO_PROXY
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/R1R3/migrate_assets.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Havoc/validate_assets.py
$env:HODGE_HAVOC_SUITE='core'
& E:/Python/python.exe Tools/Havoc/run_batch.py single
$env:HODGE_HAVOC_SUITE='hits'
& E:/Python/python.exe Tools/Havoc/run_batch.py single listen dedicated
```

最终代码构建日志：`Saved/R1R3/editor-build-4.log`、`game-build-2.log`；原生基线与结果：`Baseline/index.json`、`Final/index.json`。资产操作沿用已发现 schema 的本地 MCP，没有启用新插件或 Remote Execution。

## 保留问题与未验证范围

用户报告的“Client 模式下、释放共鸣回路前的普攻连段窗口疑似异常”已记录到 [当前待办](../KnowledgeBase/29-current-work-and-backlog.md)。用户要求暂缓，本轮不修、不推定根因，也不以其他网络回归通过宣称该问题消失。

未执行独立 Dedicated Server 可执行文件构建、Cook／打包、性能剖析、所有角色与技能的逐项回归。无依据宣称帧率提升；新规则对象每执行创建并持有少量配置／账本，其分配成本仍需性能测量。R2、R4、R6、R7、R8 没有在本轮实施。
