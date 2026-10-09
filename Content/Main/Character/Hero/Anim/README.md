# Pover 动画系统

2026-10-10 旋转模式更新：正式主角以 Free 为默认；固定层的原方向图保留为 ReservedStrafe，9 个方向资源入口按模式选择、4 个 OrientationWarping 消费模式权重。主图／固定层已编译保存，Pivot 仍禁用。下文较早的 ControllerYaw=True 基线保留历史；当前接口与验证见[旋转配置指南](../../../../../Docs/Guides/character-facing-configuration.md)及[实现报告](../../../../../Docs/Validation/character-facing-implementation-2026-10-10.md)。完整锁定目标服务和锁定 CameraMode 未实现。

> 2026-10-06 状态同步：正式主图现为 ABP_Pover_Base；固定层/FullBody/Stop 保留，Start 无入口、Pivot 禁用。角色旋转锁按 Timeline，武器可见 Mesh 在 WeaponOnBack 插槽停靠，检测 Mesh 留手部。下文迁移验收为历史过程，当前路径已同步。 当前项目事实见 [本轮更新](../../../../../Docs/KnowledgeBase/26-update-2026-10-06.md)。

2026-10-05 将已验收的 CodexText 动画系统迁入本目录。正式角色 `/Game/Main/Character/Hero/BP_Hero_Pover` 已切换到下列资源，现有 Experience、PawnData、PlayerState ASC、输入及默认剑初始化继续使用原调用链。

## 当前运行资源

- 主动画：`/Game/Main/Character/Hero/Anim/ABP_Pover_Base`，父类仍为 `HodgeAnimInstance`。
- 固定层：`/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase`。主图默认 Linked Layer 和角色 BeginPlay 的 LinkAnimClassLayers 指向同一类，装备不切换移动资源。
- 接口：`Layer/ALI_Pover_LocomotionInterface`；枚举、结构体位于 `Types`。曲线压缩设置统一使用根目录的 `UniformIndexableCurveCompressionSettings`，与原始动画共用同一配置。
- 网格及骨架：`Model/SKM_Pover_LyraLab`、`Model/SK_Pover_LyraLab`。保留已验证的脚踝虚拟骨骼及与原 Wuwa 骨架的兼容设置；名称中的 LyraLab 暂时保留。
- 移动序列：`Sequences/A_Pover_*`；BlendSpace 位于 `BlendSpaces`。
- 全身攻击：`Montages/AM_Attack01_Montage` 至 `AM_Attack05_Montage`，全部使用 FullBody；对应源序列位于 `Sequences/Attack/AM_Attack01` 至 `AM_Attack05`。现有 DefinitionCombo、旧 GA_Attack 和相关测试定义的蒙太奇引用随迁移更新，未重写能力或连击逻辑。

常态角色旋转保持验收设置：UseControllerRotationYaw=True、OrientRotationToMovement=False、UseControllerDesiredRotation=False、RotationRate.Yaw=720、MaxWalkSpeed=420。尚未实现冲刺 GA。

2026-10-08 受击接入：`HitReactions/DA_Hero_HitReaction` 保存六类目标动作与恢复配置，强动作使用原 FullBody。AdditiveHitReact 调整到全身动作之后，独立 HodgeHitFeedback Group，上身 Bip001Spine 分支按 0.35 权重混合。轻反馈源序列为项目自有的 Local Space Additive 副本，不修改原 Wuwa 动画。配置和实际验证见[主角受击接入](../../../../../Docs/Validation/hero-hit-reaction-setup-2026-10-08.md)。

Start 仍无入口，Idle/Stop 开始移动直接进入 Cycle。Stop、连续镜头转身及 FullBody 攻击时抑制腿部 IK 的修正保留；EnablePivot=False，保留 Pivot 资源和图供后续冲刺使用。

迁移时按用户授权清理了原 Main 的旧 `ABP_Pover_Base`，新迁入主图最初命名 `ABP_Pover_Lyra`，随后用户将它改名为当前 `ABP_Pover_Base`；同时清理旧 Layer、根目录旧枚举/结构体和旧 Pover BlendSpace。正式角色使用上述主图和固定层。实验地图及测试角色继续位于 `/Game/CodexText/LyraAnimation/Maps`、`Test`，它们也引用已迁入 Main 的资源。

## 迁移前备份

位置：`E:/Project/Backups/Hodgepodge/AnimationMigration/20261005-124146`。

