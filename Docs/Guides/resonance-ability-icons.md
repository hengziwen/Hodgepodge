# 共鸣技能／共鸣解放图标配置

日期：2026-10-15；引擎：UE 5.5.4；替换此前临时共鸣解放宽条，不修改技能消耗、冷却、伤害和充能规则。后续复核已修正直接挂载和运行时创建布局的问题：当前使用Experience注入扩展点，实际控件树保存在WBP中。

## 1. 实际行为

- E共鸣技能：白色蝶形纹样、半透明圆底和E键帽。技能进入CD后图标变暗，亮色从底部向顶部恢复，中央显示向上取整的剩余秒数；CD结束隐藏数字。
- R共鸣解放：相同的圆形图标和CD恢复表现；外侧金色能量环独立读取当前能量／上限。0%无亮弧，25%到3点，50%到6点，75%到9点，100%完整一圈。
- 满能量不会移除CD；CD完成但能量不足也不能释放。图标按钮和键盘E/R复用现有输入Tag及共享UI数据源，不直接调用GA、支付资源或创建GE。
- 未授予或资源尚未复制到达时透明等待；显示每帧重新读取当前玩家数据源，因此不缓存旧Pawn的属性或旧GA实例。

参考截图原样保存在`Docs/References/HUD/AbilityIcons/`。当前白色纹样根据低分辨率参考重新绘制，提供透明PNG与SVG源，不宣称是原作高清图标；后续可直接换正式美术贴图。

## 2. 资产入口

编辑器中打开以下资产：

- `/Game/Main/UI/HUD/WBP_Resonance_Skill`：E图标，父类`HodgeAbilityIconWidget`。
- `/Game/Main/UI/HUD/WBP_Resonance_Energy`：R图标，保留旧资产路径及`HodgeResonanceEnergyWidget`兼容父类，后者继承通用图标控件。
- `/Game/Main/UI/HUD/WBP_HodgeHUD`：`ReferenceHUDCanvas`只放两个`UIExtensionPointWidget`，保留血条与终焉入口；不再直接放两个技能Widget实例。
- `/Game/Main/UI/EAS_HodgeGameplayUI`：既有AddWidgets Action的Widgets配置将两个图标分别注入`UI.Slot.ResonanceSkill`、`UI.Slot.ResonanceLiberation`，使用所属LocalPlayer上下文与Client资源包。
- `/Game/Main/UI/Art/AbilityIcons/T_ResonanceSkill`、`T_ResonanceLiberation`：256×256透明纹样。
- 同目录`M_AbilityIcon_Cooldown`、`M_AbilityEnergy_Clockwise`：UI域半透明材质；每个控件创建自己的MID，不修改共享材质实例。

## 3. 修改位置、大小和样式

在`WBP_HodgeHUD`的Designer选中`ResonanceSkill`／`ResonanceEnergy`，修改Canvas Slot。当前锚点与Alignment均为(1,1)，E偏移(-196,-24)，R偏移(-48,-24)，两个Slot均76×100，含下方键帽区域。不同分辨率沿用项目现有DPI规则。

在对应子类Class Defaults修改：

- `IconTexture`：替换透明白色纹样。建议正方形、UI纹理组、保留Alpha，不将场景截图直接作为纹样。
- `IconMaterial`：冷却圆底与图标材质；保留参数契约。
- `EnergyRingMaterial`：能量环材质；E的`bShowEnergyRing`关闭，R开启。
- `CooldownDarkness`：CD未恢复区域的亮度，当前0.25，范围0～1。
- `FillColor`：R能量环颜色及透明度，当前金色(1,0.85,0.35,1)。CD不会灰化此环。
- 字体、字号、阴影：直接在Designer中选中`CooldownText`／`KeyText`调整。旧`FontSize`只保留为首次制作初值，运行时不再覆盖Designer字体。
- 图标尺寸：在Designer修改根`IconSize`（SizeBox）、`IconCanvas`各子项以及按钮区域；同步HUD扩展点Slot尺寸。旧Class Defaults的`IconSize`只供首次制作工具使用，运行时不重新排版。
- `KeyLabel`／`Caption`：键帽文本／悬浮提示。这里只改变显示，不重绑输入；换键还需修改实际IMC。
- `PreviewEnergyPercent`／`PreviewCooldownReadyPercent`：仅Designer预览，运行时忽略；后者0为刚进入CD、1为完成。

两个WBP已保存完整Designer树：根`IconSize`、`IconCanvas`、`SkillIcon`、`EnergyRing`、`CooldownText`、`KeyBackground/KeyText`及`AbilityButton`。打开Designer即可直接编辑。原生控件不再实现创建子树的`RebuildWidget`，只保留共享数据读取、MID参数和输入提交。图像与按钮为兄弟控件，避免禁用按钮将能量环整体灰化；修改布局时保留绑定名称。

