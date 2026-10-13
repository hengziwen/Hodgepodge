# 玩家目标锁定系统设计

日期：2026-10-13。版本：0.5。适用：Hodgepodge、UE 5.5.4。

**状态：首版代码与正式配置已实施。** v0.2 确定的鼠标中键、甩动切换、TargetData、受击保留及死亡清除规则保留。v0.3 同步真实接口、Init State 屏障、Indicator 装配和测试契约；构建、自动化、PIE 与未验证项分别见[本轮验证报告](../Validation/lock-on-implementation-2026-10-12.md)。配置数值仍是可编辑的原型基线，正式美术与手感调优未完成。

**v0.4 软镜头修正：** 用户确认保留中键目标选择，仅修改镜头，不加入攻击自动索敌。取消 ±8° 硬限制与横向缓存，手动环绕优先、停手宽限后才柔和构图；ControlRotation 表达视角，锁定角色面向由 CMC 独立解析。模式切换复用原 CameraMode 栈，详见[调研与修正契约](soft-lock-camera-research-2026-10-13.md)和[新验收](../Validation/soft-lock-camera-2026-10-13.md)。v0.4 为历史基线，后文镜头条款以 v0.5 为准。

关联文档：[旋转与移动动画模式](character-facing-modes.md)、[历史旋转约束](character-rotation-policy.md)、[Dash／Sprint](dash-sprint-system.md)、[近战索敌与攻击吸附草案](melee-attack-assist.md)、[攻击配置](../Guides/attack-ability-configuration.md)、[Indicator 系统](indicator-ui-system.md)。此前旋转改造没有包含完整锁定服务和锁定 CameraMode；本文开始定义这些功能，不把此前的排除范围视为已经完成。

## 1. 背景与目标

### 1.1 背景

项目已具备 Experience、PawnData、GameFeature、GAS、角色旋转仲裁、移动意图、Dash／Sprint 和八向动画隔离。玩家需要主动选择一个敌人，持续围绕该目标移动，并为镜头、标记与技能提供一致的目标来源。

锁定状态会跨越攻击、Dash、受击和静止观察。它的持续时间不等同于单次技能，因此需要明确目标状态的所有者，避免将目标查询、摄像机、角色旋转和伤害判断集中到一个长期运行的 GA。

### 1.2 目标

- **LOCK-01 目标选择**：获取、解除和切换显式目标，多个候选时使用稳定规则。
- **LOCK-02 状态保持**：目标保持、遮挡宽限、超距和失效处理有独立规则，减少边界抖动。
- **LOCK-03 职责独立**：组件维护目标，短 GA 提交命令，旋转、镜头、动画和 UI 分别消费结果。
- **LOCK-04 移动适配**：普通锁定移动使用控制朝向与八向表现；Dash／Sprint 临时覆盖后正确恢复。
- **LOCK-05 镜头一致**：现有相机栈输出视角，角色面向独立解析；不得再以目标朝向覆盖玩家的视角输入。
- **LOCK-06 网络与身份**：客户端预测体验，服务器确认合法目标，旧 Pawn、旧请求和迟到回调不能覆盖新状态。
- **LOCK-07 生命周期**：初始化幂等，死亡、解绑、换 Pawn、目标销毁及 GameFeature 撤销都能清理自身资源。
- **LOCK-08 战斗边界**：锁定不自动开启攻击吸附，不保证空间命中，不取代伤害资格与检测体。
- **LOCK-09 可调试**：候选、拒绝原因、评分、确认状态、朝向占用和释放原因可观察、可测试。

### 1.3 首版范围

首版面向正式玩家 Pawn 与普通近战敌人：单目标锁定、主动切换、标记、普通移动适配、基本镜头跟随及服务器确认。实现仍属于现有 Hodgepodge Runtime 模块，编辑器验证沿用现有工具模块。

首版只适配鼠标／键盘，不制作手柄映射和摇杆阈值；设备归一化后的方向意图共用入口，为以后适配保留契约。首版不实现自动攻击吸附、保证命中、自动绕障、复杂空中追踪、完整 Boss 多部位系统、AI 共享仇恨服务或新增体力机制。目标锚点接口可为 Boss 多部位预留扩展，不提前建立完整框架。

## 2. 核心概念与现有基础

### 2.1 概念

- **显式锁定目标**：玩家主动选择并由服务器确认的目标，供持续交互使用。
- **预测目标**：客户端已选出、尚未收到服务器确认的目标，可以提前显示反馈，不是可信伤害结果。
- **锁定锚点**：敌人胸口等稳定位置；角色方向、镜头构图和标记可以使用不同偏移。
- **本次攻击目标**：某次 GA 根据技能规则验证并保存的执行上下文，不等同于实时锁定目标。
- **基础朝向**：普通移动时的 Movement 或 Controller 驱动。
- **动作朝向**：攻击、Dash 等临时方向覆盖；结束后恢复当前基础驱动。
- **Yaw 冻结**：固定某个角度的约束，与持续追踪敌人的锁定不同。
- **表现暂让**：锁定目标仍保留，朝向或镜头控制暂时让给动作、Sprint、受击或演出。

