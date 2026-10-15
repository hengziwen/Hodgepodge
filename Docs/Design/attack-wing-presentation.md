# 攻击限定翅膀设计

版本：1.3；2026-10-13；UE 5.5.4。配置说明见 [翅膀通知与蒙太奇配置](../Guides/wing-notify-montage-configuration.md)。

## 1. 背景与目标

Anzhu 翅膀具有独立骨架与攻击动画，仅在攻击中出现，结束后材质淡出。旧设计把动作资源和时钟配置分散在 Profile 映射表与通知中，新增角色攻击仅放通知仍不能驱动翅膀。本次直接在动画通知状态配置翼蒙太奇，保留已有挂接、Box 判定和取消清理，不新增 Wing GA、管理器或 Runtime 模块。

## 2. 决策与参考

已核查本机 Lyra 的 Source/LyraGame 和 GameFeature 源码，没有可直接迁入的独立 Wing 蒙太奇实现。参考 AnimNotify_LyraContextEffects::Notify 的通知转交拥有者/组件方式；直接使用 UE UAnimNotifyState、UAnimInstance::Montage_Play、FAnimMontageInstance 和现有 Hodge Definition 清理入口。

沿用角色主蒙太奇复制与本地源位置，不增加表现 RPC。翼蒙太奇在翼 AnimInstance 播放，不能通过 ASC 的角色 PlayMontage 抢占主攻击。旧程序化资产仍保留并禁用，旧映射仅保留序列化兼容，正式运行不读取。

## 3. 模块和接口

- WingAction NotifyState：通知条定义 [Begin, End)；WingMontage、PlayRate、StartPosition、FadeOutSeconds 定义本次动作。
- WingProfile：公共网格、翼 AnimBP、挂点、偏移、材质、默认淡出及权威采样开关；新模式不显示旧程序化参数和映射表。
- WingComponent：创建/复用网格，查询本地源的通知窗口，播放与同步翼蒙太奇，维护执行身份、显隐、淡出和清理。
- GameplayAbility_Definition：沿用 EndWingForExecution 清理，不新增技能生命周期。
- WaitHitResults：沿用 PrepareAuthorityHitPose 与统一命中查询，WingMesh 保持 NoCollision。
- Editor Authoring：接入独立翼 AnimBP Slot、创建翼蒙太奇、迁移通知；Box 校准读取通知中的蒙太奇。

InitializeWing、RefreshPresentation、EndWingAction、EndWingForExecution、ResetWingPresentation、PrepareAuthorityHitPose 继续使用。BeginWingAction 的索引现为源动画通知索引，保留原名字以兼容诊断和蓝图。WindowId 和旧 AnimationBindings 不再是制作入口。

## 4. 数据与流程

WingTime = clamp(StartPosition + max(SourceTime - NotifyBegin, 0) × PlayRate, 0, WingMontageLength)。倍率有限且大于零；起播位置在蒙太奇时长内；淡出 -1 沿用 Profile、0 立即隐藏、正值覆盖本次时长。动画使用翼骨架和 DefaultSlot。

初始隐藏 → 源进入通知窗口 → 校验通知 → 建立会话 → 播放翼蒙太奇并求值正确姿势 → 显示。翼实例位置由角色源秒校正，自动推进关闭；到末尾保持末帧。通知 Begin/End 唤醒组件刷新；持续查询补偿跳段、晚加入和漏过通知，避免在身体骨骼刷新内重入翼求值。

正常离开窗口/主动画停止 → 停止姿势驱动 → 保留最后局部姿势淡出 → 完全隐藏。攻击取消使用执行身份抑制旧源复活；死亡、Avatar 更换和销毁立即清理。旧句柄不能结束新窗口，通知对象不存角色运行状态。

重叠窗口后开始者接管，结束后查询仍合法窗口。推荐单动作一个显示窗口；不用窗口引用计数构建额外状态机。预览 SingleNode 身体源循环时递增身份，翼本身不循环。

## 5. 命中、网络与性能

Box 继续配置在角色 Hodge 持续命中通知：NamedComponent/WingMesh、Box、真实翼根骨骼、Follow。手与翼共用 HitGroup + AttackPhase 时同刀去重，显隐与伤害窗口独立；伤害窗口需要有效翼动作窗口。材质尾迹不延长伤害。

更新顺序：角色姿势 → 翼组件同步 → 翼骨骼 → PostPhysics 命中。专服保留开启权威采样的隐藏翼网格，命中请求按需刷新当前角色挂点和翼姿势；不渲染、不创建 MID。纯表现专服跳过网格。拥有者和观察者消费已有角色蒙太奇复制，不发任意资源播放 RPC。

每个角色自己的 MID，不重复创建网格；空闲/淡出不额外求值翼骨骼。协调组件进行当前源窗口检查，暂停复用同位置姿势。翼蒙太奇用于表现/姿势，不在其内部另配伤害或技能事件。

## 6. 异常与兼容

挂点、资源、骨架或 AnimBP 错误：初始化失败并诊断，不无限重试。通知非法：当前窗口关闭，每个通知对象只报告一次。永久错误与正常等待区分。Profile 生命周期内只读，资源更换需明确重建组件。

保持 WingText 现有组件、挂接和 0.8 统一缩放；不修改模型导入 0.01 比例，不覆盖用户通知时间和 Box 尺寸。新蒙太奇默认零动画混入，避免源动画翼根与参考姿势差异造成显现跳变；淡出使用材质。非零动画混入须内容制作确认基础姿势匹配。

旧程序化类型/字段和旧资产保留兼容，隐藏于新模式；不新增多层资源提供者、独立网络能力或跨骨架同步组。

## 7. 验收与需求追溯

- WING-01/05/10：初始隐藏、首帧姿势正确、空闲无翼骨求值。
- WING-02/03/14：通知配置即可播放所选翼蒙太奇，无需 Profile 映射；半开窗口、重叠接管、倍率和起播位置有效。
- WING-04：暂停、变速、时间跳转和晚加入按角色源时间同步。
- WING-06/12/13：正常结束和取消淡出，死亡/Avatar 更换/销毁清理；旧句柄不影响新动作。
- WING-07/08/11：骨架和参数失败关闭，旧模式回归，预览挂接与网格复用。
- WING-09：Listen Server 与 PIE Dedicated 中拥有者/观察者播放和取消一致。
- WING-HIT-01～04：翼 Box 单独命中、范围外拒绝、同刀去重、取消/淡出无伤害；专服按需姿势有效。

常规 Editor/Game 构建、原生单元与世界集成、资产检查、真实输入 PIE 和 Box 伤害需分别记录，不能用构建成功代替玩法验收。独立专服进程、Cook/打包和主观美术手感另行验证。

历史实现与测试范围见 2026-10-12 的 attack-wing-presentation、wing-fade、wing-box-hit 验证报告，不作为本轮复跑结果。
