# 战斗代码评审整改与验证记录

日期：2026-10-18；基线：`dev-Combat / 891fd70` 及开始时未提交工作区；UE 5.5.4。

依据：[战斗模块详细评审](../Design/combat-code-review-2026-10-18.md)。本记录区分本轮实际结果与待完成内容，不沿用历史测试作为本轮通过证明。不提交、不切分支、不重存正式资产。

> 后续复审见[战斗重构复审与跟进清单](../Design/combat-refactor-follow-up-review-2026-10-18.md)：新增方向数据生命周期、跨阶段资源校验和联机断言等发现；F06 仍未关闭，F12 的断言范围需校正。下文保留整改批次当时的实现与测试结果，不表示这些新增发现已修复。

## 已确认的实施规则

- 按评审阶段顺序实施，保持单 Runtime 模块、现有 GAS／Init State／预测及资产兼容约束。
- 发现歧义先暂停对应修改，不以默认假设替代用户确认。
- F02 已获明确确认：追加伤害继续同步结算，每笔接受／生命变化事件保留自己的结果快照；UI 读取实时生命。外层 100→90、内层 90→85，最终为 85。
- 真正消耗生命的事务负责死亡通知；零生命损失的内层追加效果不抢占致死来源。死亡通知登记在广播前，监听器重入不重复触发同次死亡。
- F03：同一目标动作存在合法开放边时不等待同目标闭窗边；普通下一刀开放而请求的另一派生目标仍闭窗时，保留原有有界等待。
- F04：只在攻击 Notify 中隐藏 `Damage.Reaction`，保留序列化数据、Definition 默认反应与独立覆盖入口。
- F06：无关方向／家族错误不阻断当前有效受击；本次必需资源和安全限制仍检查，完整资产校验仍报告全部配置错误。
- 第 3～4 阶段：先在原类内收敛入口；独立拆类的具体方案另行确认，不改变现有 RPC、复制状态或 ASC 所有权。
- 正式五段终结刀漏判经报告后，用户明确选择“先记录漏判，本轮先保留已完成整改”；本轮不继续修复该问题，不调整攻击位移、通知时机、伤害或素材。

## 当前范围与进度

| 项目 | 本轮处理 | 状态 |
| --- | --- | --- |
| F01 | 捕获来源时保留 Radius；真实检测会话验证 Overlap／Sweep 内外边界 | 修复及原生回归通过 |
| F02 | 先消费 Damage Meta；独立生命快照；同步嵌套和死亡重入 | 修复及原生回归通过 |
| F03 | 共享 Ready／Wait／Reject 判定，区分同目标冲突和不同派生目标等待 | 修复及原生回归通过 |
| F04 | 相关字段校验、失败诊断、Notify 嵌套 Reaction 作者面 | 原生与实际 PropertyRowGenerator 详情树测试通过 |
| F05 | 独立 Fly 只校验本次使用的目标运动预设 | 修复及原生回归通过 |
| F06 | 完整 Profile 校验与已选路径校验分开 | 定向及最后非法重复条目回归通过；正式 Hero／Guardian／Dummy 完整资产校验通过 |
| F07 | Definition 原类内集中执行资源释放；保留现有唯一状态发布入口 | Editor／Game 与最终原生回归通过；只完成原类内切片，不代表整阶段完成 |
| F08／F09 | 受击职责、角色输入及初始化边界 | 未拆类；按用户决定另行确认具体迁移方案 |
| F10 | 登记当前作者面、兼容优先级与退出条件 | 本文已记录；未删除反射字段或迁移资产 |
| F11 | 实测后优化，禁止无测量批量调整 Tick／采样 | 尚未实施 |
| F12 | 边界用例随修复增补；新增真实玩家作为受击目标的网络基线 | 四模式 16 场景／192 断言通过；恢复输入、死亡与换 Pawn 等全矩阵仍未完成 |

## 变更与保留边界

