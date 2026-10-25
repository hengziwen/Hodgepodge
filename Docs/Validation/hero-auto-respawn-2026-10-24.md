# Hero 自动重生验证 · 2026-10-24

## 交付结论与范围

已按[实施规范](../Design/hero-auto-respawn-2026-10-24.md)完成首版：正式 Hero 执行完整死亡流程，解除旧 Pawn/Avatar 后，服务器通过既有 GameMode 生成新 Pawn。额外等待 0 秒、总计最多 3 次生成尝试、失败间隔 0.5 秒，均保存在现有 PawnData。

PlayerState/ASC、等级、库存与所选物品身份保留；生命由属性协调器恢复最终满值，装备实际实例按旧清理、新重建。共鸣沿用已有死亡清零，暗流/暗涌沿用已有清理，冷却 GE 不重施或延长。没有新增模块、RPC、出生点管理器、倒计时 UI 或统一 GE 清空规则。

前置合并为 `e738342`；正式 PawnData 的 LFS 冲突文本修复为 `6d5b4c4`，见[合并报告](merge-dev-combat-2026-10-24.md)。自动重生源码、配置与工具已于 2026-10-25 按用户要求分批提交到 `main`；本报告保留当时实际执行的验证范围。

## 改动与评估对应

| 评估要求 | 本轮实际实现 |
| --- | --- |
| 复用 Lyra 成熟职责 | `UHodgeGameplayAbility_AutoRespawn` 复用 OnSpawn、SurvivesDeath、Avatar 重绑和配对清理；只移入服务器调度职责，Lyra 参考与差异见设计第 2 节 |
| 兼容现有架构 | PlayerState 基础 AbilitySet 仅授予一次；继续使用 PawnData、Experience、Init State、属性协调器与库存装备服务 |
| 完整死亡后生成 | DeathFinished 与 PawnExtension 反初始化/Avatar EndPlay 两个门槛；零延迟也排下一 Tick；不截断死亡 GA |
| 旧请求与重复回调 | 弱引用、单调代际、单 Timer；同步 Possess 触发新 Avatar 后，旧调用栈立即失效 |
| 有界失败 | GameMode `TryRestartPlayerForAutoRespawn` 只执行一次受检尝试，按当前 Controller 调用作用域抑制旧的下一帧重试；GA 决定次数和间隔 |
| 客户端一致性 | 当前新客户端身体清旧本地死亡标签；旧 Health 的迟到死亡通知不能给新 Avatar 写标签、清资源或发送死亡事件 |
| 配置与错误 | `ValidateAutoRespawn` 由 Runtime 和资产数据校验共用；启用时检查有限非负等待、正数间隔、至少一次尝试 |
| 性能与并发 | 新增职责由事件和 Timer 驱动；没有新增扫描 Tick。实际验证双玩家同时死亡，未据此声明人数上限或性能预算 |

## 实际修改的资产

- `/Game/Main/Data/AbilitySet/DA_Pover`：原 5 项普通 GA 后追加一项原生 AutoRespawn，原 Definition 授予不改。
- `/Game/Main/Data/PawnData/DA_Dafult_PawnData`：启用 `bEnableAutoRespawn`；`AutoRespawnDelay=0`、`AutoRespawnMaxAttempts=3`、`AutoRespawnRetryInterval=0.5`；保留合并后的库存与削韧配置。

修改前的原始包字节位于 `Saved/PlayerRespawn_20261024/BeforeAutoRespawn/`；只保存这两份目标资产。冷启动读回及正式 IsDataValid 均通过、无数据校验警告。`GA_Hero_Death.Duration` 仍为 2.5 秒。

2026-10-24 自动重生独立验收时的文件 SHA256：

| 文件 | SHA256 |
| --- | --- |
| DA_Dafult_PawnData.uasset | `e61a122f9b10dd1f72d1b26dc51e2b6e7aafa8940bd904b2ea5fa796d8ad3491` |
| DA_Pover.uasset | `8f0f40a776474914c425c3eb218ad7bf37cd1bed50ab373644f841aad0b8ab17` |

后续交互功能在保留重生能力的基础上扩展了共享 `DA_Pover`；该哈希对应重生配置提交中的已验证版本，当前交互接线与验证见[刷怪点记录](encounter-spawner-2026-10-25.md)。

Hero BP、原死亡 GA、用户原有 Guardian 装备 PawnData 与 HEAD 的包字节一致，未保存测试覆盖。Guardian 的原始保护 stash 与 Saved 备份保留；其内容已随 dev-Combat 合并，因此不再作为一份额外修改出现。

## 构建、原生测试与蓝图

