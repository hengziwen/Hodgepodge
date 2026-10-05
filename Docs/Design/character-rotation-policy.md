# 角色旋转约束设计与实施记录

日期：2026-10-05。适用 UE 5.5.4 与单一 Hodgepodge Runtime 模块。本文件先记录设计，再随实际实现和验证更新状态；规划不代表已经完成。

## 问题与基线

正式 BP_Hero_Pover 使用 `/Game/Main/Character/Hero/Anim/ABP_Pover_Base`，UseControllerRotationYaw=True，OrientRotationToMovement=False，UseControllerDesiredRotation=False。HeroComponent 的视角输入继续更新 Controller Rotation，Actor 经 FaceRotation 跟随它。FullBody 蒙太奇只覆盖姿势，不会禁止这个物理旋转。

Input_Move 先记录原始移动意图，再因 Status.Attack 停止 AddMovementInput；这个次序必须保留，否则移动取消窗口失效。CharacterMovement 现有 Gameplay.MovementStopped 只约束最大速度与移动旋转增量，不完全覆盖控制器与根运动旋转。

现有 Timeline 的 WindowTag 已按窗口添加/移除 Loose Tag，并在取消、打断和任务结束时配对清理；Definition 的 Timeline 使用蒙太奇实例位置作时钟。本轮复用这些机制，不增加专用旋转窗口类型或重写技能调度。

当前 DA_Attack_1 至 DA_Attack_5 的 Timeline 没有 HitCheck 条目，只有 Recovery、MoveCancel、NextAttack。后摇开始位置依次为 0.60、0.80、0.70、1.20、0.83 秒。这些是实际资产数据，不视为完整战斗判定已经实现。

## 第一阶段目标

新增 Status.Rotation.Locked。窗口开始前允许调整朝向；窗口中保持角色胶囊世界 Yaw，镜头可以继续移动；窗口结束或技能终止后平滑恢复当前朝向驱动。保留蒙太奇内骨骼动作和根运动位移，窗口内约束根运动施加到胶囊的 Yaw。

当前五段先配置 RotationLock=[0.15, Recovery.Start)，分别结束于 0.60、0.80、0.70、1.20、0.83 秒。不是固定墙钟延时，随蒙太奇位置、暂停和播放速度变化。未来加入命中窗口后须保证锁开始不晚于首次命中、锁结束不早于最终命中；不在未知判定配置上宣称满足覆盖。

## 职责与接入位置

- UHodgeCharacterRotationComponent：Pawn 上的约束状态、锁定 Yaw、恢复状态、ASC 绑定与少量复制状态。挂在 AHodgeCombatCharacter 的默认子对象中，玩家与敌人共用。组件不 Tick，不承担位移、输入、能力调度或伤害判定。
- AHodgeCombatCharacter：在现有 ASC 初始化/反初始化回调中绑定/解绑组件；FaceRotation 作为控制器旋转的薄执行适配。CharacterBase 不引入 GAS 依赖。
- UHodgeCharacterMovementComponent：约束移动/AI旋转增量，在 MoveUpdatedComponentImpl 中约束 Root Motion 的胶囊 Yaw。模拟代理、物理模拟、Teleport 和网络位置校正遵循引擎/服务器结果，不被本地锁拦截。
- UHodgeAnimInstance：游戏线程读取解析后的锁与恢复状态、FullBody 权重，向动画蓝图提供只读表现数据。动画图抑制原地转身及程序性 RootYawOffset 修正，实际武器/身体动作保留。动画线程不访问 ASC 或可变组件。
- Timeline/GA：只提出 Tag 状态。Camera/HeroComponent 继续提供视角输入；本轮不在每个 GA 中保存并恢复旋转布尔值。

保留原来的朝向驱动配置。第一阶段聚焦约束，未来 ControllerYaw/MovementDirection/FaceTarget 的模式请求和优先级在此组件扩展，不先建立空的通用 Manager 或额外 Runtime 模块。

## 状态与生命周期

