# GameFeature、Experience 与 Init State：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 以本项目 Experience→PawnData→PawnExtension/Hero 的调用链为入口；GameFeature Action 根据 World/Context 保存资源归属，不另造并行加载器。
2. 参照 Lyra AddAbilities 的 AddToWorld/HandleActorExtension/Reset/RemoveActorAbilities，核实注册、ActorReady、激活和停用的配对关系。
3. Init State 通过 RegisterInitStateFeature、ContinueInitStateChain 和事件推进；先校验依赖，再执行阶段副作用，清理时取消绑定。
4. Experience 已加载时 CallOrRegister 会立即执行；注册回调前其依赖必须已就绪。等待依赖、坏配置和运行失败分别诊断。
5. Lyra AddAbilities、Experience、AbilitySet 属于示例/项目实现，不能假定 GameFeatures 模块自带同名现成功能；不引入 CommonGame/CommonUser。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Plugins/Runtime/ModularGameplay/Source/ModularGameplay/Public/Components/GameFrameworkComponentManager.h:152`：`AddExtensionHandler`。
- `Plugins/Runtime/ModularGameplay/Source/ModularGameplay/Public/Components/GameFrameworkInitStateInterface.h:54`：`ContinueInitStateChain`。
- `Plugins/Runtime/GameFeatures/Source/GameFeatures/Public/GameFeatureAction.h:43`：`OnGameFeatureDeactivating`。

## Lyra 参考

参考根目录：`E:/Project/UProject/study/Epic/LyraStarterGame`。

- `Source/LyraGame/GameFeatures/GameFeatureAction_AddAbilities.cpp`。
- `Source/LyraGame/GameModes/LyraExperienceManagerComponent.cpp`。
- `Source/LyraGame/Character/LyraHeroComponent.cpp`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。

## 项目切入与适配差异

- `Source/Hodgepodge/Private/Component/HodgeExperienceManagerComponent.cpp`：CallOrRegister_OnExperienceLoaded、StartExperienceLoad、OnExperienceFullLoadCompleted、EndPlay。
- `Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp`：在本项目现有 Action 上核查激活与撤销；不要假定这是 Engine 自带的同名类。
- Lyra 的 AddToWorld→HandleActorExtension→AddActorAbilities 与 Reset→RemoveActorAbilities 成对管理扩展请求和授予句柄；本项目保留自身的校验、角色类型和模块边界。
