> 2026-10-08：运行时原型布局已迁入 WBP Designer，业务控件父类已由共享数据源替代。本文保留首版框架设计；现行制作方式见 [UMG 重构方案](umg-authoring-ui-refactor.md) 和 [配置指南](../Guides/ui-foundation-configuration.md)。

# Hodge UI 基础闭环（2026-10-07）

## 范围与约束

用户授权实施：保留 UGameInstance、ULocalPlayer 和当前 Controller；使用已经启用的 CommonUI、CommonInput、UMG、Slate 与项目 UIExtension，不引入 CommonGame、CommonUser、ModularGameplayActors。业务仍在 Hodgepodge Runtime，资产工具放既有 Editor 模块。旧 UI 计划作为历史背景。

第一版完成正式 Experience 注入、真实生命属性、基础技能输入／状态、菜单／确认框、玩家隔离与资源撤销。不增加角色常驻 UI 组件。不制作登录、匹配、完整设置、库存或加载屏管理器。

## 所有权和生命周期

UIManagerSubsystem 属于 GameInstance，为每个 LocalPlayer 保存根布局和 Controller 设置委托。LocalPlayer 加入时注册并立即检查 Controller，Controller 就绪后创建 PrimaryGameLayout 并 AddToPlayerScreen；移除玩家或更换 Controller 时取消旧异步请求、移除旧根布局并通知订阅者。管理器不在专服创建。

根布局属于当前 LocalPlayer／Controller；Pawn 更换仅重绑控件的数据。根布局就绪事件在层注册、挂入玩家视口后发送；AddWidgets 同时支持立即获取和订阅就绪，避免启动先后顺序造成漏注入。

PrimaryGameLayout 管理四个现有 UI.Layer：Game、GameMenu、Menu、Modal。原生默认树提供四个 Stack；Blueprint 可自行提供树并注册相同层。Push／Pop 只接受 CommonActivatableWidget，异步请求有唯一句柄、弱 Controller 校验和成对输入挂起／恢复，根布局释放时取消全部请求。第一版由管理器配置根布局类，不新增 Policy。

## Experience 和 UIExtension

现有 AddWidgets Action 按 Context＋HUD 保存注入记录；重复 ExtensionAdded／GameActorReady 不重复注入。Actor 移除或 Context 停用时显式撤销 Layout、Extension、就绪委托和异步请求。资源通过 Client Bundle 加载，冷加载需要实际验证。

正式资产位于 /Game/Main/UI：Foundation 的根布局、HUD 的布局／血条／技能栏、Menu 的菜单／确认框、EAS_HodgeGameplayUI。默认 Experience 的 ActionSets 加入该 ActionSet。UI.Slot.PlayerVitals 和 UI.Slot.AbilityBar 是新的 HUD 插槽标签，已有 UIExtension 按 LocalPlayer Context 注入。

UIExtensionPointWidget 保存 PlayerState 设置委托，重建／释放时解绑，PlayerState 变化时撤掉旧 Context，避免重复注册。UIManager 不持有玩法属性或授予能力。

## 数据与输入

血条绑定当前 Pawn 的 HealthComponent：绑定后读取初值，监听 Health／MaxHealth／死亡，Pawn 更换时解绑重绑；等待 PawnExtension 的 ASC 就绪，控件结束时解除订阅。真实等级／装备属性仍由现有属性系统负责。

技能栏使用配置的输入 Tag 作为入口，普攻由 CombatComponent 选段，跳跃通过现有 ASC 输入路径；不固定激活 GA_Attack_1。冷却仅显示真实 GAS 效果的剩余时间，未配置冷却不伪造计时。输入可用状态由当前数据就绪、死亡与 UI 路由综合决定。

HodgeGameViewportClient 继承引擎 CommonUI 的 UCommonGameViewportClient。CommonUI 配置 Game／Menu、焦点和返回键；管理器响应输入模式变化，发送已按下能力的释放、清空 ASC／Combat 缓存和 Hero 移动意图。Hero／Controller 入口在 UI 阻断时不处理新的玩法输入，恢复时不重放旧缓存。联机菜单只阻断当前玩家，不暂停服务器。

## 原生默认界面与作者入口

原生 Widget 构造最小可用布局，正式 WBP 继承这些类；美术可通过提供自己的 WidgetTree 替换原生默认树。血条、技能栏、菜单操作通过公开属性／事件更新，框架不逐个认识业务界面。菜单有继续游戏和确认框，ConfirmationClass 指向正式 WBP；菜单保存并撤销自己创建的 Modal，确认框通过 Modal 层返回焦点；异步失败与取消必须释放输入挂起。

## 验收

- Editor／Game 常规构建，原生生命周期／资源撤销测试。
- 正式 WBP 编译、冷加载与资产依赖校验。
- 单人 HUD 初值、伤害、MaxHealth 变化、死亡／换 Pawn、菜单开关及焦点恢复。
- 启动时根布局／Action 先后次序、重复注入、卸载／再注入、异步加载取消与失败、离图。
- Listen Server／Client 的各自血量、菜单输入隔离、100ms 延迟下正式连段回归。
- 独立进程、Cook／完整打包按实际执行单独记录。

本方案已实现，实际构建／资产／PIE／联机结果和独立问题见 [2026-10-08 验证报告](../Validation/ui-foundation-2026-10-08.md)；配置见 [UI 配置指南](../Guides/ui-foundation-configuration.md)。备份仅相关源码／配置／默认 Experience，位置 Saved/UIFoundation/Before；不创建全项目备份，不提交用户工作区。
