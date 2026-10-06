# Lyra → Hodgepodge 迁移现状与检查清单

> 复核日期：2026-10-06。这是可持续更新的迁移清单，不是全部系统已验收的声明。

[项目文档目录](../README.md) · [知识库](../KnowledgeBase/README.md) · [当前接通状态](../KnowledgeBase/12-integration-backlog.md)

对照工程：

- Lyra：`E:/Project/UProject/study/Epic/LyraStarterGame`，UE 5.5。
- Hodgepodge：`E:/Project/Git/Hodgepodge`，UE 5.5.4。

依据为两边当前源码、uproject、Build.cs、Config，以及此前只读资产检查。这里的“系统”指业务子系统，不要求拆成新的 UBT 模块；Hodge 业务继续维持单一 Runtime 模块。

## 迁移现状

| 系统 | Hodge 当前状态 | 缺口／建议 |
| --- | --- | --- |
| 死亡流程 | 部分迁入：生命值、DeathState、死亡事件发送已有 | 优先核对／迁入死亡 GA，接通事件消费、死亡开始与结束 |
| UI 基础闭环 | 部分迁入：CommonUI、基础控件、UIExtension 已有 | 最值得补完的较大迁移；接 CommonGame 依赖、根布局、输入路由与 HUD 注入 |
| GameplayCue | Manager 已迁入，生命周期未接完整 | 补目录注册、启动初始化、Feature 注销与加载策略 |
| 战斗消息 | 消息调用保留为注释，消息类型与 Router 未接 | 迁入 GameplayMessageRouter 后建立最小战斗消息，明确与 Cue 的分工 |
| 加载画面 | 部分迁入：接口与配置存储已有 | 需要转场时接 CommonLoadingScreen，补实际显示、输入阻断和就绪检查 |
| 队伍／阵营 | Team 字段与部分接口预留，统一目标规则已有 | 做敌人时先补最小阵营契约，再按需要扩展完整 Teams 系统 |
| 库存／快捷栏 | 核心未迁入；装备系统已接通 | 固定默认武器阶段可暂缓；库存、装备和快捷切换职责分开 |
| 交互 | 仅占位组件 | NPC、宝箱、拾取阶段再迁入目标查询、交互接口与 GA |
| 核心框架 | Experience、ASC、Init State、输入／相机主要链路已接 | 按需求补生命周期专项验证，不重复建设已有基础 |

## 建议顺序

1. **死亡 GA 与实际战斗闭环**：优先补小而关键的死亡事件消费。正式五段 HitWindows 仍为空，填写命中配置是现有系统接线工作，不是继续迁移 Lyra 框架。
2. **UI 最小闭环**：根布局、HUD Extension、血条／技能栏与输入路由，先验收一个完整入口。
3. **GameplayCue 与战斗消息**：补项目生命周期与最小命中／死亡反馈，不混用两套职责。
4. **敌人和阵营**：明确敌人 ASC 所有者、队伍接口、伤害许可及复制。
5. **按玩法需要迁库存、交互和加载画面**；设置、联网登录、匹配和完整前端另行安排。

## 逐项检查

### 1. 死亡流程：部分迁入，优先补齐

Lyra 的 `LyraGameplayAbility_Death` 监听 `GameplayEvent.Death`，取消不保留的能力，切到阻塞激活组，再调用 HealthComponent.StartDeath；能力结束时调用 FinishDeath。

Hodge 的 [HealthComponent](../../Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp) 已发送死亡事件并提供状态接口；未发现对应 C++ 死亡 GA。现有 C++ StartDeath/FinishDeath 调用主要在复制通知内，不能把客户端补执行当成服务器的死亡启动入口。此前只读检查中，默认 PawnData 两个 AbilitySet 的普通 GA 授予数组为空，连段通过 Definition 授予；迁移时仍须核对实际授予与事件触发。

- [ ] 迁入并按 Hodge 命名适配死亡 GA，服务器消费 Death 事件。
- [ ] 将死亡 GA 授予到实际默认 AbilitySet，确认不会重复授予。
- [ ] 接死亡表现与开始／结束清理；蒙太奇 Slot 使用当前动画系统契约。
- [ ] 验证零血量只触发一次、取消战斗／清理手持请求、禁移与 Pawn 清理。
- [ ] 验证服务器、拥有者、模拟代理；另行定义重生与保留能力策略。

### 2. UI 基础闭环：最明显的未完成迁移

基础控件、CommonUI 和 UIExtension 有有效代码；24 个 UI 文件仍标记 `UI-MIGRATION-PENDING`。例如 [UIManagerSubsystem](../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h) 和 GameViewportClient 尚未参与编译。[AddWidgets](../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp) 的 Extension 注册部分有效，但 `PushContentToLayer_ForPlayer` 仍注释。

Lyra 的 CommonGame 实际依赖 CommonUser、ModularGameplayActors 等；不能只解除注释而漏掉插件及继承链。Hodge 当前 GameInstance/LocalPlayer 分别继承 UGameInstance/ULocalPlayer，与 Lyra 的 CommonGame 基类存在差异。

- [ ] 核对并接入 CommonGame 的实际依赖，保持 UE 5.5 兼容。
- [ ] 适配 GameInstance、LocalPlayer、UIManager／UIPolicy 和根布局，保留现有 Init State 初始化。
- [ ] 接 GameViewportClient 输入路由与必要配置。
- [ ] 接通 Experience 的 Layout／HUD Extension 注入，制作最小血条／技能栏。
- [ ] 验证添加、撤销、再激活、本地玩家隔离及菜单／游戏输入切换。

完整设置、登录、匹配和前端流程不必和第一轮 HUD 同时实现。详见 [UI 迁移计划](lyra-ui-migration-plan.md)。

