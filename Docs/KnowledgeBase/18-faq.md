# 常见问题

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 现在能移动、攻击和连段吗？

正式 ThirdPersonMap 已经验证 Main Hero 的移动、相机、FullBody 与 1→2→3→5→4 连段。无需重复接 HeroComponent、ProcessAbilityInput 或 PawnData 注入。

## 为什么没有 ComboComponent？

已合入 Pawn 上的 HodgeCombatComponentBase，Experience 动态注入。PlayerState 保留 ASC 所有权，不另持旧连段组件。

## 取消后连段为什么还能继续？

CombatComponent 将当前执行与连段记忆分开。前四段结束/取消后默认保留 1 秒，允许的边可续段；其他技能不延长这个时间。末段后摇可接 1，末段结束则重开。输入缓存 0.3 秒是另一项设置。

## 现在攻击会扣血吗？

测试检测/GE 链可扣血，DamageExecution 不再恒零。但正式 DA_Attack_1～5 的 HitWindows 仍为空，需配置后验证；不要把正式动画播放说成正式命中伤害通过。

## 攻击时能转镜头，是否意味着角色也能转？

RotationLock 窗口允许镜头操作但限制角色 Yaw；窗口前可调整，退出平滑恢复。检查 Status.Rotation.Locked 与角色旋转组件，不用屏蔽整个 Look 输入。

## 武器何时显现和回背？

Timeline WeaponHand.StartTime/EndTime 决定申请/释放手持。最后请求释放后按 Profile 的宽限、回背、驻留、消隐秒数执行；连续段请求及时接上会撤销回收。默认 BackSocket=WeaponOnBack，基础姿势改插槽，BackTransform 做局部微调。

## 动画主图在哪里？

`/Game/Main/Character/Hero/Anim/ABP_Pover_Base`，固定层 ABP_Pover_LocomotionBase。最初迁入名 ABP_Pover_Lyra 已被重命名。没有 Start 入口，保留 Stop，常态 Pivot 禁用，攻击用 FullBody。

## CodexText 可以删吗？

不能整体删。默认玩法仍引用 DefinitionCombo、BasicAttack 和 WeaponPresentation；动画主要资源已在 Main。Source 内旧 ALS/Grounded/Survivor 是另一套实验。

## 应改哪个同名 PawnData？

默认 Experience 使用 `/Game/Main/Data/PawnData/DA_Dafult_PawnData`；Config 的 Main/Data 根目录资产是 AssetManager 回退。先沿实际 Experience 引用追踪。

## Editor/Game 构建通过是否等于联机和打包通过？

不等于。已保存的单人与 Listen Server 证据按功能记录；窗口边界、重生/热卸载等仍需补测，Cook/打包及专服未执行。文档刷新本身不运行这些测试。

## UI 与敌人是否完整？

尚未完成。UI 有迁移停用依赖及 GameViewport 配置缺口；EnemyCharacter 尚需明确 ASC 初始化、AI/伤害/死亡/重生链。见 [当前待办](12-integration-backlog.md)。
