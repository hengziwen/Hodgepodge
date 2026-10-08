# 2026-10-07：dev-AN 通知驱动迁移验证

> 后续客户端验收发现正式连段／移动取消首帧授权竞态，已追加 [修复与客户端回归](client-combo-window-2026-10-07.md)。本报告网络命中测试不能代替正式连段客户端测试。

迁移前基线 b1482a5，用户授权实施并要求旧文件集中归档。没有切换分支、自动提交或升级引擎。迁移前工作区只有设计文档改动；用户随后保存了 DA_Attack_4 与 DA_Attack04_Timeline，并关闭编辑器。本次保留这两个保存版本和 GameplayTags.ini 的编辑器重写结果。

## 最终实现

- 退役自建 Timeline Runtime 类型、时钟推进、命中绑定、专用编辑器及对应工具；正式运行不保留兼容后端。
- Montage 的原生 Branching Point 通知直接提交命中、作用域状态、手持和玩法消息；GA 持有执行身份及资源账本。
- 检测算法、伤害执行、GAS 授予、输入缓存、连段图／记忆与武器表现继续复用。骨骼租用统一由现有 CombatComponent 管理，没有新增常驻角色组件。
- Definition 收敛为身份、执行配置与默认伤害；Profile 不再让作者选择 Strategy。HitGroup 以显式 AttackPhase 分段，不读取旧条目开始时间。
- 单次命中支持 AdditionalHits，在一个原生时刻处理多个来源；点通知与另一分支标记重叠会校验失败，避免引擎静默跳过。持续通知／标签的重叠、末次采样与取消清理由资源句柄处理。
- 原生 Montage 自动混出恢复，默认完成／中断消息改为 GameplayEvent.Attack.Completed／Interrupted，取消不等待 NotifyEnd。

## 资产与归档

正式五段 Definition 和 Montage 已迁移，顺序仍为 01 → 02 → 03 → 05 → 04。第四段改为 CharacterMeshSocket / Bip001LHand / Sphere / 15cm，并显式补入项目现有 GE_MeleeDamage_Instant。其它段沿用默认剑 MainHand 来源与已有伤害参数。连段表的旧 Timeline.End 引用换为 Completed。

7 个多段夹具 Definition 使用独立测试 Montage，避免污染正式动作；查询配置位于 Main/Combat/HitProfiles。新增通知专项夹具与 AdditionalHits 示例位于 `/Game/CodexText/AnimNotifyCombat`，不属于已打磨的正式技能。

旧文件保留在 [C++ 归档](../../Archive/Timeline/Cpp)、[蓝图／资产归档](../../Archive/Timeline/Blueprints)。311 份原始 C++ 快照、79 份原始资产快照均通过 SHA256 校验，52 个已退役资产从 Content 移入归档，归档前外部引用为零。归档约 2.9 MB，不参与 UBT/UHT、资产加载或 Cook。恢复见 [归档说明](../../Archive/Timeline/README.md)。

项目外备份 `E:/Project/Backups/Hodgepodge/AnimNotifyMigration/20261007-131152` 完整；`20261007-131320` 是磁盘满时未完成的重复备份，可清理。本次后续改为只备份涉及文件，没有继续创建全量副本。

## 已执行的玩法验证

- 单人 10 项：单次／持续三段、进入只查一次、固定中心、确认目标、整次共享、显式分段共享、首次命中前后取消与延迟进入。每段 25，目标 150→125→100→75，实际次数符合配置。
- 通知专项 6 项：隐藏 Mesh 的左手命中、同标签重叠、取消与外部标签计数保留、低帧率短区间、显式 Phase 共享、同帧来源；检测会话及骨骼租用最终归零。
- AdditionalHits：同瞬间三来源，同 Phase 的两个来源合并一次，下一 Phase 再结算一次，目标 150→100。
- 真实输入六次：01→02→03→05→04→01，提前输入缓存与末段后摇重开正常；同一 GA 再激活清空命中记录，独立技能打断后连段记忆保留。
- Section 跳过命中、循环再次进入、PlayRate=2，以及移除 CombatComponent 后停止能力／会话／租用。
- Listen Server 2 玩家、100ms 延迟 8 项：主机与客户端多段、取消、固定中心、确认目标和预测拒绝；仅服务器最终提交伤害。确认目标三段额外连续重复 5 次均为 75，结束后能力非活动。

