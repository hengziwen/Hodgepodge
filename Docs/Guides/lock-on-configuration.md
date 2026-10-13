# 玩家锁定系统配置与验证手册

适用：UE 5.5.4，Hodgepodge。设计依据：[锁定设计](../Design/lock-on-system.md)。本手册描述实际接口，测试结论以本轮验证报告为准。

## 1. 正式资产与使用方法

- 玩家：`/Game/Main/Character/Hero/BP_Hero_Pover`，继承原生 `AHodgeHeroCharacter.LockOnComponent`。
- PawnData：`/Game/Main/Data/PawnData/DA_Dafult_PawnData`，`LockOnProfile` 指向 `/Game/Main/Character/Hero/Targeting/DA_HeroLockOn`。
- 授予：PawnData 的 `AbilitySets` 增补 `/Game/Main/Character/Hero/Targeting/AS_HeroLockOn`，其中原生 `HodgeGameplayAbility_LockOn` 等级 1。保持原有攻击、受击、死亡和移动授予。
- 输入：`/Game/Main/Input/InputAction/IA_LockOn` 为 Boolean；`/Game/Main/Input/IMC_Default` 映射 `MiddleMouseButton`；`/Game/Main/Input/DA_HodgeInputConfig.AbilityInputActions` 配置 `InputTag.Ability.LockOn`。
- 标记：`/Game/Main/UI/HUD/WBP_HodgeHUD` 的 `ReferenceHUDCanvas` 下新增 `LockOnIndicatorLayer`，拉伸铺满、ZOrder=-1、禁止 Hit Test。既有血条与终焉按钮布局保留。
- Manager：`AHodgePlayerController` 创建原生 `IndicatorManager`。当前 Controller 没有 ModularGameplay Receiver 注册，因此不假定 GameFeature 注入已经可用，不为标记重构整个 Controller 生命周期。

进入游戏后，按下鼠标中键获取屏幕内最合适的敌人，松开保留选择，再次按下解除。锁定期间鼠标可自由环绕，快速左右甩动仍可切换目标；输入期间按可配置系数抑制辅助，停止输入后逐渐恢复柔和构图。菜单阻止游戏输入时不触发选择和自动构图。用户确认本次没有加入未按中键时的攻击自动索敌。

## 2. 可配置参数

配置资产：`/Game/Main/Character/Hero/Targeting/DA_HeroLockOn`。

