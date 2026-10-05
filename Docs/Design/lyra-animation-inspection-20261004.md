# Lyra MCP 安装与动画系统研究记录

检查日期：2026-10-04。Lyra 路径：`E:/Project/UProject/study/Epic/LyraStarterGame`；Hodge 路径：`E:/Project/Git/Hodgepodge`。本机引擎核实为 `E:/UE/UE_5.5`，版本 5.5.4。

本轮完成 Lyra MCP 插件安装和动画系统读取，为后续 Hodge 实现保留依据。本轮没有修改 Hodge 的 C++、配置或蓝图，没有执行动画系统迁移。

## 1. 插件安装结果

从 Hodge 的 `Plugins` 复制以下插件到 Lyra 的 `Plugins`，保留现有版本和源码，排除 `Intermediate`、`.git` 和 PDB：

- `McpAutomationBridge`：版本 0.5.30，包含 Editor 模块 `McpAutomationBridge`、`McpAutomationBridgeFab`。
- `UnrealMCP`：版本 1.0，Editor 插件。

在 `LyraStarterGame.uproject` 中显式启用两者，并设置 `TargetAllowList: ["Editor"]`。未升级引擎、插件或构建设置。复制完成、构建开始前，逐文件哈希比较没有发现差异；构建后插件二进制由本机引擎重新生成。

Lyra 的 `Config/DefaultGame.ini` 增加：

```ini
[/Script/McpAutomationBridge.McpAutomationBridgeSettings]
bEnableNativeMCP=True
NativeMCPPort=3017
bLoadAllToolsOnStart=True
ListenPorts=8117
bMultiListen=False
bAllowNonLoopback=False
bRequireCapabilityToken=True
```

Lyra 原生 MCP 地址为 `http://127.0.0.1:3017/mcp`，WebSocket 端口为 8117。Hodge 的 3016/8116 配置保持不变。能力令牌由插件生成在 Lyra 的 `Saved/MCP/capability-token`，未写入本记录或源码。

`UnrealMCP` 的 TCP 端口 55557 在插件源码中写死。两个项目同时打开时，这一 TCP 服务会发生端口竞争；原生 MCP 的 3016/3017 地址已分开。通过 TCP 操作前必须读取并核实实际项目路径，避免操作错项目。本轮每次连接均核实目标为 Lyra。

## 2. 检查范围与证据

原始检查记录保存在：

`E:/Project/UProject/study/Epic/LyraStarterGame/Saved/MCPInspection/2026-10-04`

- `asset-list.json`：30 个核心资产，包括主动画蓝图、共享/具体动画层、角色/武器蓝图、通知、枚举与 Control Rig。
- `*-graphs.json`：25 个蓝图的图清单、节点/引脚连接、默认值和已有编译状态。插件清单统计共 218 个图、1787 个节点；主蓝图为 80 个图/629 个节点，共享层为 83 个图/727 个节点。
- `*.t3d`、`*-parsed.json`：对象文本导出和解析结果，用于核对节点回调、Property Access、同名 Transition 图及默认资产引用。导出包含编译生成的重复对象，不能用其对象数替代蓝图节点数。
- `resource-samples.json`、`cosmetic-*.json`：网格、骨架、后处理、动画曲线及压缩设置的补充读取。
- `asset-hashes-before-inspection.json`、`asset-integrity.json`：用户保存并关闭编辑器后建立基线，读取前后 30 个核心资产 SHA-256 均一致。
- `LyraEditor-build.log`、`LyraGame-build.log`、`build-results.json`：本轮实际构建记录。
- `LyraStarterGame.uproject.before`、`DefaultGame.ini.before`：安装前的项目与配置备份。

检查组合使用原生 MCP 图读取、UnrealMCP 的编辑器 Python 和 UObject 文本导出。没有保存或批量重编译被检查的蓝图。25 个蓝图读取时均为 `BS_UpToDate` 且 `hasCompileErrors=false`；这表示其当前已有状态，不等于本轮逐个重新 Compile 验证通过。

## 3. 从角色到最终姿势的调用链

### 3.1 角色主网格与可见外观

`/ShooterCore/Game/B_Hero_ShooterMannequin` 的角色 Mesh 使用 `SKM_Manny_Invis`，动画类为 `ABP_Mannequin_Base_C`。它继承 `B_Hero_Default`；不能只查看默认父蓝图就推断运行角色 Mesh 的配置。