- `HodgeHitDetection.cpp`：只补齐 socket 来源的捕获半径，沿用正常 Sweep 的厘米与缩放语义。
- `HodgeHealthSet.cpp`：本笔临时伤害在可重入观察前清空；后续通知使用本笔生命快照，死亡按实时生命和消费资格去重。
- `HodgeCombatComponentBase.h/.cpp`：私有纯判定结果供本地选边与服务器等待共用，不修改 RPC、复制数据、预测键或输入窗口。
- `HodgeHitDetectionTests.cpp`：覆盖捕获半径及 ExistingSource 真实会话下的查询内外边界。
- `HodgeHealthDamageTests.cpp`：通过真实瞬时 GE 覆盖同步嵌套、事件快照、过量伤害、内外致死和死亡回调重入。
- `HodgeAbilityDefinitionTests.cpp`：补充同目标开闭边并存、开放边被来源标签阻断及另一目标开放但派生目标仍等待的情况，保留原身份拒绝测试。
- `HodgeCombatAnimNotifies.cpp`：按来源／形状校验实际消费字段；`HodgeGameplayAbility_Melee.cpp` 输出带 Definition、Montage、来源和形状的失败原因。
- `HodgeNotifyDamageCustomization.*`：在既有 Editor 模块注册定向属性定制；仅攻击 Notify 的嵌套 Damage 隐藏 Reaction。真实详情树测试确认 Definition 的同一结构仍显示该字段。
- `HodgeHitReactionTypes.*`：将独立 Fly 的速度／位移倍率校验从整份削韧规则中分离，普通削韧仍完整校验。
- `HodgeHitReactionProfile.*`：一次解析本次方向／力度资源，返回值副本；完整校验和路径校验共用单条动画契约，无缓存或新状态机。
- `HodgeHitReactionComponent.cpp`：计划只检查实际路径，击退曲线只在外部击退运动中求积分；持续沿用 Body、实际空中／落地及控制链限制。
- `HodgeGameplayAbility_Definition.*`：`EndAbility` 内调用唯一私有 `ReleaseExecutionResources`，原清理语句及顺序逐字保留，不增加独立实现文件。
- `Tools/ACTCombat/validate_poise_runtime.py`：显式可选真实玩家目标、独立报告目录、按 PlayerState 身份定位各端对应目标，保留原假人默认入口；测试修改的通知参数在结束时恢复。
- `Tools/HitReactionDummy/control.py`：测试观察器支持明确传入服务器玩家目标，站位重置同时对齐攻击者和受击玩家各自的拥有者；不创建替代玩家 ASC、不篡改生命上限。
- 正式 HUD 已在通知后调用 Refresh，重新读取实时 Health；本轮不修改 UI 或伤害公式。

本轮留存的修改前文件位于 `Saved/CombatReviewRefactor/20261018/Before`，起始工作区状态与首批七个文件的 SHA-256 记在同目录；这不是整个工作区的统一快照。该目录、构建日志与测试输出均为忽略生成物，不纳入提交。

### 文件清单

下列路径相对于项目根目录，仅列本轮改动，不将开始前其他未提交资产归为本轮成果：

- Runtime：`Private/Combat/HodgeHitDetection.cpp`、`Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp`、`Public|Private/Component/HodgeCombatComponentBase.h|cpp`。
- 作者校验与诊断：`Private/Animation/HodgeCombatAnimNotifies.cpp`、`Private/AbilitySystem/Abilities/HodgeGameplayAbility_Melee.cpp`。
- 受击：`Public|Private/Combat/HodgeHitReactionTypes.h|cpp`、`Public|Private/Data/HodgeHitReactionProfile.h|cpp`、`Private/Component/HodgeHitReactionComponent.cpp`。
- 执行收敛：`Public|Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h|cpp`。以上文件均位于 `Source/Hodgepodge/`。
- Runtime 测试：`Source/Hodgepodge/Private/Tests/` 下的 `HodgeHitDetectionTests.cpp`、`HodgeAbilityDefinitionTests.cpp`、`HodgeAnimNotifyTests.cpp`、`HodgeHitReactionTests.cpp` 和新增 `HodgeHealthDamageTests.cpp`。
- Editor：`Source/HodgeAbilityEditor/Private/HodgeAbilityEditorModule.cpp`，新增 `HodgeNotifyDamageCustomization.h/.cpp`、`Tests/HodgeNotifyDamageCustomizationTests.cpp`。
- 工具：`Tools/ACTCombat/validate_poise_runtime.py`、`Tools/HitReactionDummy/control.py`。
- 文档：本文件、`Docs/Design/combat-refactor-next-boundaries-2026-10-18.md`、原评审的实施记录链接、`Docs/README.md` 索引。

