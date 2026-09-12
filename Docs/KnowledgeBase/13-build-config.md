# 本地环境、构建与配置

[返回首页](README.md)

## 构建基线

uproject 关联 UE 5.5，Runtime 模块为 Hodgepodge，已有 Game 和 Editor Target。Build.cs 启用显式/共享 PCH、UE 5.5 include 顺序、内联生成代码警告及 SetupIrisSupport。

Public 依赖包含 Core、CoreUObject、Engine、InputCore、GameplayAbilities、GameplayTags、GameplayTasks、ModularGameplay、GameFeatures、AIModule、EngineSettings、NetCore、AnimGraphRuntime、RigVM、ControlRig。Private 依赖包含 EnhancedInput、PhysicsCore、Niagara、SignificanceManager。

未启用的 Slate UI 和 OnlineSubsystem 注释不能作为依赖已经加入的证据。ALS 不在当前 Build.cs 依赖中。

## 本地构建命令模板

以下是待用户在具备引擎环境时执行的模板，本轮未运行。E:\UE\UE_5.5 来自原 README 示例，需先确认本机真实引擎目录。

```powershell
$ueRoot = 'E:\UE\UE_5.5'
$projectFile = (Resolve-Path './Hodgepodge.uproject').Path
& "$ueRoot\Engine\Build\BatchFiles\Build.bat" HodgepodgeEditor Win64 Development "-project=$projectFile" -WaitMutex
& "$ueRoot\Engine\Binaries\Win64\UnrealEditor.exe" $projectFile
```

反射类、UPROPERTY、UFUNCTION 或继承变化后，需要考虑关闭编辑器完整构建再重开，避免只凭 Live Coding 旧状态判断资产与反射成功。

## 配置检查重点

DefaultEngine.ini：默认地图、GameInstance/GameMode、AssetManagerClassName、LocalPlayerClassName、GameFeaturePolicy 和 CoreRedirects。当前 GameMode/GameInstance 使用旧名字，依赖重定向解析。

本仓库 Config 未找到 LocalPlayerClassName 和项目 GameFeature 策略类的显式配置，不能认定 HodgeLocalPlayerBase 与 HodgeGameFeaturePolicy 已被引擎选用。Hero 草稿对 HodgeLocalPlayerBase 有类型假设，启用时需补查实际 LocalPlayer 类型；Policy 也需明确引擎选择入口。

DefaultGame.ini：GameData/PawnData 路径、PrimaryAsset 扫描、CookRule。资源更名必须同时处理资产引用和配置，不要仅搜索 C++ 字符串。

DefaultInput.ini：DefaultPlayerInputClass 为 EnhancedPlayerInput；DefaultInputComponentClass 仍是 EnhancedInputComponent。旧 AxisMappings 存在不意味着新角色调用 BindAxis；实际输入链取决于有效绑定。

## 插件

uproject 启用 GameplayAbilities、GameFeatures、AnimationLocomotionLibrary、AnimationWarping、ModelingToolsEditorMode（Editor）及 UNTLink。项目内未确认 UNTLink 实现，可能依赖引擎或本机插件安装；换机器构建应检查可用性。

模块依赖和 uproject 插件声明是不同层级。出现插件依赖警告时对照引擎插件所属模块修正，不能仅删除 Build.cs 依赖来消除警告。

## 重定向风险

CoreRedirects 可让旧资产类名迁移到新类，但 NewName 必须真实存在。当前有旧 GameplayAbilityBase 重定向到 HodgeGameplayAbilityBase 的历史条目，而代码已迁移到 HodgeGameplayAbility，应结合资产加载日志核对。HeroComponent 新重定向的目标类目前仍注释停用，也需要在启用后验证。

不要批量删除所有重定向；旧资产可能仍依赖它们。先查加载错误，再在编辑器打开并保存迁移资产，确认后做有依据的清理。

## 构建结果记录

每次记录 Git HEAD、未提交修改、引擎版本、目标、配置、命令、退出码和首个编译错误。README 的历史编译通过仅适用于其记录基线，不代表当前新增字段和草稿状态已重新编译。

完整配置索引见 [Reference/config.md](Reference/config.md)。
