# Tests 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityTimelineTests.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Tests/HodgeAbilityTimelineTests.cpp](../../../Source/Hodgepodge/Private/Tests/HodgeAbilityTimelineTests.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityTask_PlayTimeline.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

定义候选（多行签名仅展示首行）：

- L77: `bool FHodgeTimelineValidationTest::RunTest(const FString& Parameters)`
- L115: `bool FHodgeTimelineRejectTest::RunTest(const FString& Parameters)`
- L152: `bool FHodgeTimelineCleanupTest::RunTest(const FString& Parameters)`
