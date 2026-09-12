# 相机、移动与动画

[返回首页](README.md)

## 相机实现结构

HodgeCameraComponent 持有 HodgeCameraModeStack。每次 GetCameraView 更新模式，再计算模式栈视图，将结果写入组件位置、旋转、FOV 和控制器 ControlRotation。

HodgeCameraMode 定义视图、混合与 Pivot 基础；HodgeCameraMode_ThirdPerson 扩展第三人称偏移、蹲伏相关处理和防穿透探测；PenetrationAvoidanceFeeler 保存探测配置；PlayerCameraManager 和 UICameraManagerComponent 提供更高层扩展。

CombatCharacter 已创建 CameraComponent，但 PlayerController 构造没有显式设置项目 PlayerCameraManagerClass。应区分“组件能参与引擎视图计算”与“自定义 PlayerCameraManager 已被实例化”。资产覆盖仍待验证。

## 当前关键缺口

UpdateCameraModes 只有在 DetermineCameraModeDelegate 绑定后才 Push 模式。有效源码没有绑定者，唯一草稿在已注释 HeroComponent。PawnData.DefaultCameraMode 字段已启用，但尚无有效消费入口。

CameraComponent 的相对偏移不是默认模式的替代。模式栈没有有效视图时，当前代码仍会将 CameraModeView 写回相机；默认视图位置/旋转为零，因此存在异常视角或控制旋转被覆盖的风险。实际表现需要 PIE 确认。

## 推荐模式选择规则

HeroComponent 返回技能临时覆盖模式；没有覆盖时返回 PawnData.DefaultCameraMode。覆盖应记录拥有它的 Ability SpecHandle，清除时只接受对应句柄，防止旧技能结束误清新技能设置。

该规则在草稿有表达，但 Ability 的 HeroComponent 相机桥接也有注释，启用时需要同时检查调用端和清理端。

## 移动组件

CharacterBase 在构造中通过 SetDefaultSubobjectClass 替换 CharacterMovement，为所有派生角色提供 HodgeCharacterMovementComponent。这里仍建立在 UE CharacterMovement 之上，不是 ALS 完整 locomotion 的复制。

组件提供 GroundInfo、地面距离、模拟代理加速度及与 ASC/Tag 关联的行为入口。角色侧 ASC 为空时，依赖接口查询的移动状态逻辑不会得到预期数据。

CombatCharacter 有压缩加速度和 FastSharedReplication 支持代码。函数存在不等于额外快速复制调度已接好；普通 CharacterMovement 复制与自定义优化是两层。

## 动画实例

HodgeAnimInstance::InitializeWithAbilitySystem 初始化 GameplayTagPropertyMap。NativeInitializeAnimation 尝试从 OwningActor 查询 ASC；ASC 新 Avatar 初始化也会尝试对动画实例补充初始化。因此要同时考虑动画先到与 ASC 先到。

NativeUpdateAnimation 从 HodgeCharacterMovementComponent 取得 GroundDistance。它不等于完整状态机：Idle/Run、转身、起停、空中、落地、Montage Slot 和动画层由蓝图配置决定。

Content/Main 中有 ABP_Pover_Base、动画层和敌人动画资源，但本轮只确认文件存在。Mesh Skeleton 是否匹配、AnimClass 是否赋值、曲线名称和 Slot 是否一致都待编辑器验证。

## 建议验收顺序

先确认基础相机跟随和鼠标视角，再验证移动方向随控制器 Yaw；然后查看速度/地面距离与 ABP 变量；最后验证技能 Montage、RootMotion、相机覆盖和结束恢复。先让角色位置真的移动，再排查“动画不走”，避免把输入问题当作动画问题。

源码：[CameraComponent](../../Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp)、[CameraMode](../../Source/Hodgepodge/Private/Camera/HodgeCameraMode.cpp)、[ThirdPerson](../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_ThirdPerson.cpp)、[AnimInstance](../../Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp)。
