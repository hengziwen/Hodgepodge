# 双模式刷怪点验证记录

日期：2026-10-25。设计与配置见 [方案](../Design/encounter-spawner-2026-10-25.md)，工具入口见 [Tools/Encounter](../../Tools/Encounter/README.md)。

## 1. 范围与当前状态

已实现同一服务器刷怪实例的交互型／区域型入口、逐波生成、死亡清场、异常清理、独占领奖、重复触发门禁及首次区域就绪补触发。普通敌人继续使用已有 PawnData、EnemyInit、AI Controller、Combat 与 Equipment；奖励继续使用 PlayerState 库存。

五种模式的真实 PIE 共 381 条断言通过，包括交互底座 55 条、正式完整流程 276 条、死亡重生 50 条。最终全量原生报告为 281 项成功（143 项无警告、138 项带警告），0 失败、0 未执行；警告保留在报告中，没有通过吞掉 ensure 降低验收。

## 2. 已执行的实际 PIE

### 2.1 交互入口：55 条

单人、Listen 0／100ms、PIE Dedicated 0／100ms 各 11 条，均通过。验证真实 F 键 → IMC → InputAction → GAS → 拥有者组件 RPC → 服务器目标执行，包含长按只执行一次、松开再按使用新修订、目标销毁后拒绝及结果／修订复制。

证据：`Saved/Encounter_20261025/{single,listen0,listen100,dedicated0,dedicated100}/*-interaction-e2e.json`。目标为原生隔离测试 Actor，此项不能单独代替完整刷怪验收。

### 2.2 正式示例完整流程：276 条

- 单人：42 条。
- Listen 0／100ms：各 54 条。
- PIE Dedicated 0／100ms：各 63 条。

全部通过。使用 `/Game/Main/Maps/L_EncounterValidation`、两份正式示例 Definition 和未改战斗参数的装备 Guardian。

逐项核对：

1. F 键启动后开关隐藏，各端状态收敛。
2. 首批一只、第二批两只，全部由真实 EnemyInit 与 Brain 就绪。
3. 通过权威 Damage GameplayEffect → Health → DeathStarted 清场，剩余一只时不结算。
4. 清场后显示“领取奖励”；领取前不入库，成功后增加 ID_TestMaterial 一件，其他玩家库存不变。
5. 持续按键不重复领取或启动下一轮；成功后回到新轮次的未激活状态。
6. 进入实际 Box 自动启动区域；区域清场自动完成且不发奖励。
7. 正式配置的 30 秒冷却真实运行；持续站在区域内不重刷。
8. 全员离开只重新武装；新的进入才建立下一轮，再走完整两波。
9. 拥有者与模拟端的开关、中文 TextBlock 内容、完成状态及存活计数一致。

证据：`Saved/Encounter_20261025/run_e2e/<mode>/*-encounter-e2e.json`。

### 2.3 死亡与新 Pawn：50 条

同上五种模式各 10 条，均通过。待领取期间通过真实自伤 Damage GE 进入死亡，保留同轮 PlayerState 资格；沿正式死亡与自动重生生成新 Pawn，再用 F 键领取旧轮奖励一次。第二轮战斗中死亡则立即取消、主动清理本轮存活怪，不额外发奖；再次重生及旧回调不能恢复已取消轮次。

未修改重生等待、次数、生命、敌人参数或正式恢复时机。

证据：`Saved/Encounter_20261025/run_lifecycle_e2e/<mode>/*-encounter-lifecycle.json`。

## 3. 原生回归

最终结果已从 `Saved/Encounter_20261025/VerifiedNativeReport/index.json` 读回确认。现包含 7 组交互、18 组刷怪运行、4 组刷怪配置及其余既有回归，共 281 项。新增覆盖包括：

- 配置矩阵：模式、波次、敌人类、等级、时间、区域与奖励，以及休眠字段规则。
- 交互：旧 Avatar、距离、遮挡、轮次／修订、重复请求、非有限几何、PawnData 距离配置及 GAS 短命令。
- 波次：构造期同步死亡、首／中途生成失败、死亡后构造失败、初始化／Brain／超时、失效与死亡顺序、意外移除、旧轮次和主动回收。
- 结算：归属、真实库存满失败重试、重复领取与同步委托重入、死亡后新 Pawn 领取、断线回调。
- 区域：多组件去重、全员离开、冷却、新边沿、Exit 见证、一次性成功终态与失败重试。
- 首次补触发：初始重叠、未就绪恢复、恢复前离开、旧就绪回调、Exit 仅补进入见证。