精确逐文件列表另保存在 `Saved/CombatReviewRefactor/20261018/task-file-manifest.json`（28 个文本文件）。没有修改模块依赖、引擎配置、第三方插件或正式二进制资产，没有提交或切换分支。

## 修复前：先确认新增回归能够捕获问题

先只加入测试，保留原生产逻辑，执行常规 Editor 构建和 `Hodge.*` 自动化：

- Editor 构建成功，日志 `Editor-Regression-BeforeFix.log`，总耗时 258.87s。
- 共 175 项：105 Success、65 SuccessWithWarnings、5 Failed、0 NotRun。
- 五项失败为 `Hodge.Combat.DamageAcceptedDeathReentry`、`DamageAcceptedReentry`、`ExistingSourceQueryRadius`、`HitGeometry` 和 `Hodge.Combo.Retention`。
- 实际观察到同步 10＋5 后生命为 75、死亡重入通知两次、捕获 Radius 为 0、Overlap 半径内漏检，以及开放边仍被送入等待。
- 进程退出码为 0，但 JSON 报告明确有五项失败；不以进程退出码代替断言结果。

完整证据位于 `Saved/CombatReviewRefactor/20261018/Automation-BeforeFix/index.json` 和相邻日志。本轮由静态条件性问题进一步取得了隔离原生运行证据；不表示正式全部资产都曾触发这些问题。

## 修复后验证

- 阶段 1：Editor 构建成功（282.21s）、Game 构建成功（294.91s）；175 项原生测试零失败，109 Success／66 SuccessWithWarnings。证据归档于 `Stage1/`。
- F04／F05：Editor 构建成功（286.10s），178 项零失败，112 Success／66 SuccessWithWarnings；包括真实通知详情面板树检查。证据归档于 `Stage2-Initial/`。
- F03 最终规则与 F06：Editor 构建成功（175.95s）；181 项零失败，115 Success／66 SuccessWithWarnings。新增左右坏配置隔离、实际击退曲线拒绝、Fly 必需 Loop／Landing 缺失用例。
- Definition 原类内清理提取后 Editor 再次成功（167.95s），其后完整 181 项原生回归零失败。
- 后续 Editor（47.04s）和 Game（45.07s）增量构建通过，181 项原生测试为 115 Success／66 SuccessWithWarnings／0 Failed。
- **最终源码：Editor 构建成功（103.44s）、Game 构建成功（69.83s）；181 项原生回归为 114 Success／67 SuccessWithWarnings／0 Failed／0 NotRun。** 最后一项警告变化来自实际选中重复条目的预期拒绝诊断；重复配置不能伪装成“缺少条目”而绕到 Fallback。
- 最后补充只涉及旧布局实际所需条目的重复检查及相关原生用例；前述正式资源均使用 Compact。没有再重跑整套 PIE，已运行的正式资源／PIE 范围与最终非法配置回归分别保留，不将两者混称为最终全矩阵通过。

自动化警告与成功分别计数，不将 SuccessWithWarnings 写成无警告通过；本轮非法配置用例的预期诊断也会进入警告统计。既有 `CodexText` 的 UButton 弃用、插件和 include order 提示未做无关修改。

## 当前配置解释与兼容登记（F10）

### 攻击 Notify

- `bUseDefaultDamage` 只决定 GE／伤害倍率／伤害标签来自 Definition 还是通知覆盖。
- `bUseDefaultReaction` 单独决定 Reaction 来自 Definition 默认值还是 `ReactionOverride`。
- `Damage.Reaction` 在这个通知入口不作为反应来源，现已隐藏；在 Definition 的默认命中配置中仍有效。
- `bCanLaunch` 始终以当前攻击段为准；不能通过旧默认 Impact 或累计削韧重新取得击飞资格。
- 保留四种默认／覆盖组合，不把隐藏字段的历史值改为零，也不删除原 UPROPERTY。
- 退出条件：受支持入口及正式资产依赖全部盘点后，才讨论清除序列化兼容；本轮只关闭重复作者面。

### 来源与几何

- 武器来源使用武器来源的采样几何，不校验通知中未消费的局部变换、Shape 尺寸或锚点距离。
- 非武器 Sphere／Box／Capsule 分别检查自身尺寸，通用倍率和重复间隔始终检查。
- 目标距离仅在指定目标或目标锚点路线生效；AnyInVolume 不因隐藏的旧目标距离被拒绝。
- ConfirmedTarget 的可编辑非武器几何仍保留现有校验，未借本轮扩大目标确认能力。