`/Game/Characters/Cosmetics/B_MannequinPawnCosmetics` 通过外观标签选择 Manny/Quinn 隐形骨架网格，并指定 `PA_Mannequin`。可见的 `/Game/Characters/Cosmetics/B_Manny`、`B_Quinn` 使用 `SKM_Manny`、`SKM_Quinn`，其 MeshComponent 的动画类是 `ABP_Mannequin_CopyPose_C`，动画更新选项为 `AlwaysTickPoseAndRefreshBones`。

这些网格共用 `/Game/Characters/Heroes/Mannequin/Meshes/SK_Mannequin`。可见网格另有 `ABP_Manny_PostProcess` / `ABP_Quinn_PostProcess` 后处理，其中使用 `CR_Mannequin_Procedural`；隐形网格没有这一后处理动画蓝图。

因此主运动逻辑、外观 Copy Pose 和可见网格后处理是三个相互配合的入口。后续 Hodge 是否需要沿用这一外观结构，须结合实际角色网格、骨架和部件系统决定。

### 3.2 C++ 提供基础数据和 GAS 标签

`ULyraAnimInstance` 主要负责两项工作：

- 初始化 `GameplayTagPropertyMap`，把 ASC 标签映射到蓝图属性。既有动画实例初始化入口，也有 ASC 更换 Pawn Avatar 后的主动初始化入口。
- 每帧从 `ULyraCharacterMovementComponent::GetGroundInfo()` 取得 `GroundDistance`，供落地逻辑使用。

大量动画逻辑实际位于蓝图。角色/移动组件还提供模拟代理的加速度复制和地面距离计算：角色将加速度压缩后以 `COND_SimulatedOnly` 复制，客户端解压并调用 `SetReplicatedAcceleration`；移动组件模拟时保留这一加速度。只复制动画实例类无法覆盖多人移动表现所需的数据链。

### 3.3 主动画蓝图

资产：`/Game/Characters/Heroes/Mannequin/Animations/ABP_Mannequin_Base`，父类 `LyraAnimInstance`。

核心更新在 `BlueprintThreadSafeUpdateAnimation`，按顺序更新位置、旋转、速度、加速度、角色状态、混合权重、瞄准、跳跃/下落、Root Yaw Offset 和撞墙检测，最后结束首次更新标记。Property Access 的实际来源包括：

- Owner 的 Actor Location。
- Pawn 的 Velocity、Base Aim Rotation Pitch。
- Movement Component 的 Current Acceleration、Is Crouching、Is Moving On Ground、Movement Mode、Gravity Z。

主蓝图将以下标签映射到布尔变量：

- `Event.Movement.ADS` → `GameplayTag_IsADS`。
- `Event.Movement.WeaponFire` → `GameplayTag_IsFiring`。
- `Event.Movement.Reload` → `GameplayTag_IsReloading`。
- `Event.Movement.Dash` → `GameplayTag_IsDashing`。
- `Event.Movement.Melee` → `GameplayTag_IsMelee`。

`LocomotionSM` 包含 Idle、Start、Cycle、Stop、Pivot，以及 JumpSelector、JumpStart、JumpStartLoop、JumpApex、FallLoop、FallLand、EndInAir 和状态别名。状态转换同时使用加速度、速度、局部速度/加速度点积、撞墙、蹲伏/ADS 改变、动画层改变、近战状态及落地距离。

Start/Stop/Pivot 的状态节点绑定设置/更新回调；状态机更新比较 `GetLinkedAnimInstance` 与上一动画层，处理装备切换后重新进入适当状态。迁移时必须保留回调绑定及转换规则，不能仅复制看起来相同的姿势节点。

主姿势图组合 Locomotion 缓存、左右手/瞄准动画层、全身与上身蒙太奇、加法姿势、分层混合、Inertialization、Rotate Root Bone、骨骼控制和 Foot Plant Control Rig。实际 Slot 名称为 `FullBody`、`UpperBody`、`UpperBodyAdditive`、`FullBodyAdditivePreAim`、`AdditiveHitReact`，迁移的蒙太奇必须与这些 Slot 对应。

已核实的默认值：

- Root Motion Mode：`RootMotionFromMontagesOnly`。
- 启用多线程动画更新。
- Cardinal Direction Dead Zone：10。
- Root Yaw Offset 支持 BlendOut/Hold/Accumulate；一般角度限制为 -120～100，蹲伏为 -90～80。
- `UseFootPlacement` 默认 false；Foot Placement 节点和 Foot Plant Control Rig 都存在，但实际执行受开关、曲线、LOD 等条件控制，不能认为两者总是同时生效。