### 3. GameplayCue：Manager 已有，生命周期缺口仍在

[GameplayCueManager](../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp) 与 GAS 全局配置已经存在。当前 [Feature Policy](../../Source/Hodgepodge/Private/GameFeatures/HodgeGameFeaturePolicy.cpp) 未创建 Cue 路径 Observer，注销目录主体注释；[AssetManager](../../Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp) 的启动 `LoadAlwaysLoadedCues` 调用也注释，显式 GameplayCueNotifyPaths 未配置。

这不表示基础 GAS Cue 全部不可用；缺的是项目目录、Feature 生命周期及加载策略的完整接入。

- [ ] 配置正式 Cue 内容目录，选择适合项目的加载策略。
- [ ] 接启动初始化及 Feature Cue 路径注册。
- [ ] 配对处理注销、重新扫描和需要的预加载，避免残留路径。
- [ ] 用一个命中特效／音效验证首次播放、取消及网络观察者表现。

### 4. 战斗消息：预留注释，未接 Router

Hodge 的 [HealthSet](../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)、HealthComponent 和 Ability 中保留伤害、淘汰或失败消息的注释；没有完整 VerbMessage/Helpers，Build.cs 也未引用 GameplayMessageRuntime。

- [ ] 接 GameplayMessageRouter，定义最小伤害／死亡／技能失败消息载荷。
- [ ] 确定发布者、监听者和订阅解绑，不让 GA 硬引用 UI。
- [ ] 明确网络传输入口：消息广播本身不会自动跨网络。
- [ ] 伤害仍由服务器 GE 决定；特效／音效走 Cue，血条可直接订阅属性委托。

### 5. 加载画面：已有接口，缺实际管理器

LoadingProcessInterface 与 [LoadingScreenSubsystem](../../Source/Hodgepodge/Private/UI/Foundation/HodgeLoadingScreenSubsystem.cpp) 已有；后者主要保存 Widget 配置，不等于 Lyra CommonLoadingScreen 的 LoadingScreenManager。

- [ ] 需要转场时接实际加载管理器和显示入口。
- [ ] 汇总 Experience、Pawn／本地玩家等就绪条件。
- [ ] 配对显示、输入阻断、失败退出与隐藏，不让画面提前消失或永久残留。

### 6. 队伍／阵营：最小规则已有，完整系统未迁

PlayerState/Pawn 留有 Team 字段，但 PlayerState 的团队接口仍有注释；没有完整 Lyra Teams 子系统。[DamageRules](../../Source/Hodgepodge/Private/Combat/HodgeDamageRules.cpp) 已读取 GenericTeam 接口，未提供队伍时回退 NoTeam。

- [ ] 明确玩家、敌人、友军的最小队伍身份及权威来源。
- [ ] 完成 Actor／Controller 的接口与需要的复制、变更通知。
- [ ] 验证友伤、自伤、无队伍及目标死亡时的过滤。
- [ ] 只有确有需要时再加入 TeamCreation、公共／私有信息和显示资产。

### 7. 库存／快捷栏：未迁核心，装备已完成主链接入

Lyra 有 Inventory Definition/Instance/Manager、Item Fragments 和 QuickBar。Hodge 的 Equipment/WeaponInstance、默认剑与显隐已接通，不能据库存缺失判定“装备未做”。

- [ ] 出现拾取、背包或换武器需求后再迁库存核心。
- [ ] 区分持有物品、当前装备和快捷选择，明确授予／撤销与复制。
- [ ] 按动作 RPG 需求设计 Fragment 与堆叠，避免把 Shooter QuickBar 当完整背包。

### 8. 交互：占位

[InteractionComponentBase](../../Source/Hodgepodge/Private/Component/HodgeInteractionComponentBase.cpp) 当前没有完整交互实现。Lyra 对应 InteractionOption、IInteractableTarget、Interact GA 与扫描／授予任务。

- [ ] 定义交互目标、选项、范围及玩家输入。
- [ ] 接目标扫描和必要的交互能力授予／撤销。
- [ ] 服务端验证交互合法性，完成一次宝箱／拾取或 NPC 交互验收。

### 9. 核心框架：主要迁入，按需补验证

Experience、ASC、PawnData、Init State、输入与相机主链已有接入。固定移动层、统一近战 Combat、连段记忆、旋转窗口和默认剑表现为 Hodge 的需求适配。

- [ ] 专项验证换 Pawn、重生、Feature 卸载／再激活、输入／相机解绑与重新绑定。
- [ ] 按发布目标补独立进程、Cook／打包与 Dedicated Server；现有 Listen Server 不代替专服验收。

以下不是漏迁：

- 不使用常态 Start、暂时禁用 Pivot、不按武器切移动动画层，是当前已确定的需求。
- PlayerState 授予 PawnData AbilitySet 时传 `nullptr`，Lyra 原版也这样写；来源撤销账本是项目增强需求，装备 Manager 自身已有精确撤销。
- 枪械、弹药、竞技比分、匹配等 Shooter 内容不因 Lyra 存在就必须迁入动作 RPG。

## 如何维护

每完成一项，更新日期、当前状态、对应代码／资产路径、实际命令和验证结果，再勾选复选框。代码已复制、编译通过、蓝图接线完成、单人／联机／打包通过分别记录，不用一个“完成”覆盖全部层级。

建议记录：

```text
日期：
系统／检查项：
状态变化：
代码与资产：
实际命令及结果：
未验证项／后续工作：
```

本次仅将既有对照结论落入文档，未实现上述待办，未执行 C++ 构建、PIE 或打包；只验证文档差异、链接和知识库快照。