备份包含整个 `Content/Main`、`Content/CodexText`，以及 `Source`、`Config` 和项目描述，共 938 个文件、约 645 MB。迁移前逐文件验证 SHA-256，迁移后再次验证备份全部文件；备份位于 Content 之外，不参与 UE 资产注册。

`manifest-disk-before.json` 记录原始文件、哈希及当时 Git 状态。附带的 `migration-plan.json` 给出全部 52 个资产的源路径和目标路径；`RESTORE.md` 说明恢复方法。不要在编辑器运行时用文件复制覆盖资产；恢复前也应备份恢复时的当前工作。

## 迁移阶段验证

- 52 个资产通过 UE AssetTools.rename_assets 移动并更新引用，没有直接复制 uasset 二进制冒充迁移。实际运行资源不再依赖旧的 CodexText 动画、攻击序列或蒙太奇路径；现有 GAS 定义与测试基础设施仍可位于 CodexText。
- 7 个相关蓝图重新编译并保存，compilerStatus 全部为 UpToDate，无错误或编译警告。包含主图、固定层、接口、Main 角色、旧 GA_Attack 和两个实验测试蓝图。
- `/Game/ThirdPerson/Maps/ThirdPersonMap` 单人 PIE 实际生成 Main 的 BP_Hero_Pover，使用新主图、固定层及默认剑。1346 条记录覆盖前后左右 420 cm/s 移动、Stop、持续双向镜头转动、跳跃/下落和 Enhanced Input→GAS→FullBody 攻击。Pivot 始终禁用；Stop 权重达到 1 并衰减至约 0.012，随后进入静止转身。StopAnimWeight 是节点回调诊断值，状态退出后可能保留最后采样值，不能将它当作实时状态权重查询。
- 五段 FullBody 攻击采集 644 条姿势记录。满权重且混合稳定时，骨盆、腿和脚与原序列姿势的最大位置误差约 0.125 cm，未出现迁移后腿部重新被 IK 固定的问题。
- 同一正式地图双人 Listen Server PIE 采集 360 条记录，覆盖 Authority、Autonomous Proxy、Simulated Proxy，均使用新主图/固定层，取得移动及攻击蒙太奇/标签数据。测试等待 Pawn/Experience 完成初始化后执行，不覆盖晚加入、丢包、独立进程或完整连击链。
- 原 Main 已存在的 51 个资产中，仅 BP_Hero_Pover 和 GA_Attack 在本轮改变；原 Main 旧动画资产保持不变。Source、Config 和项目描述共 292 个文件与迁移前备份哈希相同，本轮无 C++ 修改，未重复 Editor/Game 构建。未执行 Cook、打包或独立服务器构建。
- 结束时停止 PIE，恢复 1 玩家 Standalone 与原后台 CPU 节流，正式地图保持打开，无未保存地图/内容包。资产移动自动更新了相关 CodexText 定义及历史测试副本中的路径；迁移前内容均可从外部备份恢复。

实际主要命令（PowerShell；脚本、回执、导出和原始记录位于 Saved，不纳入源码）：

```powershell
python -X utf8 Saved/LyraAnimationWork/backup_for_main_migration.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/inspect_main_migration.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/plan_main_migration.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/move_animation_assets_to_main.py
python -X utf8 Saved/LyraAnimationWork/link_main_hero_layer.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/configure_main_hero_animation.py
python -X utf8 Saved/LyraAnimationWork/compile_main_migration.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/audit_main_migration.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/open_main_animation_test.py
python -X utf8 Saved/LyraAnimationWork/start_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_main_animation_runtime.py
python -X utf8 Saved/LyraAnimationWork/prepare_main_pose_probe.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_main_fullbody_pose.py
python -X utf8 Saved/LyraAnimationWork/update_migrated_animation_tools.py
python -X utf8 Saved/LyraAnimationWork/prepare_main_network_probe.py
python -X utf8 Saved/LyraAnimationWork/stop_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/configure_main_network_pie.py
python -X utf8 Saved/LyraAnimationWork/start_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_main_network.py
python -X utf8 Saved/LyraAnimationWork/stop_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/restore_main_pie_settings.py
python -X utf8 Saved/LyraAnimationWork/validate_main_migration.py
```

详细结果位于 `Saved/LyraAnimationWork/MainMigration-20261005/verification-summary.json`。只验收正式玩法时，打开 ThirdPersonMap 并 Play 即可。CodexText 下的原测试脚本已更新资源路径，仍可在实验地图运行；迁移脚本属于一次性操作，不应重跑。