按 ASC 的当前约束计数解析状态，Status.Rotation.Locked 与既有 Gameplay.MovementStopped 都能提出 Yaw 限制。只有从无锁到有锁时捕获 Yaw；任意有效约束还存在就不释放。回调只更新约束数据，不调用 SetActorRotation，避免同帧 WindowEnd→WindowBegin 产生中间旋转。

解除锁后以可配置速率回到当前控制器/移动期望朝向；不恢复锁开始前缓存的旋转模式布尔值。镜头在锁定期间积累的角度不会在结束帧直接变成角色的瞬间掉头。

ASC 从 PawnExtension 获取，玩家 Owner 仍为 PlayerState、Pawn 为 Avatar。绑定前验证 Avatar；重复初始化幂等；解绑移除自己的 Tag 委托、清理自己的缓存和重播数据，不清空共享 ASC 标签。EndPlay、Pawn 更换与反初始化都覆盖清理。新组件应在注册 ASC 回调之前创建。

## 联机与预测

Window Loose Tag 不自动复制。拥有者由本地预测 Timeline 立即解析锁；服务器按自己的 Timeline 执行权威约束。组件复制小型解析状态（锁、锁定 Yaw、恢复状态），供模拟代理表现与拥有者的权威 Yaw 对齐，不创建每帧旋转 RPC 或第二条 Actor Transform 复制通道。

模拟代理只跟随服务器角色移动复制，不在 MoveUpdatedComponentImpl 中用滞后的本地状态修改服务器旋转。拥有者释放以本地窗口为准，避免网络延迟延长输入锁；锁仍有效时接受服务器的锁定 Yaw。服务器不信任客户端提交的“没有锁”标志。

客户端 SavedMove 保存旋转约束快照，跨锁状态边界或不同锁定 Yaw 的移动不合并。校正重播使用历史快照，不将当前 Tag 计数套到历史移动。网络校正与 Teleport 必须能应用权威位置/旋转。第一阶段不额外传输自定义客户端旋转授权；若联机验证揭示时钟/朝向误差，修正后再交付，不把简单读 Tag 描述为完整预测支持。

## 动画衔接

FullBody 期间程序性根骨 Yaw 修正随蒙太奇覆盖权重退出，避免视觉朝向与已锁胶囊相互补偿。只有程序性修正权重归零后才能安全清零残留 RootYawOffset；中途打断须保持连续。锁/恢复期间不产生新的 TurnInPlace，结束后沿用正常静止转身。

不关闭镜头，不抹掉攻击序列的骨骼旋转，不改变腿部 IK 的 FullBody 权重抑制、Stop、无起步入口或 EnablePivot=False。

## 文件与资产范围

新增 Public/Private/Component/HodgeCharacterRotationComponent.h/.cpp；修改 CombatCharacter、CharacterMovement、HodgeAnimInstance 和 HodgeGameplayTags。编辑器辅助如需对当前 Main 动画图写入，仅扩展已授权动画目录的限制，不引入 Runtime 的 Editor 依赖。

资产只涉及当前 Main 主图/固定层与 `/Game/CodexText/BasicAttack/DA_Attack01_Timeline` 至 05。先备份这些资产及本轮源码基线，保留用户已有迁移、清理、重命名和蒙太奇改动。

## 验证与交付标准

1. 有意义的原生测试覆盖：两份约束持有者、移除一份仍锁、最后一份释放；FaceRotation 与移动/根运动旋转入口；ASC 更换与解绑不会清掉其他持有者；恢复不会瞬时对齐；历史 SavedMove 不与当前状态混用。
2. 保存并正常关闭编辑器后执行常规 HodgepodgeEditor Win64 Development 与 Hodgepodge Win64 Development。Live Coding 不作为构建验证。
3. 主图、固定层和正式角色编译；单人实际 GAS 五段攻击：前 0.15 秒可转，窗口内镜头可转而胶囊与程序性朝向不变，退出后有界恢复，取消/打断不残留 Tag；修改播放速度验证按蒙太奇时钟。
4. 两玩家 Listen Server：拥有者、服务器及模拟代理的约束/朝向/攻击姿势采样；联机模拟有延迟时再核对校正重播。保留未覆盖边界的明确记录。
5. 编译通过、蓝图通过、PIE、联机、Cook/打包分别报告。不得用构建成功替代运行验证，不自动提交。

