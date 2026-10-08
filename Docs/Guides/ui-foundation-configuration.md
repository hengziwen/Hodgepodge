# UI 基础配置与 UMG 制作入口

本轮将正式界面迁入 UMG Designer，保留现有 CommonUI／CommonInput／UMG／Slate，不引入 CommonGame、CommonUser 或 ModularGameplayActors。通用框架与一个共享数据源负责生命周期和玩法接入，界面布局与交互在 WBP 中编辑。

## 正式资产与父类

- `/Game/Main/UI/Foundation/WBP_HodgePrimaryLayout`：HodgePrimaryGameLayout，四个层栈。
- `/Game/Main/UI/HUD/WBP_HodgeHUD`：HodgeHUDLayout，HUD 布局、扩展点与 Escape 菜单入口。
- `/Game/Main/UI/HUD/WBP_PlayerVitals`：CommonUserWidget，生命显示。
- `/Game/Main/UI/HUD/WBP_AbilityBar`：CommonUserWidget，技能项容器及统一刷新。
- `/Game/Main/UI/HUD/WBP_AbilitySlot`：CommonUserWidget，单个技能项。
- `/Game/Main/UI/Foundation/WBP_UIButton`：CommonButtonBase，可复用按钮模板。
- `/Game/Main/UI/Menu/WBP_HodgePauseMenu`、`WBP_HodgeConfirmation`：HodgeActivatableWidget，菜单与一次性确认结果。
- `/Game/Main/UI/EAS_HodgeGameplayUI`：Experience AddWidgets 配置。
- `Foundation/B_HodgeUIInputData`、`DT_UIInputActions`：返回／确认输入。

这些界面现在都有保存的控件树，Designer 能看到并编辑。CommonUserWidget／CommonButtonBase 属于引擎 CommonUI，与 CommonUser 插件没有依赖关系。

## 调整血条

打开 `WBP_PlayerVitals` 的 Designer：`VitalsSize` 控制宽度，`VitalsBackground` 控制底色和边距，`HealthText` 控制字体／颜色，`HealthBar` 控制进度条外观。这里的预览默认显示 HP 75/100；进入游戏后由真实数据覆盖，预览数值不改变角色属性。

血条摆放位置在 `WBP_HodgeHUD` 的 `PlayerVitalsPoint` 的 Overlay Slot 中调整，包括对齐与 Padding；替换成 Canvas 布局也可以按 UMG 的锚点／位置控制，但须保留对应扩展点和 Tag。

Graph 的 `ApplyVitals` 接收数据快照并设置文字与进度；Construct 绑定共享数据源，Destruct 解绑自己的事件。改外观不需要修改 C++。重命名被 Graph 使用的控件时用蓝图编辑器的 Rename，并编译确认相关节点仍有效。

## 调整技能栏

打开 `WBP_AbilityBar`，在 Class Defaults 编辑 Slots：Label 为显示名称／键位提示，InputTag 为实际能力输入意图。默认普攻为 InputTag.Ability.Melee，跳跃为 InputTag.Jump。展示新的槽位不代表自动授予技能，授予仍由 AbilitySet／装备等玩法系统负责。

Designer 的 `AbilityBackground` 控制背景／边距，`AbilityEntries` 控制动态条目布局与 EntrySpacing。其 EntryClass 是 `WBP_AbilitySlot`。单个技能的外观在 `WBP_AbilitySlot` 编辑，按钮公共样式在 `WBP_UIButton` 编辑；LabelText 是按钮显示文字。

Graph 的 InitializeSlot 接收配置和共享数据源，RefreshState 查询实际可用状态／冷却，按钮点击调用 SubmitInput。普攻继续由 CombatComponent 选段。技能栏仅一个 0.1 秒刷新定时器，移除界面时清理；没有配置冷却 GE 的技能不显示伪造倒计时。

## 调整菜单与确认框

打开两个 Menu WBP 的 Designer，编辑 `PanelSize`、`PanelBackground`、`Content`、标题和按钮位置。按钮使用 `WBP_UIButton` 模板；菜单的 ResumeButton／ConfirmButton，确认框的 AcceptButton／CancelButton 在 Graph 中绑定各自行为。

