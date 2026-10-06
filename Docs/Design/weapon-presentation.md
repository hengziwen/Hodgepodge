# 武器显现、回背与消隐

> 2026-10-06 状态同步：方案已实施；BackSocket 默认 WeaponOnBack，BackTransform 为插槽内偏移，当前 Profile 为 Identity。检测来源与可见 Mesh 分离；验证范围与未测项保留原记录。 当前项目事实见 [本轮更新](../KnowledgeBase/26-update-2026-10-06.md)。

## 已核实的项目基线

最初实施前，默认装备 BP_Equipment_Sword 使用 BP_WeaponInstance_Sword，生成 BP_Weapon_Sword，挂接角色 Mesh 的 WeaponOnHand，初始偏移为单位变换。原武器 Actor 可复制、未启用移动复制，继承 BP_Weapon_Base；可见组件为 SkeletalMesh，模型 /Game/Wuwa/Weapon/Sword_Qiuyuan，材质 MI_R5Sword506Md20001Effect。当时没有专门的背部 Socket；2026-10-05 用户已在当前角色骨骼添加 WeaponOnBack，本次据此改为插槽停靠。

装备由服务器管理，EquipmentInstance 为 Pawn 所有的复制 UObject，Actor 列表通过 SpawnedActors 复制。普攻定义来自 PawnData 的 AS_LightCombo，不能假定 GA SourceObject 一定是 WeaponInstance。命中检测按 WeaponActorIndex 与 ComponentName 定位武器组件，再采样其 Socket。

本方案扩展 WeaponInstance，不增加 Pawn 常驻组件、不增加 Runtime 模块，不改变固定移动动画层和 FullBody Slot。

## 职责与配置

- EquipmentManager 保留装备、技能授予和卸装职责。
- UHodgeWeaponInstance 持有手持请求、状态、定时器及权威复制状态，处理装备/卸装、死亡、预测拒绝和迟到 Actor 引用。
- AHodgeWeaponPresentationActor 执行显隐、回背曲线、悬浮和材质渐变；Actor 根保持逻辑手部挂接，不与附件复制竞争。
- UHodgeWeaponPresentationProfile 保存 SkeletalMesh、覆盖材质、HandSocket/BackSocket、插槽内偏移、曲线与时间参数。BackSocket 默认 WeaponOnBack；背部停靠跟随角色 Mesh 上的插槽，不修改原骨架 Socket。
- Definition GA 用独立 WeaponUseWindowTag 指定 Timeline 手持窗口。配置留空时维持已有非武器技能行为。

首版时间默认：显现 0.08 秒、收回宽限 0.06 秒、回背 0.3 秒、悬浮驻留 2 秒、消隐 0.35 秒。均为项目初始调参值，不宣称是鸣潮原值。悬浮可配置幅度、频率；材质参数 WeaponVisibility 表示 0 隐藏、1 完全显现。

新表现资产放 /Game/Main/Weapon/Presentation。正式装备定义引用新表现 Actor，原武器 Actor、实例蓝图和原始模型/材质保留。装备实例配置表现 Profile，五个普攻定义配置武器使用窗口，现有 Timeline 追加窗口；其他事件、连段顺序、攻击动画保持原状。

## 请求、状态与计时

Hidden → Hand → Returning → Hovering → Fading → Hidden。Returning/Hovering/Fading 收到新手持请求可立即回 Hand，不等待旧过程结束。

每次请求生成 FGuid 句柄，同时记录 ExecutionId、窗口序号、激活 PredictionKey。同一执行/窗口重复申请返回原句柄；释放只作用于对应请求。所有请求结束后开始收回宽限，之后依次定时切换回背、悬浮、消隐和隐藏。UObject 不 Tick，Actor 仅在显现、回背、悬浮或消隐阶段 Tick。

切段时旧 GA 释放、新 GA 随后申请；宽限会被新请求撤销，不造成每段回背。计时回调必须检查状态版本与阶段；死亡、卸装、Pawn 解绑清空请求与全部定时器。状态有明确装备有效标志，旧执行不能重新激活卸下的武器。

手持窗口以蒙太奇实例时间为准，起点先于实际命中，终点放在角色释放武器的姿势处。自然结束、中断、移动取消和激活失败统一经过 Definition 的清理入口释放请求。命中窗口若与该功能共同配置，必须完整被手持区间覆盖；同帧先进入手持、再采样命中，退出时先关闭命中会话、再释放手持。

