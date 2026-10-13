# 攻击限定翅膀表现系统设计

版本：1.2。日期：2026-10-12。适用：Hodgepodge、UE 5.5.4。

## 1. 背景

新 Anzhu 翅膀已导入 `/Game/Wuwa/Wing`，拥有独立的 139 骨骼骨架和攻击动画。`/Game/Main/WingText` 已校准角色 `AM_Attack04` 与翼动画 `B_Attack04` 的挂接，但初始配置为持续可见、循环播放。参考姿势的翼根位置与攻击姿势不同，因此不显示常态参考姿势，翅膀仅在指定攻击表现窗口出现。

旧 `HodgeWingComponent`、`HodgeWingProfile` 和 `HodgeWingAnimInstance` 是三段骨骼旋转的程序化方案；旧 Experience 注入保持禁用。本次扩展现有组件，不新增业务 Runtime 模块，不重复实现 GAS、角色旋转或伤害系统。

## 2. 目标及可追溯需求

- **WING-01 初始与空闲隐藏**：创建或复用翼网格后立即隐藏，空闲不求值翼骨骼。
- **WING-02 攻击白名单**：只有配置映射的角色动画可以驱动翅膀；首版配置 AM_Attack04/B_Attack04，其他攻击默认隐藏。
- **WING-03 单一窗口事实来源**：角色动画的 `HodgeAnimNotifyState_WingAction` 与 WindowId 定义半开区间 `[Begin, End)`，重叠窗口取并集。
- **WING-04 主时钟同步**：翼动画按角色源秒映射求值，支持变速、暂停、时间跳转和从窗口中间进入；不独立累积时间，不循环翼动作。
- **WING-05 姿势先于显现**：设置并求值当前正确翼姿势后才允许显示，避免参考姿势闪现。
- **WING-06 身份与清理**：会话以源实例及执行身份区分，重复开始幂等，旧句柄不能结束新会话；正常结束和取消淡出后隐藏，死亡、解绑及销毁立即隐藏。
- **WING-07 挂接与兼容**：正式角色复用 WingText 校准到 WingCase 的变换及 0.8 统一缩放；旧程序化模式继续使用旧校验，新动画模式不要求旧骨骼或旧 AnimBP。
- **WING-08 失败关闭**：资源、骨架、时间、窗口或挂点非法时不显示，并输出具体诊断；初始化失败不每帧刷日志。
- **WING-09 多端表现**：拥有者与观察者各自消费本地角色动画，使用已经复制的主角 Montage 恢复表现，不新增表现 RPC；迟到通知不覆盖新会话。
- **WING-10 成本与权限**：网格复用；空闲关闭翼网格 Tick；纯表现配置在专服跳过翼网格，启用权威采样时专服保留隐藏网格并由命中任务按需求值。
- **WING-11 预览隔离**：WingText 使用原有翼组件、原有挂接和原始源动画，不改其父类，不成为正式技能逻辑依赖。
- **WING-12 生命周期接入**：接入现有 Definition 的执行结束清理，支持主角 ASC 保持而 Pawn 更换；不得把预览 Character 当作正式 Hero。
- **WING-13 隐藏淡出**：窗口关闭和攻击取消平滑降低三个槽位的材质可见度，重复关闭不重置计时，新攻击取消旧淡出；不延长伤害窗口。

## 3. 方案概述

保留 `Procedural` 模式，新增 `AnimationAsset` 模式。新模式使用独立 SingleNode 翼实例按时间采样，不要求创建另一个 AnimBP 或翼 GA；以后需要复杂混合可扩展提供者，但本次不建立空框架。

通知是无运行状态的窗口配置及刷新入口。组件每帧轻量检查本地白名单源实例，并根据动画当前时间查询窗口。这个持续状态查询用于补偿通知漏过、跳段和观察者中途收到 Montage；Begin/End 不保存角色引用，不直接持有显示布尔值。

主角结束攻击时通过已有执行结束入口结束匹配的翼会话。停止中的 Montage 不再作为有效源。显隐及同步只在游戏线程修改，隐藏网格仍可在显现前被显式求值。

## 4. 系统架构与模块划分

### 4.1 HodgeWingProfile

保存模式、翼网格、挂点、偏移及动画映射，执行编辑器和运行时预校验。旧旋转姿势配置保持兼容，不删除已有字段或修改资源名。

### 4.2 HodgeWingComponent

负责网格创建/复用、源动画查询、会话身份、窗口并集、翼动画时间、骨骼刷新、显隐与统一清理。拥有运行时状态；GC 强引用资源及网格，弱引用角色和源动画实例。

