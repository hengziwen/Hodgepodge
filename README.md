# Hodgepodge

基于 Unreal Engine 5.5.4 的动作 RPG 框架，沿用 Lyra 的 Experience、PawnData、GameFeature、Init State 与 GAS 组织方式。当前可运行玩家移动、固定移动动画层、五段普攻、攻击朝向约束与武器显隐；目标锁定与软锁定镜头、Dash/Sprint、角色朝向模式、受击系统、Guardian 敌人 AI、基础 HUD 与真实血条／技能栏、共鸣（Resonance）与暗涌（Havoc）技能体系均已接入并各有验证报告；独立专服发布、Cook/打包、完整前端与库存仍需后续实现或验证。

> 状态核对：**2026-10-15**（HEAD `b702482`），基于当前磁盘源码、配置及资产内部值。当前提交基线与文件 hash 见 [扫描快照](Docs/KnowledgeBase/Reference/snapshot.md)。本文不把历史构建通过等同于所有玩法、联机或打包通过。

> **攻击时机已彻底迁到 Montage 动画通知**：旧 `HodgeAbilityTimeline` / `PlayTimeline` / `TimelineEvaluator` 已移出 `Source/` 并归档在 `Archive/Timeline`（**不参与构建**）。凡看到"Timeline 窗口"的旧描述，请以 [通知配置手册](Docs/Guides/attack-ability-configuration.md) 为准。

## 文档入口

- [UI 基础配置指南](Docs/Guides/ui-foundation-configuration.md)、[本轮 UI 验证](Docs/Validation/ui-foundation-2026-10-08.md)：自有根布局、Experience 注入、真实 HUD 与菜单输入路由。

- [攻击能力配置手册](Docs/Guides/attack-ability-configuration.md)：Definition、AnimNotify、HitCheck、Profile、连段与输入的逐字段说明、配置配方和排查入口。
- [客户端连段／移动取消窗口修复](Docs/Validation/client-combo-window-2026-10-07.md)：通知首帧授权顺序与正式连段客户端回归。
- [本地知识库](Docs/KnowledgeBase/README.md)：对象职责、初始化链、配置、操作与排障。
- [本轮完整更新](Docs/KnowledgeBase/30-update-2026-10-15.md)：共鸣／暗涌、技能配置工具、架构评审与 Timeline 退役；上一轮见 [26 号](Docs/KnowledgeBase/26-update-2026-10-06.md)、[27 号通知迁移](Docs/KnowledgeBase/27-update-2026-10-07-anim-notify.md)、[28 号 UI](Docs/KnowledgeBase/28-update-2026-10-08-ui.md)。
- [当前任务与待办复核](Docs/KnowledgeBase/29-current-work-and-backlog.md)：已完成范围、后续优先级与用户暂缓事项。
- [当前接通状态与待办](Docs/KnowledgeBase/12-integration-backlog.md)、[验证范围](Docs/KnowledgeBase/16-validation.md)。
- [架构审查与分阶段重构方案](Docs/Design/architecture-refactor-review-2026-10-15.md)：只完成评审，尚未实施结构重构。
- [设计与实施文档目录](Docs/README.md)、[开发约定](AGENTS.md)、[开发和构建流程](Docs/AI_DEVELOPMENT.md)。
- [源码索引](Docs/KnowledgeBase/Reference/source-index.md)、[资产清单](Docs/KnowledgeBase/Reference/assets.md)、[配置](Docs/KnowledgeBase/Reference/config.md)。
- [正式资产迁入 Main](Docs/Validation/main-assets-migration-2026-10-06.md)：39 个资产的新路径、备份及本次验证。
- [更新前长版 README](Docs/History/README-before-20261006.md) 保留学习材料；其中旧状态不作为当前事实。

## 当前运行入口

