# 锁定标记 WBP 与 Dash 起手修复 · 2026-10-13

## 交付与配置

正式标记为 `/Game/Main/UI/Targeting/WBP_HeroLockOnIndicator`，父类 HodgeLockOnIndicatorWidget。DA_HeroLockOn.IndicatorClass 已引用它，原生 RebuildWidget 优先使用 Designer 根控件，空树才回退原生菱形。

- `MarkerSize`：SizeBox，Width Override／Height Override 初始 36×36。
- `MarkerGlyph`：TextBlock，初始“◇”、Font Size=26、Bold、白色。可改字体、颜色、文字，或换成 Image／材质。
- 修改尺寸时同时考虑字体尺寸；SizeBox 变大不会自动放大字形。位置由目标锚点投影，不要硬写屏幕坐标。
- Blueprint 已 Compile／Save 并在编辑器打开。作者工具只初始化空树，后续不会覆盖用户 Designer 内容。

正式 Dash Profile `/Game/Main/Character/Hero/Ability/Sprint/DA_Hero_SprintAbility` 的 `bInstantFacingAtStart=True`，新 Profile 默认也改为 true。保留原有后向锥形 B 和其他方向 F、攻击中断映射、约束、网络确认、防御／衔接窗口和恢复。

## 停顿原因与修正

原配置 False 时，DashPreparing 先阻止普通移动，ActionFacing 按转向速率逐渐对齐；只有 IsActionFacingApplied 成立后才播放蒙太奇、启动 RMS。锁定时角色持续面敌，侧向／斜向的世界移动方向经常与身体不一致；后撤保留身体朝向，所以通常不等待。

本次使用已经存在的即时朝向机制，在提交前同次对齐角色，不通过提高行走转向速率或取消权限检查掩盖等待。False 仍可用于刻意设计的转身准备，不是当前即时 Dash 的正式配置。

另补齐 SActorCanvas.OnIndicatorAdded 的 UpdateActiveTimer：空列表停止计时后，新增目标需重新启动投影更新。新增 UI 诊断显示描述符数量、实际控件是否就绪及所属世界。

源码：

- [Profile 默认值](../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)、既有 Dash.BeginMovement／UpdateMovement／CommitDash。
- [尊重 Designer 树](../../Source/Hodgepodge/Private/UI/Targeting/HodgeLockOnIndicatorWidget.cpp)、[画布恢复更新](../../Source/Hodgepodge/Private/UI/IndicatorSystem/SActorCanvas.cpp)。
- HodgeUIAuthoringLibrary 的 ConfigureLockOnIndicator 和 InspectPlayerUI。
- [即时朝向测试](../../Source/Hodgepodge/Private/Tests/HodgeCharacterFacingTests.cpp)、[Designer 实际尺寸测试](../../Source/Hodgepodge/Private/Tests/HodgeLockOnIndicatorTests.cpp)。

## 实际测试结果

- Editor／Game Win64 Development 常规构建退出 0，编辑器关闭后构建并冷启动。
- **33/33 原生回归 Success**：LockOn、Facing、Rotation、Movement、UI；新增 DashInstantStart 和 LockOnIndicatorDesigner。覆盖九方向即时应用、130° 前向与后锥边界、以及 Designer 64×48／80×48 真正进入 Slate。
- **四种模式各 36/36 方向检查**：single、listen、dedicated PIE、listen-lag（100ms）；每组测试 0、±45、±90、±135、180、130 度。
- 每种模式的九个方向，拥有者动画和实际胶囊位移均在 PressedAt 的同一游戏帧被观察到，报告 animation_delay／movement_delay 为 0.0。含义是没有额外转身等待，不是物理设备端到端零毫秒。
- F/B 语义、角色实际朝向、各端确认目标保持通过。不是仅检查 GA 已激活。
- 单人原锁定／切换／Dash／Sprint／死亡回归 **20/20**；本轮未重复全部相机专项。
- WBP 已在实际 Indicator 中出现；独立运行时实例把 MarkerSize 改为 64×64 后实际 DesiredSize 为 **64×64**，随后恢复。正式 WBP 样式保持 36／26，供用户自行调整。
- 编辑器原地图／性能恢复，PIE=0、无未保存包；完整原播放选项未记录，恢复单人基线。

源文件／资产和报告 SHA256、逐项结果见[机器可读记录](lock-on-indicator-dash-start-2026-10-13-results.json)。原始帧数据、配置前备份、构建／失败日志在 Saved/LockOnDash，不提交生成缓存。代码未自动提交。

## 验证中的问题与边界

UI 软类需要等待加载；Python 的 EditorAssetLibrary 不能在 PIE 内加载 Blueprint 类，部分原生 C++ Getter 不暴露给 Python，WidgetTree 是受保护字段。夹具改用运行时对象与独立 UI 检查，没有伪造通过结果。早期失败记录保留。

隔离怪物放到地面外曾触发 CheckStillInWorld／DamageSelfDestruct 现有异常并退出编辑器。夹具改为停止隔离角色物理移动；本次没有修改死亡系统，该分支根因仍待单独调查，不能把这次修复当成已解决 KillZ／自毁问题。

方向夹具仅在 PIE 实例临时使用胶囊 Bounds 锚点，消除胸口细抖对 ±135° 锥形边界的影响；正式 Guardian 骨骼锚点／武器插槽未改动。网络测试验证拥有者起手与各端目标状态，不将模拟代理播放时刻描述为没有网络延迟。物理键盘手感、丢包／抖动、独立服务器与打包未验收。

## 重跑命令

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.LockOn+Hodge.Facing+Hodge.Rotation+Hodge.Movement+Hodge.UI' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/LockOnDash/Automation' '-abslog=E:/Project/Git/Hodgepodge/Saved/LockOnDash/automation.log'
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/LockOn/configure_indicator_dash.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOnDash/start_single.py
& E:/Python/python.exe Saved/LockOnDash/run_remaining.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOnDash/start_regression.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOnDash/resize_live_ui.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOnDash/check_live_ui.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOnDash/restore.py
```

方向工具为 Tools/LockOn/run_dash_directions_e2e.py，用 start_test.py 的 LOCK_ON_RUNNER／LOCK_ON_WORK 指定。默认仅测启动时序，UI 独立检查避免异步加载／绘制影响计时。没有覆盖率插桩，不提供虚构覆盖率百分比。
