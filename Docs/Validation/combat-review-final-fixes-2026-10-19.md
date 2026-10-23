# 战斗复审收尾修复与验证记录

日期：2026-10-19；基线：dev-Combat / 891fd70 加开始时 92 条未提交工作区状态；UE 5.5.4。

依据：[修复后详细评审](../Design/combat-refactor-post-fix-review-2026-10-19.md)第 4.2～4.5 节。本文记录当前实施批次，历史 186 项原生／216 项玩家检查不算本轮复跑。

## 1. 范围与用户确认

- 修复最终受击选片的动画依赖检查；保留无动画许可、分段击飞、伤害与正式资源。
- 用户确认保留 InspectMontageState 逐帧采样，并新增 InspectMontageStates 查询全部同资源实例用于释放检查。
- 用户确认验证脚本仅允许本任务独占启动的编辑器运行，不支持已有编辑会话的状态恢复。
- 加强服务器等待测试的截止时间断言，不改变真实等待时间或网络协议。
- 拆类、正式终结刀漏判、Client 共鸣回路前窗口异常继续暂缓；本轮不启动它们的实施或排查。
- 保留用户已有文件、正式资产和当前分支；不自动提交。

## 2. 实现及契约

### 2.1 最终选片先验证，再提交副作用

UHodgeHitReactionComponent::BuildPlan 将 AnimInstance 检查移至空中换片及地面续播决策之后，并检查有效的 AbilityActorInfo。实际最终主片非空但依赖未就绪时直接失败；合法无动画路径继续可用。

新增 Hodge.HitReaction.FinalAnimationDependency 使用正式原地受击片和兼容 Mesh，故意不创建 AnimInstance；真实 GAS 动作仍处于活动状态。分别验证计划构建拒绝，以及通过真实伤害提交的命中返回 Unsupported，旧动作、控制标签和速度不受失败计划影响。

仅移动原有依赖检查，不改变控制时长、蒙太奇选择、已解析状态复制或 CMC／RMS 执行方式。

### 2.2 同资源全部实例检查

在现有 HodgeAbilityEditor 的主实现文件中新增 InspectMontageStates，返回全部匹配的 FHodgeMontageValidationState；原单实例函数未改变。

运行时脚本的逐帧采样继续使用原接口，最终释放使用全实例接口和实际实例 ID，保留正在播放或仍有权重的所有实例。

新增 Hodge.Editor.MontageAllInstances 使用真实 AnimInstance 连续播放同一资源，停止最新实例而保留旧实例，验证旧实例不能被遮蔽；随后全部停止才允许释放。测试操作资产副本，不保存正式包。

Python 离线失败注入同时覆盖“旧实例有权重、最新实例已归零”以及原有观察者身份和控制／运动残留断言。

### 2.3 独占编辑器入口保护

validate_poise_runtime.py 在修改地图、PIE 设置或探针之前检查：

1. 编辑器命令行包含精确的 -HodgeCombatValidationExclusive 标记。
2. 调用上下文 POISE_EXCLUSIVE_EDITOR_PID 等于当前进程的真实 PID。
3. 没有已存在的脏内容包或脏地图包。

缺少任一条件明确报错，不启动测试或重载资源。该标记是防误用的执行契约，不是权限或安全隔离机制。原有临时探针恢复和重载只在通过入口保护的独占会话中运行。

离线测试执行实际脚本的拒绝入口，确保保护失败先于任何编辑器修改；不以纯函数测试冒充实际引擎启动验证。

### 2.4 等待超时精度

Hodge.Combo.DeferredServerPipeline 记录与生产入口相同的世界时钟截止时间。在 Timeout 场景逐帧确认来源仍活动、截止前队列保留且目标不激活、到期后移除。原开窗、取消和重复请求用例保留，不修改生产 ASC 调度。

## 3. Lyra 与引擎核查

- 本机 Lyra 的 LyraGameplayAbility::CanActivateAbility 和 LyraAbilitySystemComponent 保留 ActorInfo／现有 GAS 权限链，本轮没有替换该链或迁入新能力系统。
- LyraGameplayAbility_RangedWeapon::OnTargetDataReadyCallback 的稳定数据持有原则继续沿用；本轮未修改上批 TargetData 修复。
- 本机 Lyra 未找到可直接迁入的同职责全实例释放检查；现有项目工具已使用 UAnimInstance::MontageInstances，本轮在同类中补齐遍历范围。
- UE 5.5.4 AnimInstance.cpp 的 Montage_PlayInternal 为每次播放创建 FAnimMontageInstance 并加入 MontageInstances，说明资源与实例身份不能混同。
- FileHelpers.h 的 UEditorLoadingAndSavingUtils::GetDirtyContentPackages／GetDirtyMapPackages 和 KismetSystemLibrary.h 的 GetCommandLine 提供本轮入口校验所用接口。
- 不增加模块依赖、插件、Runtime 反射字段或玩法 RPC；仅在现有 Editor 工具类增加用户确认的查询接口。

