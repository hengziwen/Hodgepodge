# 刷怪点配置与验证工具

实现规则与制作入口见 [双模式刷怪点方案](../../Docs/Design/encounter-spawner-2026-10-25.md)，实际结果和未验证范围见 [验证记录](../../Docs/Validation/encounter-spawner-2026-10-25.md)。

## F 键交互接线

前置条件：完成本轮常规 Editor 构建并重新打开 Hodgepodge；无 PIE、无未保存关卡或内容资产。脚本不处理用户的脏资产，也不批量保存其他资产。

```powershell
python Tools/HitReaction/mcp_execute.py Tools/Encounter/configure_interaction.py
```

脚本检查并备份现有目标文件，记录引用与哈希到 `Saved/Encounter_20261025/BeforeInteraction` 和 `interaction-setup.json`。保留已有条目，只增补以下接线：

- 新建 `/Game/Main/Input/InputAction/IA_Interact`，Boolean，Pressed 触发器；持续按住不跨状态连发。
- `/Game/Main/Input/IMC_Default`：将 F 绑定 IA_Interact，其他映射保留。
- `/Game/Main/Input/DA_HodgeInputConfig`：将该动作映射到已有 `InputTag.Ability.Interact`。
- `/Game/Main/Data/AbilitySet/DA_Pover`：授予原生 `HodgeGameplayAbility_Interact`，等级 1，使用同一输入标签；保留重生等已有能力。

F 被其他动作占用、标签重复、已有交互配置与预期不符时停止，不覆盖手工配置。失败报告保留错误；操作超时后应先检查资产与报告，不直接重跑写操作。

交互距离统一放在 `/Game/Main/Data/PawnData/DA_Dafult_PawnData` 的 `InteractionDistance`，默认 250cm。查询、服务器组件和目标最终复核读取同一值；不修改原生 Controller 的临时实例默认值。

## 两类示例

```powershell
python Tools/HitReaction/mcp_execute.py Tools/Encounter/configure_examples.py
python Tools/HitReaction/mcp_execute.py Tools/Encounter/validate_interaction_assets.py
python Tools/HitReaction/mcp_execute.py Tools/Encounter/validate_examples.py
```

首次创建以下资产；已有同名制作配置只校验，不覆盖：

- `/Game/Main/Data/Encounters/DA_Encounter_Interaction`：两波装备 Guardian（1／2只），ID_TestMaterial ×1。
- `/Game/Main/Data/Encounters/DA_Encounter_Area`：同样两波，Enter，重复，冷却 30 秒，无领奖。
- `/Game/Main/Maps/L_EncounterValidation`：独立地面、导航、玩家出生点及两个刷怪实例。

在该关卡 PIE 后，出生点前方的开关用 F 启动／领奖；区域实例位于 `(1500,0,100)`，Box 半尺寸 `(300,300,150)`。完成区域后必须冷却并离开，再重新进入才重刷。地图与既有 Guardian／Experience 均分开保存，原战斗参数不改。

## 自动化

```powershell
python Tools/Encounter/run_interaction_batch.py
python Tools/Encounter/run_batch.py
python Tools/Encounter/run_batch.py --runner run_lifecycle_e2e.py
```

默认覆盖单人、Listen 0／100ms、PIE Dedicated 0／100ms；也可在命令末尾指定 `single`、`listen0`、`listen100`、`dedicated0`、`dedicated100`。每项实际断言、错误和设置恢复记录均在 `Saved/Encounter_20261025`。

`run_batch.py` 不论成功失败均结束 PIE 并恢复已记录的性能／播放设置。若单独运行交互底座工具或手动启动测试，结束后可执行 `restore_editor.py`；它等待 PIE 退出，拒绝覆盖脏资产，默认保留当前地图。原始交互脚本验证输入底座，完整两种流程以 `run_batch.py` 为准。

原生回归使用 `Automation RunTests Hodge.`；须读取 JSON 报告的 failed／notRun，不能仅凭进程退出码判断通过。配置脚本通过不代替 PIE；PIE Dedicated 也不代表独立专服打包通过。
