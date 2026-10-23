# 战斗复审后续修复与验证记录

日期：2026-10-18；基线：`dev-Combat / 891fd70` 加开始时未提交工作区；UE 5.5.4。

依据：[复审与跟进清单](../Design/combat-refactor-follow-up-review-2026-10-18.md)。本文仅记录本次实施，不覆盖[上一整改批次](combat-review-refactor-2026-10-18.md)的原始结果。

## 1. 本轮范围及用户确认

- 用户授权按复审文档实施修复及验证，不添加新战斗技术或功能，不改变现有伤害公式、分段击飞、网络身份或正式素材。
- **RV04 已确认：**完整资产检查继续报告全局违规，运行时只约束本次实际执行的阶段。未使用的落地／起身配置不阻断地面受击；普通削韧实际使用的阶段仍禁止动画根运动。
- **拆类继续暂缓：**用户明确选择“本轮只完成修复，拆类继续暂缓”。不创建 PlanBuilder、接触表现宿主或角色会话类，不实施 RV08～RV10 的职责迁移。
- 正式五段终结刀漏判、Client 共鸣回路前普攻窗口异常继续按已有决定暂缓；不以其他回归通过关闭问题，不调整正式位移、通知、数值或素材。
- 保持单 Runtime、同类主 `.cpp`、玩家 PlayerState ASC 与怪物现有 ASC 所有权；不自动提交或切分支。

## 2. 实现内容

| 复审项 | 本轮实现 | 验证边界 |
| --- | --- | --- |
| RV01 | Direction TargetData 先复制为本次稳定 Handle，再消费 GAS 缓存；保留原身份和数据合法性检查 | 新增先到／后到真实 GAS 委托、真实 Montage 实例、重复交付和取消后迟到回归；非独立网络传输测试 |
| RV02 | 旧布局真实落地回退到 Knockdown 时，检查其主片必需性和明确无动画许可 | 独立 Landing 可替代未使用的 Knockdown 主片；旧无动画兼容许可保留 |
| RV03 | 空中换片携带实际 AirHit 选择；实际 Knockdown 落地主片独立检查；只有明确续播分支才清除未播放的新变体 | 覆盖 AirHit／Landing 歧义拒绝、合法跨阶段和旧片续播，不修改状态复制格式 |
| RV04 | 普通削韧只检查实际主阶段及会使用的落地／起身运动来源；未来倒地按运行时原有 Knockdown 回退读取 | PawnData 的完整资产政策保留；没有起身动画时不把未执行的起身运动标记当成实际运动 |
| RV05 | 观察者断言排除服务器和受击拥有者并核对 PlayerId；各端最终检查受控、动画根运动、RMS 和播放过的受击 Montage 残留 | Listen 双人报告主机观察；Dedicated 报告独立观察客户端，不虚构第三个 Listen 客户端 |
| RV06 | 新增真实服务器等待 Timer 的开窗、超时、来源取消及重复请求用例；复用现有恢复／死亡／解绑原生回归 | 服务器请求通过测试入口注入，不声称是真实 RPC；完整玩家生命周期网络矩阵仍未完成 |
| RV07 | BuildPlan 提前失败包含 Owner、Profile、ExecutionId、阶段和原因；沿用既有结果枚举 | 不添加重试、RPC 或另一套状态；非法测试产生的预期告警单独计数 |

### 2.1 文件及接口

- `Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.cpp`：仅调整回调的数据持有和读取。
- `Source/Hodgepodge/Private/Component/HodgeHitReactionComponent.cpp`、`Private/Data/HodgeHitReactionProfile.cpp`：当前路径校验、真实选择上下文和诊断；不改变公开反射数据或资产格式。
- `Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h`：仅为现有测试 Access 增加友元，不增加业务接口；保留原 UTF-16 编码和 BOM。
- `Source/Hodgepodge/Private/Tests/HodgeAttackWarpTests.cpp`、`HodgeHitReactionTests.cpp`、`HodgeCombatActionRequestTests.cpp`：新增风险回归。
- `Source/Hodgepodge/Private/Tests/HodgeAttackWarpTestTypes.h/.cpp`：朝向夹具隔离近战来源准备，数据处理、朝向和实际播放继续走 Definition；不将其结果当成完整近战验证。
- `Tools/ACTCombat/validate_poise_runtime.py`：加强身份及逐端清理断言，补实际播放资源路径和释放快照。
- `Tools/ACTCombat/tests/test_runtime_assertions.py`：两项离线失败注入测试，验证服务器不能冒充观察者、残留控制／运动／动画不能通过释放断言；不启动编辑器。