- `AcquireDistance=1500`、`KeepDistance=2000`：厘米，获取和保持使用不同阈值。
- `AcquireHalfAngle=60`：视线半角；客户端用实际镜头过滤候选，服务器不信任屏幕评分。
- `ScoreWeights=(0.55,0.30,0.15)`：屏幕中心、距离、目标优先级；归一化后组合。
- `KeepCheckInterval=0.1`、`DistanceGrace=0.35`、`OcclusionGrace=1.0`：秒；无锁定时不持续查询所有候选。
- `SightChannel=Visibility`：权威视线从 Pawn 视点到目标锚点；忽略双方和附着装备。
- `GestureWindow=0.12`、`SwitchAngle=18`、`SwitchSpeed=150`、`DirectionConsistency=0.8`：时间窗口、等效角度、每秒速率和方向一致性。
- `SwitchDebounce=0.25`、`RearmWindow=0.05`、`RearmAngle=3`：一次持续甩动只切一次，低输入后重新武装。
- `CommandTimeout=1.5`、`ReferenceTimeout=1.0`：命令与未解析目标引用的有限等待。
- `bEnableSoftCameraAssist=true`：可关闭自动构图而保留目标选择与手动镜头。
- `bEnableMouseFlickSwitch=true`：可单独关闭甩动切换，镜头仍可自由转动。
- `CameraAcquireDuration=0.35`：秒；首次目标和目标切换使用五次缓入缓出转向，建议 0.25～0.45。获取时不额外等待鼠标宽限。
- `CameraAcquireSpeed=720`：度／秒；获取转向安全上限，大角度或贴近目标时可能无法在配置时间内完全居中，随后继续柔和跟随。
- `CameraScreenDeadZone=(0.12,0.16)`：水平／垂直半宽。屏幕中心为 0、边缘为 ±1，即中心区域总宽 12% 屏幕、总高 16% 屏幕；区域内不纠正。
- `CameraScreenSoftZone=(0.6,0.65)`：偏差从死区边界到软区边界时，修正响应平滑增强；边界外保持最大响应。每轴必须大于死区且不超过 1。
- `CameraYawStrength=5`、`CameraPitchStrength=2`：独立轴指数响应，越大跟随越快；0 可关闭该轴持续跟随，首次获取仍会转向。自动角速度仍受下项上限限制。
- `CameraManualHold=0.15`：秒；最后一次鼠标输入之后的宽限。
- `CameraMouseSuppression=1`：0～1；1 表示输入和宽限内完全抑制辅助，0 表示不抑制。该参数只影响自动修正，不缩放鼠标输入。
- `CameraAssistRecoveryTime=0.35`：秒；宽限后平滑恢复辅助强度。
- `CameraMinTargetDistance=100`：厘米；目标与镜头 Pivot 的水平距离小于等于此值时不自动旋转，100～200cm 逐渐恢复，避免交叠翻转。
- `PitchLimits=(-55,55)`、`CameraYawSpeed=120`、`CameraPitchSpeed=60`、`CameraBlendTime=0.2`：俯仰安全范围、自动角速度与模式混合；手动角位移不受自动速度限制。
- 旧 `FramingYawLimit`、`CameraYawDeadZone`、`CameraPitchDeadZone`、`CameraRecenterSharpness` 仅保留序列化兼容，不参与算法和配置资格验证，不再用于调参。
- `CameraMode` 默认原生 `HodgeCameraMode_LockOn`；复用 PawnData 默认第三人称镜头的距离曲线、FOV 和防穿透配置。
- 正式 `IndicatorClass` 已指向 `/Game/Main/UI/Targeting/WBP_HeroLockOnIndicator`。原生 HodgeLockOnIndicatorWidget 只作为无 Designer 根控件时的回退。

### 自己调整锁定标记

1. 打开 `/Game/Main/UI/Targeting/WBP_HeroLockOnIndicator`，进入 Designer。
2. 选择根 `MarkerSize`（SizeBox），修改 Width Override／Height Override，初始为 36×36 UMG 单位。
3. 选择 `MarkerGlyph`（TextBlock），修改文字、Font Size（初始 26）、字体和 Color and Opacity。仅扩大 SizeBox 不会自动扩大字形，要同时调整 Font Size。
4. 若需要图标，将 TextBlock 换成 Image，设置 Brush／纹理和 Desired Size；保留根控件的合理期望尺寸与居中 Slot。
5. Compile／Save 后重新运行 PIE。Profile 已引用该 WBP，不需要重配 GA 或 HUD。

位置由 IndicatorLayer 投影目标 LockOnSocket 决定，不在 WBP 中写死屏幕坐标。UMG DPI 会影响实际像素大小；尺寸可在不同分辨率下预览。作者工具只在空 Designer 树时建立初始样式，重跑不会覆盖已有编辑。

这些数值是可编辑基线，自动化不代表不同 DPI／帧率的最终手感。v0.4 已移除 0.12 秒横向镜头缓存：GestureWindow 只识别切换，不延迟或吞掉镜头输入。正式 Profile.CameraMode 必须为非抽象的 LockOn 派生类。

## 3. 敌人锚点与接口

`AHodgeEnemyCharacter` 原生实现 `HodgeLockOnTargetInterface`。Guardian 蓝图继承此接口，两只正式 Guardian 已配置 `LockOnSocket=b_MF_Spine_03`，无须新增伤害逻辑。