## 4. 共享层与武器动画集合

### 4.1 动画层接口

`/Game/Characters/Heroes/Mannequin/Animations/LinkedLayers/ALI_ItemAnimLayers` 定义 14 个接口：

```text
FullBodyAdditives
FullBody_IdleState / FullBody_StartState / FullBody_CycleState
FullBody_StopState / FullBody_PivotState
FullBody_Aiming
FullBody_JumpStartState / FullBody_JumpStartLoopState
FullBody_JumpApexState / FullBody_FallLoopState / FullBody_FallLandState
FullBody_SkeletalControls
LeftHandPose_OverrideState
```

共享实现为同目录 `ABP_ItemAnimLayersBase`，父类为 `AnimInstance`。其 `GetMainAnimBPThreadSafe` 通过 OwningComponent 获取主 AnimInstance，再转换为 `ABP_Mannequin_Base`。后续迁移必须替换这一蓝图类型及相关 Property Access 引用；仅将父类改为 `HodgeAnimInstance` 并不能解决这一依赖。

共享层管理不同方向的 Walk/Jog、Crouch、ADS 起步/循环/停止/Pivot、Idle/Idle Break、Jump、Turn 和瞄准姿势序列。多数具体武器层以继承和默认资源替换复用算法。

### 4.2 距离匹配与姿势修正

- Start：`AdvanceTimeByDistanceMatching` 使用本帧位移和 `Distance` 曲线推进序列。
- Stop：`PredictGroundMovementStopLocation` 使用速度、地面摩擦、制动摩擦/系数和制动减速度预测停止距离，再执行 `DistanceMatchToTarget`。
- Pivot：`PredictGroundMovementPivotLocation` 预测转向距离，结合距离匹配和按位移推进。
- Cycle：`SetPlayrateToMatchSpeed`；默认播放速率范围 0.8～1.2，Start/Pivot 的范围为 0.6～5。
- Landing：按 `GroundDistance` 曲线和 C++ 提供的离地距离执行 `DistanceMatchToTarget`。
- Orientation Warping、Stride Warping：修正方向与步幅；骨骼控制还包含 Leg IK / Foot Placement。
- Turn In Place：包含 Rotation/Recovery 状态，依赖 `TurnYawWeight` 和 `RemainingTurnYaw` 曲线。

Sequence 节点的 `OnInitialUpdate`、`OnBecomeRelevant`、`OnUpdate` 回调承担选序列、时间初始化与距离推进。复制时需要同时保留节点引用转换、回调绑定和所依赖的函数。

抽查 Rifle 的 Start/Stop/Pivot、Fall Land、TurnLeft_90 五个序列，分别确认 `Distance`、`GroundDistance`/`DisableLegIK`、`RemainingTurnYaw`/`TurnYawWeight` 曲线。五个序列均启用 Root Motion、Force Root Lock，Root Lock 为 Ref Pose，使用：

`/Game/Characters/Heroes/Mannequin/Animations/UniformIndexableCurveCompressionSettings`

这些是抽查结论，不代表全部动画序列逐个检查完成。动画序列启用 Root Motion 与主蓝图采用 `RootMotionFromMontagesOnly` 是不同层面的设置，需要分别保留。

### 4.3 具体动画层

`Animations/Locomotion/Rifle`、`Pistol`、`Unarmed` 下的普通及 Feminine 动画层均继承 `ABP_ItemAnimLayersBase`，主要替换默认动画资源。Shotgun 普通层继承 `ABP_RifleAnimLayers`，Feminine 层继承 `ABP_RifleAnimLayers_Feminine`。

外观标签 `Cosmetic.AnimationStyle.Feminine` 用于选择对应动画层；没有匹配规则时使用 Default Layer。卸装时选择 Unarmed 动画集合。

## 5. 装备驱动动画层切换

入口：`/ShooterCore/Weapons/B_WeaponInstance_Base`，父类 `LyraRangedWeaponInstance`。

实际链路为：

1. `OnEquipped` / `OnUnequipped`。
2. `DetermineCosmeticTags` 从角色部件获取 `Cosmetic.AnimationStyle` 标签。
3. 宏 `ActivateAnimLayerAndPlayPairedAnim` 调用 `PickBestAnimLayer(bEquipped, CosmeticTags)`。
4. 检查返回类有效，然后对 Pawn Mesh 调用 `LinkAnimClassLayers`。
5. 播放对应的 Weapon Equip / Unequip Montage。

