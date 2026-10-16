# 高血量受击假人：使用与观察

日期：2026-10-16；UE 5.5.4。当前系统细节与已知问题见[实现记录](../Design/hit-reaction-implementation-audit-2026-10-16.md)。本工具不优化受击表现，只提供可重复测试目标。

## 1. 打开即可测试

1. 在内容浏览器打开 `/Game/Tests/Combat/L_HitReactionDummy`。
2. 首次先在 Play 下拉选择 Play Net Mode=Play Standalone、Number of Players=1，再点击 Play，等待正式 Hero 与假人完成 Experience/ASC 初始化；本轮结束已恢复你原有的 Client/2 玩家偏好。
3. 假人标签为 `HitReactionDummy_1000000HP`，在出生点前方；移动到正常武器可触及距离，用正式普攻、E 等技能攻击。
4. 假人资源：`/Game/CodexText/HitReactionValidation/Dummy/BP_HitReactionDummy`。也可以把该蓝图拖入已有支持默认 Experience 的地图；观察工具要求服务器世界恰有一个同类假人。

假人使用当前 Hero 骨架/动画的完整受击 Profile，具有 Pawn 自有 ASC 和受击 GA。它不追击、不反击，但保留正常 CharacterMovement 和碰撞。最大生命与出生生命均为 **1,000,000**，不会自动回血；重新开始 PIE 恢复满血。

该数值下 float 步长约 0.0625；当前测试攻击 3.125 的扣血可准确观察，若研究更小的微量伤害应降低测试血量。血条百分比下降很小，应看真实数值日志。

不要添加 `Gameplay.DamageImmunity` 或 GodMode，它们会改变真实伤害接受结果；不要只改 Health 而不改 MaxHealth，否则现有夹取可能把数值截回。

## 2. 可调配置

资产位于 `/Game/CodexText/HitReactionValidation/Dummy/`：

- `DA_DummyStats.MaxHealth.Value = 1000000`，没有血量曲线覆盖；等级范围 1～1，蓝图 InitialLevel=1。需要调整耐久时只改该副本。
- `DA_DummyPawnData` 引用独立 Stats、Profile、AS_Dummy 和假人蓝图，无默认武器。
- `AS_Dummy` 只授予原生 HitReaction，不重复授予 HealthSet/CombatSet。
- `DA_DummyHitReaction` 是当前正式 Hero Profile 快照，保留全部六类 Impact。修改这个副本不会写回正式 Profile；动画引用仍指向正式资产，修改动画本体仍会影响正式角色。
- 蓝图使用 `/Script/AIModule.AIController`，AutoPossessAI=PlacedInWorldOrSpawned。不要移除控制器或禁用移动，否则会妨碍初始化或运动受击。

## 3. 打开观察日志

在编辑器 Output Log 的命令输入模式选择 **Python**，执行：

```python
import sys; sys.path.insert(0, 'E:/Project/Git/Hodgepodge/Tools/HitReactionDummy'); import control as dummy; dummy.start()
```

观察工具在服务器 PIE 世界监听真实生命和轻反馈事件，并逐帧读取现有公开裁决/状态接口：

- Health 真实变化：旧值、新值、来源。
- Reaction：HitId、实际伤害、AttackRank、BodyRank、Outcome。
- State：阶段、类型、Sequence、Montage、播放位置和混合权重。
- 运动：胶囊位置、速度、空中追击计数。
- LightFeedback：实际收到的方向事件。

事件输出到画面及 Output Log 的 `[HitDummy]`。服务器记录包含未复制的裁决结果；Dedicated 情况下画面文字不会自动出现在客户端，必须检查服务器日志，不能把它当作跨端 HUD。

生命事件可能先于 FinishHit，所以观察工具不会把旧 LastResult 错配给本次扣血。没有 Reaction 配置的攻击可能只有 health 事件，这是有效的基线结果。

UE5.5 Python 不能直接匹配当前 const-ref 结构体委托，故裁决/状态使用公开 getter 观察。同一帧发生多次结算时，可能只看到最后一次裁决；这不是逐接触完整遥测。运动阶段约每 0.1 秒采样一次。Montage 权重也不是上半身分骨层权重。

结束观察：

```python
dummy.stop()
```

