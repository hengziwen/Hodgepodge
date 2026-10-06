# Pover 的 Lyra 动画实验

> 2026-10-06 状态同步：动画主资源已迁 Main/Character/Hero/Anim，主图当前 ABP_Pover_Base；本目录保留实验地图/测试角色。DefinitionCombo/BasicAttack/WeaponPresentation 仍是正式默认玩法依赖，不整体删除 CodexText。 当前项目事实见 [本轮更新](../../../Docs/KnowledgeBase/26-update-2026-10-06.md)。

2026-10-05 已按用户授权迁入 `/Game/Main/Character/Hero/Anim` 并替换正式角色的引用。当前运行路径、备份、验证及恢复说明见 `Content/Main/Character/Hero/Anim/README.md`。本目录保留实验地图、测试角色和工具；下文是迁移前的开发记录，其中旧资源路径不再代表当前存放位置。工具脚本已更新为 Main 资源路径。

## 直接体验

1. 使用 UE 5.5.4 打开 Hodgepodge。
2. 打开 `/Game/CodexText/LyraAnimation/Maps/L_PoverLyraLab`。
3. 点击 Play。关卡使用 `BP_Pover_LyraGameMode`，生成 `BP_Pover_LyraHero`，继续走现有 Experience、PawnData、PlayerState ASC 和默认剑初始化。
4. 使用项目已有移动、跳跃和攻击输入。测试平台包含平地、斜坡和台阶。
5. 检查直接进入移动循环、急停、常态反向不触发 Pivot、连续镜头转身、跳跃落地以及剑攻击。

保留本次修正前用户的实验角色设置：`UseControllerRotationYaw=True`、`OrientRotationToMovement=False`、`UseControllerDesiredRotation=False`、`RotationRate.Yaw=720`。静止时由 RootYawOffset 和转身动画补偿镜头转向。

## 资产分工

- `Animation/ABP_Pover_Lyra`：继承现有 `HodgeAnimInstance`，移植完整 Lyra 主动画图及线程安全更新、移动状态机、RootYawOffset、TurnInPlace、跳跃和蒙太奇组合。
- `Animation/ABP_Pover_LocomotionBase`：固定角色移动资源层，承载停止的距离匹配、循环速度匹配、方向和步幅扭曲、脚部放置及 Leg IK。Pivot 资源与距离匹配逻辑保留，当前入口关闭。
- `Animation/ALI_Pover_LocomotionInterface`：主图和固定层之间的 14 个动画接口。
- `Model/SK_Pover_LyraLab`、`SKM_Pover_LyraLab`：Pover 骨架/网格副本，骨骼引用映射为 Bip001 和现有虚拟骨骼。副本骨架兼容原 Wuwa 骨架，支持已有攻击蒙太奇。
- `Sequences/A_Pover_*`：30 个角色动画副本和实验用曲线，原动画不变。
- `Types`：方向/RootYawOffset 枚举、方向资源结构体、转身数据结构体及曲线压缩设置。
- `Test`：实验角色和 GameMode，避免修改 Main 的默认角色、Experience 或项目配置。

主图的 Linked Anim Layer 实例类和实验角色 BeginPlay 链接类始终指向 `ABP_Pover_LocomotionBase`，装备生命周期不负责切换移动资源。保留接口是为了复用 Lyra 图和回调，不构成武器动画层选择机制。

移除主图对 Manny ControlRig 的执行依赖。固定层输出绕过枪械瞄准姿势、左手覆盖和枪械手 IK；相关旧节点仍是断开的参考节点，不参与最终姿势。骨骼遮罩改为 Pover 脊柱分支过滤。

保留 Lyra 的 FullBody、UpperBody 等插槽，主图已移除额外添加的 `DefaultSlot`。用户负责修改现有蒙太奇的 Slot：全身剑攻击使用 `FullBody`；只有上半身的动作使用 `UpperBody`。仍使用 DefaultSlot 的蒙太奇暂时不会从本主图输出。`Status.Attack` 映射为主实例 `GameplayTag_IsMelee`，能力激活仍需通过现有连击输入授权流程。