### 受击配置

- `Compact` 与旧 `Animations` 的选择仍由 `bUseCompactAnimations` 决定，旧字段保留，不混合回退读取隐藏资源。
- 完整资产校验检查所有方向、变体、家族、恢复限制和曲线；运行时只检查当前所选计划。
- 地面受击不因未使用 Push 或轻反馈错误而拒绝；实际外部 Knockback 仍要求有效曲线。
- Compact 的空中路线仍需要实际 Start／Loop／Landing；空中落地倒地路径检查最终使用的 Knockdown／GetUp 资源及运动契约。
- 旧布局显式允许无 Montage／可选恢复的契约不被升级为全资产必填；新入口不隐式放宽其必需资源。
- `FHodgeHitReactionState` 的复制字段、已解析资源及阶段数据未改；本轮不重写正式 Hero／Guardian／Dummy 资产。
- 退出条件：明确旧入口实际资产依赖、迁移备份、引用核查及运行回归后，再确认是否移除旧字段或旧运动路径。

### 角色规则兼容

- `HodgeLegacyAttackRuleAdapter` 仍只在没有同类新 AttackRules 时读取旧收益字段；本轮不修改角色收益或 DamageExecution 公式。
- 保留原因：正式资产的新旧规则依赖未在本轮做完整迁移审计。退出条件为逐项迁移、收益次数对比、撤销与生命周期回归后再单独确认。

## 原类内的事务与清理归属

- Combat：共享选边，准备授权，绑定本地请求；`SetActionRequestStatus` 是记录／发布状态的唯一入口，终结记录不再次修改。
- ASC：继续拥有 GAS 激活、预测键、服务器短暂等待和成功／失败网络适配。等待请求不是另一笔伤害或另一动作状态机。
- Definition：生命周期事件只发一次；`EndAbility` 是结束入口，私有 `ReleaseExecutionResources` 在结束保护内释放 TargetData／Timer、朝向、相机、规则、翼、Montage 委托、手部占用、状态 Tag 与姿势租约，随后才归还 GAS。
- 受击组件：唯一持有权威计划及阶段；受击 GA 执行 Montage／RMS／Launch；接触表现不修改控制或伤害。
- 独立计划解析类、接触表现宿主、角色长按与电影会话拆分均未执行，不以本轮函数提取冒充已完成结构迁移。

后续类、输入输出、权限和停止条件见[待确认的职责迁移方案](../Design/combat-refactor-next-boundaries-2026-10-18.md)。其中已明确现有 HavocRules 为 ServerOnly，不能直接承接本地长按计时。

## 正式资源与真实 PIE 冒烟结果

### 资源及蓝图

- 当前 Hero、Guardian、Dummy 的 HitReactionProfile 均通过完整 EditorValidator 校验，零资产校验错误和警告。
- Hero `/Game/Main/Character/Hero/Anim/ABP_Pover_Base` 编译结果为 `ERRORS=0 WARNINGS=0`。编译信息还包含已有的“Result was visible but ignored”说明，不将其计作编译错误。
- 当前 Hero／Dummy 引用已有 S_L／S_R、B_L／B_R、Fly 和 Push 原地素材家族；Guardian 的 Fly／Push 未配置且完整校验仍合法，本轮未为它补造素材。
- 只在内存编译、查询或临时修改探针；没有执行迁移脚本或保存正式资产。

### 共鸣解放

`validate_resonance_launch.py` 使用正式 R 输入、正式 Definition 与百万血假人，9 项检查通过。包括真实伤害、Start／Loop／Fall、实际空中和落地、起身及控制释放。证据：`Smoke-Final/r-input-smoke.json`。

### 真实玩家受击

Listen Server 和同进程 PIE Dedicated 分别执行 0／100ms 配置。每组使用真正由 PlayerState 持有 ASC 的第二名玩家作为目标，分别采样 Authority、受击拥有者与观察者；通过 Actor／PlayerState 身份区分对象。

每组四个场景为轻硬直左侧、重硬直右侧、击飞后开关开启续飞、击飞后开关关闭正常落地起身。共 16 场景／192 检查通过：Listen 每组 46 项，Dedicated 每组 50 项。