GA 是能力执行单元，不是 UE 的 C++ Module。“独立组件”与“完整锁定功能”可以同时成立，不能将模块化理解为必须新建模块或把所有逻辑装进一个 GA。

### 2.2 源码已确认的基础

- [HodgeCharacterRotationComponent](../../Source/Hodgepodge/Private/Component/HodgeCharacterRotationComponent.cpp) 提供 AcquireBaseFacingMode、RequestActionFacing 和 ReleaseFacingRequest；Controller 基础驱动映射 ReservedStrafe，同层请求目前按申请先后选择。
- [HodgeAnimInstance](../../Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp) 从朝向快照选择侧移表现并控制 Pivot 资格。保留八向分支不代表资源、混合和正式锁定场景已经验收。
- [Dash](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Dash.cpp) 请求 ActionDirection；[Sprint](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.cpp) 请求 Movement 基础驱动。
- [HeroComponent](../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp) 接入输入、PawnData 与 CameraMode 选择；技能临时镜头优先于默认镜头。
- [CameraComponent](../../Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp) 将 CameraModeView.ControlRotation 写回 PlayerController。GetTargetActor 默认返回相机所属 Pawn，不是敌人目标接口。
- HodgeCameraAssistInterface 当前提供防穿透协作，不能直接承担锁敌服务。
- HodgeIndicatorManagerComponent 已有 AddIndicator／RemoveIndicator；v0.3 已在 AHodgePlayerController 装配原生 Manager，并在 WBP_HodgeHUD 增补 IndicatorLayer。

v0.1／v0.2 评估时原生源码没有完整锁定服务；v0.3 已新增组件、短命令 GA、TargetData、Profile、目标接口与 CameraMode。没有逐个导出所有蓝图试验；正式资产与操作路径见[配置手册](../Guides/lock-on-configuration.md)。

## 3. 方案对比与关键决策

### 3.1 独立组件承载核心

适用于锁定跨越多次技能、多个消费者需要读取同一目标、目标保持不应依赖某次攻击是否结束的情况。

优点是状态来源明确，镜头、UI 与攻击解耦，搜索和切换容易单独测试，可沿用 Pawn 生命周期与 Experience 注入。代价是需要明确实现权限、复制、预测纠正、资源释放和初始化条件；可复制组件本身不会自动提供这些规则。

### 3.2 完整功能由长期 GA 承载

适用于锁定本身就是持续瞄准、蓄力选取、资源消耗、限时标记等技能。GA 能利用 GAS 的激活标签、消耗、任务、取消与预测机制，长期运行在技术上可行。

代价是重复按键需要路由到活动能力，不能直接依赖重新激活；目标失效、换 Pawn、镜头与 UI 清理都容易与能力结束绑定；消费者若查找具体活动 GA，会产生类型和生命周期耦合。错误的取消关系可能在 Dash 或攻击切换时清掉仍应保留的目标。

### 3.3 本项目决定

采用 **Pawn 级锁定组件为核心，短生命周期 GA 为命令入口**。一次输入 GA 结束不解除锁定，镜头和标记不依赖它仍处于 Active。以后特殊瞄准或标记技能可单独使用长期 GA，复用同一目标契约。

锁定 GA 的能力标签独立于攻击与移动标签，避免被现有攻击取消关系误伤。资格检查主要限制新获取和切换，清理与解除不能依赖成功激活一个被死亡／控制标签阻止的 GA。

## 4. 系统架构与职责

### 4.1 新增 UHodgeLockOnComponent

依附当前玩家 Pawn，作为锁定状态唯一协调入口。负责候选查询与评分、请求身份、预测及确认目标、保持规则、失效处理和状态变更事件；持有自己的旋转请求、委托与状态标签句柄。

组件不直接设置 Actor Rotation、移动组件旋转开关或每刀伤害；不创建 Widget，不将目标搜索塞入 CameraMode，不维护一份与旋转组件平行的仲裁器。

### 4.2 新增 UHodgeGameplayAbility_LockOn

接入现有 GAS 输入链路，检查组件就绪与获取资格，提交锁定或切换命令，在服务器处理或超时清理后结束。可以复用同一命令入口处理不同输入，不为了两个切换方向立即拆成多份 GA。

首版确定使用 LocalPredicted＋GAS TargetData，不新增组件的锁定命令 RPC。GA 采用项目默认 InstancedPerActor、ReplicateNo 和非互斥激活组；一次激活处理一条命令，不承担持续锁定生命周期。服务器收到、校验并处理 TargetData 后结束命令；客户端不能在发送后立即向服务器复制 EndAbility，提前关闭尚未消费数据的激活。

GAS 复用激活 PredictionKey、ScopedPredictionWindow、TargetData 委托与缓存传输；服务器确认后 LockOnComponent 保存长期状态。能力结束或 PredictionKey 被接受不等同于目标资格被接受，不能认为 GAS 会自动预测／回滚任意组件字段、UI 或旋转句柄。组件仍按请求序号维护轻量的本地待确认表现与纠正，不建立另一套通用预测传输框架。

