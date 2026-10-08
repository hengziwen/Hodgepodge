# Lyra → Hodgepodge 迁移现状与检查清单

> 复核日期：2026-10-08（本次更新 UI 项，其他条目保留各自验收范围）。这是可持续更新的迁移清单，不是全部系统已验收的声明。

[项目文档目录](../README.md) · [知识库](../KnowledgeBase/README.md) · [当前接通状态](../KnowledgeBase/12-integration-backlog.md)

对照工程：

- Lyra：`E:/Project/UProject/study/Epic/LyraStarterGame`，UE 5.5。
- Hodgepodge：`E:/Project/Git/Hodgepodge`，UE 5.5.4。

依据为两边当前源码、uproject、Build.cs、Config，以及此前只读资产检查。这里的“系统”指业务子系统，不要求拆成新的 UBT 模块；Hodge 业务继续维持单一 Runtime 模块。

## 迁移现状

| 系统 | Hodge 当前状态 | 缺口／建议 |
| --- | --- | --- |
| 死亡流程 | 死亡 GA 已授予，事件和死亡阶段已测试 | 互斥能力计数／PIE 清理错误待处理；死亡表现和重生待补 |
| UI 基础闭环 | 自有根布局、Experience HUD、真实血条、基础技能栏与菜单已接通 | 不引入 CommonGame／CommonUser／ModularGameplayActors；完整前端、独立进程和打包待后续验证 |
| GameplayCue | Manager 已迁入，生命周期未接完整 | 补目录注册、启动初始化、Feature 注销与加载策略 |
| 战斗消息 | 消息调用保留为注释，消息类型与 Router 未接 | 迁入 GameplayMessageRouter 后建立最小战斗消息，明确与 Cue 的分工 |
| 加载画面 | 部分迁入：接口与配置存储已有 | 需要转场时接 CommonLoadingScreen，补实际显示、输入阻断和就绪检查 |
| 队伍／阵营 | Team 字段与部分接口预留，统一目标规则已有 | 做敌人时先补最小阵营契约，再按需要扩展完整 Teams 系统 |
| 库存／快捷栏 | 核心未迁入；装备系统已接通 | 固定默认武器阶段可暂缓；库存、装备和快捷切换职责分开 |
| 交互 | 仅占位组件 | NPC、宝箱、拾取阶段再迁入目标查询、交互接口与 GA |
| 核心框架 | Experience、ASC、Init State、输入／相机主要链路已接 | 按需求补生命周期专项验证，不重复建设已有基础 |

## 建议顺序

1. **死亡 GA 与实际战斗闭环**：优先补小而关键的死亡事件消费。正式五段 HitWindows 仍为空，填写命中配置是现有系统接线工作，不是继续迁移 Lyra 框架。
2. **UI 最小闭环已接通首版**：后续随玩法补图标、设置和业务页面；不要重复引入 CommonGame 框架。
3. **GameplayCue 与战斗消息**：补项目生命周期与最小命中／死亡反馈，不混用两套职责。
4. **敌人和阵营**：明确敌人 ASC 所有者、队伍接口、伤害许可及复制。
5. **按玩法需要迁库存、交互和加载画面**；设置、联网登录、匹配和完整前端另行安排。

## 逐项检查

### 1. 死亡流程：部分迁入，优先补齐

Lyra 的 `LyraGameplayAbility_Death` 监听 `GameplayEvent.Death`，取消不保留的能力，切到阻塞激活组，再调用 HealthComponent.StartDeath；能力结束时调用 FinishDeath。

Hodge 已由用户迁入 HodgeGameplayAbility_Death，并在 `/Game/Main/Data/AbilitySet/DA_Pover` 授予 GA_Hero_Death。真实致死伤害能够通过 [HealthComponent](../../Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp) 发送事件，由服务器 GA 开始/结束死亡，拥有者和模拟代理同步阶段并销毁 Pawn。但攻击中死亡及结束 PIE 出现互斥能力计数与活动 Spec 清理错误，整体验收尚未通过，详见 [本次验证](../Validation/jump-death-2026-10-06.md)。此前“普通 GA 授予为空”是迁入前状态。

- [x] 迁入并按 Hodge 命名适配死亡 GA，服务器消费 Death 事件。
- [x] 将死亡 GA 授予到实际默认 AbilitySet，确认不会重复授予。
- [ ] 接死亡表现与开始／结束清理；蒙太奇 Slot 使用当前动画系统契约。
- [ ] 验证零血量只触发一次、取消战斗／清理手持请求、禁移与 Pawn 清理。
- [ ] 验证服务器、拥有者、模拟代理；另行定义重生与保留能力策略。

### 2. UI 基础闭环：已接通首版

用户明确选择保留 UGameInstance／ULocalPlayer 和现有 Controller，不引入 CommonGame、CommonUser、ModularGameplayActors。现有引擎 CommonUI／CommonInput 配合自有管理器与根布局实现所需契约；GameInstance 原有 Init State 初始化仍保留。

- [x] 现有依赖完成根布局、LocalPlayer 与 Controller 生命周期；专服不创建 UIManager。
- [x] GameViewportClient 路由、CommonUI 输入配置、菜单焦点和玩法输入清理。
- [x] Experience Client Bundle 加载、Layout／HUD Extension 注入与撤销。
- [x] 正式 Main 资产、真实血条、基础普攻／跳跃栏、菜单和确认框。
- [x] 单人、Listen Server／两个纯客户端的菜单隔离与重建；100ms 连段／移动取消回归。
- [ ] 独立进程、Cook／打包、完整设置／登录／匹配前端。

设计见 [Hodge UI 基础闭环](hodge-ui-foundation.md)，配置见 [UI 配置指南](../Guides/ui-foundation-configuration.md)，运行证据见 [本轮验证](../Validation/ui-foundation-2026-10-08.md)。旧 UIPolicy／CommonGame 迁移文件仍作为停用学习材料，不应据文件存在判定已启用完整前端。

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

2026-10-06 的原清单为静态核对；2026-10-08 更新的 UI 项已实际实现并验证，其他条目仍按各自历史报告判断，不扩展本轮通过范围。