游戏默认地图为 `/Game/ThirdPerson/Maps/ThirdPersonMap`；编辑器启动地图为 `/Game/CodexText/L_MainMenu`，两者用途不同。默认 Experience 为 `/Game/Main/Experiences/Exp_HodgeDefaultExperience`。

Experience 的 DefaultPawnData 实际指向 `/Game/Main/Data/PawnData/DA_Dafult_PawnData`，PawnClass 为 `/Game/Main/Character/Hero/BP_Hero_Pover`，输入配置为 `/Game/Main/Input/DA_HodgeInputConfig`，默认相机为 `/Game/Main/Camera/CM_ThirdPerson`。AbilitySets 含 `Main/Data/AbilitySet/DA_Pover` 与 `Main/Data/AbilitySet/AS_LightCombo`；关系映射目前为空。

`DefaultGame.ini` 中 AssetManager 的回退 PawnData 仍是 `/Game/Main/Data/DA_Dafult_PawnData`。这是回退入口，不要把它与 Experience 正在使用的子目录资产混为一谈，也不要仅改文档就变更配置。

## 已接入的功能

- Main/UI 的八个界面 WBP 已保存 Designer 控件树与 Graph；血条、技能栏、菜单和确认框使用引擎或通用页面基类，一个共享数据源接入玩法。旧业务 C++ 控件类型归档在 Archive/UI/NativePrototype。

- 玩家 ASC 由 PlayerState 持有，PawnExtension 绑定当前 Pawn Avatar；HeroComponent 负责输入和相机就绪。Experience 加载完成后再允许生成玩家。
- Experience 的 AddComponents 为 Pawn 添加 EquipmentManager 和统一 CombatComponent。CombatComponent 管连段、命中检测会话和结果上报；Melee GA 构建与应用伤害 GE。
- 默认装备为 `Main/Data/Equipments/BP_Equipment_Sword`，实例为 `Main/Weapon/BP_WeaponInstance_Sword`。服务器在 ASC 就绪后装备，装备授予句柄可精确撤销。
- 主动画为 `Main/Character/Hero/Anim/ABP_Pover_Base`，固定层为 `Layer/ABP_Pover_LocomotionBase`，接口为 `ALI_Pover_LocomotionInterface`。当前不按武器更换移动动画层。
- 移动直接进入 Cycle，不使用 Start；保留 Stop，暂时禁用 Pivot。腿部 IK、连续镜头转身及 FullBody 攻击姿势修正已接入。攻击蒙太奇使用 FullBody，DefaultSlot 已移除。
- 普攻按 `01→02→03→05→04` 执行，Definition 位于 `/Game/Main/Character/Hero/Ability/BasicAttack`，连段定义和表位于 `/Game/Main/Data/Combo`。默认输入缓存 0.3 秒，动作结束后连段记忆 1 秒；移动取消或其他中断不直接清空前四段进度。末段后摇窗口可直接接第一段，末段结束后从第一段重开。
- Montage 状态通知的 `Status.Rotation.Locked` 区间由角色旋转组件响应，约束角色 Yaw，允许镜头继续旋转；退出后有界恢复，并处理移动重放与网络校正。
- WeaponInstance 响应武器手持 NotifyState、独立请求及复制/预测；可见 Mesh 显现、回背、悬浮、消隐，检测 Mesh 保持手部来源。背部默认挂接 `WeaponOnBack`，`BackTransform` 是插槽内偏移。
- 伤害 Execution 已使用统一目标规则和倍率，不再恒为零；测试夹具验证了服务器命中→GE→扣血。正式五段已迁移为 Montage 命中通知；第四段采用 Bip001LHand 直接身体检测，正式配置与本次验证见通知迁移报告。
- 命中检测由统一 CombatComponent 承载：Profile 选策略（刀身扫掠／盒体扫掠／形状查询），检测体用形状配置（Sphere／Box／Capsule + 锚点），调试绘制由 `Hodge.Combat.DebugDraw.*` 控制（仅服务器绘制）。
- 目标锁定：中键获取／解除与鼠标甩动切换，短命令 GA + 服务器资格确认 + 拒绝恢复；锁定镜头使用独立 CameraMode，含屏幕死区、停手宽限与分轴响应；**攻击自动索敌未启用**。
- Dash／Sprint：Shift 起停，LocalPredicted 会话 + 根运动位移；支持离地续冲、Pivot 滑步修复与被攻击打断（标签映射）；**体力系统按用户要求暂时移除**。
- 角色朝向：`HodgeCharacterRotationComponent` 仲裁 + 复制 + SavedMove 回放；Free／ReservedStrafe 两种风格，`bUseControllerRotationYaw` / `bOrientRotationToMovement` 只是执行结果。
- 受击系统：四级 Body／独立判定／Impact／恢复；链路为攻击方 Melee GA → HealthSet 记录 → 目标侧受击 GA（**动画通知驱动**）。
- 敌人 AI：Guardian 已接入主流程（AIC／BT／BB + GAS 攻击 + EnemyInit 初始化 + 死亡 GA），有原生／蓝图／单人／Listen 四类验证。
- 共鸣／暗涌：能量与暗流燃料挂在 PlayerState 的 ASC；E 键共鸣技能、R 键共鸣解放（含前半段电影镜头）、暗涌灭音与强化。⚠️ 参数为原型初值，`HodgeFill*` 是调试命令。

