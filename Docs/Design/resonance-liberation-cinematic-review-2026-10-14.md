# 共鸣解放：AM_Burst01 前半段电影镜头评估与实施约束

原评审日期：2026-10-14；实施状态更新：2026-10-15。本文保留当时的方案复核依据，后续已按CameraMode＋精确蒙太奇实例路线实现；当前实际接口、资产与规则以[共鸣解放设计第0节](resonance-liberation.md#0-当前实施规格2026-10-15优先于历史草案)、[配置指南](../Guides/resonance-liberation-configuration.md)及[实施验收](../Validation/resonance-liberation-2026-10-15.md)为准。下文“拟新增”“尚未编码”仅代表原评审时点，建议接口名不作为最终API。

## 1. 本轮目标与结论

保留现有 Melee／Standalone、CameraModeStack、原生动画通知、WingAction 和服务器命中链路。大招使用现有角色动画创建的 AM_Burst01 蒙太奇，前半段安排特写和返回战斗镜头，后半段继续正式角色动作与攻击。翅膀在整个大招动作期间显示，播放由 G_Burst01 创建的翼蒙太奇。

自身为金刚体，输出为金刚体判定；这是两项独立配置，不是新的无敌机制。满100消耗100、独立12秒CD等已确认资源规则维持原设计，本轮相机原型不要求先实现资源和伤害。

推荐首版采用「Montage实际位置 + 角色局部构图数据 + Cinematic CameraMode」。先验证三个构图与平滑进入／恢复，再加特效。暂不引入 Sequencer 最终视角接管，不新增另一套 PlayerCameraManager、SpringArm、攻击Timeline或万能演出管理组件。

## 2. 参考资料与证据边界

用户提供的[暗主GIF](https://media.giphy.com/media/RXFz6h6qBccF39YrNt/giphy.gif)和[B站慢放入口](https://www.bilibili.com/video/BV1oZ421M7DF/)本轮网络读取失败，未获得可逐帧核对的画面。搜索另检索到[标为暗主共鸣解放的实机视频](https://www.youtube.com/watch?v=HPvGoLBE5_k)，也没有完成逐帧观看。不能据此认定原作有六个镜头、前冲时段或2.4秒固定演出。

暗主的脸部／手部、暗翼、暗紫色能量与爆发作为用户指定的视觉参考；本文的构图和时间比例均是Hodge预演建议，不是原作测量数据。若后续需要逐帧复刻，应取得可实际查看的参考视频，对照已有角色动画的关键姿势再调整。

Epic的[UE5.5 Gameplay与Sequencer混合资料](https://dev.epicgames.com/documentation/en-us/unreal-engine/blend-gameplay-animation-to-cinematic-animation-in-unreal-engine?application_version=5.5)说明进入／退出需要处理相机混合、投影一致性和输入恢复；[Camera Cut Track资料](https://dev.epicgames.com/documentation/en-us/unreal-engine/cinematic-camera-cut-track-in-unreal-engine?application_version=5.5)说明Camera Cuts参与活动相机选择。这支持Sequencer方案的技术可行性，不证明《鸣潮》内部采用了该方案。

## 3. 本地源码与 Lyra 的实际复用点

### 3.1 Lyra 已核对的调用链

本机目录：`E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame`。

- `AbilitySystem/Abilities/LyraGameplayAbility.cpp::SetCameraMode/ClearCameraMode`：经Hero申请、按AbilitySpecHandle释放，EndAbility负责结束清理。
- `Character/LyraHeroComponent.cpp::DetermineCameraMode/SetAbilityCameraMode/ClearAbilityCameraMode`：技能临时模式优先，否则选择PawnData默认模式；只保存当前技能覆盖，不是完整的多请求仲裁栈。
- `Camera/LyraCameraComponent.cpp::GetCameraView`：EvaluateStack后将ControlRotation写回PlayerController，再输出最终视图。
- `Camera/LyraCameraMode.cpp::PushCameraMode/GetCameraModeInstance/UpdateStack`：按类缓存实例，栈顶相同对象不再激活；完全覆盖后移除下层模式。

Hodge已经迁入上述机制。继续复用其所有权与混合职责；本次核查的Lyra链路没有提供「与当前攻击蒙太奇实例同步的多分镜大招」实现，不能把找到CameraMode类当成电影镜头功能已经存在。

### 3.2 Hodge 的相机职责

- `HodgeGameplayAbility::SetCameraMode/ClearCameraMode`调用Hero；`EndAbility`自动清理。
- `HodgeHeroComponent::DetermineCameraMode`：技能CameraMode > 有效软锁定CameraMode > PawnData默认CameraMode。
- `HodgeCameraComponent::GetCameraView`：模式选择、栈求值、ControlRotation写回、相机位置／视觉旋转／FOV输出；这是现有最终游戏相机执行链，不应另建Tick直接改相机。
- `HodgeCameraMode_LockOn::OnBecomeTop`从上一帧模式视图旋转种下软锁定构图；`UpdateView`调用锁定组件的辅助修正，再复用ThirdPerson避障。
- `HodgeCameraModeStack`缓存模式实例、执行混合和清除被完全覆盖的下层模式；Hodge增加了OnBecomeTop，但栈顶类不变时仍提前返回。
- `HodgePlayerCameraManager::UpdateViewTarget`还允许UI相机覆盖视角。上述优先级只属于游戏相机选择，不能宣称大招无条件高于UI镜头。

`FHodgeCameraModeView`已有Location、Rotation、ControlRotation、FOV，可表达「视觉朝向和操作朝向不同」；尚无逐模式PostProcess字段。现有CameraComponent的后处理属于组件级配置，不是大招专属可自动混合的PP轨。

### 3.3 Hodge 的动作与表现职责

- `HodgeGameplayAbility_Definition::ActivateAbility`先校验、CommitAbility，再添加ExecutionBodyTag并请求攻击朝向。
- `StartExecutionMontage`由ASC播放主蒙太奇，取得MontageInstanceId，并按ExecutionId＋实例ID绑定结束委托。现有OnExecutionReady发生在PlayMontage之前，不能把它当作实际播放实例已经建立的事件。
- 通知资源和结束回调检查执行及实例身份；EndAbility清理体状态、武器、翼和转向资源。
- `HodgeAnimNotifyState_WingAction`直接配置WingMontage、PlayRate、StartPosition和FadeOutSeconds，窗口控制显隐；WingComponent以主蒙太奇源时间求翼姿势。
- 当前翅膀Profile动画绑定仅为旧资产兼容，不是新增大招的正式入口。外部讨论建议再扩展旧Profile动画绑定的部分不采纳。
- Standalone移动取消预测被拒绝时，Definition可在同一个ExecutionId下重建蒙太奇实例。电影镜头必须识别这次合法重绑，不能仍采样已经停止的旧实例。

## 4. 必须修改原建议的部分

### 4.1 使用现有动作，前半段特写，不套用2.4秒全程电影

只读编辑器盘点确认：`/Game/Wuwa/Anim/AM_Burst01`目前是AnimSequence，长度约4.533333源秒；`/Game/Wuwa/Wing/BaseAnim/G_Burst01`是约4.433333源秒的翼序列。名称含Burst的AnimMontage查询为空。用户确认尚未创建蒙太奇。

后续创建 `/Game/Main/Character/Hero/Ability/ResonanceLiberation/AM_Burst01`，引用上述角色序列，不改源资产名称，不制作临时角色动作。使用现有主角FullBody Slot，并实际验证骨架、Slot和根运动设置。翼创建AM_Wing_Burst01，使用翼AnimBP独立Slot。

未裁剪且PlayRate=1时，前半段约2.266667秒；这只是源序列长度换算，正式蒙太奇的剪辑、段落和播放速率确定后重新计算。不能为了贴合外部建议把角色动作强行压缩至2.4秒。

### 4.2 视觉旋转不能污染操作旋转

正面面部特写可能让相机面向角色，视觉Yaw与角色出招方向相反。若照搬ThirdPerson的 `View.ControlRotation=View.Rotation`，CameraComponent将把电影Yaw写给PlayerController，影响镜头相对移动、后续攻击的零输入镜头瞄准和恢复过程。

电影模式仅输出视觉Rotation；ControlRotation保留该本地玩家的游戏操作朝向。维持CameraComponent既有写回入口，不全局禁用普通／锁定相机的ControlRotation更新，不直接旋转Actor，也不把电影镜头的LookAt结果发送成攻击TargetData。

默认建议特写期间暂停本地Look输入，窗口结束恢复；使用带执行归属的输入占用，不能ResetIgnoreLookInput清除其他系统的限制。移动／攻击的服务器限制仍走原有GAS和通知许可，不能因为镜头存在就关闭所有ASC输入。是否禁止Dash、后摇移动取消仍是独立玩法决策，未确认前保持现有规则。

电影模式生效期间，软锁定辅助旋转不能继续参与ControlRotation写回；LockOn若作为混合中的下层模式求值，只能保留恢复所需状态，不能与电影模式争夺视角。结束后再恢复其正常软吸附，不清除锁定目标来规避镜头冲突。

### 4.3 相机类缓存不等于新的演出会话

同类连续释放时，即使相机类没变化，也必须显式重绑定Profile、Avatar、ExecutionId、蒙太奇实例、进入POV和段落状态。不能只在OnActivation或OnBecomeTop清零计时。

现有AbilitySpecHandle保护不同技能之间的清理，但不足以区别同一Spec的不同激活。需要给技能相机申请补充执行／代次身份；旧结束回调、旧通知结束或旧Pawn解绑只能释放自己拥有的资源。

### 4.4 恢复应查询当前游戏状态

栈顶完全覆盖后，下层CameraMode可能已被移除，不应保存释放前一个类然后无条件恢复。结束时按Hero现有规则重新选择当前有效技能覆盖、有效锁定或默认镜头，尊重UI覆盖。

锁定模式当前用上一帧视觉Rotation种构图；电影正面镜头会把这种种子变成错误的恢复方向。需提供有归属的恢复种子：保留的游戏ControlRotation用于操作／锁定方向初始化，电影当前POV仅用于视觉位置和FOV的连续混合。普通软锁定切入行为维持原有设计，不全局替换所有种子来源。

## 5. 推荐首版构图与时间约束

T为正式AM_Burst01的源时间长度；镜头参数在配置资产／蒙太奇通知上可编辑，不写死在C++。下列比例仅供预演，应根据手、脸、翼的实际关键帧调整。

1. **进入：约0～0.08T。** 从本地PlayerCameraManager当前最终POV平滑混合到角色斜前方上半身构图，避免使用角色朝向替代当前视角。
2. **聚焦：约0.08～0.25T。** 面部与手部共同构图，适度推进、收窄FOV；角色并无聚能姿势时不凭空假设已有手势。
3. **展翼：约0.25～0.40T。** 稍微拉开或调整到斜侧构图，保持脸、躯干和暗翼可读；不要求已有动画额外增加前冲。
4. **返回：约0.40～0.50T。** 从当前电影视图平滑返回当前游戏镜头，后半段不继续强制面部特写，显示真实战斗和攻击区域。

特写、WingAction和Hit是三条独立通知：电影窗口前半段，翼窗口全程，固定Box单次通知在实际冲击帧T_Hit。伤害不由镜头切点或电影播放完成授权；不得为镜头效果移动已有服务器伤害事件。

如果冲击在前半段，构图应提前拉开，或缩短特写；T_Hit尚未预览确认，不能假定必在后半段。正常退出尽量在前半段完成；播放倍率改变时按剩余源时间调整混合权重，暂停时构图随主动作停住。取消、死亡或没有有效蒙太奇可采样时，退出才使用本地实时时长完成，不再等待失效的源时间。

禁止同时使用完整的外层Stack进入混合和另一套同长度全量POV进入混合，造成双重缓动。Stack负责外部模式交接，Profile曲线负责内部运镜；若现有混合接口不能按源位置处理前半段返回，仅增加本次会话的可选混合驱动，默认普通／锁定模式保持原算法。

## 6. 拟新增配置、接口与事实来源

以下为待实现接口，不是当前编辑器已有字段。

### 6.1 UHodgeCinematicCameraProfile

UDataAsset，存放Shots、进入／退出设置和安全参数。只保存配置，不保存玩家或动画实例状态。

`FHodgeCinematicCameraShot`建议字段：ShotId；窗口内归一化Start／End；AnchorBoneOrSocket；局部位置曲线；独立LookAt骨骼及局部偏移；FOV曲线；段间Blend／Cut策略；位置和朝向的参考策略。

坐标建议：相机位置基于角色水平朝向坐标架，锚点只提供位置；面部／手部LookAt取本帧骨骼位置。避免继承头部骨骼全部旋转导致头部摆动带着镜头晃动。第一版明确只支持稳定正倍率、不反向播放的连续窗口，分段跳跃按当前位置重求构图。

Profile安全参数：碰撞探测半径、最小面部距离、允许Pitch范围、可用的角色根部备用锚点、进入／退出曲线。缺必需骨骼为配置错误；只有显式指定的备用锚点才可回退，并记录诊断。骨骼采样应在主角姿势更新后、CameraMode求值时读取，核实CMC／网格／相机实际更新顺序，不使用上帧手部位置冒充同步完成。

后处理作为P2可选扩展，不在P0定义假字段宣称已有PP轨。只有需要时再给模式视图增加可混合PP数据或按归属的覆盖入口，保留相机组件与场景既有设置。

### 6.2 相机会话身份

`FHodgeAbilityCameraHandle`建议包含拥有者SpecHandle、ExecutionId和递增Generation。实际运行上下文同时持有弱Avatar、弱GA、主蒙太奇资源和当前MontageInstanceId。

Spec用于能力归属；ExecutionId／Generation用于防止旧激活清理新激活；MontageInstanceId用于选中实际播放实例；Avatar用于拒绝旧Pawn回调。ExecutionId和实例ID只在本地匹配，不假定客户端与服务器数值相同，不为电影同步新增网络RPC。

Hero保存当前技能相机覆盖及本次归属上下文；CameraMode消费上下文，按类缓存但按会话重置。保留原SetCameraMode接口作为兼容入口，新电影路径使用完整身份，并相应更新基类结束释放，避免旧的仅Spec清理绕过新校验。

建议接口职责：

- `BeginCinematicCamera(Context, Profile)`：只允许本地拥有者、当前Avatar与实际播放实例；返回有归属句柄，失败不锁输入、不终止服务器攻击。
- `UpdateCinematicSource(Handle, MontageInstanceId)`：由当前Definition合法的播放／预测恢复入口重绑；旧实例自己不能换绑。
- `EndCinematicCamera(Handle, Reason)`：身份匹配才释放；正常结束按源进度返回，异常中止捕获当前POV后实时间混合。
- `GetCinematicFrame(Handle)`：在游戏线程验证上下文、读取精确实例Position／PlayRate及锚点，返回不可用时请求退出。

最终接口命名可按相邻代码收敛；不要求新增一个独立组件持有重复的事实来源。UObject资源由UPROPERTY等GC可见引用持有，Avatar／GA等运行对象使用弱引用，并处理解绑。

### 6.3 原生通知与主蒙太奇入口

拟新增 `UHodgeAnimNotifyState_CinematicCamera`，仅配置Profile，通知条长度表达电影窗口。Notify对象不保存玩家运行状态，资源归属存入Hero／当前Definition上下文。

Definition在实际MontageInstanceId建立之后提供会话注册入口，不能在Commit前抢相机。结合当前位置核对电影窗口，避免首帧Begin、跳段、网络位置修正或预测恢复越过通知边界时漏开／漏关；EndAbility是最终清理保障，不只依赖NotifyEnd。

演出内姿态、FOV、位置由同一实例的实际Position求值，不另建Elapsed累加时钟。允许小幅校正时短暂视觉缓动，但缓动不能改主蒙太奇、命中帧或资源提交。

## 7. 金刚体配置和边界

`DA_Resonance_Liberation.ExecutionBodyTag = State.Combat.Body.Vajra`。

`DA_Resonance_Liberation.DefaultHitConfig.Reaction.AttackJudgementTag = Combat.Attack.Judgement.Vajra`。

固定Box的命中通知选择 `bUseDefaultReaction=true`；若另配ReactionOverride，也应使用同一输出标签。`bUseDefaultDamage`是另一个开关。具体Impact与预留削韧尚未冻结，最高等级不自动生成任何控制效果。

源码事实：`HodgeHitReactionTypes.cpp::BodyRank/JudgementRank`将金刚体映射为4；`CanImpact`为攻击等级>=目标体状态等级。普通／技能／霸体攻击等级不足时不能通过控制等级比较；自身仍承受生命伤害。同级金刚体攻击可以通过比较，不改变先前明确的同级可打断规则。

`HodgeHitReactionComponent::FinishHit`还检查死亡、拒绝／免疫、接受策略、Impacts和目标受击配置。金刚体输出不能绕过这些条件，尤其空Impacts不会凭判定标签制造僵直。

自身体状态从现有Definition提交成功后添加，随整个执行生命周期到EndAbility释放，包括正式攻击及后摇；不在电影窗口结束时提前解除。现有添加入口为LooseGameplayTag，不能额外假定它已自动复制给旁观客户端。服务器用于权威受击判定；如需远端金刚体提示，另用既有复制状态／Cue设计，不重复添加一份生命周期不一致的标签。

金刚体不修改Death、GAS取消关系、技能互斥或移动取消许可。若希望大招也禁止Dash中断，后续必须修改对应技能／关系配置并明确时段，不能把“Body.Vajra”当作所有取消通道的最高优先级豁免。

## 8. CameraMode 与 Sequencer 取舍

**首版CameraMode＋曲线：** 沿用现有视角仲裁，无额外最终视角拥有者；容易绑定主蒙太奇Position与本地执行身份。代价是需制作构图数据、预览工具及针对近景的安全处理。三种构图和前半段演出适合采用此方案。

**后续Camera-only Level Sequence：** 适合美术要求大量镜头切点、可视化轨道编辑和复杂分镜。仍由蒙太奇管理正式角色动作；Sequence不写角色Transform、不接管角色SkeletalMesh，不把Sequence事件变成正式伤害事件。

若引入，推荐禁用自动Camera Cuts最终视角接管，让Sequence只求镜头样本并交给同一CameraMode输出；需实现局部参考坐标、绑定、跳转求值、位置校正和释放。直接同时SetViewTarget与CameraModeStack会产生两个控制源。届时核实UE5.5的LevelSequence／MovieScene／CinematicCamera模块可见性与Runtime依赖，不为P0提前加入这些模块。

不同时建设两套后端；先证实纯CameraMode预演不足，再投入Sequence适配。

## 9. 异常、多人和性能策略

- 仅本地拥有者申请电影镜头／屏幕PP；远端客户端继续看角色和翼的世界表现。专服不创建电影模式、镜头或PP资源，但正式动画通知与攻击查询仍需可靠执行。
- 缺相机Profile、无效曲线或不兼容锚点：资产验证报错；运行中只退出电影镜头，技能权威逻辑仍按原链路进行，不能因本地资源缺失返还已提交消耗。
- Montage停止、预测拒绝、死亡、换Pawn、相机被新技能替换：立即使旧上下文失效；只清理对应句柄、输入占用和PP，不能清空全部技能相机或控制限制。
- 预测移动取消：本地及时退出电影镜头；服务器拒绝后仅从当前Definition合法的新实例恢复相机会话，按恢复位置选Shot，不重播已经过去的整段演出。
- 目标消失：人物特写不依赖敌人目标，继续有效镜头；退出时目标仍有效才回软锁定，否则默认镜头。沿用已有目标死亡清理，不凭空增加敌人失效取消大招规则。
- 近墙：复用项目／Lyra的相机避障思路，但验证近景安全点，普通第三人称Feelers不能自动保证面部特写不穿脸／穿翼。遮挡严重时减弱特写并回游戏镜头，不能移动角色来凑构图。
- 手势／面部锚点重合：保留上一有效旋转，限制Pitch，使用明确备用LookAt点，避免零向量LookAt翻转。
- UI覆盖：尊重UI相机，不抢UI视角；世界中主动作继续播放，恢复时按当前源位置求值，过了电影窗口不补播。
- PlayerCameraManager最终POV可能包含震屏／相机修饰，与CameraComponent上一帧模式视图不同；进入前核对两层结果，避免把已经叠加的震屏再次作为基础视图重复叠加。P0灰盒先不新加震屏，正式表现集成时覆盖此项测试，不能宣称GetLastCameraModeView天然等于最终屏幕视图。
- 不使用SetGlobalTimeDilation；局部震屏／音效为本地表现，世界Niagara走现有通知／Cue职责，命中授权不依赖电影结束。
- P0只评估当前模式／当前Shot，按需做一次安全检测，不每帧扫描全世界、创建新CameraActor或加载软资产。资源在激活前准备，配置缺失与加载等待分别诊断。

## 10. 预计新增和修改文件

新增Runtime类，均放在现有Hodgepodge模块，每类一个主cpp：

- `Public/Camera/HodgeCameraMode_Cinematic.h`和`Private/Camera/HodgeCameraMode_Cinematic.cpp`：构图求值、安全处理、会话重置。
- `Public/Data/HodgeCinematicCameraProfile.h`和`Private/Data/HodgeCinematicCameraProfile.cpp`：镜头配置、参数校验。
- `Public/Animation/HodgeAnimNotifyState_CinematicCamera.h`和`Private/Animation/HodgeAnimNotifyState_CinematicCamera.cpp`：原生电影窗口。
- `Private/Tests/HodgeCinematicCameraTests.cpp`：纯求值、身份、状态流转测试。

预计最小适配点：

- `HodgeHeroComponent.h/.cpp`：有归属的技能相机会话与本地Look占用，保持现有模式选择优先级。
- `HodgeGameplayAbility.h/.cpp`：现有相机申请／结束释放增加执行安全身份，保留旧调用兼容。
- `HodgeGameplayAbility_Definition.h/.cpp`：精确实例读取／播放后绑定／预测恢复重绑／结束清理，继续复用主cpp，不按演出功能拆分文件。
- `HodgeCameraMode.h/.cpp`、`HodgeCameraComponent.h/.cpp`：仅在必要时补充会话混合／进入实际POV接口，不重写Stack。
- `HodgeCameraMode_LockOn.h/.cpp`：电影返回时使用安全操作旋转种子，正常软锁定行为不变。
- `HodgeAbilityDefinition.cpp`：校验电影窗口、金刚体输出及通知继承配置；不恢复归档Timeline。

P0不修改PlayerCameraManager、角色旋转组件、MotionWarping、伤害Execution或Wing生命周期实现；使用它们已有接口。只有相应测试证明缺口才追加关联改动。

资源计划：AM_Burst01、AM_Wing_Burst01、DA_Resonance_Liberation_Camera、GA／DA／AS_Resonance_Liberation和既有方案GE。Niagara、音效、CameraShake和PP是P2资产，当前没有完成盘点或制作，不能宣称已有正式效果。

## 11. 分阶段 Agent 任务与验收映射

### P0：电影镜头灰盒

CIN-01：创建同名主蒙太奇、Profile和三种可编辑构图，通过原生窗口进入前半段镜头；不做临时角色动画、不实现资源和伤害、不制作新Niagara。

CIN-02：接入现有Hero→CameraModeStack；从实际POV进入，正常／异常退出平滑恢复自由或当前有效锁定镜头；ControlRotation不被正面电影Yaw替换。

CIN-03：补全执行身份、缓存重置、取消／死亡／换Pawn清理。相同Spec连续激活和旧NotifyEnd不能清除新会话。

验收：在不同世界位置及0／90／180度角色朝向、自由／锁定两种状态进入；近墙、宽屏／普通比例分别观察；记录切入／退出的POV位置、角度和FOV连续性。至少三个构图区分明显，前半特写结束后能看见真实战斗；截图／录屏作为手感证据，数值测试不能替代观察。

### P1：正式同步与技能配置

CIN-04：镜头读取匹配蒙太奇实例Position；WingAction全程同步G_Burst01；修改PlayRate、暂停、跳段、位置校正、移动取消拒绝重建实例后，没有旧会话采样或残留。

CIN-05：自身／输出金刚体分别配置，单次Box复用现有管线；不依赖本地镜头事件。若资源与伤害由其他开发任务实现，只验证接口，不重复开发一套。

验收：倍率0.5／1／1.5及大幅位置跳变；若不支持反向／循环配置，明确校验拒绝。普通／技能／霸体攻击不能以等级优势控制本大招；同级金刚体可以通过比较；空Impacts、伤害免疫、死亡和未配置目标受击Profile分别得到正确结果。取消、打空不改变原消耗／CD策略。

### P2：视觉补齐

CIN-06：添加可替换的聚能、翼能量、爆发Cue／通知、短促震屏和按归属PP。先对照实际AM_Burst01关键帧再制作；没有资源则标记待处理，不放无关联Delay冒充演出同步。

### P3：网络与回归

CIN-07：Editor／Game常规构建、原生测试、单人、Listen Server双客户端、Dedicated Server PIE和延迟场景。确认只有释放者有电影镜头，服务器伤害独立；取消／死亡／换Pawn／UI覆盖恢复正确。

CIN-08：回归软锁定、Dash离地继续、Dash→Sprint、普通攻击前摇Warp、Standalone后摇取消与现有翼淡出。专服确认主角动画通知和骨骼更新策略；Listen通过不能代替专服结果。

单元测试覆盖配置有限值、时间区间、FOV、锚点回退、分镜选择、重复／迟到释放、会话换代；集成测试覆盖通知和精确实例；端到端输出录屏／截图、逐条需求结果和未验证项。所有新代码、工具和测试写简洁中文注释，遵守现有单cpp与模块约束。

## 12. 本轮交付和待决事项

已完成源码与Lyra调用链复核、编辑器只读Burst资产盘点、UE5.5官方文档查阅及方案修订。只修改设计文档／索引；没有创建蒙太奇、改Runtime／蓝图、运行构建或PIE。

已冻结：已有动画创建同名AM_Burst01蒙太奇、前半段特写、全程翼动画、自身和输出金刚体、既定资源／CD规则。仍待实际预览确定：关键姿势与切点、T_Hit、具体Impacts、Box尺寸、相机曲线／安全距离；无敌、Dash特权和移动取消禁用不属于已批准功能。

下一步按上述P0交付可调镜头灰盒，评审实际效果后再进入P1；不把外部六镜头建议、未读取的视频帧数或尚未实现的Profile字段当作已经验证的事实。
