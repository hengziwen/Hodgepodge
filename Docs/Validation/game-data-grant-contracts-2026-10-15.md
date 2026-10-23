# GameData 与能力授予契约修复

日期：2026-10-15。范围：架构方案 R9、R5；开发起点为 `d028591`，起始工作区干净。UE 5.5.4，单一业务 Runtime 模块，不修改生产资产、Tag、RPC 或模块依赖，不自动提交。

## 实现与契约

- `UHodgeGameData::Get()` 恢复返回 `UHodgeAssetManager::Get().GetGameData()`；配置资产由 AssetManager 的既有持有链负责 GC，不再创建隐藏空实例。两个原有 UTF-16 文件保留编码。
- 正式 `/Game/Main/Data/DA_Dafult_GameData` 的伤害、治疗、动态 Tag GE 均已配置。自毁伤害使用 `GE_Damage_Basic_SetByCaller`，其 `HodgeDamageExecution` 原本拒绝普通自伤；新增只接受服务器、同一 ASC、明确 SelfDestruct 标记和有限正 SetByCaller 数值的分支。生命组件只允许当前权威 Avatar 自毁，普通自伤规则保留。
- AbilitySet 单包、整批和旧 void 签名共用唯一受检执行路径。整批先预检，Pending 资源完整提交后才发布句柄；同步回调换绑、销毁或丢失资源时失败并清理本批可撤销内容。撤销先取走账本，再执行回调，防止重复撤销。
- `OwnerInitialized` 允许注册且已初始化 Owner 的权威 ASC 在没有 Pawn 时授予玩家持久资源；`AvatarBound` 仍要求有效 Avatar。两者都固定本次 Owner/Avatar 身份。`bForInitialization` 原有的 OnSpawn 限制独立保留，不能将其等同于 Owner-only。
- PlayerState 保存基础来源句柄，整批成功后才发布 PawnData／AbilityReady；同配置重复调用幂等，失败不假报就绪、不自动重试，EndPlay 只撤销自己的来源。Hero 等待已提交的 PS PawnData，客户端晚到的 PawnData 复制会再次推动初始化。玩家基础 OnSpawn 能力仍在真实 Pawn 到达后由 ASC 激活。
- EnemyInit 与 Equipment 已使用受检入口及各自账本，继续沿用，自动获得统一的单包执行与回滚语义；不重写两者的协调流程。
- GameFeature 按 Context、Actor、EntryIndex 分别记录状态和授予时 ASC；直接能力、直接属性集、AbilitySet 三分支一起预检。重复事件不重授，重叠条目各自提交，失败不登记成功；撤销使用原 ASC，其他来源保留。直接能力正常停用沿用 Lyra 的结束后移除语义，失败或 Pawn 解绑强制清理。
- GameFeature 对尚未注册／绑定的 ASC 等待真实 Init State 或 PawnExtension 绑定事件；玩家接口暂时返回空 ASC 时不创建第二份 ASC。软引用不驻留或配置非法属于明确失败，需要修复加载／配置后重新激活，不进行无限重试。
- PawnExtension 新增本地 ASC 绑定／解绑扩展事件，让 GameFeature 不必等待依赖能力本身才能推进的较高 Init State；这不表示角色全部初始化完成，不新增复制协议。

Instant GE、OnSpawn 以及其他外部副作用不能保证回滚。失败结果保留不可逆标记；句柄清理不冒充数值和外部行为全部恢复。基础授予失败后，应修正配置并重新开始会话／创建来源；不重放可能已执行的副作用。

## Lyra 依据与适配差异

本机参考根目录：`E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame`。

