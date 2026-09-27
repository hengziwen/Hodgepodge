# UI 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## MaterialProgressBar.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Basic/MaterialProgressBar.cpp](../../../Source/Hodgepodge/Private/UI/Basic/MaterialProgressBar.cpp)

项目内直接 include（不是运行调用关系）：[UI/Basic/MaterialProgressBar.h](../../../Source/Hodgepodge/Public/UI/Basic/MaterialProgressBar.h)

定义候选（多行签名仅展示首行）：

- L11: `void UMaterialProgressBar::SynchronizeProperties()`
- L80: `void UMaterialProgressBar::OnWidgetRebuilt()`
- L139: `void UMaterialProgressBar::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)`
- L149: `void UMaterialProgressBar::SetProgress(float Progress)`
- L157: `void UMaterialProgressBar::SetStartProgress(float StartProgress)`
- L165: `void UMaterialProgressBar::SetColorA(FLinearColor ColorA)`
- L173: `void UMaterialProgressBar::SetColorB(FLinearColor ColorB)`
- L181: `void UMaterialProgressBar::SetColorBackground(FLinearColor ColorBackground)`
- L189: `void UMaterialProgressBar::AnimateProgressFromStart(float Start, float End, float AnimSpeed)`
- L196: `void UMaterialProgressBar::AnimateProgressFromCurrent(float End, float AnimSpeed)`
- L208: `void UMaterialProgressBar::SetProgress_Internal(float Progress)`
- L217: `void UMaterialProgressBar::SetStartProgress_Internal(float StartProgress)`
- L226: `void UMaterialProgressBar::SetColorA_Internal(FLinearColor ColorA)`
- L235: `void UMaterialProgressBar::SetColorB_Internal(FLinearColor ColorB)`
- L244: `void UMaterialProgressBar::SetColorBackground_Internal(FLinearColor ColorBackground)`
- L253: `UMaterialInstanceDynamic* UMaterialProgressBar::GetBarDynamicMaterial() const`

## HodgeBoundActionButton.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeBoundActionButton.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeBoundActionButton.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeBoundActionButton.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeBoundActionButton.h)

定义候选（多行签名仅展示首行）：

- L12: `void UHodgeBoundActionButton::NativeConstruct()`
- L23: `void UHodgeBoundActionButton::HandleInputMethodChanged(ECommonInputType NewInputMethod)`

## HodgeListView.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeListView.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeListView.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeListView.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeListView.h)、[UI/Common/HodgeWidgetFactory.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h)

定义候选（多行签名仅展示首行）：

- L14: `UHodgeListView::UHodgeListView(const FObjectInitializer& ObjectInitializer)`
- L21: `void UHodgeListView::ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const`
- L35: `UUserWidget& UHodgeListView::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass,`

## HodgeTabButtonBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeTabButtonBase.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeTabButtonBase.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeTabButtonBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabButtonBase.h)、[UI/Common/HodgeTabListWidgetBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h)

定义候选（多行签名仅展示首行）：

- L13: `void UHodgeTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)`
- L21: `void UHodgeTabButtonBase::SetIconBrush(const FSlateBrush& Brush)`
- L29: `void UHodgeTabButtonBase::SetTabLabelInfo_Implementation(const FHodgeTabDescriptor& TabLabelInfo)`

## HodgeTabListWidgetBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeTabListWidgetBase.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeTabListWidgetBase.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeTabListWidgetBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h)

定义候选（多行签名仅展示首行）：

- L10: `void UHodgeTabListWidgetBase::NativeOnInitialized()`
- L15: `void UHodgeTabListWidgetBase::NativeConstruct()`
- L22: `void UHodgeTabListWidgetBase::NativeDestruct()`
- L36: `bool UHodgeTabListWidgetBase::GetPreregisteredTabInfo(const FName TabNameId, FHodgeTabDescriptor& OutTabInfo)`
- L53: `void UHodgeTabListWidgetBase::SetTabHiddenState(FName TabNameId, bool bHidden)`
- L65: `bool UHodgeTabListWidgetBase::RegisterDynamicTab(const FHodgeTabDescriptor& TabDescriptor)`
- L78: `void UHodgeTabListWidgetBase::HandlePreLinkedSwitcherChanged()`
- L92: `void UHodgeTabListWidgetBase::HandlePostLinkedSwitcherChanged()`
- L103: `void UHodgeTabListWidgetBase::HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton)`
- L132: `bool UHodgeTabListWidgetBase::IsFirstTabActive() const`
- L142: `bool UHodgeTabListWidgetBase::IsLastTabActive() const`
- L152: `bool UHodgeTabListWidgetBase::IsTabVisible(FName TabId)`
- L165: `int32 UHodgeTabListWidgetBase::GetVisibleTabCount()`
- L180: `void UHodgeTabListWidgetBase::SetupTabs()`

## HodgeWidgetFactory.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeWidgetFactory.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h)

定义候选（多行签名仅展示首行）：

- L10: `TSubclassOf<UUserWidget> UHodgeWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const`

## HodgeWidgetFactory_Class.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory_Class.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory_Class.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeWidgetFactory_Class.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory_Class.h)

定义候选（多行签名仅展示首行）：

- L9: `TSubclassOf<UUserWidget> UHodgeWidgetFactory_Class::FindWidgetClassForData_Implementation(const UObject* Data) const`

## UIExtensionPointWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp](../../../Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/Extension/UIExtensionPointWidget.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

定义候选（多行签名仅展示首行）：

- L39: `UUIExtensionPointWidget::UUIExtensionPointWidget(const FObjectInitializer& ObjectInitializer)`
- L45: `void UUIExtensionPointWidget::ReleaseSlateResources(bool bReleaseChildren)`
- L56: `TSharedRef<SWidget> UUIExtensionPointWidget::RebuildWidget()`
- L123: `void UUIExtensionPointWidget::ResetExtensionPoint()`
- L142: `void UUIExtensionPointWidget::RegisterExtensionPoint()`
- L186: `void UUIExtensionPointWidget::RegisterExtensionPointForPlayerState(UHodgeLocalPlayerBase* LocalPlayer,`
- L214: `void UUIExtensionPointWidget::OnAddOrRemoveExtension(EUIExtensionAction Action, const FUIExtensionRequest& Request)`
- L297: `void UUIExtensionPointWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const`

## UIExtensionSystem.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Extension/UIExtensionSystem.cpp](../../../Source/Hodgepodge/Private/UI/Extension/UIExtensionSystem.cpp)

项目内直接 include（不是运行调用关系）：[UI/Extension/UIExtensionSystem.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionSystem.h)

定义候选（多行签名仅展示首行）：

- L24: `void FUIExtensionPointHandle::Unregister()`
- L38: `void FUIExtensionHandle::Unregister()`
- L52: `bool FUIExtensionPoint::DoesExtensionPassContract(const FUIExtension* Extension) const`
- L101: `void UUIExtensionSubsystem::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)`
- L136: `void UUIExtensionSubsystem::Initialize(FSubsystemCollectionBase& Collection)`
- L143: `void UUIExtensionSubsystem::Deinitialize()`
- L150: `FUIExtensionPointHandle UUIExtensionSubsystem::RegisterExtensionPoint(const FGameplayTag& ExtensionPointTag,`
- L162: `FUIExtensionPointHandle UUIExtensionSubsystem::RegisterExtensionPointForContext(`
- L225: `FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsWidget(const FGameplayTag& ExtensionPointTag,`
- L235: `FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsWidgetForContext(`
- L244: `FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsData(const FGameplayTag& ExtensionPointTag,`
- L307: `void UUIExtensionSubsystem::NotifyExtensionPointOfExtensions(TSharedPtr<FUIExtensionPoint>& ExtensionPoint)`
- L357: `void UUIExtensionSubsystem::NotifyExtensionPointsOfExtension(EUIExtensionAction Action,`
- L412: `void UUIExtensionSubsystem::UnregisterExtension(const FUIExtensionHandle& ExtensionHandle)`
- L466: `void UUIExtensionSubsystem::UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle)`
- L502: `FUIExtensionRequest UUIExtensionSubsystem::CreateExtensionRequest(const TSharedPtr<FUIExtension>& Extension)`
- L526: `FUIExtensionPointHandle UUIExtensionSubsystem::K2_RegisterExtensionPoint(`
- L544: `FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsWidget(FGameplayTag ExtensionPointTag,`
- L553: `FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsWidgetForContext(`
- L572: `FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsData(FGameplayTag ExtensionPointTag, UObject* Data,`
- L580: `FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsDataForContext(`
- L600: `void UUIExtensionHandleFunctions::Unregister(FUIExtensionHandle& Handle)`
- L606: `bool UUIExtensionHandleFunctions::IsValid(FUIExtensionHandle& Handle)`
- L614: `void UUIExtensionPointHandleFunctions::Unregister(FUIExtensionPointHandle& Handle)`
- L620: `bool UUIExtensionPointHandleFunctions::IsValid(FUIExtensionPointHandle& Handle)`

## HodgeActionWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgeActionWidget.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeActionWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgeActionWidget.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeActionWidget.h)

定义候选（多行签名仅展示首行）：

- L10: `FSlateBrush UHodgeActionWidget::GetIcon() const`
- L35: `UEnhancedInputLocalPlayerSubsystem* UHodgeActionWidget::GetEnhancedInputSubsystem() const`

## HodgeButtonBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgeButtonBase.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeButtonBase.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgeButtonBase.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h)

定义候选（多行签名仅展示首行）：

- L8: `void UHodgeButtonBase::NativePreConstruct()`
- L16: `void UHodgeButtonBase::UpdateInputActionWidget()`
- L24: `void UHodgeButtonBase::SetButtonText(const FText& InText)`
- L31: `void UHodgeButtonBase::RefreshButtonText()`
- L49: `void UHodgeButtonBase::OnInputMethodChanged(ECommonInputType CurrentInputType)`

## HodgeConfirmationScreen.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgeConfirmationScreen.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeConfirmationScreen.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeControllerDisconnectedScreen.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgeControllerDisconnectedScreen.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeControllerDisconnectedScreen.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgeControllerDisconnectedScreen.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeControllerDisconnectedScreen.h)

定义候选（多行签名仅展示首行）：

- L23: `UHodgeControllerDisconnectedScreen::UHodgeControllerDisconnectedScreen(const FObjectInitializer& ObjectInitializer)`
- L30: `void UHodgeControllerDisconnectedScreen::NativeOnActivated()`
- L64: `bool UHodgeControllerDisconnectedScreen::ShouldDisplayChangeUserButton() const`
- L79: `void UHodgeControllerDisconnectedScreen::HandleChangeUserClicked()`
- L97: `void UHodgeControllerDisconnectedScreen::HandleChangeUserCompleted(const FPlatformUserSelectionCompleteParams& Params)`

## HodgeLoadingScreenSubsystem.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgeLoadingScreenSubsystem.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeLoadingScreenSubsystem.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgeLoadingScreenSubsystem.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeLoadingScreenSubsystem.h)

定义候选（多行签名仅展示首行）：

- L14: `UHodgeLoadingScreenSubsystem::UHodgeLoadingScreenSubsystem()`
- L18: `void UHodgeLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)`
- L28: `TSubclassOf<UUserWidget> UHodgeLoadingScreenSubsystem::GetLoadingScreenContentWidget() const`

## ApplyFrontendPerfSettingsAction.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Frontend/ApplyFrontendPerfSettingsAction.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/ApplyFrontendPerfSettingsAction.cpp)

项目内直接 include（不是运行调用关系）：[UI/Frontend/ApplyFrontendPerfSettingsAction.h](../../../Source/Hodgepodge/Public/UI/Frontend/ApplyFrontendPerfSettingsAction.h)

定义候选（多行签名仅展示首行）：

- L23: `void UApplyFrontendPerfSettingsAction::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)`
- L32: `void UApplyFrontendPerfSettingsAction::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`

## HodgeFrontendStateComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Frontend/HodgeFrontendStateComponent.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/HodgeFrontendStateComponent.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeLobbyBackground.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Frontend/HodgeLobbyBackground.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/HodgeLobbyBackground.cpp)