## 资源与调参边界

这是可运行的算法与蓝图迁移版本，动作美术还需要验收：

- 当前 Wuwa 步行/跑步资源为原地动画。在副本中为距离匹配添加标称 Root 位移和 `Distance` 曲线，步行 160 cm/s、跑步 500 cm/s；它们是实验标定值，不是从原始 Root Motion 测出的速度。角色移动速度仍保留 Main 的 420 cm/s。
- 不使用起步动画：Idle 开始移动及 Stop 中重新移动均直接进入 Cycle，Cycle→Stop→Idle 仍保留。早期的 Start 状态、函数和副本素材留作断开的参考，没有入口，不参与运行。急停复用左右 Stop Run。四方向 Pivot 的 Run Turnback 资源仍保留，常态移动暂不进入 Pivot，后续冲刺 GA 完成后再接入冲刺状态和速度条件。侧向素材部分使用 LF/RF，需要检查脚滑和姿势过渡。
- 跳跃分为起跳、顶点、下落和落地；落地片段添加 `GroundDistance`。左右 90 度转身添加 `RemainingTurnYaw`、`TurnYawWeight`。
- 没有找到对应的完整蹲姿资源，目前蹲姿资源槽使用站姿后备，不代表蹲姿动作已经完成。
- Lean 使用局部叠加的中性后备资源，保留计算接线但没有独立的倾身动作素材。落地恢复使用局部叠加资源。
- Foot Placement 使用 UE 5.5 的实验节点。已映射 Pover 骨骼，但斜坡、台阶、极端速度和不同角色比例的最终视觉效果需要逐项验收。

`BS_Pover_Fallback` 和 `ALI_Pover_Locomotion` 是早期建立的后备资产；运行路径使用 `BS_Pover_NeutralLean` 和 `ALI_Pover_LocomotionInterface`。

## 编辑器辅助代码

`Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.*` 为现有 Editor 模块补充完整动画图文本导入、节点/CDO 属性配置和编译诊断，用于弥补 MCP 对状态机和原生覆盖函数的创建限制。资产写操作限制在本实验资产目录，导入不覆盖已有资产。另有实验关卡限定的临时 PIE 设置入口，用于绕过 Python 未暴露 PIE 网络枚举的限制；不调用 SaveConfig。

仅在 `HodgeAbilityEditor.Build.cs` 添加编辑器依赖 `BlueprintGraph`、`KismetCompiler`，没有修改 Runtime 模块或引擎配置。正常运行已保存的蓝图不需要重新执行导入脚本。

## 本次验证记录

构建、蓝图编译、运行采样和依赖检查的原始记录位于 `Saved/LyraAnimationWork`，该目录不作为源码提交。最终结果以本次交付说明和下方更新记录为准。C++ 构建成功不代表 Cook、打包或独立服务器验证通过。

2026-10-04 初版验证结果（下方修正记录补充本次用户反馈后的检查）：