## 实施状态

- 设计及实时资产基线：已完成。
- Runtime 实现：已完成。新增旋转组件及 Status.Rotation.Locked，接入 CombatCharacter 的默认子对象、ASC 生命周期与 FaceRotation；移动组件覆盖 PhysicsRotation、最终移动旋转、SavedMove 快照及重播清理。约束状态按改变复制，未新增每帧旋转 RPC。
- 动画实现：已完成。NativeUpdateAnimation 缓存 bSuppressLocomotionYaw、bResetLocomotionYaw、LocomotionRootYawScale；Main 主图的程序性根骨 Yaw 按 FullBody 权重混出，完全覆盖后清零残留；UpdateRootYawOffset/ProcessTurnYawCurve 与固定层共享 WantsTurnInPlace 入口消费同一抑制状态。现有 TurnInPlace 片段可在隐藏的基础姿势中完成恢复，不阻断蒙太奇自身的骨骼动作。
- Timeline：已完成。五个 RotationLock 条目按设计配置，事件时间保持蒙太奇坐标，未改 Recovery、MoveCancel、NextAttack 或原连招跳转。
- 编辑器辅助：原 HodgeAnimationAuthoringLibrary 的限定目录扩展到 Main/Character/Hero/Anim；属性路径可进入节点/CDO 自己拥有的对象以清除旧动画输入绑定，禁止沿外部资产引用修改其他资源。没有新增 Editor 模块或 Runtime 编辑器依赖。
- 实现边界：本阶段统一的是旋转约束，基础朝向模式继续沿用当前 Pawn/移动组件配置；冲刺模式请求、目标朝向优先级、专用 Root Motion 授权和全项目 SetActorRotation 调用治理留给对应功能。显式 Teleport、网络校正与物理模拟可应用引擎/服务器旋转，不能把这个组件理解为拦截所有外部 Actor Transform 写入。

## 本轮执行与结果

源码及资产基线备份：`E:/Project/Backups/Hodgepodge/RotationLock/20261005-152522`，344 个文件，逐文件 SHA-256 校验；包含当时的 Source、当前 Main 动画目录、正式角色及五个 Timeline。保留本对话之前的迁移、清理和用户 ABP 重命名状态。