- `bAllowPlayerLockOn`：目标能否被选择，默认 true。
- `LockOnSocket`：目标身体上的骨骼或 Socket，建议填写实际胸口骨骼。查询到有效骨骼时优先使用其世界位置。
- `bAllowLockOnBoundsFallback`：骨骼缺失时是否允许 Bounds 中心回退，默认 true；禁止回退到世界原点。
- `LockOnPriority`：0～1，用于本地候选评分，不作为服务器授权。
- `CanBeLockedBy(SourcePawn)`：目标存活、显式允许且不是自身；组件另行复用 `FHodgeDamageRules` 验证 ASC、生命和队伍。
- `GetLockPoint(LockPointId, OutLocation)`：当前默认锚点使用 `NAME_None`；非空部位 ID 返回 false，首版没有 Boss 多部位选择。
- `GetLockPriority()`：返回评分优先级。

死亡立即解除。眩晕、击倒、挑飞与普通受击保留选择，只暂让朝向。持续超距、持续遮挡、锚点失效、销毁、解绑和配置撤销属于有效性或生命周期清理，不自动获取邻居。

## 4. 程序接口与职责

`HodgeLockOnComponent`：

- `CheckDefaultInitialization()`：真实协调入口，显式进入 Spawned，事件重入时有限追加检查；DataAvailable 不等待 ASC，避免与 PawnExtension 屏障互锁。
- `IsReady()`：当前 Pawn、配置、ASC Avatar 和本 Feature GameplayReady 全部匹配。
- `ToggleLock()`、`TryLockTarget(Target, Point)`、`SwitchTarget(Direction)`：返回提交或排队结果，不代表服务器已授权。
- `ClearLock(Reason)`：用户操作通过短 GA；权威死亡和失效直接清理。
- `GetPresentationTarget()`：本地镜头和 UI 读取预测选择或确认选择。
- `GetConfirmedTarget()`、`IsConfirmedTargetValid()`：每刀起手先验证再读取；依然执行真实几何命中。
- `OnLockStateChanged(Confirmed, PresentationTarget, PresentationRevision)`：表现变化通知；重复 RPC／属性回执去重。
- `GetPresentationRevision()`：本地表现修订，包含预测／待确认变化；只按服务器 StateVersion 去重会漏掉本地预测。
- `GetConfirmedState()`、`GetLastLocalReason()`、`IsCommandPending()`：诊断与 Blueprint 消费入口。
- `SubmitLookIntent(Degrees, bAllowTargetSwitch)`：记录一次等效角位移，原始输入仍送 Controller；临时技能期间禁止切换，仅供仍在混合的锁定层同步视角。
- `Uninitialize()`：撤销自己的句柄、GE、标记和监听，不清空共享 ASC 的整类标签。

命令 GA：独立激活组、LocalPredicted、InstancedPerActor、ReplicateNo，无消耗和冷却。最多一条活动命令与一条最新待发意图。支持继承该原生 GA 的蓝图子类，但同一个 ASC 同时授予多份锁定命令 GA 会拒绝分发，避免随机选中错误能力。

TargetData 包含 Avatar、序号、操作、候选、锚点和基础版本。服务器验证后发布确认；拒绝也推进已处理序号。ASC 只对锁定数据增加退役键与旧 Avatar 校验，其他技能 TargetData 链路保留。

旋转由既有 `HodgeCharacterRotationComponent` 执行；锁定占用不经过通用朝向 RPC，服务器从确认目标派生，本地从预测表现派生。Dash 配对持有 Movement 基础占用和动作朝向；Sprint 使用已有 Movement，占用交接过程中不恢复 Controller。锁定组件只释放自己的占用。

ControlRotation 表达相机视角并保持现有 CameraMode 栈写回；CMC 独立解析锁定角色朝向，Hero.FaceRotation 防止同帧抢写。临时技能镜头优先并接管鼠标，结束后连续恢复锁定镜头。GetDesiredCameraMode／GetActiveCameraModeClass 区分期望与实际；GetBlendInfo 返回真正栈顶权重。

## 5. 验证工具