## 常用调参

角色属性首版已接入：PawnData.StatProfile 提供等级基础值，EquipmentDefinition.StatProfile 提供独立装备加成，PlayerState 属性协调器在默认装备完成后提交出生资源。升级保持血量比例，装备变化保留绝对血量。配置在 `/Game/Main/Data/CharacterStats`；[成长配置说明](Content/Main/Data/CharacterStats/README.md) 和 [实现/验证](Docs/Design/character-attribute-growth.md) 记录实际 API 与示例数值。经验、突破、库存和存档后端仍未实现。

- 移动/动画：[Main 动画说明](Content/Main/Character/Hero/Anim/README.md)。
- 攻击时机：在 `/Game/Main/Character/Hero/Anim/Montages/AM_Attack01～05_Montage` 通知轨调整命中、旋转锁、接段、移动取消与手持区间；旧 Timeline 资产及 C++ 集中保留在 `Archive/Timeline`。
- 连段：`/Game/Main/Data/Combo/DA_LightCombo` 的 ComboRetentionSeconds，以及 DT_LightCombo 的跳转与窗口条件。
- 武器：`/Game/Main/Weapon/Presentation/DA_SwordPresentation`。BackSocket 默认 WeaponOnBack；背部基础位置和旋转在骨骼插槽调整，BackTransform 只做单件偏移。当前显现 0.08、宽限 0.06、回背 0.3、悬浮 2、消隐 0.35 秒。
- 锁定：`DA_HeroLockOn` + [配置手册](Docs/Guides/lock-on-configuration.md)；朝向：[配置手册](Docs/Guides/character-facing-configuration.md)。
- Dash／Sprint：[配置手册](Docs/Guides/dash-sprint-configuration.md)、[中断配置](Docs/Guides/dash-interrupt-configuration.md)。
- 受击：[配置手册](Docs/Guides/hit-reaction-configuration.md)。共鸣：[技能配置](Docs/Guides/resonance-skill-configuration.md)、[共鸣解放配置](Docs/Guides/resonance-liberation-configuration.md)、[图标接入](Docs/Guides/resonance-ability-icons.md)。翅膀：[通知蒙太奇配置](Docs/Guides/wing-notify-montage-configuration.md)。HUD：[参考 HUD 配置](Docs/Guides/main-hud-reference-configuration.md)。敌人：[配置手册](Docs/Guides/enemy-ai-configuration.md)。
- [连段记忆](Docs/Design/combo-retention.md)、[旋转约束](Docs/Design/character-rotation-policy.md)、[武器表现](Docs/Design/weapon-presentation.md) 包含实际配置与验证记录。

