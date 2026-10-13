# 参考主 HUD 配置

对应 [用户原图](../References/HUD/main-hud-reference.png)。本轮仅显示生命条与终焉入口，其余图示功能不制作。

## 正式资产

- `/Game/Main/UI/HUD/WBP_HodgeHUD`：ReferenceHUDCanvas；PlayerVitalsPoint 使用底部中心锚点，位置 `(0,-12)`、对齐 `(0.5,1)`、自动大小；EndButton 使用右上锚点，位置 `(-26,20)`、对齐 `(1,0)`、尺寸 `64×70`。
- `/Game/Main/UI/HUD/WBP_PlayerVitals`：VitalsSize 为 `320×20`；血条右侧留 52 Slate 单位给等级。HealthText 显示真实生命／最大生命，LevelText 显示 PlayerState 的真实等级。
- `/Game/Main/UI/Art/T_EndIcon`：透明底白色圆环图标，原始生成文件在 Tools/HUDReference/Art/end-icon.png。
- `/Game/Main/UI/Menu/WBP_HodgeEndMenu`：终焉占位页，显示“敬请期待”，返回按钮／Escape 关闭。正式终焉业务不在本轮范围。
- `/Game/Main/UI/EAS_HodgeGameplayUI`：HUD 的 Layout 保留，Widget 扩展仅启用 PlayerVitals。旧技能栏和菜单资产保留。

位置单位遵循项目 UMG DPI 规则，底部居中和右上锚点适配不同视口；不把原图说明文字、白框、能量条或其他入口画进 HUD。

## 数据和交互

血条继续由当前 LocalPlayer 的 HodgeGameplayUIDataSource 订阅 Health／MaxHealth／属性就绪事件。等级加入同一快照。未就绪显示 `--/--`、`Lv. --` 和空进度；死亡与更换 Pawn 沿用现有解绑／重绑。

EndButton 的 OnClicked Graph 调用 HUD.ToggleHUDMenu，与 Escape 共用所属菜单的加载、去重、取消和停用清理。页面打开后由 CommonUI 阻断当前玩家玩法输入；关闭恢复。游戏保持既有视角和鼠标捕获策略，Escape 可直接打开入口；PIE 中需要直接鼠标点 HUD 时使用编辑器的 Shift+F1 释放鼠标。生产环境的按住 Alt 显示光标需求另行设计，不在本轮改写角色输入。

## 制作与验证

Tools/HUDReference/configure_assets.py 配合已构建的 Editor 作者函数制作资产。ConfigureReferenceHUD 带版本标记，复跑不重复生成控件或 Graph；需要重新制作时先备份并明确处理标记，不能为方便直接覆盖后续 Designer 手工修改。

Tools/HUDReference/test_hud.py 使用真实 PIE 世界和原生 Tick 调度，验证属性刷新、按钮委托→Blueprint、菜单返回、Escape、HUD 卸载／重建与双客户端隔离。按钮事件派发测试不等于桌面鼠标输入验收；截图和运行结果分别记录在验证报告。
