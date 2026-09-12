# 验收场景与调试观察点

[返回首页](README.md)

## 本轮验证范围

本知识库本轮只做静态源码、配置、文件存在性和链接校验。以下场景均是待执行验收，未填写通过结果。历史 README 编译状态不等于本轮结果。

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

激活、停用、再次激活，记录能力/组件/输入数量。通过条件：无重复授予，撤销完整，多 PIE 上下文隔离。重点覆盖 Receiver 移除与额外输入解除。

## V12：构建与打包

记录完整 Editor 构建，再测试所需 Game/Server 和打包资源。通过条件：无缺类、无旧重定向目标失效、默认地图和数据资产可加载。PIE 成功不能替代此项。

## 验收记录模板

每项记录以下字段：日期；执行者；Git HEAD 与 dirty 状态；引擎版本；地图/Experience/PawnData；NetMode 与客户端数量；复现步骤；期望结果；实际结果；日志路径；通过/失败/阻塞；后续条目编号。

截图可帮助描述视觉结果，但 ASC、权威死亡与复制结论应有日志或调试观察支持。通过后更新相关章节的验证范围，避免知识库永久停在“待验证”。
