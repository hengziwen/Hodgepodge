# 项目定位、目录与文档可信度

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 当前基线

引擎 UE 5.5.4，单一业务 Runtime Hodgepodge，加 Editor 工具模块 HodgeAbilityEditor。CodexText 位于主模块与 Content 内，不能称独立模块。

默认地图 ThirdPersonMap，编辑器启动 MainMenu。默认 Experience 使用 `/Game/Main/Data/PawnData/DA_Dafult_PawnData`，创建 BP_Hero_Pover；配置回退 PawnData 仍在 Main/Data 根目录。

## 目录职责

- Source/Hodgepodge/Public 与 Private：反射接口、实现；同类实现集中一个主 cpp。
- Source/HodgeAbilityEditor：Definition 编辑器、动画文本导入/引用修正/编译诊断，不进入 Game 目标。
- Content/Main/Character/Hero/Anim：正式主图 ABP_Pover_Base、固定层、骨架、移动和 FullBody 攻击资源。
- Content/Main/Character/Hero/Ability/BasicAttack、Main/Data/Combo、Main/Weapon/Presentation、Main/Data/CharacterStats：正式技能、窗口、连段、武器表现与成长配置。
- Content/CodexText：菜单、测试夹具、旧资源和历史备份；正式角色依赖的 39 个资产已迁入 Main。
- Content/CodexText/LyraAnimation：实验地图、测试角色和研究入口；旧 ALS 风格 CodexText 源码仍是另一套实验。
- Equipment、Combat、Component：默认装备与表现、检测规则与数据、连段/检测协调和旋转约束。
- UI：CommonUI 迁移层；有停用代码和未接前端依赖，文件数量不代表完成度。
- Config、uproject、Build.cs/Target：配置与模块事实；Plugins 的默认启用与主模块直接依赖是不同层级。

## 已纠正的旧结论

CombatComponent 不再是占位；不存在旧独立 ComboComponent，职责已统一到 Pawn 的 HodgeCombatComponentBase。装备通过 Experience AddComponents 注入，不能以没有 CreateDefaultSubobject 判断未挂载。伤害倍率已按目标规则计算，测试链可扣血；正式五段 HitWindows 仍为空。

精确文件数、HEAD、hash 用 [快照](Reference/snapshot.md)，行为与配置用 [本轮更新](26-update-2026-10-06.md)。README 为当前入口，旧 README、评审和每日更新均标记历史。保留 Dafult、PlayState 等实际拼写。