项目内直接 include（不是运行调用关系）：[UI/Frontend/HodgeLobbyBackground.h](../../../Source/Hodgepodge/Public/UI/Frontend/HodgeLobbyBackground.h)

定义候选（多行签名仅展示首行）：


## HodgeActivatableWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeActivatableWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeActivatableWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeActivatableWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h)

定义候选（多行签名仅展示首行）：

- L17: `UHodgeActivatableWidget::UHodgeActivatableWidget(const FObjectInitializer& ObjectInitializer)`
- L24: `TOptional<FUIInputConfig> UHodgeActivatableWidget::GetDesiredInputConfig() const`
- L62: `void UHodgeActivatableWidget::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree,`

## HodgeGameViewportClient.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeGameViewportClient.cpp](../../../Source/Hodgepodge/Private/UI/HodgeGameViewportClient.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeHUDLayout.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp](../../../Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeHUDLayout.h](../../../Source/Hodgepodge/Public/UI/HodgeHUDLayout.h)、[UI/Foundation/HodgeControllerDisconnectedScreen.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeControllerDisconnectedScreen.h)、[UI/HodgeActivatableWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h)

定义候选（多行签名仅展示首行）：

- L77: `UHodgeHUDLayout::UHodgeHUDLayout(const FObjectInitializer& ObjectInitializer)`
- L90: `void UHodgeHUDLayout::NativeOnInitialized()`
- L139: `void UHodgeHUDLayout::NativeDestruct()`
- L169: `void UHodgeHUDLayout::EnsureMenuLayerStack()`
- L199: `void UHodgeHUDLayout::HandleEscapeAction()`
- L247: `void UHodgeHUDLayout::HandleInputDeviceConnectionChanged(EInputDeviceConnectionState NewConnectionState,`
- L271: `void UHodgeHUDLayout::HandleInputDevicePairingChanged(FInputDeviceId InputDeviceId, FPlatformUserId NewUserPlatformId,`
- L295: `bool UHodgeHUDLayout::ShouldPlatformDisplayControllerDisconnectScreen() const`
- L328: `void UHodgeHUDLayout::NotifyControllerStateChangeForDisconnectScreen()`
- L369: `void UHodgeHUDLayout::ProcessControllerDevicesHavingChangedForDisconnectScreen()`
- L442: `void UHodgeHUDLayout::DisplayControllerDisconnectedMenu_Implementation()`
- L481: `void UHodgeHUDLayout::HideControllerDisconnectedMenu_Implementation()`

## HodgeJoystickWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeJoystickWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeJoystickWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeJoystickWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeJoystickWidget.h)

定义候选（多行签名仅展示首行）：

- L12: `UHodgeJoystickWidget::UHodgeJoystickWidget(const FObjectInitializer& ObjectInitializer)`
- L18: `FReply UHodgeJoystickWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L32: `FReply UHodgeJoystickWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L45: `FReply UHodgeJoystickWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L51: `void UHodgeJoystickWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)`
- L57: `void UHodgeJoystickWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)`
- L76: `void UHodgeJoystickWidget::HandleTouchDelta(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L101: `void UHodgeJoystickWidget::StopInputSimulation()`

## HodgeSettingScreen.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeSettingScreen.cpp](../../../Source/Hodgepodge/Private/UI/HodgeSettingScreen.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeSimulatedInputWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeSimulatedInputWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeSimulatedInputWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeSimulatedInputWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h)

定义候选（多行签名仅展示首行）：

- L11: `UHodgeSimulatedInputWidget::UHodgeSimulatedInputWidget(const FObjectInitializer& ObjectInitializer)`
- L18: `const FText UHodgeSimulatedInputWidget::GetPaletteCategory()`
- L24: `void UHodgeSimulatedInputWidget::NativeConstruct()`
- L38: `void UHodgeSimulatedInputWidget::NativeDestruct()`
- L48: `FReply UHodgeSimulatedInputWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L55: `UEnhancedInputLocalPlayerSubsystem* UHodgeSimulatedInputWidget::GetEnhancedInputSubsystem() const`
- L67: `UEnhancedPlayerInput* UHodgeSimulatedInputWidget::GetPlayerInput() const`
- L76: `void UHodgeSimulatedInputWidget::InputKeyValue(const FVector& Value)`
- L111: `void UHodgeSimulatedInputWidget::InputKeyValue2D(const FVector2D& Value)`
- L116: `void UHodgeSimulatedInputWidget::FlushSimulatedInput()`
- L124: `void UHodgeSimulatedInputWidget::QueryKeyToSimulate()`
- L140: `void UHodgeSimulatedInputWidget::OnControlMappingsRebuilt()`

## HodgeTaggedWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeTaggedWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeTaggedWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeTaggedWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeTaggedWidget.h)

定义候选（多行签名仅展示首行）：

- L13: `UHodgeTaggedWidget::UHodgeTaggedWidget(const FObjectInitializer& ObjectInitializer)`
- L19: `void UHodgeTaggedWidget::NativeConstruct()`
- L44: `void UHodgeTaggedWidget::NativeDestruct()`
- L61: `void UHodgeTaggedWidget::SetVisibility(ESlateVisibility InVisibility)`
- L120: `void UHodgeTaggedWidget::OnWatchedTagsChanged()`

## HodgeTouchRegion.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeTouchRegion.cpp](../../../Source/Hodgepodge/Private/UI/HodgeTouchRegion.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeTouchRegion.h](../../../Source/Hodgepodge/Public/UI/HodgeTouchRegion.h)

定义候选（多行签名仅展示首行）：

- L11: `FReply UHodgeTouchRegion::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L17: `FReply UHodgeTouchRegion::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L26: `FReply UHodgeTouchRegion::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L32: `void UHodgeTouchRegion::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)`

## HodgeIndicatorManagerComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/IndicatorSystem/HodgeIndicatorManagerComponent.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/HodgeIndicatorManagerComponent.cpp)

项目内直接 include（不是运行调用关系）：[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)、[UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)

定义候选（多行签名仅展示首行）：

- L14: `UHodgeIndicatorManagerComponent::UHodgeIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)`
- L28: `UHodgeIndicatorManagerComponent* UHodgeIndicatorManagerComponent::GetComponent(AController* Controller)`
- L43: `void UHodgeIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)`
- L58: `void UHodgeIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)`

## IndicatorDescriptor.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorDescriptor.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorDescriptor.cpp)

项目内直接 include（不是运行调用关系）：[UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)、[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L21: `bool FIndicatorProjection::Project(const UIndicatorDescriptor& IndicatorDescriptor,`
- L254: `void UIndicatorDescriptor::SetIndicatorManagerComponent(UHodgeIndicatorManagerComponent* InManager)`
- L267: `void UIndicatorDescriptor::UnregisterIndicator()`

## IndicatorLayer.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLayer.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLayer.cpp)

项目内直接 include（不是运行调用关系）：[UI/IndicatorSystem/IndicatorLayer.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.h)、[UI/IndicatorSystem/SActorCanvas.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h)

定义候选（多行签名仅展示首行）：

- L24: `UIndicatorLayer::UIndicatorLayer(const FObjectInitializer& ObjectInitializer)`
- L37: `void UIndicatorLayer::ReleaseSlateResources(bool bReleaseChildren)`
- L48: `TSharedRef<SWidget> UIndicatorLayer::RebuildWidget()`

## IndicatorLibrary.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLibrary.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLibrary.cpp)

项目内直接 include（不是运行调用关系）：[UI/IndicatorSystem/IndicatorLibrary.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLibrary.h)、[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L16: `UIndicatorLibrary::UIndicatorLibrary()`
- L21: `UHodgeIndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)`

## SActorCanvas.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/IndicatorSystem/SActorCanvas.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/SActorCanvas.cpp)

项目内直接 include（不是运行调用关系）：[UI/IndicatorSystem/SActorCanvas.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h)、[UI/IndicatorSystem/IActorIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IActorIndicatorWidget.h)、[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)、[UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)

定义候选（多行签名仅展示首行）：

- L207: `void SActorCanvas::Construct(const FArguments& InArgs, const FLocalPlayerContext& InLocalPlayerContext,`
- L253: `EActiveTimerReturnType SActorCanvas::UpdateCanvas(double InCurrentTime, float InDeltaTime)`
- L484: `void SActorCanvas::SetShowAnyIndicators(bool bIndicators)`
- L506: `void SActorCanvas::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const`
- L796: `int32 SActorCanvas::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,`
- L858: `SActorCanvas::~SActorCanvas()`
- L876: `FString SActorCanvas::GetReferencerName() const`
- L882: `void SActorCanvas::AddReferencedObjects(FReferenceCollector& Collector)`
- L890: `void SActorCanvas::OnIndicatorAdded(UIndicatorDescriptor* Indicator)`
- L904: `void SActorCanvas::OnIndicatorRemoved(UIndicatorDescriptor* Indicator)`
- L918: `void SActorCanvas::AddIndicatorForEntry(UIndicatorDescriptor* Indicator)`
- L955: `void SActorCanvas::OnIndicatorClassLoaded(TWeakObjectPtr<UIndicatorDescriptor> IndicatorPtr)`
- L1014: `void SActorCanvas::RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator)`
- L1047: `SActorCanvas::FScopedWidgetSlotArguments SActorCanvas::AddActorSlot(UIndicatorDescriptor* Indicator)`
- L1068: `int32 SActorCanvas::RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget)`
- L1093: `void SActorCanvas::GetOffsetAndSize(const UIndicatorDescriptor* Indicator,`
- L1170: `void SActorCanvas::UpdateActiveTimer()`

## HodgePerfStatContainerBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatContainerBase.cpp](../../../Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatContainerBase.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgePerfStatWidgetBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatWidgetBase.cpp](../../../Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatWidgetBase.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeUIManagerSubsystem.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Subsystem/HodgeUIManagerSubsystem.cpp](../../../Source/Hodgepodge/Private/UI/Subsystem/HodgeUIManagerSubsystem.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeUIMessaging.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Subsystem/HodgeUIMessaging.cpp](../../../Source/Hodgepodge/Private/UI/Subsystem/HodgeUIMessaging.cpp)

**全部为注释或空白；无有效声明/实现。**

## CircumferenceMarkerWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Weapons/CircumferenceMarkerWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/CircumferenceMarkerWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/Weapons/CircumferenceMarkerWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/CircumferenceMarkerWidget.h)

定义候选（多行签名仅展示首行）：

- L10: `UCircumferenceMarkerWidget::UCircumferenceMarkerWidget(const FObjectInitializer& ObjectInitializer)`
- L17: `void UCircumferenceMarkerWidget::ReleaseSlateResources(bool bReleaseChildren)`
- L24: `TSharedRef<SWidget> UCircumferenceMarkerWidget::RebuildWidget()`
- L34: `void UCircumferenceMarkerWidget::SynchronizeProperties()`
- L42: `void UCircumferenceMarkerWidget::SetRadius(float InRadius)`

## HitMarkerConfirmationWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Weapons/HitMarkerConfirmationWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/HitMarkerConfirmationWidget.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeReticleWidgetBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Weapons/HodgeReticleWidgetBase.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/HodgeReticleWidgetBase.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeWeaponUserInterface.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Weapons/HodgeWeaponUserInterface.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/HodgeWeaponUserInterface.cpp)

**全部为注释或空白；无有效声明/实现。**

## SCircumferenceMarkerWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Weapons/SCircumferenceMarkerWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/SCircumferenceMarkerWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/Weapons/SCircumferenceMarkerWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/SCircumferenceMarkerWidget.h)

定义候选（多行签名仅展示首行）：

- L13: `SCircumferenceMarkerWidget::SCircumferenceMarkerWidget()`
- L17: `void SCircumferenceMarkerWidget::Construct(const FArguments& InArgs)`
- L26: `FSlateRenderTransform SCircumferenceMarkerWidget::GetMarkerRenderTransform(`
- L55: `int32 SCircumferenceMarkerWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,`
- L92: `FVector2D SCircumferenceMarkerWidget::ComputeDesiredSize(float) const`
- L100: `void SCircumferenceMarkerWidget::SetRadius(float NewRadius)`
- L109: `void SCircumferenceMarkerWidget::SetMarkerList(TArray<FCircumferenceMarkerEntry>& NewMarkerList)`

## SHitMarkerConfirmationWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Weapons/SHitMarkerConfirmationWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/SHitMarkerConfirmationWidget.cpp)

**全部为注释或空白；无有效声明/实现。**

## MaterialProgressBar.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Basic/MaterialProgressBar.h](../../../Source/Hodgepodge/Public/UI/Basic/MaterialProgressBar.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CommonUserWidget.h"
   7: #include "MaterialProgressBar.generated.h"
   9: class UImage;
  10: class UMaterialInstanceDynamic;
  11: class UMaterialInterface;
  12: class UWidgetAnimation;
  14: UCLASS(Abstract, meta = (DisableNativeTick))
  15: class UMaterialProgressBar : public UCommonUserWidget
  16: {
  17: 	GENERATED_BODY()
  19: protected:
  20: 	virtual void SynchronizeProperties() override;
  22: #if WITH_EDITOR
  23: 	virtual void OnWidgetRebuilt() override;
  24: #endif
  26: 	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
  28: public:
  29: 	UFUNCTION(BlueprintCallable)
  30: 	void SetProgress(float Progress);
  32: 	UFUNCTION(BlueprintCallable)
  33: 	void SetStartProgress(float StartProgress);
  35: 	UFUNCTION(BlueprintCallable)
  36: 	void SetColorA(FLinearColor ColorA);
  38: 	UFUNCTION(BlueprintCallable)
  39: 	void SetColorB(FLinearColor ColorB);
  41: 	UFUNCTION(BlueprintCallable)
  42: 	void SetColorBackground(FLinearColor ColorBackground);
  44: 	UFUNCTION(BlueprintCallable)
  45: 	void AnimateProgressFromStart(float Start, float End, float AnimSpeed = 1.0f);
  47: 	UFUNCTION(BlueprintCallable)
  48: 	void AnimateProgressFromCurrent(float End, float AnimSpeed = 1.0f);
  50: 	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFillAnimationFinished);
  52: 	UPROPERTY(BlueprintAssignable)
  53: 	FOnFillAnimationFinished OnFillAnimationFinished;
  55: private:
  56: 	void SetProgress_Internal(float Progress);
  57: 	void SetStartProgress_Internal(float StartProgress);
  58: 	void SetColorA_Internal(FLinearColor ColorA);
  59: 	void SetColorB_Internal(FLinearColor ColorB);
  60: 	void SetColorBackground_Internal(FLinearColor ColorBackground);
  62: 	UMaterialInstanceDynamic* GetBarDynamicMaterial() const;
  64: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "CachedColorA"))
  65: 	bool bOverrideDefaultColorA = false;
  67: 	UPROPERTY(EditAnywhere, meta = (DisplayName = "Color A", EditCondition = "bOverrideDefaultColorA"))
  68: 	FLinearColor CachedColorA;
  70: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "CachedColorB"))
  71: 	bool bOverrideDefaultColorB = false;
  73: 	UPROPERTY(EditAnywhere, meta = (DisplayName = "Color B", EditCondition = "bOverrideDefaultColorB"))
  74: 	FLinearColor CachedColorB;
  76: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "CachedColorBackground"))
  77: 	bool bOverrideDefaultColorBackground;
  79: 	UPROPERTY(EditAnywhere,
  80: 		meta = (DisplayName = "Color Background", EditCondition = "bOverrideDefaultColorBackground"))
  81: 	FLinearColor CachedColorBackground;
  83: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "Segments"))
  84: 	bool bOverrideDefaultSegments = false;
  86: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultSegments"))
  87: 	int32 Segments;
  89: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "SegmentEdge"))
  90: 	bool bOverrideDefaultSegmentEdge = false;
  92: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultSegmentEdge"))
  93: 	float SegmentEdge;
  95: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "FillEdgeSoftness"))
  96: 	bool bOverrideDefaultFillEdgeSoftness;
  98: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultFillEdgeSoftness"))
  99: 	float FillEdgeSoftness;
 101: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "GlowEdge"))
 102: 	bool bOverrideDefaultGlowEdge = false;
 104: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultGlowEdge"))
 105: 	float GlowEdge;
 107: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "GlowSoftness"))
 108: 	bool bOverrideDefaultGlowSoftness = false;
 110: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultGlowSoftness"))
 111: 	float GlowSoftness;
 113: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "OutlineScale"))
 114: 	bool bOverrideDefaultOutlineScale = false;
 116: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultOutlineScale"))
 117: 	float OutlineScale;
 119: 	UPROPERTY(EditAnywhere)
 120: 	bool bUseStroke = true;
 122: 	UPROPERTY(EditDefaultsOnly)
 123: 	TObjectPtr<UMaterialInterface> StrokeMaterial;
 125: 	UPROPERTY(EditDefaultsOnly)
 126: 	TObjectPtr<UMaterialInterface> NoStrokeMaterial;
 128: #if WITH_EDITORONLY_DATA
 129: 	UPROPERTY(EditAnywhere, meta = (DisplayName = "Design Time Progress"))
 130: 	float DesignTime_Progress = 1.0f;
 131: #endif
 133: 	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess))
 134: 	TObjectPtr<UImage> Image_Bar;
 136: 	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
 137: 	TObjectPtr<UWidgetAnimation> BoundAnim_FillBar;
 139: 	UPROPERTY(Transient)
 140: 	mutable TObjectPtr<UMaterialInstanceDynamic> CachedMID;
 142: 	float CachedProgress = -1.0f;
 143: 	float CachedStartProgress = -1.0f;
 144: };
