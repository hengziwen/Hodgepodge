# 玩家锁定实现与验收 · 2026-10-12

依据[锁定系统 v0.3](../Design/lock-on-system.md)，覆盖 LOCK-01～09。UE 5.5.4，业务仍属于 Hodgepodge Runtime；编辑器作者工具属于现有 HodgeAbilityEditor。没有提交 Git、回退用户改动、升级引擎或修改 Guardian 已修复的行走／武器插槽。

## 1. 完成结论与交付边界

**首版实现与列明的自动化／PIE 验收已完成。** 正式 Hero 已接入中键锁定、甩鼠标切换、目标保持、预测确认、锁定朝向与镜头、HUD 标记、Dash／Sprint 暂让和恢复。正式资产已保存，不需要用户再补不存在的字段或重复授予 GA。

测试通过的范围是下列具体场景，不代表所有设计验收场景已覆盖。手感／美术、复杂场景和发布验证保持待办；没有代码行／分支覆盖率插桩，不能从测试数量推导覆盖率百分比。

[机器可读结果](lock-on-implementation-2026-10-12-results.json)包含检查名、结果、源码清单、源码和原始报告 SHA256。原始日志／帧数据在 `Saved/LockOn`，不提交生成缓存。

## 2. 已实现功能清单

1. **输入与选择（LOCK-01）：已完成。** 中键 Started 获取／解除，松开保留；鼠标共用现有视角灵敏度后的语义入口，大幅快速水平甩动按屏幕左右切换。筛选距离、视角、目标接口、生命、阵营和视线；评分与并列排序稳定，切换无候选时保留原目标。
2. **保持与失效（LOCK-02）：已完成。** 获取／保持半径分离，10Hz 有效性检查，超距／遮挡独立宽限。死亡、销毁、锚点失效和生命周期清理解除；普通受击不清选择，无自动重锁。无目标时不持续扫描全部候选。
3. **分工与初始化（LOCK-03/07）：已完成。** Hero 原生组件，PawnData Profile，可选 Init State Feature；先显式进入 Spawned，DataAvailable 只等待 PawnData，防止与 PawnExtension ASC 屏障互锁。ASC／Avatar 绑定、状态推进、重复回调和清理有幂等／防重入处理。没有 Profile 不启用锁定，也不阻塞通用 Pawn 初始化。
4. **GAS 网络命令（LOCK-06）：已完成。** 短 LocalPredicted GA 使用 GAS TargetData；显式 Acquire／Switch／Clear、当前 Avatar、序号和基础版本。服务器校验，确认状态复制并向拥有者回执；拒绝保留原确认目标。早到数据、缓存消费、旧 Avatar／退役激活、迟到结果、待解析引用及有限超时均有处理。组件不另建上行命令 RPC。
5. **持续状态所有权（LOCK-03/07）：已完成。** 服务器确认时通过无限 GE 授予 State.Targeting.LockedOn，组件只撤销自己的句柄；切换不叠加，短 GA 或 Dash 结束不误清标签。同一 ASC 只允许一份命令 GA，支持蓝图子类，重复授予明确拒绝。
6. **旋转／移动（LOCK-04）：已完成。** 普通锁定消费 Controller＋ReservedStrafe；Dash 临时 Movement＋ActionDirection，Sprint 使用 Movement。保留目标／镜头，动作结束恢复锁定朝向，旧句柄／快照不复活已清除选择。模拟代理消费权威快照，不自行申请本地锁定。
7. **镜头（LOCK-05）：已完成。** 角色 ControlRotation 与相机实际 Rotation 分离；PlayerController 更新与 CMC 消费统一目标朝向，锁定时相机不二次写回。锁定 CameraMode 复用当前第三人称配置和避障；技能临时镜头优先，构图／Yaw／Pitch／速度可调。
8. **HUD（LOCK-05）：已完成原型。** Controller 原生非复制 IndicatorManager，既有 HUD 接全屏 IndicatorLayer。所属玩家显示目标胸口菱形，死亡／清除移除，无 Hit Test；正式美术可以用 Profile 软类替换。
9. **攻击边界（LOCK-08）：代码接入完成。** Definition 起手读取并验证确认目标，保存弱引用执行快照；后续切换不改当前攻击快照。没有自动开启吸附、强制位移、命中或扣血，继续使用原几何检测与伤害规则。真实刀次切换／范围外攻击专项场景未在本轮 E2E 执行。
10. **诊断与工具（LOCK-09）：已完成基础。** 可查询确认版本、表现修订、待确认状态和原因；Verbose 日志记录候选评分／拒绝。提供资产制作、检查、四模式 PIE、恢复和报告导出工具。独立调试面板未制作。