冷加载阶段未发现旧 Timeline 类型或退役资产，46 个相关资产校验通过，6 个 GA 蓝图编译并保存通过。最终扩展夹具与工程命令结果在下方追加；以上结论以对应 Saved/AnimNotifyMigration 原始记录为证据。

## 修复与限制

过渡方案中的 Queued 派发在远端服务器重复姿势更新下出现漏段，最终改用原生 Branching Point。测试时钟同时改为实际 Montage 位置／结束状态，不用世界时间强制提前卸装能力。引擎同精确时刻点标记冲突通过 AdditionalHits 与校验处理，没有恢复一个中央时机调度器。

转换工具曾遇到 Notify UObject 名称冲突，已改为唯一名称；通知触发偏移枚举到秒的转换已修正，并检查实际轨道时间。初次自动化的武器测试夹具缺少统一租用入口，已补真实 CombatComponent 和重叠租用验证。

未制作独立技能 Actor、独立 Server Target、完整打包、逐帧视觉打磨或全部根运动组合。专服 PIE 不代表独立专服可执行文件通过；目标地图 Cook 不代表完整发布包通过。低帧率只保证处理原生事件与实际姿势，不回放丢失的全部骨骼帧。已有插件弃用、CommonUI／Cue 等非本次问题不以删插件或改引擎设置掩盖。

配置请查阅 [攻击能力配置手册](../Guides/attack-ability-configuration.md)。日志位于 `Saved/AnimNotifyMigration`，不纳入源码。

## 最终工程命令与结果

实际 PowerShell 命令如下，均使用本机 UE 5.5.4；日志目录 Saved/AnimNotifyMigration。没有清理用户缓存或修改构建版本。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/AnimNotifyMigration/AutomationBranchingFinal' '-abslog=E:/Project/Git/Hodgepodge/Saved/AnimNotifyMigration/AutomationBranchingFinal.log'
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -run=Cook -TargetPlatform=Windows '-Map=/Game/ThirdPerson/Maps/ThirdPersonMap' -unattended -UTF8Output -nosplash -SkipShaderCompile '-abslog=E:/Project/Git/Hodgepodge/Saved/AnimNotifyMigration/Cook.log'
python Docs/KnowledgeBase/tools/kb.py refresh
python Docs/KnowledgeBase/tools/kb.py check
git diff --check
```

Editor 常规构建（Editor-release-validation.log）退出 0；Game 常规构建（Game-release-validation.log）退出 0。Cook 退出 0，650 个包完成，0 errors / 53 warnings；其中包含现有工程警告及持续状态边界重叠的 Branching Point 警告。这些持续状态已有同帧、标签计数、连段和取消回归，不能把该警告解读为点通知允许重叠。Cook 使用 SkipShaderCompile，只验证该地图依赖的资产处理，不代表完整材质 Shader 或发布包通过。

最终 Branching Point 原始记录：single-branching-final.json（10）、extra-branching-final.json（6）、point-multi-tests.json（1）、combo-branching-final.json（完整六次输入）、reactivation-branching-final.json（4）、sections-branching-final.json（4）、network-branching-final.json（8）、repeat-confirmed-tests.json（连续 5 次确认目标）、dedicated-branching-final.json（2）。专服 PIE 两用例分别为目标 150→125 与 150→75，两端一致；未渲染左手最大位移约 50cm，所有会话／租用归零。

当前配置手册与 README／知识库入口已更新，历史 Timeline 材料加退役说明并指向归档；本地知识库 check 检查 53 个 Markdown，0 链接错误／0 漂移。新增夹具后冷校验范围为 48 个资产。没有提交、推送或打包。

最终原生自动化 AutomationBranchingFinal：18 项全部 Success，命令退出 0。范围包括通知参数与上下文、伤害 Spec／检测路由、连段、属性成长、旋转约束、武器表现及统一骨骼租用。没有把单人测试代替网络测试，也没有把 Cook／Game 构建代替完整打包。
