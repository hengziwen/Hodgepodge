# 本地环境、构建与配置

> 最近源码核对：2026-09-28（HEAD `6eec094`）。源码接入状态与运行验收分开记录。
[返回首页](README.md)

## 构建基线

uproject 关联 UE 5.5。**现在有两个模块**：`Hodgepodge`（Runtime，唯一进游戏构建的模块）与 **`HodgeAbilityEditor`**（2026-09-29 新增，`Type: Editor`，`LoadingPhase: PostEngineInit`，已在 `HodgepodgeEditor.Target.cs` 的 `ExtraModuleNames` 登记，**不进 Runtime 构建**）。已有 Game 和 Editor Target。Build.cs 启用显式/共享 PCH、UE 5.5 include 顺序、内联生成代码警告及 SetupIrisSupport。

`HodgeAbilityEditor.Build.cs` 依赖：`Hodgepodge`、`UnrealEd`、`AssetTools`、`PropertyEditor`、`Slate`、`SlateCore`、`EditorStyle`、`AdvancedPreviewScene`、`AnimationEditor`、`AnimGraph`、`BlueprintGraph`、`GraphEditor`、`UnrealEd` 类工具等（编辑器模块，运行时不存在）。

Public 依赖包含 Core、CoreUObject、Engine、InputCore、GameplayAbilities、GameplayTags、GameplayTasks、ModularGameplay、GameFeatures、AIModule、EngineSettings、NetCore、AnimGraphRuntime、RigVM、ControlRig，以及 **UMG、Slate、SlateCore**（最初只为 CodexText 的 UUserWidget，2026-09-28 起成为 UI 迁移的正式依赖）、**`CommonUI`、`CommonInput`**（本轮新增，Lyra UI 迁移用）。Private 依赖包含 EnhancedInput、PhysicsCore、Niagara、SignificanceManager、**`ApplicationCore`**（本轮新增）。编辑器专用块（`Target.bBuildEditor`）包含 UnrealEd、AnimGraph、BlueprintGraph（供 CodexText 的 Authoring 库生成动画图）以及 **`AnimationWarpingRuntime`、`AnimationWarpingEditor`**（CodexText 实验用 UE 5.5 的 Stride Warping / Foot Placement）。**编辑器块内的依赖不进 Runtime 构建**。

⚠️ 未启用：`GameplayMessageRuntime`（源码也无引用）。CommonGame / GameSettings / CommonUser 未引入 —— 这是 `UI/` 中 24 个文件只能注释保留的根因。

未启用的 Slate UI 和 OnlineSubsystem 注释不能作为依赖已经加入的证据。ALS 不在当前 Build.cs 依赖中。

## 本地构建命令模板

**本轮已实际运行**（2026-09-29），引擎在本机 `E:\UE\UE_5.5`（由注册表 `HKLM:\SOFTWARE\EpicGames\Unreal Engine\5.5` 的 `InstalledDirectory` 核实，版本 5.5.4）。文档里残留的 `D:\UE_5.5` / `D:\Hodgepodge` 是旧路径，**不要照抄**。

```powershell
$ueRoot = 'E:\UE\UE_5.5'
$projectFile = (Resolve-Path './Hodgepodge.uproject').Path
& "$ueRoot\Engine\Build\BatchFiles\Build.bat" HodgepodgeEditor Win64 Development "-project=$projectFile" -WaitMutex
& "$ueRoot\Engine\Binaries\Win64\UnrealEditor.exe" $projectFile
```

反射类、UPROPERTY、UFUNCTION 或继承变化后，需要考虑关闭编辑器完整构建再重开，避免只凭 Live Coding 旧状态判断资产与反射成功。

## 配置检查重点

DefaultEngine.ini：默认地图、GameInstance/GameMode、AssetManagerClassName、LocalPlayerClassName、GameFeaturePolicy 和 CoreRedirects。当前 GameMode 仍使用旧名字；GameInstance 已直接选择 HodgeGameInstanceBase。

LocalPlayerClassName 已选择 HodgeLocalPlayerBase；DefaultGame.ini 已选择 HodgeGameFeaturePolicy、HodgeAbilitySystemGlobals、HodgeGameplayCueManager，并设置激活失败 Tag、bUseDebugTargetFromHud=True、PredictTargetGameplayEffects=False。运行时实例仍待验证。

DefaultGame.ini：GameData/PawnData 路径、PrimaryAsset 扫描、CookRule。资源更名必须同时处理资产引用和配置，不要仅搜索 C++ 字符串。

DefaultInput.ini：DefaultPlayerInputClass 为 EnhancedPlayerInput；DefaultInputComponentClass 已是 HodgeInputComponent。旧 AxisMappings 存在不意味着新角色调用 BindAxis；实际输入链取决于有效绑定。

## 插件