检查覆盖真实 Notify／GE 伤害、S／B 选片、重击约 65cm、外部运动和控制释放、Fly 三段及起身、开关决定的 Montage 实例次数、原控制链起点、拥有者实际受控、观察者收到阶段、各端最终位置收敛。

证据位于 `Smoke-Final/runtime-{listen,dedicated}-{0,100}ms.json`。这里的 100ms 为 `Net PktLag=100` 配置，不是实测端到端延迟；最终位置收敛不证明全程没有网络纠正，不替代独立进程专服或手感验收。

### 正式五段普攻：未通过，保留失败

`validate_action_loop.py` 确认锁定目标和顺序 `1→2→3→5→4`，前四段均产生 31 点实际伤害，轻／重受击也真实发生。**终结刀没有在 3 秒观察期限内产生伤害，后续移动取消／闪避／闪避后普攻阶段因此未执行，不能标记这条完整闭环通过。**

采样事实：

- 第三段后目标约在 `(244.77, -5.54)`，产生既定重受击退步。
- 终结段起手攻击者约 `(259.94, 73.21)`，与目标水平距离约 80.2cm；起手 Yaw 约 -30.87°，当时目标方向约 -100.90°。
- 观察期限末攻击者约 `(308.58, -187.23)`，朝向约 -95.23°，与目标距离约 192.57cm；没有第五笔终结段结算。

上述数据证明当前场景漏判，不足以确认根因或归因于本轮重构；不能直接认定动画、转向、判定时机中的某一个已经有错，也不能称为已证明的历史失败。没有为了让测试通过而移动目标、改伤害、扩大判定体或重存正式 Montage。

失败证据：`Smoke/action-loop.json`、`Smoke/batch.json` 与 `Smoke-backup-2026.10.18-11.50.15.log`。用户已明确要求本轮先记录，保留已完成整改，不继续修复漏判。依据评审路线图的停止条件，本批不扩大后续结构迁移；日后重启该问题时，再隔离正式攻击的位移、起手朝向和通知时机。

### 测试启动器问题

首次 `-ExecutePythonScript` 启动器没有开启异步保活，UE 在脚本返回后提前正常退出，未生成有效完整测试报告。这次尝试不计为通过。

已按本机 `EditorPythonExecuter.cpp`／`EditorPythonScriptingLibrary.h` 核对，补 `EditorPythonScripting.set_keep_python_script_alive(True)`，再运行上述正式用例；批次自行退出，不结束用户的其他编辑器。首轮日志保留于 `Smoke-StartupOnly.log`。

## 实际命令

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash -nosound '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/CombatReviewRefactor/20261018/Automation-BeforeFix' '-abslog=E:/Project/Git/Hodgepodge/Saved/CombatReviewRefactor/20261018/Automation-BeforeFix.log'
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash -nosound '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/CombatReviewRefactor/20261018/Automation-AfterFix' '-abslog=E:/Project/Git/Hodgepodge/Saved/CombatReviewRefactor/20261018/Automation-AfterFix.log'
$taskEditor = Start-Process -FilePath 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor.exe' -ArgumentList @('E:/Project/Git/Hodgepodge/Hodgepodge.uproject','-unattended','-nosplash','-nosound','-windowed','-ResX=1280','-ResY=720','-ExecutePythonScript=E:/Project/Git/Hodgepodge/Saved/CombatReviewRefactor/20261018/run_smoke.py','-abslog=E:/Project/Git/Hodgepodge/Saved/CombatReviewRefactor/20261018/Smoke.log') -WindowStyle Hidden -PassThru
$taskEditor.WaitForExit()
```

构建命令的输出另重定向至本轮对应日志。UBT 首次在沙箱内因 AppData 配置缓存访问被拒绝而失败，经工具授权后常规构建成功；没有修改引擎、插件或构建设置回避错误。已有插件／include order／弃用告警保留。

## 尚未验证

正式五段终结刀漏判原因及移动取消／闪避后续闭环仍未解决；真实玩家恢复输入、受击中死亡／换 Pawn／重生／解绑全矩阵、独立进程专服、Cook／打包、CPU 性能采样与主观手感验收尚未完成。最后的非法重复动画条目检查有原生回归，正式资源／PIE 不借历史结果冒充最终完整验收。
