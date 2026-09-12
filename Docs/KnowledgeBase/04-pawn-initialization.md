# PawnExtension 与 Hero 初始化

[返回首页](README.md) · [输入](06-input.md) · [GAS](07-gas.md)

## PawnExtension 已有的职责

组件继承 UPawnComponent 和 IGameFrameworkInitStateInterface，默认复制 PawnData。OnRegister 验证挂载对象和唯一性并注册 Feature；BeginPlay 监听其他组件状态，先进入 Spawned，再尝试推进；EndPlay 解除 ASC 并注销 Feature。

SetPawnData 只接受权威端赋值，不允许已有数据被重复覆盖。成功后 ForceNetUpdate 并重试初始化。客户端 OnRep_PawnData 也会重试。这不是可任意切换装备或职业的数据 setter，动态换数据需要另行定义生命周期。

## 状态推进的实际条件

### Spawned

Pawn 存在即可进入。它不保证 Controller、PlayerState、InputComponent、Mesh 或 ASC 已经准备好。

### DataAvailable

PawnData 必须存在。权威端和本地控制 Pawn 还需要 Controller；模拟代理不按相同方式要求本地控制器。当前 GameMode 未注入数据，是这一步的直接阻塞。

### DataInitialized

PawnExtension 检查该 Pawn 上所有 Feature 是否到达 DataAvailable。它自己的 HandleChangeInitState 在此阶段没有实际装配代码。因此新增 Feature 时必须检查状态依赖是否互相等待。

### GameplayReady

当前由上一阶段直接允许进入，没有全面检查技能、相机和输入完成。把它作为真实可玩信号前，应补足参与组件的职责与条件。

## ASC 接入与解除

InitializeAbilitySystem 的输入是 ASC 和 OwnerActor。它处理已有 ASC、旧 Avatar，保存组件指针，调用 InitAbilityActorInfo，然后广播 OnAbilitySystemInitialized。PawnData 的 TagRelationshipMapping 应用仍在注释中。

UninitializeAbilitySystem 在自己仍为 Avatar 时取消大部分能力（保留 SurvivesDeath 类型）、移除 Cue、清空 Avatar 或 ActorInfo，广播解除事件并释放组件引用。是否清空技能输入缓存要单独核对；当前 ClearAbilityInput 尚未完整串入这条清理链。

不要在服务器强行把仍归属另一个 Pawn 的 ASC 重绑定而忽略旧 Pawn 解除。函数里存在对旧 Avatar 权威性的 ensure，它体现了重生顺序约束。

## 当前 HeroComponent 的准确状态

文件已存在，但 `.h` 和 `.cpp` 全部是 `//` 注释，UCLASS、GENERATED_BODY 和全部函数都没有启用。DefaultEngine.ini 新增类重定向也不会把注释变成真实反射类。

草稿描述了正确的协调方向：等待 PlayerState 与 Controller 配对、本地输入和 LocalPlayer；在数据初始化阶段调用 PawnExtension::InitializeAbilitySystem；初始化输入；绑定相机模式委托。它是迁移素材，不能直接无审查地整体解注释。

草稿仍有待处理点：额外输入移除为空；部分 AutoRun API 被注释；输入映射和设置注册的条件耦合；依赖 include 需要按现有目录修正；缺少有效挂载。它也没有解决控制器调用 ProcessAbilityInput 的问题。

## 推荐接通方式

1. 保持 PawnExtension 管公共 Pawn 数据和 ASC 接口，HeroComponent 管玩家依赖。
2. 在 Hero 构造或明确的组件注入机制中挂载有效 HeroComponent。
3. GameMode 在 FinishSpawning 前给 PawnExtension 设置 PawnData。
4. PlayerState、Controller、InputComponent 任一就绪时均触发重试，避免仅依赖一次 BeginPlay。
5. 将 Hero 的双入口直接 InitAbilityActorInfo 收敛到统一组件入口，保留复制通知驱动状态检查。
6. 定义解除和重生顺序，验证旧 Avatar、输入缓存和相机覆盖均被清理。

验收不能只看日志打印了 Ready：还要证明角色 GetAbilitySystemComponent 与 PlayerState 返回同一 ASC，Owner 是 PlayerState、Avatar 是当前 Pawn。

源码：[PawnExtension](../../Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp)、[Hero 草稿](../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)、[HeroCharacter](../../Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp)。