## 重复资产清理（2026-10-05）

删除前另做备份：`E:/Project/Backups/Hodgepodge/AnimationDuplicateCleanup/20261005-131233`，共 77 个文件、约 77 MB，覆盖当前 Main 动画目录、CodexText 动画实验目录及两个需要解除旧引用的角色蓝图；逐文件验证 SHA-256。原迁移前备份保持不变。

共清理 12 个资产/残留文件：

- 根目录的 `ABP_Pover_Base`、`AnimEnum_CardinalDirection`、`AnimEnum_RootYawOffsetMode`、`AnimStruct_CardinalDirections`、`AnimStruct_TurnInPlaceEntry`、`Pover`。
- Layer 中的 `ABP_ItemAnimLayers_Pover_Base`、`ABP_Pover_Dark`、`ALI_ItemAnimLayers`。
- 未引用的 `BlendSpaces/BS_Pover_Fallback`。
- 重复的 `Types/UniformIndexableCurveCompressionSettings`。原根目录配置仍被大量 Wuwa/CodexText 动画使用，因此保留它；30 个 A_Pover 移动序列改为使用这个同类、同参数的 UniformIndexable 配置，未修改原始 Wuwa 动画。
- CodexText 中早期创建、无法加载且无引用的 `Animation/ALI_Pover_Locomotion.uasset` 占位文件。实际接口 `ALI_Pover_LocomotionInterface` 保留。

删除前核对 Asset Registry 的硬引用、软引用、管理引用、可搜索引用及强制加载后的包引用。旧蓝图还有两处外部引用：ThirdPerson 示例角色已改用新动画实例、网格及固定层，保留原生 Character 父类与输入；近战测试角色实际使用 ALS，因此只移除不匹配的旧 LinkAnimClassLayers 节点，保留原有 ALS 动画实例和模型。没有把这些示例/测试角色改为新的正式 Pawn 或重写初始化流程。

旧蓝图、类型和 BlendSpace 通过 UE 删除。重复压缩配置的磁盘引用已解除，但 UE 因内存仍持有配置对象而报告 ForceDeleteObject 卸载失败；确认无未保存工作后正常关闭编辑器，在编辑器关闭期间删除这个精确的残留文件及无效接口占位文件，再重开验证。MCP 的退出控制台命令被策略拒绝，原交互编辑器改用正常关闭窗口操作；没有强制结束含未保存工作的用户编辑器。

清理后的结果：

- Main 动画目录剩余 51 个 UE 资产，删除目标在磁盘和注册表中均不存在，未发现对它们的引用。新类型、Stop、Turn、Pivot、攻击资源均保留，EnablePivot=False。
- 主图、固定层、接口、正式角色及两个处理引用的角色共 6 个蓝图编译状态全部 UpToDate，无错误或编译警告。
- 正式地图单人 PIE 采集 809 条记录，覆盖四方向移动、Stop、持续双向镜头转向、跳跃/下落及真实 GAS 攻击；Stop 权重达到 1，FullBody 攻击权重达到 1，使用新 Main 蒙太奇，未触发 Pivot。本轮没有重新做双人联机、五段原始姿势对比、近战碰撞回归或 ThirdPerson 模板的完整玩法验证。
- 本轮没有 C++ 或项目配置修改，未重复 Editor/Game 构建或 Cook/打包。确认无未保存地图/内容包后恢复编辑器。

主要实际命令（完整回执和删除预检存于 `Saved/LyraAnimationWork/DuplicateCleanup-20261005`）：

```powershell
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/inspect_animation_duplicates.py
python -X utf8 Saved/LyraAnimationWork/backup_duplicate_cleanup.py
python -X utf8 Saved/LyraAnimationWork/rewire_duplicate_consumers.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/configure_cleanup_template.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/cleanup_animation_duplicates.py
python -X utf8 Saved/LyraAnimationWork/compile_duplicate_cleanup.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/audit_duplicate_cleanup.py
python -X utf8 Saved/LyraAnimationWork/start_pie.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/LyraAnimationWork/verify_cleanup_runtime.py
python -X utf8 Saved/LyraAnimationWork/stop_pie.py
```

两处残留文件是在确认编辑器已退出、路径位于项目内且已有备份后，用 PowerShell 的 `Remove-Item -LiteralPath` 精确删除，没有递归删除目录。清理脚本为本轮一次性操作，不应重跑。备份目录附有恢复说明与精确删除清单。