```

## HodgeBoundActionButton.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeBoundActionButton.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeBoundActionButton.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "Input/CommonBoundActionButton.h"
   7: #include "HodgeBoundActionButton.generated.h"
   9: class UCommonButtonStyle;
  10: class UObject;
  15: UCLASS(Abstract, meta = (DisableNativeTick))
  16: class HODGEPODGE_API UHodgeBoundActionButton : public UCommonBoundActionButton
  17: {
  18: 	GENERATED_BODY()
  20: protected:
  21: 	virtual void NativeConstruct() override;
  23: private:
  24: 	void HandleInputMethodChanged(ECommonInputType NewInputMethod);
  26: 	UPROPERTY(EditAnywhere, Category = "Styles")
  27: 	TSubclassOf<UCommonButtonStyle> KeyboardStyle;
  29: 	UPROPERTY(EditAnywhere, Category = "Styles")
  30: 	TSubclassOf<UCommonButtonStyle> GamepadStyle;
  32: 	UPROPERTY(EditAnywhere, Category = "Styles")
  33: 	TSubclassOf<UCommonButtonStyle> TouchStyle;
  34: };
```

## HodgeListView.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeListView.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeListView.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CommonListView.h"
   7: #include "HodgeListView.generated.h"
   9: class UUserWidget;
  10: class ULocalPlayer;
  11: class UHodgeWidgetFactory;
  13: UCLASS(meta = (DisableNativeTick))
  14: class HODGEPODGE_API UHodgeListView : public UCommonListView
  15: {
  16: 	GENERATED_BODY()
  18: public:
  19: 	UHodgeListView(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  21: #if WITH_EDITOR
  22: 	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const override;
  23: #endif
  25: protected:
  26: 	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass,
  27: 	                                                   const TSharedRef<STableViewBase>& OwnerTable) override;
  30: protected:
  31: 	UPROPERTY(EditAnywhere, Instanced, Category="Entry Creation")
  32: 	TArray<TObjectPtr<UHodgeWidgetFactory>> FactoryRules;
  33: };
```

## HodgeTabButtonBase.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeTabButtonBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabButtonBase.h)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgeButtonBase.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "HodgeTabListWidgetBase.h"
   6: #include "UI/Foundation/HodgeButtonBase.h"
   8: #include "HodgeTabButtonBase.generated.h"
  10: class UCommonLazyImage;
  11: class UObject;
  12: struct FFrame;
  13: struct FSlateBrush;
  15: UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
  16: class HODGEPODGE_API UHodgeTabButtonBase : public UHodgeButtonBase, public IHodgeTabButtonInterface
  17: {
  18: 	GENERATED_BODY()
  20: public:
  21: 	void SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject);
  22: 	void SetIconBrush(const FSlateBrush& Brush);
  24: protected:
  25: 	UFUNCTION()
  26: 	virtual void SetTabLabelInfo_Implementation(const FHodgeTabDescriptor& TabLabelInfo) override;
  28: private:
  29: 	UPROPERTY(meta = (BindWidgetOptional))
  30: 	TObjectPtr<UCommonLazyImage> LazyImage_Icon;
  31: };
```

## HodgeTabListWidgetBase.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CommonTabListWidgetBase.h"
   7: #include "HodgeTabListWidgetBase.generated.h"
   9: class UCommonButtonBase;
  10: class UCommonUserWidget;
  11: class UObject;
  12: class UWidget;
  13: struct FFrame;
  15: USTRUCT(BlueprintType)
  16: struct FHodgeTabDescriptor
  17: {
  18: 	GENERATED_BODY()
  20: public:
  21: 	FHodgeTabDescriptor()
  22: 		: bHidden(false)
  23: 		  , CreatedTabContentWidget(nullptr)
  24: 	{
  25: 	}
  27: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  28: 	FName TabId;
  30: 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
  31: 	FText TabText;
  33: 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
  34: 	FSlateBrush IconBrush;
  36: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  37: 	bool bHidden;
  39: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  40: 	TSubclassOf<UCommonButtonBase> TabButtonType;
  43: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  44: 	TSubclassOf<UCommonUserWidget> TabContentType;
  46: 	UPROPERTY(Transient)
  47: 	TObjectPtr<UWidget> CreatedTabContentWidget;
  48: };
  50: UINTERFACE(BlueprintType)
  51: class UHodgeTabButtonInterface : public UInterface
  52: {
  53: 	GENERATED_BODY()
  54: };
  56: class IHodgeTabButtonInterface
  57: {
  58: 	GENERATED_BODY()
  60: public:
  61: 	UFUNCTION(BlueprintNativeEvent, Category = "Tab Button")
  62: 	void SetTabLabelInfo(const FHodgeTabDescriptor& TabDescriptor);
  63: };
  65: UCLASS(Blueprintable, BlueprintType, Abstract, meta = (DisableNativeTick))
  66: class HODGEPODGE_API UHodgeTabListWidgetBase : public UCommonTabListWidgetBase
  67: {
  68: 	GENERATED_BODY()
  70: public:
  71: 	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
  72: 	bool GetPreregisteredTabInfo(const FName TabNameId, FHodgeTabDescriptor& OutTabInfo);
  75: 	const TArray<FHodgeTabDescriptor>& GetAllPreregisteredTabInfos() { return PreregisteredTabInfoArray; }
  78: 	UFUNCTION(BlueprintCallable, Category = "Tab List")
  79: 	void SetTabHiddenState(FName TabNameId, bool bHidden);
  81: 	UFUNCTION(BlueprintCallable, Category = "Tab List")
  82: 	bool RegisterDynamicTab(const FHodgeTabDescriptor& TabDescriptor);
  84: 	UFUNCTION(BlueprintCallable, Category = "Tab List")
  85: 	bool IsFirstTabActive() const;
  87: 	UFUNCTION(BlueprintCallable, Category = "Tab List")
  88: 	bool IsLastTabActive() const;
  90: 	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
  91: 	bool IsTabVisible(FName TabId);
  93: 	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
  94: 	int32 GetVisibleTabCount();
  97: 	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTabContentCreated, FName, TabId, UCommonUserWidget*, TabWidget);
  99: 	DECLARE_EVENT_TwoParams(UHodgeTabListWidgetBase, FOnTabContentCreatedNative, FName            ,
 100: 	                        UCommonUserWidget*                );
 103: 	UPROPERTY(BlueprintAssignable, Category = "Tab List")
 104: 	FOnTabContentCreated OnTabContentCreated;
 105: 	FOnTabContentCreatedNative OnTabContentCreatedNative;
 107: protected:
 109: 	virtual void NativeOnInitialized() override;
 110: 	virtual void NativeConstruct() override;
 111: 	virtual void NativeDestruct() override;
 114: 	virtual void HandlePreLinkedSwitcherChanged() override;
 115: 	virtual void HandlePostLinkedSwitcherChanged() override;
 117: 	virtual void HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton) override;
 119: private:
 120: 	void SetupTabs();
 122: 	UPROPERTY(EditAnywhere, meta=(TitleProperty="TabId"))
 123: 	TArray<FHodgeTabDescriptor> PreregisteredTabInfoArray;
 129: 	UPROPERTY()
 130: 	TMap<FName, FHodgeTabDescriptor> PendingTabLabelInfoMap;
 131: };