### 4.3 HodgeAnimNotifyState_WingAction

只保存 WindowId，挂在角色源动画或 Montage 上，通知组件刷新本地源状态。支持分支点及普通通知，不依赖某个测试蓝图的 Cast，不发起 GAS 激活。

### 4.4 GameplayAbility_Definition

在现有 EndAbility 清理中通知翼组件结束对应 ExecutionId。不得另行接管该 GA 的 Montage delegate，也不改变攻击取消或命中逻辑。

### 4.5 资产制作与验证

Editor 工具补充通知窗口和 WingText 的组件配置；新正式 Profile/组件蓝图经 Experience 注入正式 Hero。旧注入条目保持 false，不修改其他 GameFeature 配置。所有用户已有挂接及通知在修改前备份。

## 5. 接口定义

接口名称以最终头文件为准；以下是本版职责契约。

- `InitializeWing()`：预校验并创建或绑定翼网格；重复成功调用不创建第二个网格。
- `RefreshPresentation()`：读取当前合法源，创建/更新会话并应用当前姿势及窗口；通知和正常 Tick 共用。
- `BeginWingAction(BindingIndex, SourceInstanceId, ExecutionId)`：返回带初始化代的表现句柄；同一源身份返回相同句柄。
- `EndWingAction(Handle)`：只结束匹配会话，开始淡出并阻止仍存活的已取消源再次显现；重复/旧句柄返回 false。
- `EndWingForExecution(ExecutionId)`：供正式攻击结束使用；不结束另一执行的表现。
- `ResetWingPresentation()`：隐藏并失效当前会话；换 Pawn/销毁使用，释放相关引用。
- `GetPresentationState()`：只读诊断快照，供蓝图、集成及端到端测试读取。
- `GetWingMesh()`：返回绑定网格；纯表现新模式专服允许为 null，开启权威命中采样时必须存在隐藏网格。
- `PrepareAuthorityHitPose(ExecutionId)`：服务器命中任务使用，验证当前执行和主动画后准备挂点及翼姿势，不改变显示窗口。

这些接口负责表现与骨骼准备。组件不开放客户端任意资源选择的 RPC，不创建/撤销能力，不增加伤害窗口；实际查询和伤害仍由既有命中任务与 GAS 执行。

## 6. 数据结构

### 6.1 动画映射 FHodgeWingAnimationBinding

SourceAnimation 为角色 Montage 或预览 Sequence；WingAnimation 为同一翼骨架的 AnimSequence；WindowId 选择对应通知窗口；SourceStartTime、WingStartTime、PlayRateRatio 定义映射。

`WingTime = clamp(WingStartTime + (SourceTime - SourceStartTime) × PlayRateRatio, 0, WingLength)`。

源及翼起始时间必须有限且非负，倍率必须有限且正；同一源只能出现一次映射，窗口必须位于源时长内且 End>Begin。翼动画到末尾保持末帧，不自主回到第零帧。

### 6.2 表现句柄及会话

句柄包含唯一 Id 和初始化代 Generation。会话记录映射索引、源实例身份、ExecutionId、源秒、翼秒和可见性。远端没有本地 GA 实例时，以本地 Montage 实例建立表现身份；该身份不作为服务器伤害授权。

### 6.3 复用网格

ExistingMeshComponentName 为空时组件创建自己的 WingMesh；WingText 设置为已有 SkeletalMesh 组件名。复用时保留用户的挂接与变换，校验资源一致；销毁协调组件不能误销毁用户创建的网格。

## 7. 关键流程

### 7.1 初始与空闲

BeginPlay → 校验 Profile → 创建/复用网格 → 关闭碰撞及隐藏阴影 → 隐藏 → 关闭翼网格自动 Tick。协调组件保留轻量源检查，避免为观察者依赖一次广播。

### 7.2 正常攻击

源 Montage 已播放 → 找到白名单映射和本地实例 → 幂等建立会话 → 查询窗口 → 窗内设置翼动画时间 → 显式更新动画并刷新骨骼 → 显示；离开窗口后开始材质淡出。

### 7.3 结束与取消

GA EndAbility/蒙太奇停止 → 匹配身份结束会话 → 保留最后一帧翼姿势并淡出 → 完全隐藏。死亡、Avatar 更换、销毁和配置失败立即清理。旧 End 通知只刷新当前源，不能按旧布尔值隐藏新攻击。程序化模式保留原插值测试与资源校验。

### 7.4 预览

WingText 身体继续播放用户选定的 AM_Attack04；已有翼组件由协调组件采样 B_Attack04。主时钟循环开始新一轮时产生新的预览实例身份；翼自身不循环。独立骨架编辑器只有静态 Preview Asset 时不会假装拥有 WingText 运行时。