沿用 InstancedPerActor 时，活动命令不能再次直接激活。组件最多保留一条最新待发意图，解除意图可替换待发获取／切换；本地反馈可以立即更新，但下一条 TargetData 使用新的 GA 激活与 PredictionKey。GA 结束后发出待发命令，禁止复用已结束的激活缓存。处理超时基线为 1.5 秒，可配置；超时只结束该命令并恢复对应预测表现，不解除此前仍有效的确认目标。

### 4.3 新增 UHodgeLockOnProfile

保存获取与保持距离、候选视角、评分权重、遮挡宽限、切换阈值、检查频率及表现策略。由 PawnData.LockOnProfile 引用，正式资产为 /Game/Main/Character/Hero/Targeting/DA_HeroLockOn，具体字段见配置手册。

### 4.4 新增目标接口

UHodgeLockOnTargetInterface／IHodgeLockOnTargetInterface 提供目标是否允许锁定、锚点与优先级。普通怪首版使用一个稳定锚点；预留锚点 ID，以后扩展 Boss 多部位。服务器使用目标契约与 GAS／队伍状态判定资格，不仅按某个蓝图类名筛选。

### 4.5 现有消费者

- RotationComponent：按句柄仲裁并执行基础／动作朝向，不负责选敌。
- CMC 与 AnimInstance：消费已解析驱动和风格，保持移动预测与动画线程的数据边界。
- 新增锁定 CameraMode：跟随目标并输出构图，复用已有镜头混合与防穿透。
- HeroComponent：输入就绪与 CameraMode 选择；不保存第二份权威目标。
- Indicator 系统：注册／移除本地玩家标记，显示预测或确认状态。
- CombatComponent＋攻击 GA：按技能规则读取目标，写入本次执行上下文并继续真实几何检测。

主数据流：

```text
输入 → LockOnComponent 选择／预测 → 短 GA 提交 TargetData
                     ↓ 服务器校验与结果确认
                 确认锁定状态
                     ↓
       旋转仲裁 · 镜头模式 · 标记 · 技能执行上下文
```

## 5. 接口与数据结构（实现规格）

### 5.1 组件接口

- ToggleLock()：无目标时请求获取，有目标时请求解除；本地立即反馈，权威结果异步确认。
- TryLockTarget(Target, LockPointId)：提交候选，返回是否成功提交，不以这个返回值冒充服务器已接受。
- SwitchTarget(Direction)：依据本地屏幕方向与当前目标请求切换；不要在多个候选间自动来回切换。
- ClearLock(Reason)：解除选择；死亡／解绑强制清理不依赖输入 GA。
- GetPresentationTarget()：供本地镜头和 UI 读取预测或确认目标。
- GetConfirmedTarget()：读取确认目标；攻击仍须按自身规则验证，客户端返回值不能直接决定伤害。
- OnLockStateChanged(Confirmed, PresentationTarget, PresentationRevision)：驱动消费者同步；订阅者先绑定事件再读快照。确认状态按 StateVersion 去重，预测表现按 GetPresentationRevision() 去重，拒绝原因从确认状态或 GetLastLocalReason() 查询。
- CheckDefaultInitialization()／Uninitialize()：沿用 PawnExtension 与 Init State；显式建立 Spawned，事件重入有限补查，不手动从 GameplayReady 回退。

目标接口拟提供 CanBeLockedBy(SourcePawn)、GetLockPoint(LockPointId, OutLocation)、GetLockPriority()。获取位置失败必须报告无效，不能回退到世界原点；首版可显式配置 Actor Bounds 中心作为锚点回退。

### 5.2 FHodgeLockOnTarget 与状态

本地目标引用包含 TargetActor 弱引用、LockPointId、最近一次有效锚点位置与采样时间。复制快照另行使用受 UE 支持的 Actor 引用序列化；不假设弱引用字段会自动复制。

FHodgeLockOnState 包含确认目标 Target、bHasTarget、LockPointId、StateVersion、RequestSequence、Reason 与 bAccepted。bHasTarget 区分服务器解除与目标引用尚未映射。Unlocked／Locked 表示目标选择事实；Pending／Rejected 是请求结果。朝向暂让和镜头暂让是表现策略，不能据此把有效目标误清为空。

本地预测保存自己的请求序号、候选和提交时间，与确认快照分开。组件使用当前 Pawn 初始化代使本地旧回调失效，不把客户端和服务器各自生成的初始化代假定为相同值。

### 5.3 请求与结果

FHodgeLockOnTargetData 派生自 FGameplayAbilityTargetData，使用 FGameplayAbilityTargetDataHandle 承载一条请求。载荷包含 SourceAvatar、RequestSequence、操作类型 Acquire／Switch／Clear、CandidateActor、LockPointId 及 BaseStateVersion。Actor 引用使用 PackageMap 支持的网络序列化，服务器限制载荷类型、数量、操作、名称和合法范围，不信任任意世界坐标或客户端分数。