```

## HodgeWidgetFactory.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "UObject/Object.h"
   7: #include "HodgeWidgetFactory.generated.h"
   9: template <class TClass>
  10: class TSubclassOf;
  12: class UUserWidget;
  13: struct FFrame;
  15: UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
  16: class HODGEPODGE_API UHodgeWidgetFactory : public UObject
  17: {
  18: 	GENERATED_BODY()
  20: public:
  21: 	UHodgeWidgetFactory()
  22: 	{
  23: 	}
  25: 	UFUNCTION(BlueprintNativeEvent)
  26: 	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
  27: };
```

## HodgeWidgetFactory_Class.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory_Class.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory_Class.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "HodgeWidgetFactory.h"
   6: #include "Templates/SubclassOf.h"
   7: #include "UObject/SoftObjectPtr.h"
   9: #include "HodgeWidgetFactory_Class.generated.h"
  11: class UObject;
  12: class UUserWidget;
  14: UCLASS()
  15: class HODGEPODGE_API UHodgeWidgetFactory_Class : public UHodgeWidgetFactory
  16: {
  17: 	GENERATED_BODY()
  19: public:
  20: 	UHodgeWidgetFactory_Class()
  21: 	{
  22: 	}
  24: 	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;
  26: protected:
  27: 	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
  28: 	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
  29: };
```

## UIExtensionPointWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   7: #include "Components/DynamicEntryBoxBase.h"
  11: #include "UIExtensionSystem.h"
  13: #include "UIExtensionPointWidget.generated.h"
  17: class IWidgetCompilerLog;
  21: class UHodgeLocalPlayerBase;
  25: class APlayerState;
  36: UCLASS()
  37: class HODGEPODGE_API UUIExtensionPointWidget : public UDynamicEntryBoxBase
  38: {
  39: 	GENERATED_BODY()
  41: public:
  47: 	DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(TSubclassOf<UUserWidget>, FOnGetWidgetClassForData, UObject*, DataItem);
  51: 	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnConfigureWidgetForData, UUserWidget*, Widget, UObject*, DataItem);
  54: 	UUIExtensionPointWidget(const FObjectInitializer& ObjectInitializer);
  60: 	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
  64: 	virtual TSharedRef<SWidget> RebuildWidget() override;
  66: #if WITH_EDITOR
  68: 	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
  69: #endif
  73: private:
  78: 	void ResetExtensionPoint();
  82: 	void RegisterExtensionPoint();
  88: 	void RegisterExtensionPointForPlayerState(UHodgeLocalPlayerBase* LocalPlayer, APlayerState* PlayerState);
  99: 	void OnAddOrRemoveExtension(EUIExtensionAction Action, const FUIExtensionRequest& Request);
 101: protected:
 113: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
 114: 	FGameplayTag ExtensionPointTag;
 125: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
 126: 	EUIExtensionPointMatch ExtensionPointTagMatch = EUIExtensionPointMatch::ExactMatch;
 132: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
 133: 	TArray<TObjectPtr<UClass>> DataClasses;
 142: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Extension", meta=( IsBindableEvent="True" ))
 143: 	FOnGetWidgetClassForData GetWidgetClassForData;
 149: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Extension", meta=( IsBindableEvent="True" ))
 150: 	FOnConfigureWidgetForData ConfigureWidgetForData;
 158: 	TArray<FUIExtensionPointHandle> ExtensionPointHandles;
 170: 	UPROPERTY(Transient)
 171: 	TMap<FUIExtensionHandle, TObjectPtr<UUserWidget>> ExtensionMapping;
 172: };
```

## UIExtensionSystem.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Extension/UIExtensionSystem.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionSystem.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   7: #include "GameplayTagContainer.h"
  11: #include "Kismet/BlueprintFunctionLibrary.h"
  15: #include "Subsystems/WorldSubsystem.h"
  17: #include "UIExtensionSystem.generated.h"
  20: class UUIExtensionSubsystem;
  23: struct FUIExtensionRequest;
  25: template <typename T>
  26: class TSubclassOf;
  29: class FSubsystemCollectionBase;
  33: class UUserWidget;
  35: struct FFrame;
  38: DECLARE_LOG_CATEGORY_EXTERN(LogUIExtension, Log, All);
  42: UENUM(BlueprintType)
  43: enum class EUIExtensionPointMatch : uint8
  44: {
  51: 	ExactMatch,
  59: 	PartialMatch
  60: };
  64: UENUM(BlueprintType)
  65: enum class EUIExtensionAction : uint8
  66: {
  68: 	Added,
  71: 	Removed
  72: };
  77: DECLARE_DELEGATE_TwoParams(FExtendExtensionPointDelegate, EUIExtensionAction Action,
  78:                            const FUIExtensionRequest& Request);
  89: struct FUIExtension : TSharedFromThis<FUIExtension>
  90: {
  91: public:
  96: 	FGameplayTag ExtensionPointTag;
 100: 	int32 Priority = INDEX_NONE;
 105: 	TWeakObjectPtr<UObject> ContextObject;
 115: 	TObjectPtr<UObject> Data = nullptr;
 116: };
 125: struct FUIExtensionPoint : TSharedFromThis<FUIExtensionPoint>
 126: {
 127: public:
 129: 	FGameplayTag ExtensionPointTag;
 133: 	TWeakObjectPtr<UObject> ContextObject;
 136: 	EUIExtensionPointMatch ExtensionPointTagMatchType = EUIExtensionPointMatch::ExactMatch;
 140: 	TArray<TObjectPtr<UClass>> AllowedDataClasses;
 143: 	FExtendExtensionPointDelegate Callback;
 156: 	bool DoesExtensionPassContract(const FUIExtension* Extension) const;
 157: };
 167: USTRUCT(BlueprintType)
 168: struct HODGEPODGE_API FUIExtensionPointHandle
 169: {
 170: 	GENERATED_BODY()
 172: public:
 174: 	FUIExtensionPointHandle()
 175: 	{
 176: 	}
 179: 	void Unregister();
 182: 	bool IsValid() const { return DataPtr.IsValid(); }
 185: 	bool operator==(const FUIExtensionPointHandle& Other) const { return DataPtr == Other.DataPtr; }
 188: 	bool operator!=(const FUIExtensionPointHandle& Other) const { return !operator==(Other); }
 192: 	friend uint32 GetTypeHash(const FUIExtensionPointHandle& Handle)
 193: 	{
 194: 		return PointerHash(Handle.DataPtr.Get());
 195: 	}
 197: private:
 200: 	TWeakObjectPtr<UUIExtensionSubsystem> ExtensionSource;
 203: 	TSharedPtr<FUIExtensionPoint> DataPtr;
 206: 	friend UUIExtensionSubsystem;
 209: 	FUIExtensionPointHandle(UUIExtensionSubsystem* InExtensionSource,
 210: 	                        const TSharedPtr<FUIExtensionPoint>& InDataPtr) : ExtensionSource(InExtensionSource),
 211: 	                                                                          DataPtr(InDataPtr)
 212: 	{
 213: 	}
 214: };
 217: template <>
 218: struct TStructOpsTypeTraits<FUIExtensionPointHandle> : public TStructOpsTypeTraitsBase2<FUIExtensionPointHandle>
 219: {
 220: 	enum
 221: 	{
 224: 		WithCopy = true,
 227: 		WithIdenticalViaEquality = true,
 228: 	};
 229: };
 238: USTRUCT(BlueprintType)
 239: struct HODGEPODGE_API FUIExtensionHandle
 240: {
 241: 	GENERATED_BODY()
 243: public:
 245: 	FUIExtensionHandle()
 246: 	{
 247: 	}
 250: 	void Unregister();
 253: 	bool IsValid() const { return DataPtr.IsValid(); }
 256: 	bool operator==(const FUIExtensionHandle& Other) const { return DataPtr == Other.DataPtr; }
 259: 	bool operator!=(const FUIExtensionHandle& Other) const { return !operator==(Other); }
 262: 	friend FORCEINLINE uint32 GetTypeHash(FUIExtensionHandle Handle)
 263: 	{
 264: 		return PointerHash(Handle.DataPtr.Get());
 265: 	}
 267: private:
 269: 	TWeakObjectPtr<UUIExtensionSubsystem> ExtensionSource;
 272: 	TSharedPtr<FUIExtension> DataPtr;
 275: 	friend UUIExtensionSubsystem;
 278: 	FUIExtensionHandle(UUIExtensionSubsystem* InExtensionSource,
 279: 	                   const TSharedPtr<FUIExtension>& InDataPtr) : ExtensionSource(InExtensionSource),
 280: 	                                                                DataPtr(InDataPtr)
 281: 	{
 282: 	}
 283: };
 286: template <>
 287: struct TStructOpsTypeTraits<FUIExtensionHandle> : public TStructOpsTypeTraitsBase2<FUIExtensionHandle>
 288: {
 289: 	enum
 290: 	{
 293: 		WithCopy = true,
 296: 		WithIdenticalViaEquality = true,
 297: 	};
 298: };
 307: USTRUCT(BlueprintType)
 308: struct FUIExtensionRequest
 309: {
 310: 	GENERATED_BODY()
 312: public:
 315: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 316: 	FUIExtensionHandle ExtensionHandle;
 319: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 320: 	FGameplayTag ExtensionPointTag;
 323: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 324: 	int32 Priority = INDEX_NONE;
 327: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 328: 	TObjectPtr<UObject> Data = nullptr;
 331: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 332: 	TObjectPtr<UObject> ContextObject = nullptr;
 333: };
 337: DECLARE_DYNAMIC_DELEGATE_TwoParams(FExtendExtensionPointDynamicDelegate, EUIExtensionAction, Action,
 338:                                    const FUIExtensionRequest&, ExtensionRequest);
 355: UCLASS()
 356: class HODGEPODGE_API UUIExtensionSubsystem : public UWorldSubsystem
 357: {
 358: 	GENERATED_BODY()
 360: public:
 374: 	FUIExtensionPointHandle RegisterExtensionPoint(const FGameplayTag& ExtensionPointTag,
 375: 	                                               EUIExtensionPointMatch ExtensionPointTagMatchType,
 376: 	                                               const TArray<UClass*>& AllowedDataClasses,
 377: 	                                               FExtendExtensionPointDelegate ExtensionCallback);
 381: 	FUIExtensionPointHandle RegisterExtensionPointForContext(const FGameplayTag& ExtensionPointTag,
 382: 	                                                         UObject* ContextObject,
 383: 	                                                         EUIExtensionPointMatch ExtensionPointTagMatchType,
 384: 	                                                         const TArray<UClass*>& AllowedDataClasses,
 385: 	                                                         FExtendExtensionPointDelegate ExtensionCallback);
 388: 	FUIExtensionHandle RegisterExtensionAsWidget(const FGameplayTag& ExtensionPointTag,
 389: 	                                             TSubclassOf<UUserWidget> WidgetClass, int32 Priority);
 392: 	FUIExtensionHandle RegisterExtensionAsWidgetForContext(const FGameplayTag& ExtensionPointTag,
 393: 	                                                       UObject* ContextObject, TSubclassOf<UUserWidget> WidgetClass,
 394: 	                                                       int32 Priority);
 398: 	FUIExtensionHandle RegisterExtensionAsData(const FGameplayTag& ExtensionPointTag, UObject* ContextObject,
 399: 	                                           UObject* Data, int32 Priority);
 402: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 403: 	void UnregisterExtension(const FUIExtensionHandle& ExtensionHandle);
 406: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 407: 	void UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle);
 410: 	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
 412: protected:
 414: 	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
 417: 	virtual void Deinitialize() override;
 422: 	void NotifyExtensionPointOfExtensions(TSharedPtr<FUIExtensionPoint>& ExtensionPoint);
 427: 	void NotifyExtensionPointsOfExtension(EUIExtensionAction Action, TSharedPtr<FUIExtension>& Extension);
 430: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="UI Extension",
 431: 		meta = (DisplayName = "Register Extension Point"))
 432: 	FUIExtensionPointHandle K2_RegisterExtensionPoint(FGameplayTag ExtensionPointTag,
 433: 	                                                  EUIExtensionPointMatch ExtensionPointTagMatchType,
 434: 	                                                  const TArray<UClass*>& AllowedDataClasses,
 435: 	                                                  FExtendExtensionPointDynamicDelegate ExtensionCallback);
 438: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension",
 439: 		meta = (DisplayName = "Register Extension (Widget)"))
 440: 	FUIExtensionHandle K2_RegisterExtensionAsWidget(FGameplayTag ExtensionPointTag,
 441: 	                                                TSubclassOf<UUserWidget> WidgetClass, int32 Priority = -1);
 452: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension",
 453: 		meta = (DisplayName = "Register Extension (Widget For Context)"))
 454: 	FUIExtensionHandle K2_RegisterExtensionAsWidgetForContext(FGameplayTag ExtensionPointTag,
 455: 	                                                          TSubclassOf<UUserWidget> WidgetClass,
 456: 	                                                          UObject* ContextObject, int32 Priority = -1);
 463: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="UI Extension",
 464: 		meta = (DisplayName = "Register Extension (Data)"))
 465: 	FUIExtensionHandle K2_RegisterExtensionAsData(FGameplayTag ExtensionPointTag, UObject* Data, int32 Priority = -1);
 471: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="UI Extension",
 472: 		meta = (DisplayName = "Register Extension (Data For Context)"))
 473: 	FUIExtensionHandle K2_RegisterExtensionAsDataForContext(FGameplayTag ExtensionPointTag, UObject* ContextObject,
 474: 	                                                        UObject* Data, int32 Priority = -1);
 477: 	FUIExtensionRequest CreateExtensionRequest(const TSharedPtr<FUIExtension>& Extension);
 479: private:
 481: 	typedef TArray<TSharedPtr<FUIExtensionPoint>> FExtensionPointList;
 484: 	TMap<FGameplayTag, FExtensionPointList> ExtensionPointMap;
 487: 	typedef TArray<TSharedPtr<FUIExtension>> FExtensionList;
 490: 	TMap<FGameplayTag, FExtensionList> ExtensionMap;
 491: };
 496: UCLASS()
 497: class HODGEPODGE_API UUIExtensionHandleFunctions : public UBlueprintFunctionLibrary
 498: {
 499: 	GENERATED_BODY()
 501: public:
 502: 	UUIExtensionHandleFunctions()
 503: 	{
 504: 	}
 507: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 508: 	static void Unregister(UPARAM(ref) FUIExtensionHandle& Handle);
 511: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 512: 	static bool IsValid(UPARAM(ref) FUIExtensionHandle& Handle);
 513: };
 517: UCLASS()
 518: class HODGEPODGE_API UUIExtensionPointHandleFunctions : public UBlueprintFunctionLibrary
 519: {
 520: 	GENERATED_BODY()
 522: public:
 523: 	UUIExtensionPointHandleFunctions()
 524: 	{
 525: 	}
 528: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 529: 	static void Unregister(UPARAM(ref) FUIExtensionPointHandle& Handle);
 532: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 533: 	static bool IsValid(UPARAM(ref) FUIExtensionPointHandle& Handle);
 534: };