Rifle/Pistol/Shotgun 的武器实例子蓝图配置各自动画层集合和配套蒙太奇。`AN_PlayWeaponMontage` 将角色蒙太奇中的通知与武器网格播放连接起来。

装备生命周期来自服务端装备管理，以及客户端装备 Fast Array 的新增/移除回调。后续 Hodge 实现需要沿实际装备链处理本地角色、模拟代理、重生和卸装；只在服务器调用层切换不能证明客户端表现正确。

## 6. Hodge 已核实的现状与后续边界

### 已有代码入口

- `Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h` 与对应 Private 实现：已有 GAS 标签映射初始化及 GroundDistance 更新。
- `Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp`：`InitAbilityActorInfo` 检测新的 Pawn Avatar 后，调用动画实例的 `InitializeWithAbilitySystem`。
- `HodgeCombatCharacter` / `HodgeCharacterMovementComponent`：已有加速度复制、解压、模拟保留加速度及地面信息入口。
- `.uproject` 已启用 `AnimationLocomotionLibrary`、`AnimationWarping`。

源码对比还发现 Hodge 动画实例构造函数没有 Lyra 对应的显式 `Super(ObjectInitializer)` 初始化列表。本轮仅记录这一差异，没有更改，也没有据此认定现有运行问题。

### 已确认的武器动画选择代码现状

`Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h` 中 `EquippedAnimSet`、`UneuippedAnimSet` 的反射属性及选择集合类型仍被注释。对应 `.cpp` 的 `PickBestAnimLayer` 当前返回空动画类。

最初研究按完整迁移 Lyra 武器层切换机制，将此列为待补代码。用户随后明确：角色始终有默认武器，更换武器不改变各种移动动画。按这一需求，武器层选择不属于 Hodge 的必补功能，不需要恢复这些集合；新的方案见第 8 节。现有函数暂时保留，后续决定清理前须核对蓝图引用。

### 已存在但本轮未读取内部图的 Hodge 资产

```text
/Game/Main/Character/Hero/Anim/ABP_Pover_Base
/Game/Main/Character/Hero/Anim/Layer/ALI_ItemAnimLayers
/Game/Main/Character/Hero/Anim/Layer/ABP_ItemAnimLayers_Pover_Base
/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_Dark
```

同时发现相关方向/Root Yaw 枚举、结构和曲线压缩资产。这里仅确认磁盘存在，不能认定它们已完整实现 Lyra 逻辑，也不能认定其骨架与 Manny/Quinn 兼容。保留现有 `Pover` 命名。

按用户补充需求，下一轮建议从 Hodge 主蓝图、共享层和实际角色 Mesh 配置的读取开始，确认已有图、骨架、标签来源和动画资源，再按以下顺序补齐：

1. 主实例绑定、线程安全数据读取和移动状态机。
2. 共享层回调、距离匹配、方向/步幅修正、Root Yaw 和 IK。
3. 具体动画资源、曲线/压缩设置、Slot 与蒙太奇。
4. 角色固定动画层初始化、重生与客户端生命周期；装备更换不改变移动动画层。
5. Editor/Game 常规构建、相关蓝图 Compile、单人 PIE 和双人联机表现验证。

这是后续工作的检查顺序，不是本轮已完成的 Hodge 实现清单。

## 7. 实际验证命令与结果