### 7.5 网络

源 Montage 的现有 GAS 复制到各端 → 本地组件按收到的源时间恢复翼姿势与窗口。没有向服务器发送“显示翅膀”或任意翼动画的 RPC。客户端预测取消被服务器拒绝时，新的/恢复的合法源可以重建本地表现；必须通过 PIE 验证真实流程。

## 8. 异常处理

- 配置缺失或永久非法：一次明确诊断，保持隐藏；显式重新初始化可重试。
- Owner 不为 Character、Socket 不存在、翼骨架不匹配：禁止部分初始化成功。
- 源动画停止：保留最后姿势淡出；资源失效或死亡立即隐藏，不回退可见的参考姿势。
- Begin 重复或 End 迟到：依据会话句柄和源实例判断，不影响其他角色或新执行。
- 非有限源时间：结束当前表现，不把 NaN 写入骨骼。
- 通知窗口跳过/重叠：使用当前源时间和并集，避免仅靠成对回调计数。
- 复用网格被外部销毁：停止表现并诊断；不擅自接管另一个组件。

## 9. 约束条件

仅新增中文注释，每行说明一个内容点，函数体内连续 // 块最多三行。保持同类主 cpp、现有命名、UE 5.5.4 和模块边界；Runtime 不依赖 Editor。构建前关闭已保存的编辑器，不覆盖未保存工作，不自动提交。

不包含常驻背部、攻击吸附、锁定、角色动画重定向或源文件坐标归零。初版表现不包含翼伤害，后续 Box 判定接入见第 12 节。材质改动仅增加淡出控制，保留原贴图、颜色与特效参数。0.01 导入缩放保持不变，0.8 是用户校准的组件统一缩放。

Profile 的资源和模式在运行生命周期内视为只读；修改资源或模式需结束旧组件后重新创建，失败初始化可显式重试。角色暂停时复用已求值翼姿势，不重复推进或求值局部翼骨骼。测试工具使用保存的网络设置恢复编辑器；非反射的 EPlayNetMode 通过既有原生校验入口设置。

## 10. 验收标准与测试追溯

- 单元 `Hodge.Wing.AnimationMapping`：WING-02/03/04/07/08，验证时间映射、窗口边界与并集、非法倍率/资源/缩放。
- 单元 `Hodge.Wing.PresentationSession`：WING-06，验证重复开始、旧句柄、重复结束及初始化代失效。
- 保留原 `Hodge.Wing.Envelope`、`CancellationAndClock`、`AssetContract`：WING-07，旧模式回归。
- 集成 `Hodge.Wing.AttackPresentationIntegration`：WING-01/03/04/05/06/07/08/10/11/12，真实世界、网格和预览主 Sequence，验证实际骨骼、首次求值、取消、死亡、Avatar 更换、复用及空闲无翼 Tick；正式主 Montage 在真实输入端到端中验证。
- 资产集成：WING-02/03/07/11，核对正式 Profile、通知窗口、预览组件、旧注入关闭和新注入唯一。
- 单人真实输入端到端：WING-01～06/12，五段普攻只在匹配动作出现，收招与 Dash 取消无残留。
- Listen Server 两端端到端：WING-04/06/09/12，拥有者、服务器及观察者实际翼骨运动和取消一致。
- 专服 PIE：WING-09/10，纯表现配置跳过服务端翼网格，开启权威采样后保留隐藏网格，客户端仍正常显示；不等同独立 Server Target 构建。

Editor/Game 常规构建必须通过。每项执行结果记录需求 ID、测试名、命令/网络模式、预期、实际及证据路径；没有执行的测试明确标记未验证。性能验收首先确认隐藏时没有翼骨求值，不把这一结构检查描述成固定帧率基准。

## 11. 隐藏淡出修订（2026-10-12）

WING-13：正常显示窗口关闭、主动画停止和攻击取消时，材质在 `FadeOutSeconds` 秒内由当前可见度平滑降至零，之后才隐藏网格。默认时长为 0.35 秒，与现有武器消失时长一致；零表示立即隐藏。计时使用本地组件游戏时间步长，不依赖已经停止的攻击动画时钟。重复关闭不重置计时，新合法攻击显示前重新求值姿势并取消旧淡出，旧句柄不能隐藏新攻击。

`FHodgeWingFadeState` 提供 Show、BeginFade、Advance、Reset；`UHodgeWingComponent` 提供只读 `GetVisibilityAmount()` 与 `IsFadingOut()`。`Presentation.bVisible` 表示实际渲染可见，淡出期间允许为 true；不能把它作为伤害窗口或活动技能状态。淡出保留最终局部翼姿势，挂点继续跟随身体，不追加骨骼求值，不延长命中判定。

