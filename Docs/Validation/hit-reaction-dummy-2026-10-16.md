# 高血量受击假人交付验证

日期：2026-10-16；UE 5.5.4。变更为新测试资产、Python 工具和文档，不修改 Runtime/Editor C++、正式攻击或正式地图，不自动提交。既有未提交 R9/R5/R1/R3 与地图外部 Actor 删除保留。

## 资产与实现

- `/Game/Tests/Combat/L_HitReactionDummy`：独立地面、光源、PlayerStart 和一个预放置假人，使用既有正式 GameMode/默认 Experience。
- `/Game/CodexText/HitReactionValidation/Dummy`：27 个独立资源，包括假人蓝图、属性/PawnData/能力集/Profile、一个测试 Montage、七类各自的 Definition/AbilitySet/装备配置蓝图。
- Stats 的常量 MaxHealth=1,000,000；EnemyInit 通过既有 GE 初始化 MaxHealth，再设置 Health。普通 AIController 保留 Possess，不运行行为树；正常 CharacterMovement 保留。
- 受击 Profile 为正式 Hero Profile 的独立副本，仍引用正式骨架与动画，保留当前已知表现问题。只授予一份受击 GA，不添加无敌、免疫或自动回血。
- 七类测试实际经过装备来源授予、原生 Melee、Notify、检测、GE 和目标受击链。Light 不请求 Impact；AirHit/Slam 必须先离地。
- 创建脚本拒绝覆盖已有资产；只保存明确创建的资源。手动观察工具有界保存事件，结束 PIE 配对解绑并撤销自己授予的资源。

## 实际命令与结果

资产创建：

```powershell
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -run=pythonscript '-script=E:/Project/Git/Hodgepodge/Tools/HitReactionDummy/create_assets.py' -unattended -NullRHI -nosplash -nosound
```

成功输出 `Saved/HitReactionDummy/create-2.log`，退出 0，保存 27 个测试资源和独立地图。首轮脚本写入 EditDefaultsOnly 结构体成员被引擎拒绝，改用项目已有的结构体文本导入方式后重跑；首轮没有保存目标资源，没有覆盖源资产。

冷启动发现复制旧蓝图后的 HealthSet/CombatSet 默认引用为空；已改成从原生类新建蓝图，并编译持久化 PawnData/AIController 默认值、显式配置地图实例。原失败版本分别备份到 `/Game/CodexText/HitReactionValidation/DummyBackups/20261016/` 的 BeforeNativeRebuild、BeforeDefaultBake 两个资产，不在实际假人依赖链。`bake-2.log` 退出 0，之后从磁盘冷启动验证真实属性，而非运行时补数值。

单人 PIE 验证使用独立隐藏编辑器进程、该测试地图和 `-ExecutePythonScript=.../Tools/HitReactionDummy/validate_pie.py -unattended -NullRHI -nosplash -nosound`。最终日志为 `Saved/HitReactionDummy/pie-7.log`，结果在 `pie-results.json`，206 条事件/采样在 `observations.json`。最终日志没有 Error/Ensure/Traceback；编辑器正常退出。机器可读的[入库摘要](hit-reaction-dummy-2026-10-16-results.json)保留完整七组结果。

- 初始 Health=MaxHealth=1,000,000，普通 AIController，正常地面移动模式。
- Light：真实扣血 3.125，NoImpact，收到轻反馈，未进入控制。
- HitStun：Applied，进入 Controlled 并恢复。
- Knockback：Applied，目标 X 从 180 到约 330.001 cm，符合本例无障碍 150 cm 配置，之后恢复。
- Launch：Applied，胶囊中心最高约 307.705 cm，经历 Airborne 和落地 Controlled 后恢复。
- Knockdown：Applied，观察到 Downed、GettingUp，最终解除控制。
- Launch→AirHit：两次真实扣血、两次 Applied，胶囊最高约 354.329 cm，最终恢复。
- Launch→Slam：两次真实扣血、两次 Applied，经历 Airborne、Downed、GettingUp，最终恢复。
- 九次真实扣血合计 28.125，最终 Health=999,971.875；MaxHealth 保持 1,000,000。没有靠自动回血维持存活。

观察工具修正了 Python const-ref 结构体委托不匹配及 Guid wrapper 字符串含地址的问题，使用公开 getter 和 Guid 值文本去重。裁决轮询可能合并同一帧的多个中间结果，不作为完整接触遥测；真实生命及轻反馈仍通过事件记录。

Python 三文件语法检查、文档链接/知识库快照检查和 `git diff --check` 在最终交付时执行。已核实任务开始时的 PIE_Client/2 玩家/单进程偏好，测试结束后恢复；不修改项目默认地图。

## 验证边界

未改变 C++，本轮不把旧 Editor/Game 构建计为新构建。蓝图编译与保存、真实 PIE 数值/阶段、主观画面评价分别报告；NullRHI 不能确认方向、脚步、相机、接触音画或手感。未执行 Listen/Dedicated、延迟丢包、Cook/打包。

正式五段后四段无 Reaction、Hero 实际编译权重 1.0、长轻反馈素材以及 Client 普攻窗口问题保留，不以假人测试通过宣称已修复。使用步骤见[手册](../Guides/hit-reaction-dummy.md)，实现现状见[审计记录](../Design/hit-reaction-implementation-audit-2026-10-16.md)。