```

## HodgeActionWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Foundation/HodgeActionWidget.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeActionWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CommonActionWidget.h"
   6: #include "HodgeActionWidget.generated.h"
   8: class UEnhancedInputLocalPlayerSubsystem;
   9: class UInputAction;
  12: UCLASS(BlueprintType, Blueprintable)
  13: class UHodgeActionWidget : public UCommonActionWidget
  14: {
  15: 	GENERATED_BODY()
  17: public:
  20: 	virtual FSlateBrush GetIcon() const override;
  24: 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
  25: 	const TObjectPtr<UInputAction> AssociatedInputAction;
  27: private:
  29: 	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
  31: };
```

## HodgeButtonBase.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CommonButtonBase.h"
   7: #include "HodgeButtonBase.generated.h"
   9: class UObject;
  10: struct FFrame;
  12: UCLASS(Abstract, BlueprintType, Blueprintable)
  13: class UHodgeButtonBase : public UCommonButtonBase
  14: {
  15: 	GENERATED_BODY()
  17: public:
  18: 	UFUNCTION(BlueprintCallable)
  19: 	void SetButtonText(const FText& InText);
  21: protected:
  23: 	virtual void NativePreConstruct() override;
  27: 	virtual void UpdateInputActionWidget() override;
  28: 	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;
  31: 	void RefreshButtonText();
  33: 	UFUNCTION(BlueprintImplementableEvent)
  34: 	void UpdateButtonText(const FText& InText);
  36: 	UFUNCTION(BlueprintImplementableEvent)
  37: 	void UpdateButtonStyle();
  39: private:
  40: 	UPROPERTY(EditAnywhere, Category="Button", meta=(InlineEditConditionToggle))
  41: 	uint8 bOverride_ButtonText : 1;
  43: 	UPROPERTY(EditAnywhere, Category="Button", meta=( editcondition="bOverride_ButtonText" ))
  44: 	FText ButtonText;
  45: };
```

## HodgeConfirmationScreen.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Foundation/HodgeConfirmationScreen.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeConfirmationScreen.h)

**全部为注释或空白；无有效声明/实现。**

## HodgeControllerDisconnectedScreen.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Foundation/HodgeControllerDisconnectedScreen.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeControllerDisconnectedScreen.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CommonActivatableWidget.h"
   6: #include "GameplayTagContainer.h"
   8: #include "HodgeControllerDisconnectedScreen.generated.h"
  10: class UHorizontalBox;
  11: class UObject;
  12: class UCommonButtonBase;
  13: struct FPlatformUserSelectionCompleteParams;
  19: UCLASS(Abstract, BlueprintType, Blueprintable)
  20: class UHodgeControllerDisconnectedScreen : public UCommonActivatableWidget
  21: {
  22: 	GENERATED_BODY()
  23: public:
  24: 	UHodgeControllerDisconnectedScreen(const FObjectInitializer& ObjectInitializer);
  26: protected:
  27: 	virtual void NativeOnActivated() override;
  29: 	virtual void HandleChangeUserClicked();
  34: 	virtual void HandleChangeUserCompleted(const FPlatformUserSelectionCompleteParams& Params);
  40: 	virtual bool ShouldDisplayChangeUserButton() const;
  47: 	UPROPERTY(EditDefaultsOnly)
  48: 	FGameplayTagContainer PlatformSupportsUserChangeTags;
  57: 	UPROPERTY(meta = (BindWidget))
  58: 	TObjectPtr<UHorizontalBox> HBox_SwitchUser;
  65: 	UPROPERTY(meta = (BindWidget))
  66: 	TObjectPtr<UCommonButtonBase> Button_ChangeUser;
  67: };
```

## HodgeLoadingScreenSubsystem.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Foundation/HodgeLoadingScreenSubsystem.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeLoadingScreenSubsystem.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "Subsystems/GameInstanceSubsystem.h"
   6: #include "Templates/SubclassOf.h"
   8: #include "UObject/WeakObjectPtr.h"
   9: #include "HodgeLoadingScreenSubsystem.generated.h"
  11: class UObject;
  12: class UUserWidget;
  13: struct FFrame;
  15: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadingScreenWidgetChangedDelegate, TSubclassOf<UUserWidget>, NewWidgetClass);
  21: UCLASS()
  22: class HODGEPODGE_API UHodgeLoadingScreenSubsystem : public UGameInstanceSubsystem
  23: {
  24: 	GENERATED_BODY()
  26: public:
  27: 	UHodgeLoadingScreenSubsystem();
  30: 	UFUNCTION(BlueprintCallable)
  31: 	void SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass);
  34: 	UFUNCTION(BlueprintPure)
  35: 	TSubclassOf<UUserWidget> GetLoadingScreenContentWidget() const;
  37: private:
  38: 	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess))
  39: 	FLoadingScreenWidgetChangedDelegate OnLoadingScreenWidgetChanged;
  41: 	UPROPERTY()
  42: 	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;
  43: };
```

## ApplyFrontendPerfSettingsAction.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Frontend/ApplyFrontendPerfSettingsAction.h](../../../Source/Hodgepodge/Public/UI/Frontend/ApplyFrontendPerfSettingsAction.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameFeatureAction.h"
   7: #include "ApplyFrontendPerfSettingsAction.generated.h"
   9: class UObject;
  10: struct FGameFeatureActivatingContext;
  11: struct FGameFeatureDeactivatingContext;
  19: UCLASS(MinimalAPI, meta = (DisplayName = "Use Frontend Perf Settings"))
  20: class UApplyFrontendPerfSettingsAction final : public UGameFeatureAction
  21: {
  22: 	GENERATED_BODY()
  24: public:
  26: 	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
  27: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
  30: private:
  31: 	static int32 ApplicationCounter;
  32: };
```

## HodgeFrontendStateComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Frontend/HodgeFrontendStateComponent.h](../../../Source/Hodgepodge/Public/UI/Frontend/HodgeFrontendStateComponent.h)

**全部为注释或空白；无有效声明/实现。**

## HodgeLobbyBackground.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Frontend/HodgeLobbyBackground.h](../../../Source/Hodgepodge/Public/UI/Frontend/HodgeLobbyBackground.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "Engine/DataAsset.h"
   7: #include "HodgeLobbyBackground.generated.h"
   9: class UObject;
  10: class UWorld;
  15: UCLASS(config=EditorPerProjectUserSettings, MinimalAPI)
  16: class UHodgeLobbyBackground : public UPrimaryDataAsset
  17: {
  18: 	GENERATED_BODY()
  20: public:
  22: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  23: 	TSoftObjectPtr<UWorld> BackgroundLevel;
  24: };
```

## HodgeActivatableWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   7: #include "CommonActivatableWidget.h"
   9: #include "HodgeActivatableWidget.generated.h"
  13: struct FUIInputConfig;
  17: UENUM(BlueprintType)
  18: enum class EHodgeWidgetInputMode : uint8
  19: {
  21: 	Default,
  24: 	GameAndMenu,
  27: 	Game,
  30: 	Menu
  31: };
  37: UCLASS(Abstract, Blueprintable)
  38: class UHodgeActivatableWidget : public UCommonActivatableWidget
  39: {
  40: 	GENERATED_BODY()
  42: public:
  44: 	UHodgeActivatableWidget(const FObjectInitializer& ObjectInitializer);
  46: public:
  51: 	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
  55: #if WITH_EDITOR
  58: 	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree,
  59: 	                                        class IWidgetCompilerLog& CompileLog) const override;
  60: #endif
  62: protected:
  67: 	UPROPERTY(EditDefaultsOnly, Category = Input)
  68: 	EHodgeWidgetInputMode InputConfig = EHodgeWidgetInputMode::Default;
  73: 	UPROPERTY(EditDefaultsOnly, Category = Input)
  74: 	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
  75: };
```

## HodgeGameViewportClient.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeGameViewportClient.h](../../../Source/Hodgepodge/Public/UI/HodgeGameViewportClient.h)

**全部为注释或空白；无有效声明/实现。**

