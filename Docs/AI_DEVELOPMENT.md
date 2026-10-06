# AI 开发与验证流程

> 2026-10-06 状态同步：当前基线见知识库：业务 Runtime + Editor 工具，正式 Hero 可移动，默认组件由 Experience 注入；初次环境与工作区记录保持历史日期。文档修改只验证文档/差异，既有构建和游戏测试不自动算本次复跑。 当前项目事实见 [本轮更新](KnowledgeBase/26-update-2026-10-06.md)。

## 初次核实基线（2026-09-09，历史记录）

本次读取了 README、Lyra 设计理念及架构方案相关章节、项目描述、全部项目 Target/Build.cs、Source 目录及关键角色/数据/生成流程、Config、ALS/RiderLink 插件描述和 ALS 构建规则。原项目未发现 AGENTS.md 或 .mdc 开发规则。没有修改业务代码、资产、插件或现有配置；没有执行本次编译或编辑器运行验证。

- `Hodgepodge.uproject`：EngineAssociation 为 `5.5`；本机 `D:/UE_5.5/Engine/Build/Build.version` 为 **5.5.4，CL 40574608**。已有编辑器日志也记录此版本，但日志不能证明当前工作区可编译。
- 单一业务模块 `Hodgepodge`：Runtime / Default。目录按 AbilitySystem、Character、Component、Core、Data、GameFeatures、Input、Camera、Animation、Actor、Interface 组织；这些是目录，不是独立 UBT 模块。
- `Hodgepodge`（Game）和 `HodgepodgeEditor`（Editor）两个目标，均为 BuildSettingsVersion.V5。没有 Server/Client 专用 Target；Dedicated Server 是规划，不能声称已支持独立服务器构建。
- 主模块使用显式/共享 PCH，IncludeOrderVersion.Unreal5_5、内联生成代码警告和 SetupIrisSupport(Target)。调用 Iris 构建辅助不等于运行时已启用 Iris。
- Public 依赖：Core、CoreUObject、Engine、InputCore、GameplayAbilities、GameplayTags、GameplayTasks、ModularGameplay、GameFeatures、AIModule、EngineSettings、NetCore、AnimGraphRuntime、RigVM、ControlRig。
- Private 依赖：EnhancedInput、PhysicsCore、Niagara、SignificanceManager。不能仅凭 `.uproject` 插件列表判断全部模块依赖。
- `.uproject` 显式启用 ModelingToolsEditorMode（仅 Editor）、GameplayAbilities、GameFeatures、AnimationLocomotionLibrary、AnimationWarping；**UNTLink 当前禁用**，与 README 旧描述不同。
- ALS 插件位于 `Plugins/ALS-Refactored-4.15/ALS-Refactored-4.15`，版本 4.15、声明 EngineVersion 5.5.0、EnabledByDefault=true。包括 ALS/ALSCamera/ALSExtras（Runtime）及 ALSEditor（UncookedOnly）。依赖 ACLPlugin、AnimationModifierLibrary、ControlRig、EngineCameras、EnhancedInput、GameplayTagsEditor、Metasound、Niagara、PropertyAccessNode。已有日志确实记录挂载 ALS；主模块不再直接依赖 ALS 并不表示插件已禁用。
- RiderLink 2025.2.2.1 默认启用，8 个 EditorNoCommandlet 模块：RD、RiderLink、RiderLogging、RiderBlueprint、RiderGameControl、RiderShaderInfo、RiderLC、RiderDebuggerSupport（Win64）。
- `.vsconfig` 声明 VS C++/游戏开发工作负载、MSVC 14.38、Windows SDK 22621 等；这是所需组件清单，尚未核实本机实际安装工具链。`Intermediate/ProjectFiles/UECommon.props` 指向本机 Build.bat，IDE 最终走 UBT，不直接用普通 C++ 编译器构建项目。
- 初始工作区已有 `.uproject` 和两份动画资产修改：`Content/Main/Character/Hero/Anim/ABP_Pover_Base.uasset`、`Content/Main/Character/Hero/Anim/Layer/ABP_ItemAnimLayers_Pover_Base.uasset`。后续任务重新检查状态，不把此清单当作永久状态。

## C++ 实现文件组织