首版手持区间均从 0 秒开始，1/2/3/4/5 段终点分别为 0.95/1.15/1.05/1.55/1.18 秒，即 Recovery 开始后保留约 0.35 秒手部姿势。不把 Status.Attack 或整个 GA 生命周期当作武器必须握持的唯一依据。连段记忆时长与悬浮驻留时长独立。

## 命中与外观分离

表现 Actor 有两个 SkeletalMeshComponent：名为 SkeletalMesh 的隐藏逻辑 Mesh 固定在手部，用于保留原组件名和 Socket 命中来源；WeaponVisualMesh 只显示模型，可以移到背后、浮动和消隐。两者不靠物理碰撞造成伤害，仍由服务器 HitWindow/检测策略决定命中。

逻辑 Mesh 的骨骼在有效使用期间正常更新，不能因不渲染而失效。服务器手持请求存在时，暂时保证角色 Mesh AlwaysTickPoseAndRefreshBones；最后请求、死亡或卸装后恢复原有策略，避免视野外的手部 Socket 陈旧。外观变换与可见性不会改变逻辑来源的 Transform。专用服务器可跳过外观 Tick/材质，仍保留逻辑来源。投掷或飞剑不复用闲置回背轨迹作为伤害轨迹，后续应使用独立战斗来源。

Actor 从当前显示姿势开始回背，以角色根空间保存复制起点，每帧将 BackTransform × BackSocket 世界变换转换到同一空间作为动态终点；旋转用 Quaternion 插值，曲线支持弧线偏移。到达背后时仅 WeaponVisualMesh 挂接角色 Mesh 的 BackSocket，悬浮与消隐继续跟随骨骼姿势；Actor 根及检测 Mesh 留在手部，再次攻击时可见 Mesh 挂回检测 Mesh。悬浮仍沿角色根的局部 Z，避免插槽旋转改变浮动方向。插槽不存在时回退到手部姿势并记录绑定警告，不落到角色根或世界原点。

2026-10-05 背部插槽调整：BackTransform 字段保留以免破坏已有属性引用，其语义改为相对 BackSocket 的偏移，默认单位变换。当前 DA_SwordPresentation 配置 BackSocket=WeaponOnBack、BackTransform=Identity，旧角色根位置/朝向不叠加。武器背部姿势先在 Skeleton/Mesh 的 WeaponOnBack 插槽调整，单把武器的微调用 BackTransform。

## 复制与预测

武器实例复制阶段、服务器起始时间、起始角色相对 Transform、起始可见度、版本号和关联激活 Key。观察者通过 OnRep 与 SpawnedActors 的引用到达通知绑定并显示，迟到客户端可按时间重建当前阶段，不依赖本地执行 GA。

拥有者的手持请求允许本地预测，不提供任意改武器状态的客户端 RPC。服务器运行同一 GA 窗口产生权威状态。拥有者有更新请求时不接受上一段迟到的闲置状态；服务器拒绝激活后按被拒绝 Key 撤销预测并回到权威表现，不能清除后来技能的请求。

Actor 在 BeginPlay 默认隐藏，迟到 Owner/实例引用支持短期重试，SpawnedActors RepNotify 会再绑定。装备/实例/Actor 到达顺序均不能造成永久丢失显示；重复 OnEquipped 不叠加监听或请求。状态是恢复依据，材质曲线逐帧在客户端求值，不逐帧复制显示位置。

## 材质与原资产保护

原材质不修改。生成 CodexText 内的可消隐材质副本，保留原武器着色与纹理，增加 WeaponVisibility 驱动遮罩。所有材质槽均创建动态实例并应用该参数；完全消隐后关闭外观组件显示和 Tick，逻辑装备不卸下。

## 验证与交付

先备份 Source、Main/Weapon、Main/Data/Equipments、BasicAttack 和 DefinitionCombo。设计文档先于源码实现写入，备份路径见 Saved/LyraAnimationWork/WeaponPresentation-20261005/backup-path.txt。

Editor 关闭后按项目约定常规构建 Editor 和 Game；原生测试覆盖请求计数、重复释放、过期回调、取消、卸装、死亡与状态恢复。蓝图/材质编译单独记录。

正式地图验证初始隐藏、攻击显现、连续连段不回背、每段停止后回背/悬浮/消隐、回背及消隐中再次攻击、GAS 中断、移动取消、倍速；检查逻辑 Mesh 保持手部位置。两玩家 Listen Server 验证拥有者预测、模拟代理、迟到引用以及延迟下的回收；不将其等同于打包或独立服务器验证。

## 实施结果与实际验证

备份：E:/Project/Backups/Hodgepodge/WeaponPresentation/20261005-210842。未自动提交。

