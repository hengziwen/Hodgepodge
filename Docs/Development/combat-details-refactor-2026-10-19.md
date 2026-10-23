# 战斗资产 Details 重组实施记录

日期：2026-10-19。引擎保持 UE 5.5.4。依据：[战斗制作执行文档第 11 节](../Guides/combat-production-workflow.md#migration)。

**状态：Details 重组、检查及无损迁移代码已完成；常规构建、编辑器自动化、原生受击与单目标 PIE 回归已通过。** 多端、物理键鼠、完整面板视觉验收与打包没有执行，具体范围见末节。

## 本次确认的范围

用户选择定制现有资产的 Details 面板，复用 `HodgeAbilityEditor`，整理常规、高级与兼容配置，并接入检查和迁移。工作面为编辑器组织及现有配置转换，不改 GAS、ASC 所有权、受击裁决、CharacterMovement、输入或伤害规则，不新增 Runtime 模块。

用户另明确了两项迁移决策：

- 只进行无损的原地配置转换；方向歧义、不同起身或根运动等不能等价表示时停止，不生成、裁剪、烘焙或替换动画。
- 先成功保存独立备份，再转换并自动保存选中的目标；检查和预览保持只读。

原属性、枚举值、Tag、资产路径和序列化字段保留，不建立第二份削韧数值或击飞开关，不自动清空隐藏配置。已有工作区修改与存量资产不作为本次批量迁移对象。

## Details 组织与原生行为

- Definition：“攻击分段 · 命中意图”保留身份、执行路由、动作 Body 和默认命中配置；“动画时机 · Montage”使用原 `ExecutionConfig`；旧收益字段进入“兼容配置与迁移”，其余配置沿用既有入口。
- PawnData：“目标 · 基础与受击规则”集中角色、属性、能力包、Profile 和 Poise。基础削韧、地面运动及击飞速度保持常规可达；霸体积累、样本容量和距离上限进入高级组。
- Profile：按受击动画、普通接触、倒地起身与恢复分区；运动安全、恢复尾部和曲线进入高级区；旧 `Animations` 与迁移工具进入兼容区。布局选择仍由原 `bUseCompactAnimations` 唯一决定。
- 通知：几何、伤害/反应继承及分段 `Can Launch` 保持常规入口；查询与命中去重进入高级组。显式 Impact、旧动画方向变体、Sustain/Recovery 和逐阶段运动来源在对应高级组编辑。

所有编辑继续使用原 `PropertyHandle` 和原生属性行，保留 `EditCondition`、`EditConditionHides`、Clamp、标签筛选、撤销和多对象值语义。Poise 关闭不能额外禁用分段击飞速度。旧 Break 字段和运行态 Reaction 的 `bCanLaunch` 不生成作者控件；通知 `Damage.Reaction` 的既有定向隐藏继续生效。

Reaction 常规字段前显示只读制作提示：常规制作使用 PoiseDriven；Legacy 为存量兼容，ExplicitImpact 为高级专项；Can Launch 在分段通知配置。提示不修改原 InputMode 枚举、默认值或条件。

## 迁移的拒绝边界

迁移仅允许写入当前 Profile 的 `Compact` 和 `bUseCompactAnimations`，旧数组及其他冻结字段保持原值。预览展示读取目标、资产注册表包引用者、差异和备份目标；包引用列表不代替完整运行时调用链核查。

**无损要求覆盖 Runtime 全部有效输入分支的解析结果，不能只证明某一次动画播放相同。** 比较包含 Impact 支持范围、反应等级、方向、受力侧、变体身份、实际选片、控制时长和被消费的阶段资源/运动来源。仅在方向与力度变体已被限制为可完整表示的情况，才可用有限分支枚举完成等价核对。

- 旧布局的 `HitStun` 在 Heavy 等级仍解析自己的条目；Compact 在 Heavy 及以上改查 Heavy/Knockback。两者选片或控制时长不同即拒绝，不能把字段对应当作行为等价。
- Compact 的 `AirHit` 和 `Slam` 共用 `Push`。只存在其中一种旧类型，或两种类型资源、转段、起身等不能合并时拒绝；不能隐式增加支持，也不能以后写入的条目覆盖前一条。
- 不同 GetUp、不同公共空中转段时间、根运动、Sustain/Recovery、未知或不一致骨架、缺资源、非法配置、方向/力度歧义、隐藏 Compact 已有不同手调均停止当前资产。
- 备份目标已存在或已加载时拒绝覆盖；预览后配置变化时重新预览；已是 Compact 时只报告，不重复备份或保存。

执行前重新核实计划。备份失败不改源资产；目标校验或保存失败时恢复本次写入的内存布局并报告。保存失败后的磁盘状态仍须核对，必要时使用已保存备份逐项恢复，不宣称磁盘写入具有原子性。

## 使用入口

1. 按项目流程完成常规构建，打开现有 Definition、PawnData 或 Profile，核对上述 Details 分类；Montage 仍在其通知轨编辑实际时机。
2. 点击“检查当前资产（不保存）”，阅读当前资产既有 `IsDataValid` 诊断。检查不负责修复配置，也不保存包。
3. 需要迁移时只选择一份旧布局 Profile，在“兼容配置与迁移”填写新备份目录，例如 `/Game/Tests/Combat/ProductionWorkflow/Backups/20261019_01`；此路径仅为示例，必须核对本批次未占用。
4. 点击“只读迁移预览”，核对目标、引用者、差异、备份目标及停止原因。任一拒绝条件存在时保留旧布局，不手动改开关绕过检查。
5. 预览允许后点击“备份并转换、保存当前目标”。核对明确成功或失败结果；修改备份目录或目标配置后重新预览。
6. 重新打开目标核查持久化值，再按制作执行文档第 9、13～16 节执行单目标与网络验收。配置检查通过不代表动作、真实落地或控制释放通过。

## 复用依据

已核查本机 Lyra 的 `Source/LyraGame/Character/LyraPawnData.h`：继续使用 PawnData 资产组织，不增加平行角色配置。`Source/LyraEditor/LyraEditor.cpp` 的 `CheckGameContent_Clicked` 调用现有校验链；本次同样沿用资产 `IsDataValid`，不复制一套玩法校验。检索 Lyra 的 Source/Plugins 未找到同职责的战斗 Profile 无损转换与 Details 分组实现，因此不声称直接迁入该功能。

本项目复用 [既有模块注册](../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorModule.cpp)、[通知伤害定向定制](../../Source/HodgeAbilityEditor/Private/HodgeNotifyDamageCustomization.cpp) 与 [Runtime 解析](../../Source/Hodgepodge/Private/Data/HodgeHitReactionProfile.cpp)。新增代码位于 [资产面板](../../Source/HodgeAbilityEditor/Private/HodgeCombatAssetCustomization.cpp)、[结构分组](../../Source/HodgeAbilityEditor/Private/HodgeCombatPropertyCustomization.cpp) 和 [迁移实现](../../Source/HodgeAbilityEditor/Private/HodgeHitReactionMigration.cpp)。

引擎依据根目录为 `E:/UE/UE_5.5/Engine`：`Source/Editor/PropertyEditor/Public/IDetailChildrenBuilder.h`、同目录 `IDetailGroup.h`、`IPropertyRowGenerator.h` 及该模块 `Private/DetailPropertyRow.cpp` 核实原生行、条件隐藏与实际详情树；`Source/Editor/UnrealEd/Public/ScopedTransaction.h`、`Source/Runtime/CoreUObject/Public/UObject/SavePackage.h` 和 `Source/Runtime/AssetRegistry/Public/AssetRegistry/IAssetRegistry.h` 核实事务、指定包保存及引用查询。适配仅在已有 Editor 工具模块内实现。

## 实际验证结果

- Editor 常规构建退出 `0`；Game 常规构建退出 `0`，提示目标已是最新。没有改 Runtime、反射字段、模块依赖或构建设置。构建前通过编辑器 API 确认脏内容和地图均为空，正常退出编辑器；没有使用 Live Coding 或强杀用户会话。
- `Hodge.Editor` 共 **8/8 通过**，包括三个真实 Details 属性树测试、三个迁移测试、既有通知伤害定制及动画实例检查。最终报告为 `Saved/CombatDetailsRefactor/EditorAutomation-final2/index.json`，其中7项成功、1项带既有警告成功、0失败。
- Details 测试验证实际分类、Compact/Animations 互斥、原生条件隐藏、削韧关闭时窗口禁用而两项击飞速度仍可编辑、多对象差异、通知继承及唯一分段击飞入口。初轮测试即时读取了尚未更新的 Slate 状态缓存，修正为刷新真实控件属性并同时检查原生行条件，保留原禁用断言后通过；未为该问题改变生产逻辑。
- 迁移测试实际保存自己的随机新建源、资源和备份包，验证无损映射、Heavy/Push/方向力度/起身/根运动/骨架/手调等拒绝边界、过期预览、已有备份、自动保存、旧数组与冻结参数保留、已 Compact 不重复保存。随后通过独立内存包身份冷读源文件和备份文件，验证磁盘内容；读取前故意改变原内存值，避免缓存假通过。测试结束精确清理自己的文件，没有转换现有角色资产。
- `Hodge.HitReaction` **38/38 通过**（29项成功、9项带既有警告成功、0失败），报告为 `Saved/CombatDetailsRefactor/ReactionAutomation/index.json`。
- 冷启动只读加载三份实际资产成功，加载前后脏包均为空：`/Game/CodexText/HitReactionValidation/DA_ReactionProfile`（旧布局）、`/Game/CodexText/HitReactionValidation/Dummy/DA_DummyPawnData`、`/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1`。读取报告为 `Saved/CombatDetailsRefactor/existing-assets.json`；没有保存这些资产。
- 在新的隐藏专用编辑器中运行目标受击套件，**12场景、89断言通过**，包含轻重左右、任意削韧不自动击飞、明确击退、取消、高抗、续飞开关与次数上限、密集命中、真实 Fly Start/Loop/Fall、起身和每场最终释放。12份释放快照均无残留控制、RMS、动画根运动或受击实例；日志有 `POISE_RUNTIME_CLEANUP single 0 None`。
- 另起专用编辑器运行正式输入动作闭环，**7项检查通过**：锁定、五段顺序、逐刀伤害/反应、仅终结刀 Launch、移动、闪避和闪避后攻击；日志有 `ACT_ACTION_LOOP_CLEANUP_OK`。这是 Enhanced Input 注入，不是物理键鼠或主观动画评分。

两次 PIE 的批次为 `DetailsRefactor_20261019_143912`，模式均为 single、0ms、假人目标。报告位于 `Saved/CombatProduction/DetailsRefactor_20261019_143912/poise-single-0ms-dummy/` 和同批次的 `action-single-0ms-dummy/`，各自 `batch.json.error=null`。启动器从制作文档模板提取，仅选择任务与新批次；遵守独占进程、PID和初始无脏包保护，每个任务单独进程，不保存临时覆盖。

首次构建发现测试使用了本机不存在的 `FindFPropertyChecked`，改为 UE5.5.4 实际支持的 `FindFProperty` 后构建通过。最后的 Editor 构建日志为 `Saved/CombatDetailsRefactor/EditorBuild-final.log`。既有插件依赖/包含顺序等提示保留，未通过升级或清理无关代码隐藏它们。

## 实际命令

以下在项目根目录以 PowerShell 执行；命令行自动化进程退出码均为0，结论同时核对报告中实际成功/失败数量，没有仅凭退出码判定：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecutePythonScript=E:/Project/Git/Hodgepodge/Saved/CombatDetailsRefactor/check_existing_assets.py' '-ExecCmds=Automation RunTests Hodge.Editor' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/CombatDetailsRefactor/EditorAutomation-final2' '-abslog=E:/Project/Git/Hodgepodge/Saved/CombatDetailsRefactor/EditorAutomation-final2.log'
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.HitReaction' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/CombatDetailsRefactor/ReactionAutomation' '-abslog=E:/Project/Git/Hodgepodge/Saved/CombatDetailsRefactor/ReactionAutomation.log'
```

两次专用 PIE 的实际启动参数如下（启动时使用 `Start-Process -WindowStyle Hidden -Wait -PassThru`，退出后核对对应 batch、用例断言与清理标记）：

```powershell
Start-Process -FilePath 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor.exe' -ArgumentList @('"E:/Project/Git/Hodgepodge/Hodgepodge.uproject"', '-unattended', '-nosplash', '-nosound', '-windowed', '-ResX=1280', '-ResY=720', '-HodgeCombatValidationExclusive', '-ExecutePythonScript="E:/Project/Git/Hodgepodge/Saved/CombatDetailsRefactor/launch_workflow.py"', '-abslog=E:/Project/Git/Hodgepodge/Saved/CombatProduction/DetailsRefactor_20261019_143912/Editor-poise-single-0ms.log') -WindowStyle Hidden -Wait -PassThru
Start-Process -FilePath 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor.exe' -ArgumentList @('"E:/Project/Git/Hodgepodge/Hodgepodge.uproject"', '-unattended', '-nosplash', '-nosound', '-windowed', '-ResX=1280', '-ResY=720', '-HodgeCombatValidationExclusive', '-ExecutePythonScript="E:/Project/Git/Hodgepodge/Saved/CombatDetailsRefactor/launch_action.py"', '-abslog=E:/Project/Git/Hodgepodge/Saved/CombatProduction/DetailsRefactor_20261019_143912/Editor-action-single-0ms.log') -WindowStyle Hidden -Wait -PassThru
```

以上报告目录已占用；重新执行时按制作文档使用新批次，不覆盖本次证据。Saved 中启动器、日志和报告是本次验证产物，不纳入源码。

## 未执行项与交付边界

没有执行交互式完整面板视觉/按钮点击验收、蓝图 Compile、物理键鼠/手柄、主观动画评分、Listen/PIE Dedicated/独立多进程、100ms网络矩阵、Cook或打包。自动化已验证真实属性树和迁移后端，但不能据此声称面板视觉、联机或打包通过。没有开启第16.4节暂缓的玩法修复，也没有批量迁移现有资产。

开始工作区已有214条状态记录；本次新增9个文件，修改既有 Editor 模块注册和制作文档，结束核对保留原214条记录，无移除项。新文件为三对 Editor 实现头/源、两份测试及本文；没有自动提交或推送。测试新建包已清理，源码变更不包含生成文件、缓存、日志或正式二进制资产。

交付文档检查通过：34个本地文件链接、4个Python示例AST、5个PowerShell代码块语法，以及本次代码/文档尾随空白和差异检查。现有日常编辑器已正常关闭用于构建；重新打开项目后即可在原资产 Details 使用上述入口。