## HodgeHUDLayout.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeHUDLayout.h](../../../Source/Hodgepodge/Public/UI/HodgeHUDLayout.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   7: #include "HodgeActivatableWidget.h"
  11: #include "Containers/Ticker.h"
  15: #include "GameplayTagContainer.h"
  17: #include "HodgeHUDLayout.generated.h"
  21: class UCommonActivatableWidget;
  24: class UObject;
  27: class UHodgeControllerDisconnectedScreen;
  30: class UCommonActivatableWidgetStack;
  45: UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "Hodge HUD Layout", Category = "Hodge|HUD"))
  46: class UHodgeHUDLayout : public UHodgeActivatableWidget
  47: {
  48: 	GENERATED_BODY()
  50: public:
  52: 	UHodgeHUDLayout(const FObjectInitializer& ObjectInitializer);
  58: 	virtual void NativeOnInitialized() override;
  63: 	virtual void NativeDestruct() override;
  65: protected:
  70: 	void HandleEscapeAction();
  73: 	void EnsureMenuLayerStack();
  89: 	void HandleInputDeviceConnectionChanged(EInputDeviceConnectionState NewConnectionState,
  90: 	                                        FPlatformUserId PlatformUserId, FInputDeviceId InputDeviceId);
 104: 	void HandleInputDevicePairingChanged(FInputDeviceId InputDeviceId, FPlatformUserId NewUserPlatformId,
 105: 	                                     FPlatformUserId OldUserPlatformId);
 117: 	void NotifyControllerStateChangeForDisconnectScreen();
 133: 	virtual void ProcessControllerDevicesHavingChangedForDisconnectScreen();
 143: 	virtual bool ShouldPlatformDisplayControllerDisconnectScreen() const;
 156: 	UFUNCTION(BlueprintNativeEvent, Category="Controller Disconnect Menu")
 157: 	void DisplayControllerDisconnectedMenu();
 166: 	UFUNCTION(BlueprintNativeEvent, Category="Controller Disconnect Menu")
 167: 	void HideControllerDisconnectedMenu();
 177: 	UPROPERTY(EditDefaultsOnly)
 178: 	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
 185: 	UPROPERTY(EditDefaultsOnly, Category="Controller Disconnect Menu")
 186: 	TSubclassOf<UHodgeControllerDisconnectedScreen> ControllerDisconnectedScreen;
 202: 	UPROPERTY(EditDefaultsOnly, Category="Controller Disconnect Menu")
 203: 	FGameplayTagContainer PlatformRequiresControllerDisconnectScreen;
 211: 	UPROPERTY(Transient)
 212: 	TObjectPtr<UCommonActivatableWidget> SpawnedControllerDisconnectScreen;
 215: 	UPROPERTY(meta = (BindWidgetOptional))
 216: 	TObjectPtr<UCommonActivatableWidgetStack> MenuLayerStack;
 224: 	FTSTicker::FDelegateHandle RequestProcessControllerStateHandle;
 225: };
```

## HodgeJoystickWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeJoystickWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeJoystickWidget.h)

项目内直接 include（不是运行调用关系）：[UI/HodgeSimulatedInputWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "UI/HodgeSimulatedInputWidget.h"
   7: #include "HodgeJoystickWidget.generated.h"
   9: class UImage;
  10: class UObject;
  11: struct FGeometry;
  12: struct FPointerEvent;
  22: UCLASS(meta=( DisplayName="Hodge Joystick" ))
  23: class HODGEPODGE_API UHodgeJoystickWidget : public UHodgeSimulatedInputWidget
  24: {
  25: 	GENERATED_BODY()
  27: public:
  29: 	UHodgeJoystickWidget(const FObjectInitializer& ObjectInitializer);
  32: 	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  33: 	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  34: 	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  35: 	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
  36: 	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
  39: protected:
  49: 	void HandleTouchDelta(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent);
  52: 	void StopInputSimulation();
  55: 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
  56: 	float StickRange = 50.0f;
  59: 	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
  60: 	TObjectPtr<UImage> JoystickBackground;
  63: 	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
  64: 	TObjectPtr<UImage> JoystickForeground;
  67: 	UPROPERTY(BlueprintReadWrite, EditAnywhere)
  68: 	bool bNegateYAxis = false;
  71: 	UPROPERTY(Transient)
  72: 	FVector2D TouchOrigin = FVector2D::ZeroVector;
  74: 	UPROPERTY(Transient)
  75: 	FVector2D StickVector = FVector2D::ZeroVector;
  76: };
```

## HodgeSettingScreen.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeSettingScreen.h](../../../Source/Hodgepodge/Public/UI/HodgeSettingScreen.h)

**全部为注释或空白；无有效声明/实现。**

## HodgeSimulatedInputWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CommonUserWidget.h"
   6: #include "HodgeSimulatedInputWidget.generated.h"
   8: class UEnhancedInputLocalPlayerSubsystem;
   9: class UInputAction;
  10: class UCommonHardwareVisibilityBorder;
  11: class UEnhancedPlayerInput;
  17: UCLASS(meta=( DisplayName="Hodge Simulated Input Widget" ))
  18: class HODGEPODGE_API UHodgeSimulatedInputWidget : public UCommonUserWidget
  19: {
  20: 	GENERATED_BODY()
  22: public:
  23: 	UHodgeSimulatedInputWidget(const FObjectInitializer& ObjectInitializer);
  26: #if WITH_EDITOR
  27: 	virtual const FText GetPaletteCategory() override;
  28: #endif
  32: 	virtual void NativeConstruct() override;
  33: 	virtual void NativeDestruct() override;
  34: 	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  38: 	UFUNCTION(BlueprintCallable)
  39: 	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
  42: 	UEnhancedPlayerInput* GetPlayerInput() const;
  45: 	UFUNCTION(BlueprintCallable)
  46: 	const UInputAction* GetAssociatedAction() const { return AssociatedAction; }
  49: 	UFUNCTION(BlueprintCallable)
  50: 	FKey GetSimulatedKey() const { return KeyToSimulate; }
  56: 	UFUNCTION(BlueprintCallable)
  57: 	void InputKeyValue(const FVector& Value);
  63: 	UFUNCTION(BlueprintCallable)
  64: 	void InputKeyValue2D(const FVector2D& Value);
  66: 	UFUNCTION(BlueprintCallable)
  67: 	void FlushSimulatedInput();
  69: protected:
  71: 	void QueryKeyToSimulate();
  74: 	UFUNCTION()
  75: 	void OnControlMappingsRebuilt();
  78: 	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
  79: 	TObjectPtr<UCommonHardwareVisibilityBorder> CommonVisibilityBorder = nullptr;
  82: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  83: 	TObjectPtr<const UInputAction> AssociatedAction = nullptr;
  86: 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
  87: 	FKey FallbackBindingKey = EKeys::Gamepad_Right2D;
  90: 	FKey KeyToSimulate;
  91: };
```

## HodgeTaggedWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeTaggedWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeTaggedWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   7: #include "CommonUserWidget.h"
  11: #include "GameplayTagContainer.h"
  13: #include "HodgeTaggedWidget.generated.h"
  15: class UObject;
  29: UCLASS(Abstract, Blueprintable)
  30: class UHodgeTaggedWidget : public UCommonUserWidget
  31: {
  32: 	GENERATED_BODY()
  34: public:
  36: 	UHodgeTaggedWidget(const FObjectInitializer& ObjectInitializer);
  43: 	virtual void SetVisibility(ESlateVisibility InVisibility) override;
  51: 	virtual void NativeConstruct() override;
  55: 	virtual void NativeDestruct() override;
  59: protected:
  64: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
  65: 	FGameplayTagContainer HiddenByTags;
  70: 	UPROPERTY(EditAnywhere, Category = "HUD")
  71: 	ESlateVisibility ShownVisibility = ESlateVisibility::Visible;
  76: 	UPROPERTY(EditAnywhere, Category = "HUD")
  77: 	ESlateVisibility HiddenVisibility = ESlateVisibility::Collapsed;
  84: 	bool bWantsToBeVisible = true;
  86: private:
  89: 	void OnWatchedTagsChanged();
  90: };
```

## HodgeTouchRegion.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeTouchRegion.h](../../../Source/Hodgepodge/Public/UI/HodgeTouchRegion.h)