最初构建被已运行的 Lyra 编辑器及 Live Coding 阻止。用户保存并完全退出后，执行以下常规 UBT 构建：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' LyraEditor Win64 Development '-Project=E:/Project/UProject/study/Epic/LyraStarterGame/LyraStarterGame.uproject' -WaitMutex -architecture=x64
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' LyraGame Win64 Development '-Project=E:/Project/UProject/study/Epic/LyraStarterGame/LyraStarterGame.uproject' -WaitMutex -architecture=x64
```

- Editor：退出码 0，123 个构建动作，约 347 秒。
- Game：退出码 0，362 个构建动作，约 316 秒。
- 插件有 UE API 弃用警告，未借本轮任务修改第三方实现。

然后重新启动 Lyra：

```powershell
Start-Process -FilePath 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor.exe' -ArgumentList '"E:/Project/UProject/study/Epic/LyraStarterGame/LyraStarterGame.uproject"','-NoLiveCoding','-NoSplash' -WindowStyle Hidden
```

编辑器日志确认两个插件加载，TCP 55557 和 WebSocket 8117 启动；3017 原生 MCP 完成初始化、能力发现和蓝图读取。项目身份读取返回目标 Lyra `.uproject` 路径。

本轮未执行逐资产重新 Compile、PIE、联机、Cook 或打包验证。Hodge 只有本研究文档新增，因此未对 Hodge 执行 C++ 构建。Lyra 保持打开供继续查看。

## 8. 按 Hodge 需求重新评估（2026-10-04）

用户明确的玩法约束：角色一定有默认武器，换武器不改变角色的各种移动动画。这是后续实现依据；本节取代第 6 节最初对武器层切换的必补判断，前面关于 Lyra 原始实现的事实记录保持有效。

### 8.1 可以省略按武器选择移动动画层

决定是否需要武器动画层切换的关键是移动姿势是否随武器改变。默认武器保证装备基线，但不能单独推导出动画层无用；用户同时明确所有武器共享移动动画，因此 Hodge 可以采用一套固定移动动画集合。

不需要为每种武器复制 Rifle/Pistol/Shotgun/Unarmed 式的移动层，不需要装备/卸装时通过 `PickBestAnimLayer` 选择移动层，也不需要仅为这一用途恢复 `EquippedAnimSet`、`UneuippedAnimSet` 或外观标签驱动的武器层集合。

现有源码已提供默认武器入口：`UHodgePawnData::DefaultWeaponDefinition`，以及 `AHodgeCombatCharacter::InitializeDefaultEquipment`。后者在服务器侧、ASC/Pawn 等条件满足后调用装备管理组件创建武器，客户端接收复制。该配置当前允许留空，初始化和复制也有时序，因此“始终有武器”的玩法约束不表示动画初始化时武器实例指针一定已经有效。固定移动动画应能独立初始化，避免等待装备实例才获得基本姿势。

### 8.2 保留共享动画层作为固定的角色移动实现

Lyra 的 `ABP_ItemAnimLayersBase` 同时承载距离匹配、起步/刹停/Pivot 回调、方向/步幅修正、原地转身和 IK。取消武器切换不意味着这些功能可以一并删除。

建议迁移初期复用 Hodge 已有 `ALI_ItemAnimLayers` 和 `ABP_ItemAnimLayers_Pover_Base` 的结构，将共享层视为固定的角色移动实现：

- 主蓝图负责线程安全数据读取、状态机、Root Yaw 和最终姿势组合。
- 固定共享层负责序列选择、节点回调、距离匹配及骨骼修正。
- 在角色/动画实例初始化时确定固定实现类；每次新 Pawn 或动画实例重新初始化都要恢复绑定。武器装备回调不负责这一绑定。
- 保留所需的初始化处理。Lyra 的 `LinkedLayerChanged` 检测与因武器切换产生的转换分支，可在核实 Hodge 实际图和回调依赖后裁剪，不提前整体删除。

这里的“固定共享层”仍可使用 Linked Anim Layers 技术，但其归属和选择依据是角色，而非当前武器。上述是推荐结构，Hodge 现有蓝图的内部图尚未读取，不能据此声称已经实现。

也可以最终将共享层逻辑合并进一个主 AnimBP，彻底省去链接层。这需要重接主实例访问、Sequence 节点回调、函数引用和相关状态初始化。当前已有主蓝图与共享层资产，先采用固定层可减少迁移范围；是否合并应在读完 Hodge 实际图后决定。

### 8.3 战斗动画和移动动画分别确定资源

换武器不改变移动集合，仍可改变攻击、技能、装备动作或武器自身动画。这些差异可以由能力/战斗数据选择 Montage，再通过固定的全身或上身 Slot 与移动姿势混合，无需因此切换整套移动层。

Hodge 的 `HodgeAbilityDefinition`、`HodgeGameplayAbility_BasicAttack` 和 ASC 已有 Montage 入口。后续沿实际调用链确认资源选择、混合与复制，不默认所有武器必须共用攻击蒙太奇。需要持续的持握姿势或左右手对齐时，可按实际美术需求加入姿势叠加/IK 参数；当前不为尚未提出的差异建立武器层系统。

ADS、射击后坐力、换弹等 Lyra 枪械专用逻辑，以及 Copy Pose 外观结构，均应按 Hodge 实际玩法和骨架需求选择。距离匹配、转身、跳跃落地等移动品质算法仍然是本次研究的可复用部分。

### 8.4 本次重评估验证范围

重新读取项目开发约定、当前默认装备/武器/能力相关源码和前轮 Lyra 图记录，更新本研究文档。执行 `git status --short`、`git diff --check` 并检查文档编码和格式；未修改 C++、配置、资产或插件，未执行新的构建、蓝图 Compile 或 PIE。