同一个项目自有类的非内联成员函数统一放在一个主 `.cpp`，不再按功能拆出同类的多个实现文件。例如连段实现放回 `HodgeCombatComponentBase.cpp`，武器表现放回 `HodgeWeaponInstance.cpp`，蒙太奇实现放回 `HodgeAbilitySystemComponent.cpp`，编辑器资产校验在 `HodgePawnData.cpp` 内用 `WITH_EDITOR` 保护。

禁止通过 `#include "Other.cpp"`、`.inl` 或其他片段文件把该类的实现再次拆散。文件较长时使用清晰的函数分组；确有独立职责时再设计独立类型，不以整理文件为由引入额外模块。

独立类自己的 `.h/.cpp`、测试和 UE 自动生成代码可以独立存在；例如 `HodgeWidgetFactory_Class` 与 `HodgeWidgetFactory` 是两个类，不因名称相近而合并。第三方插件及生成目录不做此类整理。

本约定适用于后续新建和修改的项目自有 C++ 源码。修改前检查成员定义分布，而不只检查文件名后缀。

### 2026-10-05 合并与验证记录

- 已将 `_Combo.cpp`、`_Presentation.cpp`、`_Montage.cpp`、`_Validation.cpp` 四个同类实现片段合入上述对应主文件，并删除片段文件。保持原函数实现、反射入口和文件编码；PawnData 编辑器校验继续受 `WITH_EDITOR` 保护。
- 扫描项目 `Source` 下的成员函数定义，合并后未发现同类定义分布于多个 `.cpp`；未整理第三方插件或生成代码。文档内相关源码链接同步更新，历史快照路径及 hash 保留原记录。
- 修改前源码与规则备份：`E:/Project/Backups/Hodgepodge/CppImplementationMerge/20261005-225937`。本次未改动二进制资产，没有提交或回退已有工作。
- 以下 Editor、Game 常规构建均退出 `0`。`-NoUBA`、`-NoUBALocal` 和单并行参数仅用于本次命令，不修改项目构建设置。既有编译警告保留。
- 连段组 5 项、武器表现组 2 项自动化测试均为 `Success`，命令退出 `0`；`Hodge.Combo.DefinitionGrant` 有一条未配置 `GameplayCueNotifyPaths` 的现有警告。日志、报告和扫描结果位于 `Saved/CppImplementationMerge`。
- 本次未执行蓝图编译、PIE、联机、Cook 或打包验证；原生测试与常规构建不代表这些验证通过。