原生夹具使用真实世界初始化与 BeginPlay，以便 Actor 动态 UFUNCTION 和物理重叠通知真实执行；未通过直接修改 Alive／Pending 或吞掉 ensure 制造成功。仅隔离外部 Experience 加载，并在冷却专项中模拟服务器截止时间到期；正式 30 秒另由 PIE 覆盖。

## 4. 构建与配置命令

已执行常规 Editor／Game 构建，关闭编辑器后进行反射布局变更编译：

```powershell
E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
```

最终构建日志为 `Saved/Encounter_20261025/VerifiedEditorBuild.log`、`VerifiedGameBuild.log`，两项目标均成功；既有构建警告保留，未顺手调整其他模块或插件。

全量原生回归命令如下；判定依据为报告中的成功／失败／未执行数量，而非仅检查进程退出码：

```powershell
E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/Encounter_20261025/VerifiedNativeReport' '-abslog=E:/Project/Git/Hodgepodge/Saved/Encounter_20261025/VerifiedNativeTests.log'
```

已运行并校验示例与输入配置：

```powershell
python Tools/HitReaction/mcp_execute.py Tools/Encounter/configure_interaction.py
python Tools/HitReaction/mcp_execute.py Tools/Encounter/configure_examples.py
python Tools/HitReaction/mcp_execute.py Tools/Encounter/validate_interaction_assets.py
python Tools/HitReaction/mcp_execute.py Tools/Encounter/validate_examples.py
python Tools/Encounter/run_interaction_batch.py
python Tools/Encounter/run_batch.py
python Tools/Encounter/run_batch.py --runner run_lifecycle_e2e.py
```

输入修改前的文件备份和引用记录位于 `Saved/Encounter_20261025/BeforeInteraction` 与 `interaction-setup.json`。新示例只创建两份 Definition 和独立关卡；原 Guardian PawnData 哈希保持不变。

最终冷启动后只读校验中，交互输入资产 11 条、示例配置 26 条均通过，分别见 `interaction-asset-validation.json`、`examples-validation.json`。其后重新执行五模式交互底座复核，55 条仍全部通过。

已只读核对正式 Experience 实际包含双端 Combat、敌人 Equipment、玩家 Inventory／EquipmentSelection 注入，证据为 `experience-components.json`；本任务未重存 Experience 或 Guardian。

## 5. 验证边界与后续建议

- PIE Dedicated 不等同于独立专用服务器 Target 或打包部署；本次未执行 cook／打包。
- 验证了真实组件显隐及 UMG 文本值，未进行人工逐帧视觉评审或录屏。
- 死亡清场使用真实伤害／死亡链，未把整轮敌人逐只按玩家攻击按键击杀作为本专项的前提；攻击与 AI 战斗细节沿用已有系统。
- 断线资格释放包含原生 EndPlay 回归，未进行物理断网／重连／丢包压力测试。
- 首次区域依赖恢复覆盖原生事件回归；联机示例覆盖正常真实进入，没有单独故意延迟 Experience 资源加载的 PIE 场景。
- 示例为每批 1／2 只，不代表大批生成性能通过。大量敌人的同步出生、动画、感知和行为树成本需另做项目规模的性能采样。
- 当前是世界内、单轮幂等奖励，没有跨存档账本或每参与者分别领奖；普通离开继续战斗，取消遵循既定服务器入口。

完整流程与生命周期批处理每项结束后恢复节流与播放设置；最后一轮交互底座复核结束后另执行 `restore_editor.py`，根目录 `Saved/Encounter_20261025/editor-restored.json` 确认恢复成功。编辑器已回到原 `/Game/CodexText/L_MainMenu`、单人 PIE、单进程和原性能设置；未覆盖脏资产。原有用户改动保留，源码、资产、工具与文档已按用户要求分批提交；Saved 中的日志与生成证据不纳入源码库。本次提交整理未重新执行构建或玩法测试，不增加新的通过结论。