鼠标中键的 Toggle 在拥有端根据当前选择解析为显式 Acquire 或 Clear，线上不传 Toggle，避免两端状态不同而执行相反动作。结果快照包含该连接已处理序号、Accepted／Rejected、拒绝原因和最新确认状态；拒绝也必须确认该序号，即使目标没变化。GAS 激活本身被拒绝、尚未启动服务器命令时，则通过该激活的拒绝／失败回调纠正对应本地表现。

SourceAvatar 必须是连接当前拥有的 Pawn。RequestSequence 在当前 Pawn 请求链中单调增加，用于拒绝重复和旧请求；序号不是客户端的授权凭证。若需跨 Possess 周期令牌，应由服务器生成并下发，不复用本地任意 GUID。

### 5.4 配置参数

- AcquireDistance／KeepDistance：获取与保持分别配置，通常保持距离略大，以形成滞回。
- AcquireHalfAngle、距离／屏幕中心／目标优先级权重：评分分量归一化后组合。
- OcclusionGrace、KeepCheckInterval：短遮挡与持续遮挡分开处理。
- SwitchAngle／SwitchSpeed／GestureWindow、SwitchDebounce：降低输入噪声造成的连续切换。
- CameraYawSpeed／PitchLimits／构图偏移：属于本地镜头调参，不作为可信伤害输入。
- 失效是否自动重新获取、Dash／Sprint 时镜头强度：作为明确策略，不在代码中隐式决定。

按键、原型基线和标签已实施，Guardian 使用 b_MF_Spine_03 胸口锚点。标记美术与最终阈值仍需调优；Profile 真实字段以源码与配置手册为准。

## 6. 关键流程

### 6.1 获取与确认

输入 → 组件就绪及本地资格检查 → 空间查询候选 → 过滤敌对、存活、可锁定、距离和视角 → 稳定评分 → 本地预测标记与镜头 → LocalPredicted GA 提交 TargetData → 服务器校验 → 发布确认快照或拒绝。拒绝更晚的请求时恢复最新确认状态；旧确认不能覆盖更晚的本地意图。

拒绝旧请求时不能无条件清掉较新的预测目标。确认后需要目标实际引用可用再绑定监听；复制引用晚到时保持等待，不将尚未解析等同于目标已经销毁。

### 6.2 保持与切换

目标位置可每帧供镜头和朝向读取；候选搜索、LOS 与保持资格检查按较低频率执行，具体频率依据测试确定。无锁定时不每帧全世界扫描，优先用范围查询及目标资格接口。

切换按屏幕相对方向选择候选，评分接近时使用稳定的最终比较规则；目标移动或跨屏幕中线不等于用户发起切换。查询无合适目标时保留当前有效目标，或按显式策略解除。

### 6.3 解除与失效

主动解除、持续超距、超过遮挡宽限、目标死亡／销毁／锚点失效、Pawn 死亡／解绑及配置撤销都会进入对应清理路径。默认不悄悄换到旁边怪物；自动重新获取需单独启用并测试。

普通受击、眩晕、击倒与挑飞不因控制标签自动解除目标，动作期间只暂让相关表现；首版战斗事件中的强制解除条件为玩家死亡和目标死亡。目标销毁、不可锁定、超距、长期遮挡和 Pawn 生命周期失效仍需清理，这是目标有效性约束，不是新增的受击解除规则。

组件撤销自己的旋转句柄、委托、标记和状态 GE／标签来源。不能直接将 ASC 的整类 TagCount 清零，也不能恢复一组历史旋转开关覆盖其他动作的当前策略。

### 6.4 普通移动、Dash 与 Sprint

普通锁定时申请 Controller 基础朝向，通过现有仲裁得到控制器 Yaw 与 ReservedStrafe；不要设置 Status.Rotation.Locked 来跟踪敌人。

Dash 保留锁定目标，动作方向仍由原移动意图与 F／B 选择控制，不变成朝敌人强制突进。现有 ActionDirection 只覆盖朝向，Style 仍取决于 BaseDriver；本次实现需由 Dash 配对申请／释放 Movement 临时基础策略，使 Dash 期间使用 FreeDirectional，不能认为已有代码自动完成了这个转换。

Sprint 保留目标，临时使用 Movement 和自由移动／Pivot，结束后恢复锁定。首版决定：Dash／Sprint 活动期间锁定组件不持有 Controller 基础占用，已有占用暂停并配对释放；Dash 临时申请 Movement 基础风格和自身 ActionDirection，Sprint 使用已有 Movement 占用。结束后只有目标仍有效且没有其他运动覆盖，才重新申请 Controller。

长按 Dash→Sprint 视为一个连续暂让区间，交接中不短暂恢复 Controller／ReservedStrafe。以实际运动会话、激活键与朝向句柄判断结束，不用固定延迟恢复；旧 Dash 结束回调不能撤销 Sprint 占用。普通受击控制期间按实际动作约束暂让朝向，目标保留。当前同层请求按“后申请者生效”，不依赖它假装已有通用优先级；上述显式暂停／恢复负责首版协调。