实际构建（编辑器保存状态确认无 dirty 包、无 PIE 后，通过正常关闭窗口退出；未使用 Live Coding）：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64
```

最终两项目标退出码均为 0，当前构建日志为 `Editor-build-current.log`、`Game-build-current.log`。首轮 Editor 构建暴露 SavedMove 参数隐藏父类 DeltaTime 的 C4458，改名后修复。项目原有插件声明、旧 IncludeOrder、非内联 generated.cpp 及 UI 弃用警告未顺手修改。

原生测试命令：

```powershell
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Rotation' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/LyraAnimationWork/RotationLock-20261005/Automation-current' '-abslog=E:/Project/Git/Hodgepodge/Saved/LyraAnimationWork/RotationLock-20261005/Automation-current.log'
```

`Hodge.Rotation.ConstraintsAndReplay` 最终 Success、0 错误、报告 1 个 warning，进程退出码 0。早期测试夹具曾重复初始化临时 World，以及在未注册 ASC 时初始化 ActorInfo，分别触发引擎断言；均已修正并重跑。测试覆盖两个锁持有者、移除其中一个仍锁、最后一个释放后有界恢复、最终移动旋转保持 Yaw 而位移可用、历史 SavedMove 与当前状态隔离、解绑保留共享标签及晚绑定既有 MovementStopped 约束。

资产修改和运行采样使用项目现有 MCP/Python 桥，实际主要命令：

```powershell
python -X utf8 Saved/LyraAnimationWork/backup_rotation_lock.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/prepare_rotation_animation_assets.py
python -X utf8 Saved/LyraAnimationWork/wire_rotation_animation_graphs.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/compile_rotation_animation_assets.py
python -X utf8 Saved/LyraAnimationWork/start_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_rotation_combo.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_rotation_cancel_speed.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_rotation_fourth_rootmotion.py
python -X utf8 Saved/LyraAnimationWork/stop_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/configure_main_network_pie.py
python -X utf8 Saved/LyraAnimationWork/start_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_rotation_network.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_rotation_network_lag.py
python -X utf8 Saved/LyraAnimationWork/stop_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/restore_main_pie_settings.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/finalize_rotation_lock.py
```

主图与固定层编译为 0 错误、0 编译警告，正式角色已编译保存；最终资产图导出和窗口配置已核对。Main 中除主图、固定层外的动画资产（包含模型、骨架、序列、蒙太奇、类型和压缩配置）哈希保持基线一致，EnablePivot=False。

单人正式地图：

- 真实 Enhanced Input→GAS 连招采集 716 条记录。现有资产实际路径是 1→2→3→5，未改成 1→2→3→4→5；各段锁均发生在配置区间，镜头持续转动，锁定期间胶囊 Yaw 最大误差约 0，退出后恢复。第四段另用直接蒙太奇及显式 Tag 窗口作姿势检查，不将它描述成真实 GA 第四段激活通过。
- 2 倍播放速率与中断采集 478 条记录：第一段锁定墙钟时间约 0.217 秒，符合约 0.225 秒的区间时长并受采样边界影响；锁定中调用 MontageStop 后，Timeline 中断清理释放约束。Stop 调用返回当刻仍可能持有 Tag，下一次实例时钟刷新完成清理；不将这个测试描述成所有取消入口均已覆盖。
- 第四段及动态 Root Motion 对照采集 538 条记录。同一转身序列作为 FullBody 动态蒙太奇，未锁时胶囊转动约 90.147 度，锁定时误差约 0.00000434 度。测试结束撤销自己添加的 Tag，并恢复临时的 UseControllerRotationYaw，未保存测试状态到资产。

两玩家正式地图 Listen Server：

- 正常网络采集 1424 条记录，覆盖 Authority、Autonomous Proxy、Simulated Proxy。服务器和自治客户端在各自锁定期间 Yaw 误差为 0；模拟代理进入锁状态时曾有约 4 度的暂时差值，因为约束状态与引擎移动复制并非同一数据包，不用本地旋转写入抵消它。
- `NetEmulation.PktLag 100` 延迟测试采集 1428 条记录。服务器和自治客户端在各自锁定期间误差为 0；模拟代理最大误差约 0.000174 度。这验证了该次延迟场景，不代表丢包、乱序、全部预测拒绝或不同机器时间误差已覆盖。延迟参数恢复为 0，PIE 恢复单人 Standalone。

未执行：完整武器/身体/判定框伤害回归、带命中窗口的锁覆盖验证、重生/换 Pawn 的实机联机回归、独立进程、Dedicated Server、Cook/打包。当前正式定义没有 HitCheck 配置，后续加入真实判定时必须再次验证锁窗口边界。

完整原始记录与最终汇总位于 `Saved/LyraAnimationWork/RotationLock-20261005`，不作为源码提交。一次性资产脚本不得盲目重跑。所有代码/资产变更未自动提交。

## 编辑器验收与后续调参

1. 打开 `/Game/ThirdPerson/Maps/ThirdPersonMap`，Play 后连续旋转镜头并攻击：出手前可转，锁窗口内身体的攻击基准不跟着镜头转，窗口后平滑恢复。
2. 打开 `/Game/CodexText/BasicAttack/DA_Attack01_Timeline` 至 05，在 Events 中找到 EventID=RotationLock、Kind=Window、WindowTag=Status.Rotation.Locked，调整 StartTime/EndTime。只修改时间，不通过 EventID 编写玩法分支。
3. 角色继承的 CharacterRotationComponent 提供 RecoveryTurnRate（默认 360 度/秒），用于窗口结束后的朝向恢复。基础移动/视角仍用现有配置。
4. 需要严格禁止输入转向的命中窗口，必须位于旋转锁区间内；需要蒙太奇自身转身的技能，应先设计明确的授权策略，不直接绕过组件写 Actor Rotation。