实际运行以下常规构建，最终退出码均为 0。为避开本机 UBA 连续内存调度等待，使用本机 UBT 已支持的单任务参数，没有修改工程构建设置：

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
```

原生自动化实际命令：

```powershell
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/PlayerRespawn_20261024/NativeTests-Final' '-abslog=E:/Project/Git/Hodgepodge/Saved/PlayerRespawn_20261024/NativeTests-Final.log'
```

最终 **252 项全部成功**：139 项无警告，113 项带既有夹具/引擎警告；失败、未运行均为 0，进程退出码 0。以报告断言结果判定，不只检查退出码。

本轮新增 `Hodge.Respawn` 5 项：配置范围、死亡/解绑门槛和去重、跨 Avatar 旧回调退休、禁用配置不监听、客户端旧死亡标签与当前死亡保护。Role 注入的单元测试只证明本地代码分支；实际网络结果另见下节。

`BP_Hero_Pover` 与 `GA_Hero_Death` 通过项目蓝图桥实际编译，返回 `compiled=true`、`blueprint_status=3`（UpToDate）。未保存两份未改图表的蓝图；该结果不替代 PIE 或美术验收。

## 真实 PIE 集成结果

编辑器以本任务独立启动的 UE 5.5.4 进程执行，使用现有认证网关、正式 Experience 与 `/Game/Main/Maps/L_EnemyAIValidation`。敌人决策按现有启动夹具隔离。致死走实际 Damage GE/Health/Death GA，不手动 FinishDeath，不用人工 Restart 代替自动重生成功证据。

| 场景 | 实际断言 | 结果 |
| --- | ---: | --- |
| 单人 | 46 | 通过 |
| Listen，远端拥有者，0ms | 49 | 通过 |
| Listen，远端拥有者，PktLag=100ms | 49 | 通过 |
| PIE Dedicated，0ms | 52 | 通过 |
| PIE Dedicated，PktLag=100ms | 52 | 通过 |
| PIE Dedicated，100ms，双玩家同轮死亡 | 13 | 通过 |
| 单人真实出生碰撞失败/恢复/耗尽/取消 | 8 | 通过 |
| **合计** | **269** | **全部通过** |

基础五模式每项连续走三轮死亡：前两轮有武器，第三轮通过拥有者正式卸装 RPC 后保持空手。验证新 Pawn、同 Controller/PlayerState/ASC/核心属性集、生命代恰增一次、最终满血、等级与库存身份/定义/等级/数量/定义可初始化 StatTags、装备选择、实际装备及 Instigator、基础被动实例与句柄不重复，以及拥有者库存和新 Ready 收敛。

冷却检查先等待服务器真实建立正式 E 冷却，重生后核对**同一活动 GE 句柄、原服务器截止时间误差不超过 0.001 秒**，另检查拥有者 HUD 仍显示尚未到期的冷却。没有改变生产冷却规则或容差。

双玩家专项同时排入两份致死 GE，分别生成新身体并收敛到各端 Ready；模拟代理不可读取他人私有库存；两名新身体的拥有者均通过正式 E 输入在服务器建立了真实冷却。

故障专项只在 PIE 内存临时改出生碰撞策略并生成阻挡盒，结束/异常时恢复，不保存覆盖：

- 首次实际 Spawn 被拒绝，第二次成功，日志间隔约 **0.518 秒**。
- 持续阻挡时恰有 **1、2、3** 三次尝试；两次间隔约 **0.505、0.513 秒**；出现 `AutoRespawn exhausted` 后继续观察，无第 4 次或 GameMode 每帧重试。
- 解除阻挡后，既有外部 Restart 入口可恢复夹具；这一步只验证外部入口，不计作自动重生成功。
- 已绑定能力期间停用配置，完整死亡后不执行生成，旧请求不恢复。

最终报告与全部失败历史保留在 `Saved/PlayerRespawn_20261024/`，入口为 `final-summary.json`。每模式 JSON 保留断言和实际样本，原始日志包括 `RuntimeEditor.log`、`RuntimeEditor-BasicModes.log`、`RuntimeEditor-DeathTagBug.log`。Saved 为生成证据，不入源码库。

## 本轮发现与修复

1. 首两轮原生失败来自隔离测试夹具未显式准备 ASC Owner/核心属性集，及使用普通 Controller 换 Pawn 前未解绑旧 ASC；修正夹具，未放宽生产校验或忽略错误。
2. PIE 工具改用 UE 5.5 的公开 GAS 查询，避免读取受保护字段；冷却基线从未确认的客户端预测截止时间改为服务器已提交的 GE 句柄/截止时间。所有失败报告保留。
3. 双玩家新身体已满血 Ready，却因客户端 ASC 残留旧 `Status.Death` 无法施放 E。复用 Lyra 绑定清理并加入 Avatar 身份保护后，补原生回归，重新构建并重跑全部模式及双玩家输入通过。
4. 日志刷盘可能晚于同帧 Possess/Ready；故障脚本只增加有界的证据等待，所有尝试和失败日志均保留，没有跳过样本或放宽重试次数。

## 未验证项与后续建议

- 本次 NullRHI 验证不评价死亡动画、相机过渡和美术手感；建议动画/玩法负责人在正式渲染窗口做一次表现评审。
- 未做独立进程服务器、独立 Server Target、Cook/打包、Iris、丢包/断线重连、切图及多人压力容量测试；由网络/测试负责人另行安排。PIE Dedicated 不是独立专服构建。
- 本轮覆盖 E 执行中死亡及新身体 E 输入，未穷举所有攻击/冲刺/受击/击飞阶段的死亡组合；已有战斗原生回归全部复跑，动画/战斗专项可继续扩展。
- 普通敌人/Boss、原地救援、检查点、出生保护/特效、重生 UI 与统一 Buff 策略仍在首版范围外。后续玩法选择由项目负责人确认，不随本次实现自动展开。
- 出生点沿用现有引擎流程；发生持续配置或空间失败时按三次策略停止并记录，不能靠无限重试掩盖。程序负责人应按具体地图再评估是否需要 Lyra 出生点管理器。