切换目标只更新选择，不重复获取基础朝向句柄，避免抢走 Sprint 请求。锁定组件不得每帧重申请 Controller。需要更一般的优先级时，再扩展现有仲裁，而不是增加第二个旋转管理器。

### 6.5 镜头与角色朝向

角色地面朝向使用 Pawn 到目标的水平向量；镜头允许自由环绕，输入期间按配置抑制辅助，停手后按胸口屏幕偏差柔和辅助。ControlRotation 保持相机视角语义，不再代表精确面敌角度；CMC 从锁定目标解析角色 Yaw。

CameraComponent 按既有流程将混合视图的 ControlRotation 写回，移除 PlayerController 的锁定目标旋转覆盖。Hero.FaceRotation 避免同帧按镜头提前旋转锁定角色，CMC 继续消费确认目标／动作约束。锁定组件不直接 SetControlRotation；专服目标面向不依赖本地相机求值。技能临时模式期间，鼠标不被锁定组件截取。

CameraMode 选择顺序确定为技能临时镜头 → 有效锁定镜头 → PawnData 默认镜头。短 GA 不长期占用 SetAbilityCameraMode；技能镜头结束后重新查询当前锁定状态。Dash／Sprint 保留镜头目标，暂让角色朝向；镜头跟随不强行改变动作朝向或位移。

移动按玩家视角 ControlYaw 生成世界方向快照，锁定角色仍面敌，因此表现可以侧移／倒退；不再假设 W 永远朝敌人。Dash 起手保持既有世界方向快照，不在动作中用后续镜头改向。默认距离、FOV 与避障沿用原配置，混合基线 0.2 秒；自动构图最大 Yaw／Pitch 速度为 120°／秒、60°／秒，手动角位移不受这两个自动速度上限限制。

### 6.6 攻击读取

每刀起手读取并验证目标，按技能规则保存到本次执行上下文；切换锁定不默认修改正在执行的刀。需要实时追踪的技能显式配置，不能所有攻击都实时读全局目标。

普通剑和翅膀仍按真实检测体查询，允许目标躲开，允许范围攻击命中其他合法目标。LockedTargetInVolume 只有技能明确要求时才作为过滤策略。保留当前攻击吸附关闭状态，锁定不会自动启用该设计草案。

## 7. 网络、初始化与资源约束

客户端负责本地镜头相关的候选选择、预测标记与表现，服务器负责确认敌对关系、存活、可锁定、距离、锚点及请求身份。服务器不以客户端上报的屏幕评分作为授权，也不需要专服拥有玩家镜头渲染结果。

GAS 上行链路沿用项目 MovementAction 已使用的模式：绑定 AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)，拥有端在 FScopedPredictionWindow 内发送 TargetData；服务器注册后调用 CallReplicatedTargetDataDelegatesIfSet 补消费先到数据，复制必要字段后 ConsumeClientReplicatedTargetData。结束、取消、超时和解绑清理委托与对应激活缓存。服务器接收目标数据不代表接受目标，必须再次执行资格校验。

现有基础朝向 RPC 校验合法驱动和 Pawn 身份，不等于锁定目标已通过校验。首版选择由 LockOnComponent 的确认／预测状态在各端派生锁定朝向占用：锁定来源的 Controller 请求增加专用约束，服务器只在确认目标有效且版本匹配时接受，本地预测请求只在本地生效，不另外通过普通旋转 RPC 授权锁定。其他技能现有旋转 RPC 保持其职责。由此同一锁定命令只经 TargetData 确认一次，拒绝时按序号释放自己的本地句柄。

拥有者可预测；服务器拒绝时只回收该预测请求拥有的资源。模拟代理消费确认状态、移动复制和动画快照，不独立重新选敌或用 SetActorRotation 抵消网络差值。目标 Actor／锚点 ID／版本按需要复制，候选列表、分数、Widget、镜头参数和每帧坐标不逐帧广播。

玩家 ASC 属于 PlayerState，锁定选择属于当前 Pawn。组件的 DataAvailable 只声明 PawnData 可用，不等待 ASC，否则会阻塞 PawnExtension 的全 Feature 屏障；DataInitialized 才检查 ASC 当前 Avatar 与旋转依赖。本地输入还需等待输入绑定就绪。服务器无需本地输入设备或 CameraMode 才能确认目标，观察者无需本地 AbilitySpec 才能展示状态。

GameplayReady 表示初始化完成，不能因 Dash、受击或镜头暂让而回退 Init State。初始化通知采用查询＋事件补偿，重复绑定和清理幂等。换 Pawn 时断开旧目标委托、清掉自己持有的 GE／标签与句柄，不影响新 Pawn 的锁定来源。

运行状态 UObject 引用考虑 GC，目标使用弱引用，配置资源由受 GC 管理的字段持有。动画线程只读既有模式快照，不在动画更新中查目标、调用 GA 或修改控制器。

## 8. 本轮确定的行为、参数与剩余验证

### 8.1 输入（用户确定）

鼠标中键按下获取／解除，松开不解除；不要求按住中键。已锁定时，大幅且快速向左／右移动鼠标切换目标。不是鼠标位置越过屏幕某条线，也不是每个 Mouse Delta 都切换。