## 3. 文件与正式资产

新增 16 个源文件、修改 24 个源文件，相对于本任务开始的 `Saved/LockOn/BeforeSource`，并非把工作区全部历史改动算成本轮。

- 核心：[组件](../../Source/Hodgepodge/Private/Component/HodgeLockOnComponent.cpp)、[命令 GA](../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.cpp)、[协议类型](../../Source/Hodgepodge/Public/Targeting/HodgeLockOnTypes.h)、[Profile](../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)、[目标接口](../../Source/Hodgepodge/Public/Interface/HodgeLockOnTargetInterface.h)、[状态 GE](../../Source/Hodgepodge/Private/AbilitySystem/HodgeLockOnStateEffect.cpp)。
- 消费者：[旋转](../../Source/Hodgepodge/Private/Component/HodgeCharacterRotationComponent.cpp)、[CMC](../../Source/Hodgepodge/Private/Component/HodgeCharacterMovementComponent.cpp)、[Controller](../../Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp)、[镜头](../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_LockOn.cpp)、[标记](../../Source/Hodgepodge/Private/UI/Targeting/HodgeLockOnIndicatorWidget.cpp)、Hero 输入、Dash 与 Definition。
- 测试：[9 项锁定测试](../../Source/Hodgepodge/Private/Tests/HodgeLockOnTests.cpp)、[PIE 工具](../../Tools/LockOn/README.md)。完整增改清单在结果 JSON 的 source_delta。
- 新资产：`/Game/Main/Character/Hero/Targeting/DA_HeroLockOn`、`AS_HeroLockOn`，`/Game/Main/Input/InputAction/IA_LockOn`。
- 既有配置：`DA_Dafult_PawnData.LockOnProfile/AbilitySets`、`DA_HodgeInputConfig.AbilityInputActions`、`IMC_Default.MiddleMouseButton`、`WBP_HodgeHUD.LockOnIndicatorLayer`。Hero／Guardian／AnimBP 已执行编译，Guardian 实际胸口骨骼 `b_MF_Spine_03`。
- `L_EnemyAIValidation` 保存反射属性传播，测试位置／暂停 AI 仅作用于 PIE 实例。受影响资产备份在 `Saved/LockOn/BeforeAssets`；Hero 蓝图编译保存未另建该目录备份，其本任务初始状态无用户改动。

具体编辑字段与使用方法见[配置手册](../Guides/lock-on-configuration.md)。

## 4. 实际测试结果与需求追溯

- **Editor 常规构建：通过，退出 0。** 最终约 44.54 秒。
- **Game 常规构建：通过，退出 0。** 最终约 40.65 秒。
- **原生综合回归：29/29 Success。** LockOn 9、Facing 8、Rotation 1、Movement 8、UI 3；按 tests 数组逐条核对。报告含既有调试／夹具警告，没有失败。退出 0 本身不作为测试成功依据。
- **资产检查：10/10。** 正式 Profile、唯一授予／输入、中键真实键值、标记与两个真实锚点。
- **单人 PIE：20/20。** 最终构建后冷启动再次执行，23:48 完成。
- **Listen Server：20/20；专服 PIE：20/20；Listen Server＋100ms 延迟：20/20。** 分别 23:23、23:25、23:31 完成。之后仅增加 Verbose 候选日志和超时边界原因修正；最终原生回归覆盖超时修正，最终单人覆盖正常运行路径，多人没有再重复全套。
- **截图：已检查所属玩家 Slate 捕获。** [单人 HUD](LockOn/single-hud.png)显示实际胸口标记，保留底部血条／右上终焉布局。不是服务器 viewport 截图。

9 项原生测试与需求的对应：

- MouseGesture → LOCK-01：轻微输入、阈值、方向一致性、防连发与重新武装。
- ProfileValidation → LOCK-01/02/05：有限值、半径／时间／资源配置。
- AuthorityIdentityAndVersions → LOCK-01/06：资格、非法来源、基础版本、重复序号和确认拒绝。
- PredictionReconciliation → LOCK-06：新旧回执、拒绝恢复、重复表现通知、Timer 截止先于 Tick 的 Timeout 分类。
- ControlAndMotionLifecycle → LOCK-03/04/05/07：朝向占用、动作暂让、恢复与自身资源清理。
- RangeGraceAndInvalidTarget → LOCK-02：保持半径、宽限和失效。
- GASShortCommand → LOCK-03/06：真实 GAS 命令激活／结束和持续选择。
- DeathClearsAndDoesNotReacquire → LOCK-02/07：死亡清理、不选邻居。
- TargetDataEarlyAndRetired → LOCK-06：早到 TargetData 缓存消费、退役激活数据拒绝。