## 4. 数据和材质契约

挂载链路：`Experience → EAS_HodgeGameplayUI/AddWidgets → UI.Layer.Game/WBP_HodgeHUD → UIExtensionPointWidget → 两个WBP`。Action负责注册／撤销ExtensionHandle，扩展点负责创建和移除所属Widget；HUD重建后由现有UI框架重新注入，不额外AddToViewport。

数据链路：`PlayerState ASC / ResonanceSet → HodgeGameplayUIDataSource → HodgeAbilityIconWidget → 每控件MID → Slate`。

数据源成对读取同一活跃GE的Remaining和Duration；存在多个匹配GE时选剩余最长者及其配对总时长。UI冷却恢复比例为`Clamp(1-Remaining/Duration,0,1)`，不使用独立Timer或材质Time。总时长无效而仍在CD时保持暗色；能量非有限数或资源未就绪时不向材质提交非法值。

`M_AbilityIcon_Cooldown`参数：`IconTexture`、`Progress`（冷却亮色恢复0～1）、`CoolingDown`（0／1）、`Darkness`（未恢复亮度）、`Unavailable`（非CD的其他不可释放状态）。UV的Y向下，因此亮色遮罩从`Y=1-Progress`以下向上扩展。数字由UMG绘制，不嵌入贴图。

`M_AbilityEnergy_Clockwise`参数：`Progress`（真实能量比例）、`ColorA`（金环RGBA）。角度使用`frac(atan2(UV.x-0.5,0.5-UV.y)/(2π)+1)`，显式处理0和1端点，避免空环留一条亮缝或满环缺口。

UI只影响本地玩家。继续读取服务器权威GE及属性的拥有者复制／预测结果，不增加网络消息；不会把本地材质进度作为伤害或释放授权。

## 5. Lyra复用依据

核查本机`E:/Project/UProject/study/Epic/LyraStarterGame`：

- `Source/LyraGame/UI/Basic/MaterialProgressBar.cpp`的`SynchronizeProperties`、`GetBarDynamicMaterial`及进度参数写入路径；本项目已有对应`UI/Basic/MaterialProgressBar`。
- 在Lyra编辑器实际加载并导出`/ShooterCore/Game/Dash/W_DashCooldown`，核对`UpdateCooldownPercent`、Duration及MID参数更新。
- 实际加载`/ShooterCore/UserInterface/HUD/W_AbilityProgress`和径向材质`/Game/UI/Foundation/Materials/M_UI_RadialProgress`；前者用于交互进度，不能误认为已存在的共鸣技能冷却UI。

沿用“每图像独立MID＋归一化进度”模式。现有`MaterialProgressBar`要求绑定填充动画，而本需求同时使用实际GAS冷却和独立能量，因此新增薄的复用Widget，不为满足该绑定创建虚假动画。没有迁入整套ShooterCore依赖，也没有新增另一套冷却状态机。

本次实际Lyra资产导出与检查结果位于`Saved/AbilityHUD/Lyra/`，它们是本机调查证据，不作为项目依赖或入库资产。

## 6. 工具与验证

- `Tools/AbilityHUD/create_icons.py`：从几何源生成PNG／SVG；不修改参考截图。
- `configure_assets.py`：创建UI材质、配置E/R子类并挂载既有HUD；仅处理目标资产。此工具会应用交付默认样式，用户调参后不要反复运行来覆盖自己的样式。
- `migrate_layout.py`：将旧直接挂载迁为扩展点，为空原型创建序列化Designer树；保留非空用户布局和已有样式，不重新生成图标或材质。重复执行不增加注入条目。
- `run_batch.py single listen dedicated`：实际E/R输入、服务器GE夹具、拥有者UI读取及HUD重建；复用既有验证地图与PIE生命周期。会临时关闭命中调试绘制、调整性能设置，结束后按下述方式恢复。
- `verify_screenshots.py`：核查实际截图的金环四象限和半CD的上下亮度，防止CPU参数正确但GPU材质错误。

恢复：通过项目MCP网关执行`Tools/CinematicUltimate/restore_editor.py`，传全局`LOCK_ON_WORK=项目绝对路径/Saved/AbilityHUD`。它读取本轮保存的原地图、性能和调试开关；PIE设置恢复单人基线，不声称完整还原未快照的PIE选项。

初版显示结果见[原验收记录](../Validation/resonance-ability-icons-2026-10-15.md)；挂载与Designer修正后的结果见[Layout接入复核](../Validation/resonance-ability-layout-2026-10-15.md)。
