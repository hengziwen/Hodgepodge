# 共鸣技能／解放图标验收

> 本文记录初版显示验收。后续发现的UIExtension和Designer接入缺口已单独修正；当前结构与新测试见[Layout接入复核](resonance-ability-layout-2026-10-15.md)，不以本文原测试结果证明架构约定已满足。

日期：2026-10-15；环境：UE 5.5.4，Windows Development，PCD3D_SM6。对应[配置与实现说明](../Guides/resonance-ability-icons.md)。工作区已有其他未提交任务，本次未回退或自动提交。

## 1. 交付范围

- E/R圆形图标替换临时宽条，占用既有HUD右下区域。
- E/R共用基于真实GAS冷却的纵向亮色恢复和中心倒计时。
- R独立的能量环：顶部起点，顺时针0～100%。
- 能量与CD独立；保留原数据源、服务器权限、输入Tag、生命条及终焉入口。
- 图标、材质、暗度、颜色、字号和布局可调整；参考图片原样存档，纹样提供可替换PNG及SVG源。

核心代码：`HodgeAbilityIconWidget.h/.cpp`、`HodgeResonanceEnergyWidget.h/.cpp`、`HodgeGameplayUIDataSource.h/.cpp`。编辑器装配与诊断在既有`HodgeUIAuthoringLibrary`，限定蓝图编译白名单补充E图标。单元测试：`Private/Tests/HodgeAbilityIconTests.cpp`。

## 2. 构建与原生测试

本轮在保存并关闭编辑器后执行常规构建，随后冷启动编辑器；未使用Live Coding替代构建。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.UI+Hodge.Resonance' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/AbilityHUD/Automation' '-abslog=E:/Project/Git/Hodgepodge/Saved/AbilityHUD/automation.log'
```

- Editor：退出0，11个构建动作，约95.75秒。
- Game：退出0，10个构建动作，约83.93秒。
- 原生自动化：10通过、0失败、0警告、0未运行。
- 新增`Hodge.UI.AbilityIcon.Cooldown`覆盖CD开始／中途／结束、普通技能不依赖大招资源、非法Duration、校正后Remaining超过Duration。
- 新增`Hodge.UI.AbilityIcon.EnergyIndependent`覆盖75%能量与半CD同时存在、满能量不清CD、NaN能量、资源晚到及技能撤销。

其余8项为共享UI／共鸣资源回归，完整用例名称和结果保存在同目录`resonance-ability-icons-2026-10-15-results.json`。

## 3. 资产与实际渲染

经MCP网关在编辑器执行`Tools/AbilityHUD/configure_assets.py`和`validate_assets.py`，后者13／13通过：两个UI材质域和混合方式、Progress参数、E/R输入标签、纹样及材质引用、键帽、能量环开关、正式HUD槽位、三个相关蓝图原生编译。只保存本次目标资产，未批量重存其他蓝图。

单人PIE实际Slate截图执行`Tools/AbilityHUD/verify_screenshots.py`，6／6通过：

- 0%四象限均无亮弧。
- 25%仅右上象限亮。
- 50%右上、右下象限亮。
- 75%增加左下象限，左上仍暗。
- 100%四象限完整亮环。
- 半CD时上方纹样明显更暗、下方已恢复亮色。

这些检查使用实际像素，不仅检查CPU参数。截图尺寸1516×450；未声称覆盖所有画幅、DPI与GPU。

## 4. PIE与网络

实际命令：

```powershell
& 'E:/Python/python.exe' Tools/AbilityHUD/run_batch.py single listen dedicated
& 'E:/Python/python.exe' Tools/HitReaction/mcp_execute.py Tools/AbilityHUD/validate_assets.py
& 'E:/Python/python.exe' Tools/AbilityHUD/verify_screenshots.py
```

单人、双玩家Listen Server、双客户端PIE Dedicated分别21／21通过。后两种模式均选择远端Autonomous Proxy作为UI拥有者，通过真实Enhanced Input释放E/R；服务器官方能量GE仅作为确定0／25／50／75／100数值的夹具。

每种模式覆盖：正式HUD挂载、初始可用性、E/R位置、五个能量状态、E进入8秒CD／中段恢复／自然结束、E不消耗R能量、R消耗100进入12秒CD、R不进入E的CD、冷却中回满不会提前可用、其他玩家能量不变、R冷却自然结束、重建HUD后无重复层、生命条和终焉入口布局不变。

测试截图关闭现有命中调试框；结束后停止PIE并恢复原地图`/Game/CodexText/L_MainMenu`、性能开关和三个命中调试开关。最终PIE为0，未保存地图／内容包均为0；Play设置恢复单人基线。

## 5. 调试过程中修复及未验证边界

- 资产脚本适配UE 5.5：CustomInput改为无参构造后赋属性，ComponentMask使用实际默认输入名；错误时脚本失败，不输出假成功。
- 首次环形材质编译发现VectorParameter默认只有RGB输出，不能直接读取Alpha；已拼接RGB和A为float4。最终资产和实际GPU像素检查均通过。旧日志中的中间编译失败不代表最终材质仍失败。
- 蓝图初始化期间可见既有`AM_Burst01`同时间点Branching Point重叠警告。本轮未修改攻击通知；R资源消耗及HUD断言通过，不代表已验收通知重叠对翅膀／状态标签的影响。
- 没有加入新的独立UI计时器、技能冷却或能量规则；没有增加网络RPC。
- 图标基于小图重绘，高清纹样仍可由美术替换。材质与图标并非原作资产的精确提取。
- 未执行独立进程服务器、Cook／打包、所有分辨率、鼠标硬件点击或手柄适配；按键及UI输入入口沿用现有实现。没有采集C++行／分支覆盖率，不将用例数换算成覆盖率。

原始日志、截图、构建输出和Lyra资产检查位于`Saved/AbilityHUD`；摘要JSON保存在Docs，避免只依赖本机缓存查看结果。