每种 PIE 20 项覆盖：各端初始化、中键获取／松开／解除／再次获取、轻微鼠标不切换、左右甩动、HUD 实例、普通侧移、Dash 暂让／恢复、长按 Sprint／保留／恢复，以及真实 GameplayEvent.Death→死亡 GA 后各端清除且不重锁。

中键经 PlayerController::InputKey 下一游戏帧进入真实 IMC／Started／Hero／GA；其他动作经 EnhancedInput 注入。没有验证桌面物理鼠标、操作系统事件与 DPI。夹具暂停敌人决策，不能将本轮报告理解为怪物主动攻击回归。

## 5. 已发现并修复的问题

1. 状态 GE 构造期动态添加组件导致启动失败：改为原生默认子对象。
2. IndicatorLayer 未导出导致 Editor 链接失败：补 API 导出。
3. ASC 等待形成 Init State 环：DataAvailable 不等 ASC，显式进入 Spawned，并有限补偿重入检查。
4. 项目实际没有 IndicatorManager：在当前非 Modular Receiver 的 Controller 创建默认组件。
5. 中键 Triggered 每帧切换：改 Started；FKey 文本误解析为 `(`：正确导入 `MiddleMouseButton` 并增加真实 InputKey 验收。
6. 持续甩动重新武装时重复释放旧输入：清缓存并返回，避免镜头尾跳。
7. Game 非 Unity 编译发现 OverlapResult 不完整类型：补明确 include。
8. 旧回执／旧旋转确认可能复活已解除选择：按请求身份及锁定来源隔离。
9. 命令 Timer 在组件 Tick 之前取消被误记 ActivationFailed：根据截止时间分类 Timeout，并新增边界断言。
10. 验证工具持有地图 Package Python 引用引发 World Memory Leaks：释放 UObject 引用、收集 Python GC、避免冗余 reload；地图恢复已实际成功。未修改插件去掩盖错误。
11. 测试 Sprint 路线撞敌人、延迟模式夹具位置未复制、viewport 抓错窗口：分别隔离路线、等待夹具复制、改所属 Slate 捕获。这些是测试夹具问题，没有绕过生产规则。

既有 `HodgeSurvivorHUD` 弃用 API 警告、测试夹具无 PlayerState 等调试输出仍保留，不作为本功能已修复项。

最终差异检查通过；任务文档本地链接有效，工具 Python 语法检查通过，新增／修改部分未出现 4 行以上连续 `//` 注释块。工作区开始 437 条、结束检查 464 条状态记录，包含大量此前未提交工作；本次没有自动提交、回退或清理这些内容。计数包含 Git 按目录汇总的未跟踪条目，不等于文件总数。

## 6. 临时、原型与固定策略清单

以下覆盖本任务引入的临时选择，不是扫描整个历史项目全部占位实现。

