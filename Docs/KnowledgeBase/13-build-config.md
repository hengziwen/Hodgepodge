# 本地环境、构建与配置

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 事实基线

UE 5.5.4 安装在 E:/UE/UE_5.5；项目 E:/Project/Git/Hodgepodge。Hodgepodge 为业务 Runtime，HodgeAbilityEditor 为 Editor/PostEngineInit；Game/Editor Target 使用 V5，暂无 Server/Client Target。旧 D:/Hodgepodge、D:/UE_5.5 是历史环境。

Build.cs 的 Public 依赖含 GAS、ModularGameplay、GameFeatures、CommonUI/CommonInput、UMG/Slate、动画/ControlRig 等；Private 含 EnhancedInput、PhysicsCore、Niagara、SignificanceManager、ApplicationCore。仅 Target.bBuildEditor 时引用 UnrealEd、AnimGraph、BlueprintGraph 与 AnimationWarpingRuntime/Editor；工具模块含 KismetCompiler。

Runtime 不无条件依赖 Editor。模块 IncludeOrderVersion=Unreal5_5，不代表 Target 默认顺序已升级，构建仍有兼容顺序提示；不自行改构建配置。

## 常规构建

保存并关闭编辑器后：

```powershell
$ueRoot = 'E:\UE\UE_5.5'
$projectFile = (Resolve-Path './Hodgepodge.uproject').Path
& "$ueRoot\Engine\Build\BatchFiles\Build.bat" HodgepodgeEditor Win64 Development "-Project=$projectFile" -WaitMutex -architecture=x64 -NoHotReloadFromIDE
& "$ueRoot\Engine\Build\BatchFiles\Build.bat" Hodgepodge Win64 Development "-Project=$projectFile" -WaitMutex -architecture=x64
```

Runtime 改动验证两目标。最近武器插槽任务两目标退出 0；当时用命令级 -NoUBA -NoUBALocal -MaxParallelActions=1，未修改配置。文档更新无需重新编译。Live Coding 和 Target is up to date 不替代玩法、蓝图或打包验证。

## 配置

游戏地图 ThirdPersonMap，编辑器启动 MainMenu；GameMode 的旧名通过 CoreRedirects，GameInstance/LocalPlayer/AssetManager/Globals/Policy 以 ini 为准。默认 Experience PawnData 在 Main/Data/PawnData；AssetManager 回退仍在 Main/Data。

PrimaryAsset 扫描 7 种类型；未另登记 Timeline/Combo，当前走硬引用链。正式攻击和武器配置仍在 CodexText，Cook 时需包含其引用依赖。

## 插件与自动化

uproject 显式启用 GameplayAbilities、GameFeatures、AnimationLocomotionLibrary、AnimationWarping、CommonUI、ModelingToolsEditorMode、McpAutomationBridge，UNTLink 禁用。ALS/RiderLink/UnrealMCP 本地描述和默认启用单独查 [插件索引](Reference/plugins.md)，不从主模块 include 推断插件加载。

MCP 为编辑器工具，原生入口 127.0.0.1:3016/mcp，认证信息从 Saved/MCP 读取；不把令牌写入文档。旧桥存在不代表默认连接可用，编辑器关闭时 HTTP 服务不可访问；工具调用不替代常规 C++ 构建。

CommonUI 的完整 GameViewport/前端依赖尚未接通；CueNotifyPaths 未配置。保留既有重定向与拼写，修改前核对资产。

入口：[AGENTS.md](../../AGENTS.md)、[流程](../AI_DEVELOPMENT.md)、[配置索引](Reference/config.md)。