## 4. 验证过程

### 4.1 修复前证据

- 仅新增最终动画依赖测试后，常规 Editor 构建成功，68.62s。
- BeforeFix/index.json：1 项失败，明确暴露计划未拒绝、真实命中结果不符、旧能力被取消三项断言。
- 测试进程退出码为 0，但 JSON 报告为失败；本轮始终以断言报告判断结果。

### 4.2 Runtime 修复后阶段结果

- Editor-AfterFix.log：常规 Editor 构建成功，55.38s。
- AfterRuntimeFix/index.json：187 项，113 Success、74 SuccessWithWarnings、0 Failed、0 NotRun。
- FinalAnimationDependency 与加强后的 DeferredServerPipeline 均为 Success。
- 该阶段尚未包含全实例 Editor 测试和最终接口，不能替代最终构建及回归结果。

### 4.3 离线检查

5 项 Python 断言测试通过：观察者身份、运动／动画残留、旧实例遮蔽、独占会话条件、实际脚本拒绝入口。Python AST 检查通过。

### 4.4 最终构建与受影响冒烟

- 新接口及 Editor 测试首次完整构建成功，226.26s；最终测试夹具修正后的常规 Editor 构建成功，11.87s。日志分别为 Editor-Final.log、Editor-Verified.log。
- 常规 Game 构建成功，79.04s，Game-Final.log。此后仅修改 Editor 测试夹具，未改变 Runtime 或 Game 代码。
- 全实例测试首次因测试夹具使用 GetInstanceForMontage 取得首个匹配实例，误将旧实例当成最新实例而失败。Automation-Final/index.json 为 188 项：113 Success、74 SuccessWithWarnings、1 Failed；改用引擎的 GetActiveInstanceForMontage 获取当次活动映射，未修改业务接口。
- 最终 Automation-Verified/index.json：188 项，114 Success、74 SuccessWithWarnings、0 Failed、0 NotRun；新最终依赖、真实重复实例与超时边界用例均通过。
- 保留既有 include order、内联生成实现和其他已有告警，不调整构建设置或无关代码压掉警告。

### 4.5 实际冒烟与入口保护

首轮串行复用同一独占进程时，正式 R 的 9 项和 Listen 0ms 的 50 项通过；进入下一组前，入口保护发现前组探针 AM_DummyProbe 恢复属性后仍有脏包标记，正确拒绝继续。该批 Smoke/batch.json 保留失败，不将编辑器退出码 0 当成全批通过，也没有放宽保护或保存脏包。

最终改为五个专项各自启动独占冷编辑器，验证后仅退出本任务自己的实例。每次启动均先注入错误 PID，确认入口拒绝且地图和 PIE 设置没有变化，再开始正式专项。

| 最终专项 | 实际结果 | 本轮证据路径 |
| --- | --- | --- |
| 正式 R 输入 | 9 项通过，真实伤害、Fly Start／Loop／Fall、落地恢复 | Smoke-Verified/r/r-input-smoke.json |
| 玩家受击 Listen 0ms | 4 场景、50 检查通过 | Smoke-Verified/listen-0ms/runtime-listen-0ms.json |
| 玩家受击 Listen 100ms | 4 场景、50 检查通过 | Smoke-Verified/listen-100ms/runtime-listen-100ms.json |
| 玩家受击同进程 Dedicated 0ms | 4 场景、58 检查通过 | Smoke-Verified/dedicated-0ms/runtime-dedicated-0ms.json |
| 玩家受击同进程 Dedicated 100ms | 4 场景、58 检查通过 | Smoke-Verified/dedicated-100ms/runtime-dedicated-100ms.json |

玩家专项合计 16 场景、216 检查、40 条逐端释放快照，正式 R 的 9 项另计。五份 batch.json 均无 error／failures，五次错误 PID 保护探针均通过。Hero／Guardian／Dummy 三份 Profile 在各组冷启动中验证有效、零错误零警告；这仍是三份不同资产，不因重复验证计作十五份。