- **P1，原型标记美术**：`Private/UI/Targeting/HodgeLockOnIndicatorWidget.cpp:8`，36×36 白色 `◇`、Bold26。改正式 Widget Blueprint 与预测／切换视觉反馈；Profile.IndicatorClass 已支持替换。
- **P1，鼠标延迟权衡**：`Private/Component/HodgeLockOnComponent.cpp:354/451`，横向输入缓存 GestureWindow 默认 0.12 秒。后续低延迟手势分类、不同 FPS／DPI／灵敏度实测；不能宣称当前普通构图输入零延迟。
- **P1，可编辑原型参数**：`Public/Data/HodgeLockOnProfile.h:15`，半径、角度、评分、宽限、超时和镜头速率。已放正式 DataAsset，后续按场景调优，不能因自动化通过认定最终手感。
- **P2，默认镜头适配限制**：`Private/Camera/HodgeCameraMode_LockOn.cpp:13`，只复制 ThirdPerson 派生默认镜头。当前 Hero 适配成立；未来不同相机需增加配置适配契约。
- **P2，单锚点策略**：`Private/Character/HodgeEnemyCharacter.cpp` 的 GetLockPoint，首版 NAME_None；可选 Bounds 中心回退。Guardian 已配置真实胸口骨骼，没有假世界原点。Boss 多部位另行设计。
- **P2，查询与切换固定策略**：组件 `:178/192`，候选 Pawn＋WorldDynamic，左右至少 1 屏幕像素。增加新目标类型／分辨率需求时考虑可配通道和归一化间隔。
- **P2，HUD 作者布局约定**：`HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.cpp:1383`，ReferenceHUDCanvas／LockOnIndicatorLayer／Z=-1。当前 HUD 有效；其他布局需要明确作者工具适配，运行时不扫描这些名字创建 UI。
- **P2，作者脚本已知资源**：`Tools/LockOn/configure_assets.py`、`configure_enemy_anchors.py`、`repair_key_mapping.py`，正式资源路径和胸口骨骼候选。扩展英雄后参数化，路径未写入运行时目标选择逻辑。
- **验证夹具，非生产待补实现**：`Private/Tests/HodgeLockOnTests.cpp` 临时世界／ASC／生命与 pending 注入、作用域脚本许可；`Tools/LockOn/start_test.py/run_e2e.py` 坐标、动作幅值、超时与 100ms 延迟；`export_results.py` 固定本轮日期／预期计数。后续 P2 抽取共享夹具，不在游戏中启用。
- **技术上限，非业务占位**：组件 `:67/266` 的 4 次补偿检查、名称长度／序号上限；ASC `:1248` 的 64 条退役身份缓存。保留有界保护，P2 根据压力采样检查容量，不应为了去掉“硬编码”而删除校验。

不存在伪造服务端授权、绕过命中扣血或空实现返回成功的临时代码。

## 7. 尚未完成与后续优先级

**P1，待验证／优化，约 1～3 人日：** 人工鼠标手感、低延迟构图、全八向动画、双方运动／近距离／高度／复杂遮挡、技能临时镜头、真实攻击快照切换与锁定范围外不吸附。资源为正式 Hero、Guardian 和相机／动画场景；验收是无重复输入、无朝向抢写／恢复跳变、真实攻击不因锁定自动命中。当前基线先保留，发现具体问题再修改。

**P1，生命周期与弱网专项，约 2～4 人日：** 真实重生／换 Pawn、GameFeature 撤销、目标引用延迟、丢包／抖动、网络相关性重入。原生校验存在，不等于这些联机场景全部执行；验收无旧回调、旧目标复活、泄漏句柄或无限 pending。

**P1，发布验证，约 3～7 人日：** Cook／打包、独立进程多客户端，按发布需求决定 Server Target；当前只有 Game／Editor Target。专服 PIE 通过不能替代独立专服。引擎及插件不自动升级。

**P2，未实现且不属于本版必需：** 正式标记美术、独立候选调试面板、其他默认镜头适配、Boss 多锁点和复杂标记 GA。各需求确定后拆独立验收。

**用户明确暂缓：** 手柄、自动攻击吸附／自动重锁、体力恢复、库存编码。库存目前仅设计。它们不能标为本次失败，也不能借“全部继续”擅自反转既有约束。

更广的项目待办见[最新状态](../KnowledgeBase/29-current-work-and-backlog.md)。没有指定整体截止日；本阶段以首版代码、资产、测试与报告交付为节点，估算不是承诺完成时间。

## 8. 实际命令与编辑器恢复

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.LockOn+Hodge.Facing+Hodge.Rotation+Hodge.Movement+Hodge.UI' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/LockOn/Regression' '-abslog=E:/Project/Git/Hodgepodge/Saved/LockOn/regression.log'
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/LockOn/validate_assets.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/LockOn/start_test.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOn/start_listen.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOn/start_dedicated.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOn/start_lag.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/LockOn/stop_test.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/LockOn/restore_editor.py
& E:/Python/python.exe Tools/LockOn/export_results.py
git diff --check -- Source Docs Tools
```

外部构建前关闭已保存编辑器；反射变化后冷启动，不以 Live Coding 代替。多模式工具的可复跑入口见 Tools/LockOn/README.md。最后恢复 `/Game/CodexText/L_MainMenu` 和记录的性能配置；PIE=0，无 dirty map/content。最初未记录全部播放设置，明确恢复单人 Standalone 基线，没有声称完整还原原播放选项。已有 UI 截图已还原，未保存包不会被恢复脚本覆盖。