## 工程与构建

业务 Runtime 模块为 Hodgepodge，工具模块 HodgeAbilityEditor 仅用于编辑器。CodexText 是主模块内的实验源码目录和 Content 目录，不是独立 UBT 模块。默认角色的普攻、连段、武器表现、成长配置及所需材质/贴图已迁入 Main；CodexText 保留菜单、测试、实验与历史备份，不能整体删除该目录。

本机引擎路径为 `E:/UE/UE_5.5`，项目为 `E:/Project/Git/Hodgepodge`。使用 VS 2022 / MSVC 14.38 与 Windows SDK；保持 UE 5.5 兼容。反射修改后先保存并关闭编辑器，再常规编译：

```powershell
$ueRoot = 'E:/UE/UE_5.5'
$projectFile = (Resolve-Path './Hodgepodge.uproject').Path
& "$ueRoot/Engine/Build/BatchFiles/Build.bat" HodgepodgeEditor Win64 Development "-Project=$projectFile" -WaitMutex -architecture=x64 -NoHotReloadFromIDE
& "$ueRoot/Engine/Build/BatchFiles/Build.bat" Hodgepodge Win64 Development "-Project=$projectFile" -WaitMutex -architecture=x64
```

Runtime 改动同时验证 Game。Live Coding 不替代常规构建；Game 构建不等于 Cook/打包。项目当前没有 Server/Client 专用 Target，不自行升级引擎或构建设置。

## 已知边界

- 正式普攻已迁移为通知驱动；实际伤害与窗口验证见 [迁移报告](Docs/Validation/anim-notify-migration-2026-10-07.md)，动作覆盖与美术打磨仍可继续调整。
- 死亡/重生完整玩法、技能取消矩阵、双持/投掷、库存（仅设计）尚待完善。常态 Pivot 当前刻意禁用；**体力系统已按用户要求移除**；**敌人 ASC 与冲刺 GA／Pivot 已不再是缺口**（见上文"已接入的功能"）。
- 正式五段普攻的动画通知命中配置需在编辑器逐段确认（旧"HitWindows 为空"结论属 Timeline 时代）；共鸣／暗涌参数是原型初值，正式 Niagara／音效／震屏素材待补。
- [架构审查](Docs/Design/architecture-refactor-review-2026-10-15.md) 的 R1~R8 **尚未实施**（优先建议把命中检测从 CombatComponent 抽离）。
- UI 基础闭环已接通，使用现有 CommonUI／CommonInput 和自有管理器，不引入 CommonGame／CommonUser／ModularGameplayActors。完整前端、设置持久化、登录和匹配另行制作。
- PlayerState 的 PawnData AbilitySet 授予仍未收集撤销句柄；不要混同于装备 Manager 已支持的精确撤销。
- Cue 路径观察者、预加载、热卸载与重生清理需要专项验证。网络后摇窗口内续段已有测试，延迟下所有窗口边界并未覆盖。
- UI 本轮执行 Editor/Game、原生测试、单人、双人 Listen Server 与两客户端 PIE 回归；PIE 的专服世界不等于独立 Server 可执行文件。未执行 Cook/打包或独立进程，其他系统以各自报告为准。

## 维护

同一个自有 C++ 类的非内联实现集中在一个主 cpp，禁止按功能拆分或 include 其他 cpp 绕过。查看文档时先区分当前指南、设计/实施记录和标注日期的历史资料。

```powershell
python -X utf8 Docs/KnowledgeBase/tools/kb.py check
python -X utf8 Docs/KnowledgeBase/tools/kb.py refresh
python -X utf8 Docs/KnowledgeBase/tools/kb.py search "WeaponOnBack"
```

先核对源码/资产与人工章节，再 refresh，最后 check。索引刷新只证明文件已扫描，不证明功能通过验证。
