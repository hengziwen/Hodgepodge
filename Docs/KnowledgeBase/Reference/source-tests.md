# Tests 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityDefinitionTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityDefinitionTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)、[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[Component/HodgeComboComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeComboComponent.h)

定义候选（多行签名仅展示首行）：

- L19: `bool FHodgeDefinitionGrantTest::RunTest(const FString& Parameters)`
- L55: `bool FHodgeComboValidationTest::RunTest(const FString& Parameters)`
- L95: `bool FHodgeComboSessionTest::RunTest(const FString& Parameters)`
- L177: `bool FHodgeMontageDurationTest::RunTest(const FString& Parameters)`

## HodgeAbilityTimelineTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAbilityTimelineTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityTimelineTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

定义候选（多行签名仅展示首行）：

- L77: `bool FHodgeTimelineValidationTest::RunTest(const FString& Parameters)`
- L115: `bool FHodgeTimelineRejectTest::RunTest(const FString& Parameters)`
- L157: `bool FHodgeTimelineCleanupTest::RunTest(const FString& Parameters)`

## HodgeTimelineEvaluatorTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeTimelineEvaluatorTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeTimelineEvaluatorTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeTimelineEvaluator.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeTimelineEvaluator.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L8: `bool FHodgeEvaluatorTest::RunTest(const FString& Parameters)`