- `System/LyraGameData.cpp::Get` 与 `System/LyraAssetManager.*::GetGameData`：直接复用配置资产来源，由项目现有 AssetManager 适配。
- `Player/LyraPlayerState.cpp::PostInitializeComponents/SetPawnData`、`AbilitySystem/LyraAbilitySet.cpp::GiveToAbilitySystem/TakeFromAbilitySystem`：保留 PlayerState Owner 和 Pawn 前基础授予；补充本项目所需的受检批次、来源账本及失败门控。
- `GameFeatures/GameFeatureAction_AddAbilities.cpp`：复用扩展监听、Bundle 引用和直接能力结束后移除策略；补足重叠条目、提交结果、同步撤销和原 ASC 归属。
- `Character/LyraPawnExtensionComponent.cpp::InitializeAbilitySystem/UninitializeAbilitySystem`：保留 Owner/Avatar 生命周期，在本项目增补 GameFeature 绑定事件。
- `Character/LyraHealthComponent.cpp::DamageSelfDestruct`：沿用自毁标签及 SetByCaller 入口；本项目正式 GE 使用额外团队／自伤过滤，需要最小专用 Execution 分支。Lyra 参考不替代本项目 GE 配置核查。

## 实际验证

- 修改前完整原生基线：132 项，68 Success、64 带警告、0 失败。
- 修改后完整 `Hodge.*`：143 项，78 Success、65 带警告、0 失败。新增 11 项覆盖配置资产身份与 GC、动态 GE 增删、自毁／旧 Avatar、防部分授予、不可逆失败、Pawn 前 OnSpawn、PlayerState 提交、GameFeature 重叠来源／缺资源／同步撤销／晚绑定／真实扩展生命周期。首轮测试夹具漏配对 Receiver，修正夹具后全量复跑通过。
- Editor 与 Game 常规 Development/Win64 构建均退出 0；编辑器已检查无脏包并正常关闭，构建后冷启动。首轮编译发现测试调用了 UE5.5 不存在的注销函数，已改用实际 `RemoveFeatureImplementer` 并重建通过。既有构建警告没有扩展为本次清理范围。
- 正式配置相关 11 个蓝图执行 `BlueprintEditorLibrary.compile_blueprint`，生成类有效，日志无 Blueprint 编译错误；未保存资产，结束时脏内容包和地图均为 0。包括 Hero、Experience、三类 GameData GE、基础原生能力蓝图和 HavocRules。
- `/Game/ThirdPerson/Maps/ThirdPersonMap` 单人 PIE、同进程双玩家 Listen PIE 均通过：初始授予、真实掉出世界自毁至零生命、旧 Pawn 销毁、服务器重生、同一 PlayerState ASC 重绑。前后 Spec 都为 21，双人服务器与拥有者客户端一致；HavocRules 仅服务器激活，重生后服务器规则仍有效。
- 正式 Experience/ActionSet 当前没有配置 AddAbilities。因此 GameFeature 修复的真实 Action 激活／撤销、三分支及绑定事件通过隔离原生世界测试验证；没有把正式 PIE 误记为真实 GameFeature 插件冷激活与 Bundle 验证。
- PIE 已结束，原测试偏好恢复，编辑器回到启动地图 `/Game/CodexText/L_MainMenu`。二进制资产和项目配置无 Git 改动。

原始日志与完整报告在 `Saved/GrantContracts/`；便于检索的摘要见 [结果 JSON](game-data-grant-contracts-2026-10-15-results.json)。

## 复跑命令

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/GrantContracts/Final' '-abslog=E:/Project/Git/Hodgepodge/Saved/GrantContracts/final.log'
```

实际构建输出：`editor-build-3.log`、`game-build-2.log`；原生报告：`Baseline/index.json`、`Final/index.json`。PIE 和资产检查通过已发现 schema 的本地 MCP 执行 `Saved/GrantContracts/validate_assets.py`、`start_pie.py`／`start_network.py`、`runtime_suite.py`、`stop_pie.py`、`final_editor_state.py`；启动脚本显式使用正式游戏地图，避免误测没有玩家 Pawn 的启动菜单地图。

## 未验证范围

未执行独立 GameFeature 插件冷加载／Bundle 取消、Dedicated Server、独立客户端进程、延迟／丢包、Cook／打包、性能剖析或所有技能逐项玩法回归。没有实现 R1/R2/R3/R4/R6/R7/R8，也没有把本次 132 项原生基线扩大为全部架构重构的行为／性能基线。生产资产无需用户补填新字段。