- 常规 `HodgepodgeEditor Win64 Development` 构建退出码 0，最后一次耗时 14.36 秒；`Hodgepodge Win64 Development` 构建退出码 0，耗时 61.89 秒。
- 主动画、固定层、实验角色、实验 GameMode 均重新编译并保存：0 错误、0 编译警告。
- 单人 PIE：四方向判定、420 cm/s 移动、Distance 曲线、起停/反向、跳跃/下落/落地完成运行采样；现有 `AM_Attack01_Montage` 在 DefaultSlot 播放，插槽权重达到 1，攻击标签为真。
- 临时装备/卸下第二把剑，固定层实例保持不变，原默认剑仍保留。
- 双人 Listen Server PIE：已确认 Authority、Autonomous Proxy、Simulated Proxy 三种角色；所有角色加载实验主实例和固定层。移动、攻击蒙太奇和攻击标签取得服务端/客户端数据，默认剑正常生成。这是基础同步测试，不覆盖丢包、重生、晚加入或独立进程。
- 坡道行走采样：角色从平地走到斜坡高处，保持 grounded，脚和骨盆坐标均有限。尚未逐帧验收足底贴合和台阶稳定性。
- 45 个实验 UE 资产的注册表依赖检查没有残留 `/Script/Lyra*` 或 Lyra 角色资源依赖；Main 的 51 个资产及原 CodexText/AnimInstance 的 15 个资产哈希未变。
- PIE 设置恢复为 1 玩家、Standalone；实验关卡保留打开，未保存地图/内容包数量均为 0。没有执行 Cook、打包或独立服务器测试。

