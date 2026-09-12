# 当前状态、断点与接通顺序

[返回首页](README.md)

> 本页是 2026-09-10 的静态核对结论。优先级按“最小可玩闭环”排列；不是承诺工期。

## P0：先形成可操作玩家

### KB-01：Pawn 生成与数据注入

证据：GameMode 默认 Pawn 为 CharacterBase；SpawnDefaultPawnAtTransform 的 SetPawnData 块仍注释。影响：配置空时生成基础角色；即使生成 Combat/Hero，PawnExtension 仍拿不到数据。

处理：编辑器确认 PawnClass 指向目标 Hero 蓝图；启用生成前数据注入和错误日志。验收：实际类正确，服务器和客户端 PawnExtension 都拥有正确 PawnData。

### KB-02：Hero 协调组件停用

证据：HeroComponent 两个文件全部行注释，没有有效 UCLASS 和挂载。影响：没有统一协调 PlayerState、输入、相机和 ASC 的节点。

处理：按现有模块依赖恢复可编译的最小组件，挂载 Hero，统一 ASC 入口。验收：各种到达顺序均完成初始化，角色 ASC 与 PlayerState ASC 一致。

### KB-03：输入映射、绑定与消费

证据：默认输入组件仍引擎类型；InputComponent 映射辅助函数空；Hero 绑定停用；ProcessAbilityInput 没有有效调用者。

处理：加入基础 IMC、正确组件类、Move/Look 和能力输入绑定、每帧消费及解除。验收：移动视角正常，按下/持有/释放策略可分别验证，重生不重复触发。

### KB-04：默认相机模式入口

证据：DetermineCameraModeDelegate 没有有效绑定；PawnData 字段已启用但没有有效消费者。

处理：Hero 绑定模式选择，配置第三人称模式，核对 CameraManager 类型。验收：视角随角色移动，控制旋转不被空视图重置。

## P1：让一个技能可运行

### KB-05：AbilitySet 和 TagRelationship 接入

证据：PawnData 字段已启用；PlayerState 授予循环和 PawnExtension 应用关系映射仍注释。

处理：权威端授予，定义去重与回收策略；设置关系映射。验收：Spec 存在且 InputTag 正确，重生/重载不重复授予。

### KB-06：自定义 EffectContext 配套

证据：Ability::MakeEffectContext check 自定义提取结果；没有项目 Globals 分配实现与配置。

处理：补 Globals、分配入口及配置。验收：实际创建的 ScriptStruct 是 Hodge Context，GE Spec 可创建并应用。

### KB-07：自定义 CueManager 未启用

证据：AssetManager 初始化钩子占位，ini 未指向自定义管理器。

处理：按项目需要启用管理器和预加载；先选一个测试 Cue 验证。不能把这个缺口误写为所有原生 Cue 都不可用。

## P1：第一次伤害闭环

### KB-08：伤害与死亡主体缺失

证据：HealthSet::PostGameplayEffectExecute 主体注释；CombatCharacter 的 HealthComponent 创建/绑定注释；CombatComponentBase 无命中主体。

处理：先明确 GE→Health 的契约，再加入权威死亡与表现。验收：伤害数值、零血量事件、死亡状态和重生可观察。

### KB-09：敌人能力系统

证据：Enemy 当前只有移动、AI 自动控制和空 PossessedBy 扩展。

处理：明确敌人 ASC 所有者，创建属性并走组件初始化。验收：服务器能找到敌人 ASC，GE 对目标有效，远端正确看到属性和死亡。

## 联机和扩展前修复

### KB-10：专服 Experience 启动被接管后无后续

TryDedicatedServerLogin 条件满足时返回 true，实际登录回调停用。应提供本地直启或真实异步后续。独立 Server Target 也需补齐。

### KB-11：GameFeature 输入生命周期

AddInputBinding 的添加/绑定分支停用；AddInputContextMapping 的 Controller 添加分支停用；Hero 草稿额外输入移除为空。验收需覆盖激活、停用和再次激活。

### KB-12：Receiver 退出路径

CharacterBase::EndPlay 再发 GameActorReady，缺少与注册配对的移除。建议对照 PlayerStateBase 实现修正，再测试销毁与插件卸载。

### KB-13：本地玩家与 Feature 策略选择

本仓库 Config 未见项目 LocalPlayerClassName 和 GameFeature 策略的显式选择。Hero 草稿需要 HodgeLocalPlayerBase，类型不符合时无法按预期取用；Policy 代码存在也不等于引擎会实例化。需核对实际对象类型并补齐配置。另外 Policy 里的 Cue 路径增删主体仍停用。

## 资产待确认项

DA_Dafult_PawnData 五字段、Experience 默认数据、Hero 蓝图父类/组件/Mesh/AnimClass、InputConfig Tag、IMC 键位/轴修饰、AbilitySet 内容、GE 参数、地图 GameMode 覆盖均需编辑器查看。文件列表只证明存在。

## 实施顺序与停止条件

第一轮做 KB-01～04，并核对 KB-13 的 LocalPlayer 配置，停在“能移动且视角正常”的验收点。第二轮做 KB-05～06，停在“一个按键激活一个可结束的技能”。第三轮做 KB-08～09，停在“服务器伤害和死亡闭环”。扩展联机与插件内容前完成 KB-10～13。随后再考虑复杂动画、AI、装备、UI 和开放世界。

“停在验收点”是开发拆分建议，不表示只完成文档就算实现。所有条目目前仍需实际代码/资产修改及运行验证。