逐帧采样继续使用原接口；最终快照实际调用新的全实例查询。拥有者按 PlayerId 匹配，Dedicated 观察者排除服务器和受击拥有者，Listen 仍只声明双人拓扑的主机观察。100ms 是 Net PktLag=100 配置，不是实测 RTT；同进程 PIE Dedicated 不等于独立进程专服。

汇总见 verified-summary.json；上述路径相对 Saved/CombatReviewFinalFix/20261019。没有保存正式资产、重新编译全部蓝图或做额外视觉验收。实际冒烟期间八份代码文件哈希未变，对应已通过构建和最终回归的版本。

## 5. 文件与保护范围

本批已有文件备份、原始 Git 状态与哈希保存在 Saved/CombatReviewFinalFix/20261019/Before、files-before.json、status-before.txt。

已有文件修改：

- Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp。
- Source/Hodgepodge/Private/Tests/HodgeHitReactionTests.cpp。
- Source/Hodgepodge/Private/Tests/HodgeCombatActionRequestTests.cpp。
- Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.h／.cpp。
- Tools/ACTCombat/validate_poise_runtime.py。
- Tools/ACTCombat/tests/test_runtime_assertions.py。
- 文档评审入口及 Docs/README.md。

新增 Source/HodgeAbilityEditor/Private/Tests/HodgeCombatValidationLibraryTests.cpp 及本文。

冒烟前重新计算相关正式资源与探针共 196 份 uasset 哈希，均与上一批清单一致；最终冒烟后 196 份哈希仍全部一致，assets-after-smoke.json 的 changed 为空。该范围不等于全仓资产或未保存编辑器内存状态。

开始时 92 条 Git 状态均保留，结束为 93 条，新增状态为本文；已有未跟踪目录内的新测试文件单独列入本批清单。file-manifest.json 记录九份已有文件和两份新增文件，不将工作区其他既有改动计作本轮成果。分支和 HEAD 未变。

## 6. 实际命令入口

构建沿用 Docs/AI_DEVELOPMENT.md：Build.bat 的 HodgepodgeEditor／Hodgepodge、Win64、Development、-WaitMutex、-architecture=x64、-NoUBA、-NoUBALocal、-MaxParallelActions=1；Editor 另带 -NoHotReloadFromIDE。

原生回归使用 UnrealEditor-Cmd.exe、-unattended、-NullRHI、-ExecCmds=Automation RunTests Hodge、-TestExit=Automation Test Queue Empty，并输出独立 JSON 报告。离线检查使用 E:/Python/python.exe -B -m unittest discover -s Tools/ACTCombat/tests -v。

实际冒烟通过 Start-Process -WindowStyle Hidden 启动 UnrealEditor.exe，参数为项目绝对路径、-unattended、-nosplash、-nosound、-windowed、-ResX=1280、-ResY=720、-HodgeCombatValidationExclusive，以及对应的 -ExecutePythonScript 和 -abslog。本轮生成的五份 run_smoke_专项名.py 启动脚本向验证上下文传入 os.getpid()，每组报告单独保存；父流程同时检查进程退出码与 batch.json 断言，再进入下一组。

本轮生成日志、脚本、备份和报告统一位于 Saved/CombatReviewFinalFix/20261019，不纳入源码提交。没有使用 Live Coding 替代常规构建，没有关闭或强制结束用户编辑器。

## 7. 收尾结论

- 评审第 4.2 节最终依赖、第 4.3 节全实例采集、第 4.4 节独占会话保护已按确认范围实现并完成对应回归。
- 评审第 4.5 节服务器超时精度断言已加强；不据此关闭整个生命周期网络矩阵。
- 最终常规 Editor／Game、188 项原生、5 项离线及本批实际受影响冒烟通过；首次测试夹具失败和首个复用进程被保护拒绝的证据保留。
- 仅完成本批明确的正确性和测试工具切片，不代表全部结构重构、资产迁移或战斗手感验收完成。

## 8. 未完成及暂缓项

- 独立拆类继续暂缓；不创建 PlanBuilder、接触表现宿主、长按或电影会话类。
- 正式五段终结刀漏判、Client 共鸣回路前窗口异常不恢复排查，不用专项通过替代关闭。
- 真实网络传输、恢复输入、受击中死亡、换 Pawn／重生／解绑全矩阵不由服务器注入用例替代。
- 本轮不删除兼容字段或旧资产，完整逐资产退出台账未完成。
- 未进行同负载 CPU／性能测量、独立进程专服、Cook／打包、正式素材制作或主观手感验收。
