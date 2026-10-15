# Definition 独立技能移动取消修复与验证

日期：2026-10-14；引擎：UE 5.5.4；范围：AM_Resonance_Skill1 的移动取消及既有连段回归。设计需求见 [Definition 移动取消](../Design/definition-move-cancel.md)，调参见 [Resonance Skill 第6节](../Guides/resonance-skill-configuration.md)。

## 1. 根因与修复内容

原先 Combat 的移动取消、预测权限、服务器验证和客户端确认都依赖连段 CurrentAbility／IsComboReady。Standalone 不注册到该连段状态，因此动画通知窗口有效但不能取消。没有 ComboDefinition 时组件也会关闭更新。

本轮改为从 ASC.GetAnimatingAbility 查询实际活动 Definition 实例，校验 Spec／Avatar；取消处理独立于连段配置就绪条件，连段转换仍保持优先级。独立技能新增自身窗口标签与输入阈值，默认沿用已有 Status.Attack.Cancel.Move／0.1。Combo 继续使用原配置，并保留零阈值的既有语义。

拥有者使用现有 BeginPredictedMoveCancel 立即预测退出；服务器延后一帧校验执行、窗口、输入和可取消状态，再完整 EndAbility；拒绝调用现有恢复方法。请求 GUID 与各端自己的执行 GUID 防止迟到确认或同实例新执行被误操作。服务器不读取远端 Hero 的本地输入缓存。

取消保留 CommitAbility 已提交的八秒 CD，不修改技能动画、伤害、输入资产，不重做动画状态机。正常取消后实际行走输出恢复 Cycle，加速度 2400、稳定速度约 420。

当前 Skill1 前摇并非全程持有 Status.Attack；窗口前验收的是“移动不会提前结束 GA”，不要求该阶段加速度恒为零。本轮保留原有状态／根运动约束，不额外引入全时长移动锁。

## 2. 代码与工具

- `Source/Hodgepodge/Private/Component/HodgeCombatComponentBase.cpp` 及对应 Public 头：候选查询、路由策略、Tick、预测权限、RPC 身份及配对清理。
- `Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp` 及对应 Public 头：StandaloneMoveCancelWindowTag／StandaloneMoveIntentThreshold 默认与校验。
- `Source/Hodgepodge/Private/Tests/HodgeCombatActionRequestTests.cpp`：4 项新增测试，覆盖独立路由、配置关闭／非法值、输入幅度与阈值边界、窗口前拒绝、Avatar／执行身份、输入阻止、死亡、迟到回执和零阈值 Combo 兼容。
- `Tools/StandaloneMoveCancel`：真实输入回归、批量运行和结果导出；不新增产品测试开关。

Lyra 参考 CancelAbilitiesByFunc 的活动 Spec／实际实例校验与 Hero 原始移动输入职责。通知窗口、移动预测退出和拒绝恢复属于 Hodge 扩展，复用已有实现；没有重复新建 GA 或取消组件。

## 3. 实际验证结果

Editor／Game 常规构建均退出 0，未使用 Live Coding。动作请求组 12 项、连段组 4 项原生测试均为 Success；以 index.json 的具体断言为准，进程退出 0 不单独作为测试通过依据。

冷启动后 5 项资产配置检查通过，GA_Resonance_Skill、GE_Resonance_Skill_Cooldown、GE_Resonance_Skill_Damage 原生编译状态均 BS_UpToDate／无错误，未产生脏内容包。

真实 PIE 最终结果：单人 28 项、Listen 41 项、PIE Dedicated 41 项、Listen 单向 100ms 延迟 41 项，总计 151 项断言通过。每种模式都执行真实 E、移动和五段普攻输入；使用正式八秒冷却，不删除或缩短 GE。

MC00～07 覆盖窗口前按住后松开、低阈值、提前按住跨窗口、窗口内新输入、预测与服务器结束、CD 保留和重按拒绝、标签／命中／姿势／翅膀清理及五段连段。联机追加真实 RPC 拒绝、蒙太奇恢复、持续输入抑制和松键重试。MC08 由原生测试覆盖；这是需求覆盖记录，没有采集代码行／分支覆盖率。

服务器拒绝只在 PIE Actor 上临时添加不复制的 Gameplay.AbilityInputBlocked 标签，通过已有 GAS BlueprintLibrary 调用并配对移除。模拟代理没有完整 AbilitySpec，按动画与观察者标签验收；拥有者和服务器额外验收实际 GA 状态与冷却。

完整断言、报告路径、源码 SHA256 与未验证项见 [结果 JSON](definition-move-cancel-2026-10-14-results.json)。原始逐帧数据、失败历史和日志位于 Saved/StandaloneMoveCancel。

## 4. 实际执行命令

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.ActionRequests' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/StandaloneMoveCancel/ActionAutomation-accepted' '-abslog=E:/Project/Git/Hodgepodge/Saved/StandaloneMoveCancel/ActionAutomation-accepted.log'
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Combo' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/StandaloneMoveCancel/ComboAutomation-delivery' '-abslog=E:/Project/Git/Hodgepodge/Saved/StandaloneMoveCancel/ComboAutomation-delivery.log'
& 'E:/Python/python.exe' Tools/StandaloneMoveCancel/run_batch.py single listen dedicated listen-lag
& 'E:/Python/python.exe' Tools/StandaloneMoveCancel/export_results.py
```

最终构建日志为 EditorBuild-delivery.log／GameBuild-delivery.log。PIE 最终报告为 single/listen/dedicated/listen-lag-e2e.json；分批复跑日志保留 accepted／remaining／lag-final／final-regression 等名字，不混用历史失败报告。

## 5. 排查中发现的问题与处理

- 首次构建测试标签类型不匹配：修正测试比较方式，重新构建。
- 原生阈值边界失败：FVector2D 双精度与 float 平方舍入不同；先提升阈值精度再平方，拥有者与服务器使用一致判断。
- 新策略不能改变 Combo 零阈值契约；已按路由分开，并用明确配置窗口的测试场景验证。
- 专服脚本错误读取观察者完整 Spec，已改为按网络角色的实际复制契约读取。
- 暂停服务器蒙太奇会同步影响拥有者时间轴，不适合构造窗口拒绝；最终改用服务器局部输入约束，未保留暂停工具改动。
- 延迟脚本曾因预测退出后位置读为零而意外松键重按；最终输入保持状态独立于蒙太奇位置，验证持续输入确实被抑制。

已知构建警告包含原有 HUD UButton::IsFocusable 弃用访问及既有 IncludeOrderVersion 提示。原生测试的孤立 Enemy 场景有未配置行为树／GameplayCue 路径等既有诊断；不是正式 Guardian 资产错误。本轮未调整这些无关代码或升级引擎。

## 6. 交付边界

修复、配置入口、测试与文档已完成；本轮没有自动提交 Git，也没有覆盖既有未提交工作。编辑器恢复到原 ThirdPersonMap，性能参数恢复，PIE 停止并回到单人测试基线；未记录并恢复全部历史 PIE 偏好。

未执行 Cook／打包、独立 Server 可执行文件、手柄验证或人工手感评审；PIE Dedicated 通过不代表已实现独立 Server Target。没有声称代码行／分支覆盖率。