项目内直接 include（不是运行调用关系）：[UI/HodgeSimulatedInputWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "UI/HodgeSimulatedInputWidget.h"
   7: #include "HodgeTouchRegion.generated.h"
   9: class UObject;
  10: struct FFrame;
  11: struct FGeometry;
  12: struct FPointerEvent;
  18: UCLASS(meta=( DisplayName="Hodge Touch Region" ))
  19: class HODGEPODGE_API UHodgeTouchRegion : public UHodgeSimulatedInputWidget
  20: {
  21: 	GENERATED_BODY()
  23: public:
  25: 	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  26: 	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  27: 	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  28: 	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
  31: 	UFUNCTION(BlueprintCallable)
  32: 	bool ShouldSimulateInput() const { return bShouldSimulateInput; }
  34: protected:
  36: 	bool bShouldSimulateInput = false;
  37: };
```

## HodgeIndicatorManagerComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   7: #include "Components/ControllerComponent.h"
   9: #include "HodgeIndicatorManagerComponent.generated.h"
  13: class AController;
  17: class UIndicatorDescriptor;
  19: class UObject;
  20: struct FFrame;
  33: UCLASS(BlueprintType, Blueprintable)
  34: class HODGEPODGE_API UHodgeIndicatorManagerComponent : public UControllerComponent
  35: {
  36: 	GENERATED_BODY()
  38: public:
  40: 	UHodgeIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);
  44: 	static UHodgeIndicatorManagerComponent* GetComponent(AController* Controller);
  49: 	UFUNCTION(BlueprintCallable, Category = Indicator)
  50: 	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);
  54: 	UFUNCTION(BlueprintCallable, Category = Indicator)
  55: 	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);
  59: 	DECLARE_EVENT_OneParam(UHodgeIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)
  62: 	FIndicatorEvent OnIndicatorAdded;
  65: 	FIndicatorEvent OnIndicatorRemoved;
  69: 	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }
  71: private:
  75: 	UPROPERTY()
  76: 	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
  77: };
```

## IActorIndicatorWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/IActorIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IActorIndicatorWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "CoreMinimal.h"
   9: #include "UObject/ObjectMacros.h"
  12: #include "UObject/Interface.h"
  14: #include "IActorIndicatorWidget.generated.h"
  17: class AActor;
  21: class UIndicatorDescriptor;
  25: UINTERFACE(BlueprintType)
  26: class HODGEPODGE_API UIndicatorWidgetInterface : public UInterface
  27: {
  28: 	GENERATED_BODY()
  29: };
  33: class IIndicatorWidgetInterface
  34: {
  35: 	GENERATED_BODY()
  37: public:
  44: 	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
  45: 	void BindIndicator(UIndicatorDescriptor* Indicator);
  52: 	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
  53: 	void UnbindIndicator(const UIndicatorDescriptor* Indicator);
  54: };
```

## IndicatorDescriptor.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   7: #include "Components/SceneComponent.h"
  10: #include "Types/SlateEnums.h"
  12: #include "IndicatorDescriptor.generated.h"
  15: class SWidget;
  18: class UIndicatorDescriptor;
  21: class UHodgeIndicatorManagerComponent;
  24: class UUserWidget;
  26: struct FFrame;
  30: struct FSceneViewProjectionData;
  35: struct FIndicatorProjection
  36: {
  41: 	bool Project(const UIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData,
  42: 	             const FVector2f& ScreenSize, FVector& ScreenPositionWithDepth);
  43: };
  47: UENUM(BlueprintType)
  48: enum class EActorCanvasProjectionMode : uint8
  49: {
  51: 	ComponentPoint,
  54: 	ComponentBoundingBox,
  57: 	ComponentScreenBoundingBox,
  60: 	ActorBoundingBox,
  63: 	ActorScreenBoundingBox
  64: };
  87: UCLASS(BlueprintType)
  88: class HODGEPODGE_API UIndicatorDescriptor : public UObject
  89: {
  90: 	GENERATED_BODY()
  92: public:
  94: 	UIndicatorDescriptor()
  95: 	{
  96: 	}
  98: public:
 102: 	UFUNCTION(BlueprintCallable)
 103: 	UObject* GetDataObject() const { return DataObject; }
 106: 	UFUNCTION(BlueprintCallable)
 107: 	void SetDataObject(UObject* InDataObject) { DataObject = InDataObject; }
 110: 	UFUNCTION(BlueprintCallable)
 111: 	USceneComponent* GetSceneComponent() const { return Component; }
 114: 	UFUNCTION(BlueprintCallable)
 115: 	void SetSceneComponent(USceneComponent* InComponent) { Component = InComponent; }
 119: 	UFUNCTION(BlueprintCallable)
 120: 	FName GetComponentSocketName() const { return ComponentSocketName; }
 123: 	UFUNCTION(BlueprintCallable)
 124: 	void SetComponentSocketName(FName SocketName) { ComponentSocketName = SocketName; }
 128: 	UFUNCTION(BlueprintCallable)
 129: 	TSoftClassPtr<UUserWidget> GetIndicatorClass() const { return IndicatorWidgetClass; }
 132: 	UFUNCTION(BlueprintCallable)
 133: 	void SetIndicatorClass(TSoftClassPtr<UUserWidget> InIndicatorWidgetClass)
 134: 	{
 135: 		IndicatorWidgetClass = InIndicatorWidgetClass;
 136: 	}
 138: public:
 142: 	TWeakObjectPtr<UUserWidget> IndicatorWidget;
 144: public:
 146: 	UFUNCTION(BlueprintCallable)
 147: 	void SetAutoRemoveWhenIndicatorComponentIsNull(bool CanAutomaticallyRemove)
 148: 	{
 149: 		bAutoRemoveWhenIndicatorComponentIsNull = CanAutomaticallyRemove;
 150: 	}
 153: 	UFUNCTION(BlueprintCallable)
 154: 	bool GetAutoRemoveWhenIndicatorComponentIsNull() const { return bAutoRemoveWhenIndicatorComponentIsNull; }
 157: 	bool CanAutomaticallyRemove() const
 158: 	{
 160: 		return bAutoRemoveWhenIndicatorComponentIsNull && !IsValid(GetSceneComponent());
 161: 	}
 163: public:
 170: 	UFUNCTION(BlueprintCallable)
 171: 	bool GetIsVisible() const { return IsValid(GetSceneComponent()) && bVisible; }
 174: 	UFUNCTION(BlueprintCallable)
 175: 	void SetDesiredVisibility(bool InVisible)
 176: 	{
 177: 		bVisible = InVisible;
 178: 	}
 181: 	UFUNCTION(BlueprintCallable)
 182: 	EActorCanvasProjectionMode GetProjectionMode() const { return ProjectionMode; }
 185: 	UFUNCTION(BlueprintCallable)
 186: 	void SetProjectionMode(EActorCanvasProjectionMode InProjectionMode)
 187: 	{
 188: 		ProjectionMode = InProjectionMode;
 189: 	}
 193: 	UFUNCTION(BlueprintCallable)
 194: 	EHorizontalAlignment GetHAlign() const { return HAlignment; }
 197: 	UFUNCTION(BlueprintCallable)
 198: 	void SetHAlign(EHorizontalAlignment InHAlignment)
 199: 	{
 200: 		HAlignment = InHAlignment;
 201: 	}
 205: 	UFUNCTION(BlueprintCallable)
 206: 	EVerticalAlignment GetVAlign() const { return VAlignment; }
 209: 	UFUNCTION(BlueprintCallable)
 210: 	void SetVAlign(EVerticalAlignment InVAlignment)
 211: 	{
 212: 		VAlignment = InVAlignment;
 213: 	}
 217: 	UFUNCTION(BlueprintCallable)
 218: 	bool GetClampToScreen() const { return bClampToScreen; }
 221: 	UFUNCTION(BlueprintCallable)
 222: 	void SetClampToScreen(bool bValue)
 223: 	{
 224: 		bClampToScreen = bValue;
 225: 	}
 229: 	UFUNCTION(BlueprintCallable)
 230: 	bool GetShowClampToScreenArrow() const { return bShowClampToScreenArrow; }
 233: 	UFUNCTION(BlueprintCallable)
 234: 	void SetShowClampToScreenArrow(bool bValue)
 235: 	{
 236: 		bShowClampToScreenArrow = bValue;
 237: 	}
 242: 	UFUNCTION(BlueprintCallable)
 243: 	FVector GetWorldPositionOffset() const { return WorldPositionOffset; }
 246: 	UFUNCTION(BlueprintCallable)
 247: 	void SetWorldPositionOffset(FVector Offset)
 248: 	{
 249: 		WorldPositionOffset = Offset;
 250: 	}
 255: 	UFUNCTION(BlueprintCallable)
 256: 	FVector2D GetScreenSpaceOffset() const { return ScreenSpaceOffset; }
 259: 	UFUNCTION(BlueprintCallable)
 260: 	void SetScreenSpaceOffset(FVector2D Offset)
 261: 	{
 262: 		ScreenSpaceOffset = Offset;
 263: 	}
 268: 	UFUNCTION(BlueprintCallable)
 269: 	FVector GetBoundingBoxAnchor() const { return BoundingBoxAnchor; }
 272: 	UFUNCTION(BlueprintCallable)
 273: 	void SetBoundingBoxAnchor(FVector InBoundingBoxAnchor)
 274: 	{
 275: 		BoundingBoxAnchor = InBoundingBoxAnchor;
 276: 	}
 278: public:
 287: 	UFUNCTION(BlueprintCallable)
 288: 	int32 GetPriority() const { return Priority; }
 291: 	UFUNCTION(BlueprintCallable)
 292: 	void SetPriority(int32 InPriority)
 293: 	{
 294: 		Priority = InPriority;
 295: 	}
 297: public:
 299: 	UHodgeIndicatorManagerComponent* GetIndicatorManagerComponent() { return ManagerPtr.Get(); }
 302: 	void SetIndicatorManagerComponent(UHodgeIndicatorManagerComponent* InManager);
 305: 	UFUNCTION(BlueprintCallable)
 306: 	void UnregisterIndicator();
 308: private:
 310: 	UPROPERTY()
 311: 	bool bVisible = true;
 314: 	UPROPERTY()
 315: 	bool bClampToScreen = false;
 318: 	UPROPERTY()
 319: 	bool bShowClampToScreenArrow = false;
 322: 	UPROPERTY()
 323: 	bool bOverrideScreenPosition = false;
 326: 	UPROPERTY()
 327: 	bool bAutoRemoveWhenIndicatorComponentIsNull = false;
 330: 	UPROPERTY()
 331: 	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;
 334: 	UPROPERTY()
 335: 	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;
 338: 	UPROPERTY()
 339: 	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;
 342: 	UPROPERTY()
 343: 	int32 Priority = 0;
 347: 	UPROPERTY()
 348: 	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5);
 351: 	UPROPERTY()
 352: 	FVector2D ScreenSpaceOffset = FVector2D(0, 0);
 355: 	UPROPERTY()
 356: 	FVector WorldPositionOffset = FVector(0, 0, 0);
 358: private:
 361: 	friend class SActorCanvas;
 365: 	UPROPERTY()
 366: 	TObjectPtr<UObject> DataObject;
 369: 	UPROPERTY()
 370: 	TObjectPtr<USceneComponent> Component;
 374: 	UPROPERTY()
 375: 	FName ComponentSocketName = NAME_None;
 379: 	UPROPERTY()
 380: 	TSoftClassPtr<UUserWidget> IndicatorWidgetClass;
 384: 	UPROPERTY()
 385: 	TWeakObjectPtr<UHodgeIndicatorManagerComponent> ManagerPtr;
 389: 	TWeakPtr<SWidget> Content;
 393: 	TWeakPtr<SWidget> CanvasHost;
 394: };
```

## IndicatorLayer.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   7: #include "Components/Widget.h"
   9: #include "IndicatorLayer.generated.h"
  13: class SActorCanvas;
  16: class SWidget;
  18: class UObject;
  23: UCLASS()
  24: class UIndicatorLayer : public UWidget
  25: {
  26: 	GENERATED_UCLASS_BODY()
  28: public:
  31: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
  32: 	FSlateBrush ArrowBrush;
  34: protected:
  39: 	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
  43: 	virtual TSharedRef<SWidget> RebuildWidget() override;
  47: protected:
  51: 	TSharedPtr<SActorCanvas> MyActorCanvas;
  52: };
```

## IndicatorLibrary.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLibrary.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLibrary.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   7: #include "Kismet/BlueprintFunctionLibrary.h"
   9: #include "IndicatorLibrary.generated.h"
  13: class AController;
  17: class UHodgeIndicatorManagerComponent;
  19: class UObject;
  20: struct FFrame;
  24: UCLASS()
  25: class HODGEPODGE_API UIndicatorLibrary : public UBlueprintFunctionLibrary
  26: {
  27: 	GENERATED_BODY()
  29: public:
  31: 	UIndicatorLibrary();
  37: 	UFUNCTION(BlueprintCallable, Category = Indicator)
  38: 	static UHodgeIndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);
  39: };
```

## SActorCanvas.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
  10: #include "Blueprint/UserWidgetPool.h"
  14: #include "Widgets/SPanel.h"
  18: class FActiveTimerHandle;
  21: class FArrangedChildren;
  24: class FChildren;
  27: class FPaintArgs;
  31: class FReferenceCollector;
  34: class FSlateRect;
  37: class FSlateWindowElementList;
  40: class FWidgetStyle;
  44: struct FStreamableHandle;
  47: class UIndicatorDescriptor;
  50: class UHodgeIndicatorManagerComponent;
  54: struct FSlateBrush;
  65: class SActorCanvas : public SPanel, public FGCObject
  66: {
  67: public:
  74: 	class FSlot : public TSlotBase<FSlot>
  75: 	{
  76: 	public:
  78: 		FSlot(UIndicatorDescriptor* InIndicator)
  79: 			: TSlotBase<FSlot>()
  81: 			  , Indicator(InIndicator)
  83: 			  , ScreenPosition(FVector2D::ZeroVector)
  85: 			  , Depth(0)
  87: 			  , Priority(0.f)
  89: 			  , bIsIndicatorVisible(true)
  91: 			  , bInFrontOfCamera(true)
  93: 			  , bHasValidScreenPosition(false)
  95: 			  , bDirty(true)
  97: 			  , bWasIndicatorClamped(false)
  99: 			  , bWasIndicatorClampedStatusChanged(false)
 100: 		{
 101: 		}
 104: 		SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
 105: 		SLATE_SLOT_END_ARGS()
 108: 		using TSlotBase<FSlot>::Construct;
 111: 		bool GetIsIndicatorVisible() const { return bIsIndicatorVisible; }
 114: 		void SetIsIndicatorVisible(bool bVisible)
 115: 		{
 117: 			if (bIsIndicatorVisible != bVisible)
 118: 			{
 119: 				bIsIndicatorVisible = bVisible;
 120: 				bDirty = true;
 121: 			}
 124: 			RefreshVisibility();
 125: 		}
 128: 		FVector2D GetScreenPosition() const { return ScreenPosition; }
 131: 		void SetScreenPosition(FVector2D InScreenPosition)
 132: 		{
 134: 			if (ScreenPosition != InScreenPosition)
 135: 			{
 136: 				ScreenPosition = InScreenPosition;
 137: 				bDirty = true;
 138: 			}
 139: 		}
 142: 		double GetDepth() const { return Depth; }
 145: 		void SetDepth(double InDepth)
 146: 		{
 149: 			if (Depth != InDepth)
 150: 			{
 151: 				Depth = InDepth;
 152: 				bDirty = true;
 153: 			}
 154: 		}
 157: 		int32 GetPriority() const { return Priority; }
 160: 		void SetPriority(int32 InPriority)
 161: 		{
 164: 			if (Priority != InPriority)
 165: 			{
 166: 				Priority = InPriority;
 167: 				bDirty = true;
 168: 			}
 169: 		}
 172: 		bool GetInFrontOfCamera() const { return bInFrontOfCamera; }
 175: 		void SetInFrontOfCamera(bool bInFront)
 176: 		{
 178: 			if (bInFrontOfCamera != bInFront)
 179: 			{
 180: 				bInFrontOfCamera = bInFront;
 181: 				bDirty = true;
 182: 			}
 185: 			RefreshVisibility();
 186: 		}
 189: 		bool HasValidScreenPosition() const { return bHasValidScreenPosition; }
 192: 		void SetHasValidScreenPosition(bool bValidScreenPosition)
 193: 		{
 195: 			if (bHasValidScreenPosition != bValidScreenPosition)
 196: 			{
 197: 				bHasValidScreenPosition = bValidScreenPosition;
 198: 				bDirty = true;
 199: 			}
 202: 			RefreshVisibility();
 203: 		}
 206: 		bool bIsDirty() const { return bDirty; }
 209: 		void ClearDirtyFlag()
 210: 		{
 211: 			bDirty = false;
 212: 		}
 215: 		bool WasIndicatorClamped() const { return bWasIndicatorClamped; }
 218: 		void SetWasIndicatorClamped(bool bWasClamped) const
 219: 		{
 221: 			if (bWasClamped != bWasIndicatorClamped)
 222: 			{
 223: 				bWasIndicatorClamped = bWasClamped;
 224: 				bWasIndicatorClampedStatusChanged = true;
 225: 			}
 226: 		}
 229: 		bool WasIndicatorClampedStatusChanged() const { return bWasIndicatorClampedStatusChanged; }
 232: 		void ClearIndicatorClampedStatusChangedFlag()
 233: 		{
 234: 			bWasIndicatorClampedStatusChanged = false;
 235: 		}
 237: 	private:
 240: 		void RefreshVisibility()
 241: 		{
 244: 			const bool bIsVisible = bIsIndicatorVisible && bHasValidScreenPosition;
 251: 			GetWidget()->SetVisibility(bIsVisible ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed);
 252: 		}
 258: 		UIndicatorDescriptor* Indicator;
 261: 		FVector2D ScreenPosition;
 264: 		double Depth;
 267: 		int32 Priority;
 270: 		uint8 bIsIndicatorVisible : 1;
 273: 		uint8 bInFrontOfCamera : 1;
 276: 		uint8 bHasValidScreenPosition : 1;
 279: 		uint8 bDirty : 1;
 289: 		mutable uint8 bWasIndicatorClamped : 1;
 292: 		mutable uint8 bWasIndicatorClampedStatusChanged : 1;
 295: 		friend class SActorCanvas;
 296: 	};
 301: 	class FArrowSlot : public TSlotBase<FArrowSlot>
 302: 	{
 303: 	};
 307: 	SLATE_BEGIN_ARGS(SActorCanvas)
 308: 		{
 311: 			_Visibility = EVisibility::HitTestInvisible;
 312: 		}
 316: 		SLATE_SLOT_ARGUMENT(SActorCanvas::FSlot, Slots)
 319: 	SLATE_END_ARGS()
 322: 	SActorCanvas()
 324: 		: CanvasChildren(this)
 326: 		  , ArrowChildren(this)
 328: 		  , AllChildren(this)
 329: 	{
 331: 		AllChildren.AddChildren(CanvasChildren);
 334: 		AllChildren.AddChildren(ArrowChildren);
 335: 	}
 339: 	~SActorCanvas();
 352: 	void Construct(const FArguments& InArgs, const FLocalPlayerContext& InCtx,
 353: 	               const FSlateBrush* ActorCanvasArrowBrush);
 359: 	virtual void
 360: 	OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
 364: 	virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }
 368: 	virtual FChildren* GetChildren() override { return &AllChildren; }
 372: 	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
 373: 	                      FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
 374: 	                      bool bParentEnabled) const;
 380: 	void SetDrawElementsInOrder(bool bInDrawElementsInOrder) { bDrawElementsInOrder = bInDrawElementsInOrder; }
 384: 	virtual FString GetReferencerName() const override;
 389: 	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
 391: private:
 393: 	void OnIndicatorAdded(UIndicatorDescriptor* Indicator);
 396: 	void OnIndicatorRemoved(UIndicatorDescriptor* Indicator);
 400: 	void AddIndicatorForEntry(UIndicatorDescriptor* Indicator);
 403: 	void RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator);
 407: 	void OnIndicatorClassLoaded(TWeakObjectPtr<UIndicatorDescriptor> IndicatorPtr);
 410: 	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;
 413: 	FScopedWidgetSlotArguments AddActorSlot(UIndicatorDescriptor* Indicator);
 416: 	int32 RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget);
 420: 	void SetShowAnyIndicators(bool bIndicators);
 424: 	EActiveTimerReturnType UpdateCanvas(double InCurrentTime, float InDeltaTime);
 429: 	void GetOffsetAndSize(const UIndicatorDescriptor* Indicator,
 430: 	                      FVector2D& OutSize,
 431: 	                      FVector2D& OutOffset,
 432: 	                      FVector2D& OutPaddingMin,
 433: 	                      FVector2D& OutPaddingMax) const;
 437: 	void UpdateActiveTimer();
 439: private:
 442: 	TArray<TObjectPtr<UIndicatorDescriptor>> AllIndicators;
 446: 	TArray<UIndicatorDescriptor*> InactiveIndicators;
 450: 	FLocalPlayerContext LocalPlayerContext;
 454: 	TWeakObjectPtr<UHodgeIndicatorManagerComponent> IndicatorComponentPtr;
 458: 	TPanelChildren<FSlot> CanvasChildren;
 462: 	mutable TPanelChildren<FArrowSlot> ArrowChildren;
 466: 	FCombinedChildren AllChildren;
 470: 	FUserWidgetPool IndicatorPool;
 474: 	const FSlateBrush* ActorCanvasArrowBrush = nullptr;
 478: 	mutable int32 NextArrowIndex = 0;
 482: 	mutable int32 ArrowIndexLastUpdate = 0;
 488: 	bool bDrawElementsInOrder = false;
 492: 	bool bShowAnyIndicators = false;
 496: 	mutable TOptional<FGeometry> OptionalPaintGeometry;
 499: 	TSharedPtr<FActiveTimerHandle> TickHandle;
 504: 	TArray<TSharedPtr<FStreamableHandle>> IndicatorLoadHandles;
 505: };
