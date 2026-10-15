# 翅膀通知与蒙太奇配置

适用：UE 5.5.4；2026-10-13 的 Wing 通知蒙太奇实现。旧映射表不再参与正式播放。

## 1. 制作入口

打开角色的攻击蒙太奇，在通知轨添加 **Hodge Wing Action Window** 动画通知状态。拖动通知条决定显示窗口，选择该通知后直接配置：

- `WingMontage`：使用翅膀骨架的蒙太奇，不能选择主角蒙太奇。
- `PlayRate`：相对角色攻击源时间的倍率，默认 1；必须大于零。
- `StartPosition`：通知开始时播放的翼蒙太奇位置，单位秒，默认 0。
- `FadeOutSeconds`：-1 使用 Profile 默认值；0 立即隐藏；正值覆盖本次淡出时长。

同一次攻击不再需要去 Profile 添加角色动画映射。通知条开始时先准备正确翼姿势再显示；通知条结束时保留最后姿势并材质淡出，完成后隐藏。通知结束即停止本窗口的动画驱动，不等同于延长攻击伤害。

## 2. 现有资产

- 正式组件：`/Game/Main/Character/Hero/Wing/BP_PoverWing_Anzhu`。
- 公共配置：`/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu`。
- 翼动画蓝图：`/Game/Main/Character/Hero/Wing/ABP_PoverWing_Anzhu`，独立翼骨架，Reference Pose → DefaultSlot → Output。
- 第四号翼蒙太奇：`/Game/Main/Character/Hero/Wing/Montages/AM_Wing_Attack04`，来源 B_Attack04。
- 第五号翼蒙太奇：`/Game/Main/Character/Hero/Wing/Montages/AM_Wing_Attack05`，来源 B_Attack05。
- 角色通知迁移：`/Game/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage`、`AM_Attack05_Montage`。
- 用户预览：`/Game/Main/WingText`，身体仍播放既有 AM_Attack04，翼组件复用原网格和挂接。

编号对应动画资源，不是连段序号；现有连段顺序仍为 01 → 02 → 03 → 05 → 04。迁移保持用户当前通知条的开始、结束、轨道及已有命中配置。

## 3. Profile 只配置公共资源

使用 AnimationAsset 模式，配置 `Mesh`、`AnimClass`、`AttachSocket`、`MeshOffset`、三个材质和默认淡出时间。旧 `AnimationBindings` 与 WindowId 仅保留旧资产序列化兼容，不再显示为编辑入口。旧程序化时序、姿势参数仅在 Procedural 模式显示；旧资产及禁用的 Experience 条目保留。

翼蒙太奇使用 DefaultSlot，并与 AnimBP 的 Slot 一致。新资产默认零动画混入、零混出，以避免导入翼动画与参考姿势根位置差异造成闪现；隐藏过渡由材质淡出负责。需要动画混入时可调整蒙太奇 Blend In，但应先提供合适的基础姿势并检查翼根位置。

## 4. 时钟、取消与命中

`WingTime = clamp(StartPosition + max(SourceTime - NotifyBegin, 0) × PlayRate, 0, WingMontageLength)`。

组件播放真实翼蒙太奇，随后由角色源时间校正其位置；暂停不推进翼姿势，变速和晚加入按当前角色位置恢复，不建立第二条自主计时链。翼动画到末尾保持末帧，显隐仍由通知条决定。翼蒙太奇用于表现和骨骼姿势；伤害、手持与技能事件继续放在角色攻击蒙太奇，不依赖翼蒙太奇自身的通知。

Dash 取消、受击等攻击结束沿用 Definition 的执行身份清理，停止旧翼窗口；死亡、Avatar 更换和组件销毁立即清理。旧通知结束只请求查询当前窗口，不能关闭下一次攻击。

同一来源窗口重叠时，后开始的窗口接管；结束后查询当前仍有效窗口。建议每次翼动作只放一个显示通知条，连续动作使用相邻窗口。

Box 判定仍使用现有 **Hodge 持续命中**：NamedComponent / WingMesh / Box / Bone_Wing000 / Follow。显示条与伤害条独立，伤害条必须位于有效翼显示窗口内。专服不渲染，但启用 `bEnableAuthorityHitSampling` 后仍持有隐藏翼网格，命中任务按当前角色时钟准备翼蒙太奇姿势。同刀去重继续使用 HitGroup + AttackPhase。

## 5. 工具与验证

`Tools/WingAttack/configure_montages.py` 迁移已有通知并创建缺失的翼蒙太奇/AnimBP；已配置的通知和已存在的蒙太奇不重置用户参数。运行前要求无 PIE 和未保存资产。不要运行旧 configure_assets.py 重建映射配置。

验证覆盖通知参数、骨架兼容、真实蒙太奇实例与骨骼运动、首次姿势、暂停、窗口结束、淡出、取消及旧句柄、死亡/Avatar 更换、预览复用、多人表现与 Box 单独伤害。实际结果以本轮验证报告为准。
