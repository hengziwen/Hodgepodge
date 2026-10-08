# 2026-10-07：通知驱动攻击与 Timeline 归档

当前分支 dev-AN，迁移前提交 b1482a5。本次没有创建新分支、自动提交或升级引擎。

## 当前职责与调用链

Definition 保留技能身份、GA、Montage、播放倍率、Blend、执行路由和默认伤害，退出旧时机绑定。GAS 仍由 PlayerState 拥有，连段、输入缓存与记忆由当前 Pawn 的 CombatComponent 管理。

GA 激活时建立新 ExecutionId，准备命中 Task 与服务器骨骼租用，再播放 Montage。通知从原生 EventReference 的 FAnimNotifyMontageInstanceContext 核对 Montage 实例、Mesh 和 Avatar，不能把旧回调交给新动作。通知对象仅存配置，状态句柄与命中记录属于本次 GA。

命中通知直接填写身体骨骼、武器来源或虚拟形状；源解析、查询、伤害规则和 GE 提交继续复用。Task 在 PostPhysics 等待来源更新，Body.LeftHand 无需来源注册。去重默认按每次进入独立，显式组以 Group＋AttackPhase 共享，不读触发时间。

状态计数、手持请求、检测会话、Montage 委托及骨骼租用在退出和取消时回收，旧 End 不能撤销新动作。Combat 卸载会结束当前 Definition 执行。WeaponInstance 与 GA 共用 CombatComponent 的骨骼租用入口，不新增角色常驻组件。

## 作者入口

- `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1～5`：默认伤害和动作参数。
- `/Game/Main/Character/Hero/Anim/Montages/AM_Attack01～05_Montage`：命中、状态与手持通知轨。
- `/Game/Main/Data/Combo/DA_LightCombo`、`DT_LightCombo`：连段图、输入缓存及记忆。
- `/Game/Main/Combat/HitProfiles`：可选共享查询／过滤参数；不再选择 Strategy。

五段动作和测试配置已迁移。第四段使用 Bip001LHand 直接身体球扫，半径初值 15cm，并沿用现有 GE_MeleeDamage_Instant；它不受武器手持覆盖限制。伤害 GE 原先为空的第四段已明确补入该项目现有 GE，不在运行时创建隐式效果。

旧 C++／工具在 [Cpp 归档](../../Archive/Timeline/Cpp)，旧 Blueprint／Timeline 和替换前资产在 [Blueprints 归档](../../Archive/Timeline/Blueprints)。归档移出 Source／Content，不参与构建、加载与 Cook；恢复需要相应旧类型与依赖一起恢复。

## 验证证据

实际构建、蓝图、单人、网络、专服 PIE 和 Cook 范围以 [本次迁移报告](../Validation/anim-notify-migration-2026-10-07.md) 为准。旧测试历史不算本次重新通过；独立 Server 可执行文件与完整打包另行验证。

详细配置请直接引用 [攻击能力配置手册](../Guides/attack-ability-configuration.md)。

## 客户端窗口验收修复

远端客户端提前输入在通知首帧消耗时，服务器可能尚未更新当前动画，导致连段预测被拒绝。ASC 现在在现有输入缓存期限内等待有效目标边的权威通知窗口，身份、状态、优先级与能力授权继续核对；移动取消在动画更新后检查。没有修改通知资产或恢复 Timeline。实际结果及原迁移验证范围更正见 [客户端窗口修复报告](../Validation/client-combo-window-2026-10-07.md)。

确认连段还会校正拥有端多推进的动画进度，消除连续预测切段的累计偏差。最终 Client 模式 100ms 连续十轮 51 次激活与服务器一致，第二客户端及 Listen 0／100ms 各两轮也通过；两模式移动取消、过期输入和 18 项原生测试通过，Editor／Game 常规构建通过。
