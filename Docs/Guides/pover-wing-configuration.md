# 漂子残象之翼：挂接与程序挥翼

> 本页仅描述旧 PMX 程序化翅膀。2026-10-12 已接入独立的新 Anzhu 攻击限定表现，旧条目仍关闭。新功能见[设计](../Design/attack-wing-presentation.md)和[实现验证](../Validation/attack-wing-presentation-2026-10-12.md)，不要使用本页的重建脚本覆盖新配置。

> 当前状态（2026-10-11）：按用户要求暂停启用。正式 Experience 中翅膀注入的客户端和服务器选项均关闭，末段的 WingMesh 命中通知已移除。以下为保留方案的配置说明；模型、挂点、代码与三个配置资产继续保留，游戏中不生成、驱动或判定翅膀。

重新启用需要明确恢复注入选项，并在制作脚本中显式设置 `ENABLE_POVER_WING=True` 重建命中配置。默认执行制作脚本会拒绝重新启用。

## 正式资产

- 主角：`/Game/Main/Character/Hero/BP_Hero_Pover`。
- 模型：`/Game/Wuwa/PoverEquipment/残象之翼`；保留独立骨架和原始文件名。
- 翅膀动画蓝图：`/Game/Main/Character/Hero/Wing/ABP_PoverWing`，父类 `HodgeWingAnimInstance`。
- 配置：`/Game/Main/Character/Hero/Wing/DA_PoverWing`，类型 `HodgeWingProfile`。
- 协调组件蓝图：`/Game/Main/Character/Hero/Wing/BP_PoverWingComponent`，父类 `HodgeWingComponent`。
- Experience：`/Game/Main/Experiences/Exp_HodgeDefaultExperience` 为正式 Pover 类注入组件，服务器和客户端都启用。

保留用户已创建的 `WingAttach` 及其偏移，它位于 `WingCase` 下。当前姿势输入来自翅膀 Reference Pose，不需要动画序列，也没有对主角设置 Leader Pose 或 Copy Pose。

## 动作范围

按现有连段表，实际顺序为 `01 → 02 → 03 → 05 → 04`。用户选择的第 5 段对应 `DA_Attack_4` 和 `AM_Attack04_Montage`，不是文件编号 05。前四段只跟随身体，末段才驱动翼骨骼。

动画图依次执行 Local Reference Pose、Local To Component、三次 Transform Modify Bone、Component To Local、Output。受控骨骼为 `Wing000`、`Wing003`、`Wing006`；骨骼空间旋转采用 Add to Existing，平移和缩放保留模型参考值。

## 可调参数

打开 `DA_PoverWing`：

- `Mesh` / `AnimClass`：模型与独立动画蓝图。
- `AttachSocket`：主角 Mesh 的挂点；当前为 `WingAttach`。
- `MeshOffset`：相对挂点的附加偏移，保持单位缩放。当前为单位变换，直接沿用用户挂点校准。
- `AttackMontage`：触发挥翼的角色蒙太奇。
- `WindupStart`：开始收翼，源动画秒。
- `StrikeStart`：收翼结束并开始挥击，源动画秒。
- `StrikePeak`：达到最大挥击姿势，源动画秒。
- `RecoveryEnd`：回到参考姿势，源动画秒。
- `CancelReturnSeconds`：动画停止或取消后，从当前翼姿回到参考姿势的世界秒数。
- `WindupPose` / `StrikePose`：Root、Mid、Tip 分别对应三根受控骨骼的局部旋转增量。旋转轴必须结合此模型预览调节。

初始时间为 0.346667 / 0.546667 / 0.716667 / 1.066667 秒，取消回收为 0.18 秒。时间从正式末段命中区间生成。修改蒙太奇通知后，也应检查这些时间是否仍与攻击动作匹配。

旋转初值：收翼 Root Pitch=-30、Mid Yaw=-12、Tip Yaw=-8；挥击 Root Pitch=65、Mid Yaw=18、Tip Yaw=12。相邻姿势使用 SmoothStep 权重和四元数最短路径混合。

## 生命周期与时间同步

`HodgeWingComponent` 在 BeginPlay 验证 Profile、Character 和挂点，然后创建角色自身的 `WingMesh` 组件。重复初始化返回同一个组件；名称冲突、缺失资源、非法时间或挂点缺失会明确报错，不无限重试。

运行顺序为角色 Mesh → WingComponent → WingMesh → 现有 PostPhysics 命中 Task。现有 Task 已将真实检测来源组件列为 Tick 前置。WingMesh 使用 AlwaysTickPoseAndRefreshBones、关闭 URO 和物理阻挡；主角攻击仍使用现有 Combat PoseLease。

协调组件读取指定蒙太奇的实例身份及真实播放位置，而不是另外累加攻击时间。暂停保持姿势，播放倍率自然作用于源时间；停止时从当前姿势平滑回收。远端角色读取其复制后的主角蒙太奇，不复制 270 根骨骼，不额外发起翅膀 GA。

连段 GA 的结束权限按角色方向区分：客户端接受服务器结束／取消；服务器上的 ComboCoordinated 仍禁止被客户端结束，必须由连段协调器授权。Standalone 保留原有服务器接受远端取消的配置。这样服务器打断攻击时，拥有者和旁观者的主角蒙太奇与翅膀都能一起退出。

组件销毁时只销毁自己创建的 WingMesh，并清理自己的依赖和姿势，不修改玩家 ASC、既有剑装备或主角 AnimBP。

## 命中配置

模型已增加 `WingHitRoot/WingHitMid/WingHitTip`，分别挂在 `Wing000/Wing003/Wing006`。当前末段新增一个 `Hodge 持续命中`：

- `Source=NamedComponent`；`ComponentName=WingMesh`；`BoneOrSocket=WingHitTip`。
- `Shape=Sphere`；`Radius=18 cm`；`TransformPolicy=Follow`。
- 命中区间与末段原有首个命中区间相同，0.566667～0.866667 秒。
- 与该原有通知共用 `HitGroup`、`AttackPhase`、伤害和 Reaction 配置，避免同一刀重复提交伤害。

去重由现有 Melee GA 的执行内命中记录管理。没有在翅膀 Overlap 或 Actor Tick 中另写扣血。每段独立挥击可配置不同 Phase；若需要真实多段伤害，应显式设计，不通过扩大采样数量增加伤害。

## 验证与后续范围

执行记录见本轮验证报告。程序挥翼是一套可调的首版动作，仍需内容制作人员依据镜头、攻击节奏及穿插继续调角度。当前没有羽毛物理、独立飞行翼、动画烘焙、Control Rig、专用服务器可执行文件或打包验证。

制作脚本依次为 `Tools/Wing/create_sockets.py`、`Tools/Wing/configure_assets.py`。Socket 使用项目网关的原生 create_socket 能力，因为 UE 5.5 将其名称和骨骼字段暴露为 Python 只读属性。脚本对已有挂点不做覆盖；正式资产备份位于 `Saved/WingImplementation/BeforeAssets`。