鼠标视角与切换共用一次采样后的语义入口 SubmitLookIntent，首版从现有 Input_LookMouse 接入，禁用菜单／光标交互状态下的游戏切换。不重复绑定两个消费者各自累计同一份 Delta，不覆盖现有自由视角逻辑；手柄适配暂不实现。

原型基线：在 0.12 秒滚动窗口内，累计水平等效角位移达到 18°，平均速度达到 150°/秒，方向一致率达到 0.8，才生成一次切换。等效角位移使用与现有鼠标视角相同的灵敏度换算，不将像素值直接当作角度；不同 DPI／灵敏度仍需验收。方向一致率为净水平位移绝对值除以水平位移绝对值之和。

触发后清空手势累计，0.25 秒防连发，最近 0.05 秒绝对位移不超过 3° 后重新武装；一次持续甩动最多切一次。水平相机输入立即允许完整环绕，Pitch 保留 −55°～55°。手势只提交切换，不吞镜头输入；可通过 bEnableMouseFlickSwitch 单独关闭。获取必须在屏幕内，方向切换允许前方获取视锥内的屏幕外邻居，没有合适邻居保留当前目标。

首次锁定／目标切换进入 LockAcquire；从实际 POV 开始，默认 0.35 秒缓入缓出瞄准胸口。随后 LockTracking 依据屏幕 Dead Zone／Soft Zone 分轴柔和修正，区域内稳定，偏差增大时响应增强。玩家输入立即生效，默认输入及停手 0.15 秒内完全抑制辅助，再用 0.35 秒恢复辅助；输入可以立即中止获取。bEnableSoftCameraAssist 可独立关闭，不清目标。参数详见第 11 节及配置手册；GestureWindow 只识别切换。

### 8.2 TargetData 与确认（用户选择协议，具体契约由本轮确定）

采用第 4.2／5.3／7 节的 LocalPredicted、单次命令 GA、TargetData 和组件长期状态组合；不采用组件命令 RPC。使用 GAS 的预测键与缓存传输，不自建激活预测键。组件的待确认目标、UI、旋转句柄及持久选择仍须明确确认与拒绝恢复，不能依靠 PredictionKey 自动纠正任意业务字段。

服务器处理结果按 RequestSequence 与 StateVersion 确认。对有效来源的新请求，无论接受或业务拒绝都推进已处理序号；非法来源不修改其他 Pawn 的确认状态。Actor 引用晚到先等待解析，基线超时 1 秒后撤销对应预测；旧确认、拒绝或超时只处理对应请求，不能清掉较新的选择。GA 激活被 GAS 拒绝时同时取消该键关联的本地预测资源。

### 8.3 目标资格与失效（本轮决定）

原型基线：获取半径 1500 cm、保持半径 2000 cm、获取视角半角 60°；保持检查 10 Hz。评分使用归一化的屏幕中心、距离和目标优先级，权重基线分别 0.55／0.30／0.15，最后按稳定身份打破并列。服务器不用屏幕评分授权，但检查距离、敌对关系、存活、目标契约和锚点。

玩家／目标死亡立即解除；目标销毁、Pawn 更换、解绑或配置撤销立即清理。超过保持距离持续 0.35 秒、LOS 遮挡持续 1 秒后解除，期间不重新获取其他目标；短暂恢复资格则重置计时。目标离开画面不单独解除，镜头仍可跟随。客户端可以先隐藏已知无效的标记，最终状态以服务器为准。

新获取要求合理视线；权威 LOS 从角色视点到目标锚点检查，忽略自身及目标必要碰撞，专服不伪造相机视线。首版所有失效默认只解除，不自动重新锁到旁边敌人。

### 8.4 受击、运动与镜头（用户限制及本轮决定）

用户确认首版先做死亡清除：普通受击和控制不清除持续选择；死亡之外的有效性／生命周期清理仍按第 8.3 节执行。受击能否旋转继续服从现有动作约束，不因为目标保留就覆盖控制 GA。

Dash／Sprint 暂停锁定朝向占用，保持目标、标记和镜头跟随。Dash 临时 Movement 风格＋ActionDirection，Sprint 使用 Movement；交接期间连续暂让。普通移动恢复 Controller＋ReservedStrafe；结束旧动作只释放该动作自己的句柄，不抢占仍活动的其他动作。

ControlRotation 表达实际视角，由 CameraComponent 在栈混合后同步；角色目标面向仍由 CMC／旋转仲裁独立执行，运动覆盖时按已有运动策略。冻结 Yaw 只服从已有合法约束，不用它实现锁敌。技能临时镜头优先，结束后回到仍有效的锁定镜头。

### 8.5 标签与来源管理（本轮决定）

新增 InputTag.Ability.LockOn、Ability.Type.Targeting.LockOn、State.Targeting.LockedOn，全部独立于攻击和移动取消分支。切换方向作为统一命令参数，不立即再建立两份 GA 或两套持续状态标签。短 GA 使用非互斥组，无冷却与资源消耗。

