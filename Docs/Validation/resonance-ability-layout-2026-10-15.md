# 共鸣图标Layout接入修正

日期：2026-10-15；UE 5.5.4。起因：用户指出两个新图标未遵循现有Layout制作链路。

## 1. 实际问题

初版通过既有`WBP_HodgeHUD`进入`UI.Layer.Game`，因此没有绕开最外层的PrimaryLayout。但内部有两处偏离项目约定：

- 两个图标作为HUD Canvas直接子实例，未由Experience的AddWidgets配置注入扩展点。
- `UHodgeAbilityIconWidget::RebuildWidget`运行时创建控件树，WBP Designer没有保存完整布局；字体在运行时重新设置，无法完整沿用Designer调整。

此前的冷却、能量和显示测试通过，不代表这两项架构约定已经满足。本次补充资产结构、注入归属、撤销和重建验收。

## 2. 修正后的职责

`Experience → EAS_HodgeGameplayUI/AddWidgets → WBP_HodgeHUD（UI.Layer.Game）→ 两个UIExtensionPointWidget → 对应图标WBP`。

- `ResonanceSkill`扩展点：`UI.Slot.ResonanceSkill`，注入`WBP_Resonance_Skill`。
- `ResonanceEnergy`扩展点：`UI.Slot.ResonanceLiberation`，注入`WBP_Resonance_Energy`。
- HUD保留原锚点、偏移、大小、血条及终焉入口，只把两个直接实例替换为扩展点。
- 已有AddWidgets管理LocalPlayer上下文、Client资源包、ExtensionHandle与撤销；没有新增另一套管理器或AddToViewport路径。
- 两个WBP保存实际控件树：SizeBox、Canvas、两个Image、倒计时、键帽和按钮。原生控件不再实现创建布局的RebuildWidget，运行时不覆盖Designer字体。
- 通用`HodgeAbilityIconWidget`保留数据／材质更新和输入接入；`HodgeResonanceEnergyWidget`只保留旧类引用及大招默认值，不承担页面调度职责。叶子控件无须继承HUDLayout。

控件内部排版／字体在两个WBP Designer修改；整组位置在HUD的扩展点Slot修改。输入与冷却仍来自原共享数据源。

## 3. 复用依据

本轮重新核对本机Lyra：

- `Source/LyraGame/GameFeatures/GameFeatureAction_AddWidget.cpp`的`AddWidgets`、`RemoveWidgets`：Layout压入层，叶子Widget按LocalPlayer Context注册Extension并配对注销。
- `Plugins/UIExtension/Source/Private/Widgets/UIExtensionPointWidget.cpp`的`RebuildWidget`、`RegisterExtensionPoint`和`ResetExtensionPoint`：Designer显示插槽占位，运行时创建匹配条目并清理。
- `Source/LyraGame/UI/LyraHUDLayout.cpp`：页面级输入及菜单职责，不替叶子技能控件计算冷却或创建图标。

Hodge对应机制已存在于`GameFeatureAction_AddWidget.cpp`、`UIExtensionPointWidget.cpp`、UIManager与HUDLayout。直接复用现有实现，只补Tag、配置与两个Designer树；不迁入CommonGame或重写核心框架。

## 4. 代码与工具

- Runtime：删除`HodgeAbilityIconWidget`的运行时造树及字体覆盖，保留显示、材质与输入行为。
- Editor：`ConfigureAbilityIconDesigner`只为空原型保存树，非空用户树只校验；`ConfigureAbilityIconHUD`迁移为扩展点并保留原Slot。`InspectPlayerUI`从扩展实例读取实际状态。
- 配置：`Config/Tags/AbilityHUDTags.ini`、`EAS_HodgeGameplayUI`的两条新增Widgets配置。
- `Tools/AbilityHUD/migrate_layout.py`：保留现有CDO样式、材质和输入Tag，不重绘图标，不重复追加注入条目。
- `validate_assets.py`增加Designer绑定、持久字体、扩展点Tag和唯一Experience配置检查。
- `run_e2e.py`增加扩展撤销、重新注入和重建检查；当前总扩展为3个：生命、E、R。

## 5. 实际验证与边界

构建及原生测试的实际命令沿用[初版验收](resonance-ability-icons-2026-10-15.md)第2节，日志与报告目录改为`Saved/AbilityHUDLayout`。Editor、Game常规构建均退出0；原生`Hodge.UI+Hodge.Resonance`10通过、0失败。反射变动后已冷启动编辑器。

资产原生蓝图编译与33项检查通过，包含完整Designer树、持久FontObject、两个扩展点、唯一EAS注入及原布局。首次移动字体制作时发现Slate临时CompositeFont不能持久保存，已改用TextBlock的字体资产，并对无有效字体引用的初版原型修复；已有有效用户字体不覆盖。

单人、Listen Server远端拥有者、PIE Dedicated远端拥有者分别23／23通过，包含原技能显示验证、扩展停用撤销／恢复重新注入及HUD重建。GPU截图6／6通过：空环、四分之一圈、半圈、四分之三圈、满圈，以及半CD上暗下亮。

实际执行`Tools/AbilityHUD/migrate_layout.py`与`validate_assets.py`（经MCP网关），并设置`HODGE_ABILITY_HUD_WORK=E:/Project/Git/Hodgepodge/Saved/AbilityHUDLayout`执行`run_batch.py single listen dedicated`及`verify_screenshots.py`。重复迁移后仍为3条注入配置，没有重复实例。测试后恢复`/Game/CodexText/L_MainMenu`、性能及命中调试开关，PIE与未保存包均为0。

网络／截图最终结果及编辑器恢复记录保存在同名`-results.json`，原始日志和截图在`Saved/AbilityHUDLayout`。PIE Dedicated不等于独立专服；未执行Cook、打包、手柄适配或所有DPI验证。HUD蓝图存在既有未提供手柄焦点提示，本轮不扩展手柄功能。

没有自动提交或回退工作区的其他改动。旧版原型及本轮中间失败报告保留为历史，不冒充最终验证结果。