完整原文件备份和开始状态在 `Saved/CombatReviewFollowUp/20261018/Before`、`status-before.txt`。备份仅覆盖本轮涉及文件，不是整个工作区快照；生成目录不纳入源码提交。

### 2.2 Lyra 与引擎依据

- 本机 `LyraGameplayAbility_RangedWeapon::OnTargetDataReadyCallback` 先取得本地数据所有权，处理后消费缓存；Hodge 使用稳定 Handle 副本适配现有单条攻击朝向数据。
- UE 5.5.4 `AbilitySystemComponent_Abilities.cpp`：`CallReplicatedTargetDataDelegatesIfSet` 按引用广播缓存；`ConsumeClientReplicatedTargetData` 清空缓存 Handle。两者构成 RV01 的实际失效链。
- 沿用现有 `GameplayAbilities` 和 Hodge 调用链，不迁入 Lyra 武器、弹道或命中标记，不添加模块、插件或复制字段。

## 3. 修复前回归

先新增四项原生测试，再对尚未修复的生产代码执行常规 Editor 构建及 `Hodge.*` 自动化。

- Editor 构建成功，115.20s。
- 185 项：114 Success、67 SuccessWithWarnings、4 Failed、0 NotRun。
- 失败为 `Hodge.AttackWarp.TargetDataDeliveryOrder`、`Hodge.HitReaction.LandingFallbackContract`、`RelevantMotionPolicy`、`SelectedPhaseContext`，与复审发现对应。
- 方向数据先到场景实际出现朝向拒绝、目标角度未应用、主 Montage 未启动。此后朝向夹具补充隔离近战来源准备，避免无装备依赖干扰有效数据处理后的播放验证。
- 该阶段测试集尚未包含后补的服务器 Timer 用例及额外 Landing 歧义断言，不能混称与最终测试集完全相同。

证据：`Saved/CombatReviewFollowUp/20261018/Automation-BeforeFix/index.json` 及相邻日志。命令退出码为 0，但报告有四项失败；以实际断言为准。

## 4. 修复后验证

### 4.1 常规构建与原生回归

- 首轮修复后 Editor／Game 构建成功；185 项原生为 113 Success、72 SuccessWithWarnings、0 Failed、0 NotRun。之后补实际 Landing 变体检查和服务器等待 Timer 用例，以最终结果为准。
- 新服务器测试首次编译因直接对 `FNativeGameplayTag` 调用 `GetTagName` 而失败，日志 `Editor-Complete.log`；改为先取得 `FGameplayTag` 并补节点检查，未修改玩法逻辑。
- **最终 Editor 构建成功，26.57s；Game 构建成功，175.74s。** 日志：`Editor-Verified.log`、`Game-Verified.log`。保留已有 include order 和 CodexText 的 UButton 弃用警告，没有修改构建设置回避告警。
- **最终 `Hodge.*` 原生：186 项，113 Success、73 SuccessWithWarnings、0 Failed、0 NotRun。** 报告：`Automation-Complete/index.json`。新增服务器等待测试穿过实际 Timer，覆盖开窗、超时、来源取消和重复请求；属于服务器处理注入，不冒充真实 RPC。
- 两项 Python 离线失败注入通过；脚本 AST 语法检查通过。没有仅根据 Unreal 进程退出码判断通过。

### 4.2 正式资源与实际冒烟

本轮最终 Runtime 代码完成构建及原生回归后，用独占隐藏编辑器冷启动执行以下专项，全部通过；没有复用上一批次报告。

