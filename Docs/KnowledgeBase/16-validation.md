# 验收场景与调试观察点

> 最近源码核对：2026-09-19。源码接入状态与运行验收分开记录。
[返回首页](README.md)

## 本轮验证范围

本知识库本轮只做静态源码、配置、文件存在性和链接校验。以下场景均是待执行验收，未填写通过结果。历史 README 编译状态不等于本轮结果。2026-09-19 知识库更新只做文档与索引核对；V01～V12 与 V14 仍未执行，V13 仅完成下表列出的部分（调度器与标签账本已实测，窗口 GE 与 Point 派发未执行）。

## V01：默认 Experience 启动

操作：正常启动 ThirdPersonMap。观察：实际 GameMode、Identified experience 的 ID/Source、Loaded 状态、控制器和 Pawn。通过条件：默认玩法被选中并生成正确 Hero，而非意外的 CharacterBase。

## V02：指定与错误 Experience

分别传入有效资源名和不存在的名称。观察有效路径是否命中，错误路径是否记录回退，回退是否仍能完成加载。不要仅看到警告就判定失败或成功。

## V03：Pawn 初始化

在 SetPawnData、CanChangeInitState、InitializeAbilitySystem 处观察。通过条件：依赖逐步到达后推进，无重复初始化、无停在 Spawned，角色和 PlayerState 的 ASC 一致。

## V04：本地移动与相机

测试 WASD、鼠标和可用手柄。通过条件：输入方向正确，视角连续，Pawn 移动而非只动画移动，相机跟随且模式栈非空。记录实际 IMC、InputConfig 和模式类。

## V05：技能输入三阶段

分别验证按下、持有、释放。观察 Spec 动态 Tag、输入缓存和 ProcessAbilityInput。通过条件：OnInputTriggered 不重复误触发，WhileInputActive 行为符合预期，释放能送达活动技能，结束后可再次激活。

## V06：EffectContext 与 GE

创建测试 GE Spec。通过条件：Context 是 Hodge 类型，无 check，目标 ASC 收到效果。若新增自定义联网字段，还需在远端验证序列化值。

## V07：Health 与死亡

由服务器应用可控 GE，使 Health 从正值到零。通过条件：数值正确、客户端复制正确、服务器耗尽事件一次、死亡状态可见。验证治疗恢复以及死亡后禁止行为按设计执行。

## V08：重生与换 Pawn

重复销毁/重生三次。比较 ASC Owner/Avatar、Spec 数量、映射和绑定数量。通过条件：旧 Pawn 解绑、输入不重复、临时状态不泄漏，保留能力符合规则。

## V09：两客户端

分别观察拥有客户端和模拟代理的 PlayerState、PawnData、动画、移动、Health。通过条件：远端无需 LocalPlayer 初始化自己的输入，仍正确显示世界状态；双方控制独立。

## V10：Dedicated Server

先修复默认地图登录分支，再测试无渲染服务器与客户端连接。通过条件：Experience 不悬挂，无 LocalPlayer 假设，玩家成功生成，GE 结算以服务器为准。独立 Server 构建另行记录目标和引擎能力。

## V11：GameFeature 生命周期

激活、停用、再次激活，记录能力/组件/输入数量。通过条件：无重复授予，撤销完整，多 PIE 上下文隔离。Receiver 移除与额外输入解除的实现已补齐，本项转为验证运行时是否真的走到这些清理路径。

## V12：构建与打包

记录完整 Editor 构建，再测试所需 Game/Server 和打包资源。通过条件：无缺类、无旧重定向目标失效、默认地图和数据资产可加载。PIE 成功不能替代此项。

## V13：攻击时间轴（⚠️ 部分执行：主路径 + 取消路径已通过，重入类未验证）

时间轴已按统一事件模型重新实现（`UHodgeAbilityTimeline` + `UHodgeAbilityTask_PlayTimeline`，见 [本轮记录](22-update-2026-09-19.md)）；连击（`ComboSet`）仍不存在，不在本项范围。

**已通过**：数据校验规则；窗口标签的进入与退出；**窗口 GE 的施加与移除**（GE 实例数 `0 → 1 → 0`）；**Point 事件派发**与**系统事件 `Timeline.End` 派发**；**中途取消的清理**（`clear_ability(handle)` 后标签与 GE 均归零、时间轴不再推进）；自然结束后的零残留；重复激活无累积；起点接续不重放历史窗口。

**未执行**（补完这些本项才算通过；全部是异常路径）：

1. **重入用例**：造一个"收到窗口标签变化 / Point 事件就立刻 `EndAbility`"的测试能力去撞 `EnterWindow` 的两道防线、`ExitWindow` 不对称约束与清理幂等 —— 期望 `PointEffectClass` **不施加**、清理不重复执行、无残留。**这是剩下最重要的缺口。**
2. **GE 施加失败的补偿**：期望"施加失败不入账、账本不失真"。
3. **连续 20 次激活 / 取消**：期望无累积、无 GE 泄漏（单次取消已通过，压力重复还没做）。
4. **GE 校验的反向用例**：造一个 `HasDuration` 的 GE 配到窗口上，期望校验报 Error（正向用例已通过）。
5. **时钟倒退**：Replay 向后拖时间轴或临时调试入口 —— 期望该帧不消费任何节点、已触发过的 Point 不重复派发、随后正常推进并结束（见 [排障手册](14-troubleshooting.md)）。
6. **`Timeline.Interrupted` 派发**：⚠️ **当前无法验收 —— 本阶段没有触发者**（`StopTimeline(Interrupted)` 全项目无调用点，见设计 §8）。等第二阶段接入"被抢占"语义后再补。

## ~~V14：Bundle 预加载~~（⚠️ 依赖项不存在，暂不适用）

> 本项依赖的 `PreloadPrimaryAssetsOnGrant` / `PreloadHandles` **当前工作区不存在**。仍需验证的相关项是 Experience 的 `Equipped` Bundle 加载（见 [数据资产章节](05-data-assets.md)）。

~~在授予配置了 `PreloadPrimaryAssetsOnGrant` 的技能前后记录耗时与 `PreloadHandles` 数量。通过条件：预加载只发生在授予时、Montage 在首次播放前已就绪、卸载后句柄不泄漏。本项依赖 V13 的 BP 接线，当前为未执行。~~

## 验收记录模板

每项记录以下字段：日期；执行者；Git HEAD 与 dirty 状态；引擎版本；地图/Experience/PawnData；NetMode 与客户端数量；复现步骤；期望结果；实际结果；日志路径；通过/失败/阻塞；后续条目编号。

截图可帮助描述视觉结果，但 ASC、权威死亡与复制结论应有日志或调试观察支持。通过后更新相关章节的验证范围，避免知识库永久停在“待验证”。