State.Targeting.LockedOn 由服务器在确认目标存在时通过无限时长 GE_LockOnState 授予，LockOnComponent 保存自己的 ActiveGameplayEffectHandle；选择切换不重复叠加 GE，解除／死亡／解绑移除自己的句柄。GA 结束不移除该 GE，Dash／Sprint 暂让不移除该标签。客户端待确认体验使用本地表现快照，不额外手工修改 ASC TagCount；如果以后需要预测 GE，应单独按 GAS 规则评审，不能重复授予。

### 8.6 剩余资产与原型验证

第 1～8 项主要机制已确定，不再作为待用户决策的大方向。普通怪胸口锚点、Indicator、Controller 请求配置已装配；八向资源仍需更广泛动作组合检查，并实测鼠标灵敏度／DPI、镜头构图、阈值及各种帧率。首版优先单个胸口锚点；缺少有效骨骼锚点时只能使用配置允许的 Bounds 中心回退，不回退世界原点。

本节数值全部是可配置的原型起点，非已验证的最终手感。没有因此启用攻击吸附、体力或手柄适配，正式配置已引用真实新增字段。

## 9. 后续实施计划与验收

1. **P0：契约与原型。** 按第 8 节中键、鼠标甩动、资格与失效规则制作单目标原型；确认目标选择能跨多次攻击保持且 GA 结束不清目标。
2. **P0：组件生命周期。** 接 Experience／PawnData／Init State，实现查询、保持、切换、事件与清理；覆盖重复调用、缺少配置、死亡和换 Pawn。
3. **P0：服务器确认。** 实现 GAS TargetData、确认序号和受控旋转占用，先验收 Listen Server；覆盖数据早到、非法目标、超距、重复数据、旧激活、旧 Pawn、引用晚到、连续命令和拥有者拒绝恢复。
4. **P1：旋转与动画。** 接 Controller／ReservedStrafe，并验证锁定前后进入 Dash／Sprint、取消、受击、动作结束恢复；不额外冻结 Yaw。
5. **P1：镜头与标记。** 接统一控制旋转、CameraMode 选择和 Indicator；验证双方移动、近远距离、上下高度、遮挡、防穿透与临时技能镜头。
6. **P1：技能读取与回归。** 每刀执行上下文读取确认目标，保持真实命中；验证锁定存在但攻击范围不足时不自动吸附、不保证扣血。
7. **后续：扩展。** 在闭环完成后考虑多锁点、特殊标记／瞄准 GA、空中追踪和攻击吸附，分别评审。

单元测试对应 LOCK-01/02/06：过滤、评分并列、切换阈值、滞回、版本与旧请求拒绝。集成测试对应 LOCK-03/04/05/07：GA 短生命周期、组件清理、句柄所有权、模式恢复、镜头写入及标记注册。端到端对应 LOCK-01～09：真实输入下获取／解除／切换、目标死亡、遮挡边界、两客户端复制、Dash／Sprint 暂让及恢复、技能镜头与实际空间命中。

需要记录预期、实际、网络模式和证据；C++ 按项目约定做常规 Editor 与 Game 构建，蓝图、PIE、专服 PIE、弱网及 Cook／打包分别列出执行范围。专服不得依赖 UI 或本地摄像机完成目标确认；专服 PIE 成功不等于已有独立 Server Target。实际执行结果独立记录在验证报告，不把 PIE 专服与独立 Server Target 或打包混为一谈。


## 10. v0.3 实现补充与明确限制

- `AHodgeHeroCharacter` 原生持有 LockOnComponent；没有 Profile 的 Pawn 不启用锁定，但仍推进其可选 Feature 状态，不阻塞 PawnExtension。
- `AHodgePlayerController` 原生持有不复制的 IndicatorManager。现有 Controller 不具备 ModularGameplay Receiver 注册，本次采用默认子对象，避免为 UI 重构控制器生命周期。
- `OnLockStateChanged` 以本地 PresentationRevision 区分预测变化；相同 RPC／属性确认不会重复播报同一表现状态。
- 同一 ASC 只允许一份 LockOn 命令 GA，支持其蓝图子类；重复授予不随机选取。
- ASC 对锁定数据拒绝退役激活及旧 Avatar；缓存退役键有界，其他能力保留原 TargetData 路径。
- v0.3 的 GestureWindow 镜头缓存已在 v0.4 移除；手动输入在相机求值帧消费，物理设备延迟仍需独立验证。
- 标记采用可替换的原生菱形 Widget，正式美术、预测状态配色及切换反馈属于后续工作。
- 不实现手柄、自动攻击吸附、自动重锁、Boss 多部位及复杂瞄准能力。它们不作为本次首版的完成标准。

## 11. v0.4 模式连续性与诊断