已实现本方案的实例状态、请求句柄、生命周期清理、复制及预测拒绝恢复。装备 Actor 引用使用 RepNotify；实例迟到映射时补初始化。原 BP_Weapon_Base 与 BP_Weapon_Sword 的磁盘内容与备份一致，原始 Wuwa 模型和材质未修改。

新增资产为 /Game/Main/Weapon/Presentation 下 M_SwordPresentation、MI_SwordPresentation、DA_SwordPresentation、BP_Weapon_SwordPresentation。材质复制原 M_Weapon_Restored 的着色和 MI 的参数，以 WeaponVisibility 和空间噪声产生遮罩消隐。原武器模型长轴为 X，实机检查后将背部姿势设为相对 Pawn 根 (-30,15,35) cm、Pitch=-90/Yaw=15/Roll=0、Scale=1，使剑从上背向下悬浮。

正式装备定义引用新 Actor，BP_WeaponInstance_Sword 配置上述 Profile。五个普攻定义和原 Timeline 接入 Status.Weapon.Hand 区间；既有事件保持不变。角色未增加常驻组件，也未添加额外 Runtime 模块。

执行常规构建（Win64 Development），最后两目标退出码均为 0：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
```

默认 UBA 构建被内存调度持续延迟；仅停止本任务的构建进程，以命令行选择单进程普通执行器继续，未修改引擎/项目配置。第一次编译有 TObjectPtr 的 auto* 推导及测试调用 protected 函数错误，已修正并重建。

原生自动化命令 `UnrealEditor-Cmd.exe Hodgepodge.uproject -unattended -NullRHI -ExecCmds="Automation RunTests Hodge.WeaponPresentation" -TestExit="Automation Test Queue Empty"`：RequestsAndLifecycle、GeometrySeparation 均 Success，0 error/0 warning。覆盖请求去重、独立句柄、连续攻击、旧回调、死亡/卸装、服务器骨骼策略恢复、显示 Mesh 与检测 Mesh 分离、隐藏后关闭 Tick 且不销毁 Actor。

新 Actor 与修改的实例/装备蓝图在资产脚本中编译，材质副本重编译；该次操作无引擎编译错误。Profile、五个 Definition、五个 Timeline 通过 EditorValidatorSubsystem 的 IsObjectValid，11 个均 VALID，0 error/0 warning。

正式地图单人最终版本：

- 461 条记录验证初始隐藏、攻击显现、回背悬浮、GAS CancelAbility 清理、消隐中重入及旧消失计时器保护、最终完全隐藏且原 Actor 存活。
- 连续 1→2→3→5→4→1 的 271 条记录全程保持 Hand，切段仅有一个有效手持请求，不回背、不重复显现。
- 255 条倍速/移动取消记录：2 倍速度时在约 0.50 秒、蒙太奇位置 0.950 秒释放手持窗口；移动取消后请求为 0，连段记忆保留第一段，之后进入回背悬浮。
- 使用 computer-use 查看实际画面并调整背部姿势；未将日志正确等同于最终艺术品质。收刀轨迹、驻留、材质和挂点偏移仍可在 Profile 与 Timeline 调整。

两玩家 Listen Server + 100ms 延迟采集 1428 条记录：两个服务器 Pawn、拥有者和模拟代理都完成 Hidden→Hand→Returning→Hovering→Fading→Hidden。延迟恢复 0。服务器拒绝预测续段的 33 条记录验证拥有者没有残留手持请求，之后成功续第二段且新请求为 1。

制作阶段的材质引脚连接错误发生在未保存副本；尝试删除时因仍有活引用导致 UE ForceDelete ensure，原材质未动。确认仅有本任务未保存包后正常关闭并重开编辑器，重新创建、编译和保存副本。测试脚本的枚举格式、保护属性读法及世界短名称混淆均已修正，最终测试文件记录成功结果，不计失败的脚本初始化为功能通过。

最终 PIE 已停止，保留正式地图与单人 Standalone；内容/地图未保存包为 0，后台节流恢复为 true。原始构建、资产与测试记录位于 Saved/LyraAnimationWork/WeaponPresentation-20261005，不纳入源码。

未执行独立进程、Dedicated Server、Cook/打包、完整真实伤害回归、重生及网络相关性重新进入专项测试。当前正式五段仍未配置 HitWindows，后续接入伤害时需验证武器手持窗口覆盖所有实际命中区间。

## 使用与调参

1. 在正式地图攻击：默认无武器显示，起手显现；停止继续攻击后，武器回背悬浮 2 秒再消隐。连续连段不会每段回背。
2. /Game/Main/Weapon/Presentation/DA_SwordPresentation：HandSocket、HandOffset、BackSocket（默认 WeaponOnBack）、BackTransform（插槽内偏移）、ReturnArcOffset、ReturnCurve、DrawSeconds、ReturnGraceSeconds、ReturnSeconds、HoverSeconds、FadeSeconds、HoverAmplitude、HoverFrequency 与 VisibilityParameter 已存在并可编辑。调整角色背部基础位置/朝向时编辑 WeaponOnBack；BackTransform 默认 Location/Rotation=0、Scale=1。
3. /Game/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack01_Timeline 至 05：EventID=WeaponHand，WindowTag=Status.Weapon.Hand，调整 EndTime 控制角色何时释放武器。此时间跟随蒙太奇速度，不用另改延迟节点。
4. 其他 Definition 技能需要武器时，配置 WeaponUseWindowTag 并添加对应 Timeline 区间；统一清理会处理技能取消。非 Definition 技能可申请 AcquireHandUse 并在 EndAbility 释放相应句柄，不能用全局清空抢走其他技能请求。

## 2026-10-06 WeaponOnBack 验证记录

新增 BackSocket 默认 WeaponOnBack，BackTransform 默认单位变换，改为相对插槽的偏移。BackSocket 留空会被 Profile 校验拒绝；运行时缺失插槽会在绑定时警告并使用手部姿势。回背终点每帧读取实际角色 Mesh 的插槽世界变换；悬浮/消隐阶段仅可见 Mesh 挂接插槽，命中逻辑 Mesh 和 Actor 根保持 WeaponOnHand。可见 Actor 的 Tick 在角色 Mesh 和检测 Mesh 更新后求值。

修改前备份：E:/Project/Backups/Hodgepodge/WeaponBackSocket/20261005-235803。包含合并后的当前 Source、本文、表现资产和用户已添加插槽的角色 Model 目录；本次不修改角色骨骼插槽。

实际常规构建命令（PowerShell），最终 Editor 与 Game 均退出 0：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.WeaponPresentation' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/WeaponBackSocket/Automation-final' '-abslog=E:/Project/Git/Hodgepodge/Saved/WeaponBackSocket/Automation-final.log'
```

