# Hodgepodge 项目上下文

2026-10-15 按本工作区源码、配置和本机引擎重新核实。本文件用于定位，具体任务仍需读取实际调用链，不证明玩法或资产运行正确。

- 工程：`E:/Project/Git/Hodgepodge/Hodgepodge.uproject`；引擎：`E:/UE/UE_5.5`，Build.version 为 5.5.4、CL 40574608；Lyra：`E:/Project/UProject/study/Epic/LyraStarterGame`。
- 遵守根 AGENTS.md、Docs/Development/development-standards.md 和 Docs/AI_DEVELOPMENT.md。具体方案前核查 Lyra，同职责成熟机制最小适配；不升级引擎或插件。
- `.uproject` 声明 Hodgepodge Runtime 与 HodgeAbilityEditor Editor；保持单一业务 Runtime、Public/Private、Hodge 命名、V5/Unreal5_5。已有 Game/Editor Target，没有独立 Server Target。
- 当前启用 GameplayAbilities、GameFeatures、AnimationLocomotionLibrary、AnimationWarping、MotionWarping、CommonUI 和 Editor 范围 McpAutomationBridge；UNTLink 禁用。ALS/RiderLink 不做无关修改。
- `Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp` 创建玩家 ASC，使用 Mixed 复制；角色属性资源随 PlayerState 持有。`HodgeEnemyCharacter.cpp` 创建怪物自身 ASC，使用 Minimal。
- `AHodgeCombatCharacter::PossessedBy/OnRep_Controller/OnRep_PlayerState/SetupPlayerInputComponent` 通知 PawnExtension；Hero 的 `HandleChangeInitState` 在依赖就绪后调用 `InitializeAbilitySystem`。HeroCharacter 的同名覆盖只有 Super，不能按旧注释另加一条 GAS 初始化。
- `UHodgePawnExtensionComponent` 负责 Init State、Owner/Avatar 绑定和解绑；旧 Pawn 只在仍为当前 Avatar 时清理 ASC，解绑通知还承担旧 Pawn 自身资源清理。
- `Public/Data/HodgePawnData.h` 已有 PawnClass、AbilitySets、InputConfig、DefaultCameraMode、ComboDefinition、DefaultWeaponDefinition、StatProfile、HitReactionProfile、SprintAbilityProfile、LockOnProfile 等字段；资产内部值需另行读取。
- Experience 入口是 `Private/Component/HodgeExperienceManagerComponent.cpp`；AbilitySet 是 `Private/Data/HodgeAbilitySet.cpp`；GameFeature 授予是 `Private/GameFeatures/GameFeatureAction_AddAbilities.cpp`。基础/装备/Feature 的授予归属与清理分别核实。
- 自定义能力基类 `Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp` 使用 InstancedPerActor；引擎默认策略不替代项目策略。
- 相机沿用 `Private/Camera/HodgeCameraComponent.cpp` 与 HodgeCameraMode 栈；输入沿用 HodgeInputConfig/HodgeInputComponent/Hero；动画沿用 HodgeAnimInstance 的 ASC/GameplayTagPropertyMap 接入。
- Runtime Build.cs 已声明 UMG、Slate、SlateCore、CommonUI、CommonInput，现有 UIManager 位于 `Private/UI/Subsystem`。不能按旧快照断言 UI 框架不存在，也不能仅凭依赖声明断言全部资产已经接好。
- 编辑器接口位于 `Source/HodgeAbilityEditor/Private` 的 UI/Animation/Combat AuthoringLibrary；使用前检查真实 UFUNCTION 和工具 schema，不假定普通 EventGraph 桥支持 AnimGraph、UMG 树或材质图。
- MCP：`Tools/ChiR24MCP/server.py` 连接 3016 端口，入口为 unreal 的 search/describe/execute；`Tools/UnrealMCP/server.py` 使用 55557，并通过 bridge_status 验证项目身份。配置和实际连通结果见 [安装记录](UE55_SKILLS.md)。
- 三套 skill 已安装在仓库 `.agents/skills/`，共 108 个；完整上游内容和许可位于 `.agents/skill-sources/`。入口、重名处理、来源及范围见 [安装目录](UE55_SKILLS.md)，不再依赖旧 LocalUser 用户目录。
- 所有项目新代码使用简洁中文注释；同类非内联实现集中主 cpp；修改资产名、反射字段或标签前核实引用和重定向。保留 Dafult、PlayState 等已有拼写。
- 区分 Editor/Game 构建、蓝图、PIE、联机、Cook/打包的实际验证范围；保留用户已有修改，不批量保存无关资产、不自动提交。