菜单 Class Defaults 的 ConfirmationClass 指向正式确认 WBP。两页 InputConfig=Menu 并启用返回处理；BP_GetDesiredFocusTarget 返回 Designer 中的默认按钮。HUD 的 InputConfig=Game，EscapeMenuClass 指向正式菜单 WBP。

确认框的 Finish 是蓝图函数，先标记完成，只在页面仍激活时主动关闭，再广播一次结果；使用函数参数避免关闭时的重入覆盖接受结果。菜单撤销自己拥有的确认框并解绑；控件池复用、Esc、外部 Pop 与 HUD 卸载都需要保持这一清理流程。增加 UMG 动画时不要删掉这些生命周期 Graph。

## 四层与两个扩展点

根布局四个必需容器名字是 GameLayer、GameMenuLayer、MenuLayer、ModalLayer，对应 UI.Layer.Game／GameMenu／Menu／Modal。原生 BindWidget 注册这些层；根容器默认 TransitionDuration=0，切换立即完成，页面自身的过渡可以使用 UMG 动画；这些框架容器不要随意重命名。布局和大小可以在 Designer 编辑。

HUD 的 PlayerVitalsPoint／AbilityBarPoint 对应 UI.Slot.PlayerVitals／UI.Slot.AbilityBar。Experience 的 Layout 将 HUD 放入 Game 层，Widgets 按 Slot Tag 和 LocalPlayer Context 注入血条与技能栏。

根布局 Designer 显示层结构，HUD Designer 显示扩展点占位；运行时注入内容在各自子 WBP 中完整预览。这两张框架布局没有正在运行的 Experience，因此不会自动出现整套游戏实例。

## 数据、输入与配置

HodgeUIManagerSubsystem 为各 LocalPlayer 持有根布局和一个 HodgeGameplayUIDataSource。数据源读当前 Pawn／ASC／属性就绪，换 Pawn 解绑重绑，失效后拒绝输入；它不负责样式或服务器伤害。新增页面从自己的 OwningPlayer 查询根布局／数据源，不用全局 PlayerController(0)。

菜单只阻断自己的游戏输入，打开时释放能力按住状态、清空连段缓存与移动意图，关闭不重放旧输入，也不暂停联机服务器。已有根布局异步加载／取消仍由框架处理。

配置 RootLayoutClass 与 CommonInput InputData 在 DefaultGame.ini，ViewportClientClassName 在 DefaultEngine.ini，Escape Action 在 DefaultInput.ini。默认 Experience 已引用 EAS_HodgeGameplayUI，不必重复添加。

## 制作工具与边界

多人 PIE 测试时，在 Editor Preferences → General → Performance 关闭 `Use Less CPU when in Background`（编辑器失去焦点时降低 CPU 使用率）。本机 2026-10-08 已将 `EditorPerformanceSettings.bThrottleCPUWhenNotForeground=False` 保存到 UE 5.5 的用户 `EditorSettings.ini`，并通过 `RELOADCONFIG EditorPerformanceSettings` 确认磁盘加载后仍为 False；这是本机编辑器偏好，不是游戏 Runtime 配置。

测试收尾不得硬编码把此项设回 True。若临时改变其他编辑器偏好，应记录并恢复测试前值，保留用户明确要求的关闭后台限速设置。所有 UE 窗口最小化时，引擎仍可能限速；应在游戏窗口可见并操作时测量实际帧率。此次配置修正未验证前台多人稳定 60 FPS；后台采样不能当作 GPU 瓶颈或 UI 性能退化的证据。

MigrateDesignerAssets 已完成的资产会跳过，不覆盖 Designer 或 Graph；CreateFoundationAssets 不再创建空的业务原生父类 WBP。工具只在编辑模式使用，PIE 中拒绝制作／保存资产。旧原型 C++ 在 Archive/UI/NativePrototype，迁移前备份在 Saved/UMGAuthoringRefactor/Before。

设计见 [UMG 父类精简方案](../Design/umg-authoring-ui-refactor.md)，测试工具见 [Tools/UIFoundation](../../Tools/UIFoundation/README.md)。本轮实际验收结果见 [UMG 重构报告](../Validation/umg-authoring-refactor-2026-10-08.md)；以前的 UI 原型报告保留历史范围。完整设置、库存、登录、匹配、发布打包和技能图标美术不属于本轮交付。
