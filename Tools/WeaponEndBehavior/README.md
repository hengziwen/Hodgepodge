# 武器手持通知结束方式验证

运行时代码和接口见 [武器表现设计](../../Docs/Design/weapon-presentation.md)。Hodge 武器手持区间的 EndBehavior 默认 ReturnToBack；FadeAtHand 在手部直接开始武器 Profile 的 FadeSeconds 淡出。

run_e2e.py 通过真实攻击与 Dash 输入验证旧路径、直接淡出、再次攻击和取消；启动时沿用 Tools/WingAttack/start_test.py 的地图隔离与异步初始化，WING_TEST_SCRIPT 指向本文件，WING_WORK 指向 Saved/WeaponEndBehavior，WING_MODE 为 single/listen/dedicated。

本测试会临时修改已加载通知的配置，完成后恢复原值，但 UE 会把相关 Montage 标成 dirty。每次停止 PIE 后，必须验证所有通知恢复原值，再使用 EditorLoadingAndSavingUtils.reload_packages 从原磁盘包重新加载；不要保存测试的直接模式，不要通过版本控制回退资产。测试开始前必须保证这些包没有用户未保存编辑。完整脚本、初始编辑器设置与输出位于 Saved/WeaponEndBehavior。

原生测试使用 Automation RunTests Hodge.WeaponPresentation，共包括既有两项回归及新增 NotifyEndBehavior；真实多端逐帧数据保存各端 Phase、EndBehavior、可见度、请求数和修订号。构建需常规 Editor/Game 目标，反射变化不能只用 Live Coding。
