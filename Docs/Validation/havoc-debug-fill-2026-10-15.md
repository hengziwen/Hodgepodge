# 暗流回满调试命令验证

日期：2026-10-15；UE 5.5.4；未自动提交Git。

## 使用

运行中的当前玩家游戏窗口按`~`打开控制台，输入`HodgeFillDarkFlow`回车。共鸣回路暗流回满到实际容量，首版100；HUD同步。两客户端分别在各自窗口执行，各自只补自己的资源。

不自动释放灭音，不补R能量，不清除或重置E／R冷却。暗涌中使用时只补燃料，状态GE不重施、20%加成不叠加，随后继续正常消耗。死亡或暗流容量尚未初始化时拒绝。Shipping／Test用编译宏关闭命令实际效果，未构建这两种配置。

## 实现与参考

在AHodgePlayerController主头文件新增Exec，在主cpp实现；沿用HodgeFillResonance的服务器转发和精确ServerCheat白名单，不执行任意远端文本。服务器从PlayerState ASC读取DarkFlow／MaxDarkFlow，通过UHodgeDarkFlowEffect及SetByCaller.DarkFlowDelta应用实际差值，不直接设置HUD或修改Buff生命周期。

Lyra本机参考：Source/LyraGame/Player/LyraCheatManager.cpp中的God使用ServerCheat转发，ApplySetByCallerHeal在权威ASC上应用GE。本项目已有对应Controller入口，直接最小适配，没有新增通用CheatManager。

Editor QueueUIAction增加DarkFlowFill，仅在后续游戏Tick执行真实ConsoleCommand；测试没有用直接改属性冒充客户端RPC通过。

## 结果

- HodgepodgeEditor Win64 Development常规构建通过，Saved/HavocDebug/editor-build.log；反射修改前确认无脏资产并关闭编辑器，构建后冷启动。
- Hodgepodge Win64 Development常规构建通过，退出0，Saved/HavocDebug/game-build.log；使用同一UE5.5.4与既有构建设置。
- 原生40/40通过，25项无警告，15项带既有资产警告，0失败／未运行；包含新Hodge.Havoc.DebugFill及共鸣解放原命令回归。新增用例覆盖容量未初始化、25到100、重复、动态150容量、死亡、非白名单文本、大小写及不自动入态。
- 单人、Listen远端客户端、PIE Dedicated各11项通过，共33项断言。覆盖权威与拥有者回满、HUD满条、不自动入态、不补R、其他玩家保持0、E冷却保持、暗涌中补能、不重复施加Buff及继续消费。
- Python语法与目标git diff --check通过。未生成源码行覆盖率；PIE Dedicated不等同于独立专服构建。

完整用例与结果见[havoc-debug-fill-2026-10-15-results.json](havoc-debug-fill-2026-10-15-results.json)。历史共鸣回路报告保留原验收时的数据，本报告只补充本次调试入口。

## 复现

常规构建使用Build.bat HodgepodgeEditor／Hodgepodge Win64 Development，参数`-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1`，Editor额外`-NoHotReloadFromIDE`。

原生运行UnrealEditor-Cmd.exe，参数`-unattended -NullRHI -nosplash`，ExecCmds为`Automation RunTests Hodge.Havoc+Hodge.Resonance+Hodge.Combo+Hodge.Combat+Hodge.ActionRequests+Hodge.UI`，TestExit为`Automation Test Queue Empty`。报告路径Saved/HavocDebug/Automation，日志Saved/HavocDebug/automation.log；检查JSON，不只看进程退出码。

```powershell
$env:PYTHONIOENCODING='utf-8'
$env:HODGE_HAVOC_SUITE='debug'
E:/Python/python.exe Tools/Havoc/run_batch.py single listen dedicated
```

保留用户原有未提交修改；没有修改能力、美术或HUD资产。测试后的地图与性能恢复记录在Saved/HavocDebug/editor-restored.json。
