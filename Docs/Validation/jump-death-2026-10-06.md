# 跳跃衔接修正与死亡能力验证 · 2026-10-06

基于用户已提交的 `7f4e8e4`，读取正式 GA_Hero_Jump / GA_Hero_Death、DA_Pover、输入配置、主动画蓝图及固定层。两项 GA 已由 DA_Pover 授予；Jump 输入为 IA_Jump / InputTag.Jump。工作区开始时干净。

## 跳跃：已修正动画过渡

Jump GA 调用 CharacterJumpStart / CharacterJumpStop，等待按键释放后结束；动画由现有移动状态机驱动。连续采样确认起跳 SequencePlayer 每次均从约 0.0167 秒开始，没有漏执行 Jump，也不是起跳片段未重置。

`/Game/Main/Character/Hero/Anim/ABP_Pover_Base` 的 `AnimGraph → LocomotionSM → JumpStart → JumpStartLoop` 原为 0 秒过渡。短起跳片段与另一空中循环的硬切在后续跳跃出现明显姿势变化。只将该边的 CrossfadeDuration 改为 0.1 秒，保留自动过渡规则；没有改 Jump GA、跳跃物理参数、动画素材或空中状态数量。

精确节点：`AnimGraph.AnimGraphNode_StateMachine_0.LocomotionSM.AnimStateTransitionNode_22`。

约 60 FPS 的单人实测：修改前 453 条、修改后 455 条，均连续跳跃五次（包含落地后快速再次跳跃）。第二次及后续起跳衔接区间的左脚最大单帧世界位移由约 35.98 cm 降至约 10.8 cm；同一相对时刻，第一次和后续跳跃的左脚位置由约 27 cm 差异降至约 0.3 cm。此指标用于定位和比较衔接，不代表所有骨骼、帧率和镜头下均已完成视觉验收。

双人 Listen Server、100ms 延迟：1430 条采样，拥有者实际输入八次跳跃（五次连续原地、三次移动跳跃），每次拥有者和服务器均出现上升速度，移动跳跃具备水平速度。没有改 C++，未执行新的 Editor/Game 构建。

主图通过原生动画编译接口编译：ERRORS=0、WARNINGS=0；保存后数据校验 VALID。固定层及用户两项 GA 保持原文件内容。

## 死亡：事件及阶段能运行，生命周期验收未完整通过

使用服务器上另一角色的 ASC 创建实际 `GE_Damage_Basic_SetByCaller` Spec，以 `SetByCaller.DamageMultiplier=1000` 放大捕获的 BaseDamage，再应用到目标 ASC。它执行 HodgeDamageExecution → HealthSet.Damage → Health 归零 → HealthComponent.GameplayEvent.Death → GA_Hero_Death；没有直接手动调用 StartDeath 或 FinishDeath，未伪造死亡事件。

分别测试了拥有者死亡和主机死亡的模拟代理观察，均为双人 Listen Server、100ms 延迟，记录 958 / 776 条采样。观察到：

- 致死伤害后服务器及远端进入 DeathStarted，Dying Tag 生效。
- 当时正在播放的普通攻击被取消，手持窗口清理；死亡期间再尝试攻击／跳跃未恢复行动。
- 移动模式变为 MOVE_NONE，胶囊碰撞为 NO_COLLISION。
- 服务器经过约 8.01 / 8.02 秒推进 DeathFinished，Dead Tag 生效，随后 Pawn 销毁；拥有者和模拟代理补执行对应阶段并清理 Pawn。
- 每端记录一个 Started 和一个 Finished，不重复执行这两个死亡阶段。

但首次攻击中死亡的日志出现 `ExclusiveCount <= 1` ensure 和 `AddAbilityToActivationGroup: Multiple exclusive abilities are running`；随后结束该 PIE 时又出现 `Default__GA_Hero_Death_C was still active`，涉及 AbilitySpec 活动计数／清理。阶段断言通过不能掩盖这些生命周期错误。当前结论是“死亡 GA 能正常由事件触发并推进死亡阶段”，整体死亡系统验收仍未通过，计数异常的确切原因尚需进一步定位。本次按用户要求验证死亡，未改动 GAS 核心或用户死亡 GA 来处理该问题。

当前死亡蓝图调用 GameplayCue.Character.Death、设置 CM_ThirdPerson_Death、WaitDelay(Duration=8) 和 EndAbility；没有死亡蒙太奇播放节点。不将本次阶段测试等同于倒地动画或 Cue 可见效果验收，也未验证完整重生。

初次将同一角色作为伤害来源和目标的试验没有扣血，这是 CanDamage 禁止自伤且当前 Execution 使用 BaseDamage × DamageMultiplier 的规则；随后改为另一角色造成伤害。没有修改正式伤害 GE。

## 检查脚本问题与恢复

早期检查误把状态机顺序索引 0 传给要求编译节点索引的 GetCurrentStateName，导致编辑器无响应。检查前无未保存资产/地图，发送正常关闭请求后无响应，结束该测试进程并重新打开。项目文件未因此修改。随后改用经类型安全 getter 核实的资产播放器索引采样；错误记录不计入验收。

早期脚本遍历死亡类对象时包含 CDO，调用 ActorInfo getter 产生 IsInstantiated / CurrentActorInfo ensure；这是检查脚本问题。后续使用 GetGameplayAbilityFromSpecHandle / IsGameplayAbilityActive 检查实例，确认每份 ASC 只授予一份死亡 Spec。这些脚本错误与上述实际互斥计数和 PIE 清理错误分别记录，不混淆归因。

## 备份与操作记录

备份：`E:/Project/Backups/Hodgepodge/JumpDeath/20261006-170701`，537 个文件经过 SHA-256 校验，附 RESTORE.md / manifest.json。文档更新前对比备份，仅主动画蓝图一个文件改变；Source、GA_Hero_Jump、GA_Hero_Death、固定层及配置均保持原内容。

实际执行入口：

```powershell
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/JumpDeath/inspect.py
python -X utf8 Saved/JumpDeath/backup.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/JumpDeath/verify_jumps.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/JumpDeath/fix_jump_blend.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/JumpDeath/verify_jumps_after.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/JumpDeath/verify_jumps_network.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/JumpDeath/verify_death_network.py
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/JumpDeath/verify_death_simulated.py
```

原始回执、完整采样、导出和日志保留在 Saved/JumpDeath；不纳入源码。本次未执行 Cook/打包、独立进程客户端、Dedicated Server、所有取消/预测拒绝边界或重生验收；正式五段 HitWindows 仍为空，因此未声称正式普攻命中致死已接通。项目既有 CommonUI GameViewportClient 错误仍在。结束 PIE 并恢复临时延迟、单人测试设置，编辑器停在 ThirdPersonMap，资产/地图无未保存修改。未提交。