uproject 启用 GameplayAbilities、GameFeatures、AnimationLocomotionLibrary、AnimationWarping、**CommonUI（2026-09-28 新增，`Enabled=true`，随 Runtime 构建）**、ModelingToolsEditorMode（Editor）、UnrealMCP（Editor）和 McpAutomationBridge（Editor，`TargetAllowList: ["Editor"]`）；UNTLink 当前显式禁用。两个 MCP 插件均为 Editor-only 模块（McpAutomationBridge 含 McpAutomationBridge / McpAutomationBridgeFab 两个 Editor 模块），不进 Runtime 构建。**连接与工具调用已于 2026-09-19 实测通过**（UnrealMCP 55557；McpAutomationBridge 原生 MCP `POST /mcp` 3016，需 `X-MCP-Capability-Token`），实操坑见 [排障手册的 MCP 一节](14-troubleshooting.md)。注意桥**不能编译 C++**，也不注册新增 `UCLASS`。

`Config/DefaultGame.ini` 另有一节 `[/Script/McpAutomationBridge.McpAutomationBridgeSettings]`：`bEnableNativeMCP` / `NativeMCPPort=3016` / `ListenPorts=8116` / `bRequireCapabilityToken=True`。

CommonUI 启用后，ini 里**还没有** CommonUI 的按键映射（如 `UI.Action.Escape`），也未配置 `GameViewportClientClassName` —— 需要前端 UI 时补上。

模块依赖和 uproject 插件声明是不同层级。出现插件依赖警告时对照引擎插件所属模块修正，不能仅删除 Build.cs 依赖来消除警告。

## 重定向风险

CoreRedirects 可让旧资产类名迁移到新类，但 NewName 必须真实存在。当前有旧 GameplayAbilityBase 重定向到 HodgeGameplayAbilityBase 的历史条目，而代码已迁移到 HodgeGameplayAbility，应结合资产加载日志核对。HeroComponent 重定向目标当前已是有效类，仍需蓝图加载验证。

2026-09-28 新增一批 **UI 类重定向**（`LyraHUD → HodgeHUD`、`LyraUIManagerSubsystem → HodgeUIManagerSubsystem` 等），配合 `UI/` 迁移。注意 `LyraActivatableWidget` 等 Lyra 类名必须对应到真实存在的 Hodge 类型，否则加载时会报"找不到类"。

不要批量删除所有重定向；旧资产可能仍依赖它们。先查加载错误，再在编辑器打开并保存迁移资产，确认后做有依据的清理。

## 构建结果记录

每次记录 Git HEAD、未提交修改、引擎版本、目标、配置、命令、退出码和首个编译错误。README 的历史编译通过仅适用于其记录基线，不代表当前新增类与工作区修改已重新编译。

**2026-09-28 记录**：HEAD `6eec094`，工作区干净；引擎 `E:\UE\UE_5.5`；目标 `HodgepodgeEditor Win64 Development`；结果 UBT 返回 **`Target is up to date`**（0.67s，未触发重编，说明源码与产物一致）。本轮**未**执行 PIE / 蓝图 Compile / 打包。UI 的 81 个新文件与战斗相关类均已在此前的构建中编入（产物最新）。

**2026-09-29 记录**：HEAD `abb9224`，工作区有 2 个 `CodexText` 资产改动（不影响构建）；同一目标与配置；结果 UBT 返回 **`Target is up to date`**（0.75s）。说明连击系统（`AbilityDefinition` / `ComboDefinition` / `ComboComponent` / `Ability_Definition`）、独立编辑器模块 `HodgeAbilityEditor`、装备四件套、`TimelineEvaluator` 与新测试文件都已编入（产物最新）。本轮**未**执行 PIE / 蓝图 Compile / 打包。

> 注意：`Target is up to date` 只说明**源码与现有产物一致**；它由 `git status` 决定工作集，因此看到这句话时应确认自己确实在预期提交上（本轮为 `abb9224`）。若改动了 `.Build.cs` / 新增模块却仍报 up to date，需检查是否真的保存了文件。

> 引擎安装路径：`E:\UE\UE_5.5`（由注册表 `HKLM:\SOFTWARE\EpicGames\Unreal Engine\5.5` 的 `InstalledDirectory` 核实）。文档里残留的 `D:\UE_5.5` / `D:\Hodgepodge` 是旧路径，不要照抄。

完整配置索引见 [Reference/config.md](Reference/config.md)。

## 开发约定入口

遵循 [AGENTS.md](../../AGENTS.md) 与 [AI_DEVELOPMENT.md](../AI_DEVELOPMENT.md)。文档任务只检查文档和差异；C++ 变更执行 Editor 构建，Runtime/依赖变更再执行 Game 构建。常规构建前保存并关闭编辑器，禁止用 Live Coding 成功代替常规构建；本轮没有执行这些构建。