```

## HodgePerfStatContainerBase.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/PerformanceStats/HodgePerfStatContainerBase.h](../../../Source/Hodgepodge/Public/UI/PerformanceStats/HodgePerfStatContainerBase.h)

**全部为注释或空白；无有效声明/实现。**

## HodgePerfStatWidgetBase.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/PerformanceStats/HodgePerfStatWidgetBase.h](../../../Source/Hodgepodge/Public/UI/PerformanceStats/HodgePerfStatWidgetBase.h)

**全部为注释或空白；无有效声明/实现。**

## HodgeUIManagerSubsystem.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

**全部为注释或空白；无有效声明/实现。**

## HodgeUIMessaging.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Subsystem/HodgeUIMessaging.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIMessaging.h)

**全部为注释或空白；无有效声明/实现。**

## CircumferenceMarkerWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Weapons/CircumferenceMarkerWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/CircumferenceMarkerWidget.h)

项目内直接 include（不是运行调用关系）：[UI/Weapons/SCircumferenceMarkerWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/SCircumferenceMarkerWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "Components/Widget.h"
   6: #include "UI/Weapons/SCircumferenceMarkerWidget.h"
   8: #include "CircumferenceMarkerWidget.generated.h"
  10: class SWidget;
  11: class UObject;
  12: struct FFrame;
  14: UCLASS()
  15: class UCircumferenceMarkerWidget : public UWidget
  16: {
  17: 	GENERATED_BODY()
  19: public:
  20: 	UCircumferenceMarkerWidget(const FObjectInitializer& ObjectInitializer);
  23: public:
  24: 	virtual void SynchronizeProperties() override;
  25: protected:
  26: 	virtual TSharedRef<SWidget> RebuildWidget() override;
  30: public:
  31: 	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
  34: public:
  36: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
  37: 	TArray<FCircumferenceMarkerEntry> MarkerList;
  40: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=(ClampMin=0.0))
  41: 	float Radius = 48.0f;
  44: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
  45: 	FSlateBrush MarkerImage;
  49: 	UPROPERTY(EditAnywhere, Category=Corner)
  50: 	uint8 bReticleCornerOutsideSpreadRadius : 1;
  52: public:
  54: 	UFUNCTION(BlueprintCallable, Category = "Appearance")
  55: 	void SetRadius(float InRadius);
  57: private:
  59: 	TSharedPtr<SCircumferenceMarkerWidget> MyMarkerWidget;
  60: };
```

## HitMarkerConfirmationWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Weapons/HitMarkerConfirmationWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/HitMarkerConfirmationWidget.h)

**全部为注释或空白；无有效声明/实现。**

## HodgeReticleWidgetBase.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Weapons/HodgeReticleWidgetBase.h](../../../Source/Hodgepodge/Public/UI/Weapons/HodgeReticleWidgetBase.h)

**全部为注释或空白；无有效声明/实现。**

## HodgeWeaponUserInterface.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Weapons/HodgeWeaponUserInterface.h](../../../Source/Hodgepodge/Public/UI/Weapons/HodgeWeaponUserInterface.h)

**全部为注释或空白；无有效声明/实现。**

## SCircumferenceMarkerWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Weapons/SCircumferenceMarkerWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/SCircumferenceMarkerWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "Styling/CoreStyle.h"
   6: #include "Widgets/Accessibility/SlateWidgetAccessibleTypes.h"
   7: #include "Widgets/DeclarativeSyntaxSupport.h"
   8: #include "Widgets/SLeafWidget.h"
  10: #include "SCircumferenceMarkerWidget.generated.h"
  12: class FPaintArgs;
  13: class FSlateRect;
  14: class FSlateWindowElementList;
  15: class FWidgetStyle;
  16: struct FGeometry;
  17: struct FSlateBrush;
  19: USTRUCT(BlueprintType)
  20: struct FCircumferenceMarkerEntry
  21: {
  22: 	GENERATED_BODY()
  25: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ForceUnits=deg))
  26: 	float PositionAngle = 0.0f;
  29: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ForceUnits=deg))
  30: 	float ImageRotationAngle = 0.0f;
  31: };
  33: class SCircumferenceMarkerWidget : public SLeafWidget
  34: {
  35: 	SLATE_BEGIN_ARGS(SCircumferenceMarkerWidget)
  36: 		: _MarkerBrush(FCoreStyle::Get().GetBrush("Throbber.CircleChunk"))
  37: 		, _Radius(48.0f)
  38: 	{
  39: 	}
  41: 		SLATE_ARGUMENT(const FSlateBrush*, MarkerBrush)
  43: 		SLATE_ARGUMENT(TArray<FCircumferenceMarkerEntry>, MarkerList)
  45: 		SLATE_ATTRIBUTE(float, Radius)
  47: 		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
  48: 	SLATE_END_ARGS()
  50: public:
  51: 	void Construct(const FArguments& InArgs);
  53: 	SCircumferenceMarkerWidget();
  56: 	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
  57: 	virtual FVector2D ComputeDesiredSize(float) const override;
  58: 	virtual bool ComputeVolatility() const override { return true; }
  61: 	void SetRadius(float NewRadius);
  62: 	void SetMarkerList(TArray<FCircumferenceMarkerEntry>& NewMarkerList);
  64: private:
  65: 	FSlateRenderTransform GetMarkerRenderTransform(const FCircumferenceMarkerEntry& Marker, const float BaseRadius, const float HUDScale) const;
  67: private:
  69: 	const FSlateBrush* MarkerBrush;
  72: 	TArray<FCircumferenceMarkerEntry> MarkerList;
  75: 	TAttribute<float> Radius;
  78: 	TAttribute<FSlateColor> ColorAndOpacity;
  79: 	bool bColorAndOpacitySet;
  83: 	uint8 bReticleCornerOutsideSpreadRadius : 1;
  84: };
```

## SHitMarkerConfirmationWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Weapons/SHitMarkerConfirmationWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/SHitMarkerConfirmationWidget.h)

**全部为注释或空白；无有效声明/实现。**