实际构建命令（PowerShell）：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE
```

`Tools/verify_single_player.py`、`Tools/verify_network.py` 可在对应 PIE 已完成角色初始化后，从编辑器 Python 控制台执行。它们会临时控制角色并向 Saved 输出 JSON，单人脚本约 18 个游戏秒，联机脚本约 8 个游戏秒；运行期间不要手动移动角色。联机模式可先在实验地图编辑状态执行 `unreal.HodgeAnimationAuthoringLibrary.configure_animation_lab_pie(2, True)`，结束 PIE 后用 `(1, False)` 恢复。

`Tools/verify_camera_turns.py` 在单人 PIE 中依次设置 0、100、200、300、40、-60 度镜头方向，约 18 个游戏秒。结果记录权重归零、动画时间重新起算及脚踝/脚趾位置，用于复现连续转身问题。

测试过程中，一次通过内部 `GetCurrentStateName(0)` 查询动画状态机的调试调用触发了 UE 崩溃。已保存资产完整，改用动画变量、曲线、蒙太奇和骨骼坐标采样后，上述运行测试完成；不再使用该查询。PIE 日志仍有 CommonUI 的 GameViewportClient 配置提示，本次未处理 UI 配置。

## 起步、脚部 IK 与连续转身修正

- Idle→Cycle 和 Stop→Cycle 直接衔接移动循环，Start 没有入口；Cycle→Stop→Idle 和停止资源保持。没有删除 Stop 或把停止替换为 Idle。
- 原有 `VB ik_foot_left/right` 指向 `Bip001LToe0/RToe0`。Leg IK 会把目标的旋转写到脚踝，错误地把脚趾朝向当成脚踝朝向，造成脚掌下垂。只在实验骨架增加 `VB Root_Bip001LFoot/RFoot`，并同步更换 10 个 Leg IK、Foot Placement、Orientation/Stride Warping 节点的目标引用。FK 脚踝与 Ball 脚趾仍各用自己的真实骨骼，Foot Placement 保持启用。
- 转身末尾的 `TurnYawWeight` 从 1 归零，让 IdleSM 能退出转身；转身 evaluator 改为 `DoNotSync`、不循环，避免被 Idle 的同步组重新拉回循环时间。主图首次采样采用带符号的 90 度初始值，补偿低帧率时跳过的首段曲线。这个初始值只适用于当前左右 90 度素材，更换其他角度素材时需一起调整。
- 本轮没有修改 C++，不重复执行初版的 Editor/Game 构建。修正后的编译、PIE 和姿势证据另存于 `Saved/LyraAnimationWork/Fixes-20261004`。

本轮检查结果：

- 主图、固定层、实验角色、实验 GameMode：0 错误、0 编译警告。最终图检查确认 Start 无入口、两个入口直接到 Cycle、Cycle→Stop→Idle 的连接和过渡参数与修正前一致。
- 单人 PIE 采集 216 条记录，覆盖四方向、停止后重启、反向、跳跃/下落、剑攻击及临时装备；420 cm/s 移动、攻击蒙太奇/标签、固定层不随装备变化均取得运行数据。
- 分段镜头转向在正常帧率和临时限制 5 FPS 下分别采集 270、89 条记录，每个目标角度停留约 3 秒。每次新增转向都再次播放转身，权重归零，时间重新起算，反向转动同样完成。该测试没有覆盖镜头持续转动，后续连续转动修正见下一节。
- 平地近景和坡道截图已检查脚掌朝向；静止时左右 FK 脚踝的旋转分别与新 IK 目标一致。坡道采集 41 条记录，Foot Placement 开启、全程 grounded，角色高度从约 92 cm 升至 247 cm。这些检查不代表所有台阶、不同骨架和极端坡度均已验收。
- 为采样临时使用的镜头、帧率限制和后台节流均恢复；镜头仅在 PIE 世界生成，不保存到实验地图。
- 双人 Listen Server 回归采集 92 条记录，覆盖 Authority、Autonomous Proxy、Simulated Proxy；移动、攻击蒙太奇/标签及默认剑正常取得数据。没有验证晚加入、丢包和独立进程。
- 45 个 UE 资产没有残留 Lyra/Manny 依赖；Main 51 个资产和旧 CodexText/AnimInstance 15 个资产哈希保持不变。结束 PIE 后恢复 1 玩家 Standalone，未保存地图/内容包为 0，实验地图保留打开。

实际修正与检查命令（PowerShell，详细 Python 源码和 MCP 回执保留在 Saved）：

```powershell
python -X utf8 Saved/LyraAnimationWork/bypass_start.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/apply_animation_fixes.py
python -X utf8 Saved/LyraAnimationWork/fix_first_turn_sample.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/isolate_turn_playback.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Content/CodexText/LyraAnimation/Tools/verify_single_player.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Content/CodexText/LyraAnimation/Tools/verify_camera_turns.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_ramp.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/finalize_animation_fixes.py
python -X utf8 Saved/LyraAnimationWork/validate_final_graphs.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Content/CodexText/LyraAnimation/Tools/verify_network.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/audit_assets.py
```

本轮编辑曲线时曾触发异步压缩读取空曲线的编辑器崩溃；已经改为更新现有末帧键值，不再移除/重建曲线。重新打开保存的资产后完成上述检查。尚未执行 Cook、打包或独立进程网络测试。

## DefaultSlot、Stop 与持续转身修正（2026-10-04 至 2026-10-05）

- 删除主图的 DefaultSlot 节点，原先的 Additive 输出直接进入 FullBody，再进入 Inertialization。运行实例只注册 FullBody、UpperBody、AdditiveHitReact、FullBodyAdditivePreAim、UpperBodyAdditive 五个插槽。骨架的默认插槽名称不影响这条输出路径。现有 Main 蒙太奇留待用户修改，初版和上一轮的 DefaultSlot 攻击结果属于历史验证。
- 原 Stop 初始化在不能距离匹配时执行 `DistanceMatchToTarget(0)`，会直接定位到 3.23/3.33 秒的末帧。现在选好停止资源后先明确将 evaluator 时间重置到 0；速度仍非零时继续匹配预测停止距离，速度归零后推进动画时间。Cycle→Stop 混合从 0.25 秒缩短到 0.1 秒，Stop→Idle 混合从 0.75 秒缩短到 0.2 秒，保留完整停止和身体恢复片段。
- 四个 Stop 副本原 Distance 曲线在尾段反复增减，不满足 UE 距离匹配的递增要求。改为首段制动轨迹的累计距离，并在首次停止后保持 0；根骨细小的回摆不再把时间匹配到错误位置。新增 StopAnimTime/StopAnimWeight 作为节点回调诊断值，状态退出后它们可能保留最后值，不能只凭时间字段判断当前状态。
- 左右转身素材约 0.87 秒已经完成 90 度，之前 TurnYawWeight 要等到 1.5 秒片段末尾才归零。现在 RemainingTurnYaw 单调归零，TurnYawWeight 在约 0.9 秒归零；恢复节点显式连接上一段 TurnInPlaceAnimTime 作为 StartPosition，后面的身体恢复仍然保留。
- 连续转身的新一段剩余角度会重新增大或改变符号。ProcessTurnYawCurve 识别这两种情况，使用该段带符号的 90 度起点，不再把上一段残值与新段相减。Inertialization 过滤 TurnYawWeight/RemainingTurnYaw 两条控制曲线，姿势继续做惯性混合。这套起点逻辑适用于当前 90 度素材。
- 起步仍无入口，Idle/Stop 恢复移动直接进入 Cycle；脚踝 IK 的上一轮修正和用户角色旋转设置保留。本轮没有修改 C++、Main 或旧 CodexText 动画资产。

本轮实际验证：

- 主图和固定层编译、保存：0 错误、0 编译警告。最终图和运行 CDO 检查确认 DefaultSlot 已退出输出路径、Start 无入口、Stop 可达、Recovery 时间显式接入、控制曲线已过滤。
- 正常帧率单人 PIE：持续向右、持续向左及每 0.75 秒反向，共采集 749 条记录；三个阶段分别发生 5、6、4 次再次转身。约 33 毫秒的采样间隔中，最大可见朝向步进约 5.86 度，骨盆旋转步进约 5.96 度，未复现衔接处接近 90 度的跳变。镜头停止后转身权重归零。
- 临时限制 5 FPS：同一回归采集 124 条记录，三个阶段分别发生 5、4、3 次再次转身；Stop 从有效时间开始推进，未直接跳到末帧。低帧率本身仍有较大的逐帧角度步进，不代表 5 FPS 动画视觉平滑。
- 前后左右急停：采集 720 条记录，四方向速度均达到 420 cm/s，Stop 权重均达到 1，动画时间持续推进，结束时权重归零。全程使用实验地图；未重复执行联机或打包验证。
- Main 的 51 个资产、原 CodexText/AnimInstance 的 15 个资产哈希未变；用户旋转设置保持。结束 PIE 后未保存地图/内容包为 0，t.MaxFPS 恢复 0、后台 CPU 节流恢复原值，编辑器和实验地图保留打开。
- 本轮没有 C++ 变更，因此没有重复 Editor/Game 构建；移除 DefaultSlot 后没有重测仍使用它的攻击蒙太奇。

实际修正和验证命令（PowerShell）：

```powershell
python -X utf8 Saved/LyraAnimationWork/remove_default_and_probe_stop.py
python -X utf8 Saved/LyraAnimationWork/fix_stopturn_graphs.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/fix_stopturn_assets.py
python -X utf8 Saved/LyraAnimationWork/start_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Content/CodexText/LyraAnimation/Tools/verify_stop_continuous_turn.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/set_stopturn_fps.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Content/CodexText/LyraAnimation/Tools/verify_stop_continuous_turn.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/restore_stopturn_fps.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Content/CodexText/LyraAnimation/Tools/verify_stop_directions.py
python -X utf8 Saved/LyraAnimationWork/stop_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/restore_background_throttle.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/finalize_stopturn.py
python -X utf8 Saved/LyraAnimationWork/validate_stopturn.py
```

脚本与 MCP 回执、修正前备份和运行 JSON 位于 `Saved/LyraAnimationWork/StopTurn-20261004` 及其父目录。修正脚本用于本轮资产变更，不应盲目重跑；`Tools/verify_stop_continuous_turn.py`、`Tools/verify_stop_directions.py` 是可重复运行的单人 PIE 回归，各需约 25、24 个游戏秒，执行期间会控制实验角色，须等待其完成再操作或运行其他控制脚本。

## FullBody 攻击下半身被 IK 覆盖修正（2026-10-05）

用户已将 `/Game/CodexText/Montage/AM_Attack01_Montage` 至 `AM_Attack05_Montage` 改为 FullBody。检查确认插槽正确，五段攻击和原始序列均没有 DisableLegIK 曲线。FullBody 的全身姿势进入主图后仍经过固定层的 Foot Placement 和 Leg IK，腿部求解把 FK 脚拉回虚拟骨骼目标；改 Slot 本身无法解除这一步覆盖。修正前第一、二段攻击的脚部原始轨迹有约 40–50 cm 位移，最终脚部轨迹却基本静止。

修正只改变 `ABP_Pover_LocomotionBase` 的 `FullBody_SkeletalControls`：

- 从主动画实例读取 FullBody 蒙太奇的局部混合权重和 DisableLegIK 曲线。Leg IK 的浮点 Alpha 使用 `1 - Clamp(Max(FullBodyWeight, DisableLegIK), 0, 1)`，并移除原先对 Alpha 的反向 Scale/Bias。满权重的全身动作不再由移动用腿部 IK 覆盖。
- Foot Placement 也改用浮点 Alpha，使用同一抑制权重，同时保留主实例的 UseFootPlacement 开关。攻击淡出时随蒙太奇权重逐步恢复，避免用 IsMelee 或 IsSlotActive 在混合边界硬切。
- UpperBody 蒙太奇不影响 FullBody 权重，继续保留移动和腿部 IK；原有 DisableLegIK 曲线仍可控制求解权重。
- 保留之前的脚踝虚拟骨骼、Stop、无起步入口和连续转身修正。本轮没有修改主动画图、用户五个蒙太奇、原始攻击动画、Main、C++ 或项目配置。单人/联机回归脚本的插槽采样名称更新为 FullBody。

实际验证：

- 固定层编译和保存：0 错误、0 编译警告。开始时一次编译发现旧 Foot Placement 条件函数含有不能在线程安全图调用的 IsValid 宏；最终改为直接读取主实例的 UseFootPlacement，保留原条件函数的线程属性，没有关闭编译检查。
- 五个 FullBody 蒙太奇逐一播放，修正前采集 698 条、修正后 704 条记录。将最终骨盆、大腿、小腿和脚的姿势与原始动画同一时间的骨骼姿势比较；稳定满权重阶段的最大位置误差约 0.105 cm。左右脚平均误差从约 18.6–32.7 cm 降至约 0.002–0.041 cm，脚部轨迹已跟随攻击资源。
- 额外采集 330 条记录验证实际攻击输入/GAS：AM_Attack01_Montage 在 FullBody 权重达到 1，攻击标签为真，FK 脚不再固定到 IK 目标。攻击结束后权重归零，FK/IK 脚位置重新对齐。
- 临时播放 UpperBody 蒙太奇并移动角色，UpperBody 权重达到 1、FullBody 权重保持 0，腿部 IK 持续求解。该蒙太奇只存在于 PIE，不保存资产。
- Main 51 个资产、用户刚改的 5 个蒙太奇和实验主动画图均与本轮开始时相同。停止 PIE 后无未保存地图/内容包；后台 CPU 节流恢复原值，帧率限制仍为 0。
- 本轮没有 C++ 变更，未重复 Editor/Game 构建；没有执行新的联机、复杂地形攻击、完整连击链或 Cook/打包验证。

实际命令（PowerShell）：

```powershell
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/inspect_attack.py
python -X utf8 Saved/LyraAnimationWork/start_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Content/CodexText/LyraAnimation/Tools/verify_fullbody_attack.py
python -X utf8 Saved/LyraAnimationWork/stop_pie.py
python -X utf8 Saved/LyraAnimationWork/fix_fullbody_leg_ik.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/compile_fullbody_leg_ik.py
python -X utf8 Saved/LyraAnimationWork/start_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Content/CodexText/LyraAnimation/Tools/verify_fullbody_attack.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Content/CodexText/LyraAnimation/Tools/verify_attack_input_recovery.py
python -X utf8 Saved/LyraAnimationWork/stop_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/finalize_attack.py
python -X utf8 Saved/LyraAnimationWork/validate_attack.py
```

回执、修正前备份、图导出、原始姿势对比和结果位于 `Saved/LyraAnimationWork/Attack-20261005` 及其父目录。`verify_fullbody_attack.py` 约需 23.5 个游戏秒，`verify_attack_input_recovery.py` 约需 11 个游戏秒；二者会控制实验角色，须等一个脚本完成再运行另一个。验收时打开实验地图 `L_PoverLyraLab`，播放五段攻击：下半身应按原动画迈步、屈膝或抬腿，结束后恢复移动用脚部 IK。

## 常态移动暂时关闭 Pivot（2026-10-05）

- 主动画 `ABP_Pover_Lyra` 的唯一 Pivot 入口原先只判断速度与加速度反向且没有撞墙，没有冲刺状态或最低速度限制。因此当前 420 cm/s 常态移动也会触发。
- 新增布尔变量 `EnablePivot`，分类 `Locomotion|Pivot`，默认 False。入口图命名为 `PivotEntryRule`，最终条件为“原条件 AND EnablePivot”。常态反向继续使用 Cycle；Pivot 状态、退出路径和四方向素材保留，后续冲刺 GA 完成后再把冲刺状态和速度门槛接到这个入口。本轮没有实现冲刺 GA 或自动开关。
- 起步仍没有入口，Cycle→Stop→Idle 及 Stop→Cycle 不变；连续镜头转身、固定层和 FullBody 攻击的 IK 修正保持。只有实验主动画资产和本文档有本轮变更，没有迁回 Main。
- 主动画编译为 0 错误、0 警告。约 60 FPS 单人 PIE 采集 1184 条记录，覆盖前→后→前、左→右→左及两次完整停止。四次反向分别采到 4 帧满足原入口条件，EnablePivot 全程为 False，Pivot 动画初始化数据保持默认值。两次 Stop 权重均达到 1，时间推进至约 3.315 秒，结束权重小于 0.001。
- 图检查确认唯一 Pivot 入口受开关限制、Start 无入口、其他手工图节点和连线保持不变。Main 的 51 个资产、用户五个攻击蒙太奇及固定层共 57 个资产哈希与本轮开始时相同。结束 PIE 后无未保存地图/内容包，后台 CPU 节流恢复原值。
- 首次后台测试只有约 3 FPS，未捕捉到反向条件，不作为有效反向验证；切到前台后完成上述采样。本轮没有 C++ 变更，未运行 Editor/Game 构建；未重复联机或 Cook/打包测试。

实际命令（PowerShell；备份、脚本和回执保留在 Saved，不作为源码提交）：

```powershell
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/snapshot_pivot.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/name_pivot_rule.py
python -X utf8 Saved/LyraAnimationWork/disable_pivot.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/compile_pivot.py
python -X utf8 Saved/LyraAnimationWork/start_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_pivot_disabled.py
python -X utf8 Saved/LyraAnimationWork/stop_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/finalize_pivot.py
python -X utf8 Saved/LyraAnimationWork/validate_pivot.py
```

结果位于 `Saved/LyraAnimationWork/Pivot-20261005`。验收时打开 `/Game/CodexText/LyraAnimation/Maps/L_PoverLyraLab` 并 Play，连续切换前后、左右移动应继续走移动循环，松开移动仍有 Stop。现在无需手动调整开关。

上述验收已完成，用户于 2026-10-05 授权先备份再迁回 Main。本次迁移了 52 个动画相关资产，正式 BP_Hero_Pover 的网格、动画实例、旋转设置和 BeginPlay 固定层链接已更新；Main 旧同名资产保留，未覆盖。当前结果以 Main 动画目录 README 和 `Saved/LyraAnimationWork/MainMigration-20261005` 的本轮记录为准。