| 场景 | 实际结果 | 证据（相对本轮 Saved 目录） |
| --- | --- | --- |
| 正式 Hero／Guardian／Dummy HitReactionProfile | 3 份资产完整校验有效，零错误、零警告 | `Smoke/batch.json` 的 assets |
| 正式 R 输入 | 9 项通过，真实伤害、Fly Start／Loop／Fall、落地、起身及释放 | `Smoke/r-input-smoke.json` |
| 真实玩家受击，Listen 0ms | 4 场景、50 检查通过 | `Smoke/runtime-listen-0ms.json` |
| 真实玩家受击，Listen 100ms | 4 场景、50 检查通过 | `Smoke/runtime-listen-100ms.json` |
| 真实玩家受击，同进程 Dedicated 0ms | 4 场景、58 检查通过 | `Smoke/runtime-dedicated-0ms.json` |
| 真实玩家受击，同进程 Dedicated 100ms | 4 场景、58 检查通过 | `Smoke/runtime-dedicated-100ms.json` |

玩家专项合计 **16 场景、216 检查通过**，覆盖轻／重硬直、分段允许续飞及禁止续飞后的落地起身。每个实际参与端均有独立释放快照，检查受控、动画根运动、RMS 和受击 Montage 实例残留；拥有者必须匹配目标 PlayerId，Dedicated 观察客户端必须排除服务器及受击拥有者。Listen 单列主机观察，不冒充独立远端观察客户端。

`Smoke/batch.json` 五个子集 error 均为空、failures 为空，编辑器自行退出码 0。100ms 仅为 `Net PktLag=100` 配置，不是实测 RTT；同进程 PIE Dedicated 不等于独立专服。没有重新编译蓝图、做额外视觉验收或恢复两项暂缓故障。

### 4.3 工作区与资产保护

- 相关正式资源及探针共 **196 份 uasset**，冒烟前后 SHA-256 全部一致；证据为 `assets-before-smoke.json`、`assets-after-smoke.json`。不将此范围扩大为全仓所有二进制资产检查。
- 初始 85 条工作区状态均保留，无原有状态路径丢失；结束时 91 条，增加项属于本轮源码测试入口和验证文档等修改。
- 现有文件的任务内前后哈希见 `file-manifest.json`；另新增 Python 失败注入测试和本文。不回退用户原有修改，不提交、不切分支。

## 5. 实际命令与证据目录

构建参数沿用项目约定；日志输出到 `Saved/CombatReviewFollowUp/20261018/`，不用 Live Coding 替代常规构建。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash -nosound '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/CombatReviewFollowUp/20261018/Automation-Complete' '-abslog=E:/Project/Git/Hodgepodge/Saved/CombatReviewFollowUp/20261018/Automation-Complete.log'
& 'E:/Python/python.exe' -B -m unittest discover -s Tools/ACTCombat/tests -v
$taskSmokeEditor = Start-Process -FilePath 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor.exe' -ArgumentList @('E:/Project/Git/Hodgepodge/Hodgepodge.uproject','-unattended','-nosplash','-nosound','-windowed','-ResX=1280','-ResY=720','-ExecutePythonScript=E:/Project/Git/Hodgepodge/Saved/CombatReviewFollowUp/20261018/run_smoke.py','-abslog=E:/Project/Git/Hodgepodge/Saved/CombatReviewFollowUp/20261018/Smoke.log') -WindowStyle Hidden -PassThru
$taskSmokeEditor.WaitForExit()
```

正式受影响冒烟使用本轮独占隐藏编辑器运行；它不执行暂缓用例，不保存正式资源。实际场景结果及证据见第 4 节。

## 6. 保留与未完成项

- RV01～RV05、RV07 本轮定点修复及相应回归已完成；RV06 的服务器调度覆盖得到补充，但不关闭整个生命周期网络矩阵。
- RV08～RV10 拆类继续暂缓；未创建新业务类或改变既有职责宿主。
- RV11 两项故障继续保留，未恢复排查；本轮子集通过不代表正式五段闭环通过。
- RV06 完整玩家恢复输入、受击中死亡、换 Pawn／重生／解绑的网络矩阵仍需随获批迁移补齐；服务器注入不能替代网络传输验证。
- RV12 未删除字段、迁移旧资产或清理备份；引用台账尚未完成。
- F11 未进行 CPU／同负载性能测量，不新增缓存或改变 Tick／采样密度。
- 未执行独立进程专服、Cook／打包、正式素材制作或主观手感／视觉验收。
