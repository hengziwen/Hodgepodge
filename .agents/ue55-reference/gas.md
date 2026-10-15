# GAS 能力、属性和效果：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 本项目玩家 ASC 由 AHodgePlayerState 持有，Pawn 是 Avatar；怪物使用自身 ASC。先追踪 PawnExtension/Hero 的 Init State，避免复制通用示例再造一条初始化链。
2. 服务器授予和撤销分别记录 AbilitySpec、GameplayEffect 与 AttributeSet 句柄；重复初始化不得重复授予或补满持久资源。
3. UE5.5.4 已支持 GetAssetTags()/构造期 SetAssetTags()、GetDynamicSpecSourceTags()；不要简单把 5.8 弃用表全部反向替换成旧字段。
4. 自定义能力沿用项目 InstancedPerActor；引擎默认 InstancedPerExecution 不等于项目默认。Commit 失败、取消、EndAbility 与任务完成均要清理资源。
5. 先核实 Owner/Avatar、Mixed 所需拥有链、客户端 PlayerState 到达、预测键与迟到 TargetData；恢复 Pawn 时禁止旧回调清理新 Avatar。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/Abilities/GameplayAbility.h:540`：`SetAssetTags`。
- `Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/GameplayAbilitySpec.h:239`：`GetDynamicSpecSourceTags`。
- `Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/AbilitySystemComponent.h:1545`：`InitAbilityActorInfo`。
- `Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/GameplayEffectTypes.h:121`：`MultiplyAdditive`。

## Lyra 参考

参考根目录：`E:/Project/UProject/study/Epic/LyraStarterGame`。

- `Source/LyraGame/AbilitySystem/LyraAbilitySet.cpp`。
- `Source/LyraGame/Player/LyraPlayerState.cpp`。
- `Source/LyraGame/Character/LyraPawnExtensionComponent.cpp`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。

## 项目切入与适配差异

- `Source/Hodgepodge/Private/Core/PlayState/HodgePlayerState.cpp`：构造 ASC/Mixed、SetPawnData 与 Experience 回调。玩家持久 Owner 保留在 PlayerState。
- `Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp`：HandleChangeInitState 在 DataAvailable→DataInitialized 中通过 PawnExtension 绑定玩家 ASC。
- `Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp`：InitializeAbilitySystem/UninitializeAbilitySystem；当前实现对旧 Avatar 的通知处理与 Lyra 有适配差异，修改前阅读完整清理路径。
- `Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp`：ValidateForGrant/ValidateBatchForGrant、GiveToAbilitySystem、TakeFromAbilitySystem；已有批量校验和授予句柄机制，不能另造通用授予服务。
