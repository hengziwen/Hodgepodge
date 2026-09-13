# 当前状态、断点与接通顺序

[返回首页](README.md) · [本轮变更](19-update-2026-09-13.md)

> 核对日期：2026-09-13。保留 KB 编号便于追踪，但“源码已接入”与“运行验收通过”严格区分。本轮未编译、未运行 UE。

## 旧缺口现已在源码接入

### KB-01：Pawn 生成与数据注入——源码已接入

GameMode 在 FinishSpawning 前调用 PawnExtension::SetPawnData。默认 PawnClass 回退仍为 CharacterBase，新增 BP_HeroBase/BP_Hero_Pover 只确认存在；实际 PawnData.PawnClass 与地图覆盖待编辑器验证。

### KB-02：Hero 协调组件——启用并挂载，入口统一待完成

HeroComponent 已是有效类，HeroCharacter 构造创建它；Init State 进入 DataInitialized 时关联 ASC、绑定输入与相机。旧 HeroCharacter PossessedBy/OnRep_PlayerState 直接初始化仍在，应验证并收敛双路径。

### KB-03：输入绑定与消费——主链已接入，条件与解绑仍有缺口

HodgeInputComponent 与 HodgeLocalPlayerBase 已配置；Hero 绑定原生和技能输入；新控制器 PostProcessInput 调用 ProcessAbilityInput。尚需关注：DefaultInputMappings 配置和软引用加载；AddMappingContext 被 bRegisterWithSettings 条件包住；ClearAllMappings 可能清除其他来源映射；Ready 事件并不保证绑定成功；局部 BindHandles 没持久化，额外配置移除为空；AutoRun 切换调用仍注释。

### KB-04：相机入口——源码已接入，资产和效果待验证

Hero 绑定 DetermineCameraModeDelegate；模式选择优先 Ability 覆盖，否则 PawnData.DefaultCameraMode。控制器已选 HodgePlayerCameraManager。CM_Default 新增，内部值未解析。

### KB-06：自定义 EffectContext——代码与配置已接入

HodgeAbilitySystemGlobals::AllocGameplayEffectContext 返回 Hodge Context；DefaultGame.ini 已选择该类。旧“没有 Globals 导致默认 Context 不匹配”结论撤销；运行时类型与旧 DLL/配置覆盖仍需检查。

### KB-13：LocalPlayer 与 Feature Policy——配置已补齐

LocalPlayerClassName 和 GameFeaturesManagerClassName 已指向项目类型。Policy 的具体观察者注册仍是另一个环节，见 KB-07。

## 当前仍应优先处理

### KB-05：基础 AbilitySet 授予——仍未接通

PawnData.AbilitySets 存在，但 PlayerState::SetPawnData 中的 GiveToAbilitySystem 循环仍注释。TagRelationshipMapping 设置已接入，不能将两者合并标为均未完成。优先明确基础技能由哪个权威入口授予、如何防重复与回收，再测试一个技能。

### KB-07：Cue——管理器配置已有，路径和预加载不完整

GlobalGameplayCueManagerClass 已配置；Policy 观察者的添加路径回调主体已启用，但 InitGameFeatureManager 创建 AddGameplayCuePaths 观察者的一行仍注释。注销移除主体也停用；AssetManager 初始化钩子仍占位，常驻 GameplayCueNotifyPaths 未启用。不能把配置类名等同于路径注册和预加载成功。

### KB-08：Health 结算——主体已有，角色死亡衔接仍缺

HealthSet 已有 Damage/Healing 元属性、转换、免疫/调试条件和耗尽广播。CombatCharacter 的 HealthComponent 创建与事件绑定仍注释；CombatComponentBase 无命中/连招主体。剩余工作是技能命中→GE→耗尽→角色死亡/重生，而不是重复恢复已存在的 HealthSet 结算。

### KB-09：敌人 ASC——仍缺完整初始化

EnemyCharacter 仍主要设置移动和 AI 自动控制，没有完整的敌人 ASC 创建与关联。先明确 ASC 所有者，再验证服务器伤害目标和远端状态。

### KB-10：专服启动——仍有提前返回风险

TryDedicatedServerLogin 在默认地图条件满足时返回 true，实际登录和续接回调仍停用；还没有独立 Server Target。PIE 服务端与独立服务器构建必须分开记录。

### KB-11：GameFeature 输入——添加已恢复，撤销与记录待完善

AddInputBinding、AddInputContextMapping 的扩展添加分支均已启用。Hero::RemoveAdditionalInputConfig 为空，额外绑定句柄为局部变量；ContextMapping 的 ControllersAddedTo 记录路径需复核。验收必须覆盖激活、停用、再次激活和多世界，而非只看一次输入出现。

### KB-12：Receiver 与 ASC 退出顺序——仍需修正/验证

CharacterBase::EndPlay 仍发送 GameActorReady 而不是配对移除 Receiver。新控制器 OnUnPossess 先清空 ASC Avatar，可能使 PawnExtension 依赖 Avatar 匹配的清理分支跳过。后者是本次新增静态观察，需按真实生命周期验证。

## 编辑器资产检查步骤

1. 打开 `/Game/Main/Data/DA_Dafult_PawnData`，确认 PawnClass 是目标 Hero 蓝图、InputConfig 指向 `/Game/Main/Input/DA_HodgeInputConfig`、DefaultCameraMode 指向 `/Game/Main/Camera/CM_Default`。这些字段当前确实存在。
2. 打开 `/Game/Main/Character/Hero/BP_Hero_Pover`，检查父类/继承组件、HeroComponent 的 DefaultInputMappings。选择 `/Game/Main/Input/IMC_Default`，核对优先级和 RegisterWithSettings；当前代码 false 会跳过添加，不能误当成只关闭设置注册。
3. 打开 InputConfig，确认 NativeInputActions 的 Move/Look 等标签匹配；打开 IMC 检查键位、轴类型和修饰器。
4. PIE 观察 HODGE-DBG 的 CONFIG、IMC entry、BLOCKED、CALLBACK 和相机绑定日志。期望实际映射存在、回调有值、相机跟随；本轮未执行这些操作。
5. AbilitySets 可填写已有字段，但基础授予循环仍停用，单靠资产配置不会自动授予技能。

## 当前开发顺序

先验收已有玩家输入/相机主链，解决映射条件与实际资源配置；再完成 KB-05，验证可重复激活和结束的测试技能；随后连接敌人、命中和死亡。进入联机/热卸载前覆盖 KB-10～12，另行完善 Cue 路径与预加载。

本页只更新知识，不修改游戏实现。所有未执行的构建、蓝图编译、PIE、联机、Cook/打包保持“未验证”。