本机实际执行命令（PowerShell）：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Combo' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/CppImplementationMerge/ComboAutomation' '-abslog=E:/Project/Git/Hodgepodge/Saved/CppImplementationMerge/ComboAutomation.log'
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.WeaponPresentation' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/CppImplementationMerge/WeaponAutomation' '-abslog=E:/Project/Git/Hodgepodge/Saved/CppImplementationMerge/WeaponAutomation.log'
```

## 编译验证

1. 阅读 `git status --short`，检查目标改动和依赖。确定本机引擎路径；换机器不能照抄 D 盘路径。检查 `.vsconfig` 对应工具链是否安装。
2. 外部常规构建前保存编辑器工作并关闭本项目编辑器，避免 Live Coding/Hot Reload 和 DLL 占用。不由 AI 强制结束含未保存工作的编辑器。
3. 在 PowerShell 执行以下 Editor 构建。它是正常 UBT 构建，按需运行 UHT、编译和链接；不是清缓存后的全量重建。

```powershell
$ueRoot = 'E:\UE\UE_5.5'
$projectFile = (Resolve-Path './Hodgepodge.uproject').Path
& "$ueRoot\Engine\Build\BatchFiles\Build.bat" HodgepodgeEditor Win64 Development "-Project=$projectFile" -WaitMutex -architecture=x64
if ($LASTEXITCODE -ne 0) { throw "Editor build failed: $LASTEXITCODE" }
```

4. Runtime 代码、模块依赖、插件或平台条件变更，再验证非编辑器目标，避免 Editor 环境掩盖依赖问题：

```powershell
& "$ueRoot\Engine\Build\BatchFiles\Build.bat" Hodgepodge Win64 Development "-Project=$projectFile" -WaitMutex -architecture=x64
if ($LASTEXITCODE -ne 0) { throw "Game build failed: $LASTEXITCODE" }
```

5. 成功需记录目标、平台、配置、退出码和日志结果。失败时保留首个有效错误及上下文，区分环境、UHT、编译、链接、插件依赖；不以禁用不相关插件、删除用户缓存或改业务逻辑来掩盖环境错误。UBT 日志位置以命令输出为准，编辑器日志在 `Saved/Logs`。
6. 新增/删除 C++ 文件、Target、模块或插件后按需刷新 IDE 项目：通过 `.uproject` 的 Generate Visual Studio project files 或 IDE 的 UE 项目生成入口。本机未发现 `Engine/Build/BatchFiles/GenerateProjectFiles.bat`，不要使用臆造脚本路径。现有 `.sln` 和 `Intermediate` 文件是生成物，不手工修改。
7. VS 对应选择 Development Editor / Win64 构建 Hodgepodge 项目；Rider 选择 HodgepodgeEditor / Development / Win64。以上 Game 构建不是 Cook/打包，发布验证另行针对目标平台安排。
8. 2026-09-09 基线检查时未发现常规 Automation Test/Spec 注册；后续已新增测试，以当前 Source 和实际测试报告为准。按实际变更运行有价值的测试，不为文档修改运行 UE 全量编译。

## 需要用户在 UE 编辑器完成的操作

本次无需修改任何编辑器设置。后续验证按变更选择下列步骤，AI 交付时应缩小到受影响资产。

1. 常规编译前保存当前地图和正在编辑的资产，再关闭编辑器；编译成功后用 UE 5.5.4 打开 `Hodgepodge.uproject`。如果提示切换引擎或缺插件，记录具体提示，先核实环境。
2. 打开 `/Game/ThirdPerson/Maps/ThirdPersonMap`（当前默认地图）。检查 World Settings 的 GameMode Override 和实际生成的 Pawn/PlayerState/GameState，确认没有地图覆盖导致使用其他类。配置中的旧 GameMode/GameInstance 类名依赖 CoreRedirects，不随手删除重定向。
3. 涉及角色/动画反射接口时，打开受影响角色蓝图及动画蓝图执行 Compile，查看错误、失效引脚、父类、Anim Instance 和动画层引用；确认无误后保存。当前两份已修改动画资产尤其要保留用户未提交工作，不批量重存无关资产。
4. 涉及 Experience/PawnData 时，检查 `/Game/Main/Experiences/Exp_HodgeDefaultExperience` 和 `/Game/Main/Data/PawnData/DA_Dafult_PawnData`，确认 Experience 的 DefaultPawnData 与 PawnData.PawnClass 指向预期资产。GameData 配置指向 `/Game/Main/Data/DA_Dafult_GameData`。磁盘存在资产不等于内部属性已正确赋值。
5. 当前 PawnData 的输入/相机/AbilitySets/ComboDefinition/DefaultWeaponDefinition 均已启用；GameMode 已注入 PawnData，Hero/PawnExtension 接入 ASC，Experience 为 Pawn 注入 Combat/Equipment。不要重复创建已有组件，重点核对当前资产引用与生命周期。
6. 单人 PIE：查看 Output Log 的 Experience 选择及加载、Pawn 生成、输入响应和相关功能。记录预期与实际；当前正式 Hero 已有移动/攻击验证，遇到失败先追踪实际地图、输入配置与组件状态，也不能仅因 PIE 启动就判定玩法通过。
7. 网络/GAS/复制改动：在 PIE 使用 2 个玩家、Play As Listen Server，分别核对服务器和客户端的 Pawn、ASC Owner/Avatar、输入、能力及重生；需要时补 Play As Client/独立进程验证。PIE 服务端测试不等于构建了 Dedicated Server 可执行文件。
8. 反馈具体地图、资产、复现步骤、网络模式及 Output Log 错误。AI 汇报必须分别标记 C++ 构建、蓝图编译、PIE、联机和打包的“通过/失败/未执行”。

## 文档维护边界

README 的历史编译结论不可用作新修改的验证证据；架构方案含早期 ALS 路线与示例 AOW 类名，新增实现沿用实际 Hodge 类型。修改插件启用状态需同时检查资产引用与运行加载，不能直接从 Build.cs 删除依赖来证明完成迁移。

`.gitignore` 虽包含 Hodgepodge.uproject，但该文件当前已被 Git 跟踪，仍会显示修改。本次读取资产 diff 时 Git LFS 因 `.git/lfs/tmp` 写入权限受限而失败，未取得二进制内容差异；这不影响项目描述和源码的只读检查，也不能据此宣称两份资产内容已验证。
