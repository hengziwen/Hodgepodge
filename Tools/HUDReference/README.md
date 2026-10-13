# 参考 HUD 制作与验收

UE 5.5.4，使用已常规构建的 HodgeAbilityEditor 和项目既有鉴权 gateway。只修改正式 HUD、PlayerVitals、EAS 的 Widget 条目；新增 EndMenu 和图标。不要在 PIE 中制作或保存。

顺序：create_icon.py（普通 Python）→ configure_assets.py（编辑器）→ start_validation.py → 等待玩家就绪 → test_hud.py → 等待 Saved/HUDReference/single.json 的 error=null → Tools/AnimNotifyCombat/stop_pie.py → start_network.py → test_hud.py → 等待 network.json → stop_pie.py → restore_validation.py。

编辑器脚本通过 `E:/Python/python.exe Tools/HitReaction/mcp_execute.py <脚本路径>` 执行。Started 只表示测试已启动，必须读取最终 JSON；Callback 内包含 45 秒游戏时间上限，失败也会解绑 Slate 回调。

队列 EndButton 派发真实 UButton 委托并执行保存的 Blueprint；MenuReturn 进入 CommonButton 的实际点击处理。此项验证按钮事件链，不宣称验证桌面鼠标坐标输入。Capture 通过 UE 的 Slate 截取实际 UI；普通 game_viewport 像素截图不包含 Slate HUD，不能用它判定 HUD 消失。

布局断言读取运行时 CachedGeometry 相对根布局的归一化中心：生命条 x≈0.5、y>0.92；入口 x>0.85、y<0.15。数字、进度与等级读取真实控件并与玩家属性比较。

脚本保存／恢复原地图、PIE 设置和后台限速偏好。报告与原始日志留在 Saved/HUDReference；交付摘要见 Docs/Validation。
