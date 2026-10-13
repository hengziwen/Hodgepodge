# 参考主 HUD 实施与验证 · 2026-10-11

UE 5.5.4。[原图存档](../References/HUD/main-hud-reference.png)、[配置指南](../Guides/main-hud-reference-configuration.md)、[本阶段待办](../KnowledgeBase/29-current-work-and-backlog.md)。仅制作生命显示和终焉入口，未实现终焉业务、Inventory 或攻击吸附，翅膀保持禁用，用户修复的 Guardian 插槽／行走不修改。

## 改动

- 正式 WBP_HodgeHUD 使用底部中心和右上角锚点。PlayerVitals 改为细长浅绿血条、当前／最大生命与真实等级；数据继续来自各 LocalPlayer 的共享源。
- FHodgeUIVitalsSnapshot 增加 CharacterLevel，Refresh 比较等级变化并发布；未就绪等级为 0，界面显示占位。
- HUD.ToggleHUDMenu 是既有菜单流程的蓝图入口，只在 HUD 激活时响应。按钮与 Escape 共用所属异步菜单、去重与停用清理，不新增 GA 或服务器功能。
- 新增 WBP_HodgeEndMenu 与 T_EndIcon；页面只显示“敬请期待”、返回／Esc。保留旧菜单和技能栏资产，EAS 的 Widget 扩展只保留生命条。
- 作者工具保留同一主 cpp，新增 ConfigureReferenceHUD、运行位置采样和两种按钮测试动作；普通 Game 不依赖 Editor 作者工具。

## 构建与静态检查

关闭编辑器后执行常规构建，未使用 Live Coding：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.UI' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/HUDReference/Native' '-abslog=E:/Project/Git/Hodgepodge/Saved/HUDReference/Native.log'
git diff --check
```

Editor 最终代码构建 EditorBuild5.log 退出 0；GameBuild.log 退出 0，其后只改 Editor 作者工具、脚本、资产和文档，Runtime 未再修改。原生 UI 3/3 Success、0 失败、0 未运行、0 带警告成功；其他构建既有 IncludeOrder 等提示保留。Python 六份脚本 py_compile 通过。日志与原始报告在 Saved/HUDReference，不纳入源码。

## PIE 验收

编辑器 gateway 命令统一为 `E:/Python/python.exe Tools/HitReaction/mcp_execute.py <脚本>`：

1. Tools/HUDReference/configure_assets.py：制作／保存，复跑不重复 Graph 和按钮。
2. Tools/HUDReference/start_validation.py、test_hud.py：单人正式 Experience。
3. Tools/AnimNotifyCombat/stop_pie.py、Tools/HUDReference/start_network.py、test_hud.py：Listen Server 双玩家。
4. stop_pie.py、Tools/HUDReference/restore_validation.py：结束并恢复原地图／PIE 设置／后台限速偏好。

最终冷启动回归：单人 **12/12**、Listen Server 双玩家 **12/12**，两份报告均 error=null。包含真实伤害、等级变化、按钮委托→Blueprint、返回按钮、Escape、HUD 卸载／重建、客户端隔离和实际 CachedGeometry 位置断言。

生命条中心归一化 x≈0.5，y≈0.977／0.980；入口中心 x≈0.983／0.970，y≈0.058／0.051。实际 Slate 截图覆盖宽视口 1516×422 和远端窗口 1280×722，早期 640×482 窗口也观察到正确锚点。三份正式 WBP 最终独立编译并保存，状态均 UpToDate；重开编辑器后模板 Slot 为 Fill，菜单标题为“终焉”，不靠内存修改。

机器可读摘要见 [结果 JSON](main-hud-reference-results-2026-10-11.json)。最终 PIE 已结束，恢复原地图 /Game/CodexText/L_MainMenu、原 PIE 配置与限速偏好，无未保存包。原图与保存副本 SHA256 一致。

![远端客户端 HUD](HUD/main-hud-client.png)

![终焉入口页](HUD/end-menu-client.png)

## 发现的问题与修正

- 第一次常规 Editor 构建的软类引用转换不匹配：改用 GeneratedClass.Get()，后续构建通过。
- 旧 PlayerVitals 已有 GameplayData 变量：复用变量，避免重新制作 Graph 时重复添加；未保存的失败结果先从磁盘恢复。
- Canvas 和 ProgressBar 的 Overlay Slot 默认 Left／Top：改为 Fill，实际截图复核。
- UMG setter 不自动将内容包标记脏，默认 save_loaded_asset 跳过保存：明确使用第二参数 False 保存，再重开编辑器确认模板 Slot 和菜单标题。初版关键字 only_if_dirty 不符合本机 Python API，改用已核实的参数。
- 普通 game_viewport 截图不包含 Slate HUD：以 UI 作者工具的 Slate Capture 检查实际界面；不据缺少 Slate 的截图判断 UI 消失。
- Computer Use 的 node_repl 在初始化及重试时均报 kernel assets 路径不存在；本轮改用 UE 原生测试和 Slate 截图。桌面鼠标点击／手柄人工体验未验收，不能将委托派发等同于鼠标坐标输入。

## 限制

未执行 Cook／打包、独立进程、手柄或代码覆盖率插桩。保留既有游戏鼠标捕获策略；Escape 可直接进入，PIE 直接点 HUD 使用 Shift+F1。生产环境 Alt 光标交互暂未实现。其他待办仍按最新复核保持待办，不宣称全项目所有未开发功能已闭环。未提交 Git。