- `Tools/LockOn/configure_assets.py`：只增补正式配置，修改前备份。已有配置调参后不要盲目重跑授予配置。
- `Tools/LockOn/start_test.py`：启动独立验证地图的输入测试，`LOCK_ON_MODE=single/listen/dedicated/listen-lag`；最后一种向 PIE 网络驱动设置 100ms 延迟。
- `Tools/LockOn/run_e2e.py`：记录逐帧目标、角色模式、位置、速度、会话与错误，不修改生产怪物的行为资产。
- `Tools/LockOn/run_tracking_e2e.py`：v0.5 真实 POV 专项，覆盖中键获取、整圈受控轨迹、目标横移、鼠标接管／恢复、技能镜头和解锁；移动及 Dash/Sprint 使用 Enhanced Input。
- `Tools/LockOn/run_camera_e2e.py`：保留 v0.4 历史专项，其旧停手宽限断言不作为 v0.5 验收。
- `Tools/LockOn/run_validation_batch.py --kind tracking --work Saved/LockOnTracking`：依次运行四种 PIE 模式；`--kind lock` 运行目标／失效回归。
- `Tools/LockOn/configure_soft_camera.py`：只更新本轮正式软镜头参数并备份 Profile，不重建输入或装备。
- `Tools/LockOn/stop_test.py`：停止 PIE。
- `Tools/LockOn/restore_editor.py`：等待 PIE 结束后恢复原地图／性能设置；原始 PIE 选项未记录，明确恢复为单人独立模式。
- `Tools/LockOn/export_results.py`：从实际结果导出验收 JSON，包含源码与原始证据 SHA256；本机 Python 运行，不经过编辑器。
- 原生自动化组：`Hodge.LockOn`；源码测试中使用临时世界和作用域脚本许可，退出后恢复，不写入运行配置。

中键通过所属 PlayerController 的 InputKey 排入下一游戏帧，覆盖真实 IMC／Started 绑定；其他动作通过 EnhancedInput 注入，不等于物理鼠标／DPI 已验收。`Log LogTemp Verbose` 开启候选拒绝与评分日志，组件查询接口可查看确认版本、待确认状态和最后原因。当前没有独立评分调试面板。

历史日志在 Saved/LockOn，软镜头修正证据在 Saved/LockOnCamera，不纳入源码提交。[工具说明](../../Tools/LockOn/README.md)给出重跑步骤；[初版报告](../Validation/lock-on-implementation-2026-10-12.md)保留历史，[软镜头报告](../Validation/soft-lock-camera-2026-10-13.md)记录当前结论及未验证范围。

## 6. v0.5 镜头调参顺序

1. 先用 `CameraAcquireDuration` 调首次转向节奏，保持 `CameraBlendTime=0.2`，避免长模式混合覆盖获取节奏。`CameraAcquireSpeed` 是保护上限，不是匀速转向速度。
2. 绕目标移动，先调整水平死区，再调整软区和 `CameraYawStrength`。希望目标更靠近中心时缩小死区；希望镜头更稳定时扩大死区。
3. 用跳跃和目标高度变化检查 Pitch，优先保持较低 `CameraPitchStrength`，再调垂直死区与 `PitchLimits`。
4. 手动环绕后停手，调 `CameraManualHold` 和 `CameraAssistRecoveryTime`。`CameraMouseSuppression=1` 可消除主动输入时的自动拉扯。
5. 检查不同视口、近身、Dash 和技能镜头。自动转向每次只推进最多 0.05 秒，低帧率卡顿后不补偿一次大跳转；不是扩大角速度来掩盖性能问题。

镜头每帧在模式求值阶段读取上一帧实际避障 POV 与当前 Pivot／胸口锚点，按真实投影计算偏差；不写 Actor Rotation，不增加组件 Tick 的镜头写入。角色朝向、移动、保持距离、死亡清除和候选选择没有改变。获取仍遵循既有可见候选和视锥规则，未增加背后或屏幕外中键获取。

完整追溯见 [v0.5 验证](../Validation/soft-lock-tracking-2026-10-13.md)。物理鼠标 DPI、极端 FOV、完整狭窄地形和最终主观手感仍需人工试玩。