- CameraModeStack 新增 OnBecomeTop：在判断混合时间前准备动态 Profile，并从上一帧实际混合输出种子化；栈内再次晋升也会调用，目标切换本身不重建模式。
- GetBlendInfo 修正为读取索引零的真正栈顶；CameraComponent.GetActiveCameraModeClass 与 Hero.GetDesiredCameraMode 分别提供实际／期望类型。
- 临时技能模式高于锁定模式；结束覆盖、解除目标和死亡清理保持当前画面起点，不强制回到角色或敌人的历史角度。原始鼠标输入仍送 Controller，混合中的锁定层观察同一帧角位移而不吞输入，避免权重削弱手感；返回时丢弃已体现在实际画面中的历史采样。
- 新增 SoftCameraManualAndAssist、CameraModePriorityAndContinuity 自动化，并读取真实 POV 的 CAM-01～07 PIE 断言；测试不等于最终美术、全部狭窄地形或物理鼠标 DPI 已验收。

## 11. v0.5 获取与持续跟随镜头

### 11.1 差距与最小改动

旧实现没有明确获取阶段，Seed 被计作鼠标输入，首次锁定需等宽限；构图以角色与目标中点为目标，角度死区不对应实际视口。本版沿用现有 CameraMode、第三人称曲线／FOV／防穿透和模式栈，仅替换本地构图算法。没有增加 SpringArm、控制器 Tick 或第二个镜头旋转写入者。

### 11.2 模块和接口

- `HodgeCameraMode_LockOn`：种子取上一帧实际混合 POV；比较弱目标身份决定是否获取；生成当前 Pivot、实际相机局部偏移、胸口锚点及真实投影参数。
- `FHodgeLockCameraFrame`：CameraLocation、PivotLocation、TargetLocation、ViewRotation、HorizontalFOV、AspectRatio；本地几何数据，不复制。
- `FHodgeSoftLockCamera`：`Seed` 保留同帧输入并丢弃旧隐藏输入；`BeginAcquire` 开始首次转向；`AddLook` 采样；`Update(Frame, Delta, Now, Profile, bAllowAssist)` 只返回视角；`IsAcquiring` 用于测试。控制器和角色不在此处修改。
- `HodgeLockOnComponent`：提供目标锚点、同帧输入、UI／输入资格；`UpdateCameraView` 转发纯算法。原 `GetCameraAim` 中点取景已移除，其为非反射内部接口，无蓝图节点迁移。
- `HodgeCameraComponent`：依旧是混合视角和同步 ControlRotation 的唯一项目输出入口。Hero 决定默认／锁定／技能模式优先级，未修改。
- `HodgeLockOnProfile`：获取节奏、屏幕区域、分轴响应、速度、鼠标抑制、恢复时间及近距保护；具体字段和基线见配置手册。旧角度字段只留序列化，不再验证或执行。

### 11.3 流程与异常

成功预测目标 → 选择 LockOn CameraMode → 从实际画面 Seed → 新目标 BeginAcquire → 五次 SmoothStep 缓入缓出获取 → LockTracking 屏幕投影 → 轴独立指数响应 → 既有 ThirdPerson 防穿透 → 模式栈混合 → CameraComponent 输出。

同一目标的确认回包不会重启获取。切换目标在当前画面开始新获取。临时技能镜头高于锁定镜头；旧锁定层在混合期间仅消费鼠标，不自动构图；返回时继承实际视角，丢弃已经体现于画面的旧输入，不重复获取同一目标。取消或目标失效后旧层按当前 ControlRotation 退出，保留原自由镜头控制。

玩家输入中止获取，原始鼠标角位移不受辅助速度限制；可配置抑制系数只影响辅助。停手恢复用平滑曲线，不在某帧突然开满跟随。屏幕死区中心为 0、边缘 ±1，软区越界后响应饱和；Pitch 独立且保持安全范围。

目标与 Pivot 的水平距离 ≤100cm 时中止获取并关闭自动转向，100～200cm 渐增；相机至目标接近零、无效锚点或非法几何时保持安全视角。自动每步最多推进 0.05 秒并限速，防止卡顿后补偿式跳转。获取因保护上限未在指定时间居中时转入柔和跟随，不突然 Snap。角色面向／移动约束、目标死亡和超距规则均沿用现有实现。

### 11.4 验收和边界

SOFT-01：背向目标的角色中键获取，镜头连续转向，结束后目标在中心附近；SOFT-02：整圈绕行柔和跟随；SOFT-03：目标横移有修正且不剧烈；SOFT-04：鼠标优先、停手渐恢复；SOFT-05：取消无跳变；SOFT-06：技能模式接管／返回；SOFT-07：重复获取、真实移动和 Dash/Sprint 兼容。

原生测试覆盖缓入缓出、轨道偏移、屏幕死区、宽高比、跨 ±180°、近距、非法输入、帧率、速度保护、模式栈和配置校验。四种 PIE 模式记录实际 POV；控轨迹不等于全部物理移动或地形通过，网络模式不等于独立 Server 可执行文件验证。完整结果见 [验证报告](../Validation/soft-lock-tracking-2026-10-13.md)。

不扩展候选选择规则：镜头完全背向屏幕外敌人时，既有获取过滤可能不选择该敌人。本次验收把角色朝向与镜头朝向分开，未加入背后自动索敌、攻击吸附、手柄、Boss 多锁定点或敌群自动构图。