结束 PIE 也会自动解绑、撤销测试装备来源，并输出 `Saved/HitReactionDummy/observations.json`。内存记录有界，最多保留约 5000 项，供诊断使用，不属于生产遥测。

## 4. 独立触发七类反应

2026-10-16 的表现切片已补齐正式五段 Reaction：01／02 为局部反馈，03／05 为硬直，04 为击退；伤害、倍率和通知未改。此前“只有第一段有 Reaction”是修改前审计事实。独立测试攻击仍用于隔离六种 Impact，不代替正式连段演示。

先把角色与假人放到可命中站位：

```python
dummy.reset()
dummy.probe('Light')
```

其他地面用例分别执行，每次等本轮恢复后再开始：

```python
dummy.reset(); dummy.probe('HitStun')
dummy.reset(); dummy.probe('Knockback')
dummy.reset(); dummy.probe('Launch')
dummy.reset(); dummy.probe('Knockdown')
```

空中追击使用自动延迟，避免手动输入错过窗口：

```python
dummy.launch_then('AirHit')
dummy.launch_then('Slam')
```

`reset()` 只取消受击、清速度并恢复测试站位，不恢复血量。测试攻击使用约 350 cm 的检测范围，超出时明确报错；测试装备先撤销上一来源，再授予当前一个，不覆盖玩家正式武器。测试资产复用 `Status.Attack` 作为能力身份，因此不能同时装备多个测试来源。

每类资源为 `DA_Probe_<类型>`、`AS_Probe_<类型>`、`BP_ProbeGrant_<类型>`，共享 `AM_DummyProbe`。调用仍经过原生 Melee → 动画通知 → 检测 → GE → HealthSet → HitReaction，返回 queued 只代表排队，不代表已经命中。

## 5. 如何判定响应

- Light：生命下降，Outcome=NoImpact，收到轻反馈，不应产生控制或位移。
- HitStun：生命下降、Outcome=Applied，进入 Controlled，播放目标 HitStun 后恢复。
- Knockback：观察约 150 cm/0.3 秒测试配置的胶囊轨迹与脚步是否匹配；碰撞可缩短实际距离。当前恒速及结束清零原样保留。
- Launch：观察离地、Airborne、AirLoop 与落地恢复；它不是根运动动画驱动胶囊。
- AirHit：必须先离地，观察新 AirHit 及追击计数，不能用地面空挥作为通过证据。
- Slam：先挑飞再下砸，观察落地、Downed 和 GettingUp。
- Knockdown：观察倒地停留、起身、控制解除；重复命中是否突然换回站立动作属于待审查现象。

再用正式五段顺序 01→02→03→05→04 对照，参考[本轮表现验证](../Validation/hit-presentation-2026-10-16.md)。默认假人仍是普通体状态；高抗用例由验证器临时添加并配对移除 SuperArmor，不添加无敌。`control.py` 已使用具名 Yaw 参数修正测试朝向，不能用 Python Rotator 的位置参数顺序推断 C++ 顺序。真实敌人出招的主观观感与原生不取消能力断言分别验收。

测试时先关闭非必要震屏，正常速度检查，再用录像播放器慢放。不要用全局时间缩放改变服务器攻击节拍。代码日志通过不等于动画方向、接触感和恢复观感通过。

## 6. 生成、复跑与保护边界

首次生成命令（已有目标会拒绝覆盖）：

```powershell
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -run=pythonscript '-script=E:/Project/Git/Hodgepodge/Tools/HitReactionDummy/create_assets.py' -unattended -NullRHI -nosplash -nosound
```

`create_assets.py` 只创建新副本、编译新蓝图和保存明确目标，不调用 SaveAll。已有目标先检查再明确备份/另命名，不能通过删除目录或强制覆盖来重跑。假人测试资源限定于该目录，不加入正式 Experience。

`validate_pie.py` 为隔离地图启动单人 PIE，验证七类测试中的真实扣血、阶段、运动及恢复，结果在 `Saved/HitReactionDummy/pie-results.json`。它会结束本次 PIE、恢复测试偏好并退出用于验证的编辑器，**不要在有未保存工作的编辑器里直接执行此脚本**。手动观察使用 control.py，不会主动退出编辑器。

本轮实际验证结果见[假人交付记录](../Validation/hit-reaction-dummy-2026-10-16.md)。