原生测试 RequestsAndLifecycle、GeometrySeparation 最终均为 Success，0 error/0 warning。几何测试覆盖真实角色 WeaponOnBack 插槽、非零插槽内偏移、移动/旋转角色 Mesh 后的跟随、Fading 保持挂接、Hand 重新挂回检测 Mesh，以及缺失插槽回退。初次测试中三项断言失败是测试 fixture 没有将 Actor 加入装备列表、直接改变 Phase 后漏调用 RefreshPresentation，已补显式刷新并重建、重跑；不把该次失败计为通过。

执行 `python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/WeaponBackSocket/configure_and_validate.py back-socket-configure-receipt.json`，退出 0：DA_SwordPresentation 已保存 BackSocket=WeaponOnBack、BackTransform=Identity，实际角色 Mesh 存在该插槽；表现 Actor 蓝图重编译，Profile 和 Actor 蓝图资产校验均 VALID、0 error/0 warning。表现资产目录与备份比较仅 DA_SwordPresentation.uasset 发生内容变化，角色 Model/骨骼目录 hash 未改变。

正式地图单人 PIE 453 条记录通过：完成 Hidden→Hand→Returning→Hovering→Fading→Hidden，背部阶段实际挂接角色 Mesh 的 WeaponOnBack，移动/转向时跟随插槽（浮动偏移最大约 2 cm），再次攻击挂回检测 Mesh；Actor 根始终 WeaponOnHand。双人 Listen Server + 100ms 延迟 1420 条记录通过：Authority、Autonomous Proxy、Simulated Proxy 的四个角色副本均完成全部阶段，在悬浮/消隐中挂接 WeaponOnBack 且根挂点不变。验证脚本为 Saved/WeaponBackSocket/verify_runtime.py、verify_network.py，通过同一桥接命令执行；初始化阶段的 Python 根组件方法名与客户端世界就绪时间问题已修正/等待后重试，不计为玩法验证通过。

PIE 已停止，网络延迟恢复 0、后台节流恢复原值，正式地图保留为单人 Standalone。未执行实际伤害、预测拒绝、死亡/重生或相关性重新进入的专项 PIE 回归；既有请求生命周期由本次原生测试覆盖。

本次证据位于 Saved/WeaponBackSocket，不纳入源码。未执行 Cook、打包、独立进程或 Dedicated Server 验证。
