# 扫描快照

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

## 生成信息

- UTC：2026-09-27T14:09:53.862926+00:00
- Git HEAD：`ed12e62ce4b25cac7d1bbec1c4b407461dd29a9b`
- 源文件（h/cpp/cs）：244
- Content/Main 文件：44
- 原生标签注册条目：186
- 漂移跟踪文件：304

## 生成时已有的受 Git 跟踪修改

```text
M Config/DefaultEngine.ini
 M Docs/Design/lyra-ui-migration-plan.md
 M Docs/KnowledgeBase/Reference/README.md
 M Docs/KnowledgeBase/Reference/assets.md
 M Docs/KnowledgeBase/Reference/config.md
 M Docs/KnowledgeBase/Reference/gameplay-tags.md
 M Docs/KnowledgeBase/Reference/snapshot.json
 M Docs/KnowledgeBase/Reference/source-abilitysystem.md
 M Docs/KnowledgeBase/Reference/source-character.md
 M Docs/KnowledgeBase/Reference/source-codextext.md
 M Docs/KnowledgeBase/Reference/source-component.md
 M Docs/KnowledgeBase/Reference/source-core.md
 M Docs/KnowledgeBase/Reference/source-data.md
 M Docs/KnowledgeBase/Reference/source-index.md
 M Docs/KnowledgeBase/tools/kb.py
 M Docs/README.md
 M README.md
 M Source/Hodgepodge/Hodgepodge.Build.cs
 M Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp
 M Source/Hodgepodge/Private/Component/HodgeHealthComponent.cpp
 M Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp
D  Source/Hodgepodge/Private/Core/HUD/HodgeHUDBase.cpp
 M Source/Hodgepodge/Private/Core/PlayerController/HodgePlayerController.cpp
AM Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp
AM Source/Hodgepodge/Private/UI/Extension/UIExtensionSystem.cpp
 M Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h
AM Source/Hodgepodge/Public/Core/HUD/HodgeHUD.h
D  Source/Hodgepodge/Public/Core/HUD/HodgeHUDBase.h
 M Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h
AD Source/Hodgepodge/Public/UI/Basic/MaterialProgressBar.cpp
AM Source/Hodgepodge/Public/UI/Basic/MaterialProgressBar.h
AM Source/Hodgepodge/Public/UI/Common/HodgeBoundActionButton.h
AM Source/Hodgepodge/Public/UI/Common/HodgeListView.h
AM Source/Hodgepodge/Public/UI/Common/HodgeTabButtonBase.h
AM Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h
AM Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h
AM Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory_Class.h
AD Source/Hodgepodge/Public/UI/Common/LyraBoundActionButton.cpp
AD Source/Hodgepodge/Public/UI/Common/LyraListView.cpp
AD Source/Hodgepodge/Public/UI/Common/LyraTabButtonBase.cpp
AD Source/Hodgepodge/Public/UI/Common/LyraTabListWidgetBase.cpp
AD Source/Hodgepodge/Public/UI/Common/LyraWidgetFactory.cpp
AD Source/Hodgepodge/Public/UI/Common/LyraWidgetFactory_Class.cpp
AM Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h
AM Source/Hodgepodge/Public/UI/Extension/UIExtensionSystem.h
AM Source/Hodgepodge/Public/UI/Foundation/HodgeActionWidget.h
AM Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h
AM Source/Hodgepodge/Public/UI/Foundation/HodgeConfirmationScreen.h
AM Source/Hodgepodge/Public/UI/Foundation/HodgeControllerDisconnectedScreen.h
AM Source/Hodgepodge/Public/UI/Foundation/HodgeLoadingScreenSubsystem.h
AD Source/Hodgepodge/Public/UI/Foundation/LyraActionWidget.cpp
AD Source/Hodgepodge/Public/UI/Foundation/LyraButtonBase.cpp
AD Source/Hodgepodge/Public/UI/Foundation/LyraConfirmationScreen.cpp
AD Source/Hodgepodge/Public/UI/Foundation/LyraControllerDisconnectedScreen.cpp
AD Source/Hodgepodge/Public/UI/Foundation/LyraLoadingScreenSubsystem.cpp
AD Source/Hodgepodge/Public/UI/Frontend/ApplyFrontendPerfSettingsAction.cpp
A  Source/Hodgepodge/Public/UI/Frontend/ApplyFrontendPerfSettingsAction.h
AM Source/Hodgepodge/Public/UI/Frontend/HodgeFrontendStateComponent.h
AM Source/Hodgepodge/Public/UI/Frontend/HodgeLobbyBackground.h
AD Source/Hodgepodge/Public/UI/Frontend/LyraFrontendStateComponent.cpp
AD Source/Hodgepodge/Public/UI/Frontend/LyraLobbyBackground.cpp
AM Source/Hodgepodge/Public/UI/HodgeHUDLayout.h
AM Source/Hodgepodge/Public/UI/HodgeJoystickWidget.h
AM Source/Hodgepodge/Public/UI/HodgeSettingScreen.h
AM Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h
AM Source/Hodgepodge/Public/UI/HodgeTaggedWidget.h
AM Source/Hodgepodge/Public/UI/HodgeTouchRegion.h
AD Source/Hodgepodge/Public/UI/HogdeActivatableWidget.h
AM Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h
AM Source/Hodgepodge/Public/UI/IndicatorSystem/IActorIndicatorWidget.h
AD Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.cpp
AM Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h
AD Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.cpp
AM Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.h
AD Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLibrary.cpp
AM Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLibrary.h
AD Source/Hodgepodge/Public/UI/IndicatorSystem/LyraIndicatorManagerComponent.cpp
AD Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.cpp
AM Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h
AD Source/Hodgepodge/Public/UI/LyraActivatableWidget.cpp
AD Source/Hodgepodge/Public/UI/LyraGameViewportClient.cpp
AD Source/Hodgepodge/Public/UI/LyraGameViewportClient.h
AD Source/Hodgepodge/Public/UI/LyraHUD.cpp
AD Source/Hodgepodge/Public/UI/LyraHUDLayout.cpp
AD Source/Hodgepodge/Public/UI/LyraJoystickWidget.cpp
AD Source/Hodgepodge/Public/UI/LyraSettingScreen.cpp
AD Source/Hodgepodge/Public/UI/LyraSimulatedInputWidget.cpp
AD Source/Hodgepodge/Public/UI/LyraTaggedWidget.cpp
AD Source/Hodgepodge/Public/UI/LyraTouchRegion.cpp
AM Source/Hodgepodge/Public/UI/PerformanceStats/HodgePerfStatContainerBase.h
AD Source/Hodgepodge/Public/UI/PerformanceStats/LyraPerfStatContainerBase.cpp
AD Source/Hodgepodge/Public/UI/PerformanceStats/LyraPerfStatWidgetBase.cpp
AD Source/Hodgepodge/Public/UI/PerformanceStats/LyraPerfStatWidgetBase.h
AM Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h
AM Source/Hodgepodge/Public/UI/Subsystem/HodgeUIMessaging.h
AD Source/Hodgepodge/Public/UI/Subsystem/LyraUIManagerSubsystem.cpp
AD Source/Hodgepodge/Public/UI/Subsystem/LyraUIMessaging.cpp
AD Source/Hodgepodge/Public/UI/Weapons/CircumferenceMarkerWidget.cpp
A  Source/Hodgepodge/Public/UI/Weapons/CircumferenceMarkerWidget.h
AD Source/Hodgepodge/Public/UI/Weapons/HitMarkerConfirmationWidget.cpp
AM Source/Hodgepodge/Public/UI/Weapons/HitMarkerConfirmationWidget.h
AM Source/Hodgepodge/Public/UI/Weapons/HodgeReticleWidgetBase.h
AM Source/Hodgepodge/Public/UI/Weapons/HodgeWeaponUserInterface.h
AD Source/Hodgepodge/Public/UI/Weapons/LyraReticleWidgetBase.cpp
AD Source/Hodgepodge/Public/UI/Weapons/LyraWeaponUserInterface.cpp
AD Source/Hodgepodge/Public/UI/Weapons/SCircumferenceMarkerWidget.cpp
A  Source/Hodgepodge/Public/UI/Weapons/SCircumferenceMarkerWidget.h
AD Source/Hodgepodge/Public/UI/Weapons/SHitMarkerConfirmationWidget.cpp
AM Source/Hodgepodge/Public/UI/Weapons/SHitMarkerConfirmationWidget.h
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

没有运行 Unreal 编译、PIE、打包或蓝图数据解析。Content/Main 之外只统计顶层文件数量，不做引用结论。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