Profile 新增 `FadeOutSeconds`、`VisibilityParameter=WingVisibility` 和按槽位的 `Materials`。各角色分别创建 MID；实体表面使用 DitherTemporalAA 遮罩，原遮罩与淡出结果相乘，透明特效使用原透明度乘可见度。六份材质副本位于 `/Game/Main/Character/Hero/Wing/Fade`，不修改导入模型、源材质或测试变体。复用网格解绑时恢复原材质。

死亡、Avatar 更换、配置失败及组件销毁属于立即清理，不保留尾迹；专服不创建纯表现网格，开启第 12 节权威采样后仍不渲染或创建 MID。时长必须有限且非负，启用淡出时参数名不能空。`Hodge.Wing.FadeState` 覆盖幂等、边界与新攻击覆盖；世界集成覆盖窗口关闭、取消、渲染中间值、姿势冻结和完全隐藏。单人和 Listen Server 端到端增加实际 MID 可见度及淡出完成检查。

## 12. 翼面 Box 攻击判定（2026-10-12）

- **WING-HIT-01 几何配置**：使用现有 Hodge 持续命中，Source=NamedComponent、ComponentName=WingMesh、Shape=Box、BoneOrSocket=Bone_Wing000、TransformPolicy=Follow。沿既有伤害窗口以 60 Hz 采样翼动画实体材质槽的蒙皮顶点，计算翼根局部坐标包围盒并各轴追加 4 cm；特效槽不参与测量。LocalTransform.Scale=1，BoxHalfExtent 为世界厘米半尺寸，0.8 网格缩放在测量时仅应用一次。保留既有伤害窗口和其他通知，不创建常驻碰撞组件。
- **WING-HIT-02 权威姿势**：Profile 的 bEnableAuthorityHitSampling 默认 false，正式 Anzhu 配置开启。专服创建隐藏 WingMesh，不创建 MID、不自动 Tick 翼动画；命中任务在 PostPhysics 等待角色及翼协调更新后调用 PrepareAuthorityHitPose(ExecutionId)。检查服务器权限、GAS 执行身份、活动主蒙太奇、映射与骨架后求值当前翼姿势，再重置首次历史并查询盒体。配置未开启、旧执行、主源失效或死亡均拒绝采样；不以旧姿势或参考姿势代替有效源。
- **WING-HIT-03 查询和结算**：复用现有 Box Sweep 与旋转子步查询、目标资格过滤、GAS 伤害和受击流程；手部与翼面沿用相同 HitGroup+AttackPhase，每目标本刀一次。取消立即关闭命中会话，显示尾迹不延长伤害。尺寸及偏移由该条命中通知保存，不放入表现 Profile，不另建翼 GA。
- **WING-HIT-04 可调试与验收**：蒙太奇中的 NamedComponent/WingMesh 通知是盒体编辑入口；可调 BoxHalfExtent、LocalTransform 和窗口。Hodge.Combat.DebugDraw.Body=1 可查看此来源的盒体。验收包括实际顶点覆盖、骨骼跟随、盒内/盒外查询、缺失骨骼拒绝、非法执行拒绝、单人与服务器权威真实扣血、同刀去重，以及取消/淡出后的无伤害。

编辑器 ConfigureWingBoxHit 是校准工具，在临时网格上测量，成功后才提交该通知与权威开关；不修改原模型、骨架 Socket、材质、缩放或其他动画通知。盒体对翼面采用保守包围，不等同逐三角形碰撞；旋转 Sweep 使用有限子步。修改动作、翼映射、组件缩放或伤害窗口后需要重新校准，不把当前测量值当作所有翅膀的通用尺寸。

角色默认 AlwaysTickPose 只保证更新动画时钟。未被渲染的服务器角色在命中准备时按需 RefreshBoneTransforms，同一帧最多一次，然后更新翼组件挂接变换并求值局部翼动画；不永久修改角色可见性 Tick 策略。暂停恢复时也先准备当前翼姿势，再重置历史，禁止补扫暂停期间经过的路径。

当前正式第 5 段窗口为 0.566667～0.866667 秒；19 个姿势、97603 次实体顶点采样得到 LocalTransform.Location=(61.441557, 5.547495, -35.116657) cm，BoxHalfExtent=(67.642195, 33.136577, 77.690944) cm，已包含各轴 4 cm 余量。尺寸是半尺寸；若手动改为整尺寸会扩大两倍。
