# 本地环境、构建与配置

> 最近源码核对：2026-09-13。源码接入状态与运行验收分开记录。
[返回首页](README.md)

## 构建基线

uproject 关联 UE 5.5，Runtime 模块为 Hodgepodge，已有 Game 和 Editor Target。Build.cs 启用显式/共享 PCH、UE 5.5 include 顺序、内联生成代码警告及 SetupIrisSupport。

Public 依赖包含 Core、CoreUObject、Engine、InputCore、GameplayAbilities、GameplayTags、GameplayTasks、ModularGameplay、GameFeatures、AIModule、EngineSettings、NetCore、AnimGraphRuntime、RigVM、ControlRig。Private 依赖包含 EnhancedInput、PhysicsCore、Niagara、SignificanceManager。

未启用的 Slate UI 和 OnlineSubsystem 注释不能作为依赖已经加入的证据。ALS 不在当前 Build.cs 依赖中。

## 本地构建命令模板

以下是待用户在具备引擎环境时执行的模板，本轮未运行。开发流程记录引擎为 D:\UE_5.5、版本 5.5.4；它是既有核实记录，本轮没有重新探测工具链。项目路径按当前仓库解析，不能照搬文档中的旧 D:\Hodgepodge 路径。

```powershell
$ueRoot = 'D:\UE_5.5'
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

uproject 启用 GameplayAbilities、GameFeatures、AnimationLocomotionLibrary、AnimationWarping、ModelingToolsEditorMode（Editor）；UNTLink 当前显式禁用。ALS.uplugin 默认启用，主模块不引用 ALS 不等于插件停用。新增 UnrealMCP 1.0 为默认启用的 Editor 模块，依赖 EditorScriptingUtilities、Niagara、PythonScriptPlugin 和 Metasound；本轮未验证连接或加载。

模块依赖和 uproject 插件声明是不同层级。出现插件依赖警告时对照引擎插件所属模块修正，不能仅删除 Build.cs 依赖来消除警告。

## 重定向风险

CoreRedirects 可让旧资产类名迁移到新类，但 NewName 必须真实存在。当前有旧 GameplayAbilityBase 重定向到 HodgeGameplayAbilityBase 的历史条目，而代码已迁移到 HodgeGameplayAbility，应结合资产加载日志核对。HeroComponent 重定向目标当前已是有效类，仍需蓝图加载验证。

不要批量删除所有重定向；旧资产可能仍依赖它们。先查加载错误，再在编辑器打开并保存迁移资产，确认后做有依据的清理。

## 构建结果记录

每次记录 Git HEAD、未提交修改、引擎版本、目标、配置、命令、退出码和首个编译错误。README 的历史编译通过仅适用于其记录基线，不代表当前新增类与工作区修改已重新编译。

完整配置索引见 [Reference/config.md](Reference/config.md)。

## 开发约定入口

遵循 [AGENTS.md](../../AGENTS.md) 与 [AI_DEVELOPMENT.md](../AI_DEVELOPMENT.md)。文档任务只检查文档和差异；C++ 变更执行 Editor 构建，Runtime/依赖变更再执行 Game 构建。常规构建前保存并关闭编辑器，禁止用 Live Coding 成功代替常规构建；本轮没有执行这些构建。
