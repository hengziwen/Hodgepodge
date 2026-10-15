# UI 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## MaterialProgressBar.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Basic/MaterialProgressBar.cpp](../../../Source/Hodgepodge/Private/UI/Basic/MaterialProgressBar.cpp)

项目内直接 include（不是运行调用关系）：[UI/Basic/MaterialProgressBar.h](../../../Source/Hodgepodge/Public/UI/Basic/MaterialProgressBar.h)

定义候选（多行签名仅展示首行）：

- L15: `void UMaterialProgressBar::SynchronizeProperties()`
- L86: `void UMaterialProgressBar::OnWidgetRebuilt()`
- L146: `void UMaterialProgressBar::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)`
- L158: `void UMaterialProgressBar::SetProgress(float Progress)`
- L167: `void UMaterialProgressBar::SetStartProgress(float StartProgress)`
- L176: `void UMaterialProgressBar::SetColorA(FLinearColor ColorA)`
- L185: `void UMaterialProgressBar::SetColorB(FLinearColor ColorB)`
- L194: `void UMaterialProgressBar::SetColorBackground(FLinearColor ColorBackground)`
- L203: `void UMaterialProgressBar::AnimateProgressFromStart(float Start, float End, float AnimSpeed)`
- L211: `void UMaterialProgressBar::AnimateProgressFromCurrent(float End, float AnimSpeed)`
- L224: `void UMaterialProgressBar::SetProgress_Internal(float Progress)`
- L234: `void UMaterialProgressBar::SetStartProgress_Internal(float StartProgress)`
- L244: `void UMaterialProgressBar::SetColorA_Internal(FLinearColor ColorA)`
- L254: `void UMaterialProgressBar::SetColorB_Internal(FLinearColor ColorB)`
- L264: `void UMaterialProgressBar::SetColorBackground_Internal(FLinearColor ColorBackground)`
- L274: `UMaterialInstanceDynamic* UMaterialProgressBar::GetBarDynamicMaterial() const`

## HodgeBoundActionButton.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeBoundActionButton.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeBoundActionButton.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeBoundActionButton.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeBoundActionButton.h)

定义候选（多行签名仅展示首行）：

- L16: `void UHodgeBoundActionButton::NativeConstruct()`
- L29: `void UHodgeBoundActionButton::HandleInputMethodChanged(ECommonInputType NewInputMethod)`

## HodgeListView.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeListView.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeListView.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeListView.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeListView.h)、[UI/Common/HodgeWidgetFactory.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h)

定义候选（多行签名仅展示首行）：

- L18: `UHodgeListView::UHodgeListView(const FObjectInitializer& ObjectInitializer)`
- L26: `void UHodgeListView::ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const`
- L41: `UUserWidget& UHodgeListView::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass,`

## HodgeTabButtonBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeTabButtonBase.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeTabButtonBase.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeTabButtonBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabButtonBase.h)、[UI/Common/HodgeTabListWidgetBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h)

定义候选（多行签名仅展示首行）：

- L17: `void UHodgeTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)`
- L26: `void UHodgeTabButtonBase::SetIconBrush(const FSlateBrush& Brush)`
- L35: `void UHodgeTabButtonBase::SetTabLabelInfo_Implementation(const FHodgeTabDescriptor& TabLabelInfo)`

## HodgeTabListWidgetBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeTabListWidgetBase.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeTabListWidgetBase.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeTabListWidgetBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h)

定义候选（多行签名仅展示首行）：

- L14: `void UHodgeTabListWidgetBase::NativeOnInitialized()`
- L20: `void UHodgeTabListWidgetBase::NativeConstruct()`
- L28: `void UHodgeTabListWidgetBase::NativeDestruct()`
- L43: `bool UHodgeTabListWidgetBase::GetPreregisteredTabInfo(const FName TabNameId, FHodgeTabDescriptor& OutTabInfo)`
- L61: `void UHodgeTabListWidgetBase::SetTabHiddenState(FName TabNameId, bool bHidden)`
- L74: `bool UHodgeTabListWidgetBase::RegisterDynamicTab(const FHodgeTabDescriptor& TabDescriptor)`
- L88: `void UHodgeTabListWidgetBase::HandlePreLinkedSwitcherChanged()`
- L103: `void UHodgeTabListWidgetBase::HandlePostLinkedSwitcherChanged()`
- L116: `void UHodgeTabListWidgetBase::HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton)`
- L146: `bool UHodgeTabListWidgetBase::IsFirstTabActive() const`
- L157: `bool UHodgeTabListWidgetBase::IsLastTabActive() const`
- L168: `bool UHodgeTabListWidgetBase::IsTabVisible(FName TabId)`
- L182: `int32 UHodgeTabListWidgetBase::GetVisibleTabCount()`
- L198: `void UHodgeTabListWidgetBase::SetupTabs()`

## HodgeWidgetFactory.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeWidgetFactory.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h)

定义候选（多行签名仅展示首行）：

- L14: `TSubclassOf<UUserWidget> UHodgeWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const`

## HodgeWidgetFactory_Class.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory_Class.cpp](../../../Source/Hodgepodge/Private/UI/Common/HodgeWidgetFactory_Class.cpp)

项目内直接 include（不是运行调用关系）：[UI/Common/HodgeWidgetFactory_Class.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory_Class.h)

定义候选（多行签名仅展示首行）：

- L12: `TSubclassOf<UUserWidget> UHodgeWidgetFactory_Class::FindWidgetClassForData_Implementation(const UObject* Data) const`

## HodgeGameplayUIDataSource.cpp

每个本地玩家共享的生命订阅、属性就绪与技能状态/输入接入；布局和按钮行为保存在 WBP。

源码：[Source/Hodgepodge/Private/UI/Data/HodgeGameplayUIDataSource.cpp](../../../Source/Hodgepodge/Private/UI/Data/HodgeGameplayUIDataSource.cpp)

项目内直接 include（不是运行调用关系）：[UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)

定义候选（多行签名仅展示首行）：

- L24: `void UHodgeGameplayUIDataSource::Initialize(UHodgeLocalPlayerBase* Player, APlayerController* InController)`
- L37: `void UHodgeGameplayUIDataSource::UnbindPawn()`
- L50: `void UHodgeGameplayUIDataSource::UnbindState()`
- L68: `void UHodgeGameplayUIDataSource::Shutdown()`
- L84: `void UHodgeGameplayUIDataSource::StateChanged(UHodgeLocalPlayerBase* Player, APlayerState* State)`
- L106: `void UHodgeGameplayUIDataSource::PawnChanged(UHodgeLocalPlayerBase* Player, APawn* Pawn)`
- L130: `void UHodgeGameplayUIDataSource::Refresh()`
- L165: `void UHodgeGameplayUIDataSource::ResonanceAttributeChanged(const FOnAttributeChangeData& Data) { Refresh(); }`
- L167: `void UHodgeGameplayUIDataSource::HavocModeChanged(FGameplayTag Tag,int32 Count) { Refresh(); }`
- L170: `void UHodgeGameplayUIDataSource::AttributeChanged(UHodgeHealthComponent* Component, float Old, float New, AActor* Instigator) { Refresh(); }`
- L172: `void UHodgeGameplayUIDataSource::DeathChanged(AActor* Actor) { Refresh(); }`
- L175: `bool UHodgeGameplayUIDataSource::CanUseGameplay() const`
- L183: `FHodgeUIAbilityDisplayState UHodgeGameplayUIDataSource::GetAbilityDisplayState(FGameplayTag InputTag) const`
- L222: `bool UHodgeGameplayUIDataSource::SubmitInput(FGameplayTag InputTag)`

## UIExtensionPointWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp](../../../Source/Hodgepodge/Private/UI/Extension/UIExtensionPointWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/Extension/UIExtensionPointWidget.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)

定义候选（多行签名仅展示首行）：

- L42: `UUIExtensionPointWidget::UUIExtensionPointWidget(const FObjectInitializer& ObjectInitializer)`
- L48: `void UUIExtensionPointWidget::SetExtensionPointTag(FGameplayTag Tag)`
- L55: `void UUIExtensionPointWidget::ReleaseSlateResources(bool bReleaseChildren)`
- L66: `TSharedRef<SWidget> UUIExtensionPointWidget::RebuildWidget()`
- L137: `void UUIExtensionPointWidget::ResetExtensionPoint()`
- L162: `void UUIExtensionPointWidget::RegisterExtensionPoint()`
- L206: `void UUIExtensionPointWidget::RegisterExtensionPointForPlayerState(UHodgeLocalPlayerBase* LocalPlayer,`
- L236: `void UUIExtensionPointWidget::OnAddOrRemoveExtension(EUIExtensionAction Action, const FUIExtensionRequest& Request)`
- L319: `void UUIExtensionPointWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const`

## UIExtensionSystem.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Extension/UIExtensionSystem.cpp](../../../Source/Hodgepodge/Private/UI/Extension/UIExtensionSystem.cpp)

项目内直接 include（不是运行调用关系）：[UI/Extension/UIExtensionSystem.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionSystem.h)

定义候选（多行签名仅展示首行）：

- L27: `void FUIExtensionPointHandle::Unregister()`
- L41: `void FUIExtensionHandle::Unregister()`
- L55: `bool FUIExtensionPoint::DoesExtensionPassContract(const FUIExtension* Extension) const`
- L104: `void UUIExtensionSubsystem::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)`
- L139: `void UUIExtensionSubsystem::Initialize(FSubsystemCollectionBase& Collection)`
- L146: `void UUIExtensionSubsystem::Deinitialize()`
- L153: `FUIExtensionPointHandle UUIExtensionSubsystem::RegisterExtensionPoint(const FGameplayTag& ExtensionPointTag,`
- L165: `FUIExtensionPointHandle UUIExtensionSubsystem::RegisterExtensionPointForContext(`
- L228: `FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsWidget(const FGameplayTag& ExtensionPointTag,`
- L238: `FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsWidgetForContext(`
- L247: `FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsData(const FGameplayTag& ExtensionPointTag,`
- L310: `void UUIExtensionSubsystem::NotifyExtensionPointOfExtensions(TSharedPtr<FUIExtensionPoint>& ExtensionPoint)`
- L360: `void UUIExtensionSubsystem::NotifyExtensionPointsOfExtension(EUIExtensionAction Action,`
- L415: `void UUIExtensionSubsystem::UnregisterExtension(const FUIExtensionHandle& ExtensionHandle)`
- L469: `void UUIExtensionSubsystem::UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle)`
- L505: `FUIExtensionRequest UUIExtensionSubsystem::CreateExtensionRequest(const TSharedPtr<FUIExtension>& Extension)`
- L529: `FUIExtensionPointHandle UUIExtensionSubsystem::K2_RegisterExtensionPoint(`
- L547: `FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsWidget(FGameplayTag ExtensionPointTag,`
- L556: `FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsWidgetForContext(`
- L575: `FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsData(FGameplayTag ExtensionPointTag, UObject* Data,`
- L583: `FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsDataForContext(`
- L603: `void UUIExtensionHandleFunctions::Unregister(FUIExtensionHandle& Handle)`
- L609: `bool UUIExtensionHandleFunctions::IsValid(FUIExtensionHandle& Handle)`
- L617: `void UUIExtensionPointHandleFunctions::Unregister(FUIExtensionPointHandle& Handle)`
- L623: `bool UUIExtensionPointHandleFunctions::IsValid(FUIExtensionPointHandle& Handle)`

## HodgeActionWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgeActionWidget.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeActionWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgeActionWidget.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeActionWidget.h)

定义候选（多行签名仅展示首行）：

- L14: `FSlateBrush UHodgeActionWidget::GetIcon() const`
- L40: `UEnhancedInputLocalPlayerSubsystem* UHodgeActionWidget::GetEnhancedInputSubsystem() const`

## HodgeButtonBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgeButtonBase.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeButtonBase.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgeButtonBase.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h)

定义候选（多行签名仅展示首行）：

- L12: `void UHodgeButtonBase::NativePreConstruct()`
- L21: `void UHodgeButtonBase::UpdateInputActionWidget()`
- L30: `void UHodgeButtonBase::SetButtonText(const FText& InText)`
- L38: `void UHodgeButtonBase::RefreshButtonText()`
- L57: `void UHodgeButtonBase::OnInputMethodChanged(ECommonInputType CurrentInputType)`

## HodgeConfirmationScreen.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgeConfirmationScreen.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeConfirmationScreen.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeControllerDisconnectedScreen.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgeControllerDisconnectedScreen.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeControllerDisconnectedScreen.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgeControllerDisconnectedScreen.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeControllerDisconnectedScreen.h)

定义候选（多行签名仅展示首行）：

- L27: `UHodgeControllerDisconnectedScreen::UHodgeControllerDisconnectedScreen(const FObjectInitializer& ObjectInitializer)`
- L35: `void UHodgeControllerDisconnectedScreen::NativeOnActivated()`
- L71: `bool UHodgeControllerDisconnectedScreen::ShouldDisplayChangeUserButton() const`
- L87: `void UHodgeControllerDisconnectedScreen::HandleChangeUserClicked()`
- L106: `void UHodgeControllerDisconnectedScreen::HandleChangeUserCompleted(const FPlatformUserSelectionCompleteParams& Params)`

## HodgeLoadingScreenSubsystem.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgeLoadingScreenSubsystem.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgeLoadingScreenSubsystem.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgeLoadingScreenSubsystem.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeLoadingScreenSubsystem.h)

定义候选（多行签名仅展示首行）：

- L18: `UHodgeLoadingScreenSubsystem::UHodgeLoadingScreenSubsystem()`
- L23: `void UHodgeLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)`
- L35: `TSubclassOf<UUserWidget> UHodgeLoadingScreenSubsystem::GetLoadingScreenContentWidget() const`

## HodgePrimaryGameLayout.cpp

绑定 Designer 四个容器的本地玩家根布局，Push/Pop、异步加载取消与令牌恢复；不再生成运行时控件树。

源码：[Source/Hodgepodge/Private/UI/Foundation/HodgePrimaryGameLayout.cpp](../../../Source/Hodgepodge/Private/UI/Foundation/HodgePrimaryGameLayout.cpp)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgePrimaryGameLayout.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L17: `void UHodgePrimaryGameLayout::NativeOnInitialized()`
- L28: `void UHodgePrimaryGameLayout::RegisterLayer(FGameplayTag Tag, UCommonActivatableWidgetContainerBase* Container)`
- L35: `UCommonActivatableWidgetContainerBase* UHodgePrimaryGameLayout::GetLayer(FGameplayTag Tag) const`
- L42: `UCommonActivatableWidget* UHodgePrimaryGameLayout::Push(FGameplayTag Tag, TSubclassOf<UCommonActivatableWidget> WidgetClass)`
- L52: `void UHodgePrimaryGameLayout::Pop(UCommonActivatableWidget* Widget)`
- L62: `bool UHodgePrimaryGameLayout::HasBlockingPage() const`
- L77: `FGuid UHodgePrimaryGameLayout::PushAsync(FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass,`
- L98: `void UHodgePrimaryGameLayout::FinishPush(FGuid Id, FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass)`
- L110: `void UHodgePrimaryGameLayout::CancelPush(FGuid Id)`
- L121: `void UHodgePrimaryGameLayout::ReleaseLayout()`
- L136: `void UHodgePrimaryGameLayout::NativeDestruct()`

## ApplyFrontendPerfSettingsAction.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Frontend/ApplyFrontendPerfSettingsAction.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/ApplyFrontendPerfSettingsAction.cpp)

项目内直接 include（不是运行调用关系）：[UI/Frontend/ApplyFrontendPerfSettingsAction.h](../../../Source/Hodgepodge/Public/UI/Frontend/ApplyFrontendPerfSettingsAction.h)

定义候选（多行签名仅展示首行）：

- L27: `void UApplyFrontendPerfSettingsAction::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)`
- L37: `void UApplyFrontendPerfSettingsAction::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`

## HodgeFrontendStateComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Frontend/HodgeFrontendStateComponent.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/HodgeFrontendStateComponent.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeLobbyBackground.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Frontend/HodgeLobbyBackground.cpp](../../../Source/Hodgepodge/Private/UI/Frontend/HodgeLobbyBackground.cpp)

项目内直接 include（不是运行调用关系）：[UI/Frontend/HodgeLobbyBackground.h](../../../Source/Hodgepodge/Public/UI/Frontend/HodgeLobbyBackground.h)

定义候选（多行签名仅展示首行）：


## HodgeAbilityIconWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeAbilityIconWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeAbilityIconWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeAbilityIconWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeAbilityIconWidget.h)、[UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

定义候选（多行签名仅展示首行）：

- L12: `FHodgeAbilityIconVisualState UHodgeAbilityIconWidget::EvaluateState(const FHodgeUIAbilityDisplayState& Ability,const FHodgeUIResonanceSnapshot& Resource,bool bEnergy)`
- L24: `void UHodgeAbilityIconWidget::NativeConstruct()`
- L31: `void UHodgeAbilityIconWidget::SynchronizeProperties()`
- L36: `void UHodgeAbilityIconWidget::ApplyStyle()`
- L54: `void UHodgeAbilityIconWidget::NativeTick(const FGeometry& Geometry,float DeltaTime)`
- L59: `void UHodgeAbilityIconWidget::RefreshDisplay()`
- L89: `void UHodgeAbilityIconWidget::NativeDestruct()`
- L95: `void UHodgeAbilityIconWidget::RequestAbility()`

## HodgeActivatableWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeActivatableWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeActivatableWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeActivatableWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h)

定义候选（多行签名仅展示首行）：

- L20: `UHodgeActivatableWidget::UHodgeActivatableWidget(const FObjectInitializer& ObjectInitializer)`
- L27: `TOptional<FUIInputConfig> UHodgeActivatableWidget::GetDesiredInputConfig() const`
- L65: `void UHodgeActivatableWidget::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree,`

## HodgeGameViewportClient.cpp

引擎 CommonUI 视口路由，已在 DefaultEngine.ini 启用。

源码：[Source/Hodgepodge/Private/UI/HodgeGameViewportClient.cpp](../../../Source/Hodgepodge/Private/UI/HodgeGameViewportClient.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeGameViewportClient.h](../../../Source/Hodgepodge/Public/UI/HodgeGameViewportClient.h)

定义候选（多行签名仅展示首行）：


## HodgeHUDLayout.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp](../../../Source/Hodgepodge/Private/UI/HodgeHUDLayout.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeHUDLayout.h](../../../Source/Hodgepodge/Public/UI/HodgeHUDLayout.h)、[UI/Foundation/HodgeControllerDisconnectedScreen.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeControllerDisconnectedScreen.h)、[UI/HodgeActivatableWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h)、[UI/Foundation/HodgePrimaryGameLayout.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

定义候选（多行签名仅展示首行）：

- L76: `UHodgeHUDLayout::UHodgeHUDLayout(const FObjectInitializer& ObjectInitializer)`
- L89: `void UHodgeHUDLayout::NativeOnInitialized()`
- L140: `void UHodgeHUDLayout::NativeDestruct()`
- L172: `void UHodgeHUDLayout::CloseOwnedMenu()`
- L181: `void UHodgeHUDLayout::NativeOnDeactivated()`
- L189: `void UHodgeHUDLayout::EnsureMenuLayerStack()`
- L198: `void UHodgeHUDLayout::ToggleHUDMenu()`
- L204: `void UHodgeHUDLayout::HandleEscapeAction()`
- L222: `void UHodgeHUDLayout::HandleInputDeviceConnectionChanged(EInputDeviceConnectionState NewConnectionState,`
- L246: `void UHodgeHUDLayout::HandleInputDevicePairingChanged(FInputDeviceId InputDeviceId, FPlatformUserId NewUserPlatformId,`
- L270: `bool UHodgeHUDLayout::ShouldPlatformDisplayControllerDisconnectScreen() const`
- L303: `void UHodgeHUDLayout::NotifyControllerStateChangeForDisconnectScreen()`
- L344: `void UHodgeHUDLayout::ProcessControllerDevicesHavingChangedForDisconnectScreen()`
- L417: `void UHodgeHUDLayout::DisplayControllerDisconnectedMenu_Implementation()`
- L456: `void UHodgeHUDLayout::HideControllerDisconnectedMenu_Implementation()`

## HodgeJoystickWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeJoystickWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeJoystickWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeJoystickWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeJoystickWidget.h)

定义候选（多行签名仅展示首行）：

- L16: `UHodgeJoystickWidget::UHodgeJoystickWidget(const FObjectInitializer& ObjectInitializer)`
- L23: `FReply UHodgeJoystickWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L38: `FReply UHodgeJoystickWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L52: `FReply UHodgeJoystickWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L59: `void UHodgeJoystickWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)`
- L66: `void UHodgeJoystickWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)`
- L86: `void UHodgeJoystickWidget::HandleTouchDelta(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L112: `void UHodgeJoystickWidget::StopInputSimulation()`

## HodgeResonanceEnergyWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeResonanceEnergyWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeResonanceEnergyWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeResonanceEnergyWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeResonanceEnergyWidget.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)

定义候选（多行签名仅展示首行）：

- L6: `UHodgeResonanceEnergyWidget::UHodgeResonanceEnergyWidget()`

## HodgeSettingScreen.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeSettingScreen.cpp](../../../Source/Hodgepodge/Private/UI/HodgeSettingScreen.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeSimulatedInputWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeSimulatedInputWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeSimulatedInputWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeSimulatedInputWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h)

定义候选（多行签名仅展示首行）：

- L15: `UHodgeSimulatedInputWidget::UHodgeSimulatedInputWidget(const FObjectInitializer& ObjectInitializer)`
- L23: `const FText UHodgeSimulatedInputWidget::GetPaletteCategory()`
- L30: `void UHodgeSimulatedInputWidget::NativeConstruct()`
- L45: `void UHodgeSimulatedInputWidget::NativeDestruct()`
- L57: `FReply UHodgeSimulatedInputWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L65: `UEnhancedInputLocalPlayerSubsystem* UHodgeSimulatedInputWidget::GetEnhancedInputSubsystem() const`
- L78: `UEnhancedPlayerInput* UHodgeSimulatedInputWidget::GetPlayerInput() const`
- L88: `void UHodgeSimulatedInputWidget::InputKeyValue(const FVector& Value)`
- L124: `void UHodgeSimulatedInputWidget::InputKeyValue2D(const FVector2D& Value)`
- L130: `void UHodgeSimulatedInputWidget::FlushSimulatedInput()`
- L139: `void UHodgeSimulatedInputWidget::QueryKeyToSimulate()`
- L157: `void UHodgeSimulatedInputWidget::OnControlMappingsRebuilt()`

## HodgeTaggedWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeTaggedWidget.cpp](../../../Source/Hodgepodge/Private/UI/HodgeTaggedWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeTaggedWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeTaggedWidget.h)

定义候选（多行签名仅展示首行）：

- L16: `UHodgeTaggedWidget::UHodgeTaggedWidget(const FObjectInitializer& ObjectInitializer)`
- L22: `void UHodgeTaggedWidget::NativeConstruct()`
- L47: `void UHodgeTaggedWidget::NativeDestruct()`
- L64: `void UHodgeTaggedWidget::SetVisibility(ESlateVisibility InVisibility)`
- L123: `void UHodgeTaggedWidget::OnWatchedTagsChanged()`

## HodgeTouchRegion.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/HodgeTouchRegion.cpp](../../../Source/Hodgepodge/Private/UI/HodgeTouchRegion.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeTouchRegion.h](../../../Source/Hodgepodge/Public/UI/HodgeTouchRegion.h)

定义候选（多行签名仅展示首行）：

- L15: `FReply UHodgeTouchRegion::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L22: `FReply UHodgeTouchRegion::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L32: `FReply UHodgeTouchRegion::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)`
- L39: `void UHodgeTouchRegion::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)`

## HodgeIndicatorManagerComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/IndicatorSystem/HodgeIndicatorManagerComponent.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/HodgeIndicatorManagerComponent.cpp)

项目内直接 include（不是运行调用关系）：[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)、[UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)

定义候选（多行签名仅展示首行）：

- L17: `UHodgeIndicatorManagerComponent::UHodgeIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)`
- L31: `UHodgeIndicatorManagerComponent* UHodgeIndicatorManagerComponent::GetComponent(AController* Controller)`
- L46: `void UHodgeIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)`
- L61: `void UHodgeIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)`

## IndicatorDescriptor.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorDescriptor.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorDescriptor.cpp)

项目内直接 include（不是运行调用关系）：[UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)、[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L24: `bool FIndicatorProjection::Project(const UIndicatorDescriptor& IndicatorDescriptor,`
- L257: `void UIndicatorDescriptor::SetIndicatorManagerComponent(UHodgeIndicatorManagerComponent* InManager)`
- L270: `void UIndicatorDescriptor::UnregisterIndicator()`

## IndicatorLayer.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLayer.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLayer.cpp)

项目内直接 include（不是运行调用关系）：[UI/IndicatorSystem/IndicatorLayer.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.h)、[UI/IndicatorSystem/SActorCanvas.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h)

定义候选（多行签名仅展示首行）：

- L27: `UIndicatorLayer::UIndicatorLayer(const FObjectInitializer& ObjectInitializer)`
- L40: `void UIndicatorLayer::ReleaseSlateResources(bool bReleaseChildren)`
- L51: `TSharedRef<SWidget> UIndicatorLayer::RebuildWidget()`

## IndicatorLibrary.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLibrary.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/IndicatorLibrary.cpp)

项目内直接 include（不是运行调用关系）：[UI/IndicatorSystem/IndicatorLibrary.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLibrary.h)、[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L19: `UIndicatorLibrary::UIndicatorLibrary()`
- L24: `UHodgeIndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)`

## SActorCanvas.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/IndicatorSystem/SActorCanvas.cpp](../../../Source/Hodgepodge/Private/UI/IndicatorSystem/SActorCanvas.cpp)

项目内直接 include（不是运行调用关系）：[UI/IndicatorSystem/SActorCanvas.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h)、[UI/IndicatorSystem/IActorIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IActorIndicatorWidget.h)、[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)、[UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)

定义候选（多行签名仅展示首行）：

- L211: `void SActorCanvas::Construct(const FArguments& InArgs, const FLocalPlayerContext& InLocalPlayerContext,`
- L257: `EActiveTimerReturnType SActorCanvas::UpdateCanvas(double InCurrentTime, float InDeltaTime)`
- L488: `void SActorCanvas::SetShowAnyIndicators(bool bIndicators)`
- L510: `void SActorCanvas::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const`
- L800: `int32 SActorCanvas::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,`
- L862: `SActorCanvas::~SActorCanvas()`
- L880: `FString SActorCanvas::GetReferencerName() const`
- L886: `void SActorCanvas::AddReferencedObjects(FReferenceCollector& Collector)`
- L894: `void SActorCanvas::OnIndicatorAdded(UIndicatorDescriptor* Indicator)`
- L910: `void SActorCanvas::OnIndicatorRemoved(UIndicatorDescriptor* Indicator)`
- L924: `void SActorCanvas::AddIndicatorForEntry(UIndicatorDescriptor* Indicator)`
- L961: `void SActorCanvas::OnIndicatorClassLoaded(TWeakObjectPtr<UIndicatorDescriptor> IndicatorPtr)`
- L1020: `void SActorCanvas::RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator)`
- L1054: `SActorCanvas::FScopedWidgetSlotArguments SActorCanvas::AddActorSlot(UIndicatorDescriptor* Indicator)`
- L1075: `int32 SActorCanvas::RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget)`
- L1100: `void SActorCanvas::GetOffsetAndSize(const UIndicatorDescriptor* Indicator,`
- L1178: `void SActorCanvas::UpdateActiveTimer()`

## HodgePerfStatContainerBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatContainerBase.cpp](../../../Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatContainerBase.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgePerfStatWidgetBase.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatWidgetBase.cpp](../../../Source/Hodgepodge/Private/UI/PerformanceStats/HodgePerfStatWidgetBase.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeUIManagerSubsystem.cpp

GameInstance 所属的本地玩家 UI 管理器：根布局、Controller 绑定、输入挂起和精确撤销；专服不创建。

源码：[Source/Hodgepodge/Private/UI/Subsystem/HodgeUIManagerSubsystem.cpp](../../../Source/Hodgepodge/Private/UI/Subsystem/HodgeUIManagerSubsystem.cpp)

项目内直接 include（不是运行调用关系）：[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)、[UI/Foundation/HodgePrimaryGameLayout.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h)、[Core/LocalPlayer/HodgeLocalPlayerBase.h](../../../Source/Hodgepodge/Public/Core/LocalPlayer/HodgeLocalPlayerBase.h)、[Core/GameInstance/HodgeGameInstanceBase.h](../../../Source/Hodgepodge/Public/Core/GameInstance/HodgeGameInstanceBase.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)

定义候选（多行签名仅展示首行）：

- L20: `bool UHodgeUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const`
- L27: `void UHodgeUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)`
- L34: `void UHodgeUIManagerSubsystem::PlayerAdded(ULocalPlayer* Player)`
- L52: `void UHodgeUIManagerSubsystem::ControllerChanged(ULocalPlayer* Player, APlayerController* Controller)`
- L74: `void UHodgeUIManagerSubsystem::CreateRoot(ULocalPlayer* Player, TWeakObjectPtr<APlayerController> Controller)`
- L89: `void UHodgeUIManagerSubsystem::DestroyRoot(ULocalPlayer* Player)`
- L112: `void UHodgeUIManagerSubsystem::PlayerRemoved(ULocalPlayer* Player)`
- L123: `void UHodgeUIManagerSubsystem::Deinitialize()`
- L133: `UHodgePrimaryGameLayout* UHodgeUIManagerSubsystem::GetRootLayout(ULocalPlayer* Player) const`
- L140: `bool UHodgeUIManagerSubsystem::IsGameInputAllowed(ULocalPlayer* Player) const`
- L149: `bool UHodgeUIManagerSubsystem::AllowsGameplayInput(const APlayerController* Controller)`
- L158: `void UHodgeUIManagerSubsystem::ClearGameplayInput(ULocalPlayer* Player)`
- L170: `void UHodgeUIManagerSubsystem::RefreshInputState(ULocalPlayer* Player)`
- L176: `void UHodgeUIManagerSubsystem::SuspendInput(ULocalPlayer* Player, FName Token)`
- L190: `void UHodgeUIManagerSubsystem::ResumeInput(ULocalPlayer* Player, FName Token)`
- L202: `UHodgeUIManagerSubsystem* UHodgeUIManagerSubsystem::GetUIManager(const UObject* WorldContextObject)`
- L208: `UHodgePrimaryGameLayout* UHodgeUIManagerSubsystem::GetRootLayoutForController(APlayerController* Controller)`
- L216: `UHodgeGameplayUIDataSource* UHodgeUIManagerSubsystem::GetGameplayDataForController(APlayerController* Controller)`

## HodgeUIMessaging.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Subsystem/HodgeUIMessaging.cpp](../../../Source/Hodgepodge/Private/UI/Subsystem/HodgeUIMessaging.cpp)

**全部为注释或空白；无有效声明/实现。**

## HodgeLockOnIndicatorWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Targeting/HodgeLockOnIndicatorWidget.cpp](../../../Source/Hodgepodge/Private/UI/Targeting/HodgeLockOnIndicatorWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/Targeting/HodgeLockOnIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/Targeting/HodgeLockOnIndicatorWidget.h)

定义候选（多行签名仅展示首行）：

- L7: `TSharedRef<SWidget> UHodgeLockOnIndicatorWidget::RebuildWidget()`

## CircumferenceMarkerWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Weapons/CircumferenceMarkerWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/CircumferenceMarkerWidget.cpp)

项目内直接 include（不是运行调用关系）：[UI/Weapons/CircumferenceMarkerWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/CircumferenceMarkerWidget.h)

定义候选（多行签名仅展示首行）：

- L14: `UCircumferenceMarkerWidget::UCircumferenceMarkerWidget(const FObjectInitializer& ObjectInitializer)`
- L22: `void UCircumferenceMarkerWidget::ReleaseSlateResources(bool bReleaseChildren)`
- L30: `TSharedRef<SWidget> UCircumferenceMarkerWidget::RebuildWidget()`
- L41: `void UCircumferenceMarkerWidget::SynchronizeProperties()`
- L50: `void UCircumferenceMarkerWidget::SetRadius(float InRadius)`

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

- L17: `SCircumferenceMarkerWidget::SCircumferenceMarkerWidget()`
- L22: `void SCircumferenceMarkerWidget::Construct(const FArguments& InArgs)`
- L32: `FSlateRenderTransform SCircumferenceMarkerWidget::GetMarkerRenderTransform(`
- L62: `int32 SCircumferenceMarkerWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,`
- L100: `FVector2D SCircumferenceMarkerWidget::ComputeDesiredSize(float) const`
- L109: `void SCircumferenceMarkerWidget::SetRadius(float NewRadius)`
- L119: `void SCircumferenceMarkerWidget::SetMarkerList(TArray<FCircumferenceMarkerEntry>& NewMarkerList)`

## SHitMarkerConfirmationWidget.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/UI/Weapons/SHitMarkerConfirmationWidget.cpp](../../../Source/Hodgepodge/Private/UI/Weapons/SHitMarkerConfirmationWidget.cpp)

**全部为注释或空白；无有效声明/实现。**

## MaterialProgressBar.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Basic/MaterialProgressBar.h](../../../Source/Hodgepodge/Public/UI/Basic/MaterialProgressBar.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CommonUserWidget.h"
  10: #include "MaterialProgressBar.generated.h"
  12: class UImage;
  13: class UMaterialInstanceDynamic;
  14: class UMaterialInterface;
  15: class UWidgetAnimation;
  18: UCLASS(Abstract, meta = (DisableNativeTick))
  19: class UMaterialProgressBar : public UCommonUserWidget
  20: {
  21: 	GENERATED_BODY()
  23: protected:
  25: 	virtual void SynchronizeProperties() override;
  27: #if WITH_EDITOR
  29: 	virtual void OnWidgetRebuilt() override;
  30: #endif
  35: 	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
  37: public:
  41: 	UFUNCTION(BlueprintCallable)
  42: 	void SetProgress(float Progress);
  47: 	UFUNCTION(BlueprintCallable)
  48: 	void SetStartProgress(float StartProgress);
  53: 	UFUNCTION(BlueprintCallable)
  54: 	void SetColorA(FLinearColor ColorA);
  59: 	UFUNCTION(BlueprintCallable)
  60: 	void SetColorB(FLinearColor ColorB);
  65: 	UFUNCTION(BlueprintCallable)
  66: 	void SetColorBackground(FLinearColor ColorBackground);
  73: 	UFUNCTION(BlueprintCallable)
  74: 	void AnimateProgressFromStart(float Start, float End, float AnimSpeed = 1.0f);
  80: 	UFUNCTION(BlueprintCallable)
  81: 	void AnimateProgressFromCurrent(float End, float AnimSpeed = 1.0f);
  83: 	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFillAnimationFinished);
  86: 	UPROPERTY(BlueprintAssignable)
  87: 	FOnFillAnimationFinished OnFillAnimationFinished;
  89: private:
  93: 	void SetProgress_Internal(float Progress);
  97: 	void SetStartProgress_Internal(float StartProgress);
 101: 	void SetColorA_Internal(FLinearColor ColorA);
 105: 	void SetColorB_Internal(FLinearColor ColorB);
 109: 	void SetColorBackground_Internal(FLinearColor ColorBackground);
 114: 	UMaterialInstanceDynamic* GetBarDynamicMaterial() const;
 117: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "CachedColorA"))
 118: 	bool bOverrideDefaultColorA = false;
 121: 	UPROPERTY(EditAnywhere, meta = (DisplayName = "Color A", EditCondition = "bOverrideDefaultColorA"))
 122: 	FLinearColor CachedColorA;
 125: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "CachedColorB"))
 126: 	bool bOverrideDefaultColorB = false;
 129: 	UPROPERTY(EditAnywhere, meta = (DisplayName = "Color B", EditCondition = "bOverrideDefaultColorB"))
 130: 	FLinearColor CachedColorB;
 133: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "CachedColorBackground"))
 134: 	bool bOverrideDefaultColorBackground;
 136: 	UPROPERTY(EditAnywhere,
 137: 		meta = (DisplayName = "Color Background", EditCondition = "bOverrideDefaultColorBackground"))
 139: 	FLinearColor CachedColorBackground;
 142: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "Segments"))
 143: 	bool bOverrideDefaultSegments = false;
 146: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultSegments"))
 147: 	int32 Segments;
 150: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "SegmentEdge"))
 151: 	bool bOverrideDefaultSegmentEdge = false;
 154: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultSegmentEdge"))
 155: 	float SegmentEdge;
 158: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "FillEdgeSoftness"))
 159: 	bool bOverrideDefaultFillEdgeSoftness;
 162: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultFillEdgeSoftness"))
 163: 	float FillEdgeSoftness;
 166: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "GlowEdge"))
 167: 	bool bOverrideDefaultGlowEdge = false;
 170: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultGlowEdge"))
 171: 	float GlowEdge;
 174: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "GlowSoftness"))
 175: 	bool bOverrideDefaultGlowSoftness = false;
 178: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultGlowSoftness"))
 179: 	float GlowSoftness;
 182: 	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle = "OutlineScale"))
 183: 	bool bOverrideDefaultOutlineScale = false;
 186: 	UPROPERTY(EditAnywhere, meta = (EditCondition = "bOverrideDefaultOutlineScale"))
 187: 	float OutlineScale;
 190: 	UPROPERTY(EditAnywhere)
 191: 	bool bUseStroke = true;
 194: 	UPROPERTY(EditDefaultsOnly)
 195: 	TObjectPtr<UMaterialInterface> StrokeMaterial;
 198: 	UPROPERTY(EditDefaultsOnly)
 199: 	TObjectPtr<UMaterialInterface> NoStrokeMaterial;
 201: #if WITH_EDITORONLY_DATA
 202: 	UPROPERTY(EditAnywhere, meta = (DisplayName = "Design Time Progress"))
 203: 	float DesignTime_Progress = 1.0f;
 204: #endif
 207: 	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess))
 208: 	TObjectPtr<UImage> Image_Bar;
 211: 	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
 212: 	TObjectPtr<UWidgetAnimation> BoundAnim_FillBar;
 215: 	UPROPERTY(Transient)
 216: 	mutable TObjectPtr<UMaterialInstanceDynamic> CachedMID;
 219: 	float CachedProgress = -1.0f;
 221: 	float CachedStartProgress = -1.0f;
 222: };
```

## HodgeBoundActionButton.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeBoundActionButton.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeBoundActionButton.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "Input/CommonBoundActionButton.h"
  10: #include "HodgeBoundActionButton.generated.h"
  12: class UCommonButtonStyle;
  13: class UObject;
  19: UCLASS(Abstract, meta = (DisableNativeTick))
  20: class HODGEPODGE_API UHodgeBoundActionButton : public UCommonBoundActionButton
  21: {
  22: 	GENERATED_BODY()
  24: protected:
  26: 	virtual void NativeConstruct() override;
  28: private:
  32: 	void HandleInputMethodChanged(ECommonInputType NewInputMethod);
  35: 	UPROPERTY(EditAnywhere, Category = "Styles")
  36: 	TSubclassOf<UCommonButtonStyle> KeyboardStyle;
  39: 	UPROPERTY(EditAnywhere, Category = "Styles")
  40: 	TSubclassOf<UCommonButtonStyle> GamepadStyle;
  43: 	UPROPERTY(EditAnywhere, Category = "Styles")
  44: 	TSubclassOf<UCommonButtonStyle> TouchStyle;
  45: };
```

## HodgeListView.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeListView.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeListView.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CommonListView.h"
  10: #include "HodgeListView.generated.h"
  12: class UUserWidget;
  13: class ULocalPlayer;
  14: class UHodgeWidgetFactory;
  17: UCLASS(meta = (DisableNativeTick))
  18: class HODGEPODGE_API UHodgeListView : public UCommonListView
  19: {
  20: 	GENERATED_BODY()
  22: public:
  26: 	UHodgeListView(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  28: #if WITH_EDITOR
  32: 	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const override;
  33: #endif
  35: protected:
  42: 	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass,
  43: 	                                                   const TSharedRef<STableViewBase>& OwnerTable) override;
  46: protected:
  48: 	UPROPERTY(EditAnywhere, Instanced, Category="Entry Creation")
  49: 	TArray<TObjectPtr<UHodgeWidgetFactory>> FactoryRules;
  50: };
```

## HodgeTabButtonBase.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeTabButtonBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabButtonBase.h)

项目内直接 include（不是运行调用关系）：[UI/Foundation/HodgeButtonBase.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "HodgeTabListWidgetBase.h"
   9: #include "UI/Foundation/HodgeButtonBase.h"
  11: #include "HodgeTabButtonBase.generated.h"
  13: class UCommonLazyImage;
  14: class UObject;
  15: struct FFrame;
  16: struct FSlateBrush;
  19: UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
  20: class HODGEPODGE_API UHodgeTabButtonBase : public UHodgeButtonBase, public IHodgeTabButtonInterface
  21: {
  22: 	GENERATED_BODY()
  24: public:
  28: 	void SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject);
  32: 	void SetIconBrush(const FSlateBrush& Brush);
  34: protected:
  38: 	UFUNCTION()
  39: 	virtual void SetTabLabelInfo_Implementation(const FHodgeTabDescriptor& TabLabelInfo) override;
  41: private:
  43: 	UPROPERTY(meta = (BindWidgetOptional))
  44: 	TObjectPtr<UCommonLazyImage> LazyImage_Icon;
  45: };
```

## HodgeTabListWidgetBase.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeTabListWidgetBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CommonTabListWidgetBase.h"
  10: #include "HodgeTabListWidgetBase.generated.h"
  12: class UCommonButtonBase;
  13: class UCommonUserWidget;
  14: class UObject;
  15: class UWidget;
  16: struct FFrame;
  19: USTRUCT(BlueprintType)
  20: struct FHodgeTabDescriptor
  21: {
  22: 	GENERATED_BODY()
  24: public:
  26: 	FHodgeTabDescriptor()
  27: 		: bHidden(false)
  28: 		  , CreatedTabContentWidget(nullptr)
  29: 	{
  30: 	}
  33: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  34: 	FName TabId;
  37: 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
  38: 	FText TabText;
  41: 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
  42: 	FSlateBrush IconBrush;
  45: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  46: 	bool bHidden;
  49: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  50: 	TSubclassOf<UCommonButtonBase> TabButtonType;
  54: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  55: 	TSubclassOf<UCommonUserWidget> TabContentType;
  58: 	UPROPERTY(Transient)
  59: 	TObjectPtr<UWidget> CreatedTabContentWidget;
  60: };
  63: UINTERFACE(BlueprintType)
  64: class UHodgeTabButtonInterface : public UInterface
  65: {
  66: 	GENERATED_BODY()
  67: };
  70: class IHodgeTabButtonInterface
  71: {
  72: 	GENERATED_BODY()
  74: public:
  78: 	UFUNCTION(BlueprintNativeEvent, Category = "Tab Button")
  79: 	void SetTabLabelInfo(const FHodgeTabDescriptor& TabDescriptor);
  80: };
  83: UCLASS(Blueprintable, BlueprintType, Abstract, meta = (DisableNativeTick))
  84: class HODGEPODGE_API UHodgeTabListWidgetBase : public UCommonTabListWidgetBase
  85: {
  86: 	GENERATED_BODY()
  88: public:
  94: 	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
  95: 	bool GetPreregisteredTabInfo(const FName TabNameId, FHodgeTabDescriptor& OutTabInfo);
 101: 	const TArray<FHodgeTabDescriptor>& GetAllPreregisteredTabInfos() { return PreregisteredTabInfoArray; }
 108: 	UFUNCTION(BlueprintCallable, Category = "Tab List")
 109: 	void SetTabHiddenState(FName TabNameId, bool bHidden);
 115: 	UFUNCTION(BlueprintCallable, Category = "Tab List")
 116: 	bool RegisterDynamicTab(const FHodgeTabDescriptor& TabDescriptor);
 121: 	UFUNCTION(BlueprintCallable, Category = "Tab List")
 122: 	bool IsFirstTabActive() const;
 127: 	UFUNCTION(BlueprintCallable, Category = "Tab List")
 128: 	bool IsLastTabActive() const;
 134: 	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
 135: 	bool IsTabVisible(FName TabId);
 140: 	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
 141: 	int32 GetVisibleTabCount();
 144: 	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTabContentCreated, FName, TabId, UCommonUserWidget*, TabWidget);
 146: 	DECLARE_EVENT_TwoParams(UHodgeTabListWidgetBase, FOnTabContentCreatedNative, FName            ,
 147: 	                        UCommonUserWidget*                );
 151: 	UPROPERTY(BlueprintAssignable, Category = "Tab List")
 152: 	FOnTabContentCreated OnTabContentCreated;
 154: 	FOnTabContentCreatedNative OnTabContentCreatedNative;
 156: protected:
 159: 	virtual void NativeOnInitialized() override;
 161: 	virtual void NativeConstruct() override;
 163: 	virtual void NativeDestruct() override;
 167: 	virtual void HandlePreLinkedSwitcherChanged() override;
 169: 	virtual void HandlePostLinkedSwitcherChanged() override;
 175: 	virtual void HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton) override;
 177: private:
 179: 	void SetupTabs();
 182: 	UPROPERTY(EditAnywhere, meta=(TitleProperty="TabId"))
 183: 	TArray<FHodgeTabDescriptor> PreregisteredTabInfoArray;
 190: 	UPROPERTY()
 191: 	TMap<FName, FHodgeTabDescriptor> PendingTabLabelInfoMap;
 192: };
```

## HodgeWidgetFactory.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "UObject/Object.h"
  10: #include "HodgeWidgetFactory.generated.h"
  12: template <class TClass>
  13: class TSubclassOf;
  15: class UUserWidget;
  16: struct FFrame;
  19: UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
  20: class HODGEPODGE_API UHodgeWidgetFactory : public UObject
  21: {
  22: 	GENERATED_BODY()
  24: public:
  26: 	UHodgeWidgetFactory()
  27: 	{
  28: 	}
  34: 	UFUNCTION(BlueprintNativeEvent)
  35: 	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
  36: };
```

## HodgeWidgetFactory_Class.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory_Class.h](../../../Source/Hodgepodge/Public/UI/Common/HodgeWidgetFactory_Class.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "HodgeWidgetFactory.h"
   9: #include "Templates/SubclassOf.h"
  10: #include "UObject/SoftObjectPtr.h"
  12: #include "HodgeWidgetFactory_Class.generated.h"
  14: class UObject;
  15: class UUserWidget;
  18: UCLASS()
  19: class HODGEPODGE_API UHodgeWidgetFactory_Class : public UHodgeWidgetFactory
  20: {
  21: 	GENERATED_BODY()
  23: public:
  25: 	UHodgeWidgetFactory_Class()
  26: 	{
  27: 	}
  33: 	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;
  35: protected:
  37: 	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
  38: 	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
  39: };
```

## HodgeGameplayUIDataSource.h

每个本地玩家共享的生命订阅、属性就绪与技能状态/输入接入；布局和按钮行为保存在 WBP。

源码：[Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "UObject/Object.h"
   6: #include "GameplayTagContainer.h"
   7: #include "HodgeGameplayUIDataSource.generated.h"
   9: class UHodgeLocalPlayerBase;
  10: class UHodgeHealthComponent;
  11: class UHodgePawnExtensionComponent;
  12: class AHodgePlayerState;
  13: class APlayerController;
  14: class APlayerState;
  15: class APawn;
  16: class AActor;
  17: class UHodgeAbilitySystemComponent;
  18: struct FOnAttributeChangeData;
  21: USTRUCT(BlueprintType)
  22: struct HODGEPODGE_API FHodgeHavocResourceSnapshot
  23: {
  24: 	GENERATED_BODY()
  26: 	UPROPERTY(BlueprintReadOnly) float CurrentDarkFlow=0.f;
  27: 	UPROPERTY(BlueprintReadOnly) float MaxDarkFlow=0.f;
  28: 	UPROPERTY(BlueprintReadOnly) float Percent=0.f;
  30: 	UPROPERTY(BlueprintReadOnly) bool bReady=false;
  31: 	UPROPERTY(BlueprintReadOnly) bool bDarkSurge=false;
  32: };
  33: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeUIHavocChanged,const FHodgeHavocResourceSnapshot&,Snapshot);
  36: USTRUCT(BlueprintType)
  37: struct FHodgeUIResonanceSnapshot
  38: {
  39: 	GENERATED_BODY()
  41: 	UPROPERTY(BlueprintReadOnly) float Energy = 0.f;
  43: 	UPROPERTY(BlueprintReadOnly) float Capacity = 0.f;
  45: 	UPROPERTY(BlueprintReadOnly) float Percent = 0.f;
  47: 	UPROPERTY(BlueprintReadOnly) bool bReady = false;
  48: };
  49: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeUIResonanceChanged, const FHodgeUIResonanceSnapshot&, Snapshot);
  52: USTRUCT(BlueprintType)
  53: struct FHodgeUIVitalsSnapshot
  54: {
  55: 	GENERATED_BODY()
  57: 	UPROPERTY(BlueprintReadOnly) float Health = 0;
  59: 	UPROPERTY(BlueprintReadOnly) float MaxHealth = 0;
  61: 	UPROPERTY(BlueprintReadOnly) float Percent = 0;
  63: 	UPROPERTY(BlueprintReadOnly) int32 CharacterLevel = 0;
  65: 	UPROPERTY(BlueprintReadOnly) bool bReady = false;
  67: 	UPROPERTY(BlueprintReadOnly) bool bDead = false;
  68: };
  71: USTRUCT(BlueprintType)
  72: struct FHodgeUIAbilityDisplayState
  73: {
  74: 	GENERATED_BODY()
  76: 	UPROPERTY(BlueprintReadOnly) bool bGranted = false;
  78: 	UPROPERTY(BlueprintReadOnly) bool bInputAllowed = false;
  80: 	UPROPERTY(BlueprintReadOnly) float CooldownRemaining = 0;
  82: 	UPROPERTY(BlueprintReadOnly) float CooldownDuration = 0;
  83: };
  86: USTRUCT(BlueprintType)
  87: struct FHodgeHUDAbilitySlot
  88: {
  89:     GENERATED_BODY()
  91:     UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Label;
  93:     UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="InputTag")) FGameplayTag InputTag;
  94: };
  97: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeUIVitalsChanged, const FHodgeUIVitalsSnapshot&, Snapshot);
 100: UCLASS(BlueprintType)
 101: class HODGEPODGE_API UHodgeGameplayUIDataSource : public UObject
 102: {
 103: 	GENERATED_BODY()
 104: public:
 109: 	void Initialize(UHodgeLocalPlayerBase* Player, APlayerController* InController);
 111: 	void Shutdown();
 113: 	UPROPERTY(BlueprintAssignable, Category="Hodge|UI") FHodgeUIVitalsChanged OnVitalsChanged;
 117: 	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeUIVitalsSnapshot GetVitals() const { return Vitals; }
 119: 	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeUIResonanceSnapshot GetResonance() const { return Resonance; }
 121: 	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeHavocResourceSnapshot GetHavocSnapshot() const { return Havoc; }
 122: 	UPROPERTY(BlueprintAssignable, Category="Hodge|UI") FHodgeUIHavocChanged OnHavocChanged;
 124: 	UPROPERTY(BlueprintAssignable, Category="Hodge|UI") FHodgeUIResonanceChanged OnResonanceChanged;
 129: 	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeUIAbilityDisplayState GetAbilityDisplayState(FGameplayTag InputTag) const;
 134: 	UFUNCTION(BlueprintCallable, Category="Hodge|UI") bool SubmitInput(FGameplayTag InputTag);
 135: private:
 137: 	UPROPERTY(Transient) FHodgeHavocResourceSnapshot Havoc;
 138: 	FDelegateHandle DarkFlowDelegate;
 139: 	FDelegateHandle MaxDarkFlowDelegate;
 140: 	FDelegateHandle DarkSurgeDelegate;
 142: 	void HavocModeChanged(FGameplayTag Tag,int32 Count);
 144: 	UPROPERTY(Transient) FHodgeUIVitalsSnapshot Vitals;
 146: 	UPROPERTY(Transient) FHodgeUIResonanceSnapshot Resonance;
 147: 	TWeakObjectPtr<UHodgeAbilitySystemComponent> ResonanceASC;
 148: 	FDelegateHandle EnergyDelegate;
 149: 	FDelegateHandle CapacityDelegate;
 151: 	void ResonanceAttributeChanged(const FOnAttributeChangeData& Data);
 153: 	TWeakObjectPtr<UHodgeLocalPlayerBase> LocalPlayer;
 155: 	TWeakObjectPtr<APlayerController> Controller;
 157: 	TWeakObjectPtr<APawn> Avatar;
 159: 	TWeakObjectPtr<UHodgeHealthComponent> Health;
 161: 	TWeakObjectPtr<UHodgePawnExtensionComponent> Extension;
 163: 	TWeakObjectPtr<AHodgePlayerState> PlayerState;
 165: 	FDelegateHandle PawnDelegate;
 167: 	FDelegateHandle PlayerStateDelegate;
 169: 	FDelegateHandle ReadyDelegate;
 171: 	bool bBound = false;
 176: 	void PawnChanged(UHodgeLocalPlayerBase* Player, APawn* Pawn);
 181: 	void StateChanged(UHodgeLocalPlayerBase* Player, APlayerState* State);
 183: 	void UnbindPawn();
 185: 	void UnbindState();
 187: 	void Refresh();
 191: 	bool CanUseGameplay() const;
 198: 	UFUNCTION() void AttributeChanged(UHodgeHealthComponent* Component, float Old, float New, AActor* Instigator);
 202: 	UFUNCTION() void DeathChanged(AActor* Actor);
 203: };
```

## UIExtensionPointWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "Components/DynamicEntryBoxBase.h"
  14: #include "UIExtensionSystem.h"
  16: #include "UIExtensionPointWidget.generated.h"
  20: class IWidgetCompilerLog;
  24: class UHodgeLocalPlayerBase;
  28: class APlayerState;
  39: UCLASS()
  40: class HODGEPODGE_API UUIExtensionPointWidget : public UDynamicEntryBoxBase
  41: {
  42: 	GENERATED_BODY()
  44: public:
  49: 	void SetExtensionPointTag(FGameplayTag Tag);
  55: 	DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(TSubclassOf<UUserWidget>, FOnGetWidgetClassForData, UObject*, DataItem);
  59: 	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnConfigureWidgetForData, UUserWidget*, Widget, UObject*, DataItem);
  65: 	UUIExtensionPointWidget(const FObjectInitializer& ObjectInitializer);
  74: 	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
  81: 	virtual TSharedRef<SWidget> RebuildWidget() override;
  83: #if WITH_EDITOR
  88: 	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
  89: #endif
  93: private:
  99: 	void ResetExtensionPoint();
 104: 	void RegisterExtensionPoint();
 114: 	void RegisterExtensionPointForPlayerState(UHodgeLocalPlayerBase* LocalPlayer, APlayerState* PlayerState);
 129: 	void OnAddOrRemoveExtension(EUIExtensionAction Action, const FUIExtensionRequest& Request);
 131: protected:
 143: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
 144: 	FGameplayTag ExtensionPointTag;
 155: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
 156: 	EUIExtensionPointMatch ExtensionPointTagMatch = EUIExtensionPointMatch::ExactMatch;
 162: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
 163: 	TArray<TObjectPtr<UClass>> DataClasses;
 172: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Extension", meta=( IsBindableEvent="True" ))
 173: 	FOnGetWidgetClassForData GetWidgetClassForData;
 179: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Extension", meta=( IsBindableEvent="True" ))
 180: 	FOnConfigureWidgetForData ConfigureWidgetForData;
 188: 	TArray<FUIExtensionPointHandle> ExtensionPointHandles;
 200: 	UPROPERTY(Transient)
 201: 	TMap<FUIExtensionHandle, TObjectPtr<UUserWidget>> ExtensionMapping;
 203: 	TWeakObjectPtr<UHodgeLocalPlayerBase> BoundLocalPlayer;
 205: 	FDelegateHandle PlayerStateDelegate;
 207: 	FUIExtensionPointHandle PlayerStatePoint;
 208: };
```

## UIExtensionSystem.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Extension/UIExtensionSystem.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionSystem.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "GameplayTagContainer.h"
  14: #include "Kismet/BlueprintFunctionLibrary.h"
  18: #include "Subsystems/WorldSubsystem.h"
  20: #include "UIExtensionSystem.generated.h"
  23: class UUIExtensionSubsystem;
  26: struct FUIExtensionRequest;
  28: template <typename T>
  29: class TSubclassOf;
  32: class FSubsystemCollectionBase;
  36: class UUserWidget;
  38: struct FFrame;
  41: DECLARE_LOG_CATEGORY_EXTERN(LogUIExtension, Log, All);
  45: UENUM(BlueprintType)
  46: enum class EUIExtensionPointMatch : uint8
  47: {
  54: 	ExactMatch,
  62: 	PartialMatch
  63: };
  67: UENUM(BlueprintType)
  68: enum class EUIExtensionAction : uint8
  69: {
  71: 	Added,
  74: 	Removed
  75: };
  80: DECLARE_DELEGATE_TwoParams(FExtendExtensionPointDelegate, EUIExtensionAction Action,
  81:                            const FUIExtensionRequest& Request);
  92: struct FUIExtension : TSharedFromThis<FUIExtension>
  93: {
  94: public:
  99: 	FGameplayTag ExtensionPointTag;
 103: 	int32 Priority = INDEX_NONE;
 108: 	TWeakObjectPtr<UObject> ContextObject;
 118: 	TObjectPtr<UObject> Data = nullptr;
 119: };
 128: struct FUIExtensionPoint : TSharedFromThis<FUIExtensionPoint>
 129: {
 130: public:
 132: 	FGameplayTag ExtensionPointTag;
 136: 	TWeakObjectPtr<UObject> ContextObject;
 139: 	EUIExtensionPointMatch ExtensionPointTagMatchType = EUIExtensionPointMatch::ExactMatch;
 143: 	TArray<TObjectPtr<UClass>> AllowedDataClasses;
 146: 	FExtendExtensionPointDelegate Callback;
 163: 	bool DoesExtensionPassContract(const FUIExtension* Extension) const;
 164: };
 174: USTRUCT(BlueprintType)
 175: struct HODGEPODGE_API FUIExtensionPointHandle
 176: {
 177: 	GENERATED_BODY()
 179: public:
 182: 	FUIExtensionPointHandle()
 183: 	{
 184: 	}
 188: 	void Unregister();
 194: 	bool IsValid() const { return DataPtr.IsValid(); }
 201: 	bool operator==(const FUIExtensionPointHandle& Other) const { return DataPtr == Other.DataPtr; }
 208: 	bool operator!=(const FUIExtensionPointHandle& Other) const { return !operator==(Other); }
 216: 	friend uint32 GetTypeHash(const FUIExtensionPointHandle& Handle)
 217: 	{
 218: 		return PointerHash(Handle.DataPtr.Get());
 219: 	}
 221: private:
 224: 	TWeakObjectPtr<UUIExtensionSubsystem> ExtensionSource;
 227: 	TSharedPtr<FUIExtensionPoint> DataPtr;
 230: 	friend UUIExtensionSubsystem;
 237: 	FUIExtensionPointHandle(UUIExtensionSubsystem* InExtensionSource,
 238: 	                        const TSharedPtr<FUIExtensionPoint>& InDataPtr) : ExtensionSource(InExtensionSource),
 239: 	                                                                          DataPtr(InDataPtr)
 240: 	{
 241: 	}
 242: };
 245: template <>
 247: struct TStructOpsTypeTraits<FUIExtensionPointHandle> : public TStructOpsTypeTraitsBase2<FUIExtensionPointHandle>
 248: {
 249: 	enum
 250: 	{
 253: 		WithCopy = true,
 256: 		WithIdenticalViaEquality = true,
 257: 	};
 258: };
 267: USTRUCT(BlueprintType)
 268: struct HODGEPODGE_API FUIExtensionHandle
 269: {
 270: 	GENERATED_BODY()
 272: public:
 275: 	FUIExtensionHandle()
 276: 	{
 277: 	}
 281: 	void Unregister();
 287: 	bool IsValid() const { return DataPtr.IsValid(); }
 294: 	bool operator==(const FUIExtensionHandle& Other) const { return DataPtr == Other.DataPtr; }
 301: 	bool operator!=(const FUIExtensionHandle& Other) const { return !operator==(Other); }
 308: 	friend FORCEINLINE uint32 GetTypeHash(FUIExtensionHandle Handle)
 309: 	{
 310: 		return PointerHash(Handle.DataPtr.Get());
 311: 	}
 313: private:
 315: 	TWeakObjectPtr<UUIExtensionSubsystem> ExtensionSource;
 318: 	TSharedPtr<FUIExtension> DataPtr;
 321: 	friend UUIExtensionSubsystem;
 328: 	FUIExtensionHandle(UUIExtensionSubsystem* InExtensionSource,
 329: 	                   const TSharedPtr<FUIExtension>& InDataPtr) : ExtensionSource(InExtensionSource),
 330: 	                                                                DataPtr(InDataPtr)
 331: 	{
 332: 	}
 333: };
 336: template <>
 338: struct TStructOpsTypeTraits<FUIExtensionHandle> : public TStructOpsTypeTraitsBase2<FUIExtensionHandle>
 339: {
 340: 	enum
 341: 	{
 344: 		WithCopy = true,
 347: 		WithIdenticalViaEquality = true,
 348: 	};
 349: };
 358: USTRUCT(BlueprintType)
 359: struct FUIExtensionRequest
 360: {
 361: 	GENERATED_BODY()
 363: public:
 366: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 367: 	FUIExtensionHandle ExtensionHandle;
 370: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 371: 	FGameplayTag ExtensionPointTag;
 374: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 375: 	int32 Priority = INDEX_NONE;
 378: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 379: 	TObjectPtr<UObject> Data = nullptr;
 382: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 383: 	TObjectPtr<UObject> ContextObject = nullptr;
 384: };
 388: DECLARE_DYNAMIC_DELEGATE_TwoParams(FExtendExtensionPointDynamicDelegate, EUIExtensionAction, Action,
 389:                                    const FUIExtensionRequest&, ExtensionRequest);
 406: UCLASS()
 407: class HODGEPODGE_API UUIExtensionSubsystem : public UWorldSubsystem
 408: {
 409: 	GENERATED_BODY()
 411: public:
 432: 	FUIExtensionPointHandle RegisterExtensionPoint(const FGameplayTag& ExtensionPointTag,
 433: 	                                               EUIExtensionPointMatch ExtensionPointTagMatchType,
 434: 	                                               const TArray<UClass*>& AllowedDataClasses,
 435: 	                                               FExtendExtensionPointDelegate ExtensionCallback);
 447: 	FUIExtensionPointHandle RegisterExtensionPointForContext(const FGameplayTag& ExtensionPointTag,
 448: 	                                                         UObject* ContextObject,
 449: 	                                                         EUIExtensionPointMatch ExtensionPointTagMatchType,
 450: 	                                                         const TArray<UClass*>& AllowedDataClasses,
 451: 	                                                         FExtendExtensionPointDelegate ExtensionCallback);
 460: 	FUIExtensionHandle RegisterExtensionAsWidget(const FGameplayTag& ExtensionPointTag,
 461: 	                                             TSubclassOf<UUserWidget> WidgetClass, int32 Priority);
 471: 	FUIExtensionHandle RegisterExtensionAsWidgetForContext(const FGameplayTag& ExtensionPointTag,
 472: 	                                                       UObject* ContextObject, TSubclassOf<UUserWidget> WidgetClass,
 473: 	                                                       int32 Priority);
 484: 	FUIExtensionHandle RegisterExtensionAsData(const FGameplayTag& ExtensionPointTag, UObject* ContextObject,
 485: 	                                           UObject* Data, int32 Priority);
 491: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 492: 	void UnregisterExtension(const FUIExtensionHandle& ExtensionHandle);
 498: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 499: 	void UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle);
 506: 	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
 508: protected:
 513: 	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
 517: 	virtual void Deinitialize() override;
 525: 	void NotifyExtensionPointOfExtensions(TSharedPtr<FUIExtensionPoint>& ExtensionPoint);
 534: 	void NotifyExtensionPointsOfExtension(EUIExtensionAction Action, TSharedPtr<FUIExtension>& Extension);
 537: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="UI Extension",
 538: 		meta = (DisplayName = "Register Extension Point"))
 546: 	FUIExtensionPointHandle K2_RegisterExtensionPoint(FGameplayTag ExtensionPointTag,
 547: 	                                                  EUIExtensionPointMatch ExtensionPointTagMatchType,
 548: 	                                                  const TArray<UClass*>& AllowedDataClasses,
 549: 	                                                  FExtendExtensionPointDynamicDelegate ExtensionCallback);
 552: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension",
 553: 		meta = (DisplayName = "Register Extension (Widget)"))
 560: 	FUIExtensionHandle K2_RegisterExtensionAsWidget(FGameplayTag ExtensionPointTag,
 561: 	                                                TSubclassOf<UUserWidget> WidgetClass, int32 Priority = -1);
 572: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension",
 573: 		meta = (DisplayName = "Register Extension (Widget For Context)"))
 581: 	FUIExtensionHandle K2_RegisterExtensionAsWidgetForContext(FGameplayTag ExtensionPointTag,
 582: 	                                                          TSubclassOf<UUserWidget> WidgetClass,
 583: 	                                                          UObject* ContextObject, int32 Priority = -1);
 590: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="UI Extension",
 591: 		meta = (DisplayName = "Register Extension (Data)"))
 598: 	FUIExtensionHandle K2_RegisterExtensionAsData(FGameplayTag ExtensionPointTag, UObject* Data, int32 Priority = -1);
 604: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="UI Extension",
 605: 		meta = (DisplayName = "Register Extension (Data For Context)"))
 613: 	FUIExtensionHandle K2_RegisterExtensionAsDataForContext(FGameplayTag ExtensionPointTag, UObject* ContextObject,
 614: 	                                                        UObject* Data, int32 Priority = -1);
 621: 	FUIExtensionRequest CreateExtensionRequest(const TSharedPtr<FUIExtension>& Extension);
 623: private:
 625: 	typedef TArray<TSharedPtr<FUIExtensionPoint>> FExtensionPointList;
 628: 	TMap<FGameplayTag, FExtensionPointList> ExtensionPointMap;
 631: 	typedef TArray<TSharedPtr<FUIExtension>> FExtensionList;
 634: 	TMap<FGameplayTag, FExtensionList> ExtensionMap;
 635: };
 640: UCLASS()
 641: class HODGEPODGE_API UUIExtensionHandleFunctions : public UBlueprintFunctionLibrary
 642: {
 643: 	GENERATED_BODY()
 645: public:
 647: 	UUIExtensionHandleFunctions()
 648: 	{
 649: 	}
 655: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 656: 	static void Unregister(UPARAM(ref) FUIExtensionHandle& Handle);
 663: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 664: 	static bool IsValid(UPARAM(ref) FUIExtensionHandle& Handle);
 665: };
 669: UCLASS()
 670: class HODGEPODGE_API UUIExtensionPointHandleFunctions : public UBlueprintFunctionLibrary
 671: {
 672: 	GENERATED_BODY()
 674: public:
 676: 	UUIExtensionPointHandleFunctions()
 677: 	{
 678: 	}
 684: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 685: 	static void Unregister(UPARAM(ref) FUIExtensionPointHandle& Handle);
 692: 	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
 693: 	static bool IsValid(UPARAM(ref) FUIExtensionPointHandle& Handle);
 694: };
```

## HodgeActionWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Foundation/HodgeActionWidget.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeActionWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CommonActionWidget.h"
   9: #include "HodgeActionWidget.generated.h"
  11: class UEnhancedInputLocalPlayerSubsystem;
  12: class UInputAction;
  16: UCLASS(BlueprintType, Blueprintable)
  17: class UHodgeActionWidget : public UCommonActionWidget
  18: {
  19: 	GENERATED_BODY()
  21: public:
  27: 	virtual FSlateBrush GetIcon() const override;
  32: 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
  33: 	const TObjectPtr<UInputAction> AssociatedInputAction;
  35: private:
  40: 	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
  42: };
```

## HodgeButtonBase.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeButtonBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CommonButtonBase.h"
  10: #include "HodgeButtonBase.generated.h"
  12: class UObject;
  13: struct FFrame;
  16: UCLASS(Abstract, BlueprintType, Blueprintable)
  17: class UHodgeButtonBase : public UCommonButtonBase
  18: {
  19: 	GENERATED_BODY()
  21: public:
  25: 	UFUNCTION(BlueprintCallable)
  26: 	void SetButtonText(const FText& InText);
  28: protected:
  31: 	virtual void NativePreConstruct() override;
  36: 	virtual void UpdateInputActionWidget() override;
  40: 	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;
  44: 	void RefreshButtonText();
  49: 	UFUNCTION(BlueprintImplementableEvent)
  50: 	void UpdateButtonText(const FText& InText);
  53: 	UFUNCTION(BlueprintImplementableEvent)
  54: 	void UpdateButtonStyle();
  56: private:
  58: 	UPROPERTY(EditAnywhere, Category="Button", meta=(InlineEditConditionToggle))
  59: 	uint8 bOverride_ButtonText : 1;
  62: 	UPROPERTY(EditAnywhere, Category="Button", meta=( editcondition="bOverride_ButtonText" ))
  63: 	FText ButtonText;
  64: };
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
   6: #pragma once
   8: #include "CommonActivatableWidget.h"
   9: #include "GameplayTagContainer.h"
  11: #include "HodgeControllerDisconnectedScreen.generated.h"
  13: class UHorizontalBox;
  14: class UObject;
  15: class UCommonButtonBase;
  16: struct FPlatformUserSelectionCompleteParams;
  23: UCLASS(Abstract, BlueprintType, Blueprintable)
  24: class UHodgeControllerDisconnectedScreen : public UCommonActivatableWidget
  25: {
  26: 	GENERATED_BODY()
  27: public:
  31: 	UHodgeControllerDisconnectedScreen(const FObjectInitializer& ObjectInitializer);
  33: protected:
  35: 	virtual void NativeOnActivated() override;
  38: 	virtual void HandleChangeUserClicked();
  46: 	virtual void HandleChangeUserCompleted(const FPlatformUserSelectionCompleteParams& Params);
  55: 	virtual bool ShouldDisplayChangeUserButton() const;
  63: 	UPROPERTY(EditDefaultsOnly)
  64: 	FGameplayTagContainer PlatformSupportsUserChangeTags;
  74: 	UPROPERTY(meta = (BindWidget))
  75: 	TObjectPtr<UHorizontalBox> HBox_SwitchUser;
  83: 	UPROPERTY(meta = (BindWidget))
  84: 	TObjectPtr<UCommonButtonBase> Button_ChangeUser;
  85: };
```

## HodgeLoadingScreenSubsystem.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Foundation/HodgeLoadingScreenSubsystem.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgeLoadingScreenSubsystem.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "Subsystems/GameInstanceSubsystem.h"
   9: #include "Templates/SubclassOf.h"
  11: #include "UObject/WeakObjectPtr.h"
  12: #include "HodgeLoadingScreenSubsystem.generated.h"
  14: class UObject;
  15: class UUserWidget;
  16: struct FFrame;
  18: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadingScreenWidgetChangedDelegate, TSubclassOf<UUserWidget>, NewWidgetClass);
  25: UCLASS()
  26: class HODGEPODGE_API UHodgeLoadingScreenSubsystem : public UGameInstanceSubsystem
  27: {
  28: 	GENERATED_BODY()
  30: public:
  32: 	UHodgeLoadingScreenSubsystem();
  38: 	UFUNCTION(BlueprintCallable)
  39: 	void SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass);
  45: 	UFUNCTION(BlueprintPure)
  46: 	TSubclassOf<UUserWidget> GetLoadingScreenContentWidget() const;
  48: private:
  50: 	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess))
  51: 	FLoadingScreenWidgetChangedDelegate OnLoadingScreenWidgetChanged;
  54: 	UPROPERTY()
  55: 	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;
  56: };
```

## HodgePrimaryGameLayout.h

绑定 Designer 四个容器的本地玩家根布局，Push/Pop、异步加载取消与令牌恢复；不再生成运行时控件树。

源码：[Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "CommonUserWidget.h"
   6: #include "GameplayTagContainer.h"
   7: #include "HodgePrimaryGameLayout.generated.h"
   9: class UCommonActivatableWidget;
  10: class UCommonActivatableWidgetContainerBase;
  11: struct FStreamableHandle;
  14: UCLASS(Blueprintable)
  15: class HODGEPODGE_API UHodgePrimaryGameLayout : public UCommonUserWidget
  16: {
  17: 	GENERATED_BODY()
  19: public:
  24: 	UFUNCTION(BlueprintCallable, Category="Hodge|UI")
  25: 	void RegisterLayer(FGameplayTag Tag, UCommonActivatableWidgetContainerBase* Container);
  30: 	UFUNCTION(BlueprintPure, Category="Hodge|UI")
  31: 	UCommonActivatableWidgetContainerBase* GetLayer(FGameplayTag Tag) const;
  37: 	UFUNCTION(BlueprintCallable, Category="Hodge|UI")
  38: 	UCommonActivatableWidget* Push(FGameplayTag Tag, TSubclassOf<UCommonActivatableWidget> WidgetClass);
  42: 	UFUNCTION(BlueprintCallable, Category="Hodge|UI")
  43: 	void Pop(UCommonActivatableWidget* Widget);
  47: 	UFUNCTION(BlueprintPure, Category="Hodge|UI")
  48: 	bool HasBlockingPage() const;
  56: 	FGuid PushAsync(FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass,
  57: 	                TFunction<void(UCommonActivatableWidget*)> Completed, bool bSuspendInput = true);
  61: 	void CancelPush(FGuid Request);
  63: 	void ReleaseLayout();
  67: 	int32 GetPendingRequestCount() const { return Requests.Num(); }
  69: protected:
  71:     UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> GameLayer;
  73:     UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> GameMenuLayer;
  75:     UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> MenuLayer;
  77:     UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> ModalLayer;
  79: 	virtual void NativeOnInitialized() override;
  81: 	virtual void NativeDestruct() override;
  83: private:
  85: 	UPROPERTY(Transient)
  86: 	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
  89: 	struct FRequest
  90: 	{
  92: 		TSharedPtr<FStreamableHandle> Load;
  94: 		TWeakObjectPtr<APlayerController> Controller;
  96: 		TFunction<void(UCommonActivatableWidget*)> Completed;
  98: 		FName InputToken;
  99: 	};
 102: 	TMap<FGuid, FRequest> Requests;
 104: 	bool bReleased = false;
 110: 	void FinishPush(FGuid Request, FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass);
 111: };
```

## ApplyFrontendPerfSettingsAction.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Frontend/ApplyFrontendPerfSettingsAction.h](../../../Source/Hodgepodge/Public/UI/Frontend/ApplyFrontendPerfSettingsAction.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "GameFeatureAction.h"
  10: #include "ApplyFrontendPerfSettingsAction.generated.h"
  12: class UObject;
  13: struct FGameFeatureActivatingContext;
  14: struct FGameFeatureDeactivatingContext;
  23: UCLASS(MinimalAPI, meta = (DisplayName = "Use Frontend Perf Settings"))
  24: class UApplyFrontendPerfSettingsAction final : public UGameFeatureAction
  25: {
  26: 	GENERATED_BODY()
  28: public:
  33: 	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
  37: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
  40: private:
  42: 	static int32 ApplicationCounter;
  43: };
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
   6: #pragma once
   8: #include "Engine/DataAsset.h"
  10: #include "HodgeLobbyBackground.generated.h"
  12: class UObject;
  13: class UWorld;
  19: UCLASS(config=EditorPerProjectUserSettings, MinimalAPI)
  20: class UHodgeLobbyBackground : public UPrimaryDataAsset
  21: {
  22: 	GENERATED_BODY()
  24: public:
  27: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
  28: 	TSoftObjectPtr<UWorld> BackgroundLevel;
  29: };
```

## HodgeAbilityIconWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeAbilityIconWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeAbilityIconWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Blueprint/UserWidget.h"
   4: #include "GameplayTagContainer.h"
   5: #include "HodgeAbilityIconWidget.generated.h"
   6: class UImage;
   7: class UTextBlock;
   8: class UButton;
   9: class UTexture2D;
  10: class UMaterialInterface;
  11: class UMaterialInstanceDynamic;
  12: class UHodgeGameplayUIDataSource;
  13: struct FHodgeUIAbilityDisplayState;
  14: struct FHodgeUIResonanceSnapshot;
  17: USTRUCT(BlueprintType)
  18: struct HODGEPODGE_API FHodgeAbilityIconVisualState
  19: {
  20: 	GENERATED_BODY()
  22: 	UPROPERTY(BlueprintReadOnly) float CooldownRemaining=0.f;
  24: 	UPROPERTY(BlueprintReadOnly) float CooldownReadyPercent=1.f;
  26: 	UPROPERTY(BlueprintReadOnly) float EnergyPercent=0.f;
  28: 	UPROPERTY(BlueprintReadOnly) bool bCoolingDown=false;
  30: 	UPROPERTY(BlueprintReadOnly) bool bCanActivate=false;
  32: 	UPROPERTY(BlueprintReadOnly) bool bReady=false;
  33: };
  36: UCLASS(Blueprintable)
  37: class HODGEPODGE_API UHodgeAbilityIconWidget : public UUserWidget
  38: {
  39: 	GENERATED_BODY()
  40: public:
  42: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability") FGameplayTag InputTag;
  44: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability") bool bShowEnergyRing=false;
  46: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") TObjectPtr<UTexture2D> IconTexture;
  47: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") TObjectPtr<UMaterialInterface> IconMaterial;
  48: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") TObjectPtr<UMaterialInterface> EnergyRingMaterial;
  50: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") FText Caption;
  51: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") FText KeyLabel;
  53: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") FLinearColor FillColor=FLinearColor(1.f,.85f,.35f,1.f);
  54: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") int32 FontSize=22;
  56: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style", meta=(ClampMin="32",ClampMax="256")) float IconSize=72.f;
  57: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style", meta=(ClampMin="0",ClampMax="1")) float CooldownDarkness=.25f;
  59: 	UPROPERTY(EditAnywhere, Category="Preview", meta=(ClampMin="0",ClampMax="1")) float PreviewEnergyPercent=.65f;
  60: 	UPROPERTY(EditAnywhere, Category="Preview", meta=(ClampMin="0",ClampMax="1")) float PreviewCooldownReadyPercent=1.f;
  62: 	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeAbilityIconVisualState GetVisualState() const { return VisualState; }
  64: 	static FHodgeAbilityIconVisualState EvaluateState(const FHodgeUIAbilityDisplayState& Ability,const FHodgeUIResonanceSnapshot& Resource,bool bEnergy);
  65: protected:
  67: 	virtual void NativeConstruct() override;
  69: 	virtual void SynchronizeProperties() override;
  71: 	virtual void NativeTick(const FGeometry& Geometry,float DeltaTime) override;
  73: 	virtual void NativeDestruct() override;
  74: private:
  76: 	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UImage> SkillIcon;
  77: 	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UImage> EnergyRing;
  78: 	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> CooldownText;
  79: 	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> KeyText;
  80: 	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UButton> AbilityButton;
  81: 	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> IconMID;
  82: 	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> RingMID;
  83: 	UPROPERTY(Transient) TObjectPtr<UHodgeGameplayUIDataSource> Data;
  84: 	FHodgeAbilityIconVisualState VisualState;
  86: 	void ApplyStyle();
  88: 	void RefreshDisplay();
  90: 	UFUNCTION() void RequestAbility();
  91: };
```

## HodgeActivatableWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "CommonActivatableWidget.h"
  12: #include "HodgeActivatableWidget.generated.h"
  16: struct FUIInputConfig;
  20: UENUM(BlueprintType)
  21: enum class EHodgeWidgetInputMode : uint8
  22: {
  24: 	Default,
  27: 	GameAndMenu,
  30: 	Game,
  33: 	Menu
  34: };
  40: UCLASS(Abstract, Blueprintable)
  41: class HODGEPODGE_API UHodgeActivatableWidget : public UCommonActivatableWidget
  42: {
  43: 	GENERATED_BODY()
  45: public:
  50: 	UHodgeActivatableWidget(const FObjectInitializer& ObjectInitializer);
  52: public:
  60: 	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
  64: #if WITH_EDITOR
  71: 	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree,
  72: 	                                        class IWidgetCompilerLog& CompileLog) const override;
  73: #endif
  75: protected:
  80: 	UPROPERTY(EditDefaultsOnly, Category = Input)
  81: 	EHodgeWidgetInputMode InputConfig = EHodgeWidgetInputMode::Default;
  86: 	UPROPERTY(EditDefaultsOnly, Category = Input)
  87: 	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
  88: };
```

## HodgeGameViewportClient.h

引擎 CommonUI 视口路由，已在 DefaultEngine.ini 启用。

源码：[Source/Hodgepodge/Public/UI/HodgeGameViewportClient.h](../../../Source/Hodgepodge/Public/UI/HodgeGameViewportClient.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "CommonGameViewportClient.h"
   6: #include "HodgeGameViewportClient.generated.h"
   9: UCLASS()
  10: class HODGEPODGE_API UHodgeGameViewportClient : public UCommonGameViewportClient
  11: {
  12:     GENERATED_BODY()
  13: };
```

## HodgeHUDLayout.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeHUDLayout.h](../../../Source/Hodgepodge/Public/UI/HodgeHUDLayout.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "HodgeActivatableWidget.h"
  14: #include "Containers/Ticker.h"
  18: #include "GameplayTagContainer.h"
  20: #include "HodgeHUDLayout.generated.h"
  24: class UCommonActivatableWidget;
  27: class UObject;
  30: class UHodgeControllerDisconnectedScreen;
  33: class UCommonActivatableWidgetStack;
  48: UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "Hodge HUD Layout", Category = "Hodge|HUD"))
  49: class HODGEPODGE_API UHodgeHUDLayout : public UHodgeActivatableWidget
  50: {
  51: 	GENERATED_BODY()
  53: public:
  58: 	UHodgeHUDLayout(const FObjectInitializer& ObjectInitializer);
  65: 	virtual void NativeOnInitialized() override;
  71: 	virtual void NativeDestruct() override;
  73: 	virtual void NativeOnDeactivated() override;
  77: 	UFUNCTION(BlueprintCallable, Category="Hodge|UI")
  78: 	void ToggleHUDMenu();
  80: protected:
  86: 	void HandleEscapeAction();
  88: 	void CloseOwnedMenu();
  90: 	FGuid PendingEscapeMenu;
  92: 	TWeakObjectPtr<UCommonActivatableWidget> EscapeMenuInstance;
  96: 	void EnsureMenuLayerStack();
 117: 	void HandleInputDeviceConnectionChanged(EInputDeviceConnectionState NewConnectionState,
 118: 	                                        FPlatformUserId PlatformUserId, FInputDeviceId InputDeviceId);
 137: 	void HandleInputDevicePairingChanged(FInputDeviceId InputDeviceId, FPlatformUserId NewUserPlatformId,
 138: 	                                     FPlatformUserId OldUserPlatformId);
 151: 	void NotifyControllerStateChangeForDisconnectScreen();
 168: 	virtual void ProcessControllerDevicesHavingChangedForDisconnectScreen();
 181: 	virtual bool ShouldPlatformDisplayControllerDisconnectScreen() const;
 195: 	UFUNCTION(BlueprintNativeEvent, Category="Controller Disconnect Menu")
 196: 	void DisplayControllerDisconnectedMenu();
 206: 	UFUNCTION(BlueprintNativeEvent, Category="Controller Disconnect Menu")
 207: 	void HideControllerDisconnectedMenu();
 217: 	UPROPERTY(EditDefaultsOnly)
 218: 	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
 225: 	UPROPERTY(EditDefaultsOnly, Category="Controller Disconnect Menu")
 226: 	TSubclassOf<UHodgeControllerDisconnectedScreen> ControllerDisconnectedScreen;
 242: 	UPROPERTY(EditDefaultsOnly, Category="Controller Disconnect Menu")
 243: 	FGameplayTagContainer PlatformRequiresControllerDisconnectScreen;
 251: 	UPROPERTY(Transient)
 252: 	TObjectPtr<UCommonActivatableWidget> SpawnedControllerDisconnectScreen;
 255: 	UPROPERTY(meta = (BindWidgetOptional))
 256: 	TObjectPtr<UCommonActivatableWidgetStack> MenuLayerStack;
 264: 	FTSTicker::FDelegateHandle RequestProcessControllerStateHandle;
 265: };
```

## HodgeJoystickWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeJoystickWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeJoystickWidget.h)

项目内直接 include（不是运行调用关系）：[UI/HodgeSimulatedInputWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "UI/HodgeSimulatedInputWidget.h"
  10: #include "HodgeJoystickWidget.generated.h"
  12: class UImage;
  13: class UObject;
  14: struct FGeometry;
  15: struct FPointerEvent;
  26: UCLASS(meta=( DisplayName="Hodge Joystick" ))
  27: class HODGEPODGE_API UHodgeJoystickWidget : public UHodgeSimulatedInputWidget
  28: {
  29: 	GENERATED_BODY()
  31: public:
  36: 	UHodgeJoystickWidget(const FObjectInitializer& ObjectInitializer);
  44: 	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  50: 	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  56: 	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  60: 	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
  65: 	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
  68: protected:
  82: 	void HandleTouchDelta(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent);
  86: 	void StopInputSimulation();
  90: 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
  91: 	float StickRange = 50.0f;
  95: 	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
  96: 	TObjectPtr<UImage> JoystickBackground;
 100: 	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
 101: 	TObjectPtr<UImage> JoystickForeground;
 105: 	UPROPERTY(BlueprintReadWrite, EditAnywhere)
 106: 	bool bNegateYAxis = false;
 110: 	UPROPERTY(Transient)
 111: 	FVector2D TouchOrigin = FVector2D::ZeroVector;
 114: 	UPROPERTY(Transient)
 115: 	FVector2D StickVector = FVector2D::ZeroVector;
 116: };
```

## HodgeResonanceEnergyWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeResonanceEnergyWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeResonanceEnergyWidget.h)

项目内直接 include（不是运行调用关系）：[UI/HodgeAbilityIconWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeAbilityIconWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "UI/HodgeAbilityIconWidget.h"
   4: #include "HodgeResonanceEnergyWidget.generated.h"
   7: UCLASS(Blueprintable)
   8: class HODGEPODGE_API UHodgeResonanceEnergyWidget : public UHodgeAbilityIconWidget
   9: {
  10: 	GENERATED_BODY()
  11: public:
  13: 	UHodgeResonanceEnergyWidget();
  14: };
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
   6: #pragma once
   8: #include "CommonUserWidget.h"
   9: #include "HodgeSimulatedInputWidget.generated.h"
  11: class UEnhancedInputLocalPlayerSubsystem;
  12: class UInputAction;
  13: class UCommonHardwareVisibilityBorder;
  14: class UEnhancedPlayerInput;
  21: UCLASS(meta=( DisplayName="Hodge Simulated Input Widget" ))
  22: class HODGEPODGE_API UHodgeSimulatedInputWidget : public UCommonUserWidget
  23: {
  24: 	GENERATED_BODY()
  26: public:
  30: 	UHodgeSimulatedInputWidget(const FObjectInitializer& ObjectInitializer);
  33: #if WITH_EDITOR
  37: 	virtual const FText GetPaletteCategory() override;
  38: #endif
  43: 	virtual void NativeConstruct() override;
  45: 	virtual void NativeDestruct() override;
  51: 	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  58: 	UFUNCTION(BlueprintCallable)
  59: 	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
  65: 	UEnhancedPlayerInput* GetPlayerInput() const;
  71: 	UFUNCTION(BlueprintCallable)
  72: 	const UInputAction* GetAssociatedAction() const { return AssociatedAction; }
  78: 	UFUNCTION(BlueprintCallable)
  79: 	FKey GetSimulatedKey() const { return KeyToSimulate; }
  88: 	UFUNCTION(BlueprintCallable)
  89: 	void InputKeyValue(const FVector& Value);
  98: 	UFUNCTION(BlueprintCallable)
  99: 	void InputKeyValue2D(const FVector2D& Value);
 102: 	UFUNCTION(BlueprintCallable)
 103: 	void FlushSimulatedInput();
 105: protected:
 108: 	void QueryKeyToSimulate();
 112: 	UFUNCTION()
 113: 	void OnControlMappingsRebuilt();
 117: 	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
 118: 	TObjectPtr<UCommonHardwareVisibilityBorder> CommonVisibilityBorder = nullptr;
 122: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 123: 	TObjectPtr<const UInputAction> AssociatedAction = nullptr;
 127: 	UPROPERTY(BlueprintReadOnly, EditAnywhere)
 128: 	FKey FallbackBindingKey = EKeys::Gamepad_Right2D;
 132: 	FKey KeyToSimulate;
 133: };
```

## HodgeTaggedWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeTaggedWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeTaggedWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "CommonUserWidget.h"
  14: #include "GameplayTagContainer.h"
  16: #include "HodgeTaggedWidget.generated.h"
  18: class UObject;
  32: UCLASS(Abstract, Blueprintable)
  33: class UHodgeTaggedWidget : public UCommonUserWidget
  34: {
  35: 	GENERATED_BODY()
  37: public:
  42: 	UHodgeTaggedWidget(const FObjectInitializer& ObjectInitializer);
  52: 	virtual void SetVisibility(ESlateVisibility InVisibility) override;
  61: 	virtual void NativeConstruct() override;
  66: 	virtual void NativeDestruct() override;
  70: protected:
  75: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
  76: 	FGameplayTagContainer HiddenByTags;
  81: 	UPROPERTY(EditAnywhere, Category = "HUD")
  82: 	ESlateVisibility ShownVisibility = ESlateVisibility::Visible;
  87: 	UPROPERTY(EditAnywhere, Category = "HUD")
  88: 	ESlateVisibility HiddenVisibility = ESlateVisibility::Collapsed;
  95: 	bool bWantsToBeVisible = true;
  97: private:
 101: 	void OnWatchedTagsChanged();
 102: };
```

## HodgeTouchRegion.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/HodgeTouchRegion.h](../../../Source/Hodgepodge/Public/UI/HodgeTouchRegion.h)

项目内直接 include（不是运行调用关系）：[UI/HodgeSimulatedInputWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeSimulatedInputWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "UI/HodgeSimulatedInputWidget.h"
  10: #include "HodgeTouchRegion.generated.h"
  12: class UObject;
  13: struct FFrame;
  14: struct FGeometry;
  15: struct FPointerEvent;
  22: UCLASS(meta=( DisplayName="Hodge Touch Region" ))
  23: class HODGEPODGE_API UHodgeTouchRegion : public UHodgeSimulatedInputWidget
  24: {
  25: 	GENERATED_BODY()
  27: public:
  34: 	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  40: 	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  46: 	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
  51: 	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
  57: 	UFUNCTION(BlueprintCallable)
  58: 	bool ShouldSimulateInput() const { return bShouldSimulateInput; }
  60: protected:
  63: 	bool bShouldSimulateInput = false;
  64: };
```

## HodgeIndicatorManagerComponent.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "Components/ControllerComponent.h"
  12: #include "HodgeIndicatorManagerComponent.generated.h"
  16: class AController;
  20: class UIndicatorDescriptor;
  22: class UObject;
  23: struct FFrame;
  36: UCLASS(BlueprintType, Blueprintable)
  37: class HODGEPODGE_API UHodgeIndicatorManagerComponent : public UControllerComponent
  38: {
  39: 	GENERATED_BODY()
  41: public:
  46: 	UHodgeIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);
  54: 	static UHodgeIndicatorManagerComponent* GetComponent(AController* Controller);
  62: 	UFUNCTION(BlueprintCallable, Category = Indicator)
  63: 	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);
  70: 	UFUNCTION(BlueprintCallable, Category = Indicator)
  71: 	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);
  75: 	DECLARE_EVENT_OneParam(UHodgeIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)
  78: 	FIndicatorEvent OnIndicatorAdded;
  81: 	FIndicatorEvent OnIndicatorRemoved;
  88: 	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }
  90: private:
  94: 	UPROPERTY()
  95: 	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
  96: };
```

## IActorIndicatorWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/IActorIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IActorIndicatorWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "CoreMinimal.h"
  12: #include "UObject/ObjectMacros.h"
  15: #include "UObject/Interface.h"
  17: #include "IActorIndicatorWidget.generated.h"
  20: class AActor;
  24: class UIndicatorDescriptor;
  28: UINTERFACE(BlueprintType)
  29: class HODGEPODGE_API UIndicatorWidgetInterface : public UInterface
  30: {
  31: 	GENERATED_BODY()
  32: };
  36: class IIndicatorWidgetInterface
  37: {
  38: 	GENERATED_BODY()
  40: public:
  50: 	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
  51: 	void BindIndicator(UIndicatorDescriptor* Indicator);
  61: 	UFUNCTION(BlueprintNativeEvent, Category = "Indicator")
  62: 	void UnbindIndicator(const UIndicatorDescriptor* Indicator);
  63: };
```

## IndicatorDescriptor.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "Components/SceneComponent.h"
  13: #include "Types/SlateEnums.h"
  15: #include "IndicatorDescriptor.generated.h"
  18: class SWidget;
  21: class UIndicatorDescriptor;
  24: class UHodgeIndicatorManagerComponent;
  27: class UUserWidget;
  29: struct FFrame;
  33: struct FSceneViewProjectionData;
  38: struct FIndicatorProjection
  39: {
  51: 	bool Project(const UIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData,
  52: 	             const FVector2f& ScreenSize, FVector& ScreenPositionWithDepth);
  53: };
  57: UENUM(BlueprintType)
  58: enum class EActorCanvasProjectionMode : uint8
  59: {
  61: 	ComponentPoint,
  64: 	ComponentBoundingBox,
  67: 	ComponentScreenBoundingBox,
  70: 	ActorBoundingBox,
  73: 	ActorScreenBoundingBox
  74: };
  97: UCLASS(BlueprintType)
  98: class HODGEPODGE_API UIndicatorDescriptor : public UObject
  99: {
 100: 	GENERATED_BODY()
 102: public:
 105: 	UIndicatorDescriptor()
 106: 	{
 107: 	}
 109: public:
 116: 	UFUNCTION(BlueprintCallable)
 117: 	UObject* GetDataObject() const { return DataObject; }
 123: 	UFUNCTION(BlueprintCallable)
 124: 	void SetDataObject(UObject* InDataObject) { DataObject = InDataObject; }
 130: 	UFUNCTION(BlueprintCallable)
 131: 	USceneComponent* GetSceneComponent() const { return Component; }
 137: 	UFUNCTION(BlueprintCallable)
 138: 	void SetSceneComponent(USceneComponent* InComponent) { Component = InComponent; }
 145: 	UFUNCTION(BlueprintCallable)
 146: 	FName GetComponentSocketName() const { return ComponentSocketName; }
 152: 	UFUNCTION(BlueprintCallable)
 153: 	void SetComponentSocketName(FName SocketName) { ComponentSocketName = SocketName; }
 160: 	UFUNCTION(BlueprintCallable)
 161: 	TSoftClassPtr<UUserWidget> GetIndicatorClass() const { return IndicatorWidgetClass; }
 167: 	UFUNCTION(BlueprintCallable)
 168: 	void SetIndicatorClass(TSoftClassPtr<UUserWidget> InIndicatorWidgetClass)
 169: 	{
 170: 		IndicatorWidgetClass = InIndicatorWidgetClass;
 171: 	}
 173: public:
 177: 	TWeakObjectPtr<UUserWidget> IndicatorWidget;
 179: public:
 184: 	UFUNCTION(BlueprintCallable)
 185: 	void SetAutoRemoveWhenIndicatorComponentIsNull(bool CanAutomaticallyRemove)
 186: 	{
 187: 		bAutoRemoveWhenIndicatorComponentIsNull = CanAutomaticallyRemove;
 188: 	}
 194: 	UFUNCTION(BlueprintCallable)
 195: 	bool GetAutoRemoveWhenIndicatorComponentIsNull() const { return bAutoRemoveWhenIndicatorComponentIsNull; }
 201: 	bool CanAutomaticallyRemove() const
 202: 	{
 204: 		return bAutoRemoveWhenIndicatorComponentIsNull && !IsValid(GetSceneComponent());
 205: 	}
 207: public:
 217: 	UFUNCTION(BlueprintCallable)
 218: 	bool GetIsVisible() const { return IsValid(GetSceneComponent()) && bVisible; }
 224: 	UFUNCTION(BlueprintCallable)
 225: 	void SetDesiredVisibility(bool InVisible)
 226: 	{
 227: 		bVisible = InVisible;
 228: 	}
 234: 	UFUNCTION(BlueprintCallable)
 235: 	EActorCanvasProjectionMode GetProjectionMode() const { return ProjectionMode; }
 241: 	UFUNCTION(BlueprintCallable)
 242: 	void SetProjectionMode(EActorCanvasProjectionMode InProjectionMode)
 243: 	{
 244: 		ProjectionMode = InProjectionMode;
 245: 	}
 252: 	UFUNCTION(BlueprintCallable)
 253: 	EHorizontalAlignment GetHAlign() const { return HAlignment; }
 259: 	UFUNCTION(BlueprintCallable)
 260: 	void SetHAlign(EHorizontalAlignment InHAlignment)
 261: 	{
 262: 		HAlignment = InHAlignment;
 263: 	}
 270: 	UFUNCTION(BlueprintCallable)
 271: 	EVerticalAlignment GetVAlign() const { return VAlignment; }
 277: 	UFUNCTION(BlueprintCallable)
 278: 	void SetVAlign(EVerticalAlignment InVAlignment)
 279: 	{
 280: 		VAlignment = InVAlignment;
 281: 	}
 288: 	UFUNCTION(BlueprintCallable)
 289: 	bool GetClampToScreen() const { return bClampToScreen; }
 295: 	UFUNCTION(BlueprintCallable)
 296: 	void SetClampToScreen(bool bValue)
 297: 	{
 298: 		bClampToScreen = bValue;
 299: 	}
 306: 	UFUNCTION(BlueprintCallable)
 307: 	bool GetShowClampToScreenArrow() const { return bShowClampToScreenArrow; }
 313: 	UFUNCTION(BlueprintCallable)
 314: 	void SetShowClampToScreenArrow(bool bValue)
 315: 	{
 316: 		bShowClampToScreenArrow = bValue;
 317: 	}
 325: 	UFUNCTION(BlueprintCallable)
 326: 	FVector GetWorldPositionOffset() const { return WorldPositionOffset; }
 332: 	UFUNCTION(BlueprintCallable)
 333: 	void SetWorldPositionOffset(FVector Offset)
 334: 	{
 335: 		WorldPositionOffset = Offset;
 336: 	}
 344: 	UFUNCTION(BlueprintCallable)
 345: 	FVector2D GetScreenSpaceOffset() const { return ScreenSpaceOffset; }
 351: 	UFUNCTION(BlueprintCallable)
 352: 	void SetScreenSpaceOffset(FVector2D Offset)
 353: 	{
 354: 		ScreenSpaceOffset = Offset;
 355: 	}
 363: 	UFUNCTION(BlueprintCallable)
 364: 	FVector GetBoundingBoxAnchor() const { return BoundingBoxAnchor; }
 370: 	UFUNCTION(BlueprintCallable)
 371: 	void SetBoundingBoxAnchor(FVector InBoundingBoxAnchor)
 372: 	{
 373: 		BoundingBoxAnchor = InBoundingBoxAnchor;
 374: 	}
 376: public:
 388: 	UFUNCTION(BlueprintCallable)
 389: 	int32 GetPriority() const { return Priority; }
 395: 	UFUNCTION(BlueprintCallable)
 396: 	void SetPriority(int32 InPriority)
 397: 	{
 398: 		Priority = InPriority;
 399: 	}
 401: public:
 406: 	UHodgeIndicatorManagerComponent* GetIndicatorManagerComponent() { return ManagerPtr.Get(); }
 412: 	void SetIndicatorManagerComponent(UHodgeIndicatorManagerComponent* InManager);
 416: 	UFUNCTION(BlueprintCallable)
 417: 	void UnregisterIndicator();
 419: private:
 421: 	UPROPERTY()
 422: 	bool bVisible = true;
 425: 	UPROPERTY()
 426: 	bool bClampToScreen = false;
 429: 	UPROPERTY()
 430: 	bool bShowClampToScreenArrow = false;
 433: 	UPROPERTY()
 434: 	bool bOverrideScreenPosition = false;
 437: 	UPROPERTY()
 438: 	bool bAutoRemoveWhenIndicatorComponentIsNull = false;
 441: 	UPROPERTY()
 442: 	EActorCanvasProjectionMode ProjectionMode = EActorCanvasProjectionMode::ComponentPoint;
 445: 	UPROPERTY()
 446: 	TEnumAsByte<EHorizontalAlignment> HAlignment = HAlign_Center;
 449: 	UPROPERTY()
 450: 	TEnumAsByte<EVerticalAlignment> VAlignment = VAlign_Center;
 453: 	UPROPERTY()
 454: 	int32 Priority = 0;
 458: 	UPROPERTY()
 459: 	FVector BoundingBoxAnchor = FVector(0.5, 0.5, 0.5);
 462: 	UPROPERTY()
 463: 	FVector2D ScreenSpaceOffset = FVector2D(0, 0);
 466: 	UPROPERTY()
 467: 	FVector WorldPositionOffset = FVector(0, 0, 0);
 469: private:
 472: 	friend class SActorCanvas;
 476: 	UPROPERTY()
 477: 	TObjectPtr<UObject> DataObject;
 480: 	UPROPERTY()
 481: 	TObjectPtr<USceneComponent> Component;
 485: 	UPROPERTY()
 486: 	FName ComponentSocketName = NAME_None;
 490: 	UPROPERTY()
 491: 	TSoftClassPtr<UUserWidget> IndicatorWidgetClass;
 495: 	UPROPERTY()
 496: 	TWeakObjectPtr<UHodgeIndicatorManagerComponent> ManagerPtr;
 500: 	TWeakPtr<SWidget> Content;
 504: 	TWeakPtr<SWidget> CanvasHost;
 505: };
```

## IndicatorLayer.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "Components/Widget.h"
  12: #include "IndicatorLayer.generated.h"
  16: class SActorCanvas;
  19: class SWidget;
  21: class UObject;
  26: UCLASS()
  27: class HODGEPODGE_API UIndicatorLayer : public UWidget
  28: {
  29: 	GENERATED_UCLASS_BODY()
  31: public:
  34: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
  35: 	FSlateBrush ArrowBrush;
  37: protected:
  45: 	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
  52: 	virtual TSharedRef<SWidget> RebuildWidget() override;
  56: protected:
  60: 	TSharedPtr<SActorCanvas> MyActorCanvas;
  61: };
```

## IndicatorLibrary.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLibrary.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLibrary.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "Kismet/BlueprintFunctionLibrary.h"
  12: #include "IndicatorLibrary.generated.h"
  16: class AController;
  20: class UHodgeIndicatorManagerComponent;
  22: class UObject;
  23: struct FFrame;
  27: UCLASS()
  28: class HODGEPODGE_API UIndicatorLibrary : public UBlueprintFunctionLibrary
  29: {
  30: 	GENERATED_BODY()
  32: public:
  35: 	UIndicatorLibrary();
  45: 	UFUNCTION(BlueprintCallable, Category = Indicator)
  46: 	static UHodgeIndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);
  47: };
```

## SActorCanvas.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/SActorCanvas.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  13: #include "Blueprint/UserWidgetPool.h"
  17: #include "Widgets/SPanel.h"
  21: class FActiveTimerHandle;
  24: class FArrangedChildren;
  27: class FChildren;
  30: class FPaintArgs;
  34: class FReferenceCollector;
  37: class FSlateRect;
  40: class FSlateWindowElementList;
  43: class FWidgetStyle;
  47: struct FStreamableHandle;
  50: class UIndicatorDescriptor;
  53: class UHodgeIndicatorManagerComponent;
  57: struct FSlateBrush;
  68: class SActorCanvas : public SPanel, public FGCObject
  69: {
  70: public:
  81: 	class FSlot : public TSlotBase<FSlot>
  82: 	{
  83: 	public:
  88: 		FSlot(UIndicatorDescriptor* InIndicator)
  89: 			: TSlotBase<FSlot>()
  91: 			  , Indicator(InIndicator)
  93: 			  , ScreenPosition(FVector2D::ZeroVector)
  95: 			  , Depth(0)
  97: 			  , Priority(0.f)
  99: 			  , bIsIndicatorVisible(true)
 101: 			  , bInFrontOfCamera(true)
 103: 			  , bHasValidScreenPosition(false)
 105: 			  , bDirty(true)
 107: 			  , bWasIndicatorClamped(false)
 109: 			  , bWasIndicatorClampedStatusChanged(false)
 110: 		{
 111: 		}
 114: 		SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
 115: 		SLATE_SLOT_END_ARGS()
 118: 		using TSlotBase<FSlot>::Construct;
 124: 		bool GetIsIndicatorVisible() const { return bIsIndicatorVisible; }
 130: 		void SetIsIndicatorVisible(bool bVisible)
 131: 		{
 133: 			if (bIsIndicatorVisible != bVisible)
 134: 			{
 135: 				bIsIndicatorVisible = bVisible;
 136: 				bDirty = true;
 137: 			}
 140: 			RefreshVisibility();
 141: 		}
 147: 		FVector2D GetScreenPosition() const { return ScreenPosition; }
 153: 		void SetScreenPosition(FVector2D InScreenPosition)
 154: 		{
 156: 			if (ScreenPosition != InScreenPosition)
 157: 			{
 158: 				ScreenPosition = InScreenPosition;
 159: 				bDirty = true;
 160: 			}
 161: 		}
 167: 		double GetDepth() const { return Depth; }
 173: 		void SetDepth(double InDepth)
 174: 		{
 177: 			if (Depth != InDepth)
 178: 			{
 179: 				Depth = InDepth;
 180: 				bDirty = true;
 181: 			}
 182: 		}
 188: 		int32 GetPriority() const { return Priority; }
 194: 		void SetPriority(int32 InPriority)
 195: 		{
 198: 			if (Priority != InPriority)
 199: 			{
 200: 				Priority = InPriority;
 201: 				bDirty = true;
 202: 			}
 203: 		}
 209: 		bool GetInFrontOfCamera() const { return bInFrontOfCamera; }
 215: 		void SetInFrontOfCamera(bool bInFront)
 216: 		{
 218: 			if (bInFrontOfCamera != bInFront)
 219: 			{
 220: 				bInFrontOfCamera = bInFront;
 221: 				bDirty = true;
 222: 			}
 225: 			RefreshVisibility();
 226: 		}
 232: 		bool HasValidScreenPosition() const { return bHasValidScreenPosition; }
 238: 		void SetHasValidScreenPosition(bool bValidScreenPosition)
 239: 		{
 241: 			if (bHasValidScreenPosition != bValidScreenPosition)
 242: 			{
 243: 				bHasValidScreenPosition = bValidScreenPosition;
 244: 				bDirty = true;
 245: 			}
 248: 			RefreshVisibility();
 249: 		}
 255: 		bool bIsDirty() const { return bDirty; }
 259: 		void ClearDirtyFlag()
 260: 		{
 261: 			bDirty = false;
 262: 		}
 268: 		bool WasIndicatorClamped() const { return bWasIndicatorClamped; }
 274: 		void SetWasIndicatorClamped(bool bWasClamped) const
 275: 		{
 277: 			if (bWasClamped != bWasIndicatorClamped)
 278: 			{
 279: 				bWasIndicatorClamped = bWasClamped;
 280: 				bWasIndicatorClampedStatusChanged = true;
 281: 			}
 282: 		}
 288: 		bool WasIndicatorClampedStatusChanged() const { return bWasIndicatorClampedStatusChanged; }
 292: 		void ClearIndicatorClampedStatusChangedFlag()
 293: 		{
 294: 			bWasIndicatorClampedStatusChanged = false;
 295: 		}
 297: 	private:
 301: 		void RefreshVisibility()
 302: 		{
 305: 			const bool bIsVisible = bIsIndicatorVisible && bHasValidScreenPosition;
 312: 			GetWidget()->SetVisibility(bIsVisible ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed);
 313: 		}
 319: 		UIndicatorDescriptor* Indicator;
 322: 		FVector2D ScreenPosition;
 325: 		double Depth;
 328: 		int32 Priority;
 331: 		uint8 bIsIndicatorVisible : 1;
 334: 		uint8 bInFrontOfCamera : 1;
 337: 		uint8 bHasValidScreenPosition : 1;
 340: 		uint8 bDirty : 1;
 350: 		mutable uint8 bWasIndicatorClamped : 1;
 353: 		mutable uint8 bWasIndicatorClampedStatusChanged : 1;
 356: 		friend class SActorCanvas;
 357: 	};
 362: 	class FArrowSlot : public TSlotBase<FArrowSlot>
 363: 	{
 364: 	};
 368: 	SLATE_BEGIN_ARGS(SActorCanvas)
 369: 		{
 372: 			_Visibility = EVisibility::HitTestInvisible;
 373: 		}
 377: 		SLATE_SLOT_ARGUMENT(SActorCanvas::FSlot, Slots)
 380: 	SLATE_END_ARGS()
 384: 	SActorCanvas()
 386: 		: CanvasChildren(this)
 388: 		  , ArrowChildren(this)
 390: 		  , AllChildren(this)
 391: 	{
 393: 		AllChildren.AddChildren(CanvasChildren);
 396: 		AllChildren.AddChildren(ArrowChildren);
 397: 	}
 402: 	~SActorCanvas();
 420: 	void Construct(const FArguments& InArgs, const FLocalPlayerContext& InCtx,
 421: 	               const FSlateBrush* ActorCanvasArrowBrush);
 431: 	virtual void
 432: 	OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
 440: 	virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }
 447: 	virtual FChildren* GetChildren() override { return &AllChildren; }
 461: 	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
 462: 	                      FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
 463: 	                      bool bParentEnabled) const;
 472: 	void SetDrawElementsInOrder(bool bInDrawElementsInOrder) { bDrawElementsInOrder = bInDrawElementsInOrder; }
 479: 	virtual FString GetReferencerName() const override;
 487: 	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
 489: private:
 494: 	void OnIndicatorAdded(UIndicatorDescriptor* Indicator);
 500: 	void OnIndicatorRemoved(UIndicatorDescriptor* Indicator);
 507: 	void AddIndicatorForEntry(UIndicatorDescriptor* Indicator);
 513: 	void RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator);
 520: 	void OnIndicatorClassLoaded(TWeakObjectPtr<UIndicatorDescriptor> IndicatorPtr);
 523: 	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;
 530: 	FScopedWidgetSlotArguments AddActorSlot(UIndicatorDescriptor* Indicator);
 537: 	int32 RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget);
 544: 	void SetShowAnyIndicators(bool bIndicators);
 553: 	EActiveTimerReturnType UpdateCanvas(double InCurrentTime, float InDeltaTime);
 565: 	void GetOffsetAndSize(const UIndicatorDescriptor* Indicator,
 566: 	                      FVector2D& OutSize,
 567: 	                      FVector2D& OutOffset,
 568: 	                      FVector2D& OutPaddingMin,
 569: 	                      FVector2D& OutPaddingMax) const;
 574: 	void UpdateActiveTimer();
 576: private:
 579: 	TArray<TObjectPtr<UIndicatorDescriptor>> AllIndicators;
 583: 	TArray<UIndicatorDescriptor*> InactiveIndicators;
 587: 	FLocalPlayerContext LocalPlayerContext;
 591: 	TWeakObjectPtr<UHodgeIndicatorManagerComponent> IndicatorComponentPtr;
 595: 	TPanelChildren<FSlot> CanvasChildren;
 599: 	mutable TPanelChildren<FArrowSlot> ArrowChildren;
 603: 	FCombinedChildren AllChildren;
 607: 	FUserWidgetPool IndicatorPool;
 611: 	const FSlateBrush* ActorCanvasArrowBrush = nullptr;
 615: 	mutable int32 NextArrowIndex = 0;
 619: 	mutable int32 ArrowIndexLastUpdate = 0;
 625: 	bool bDrawElementsInOrder = false;
 629: 	bool bShowAnyIndicators = false;
 633: 	mutable TOptional<FGeometry> OptionalPaintGeometry;
 636: 	TSharedPtr<FActiveTimerHandle> TickHandle;
 641: 	TArray<TSharedPtr<FStreamableHandle>> IndicatorLoadHandles;
 642: };
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

GameInstance 所属的本地玩家 UI 管理器：根布局、Controller 绑定、输入挂起和精确撤销；专服不创建。

源码：[Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "Subsystems/GameInstanceSubsystem.h"
   6: #include "UObject/SoftObjectPtr.h"
   7: #include "HodgeUIManagerSubsystem.generated.h"
   8: class UHodgePrimaryGameLayout;
   9: class UHodgeGameplayUIDataSource;
  10: class ULocalPlayer;
  11: class APlayerController;
  12: struct FStreamableHandle;
  13: DECLARE_MULTICAST_DELEGATE_TwoParams(FHodgeRootLayoutChanged, ULocalPlayer*, UHodgePrimaryGameLayout*);
  16: USTRUCT()
  17: struct FHodgePlayerUILayout
  18: {
  19:     GENERATED_BODY()
  21:     UPROPERTY(Transient) TObjectPtr<UHodgePrimaryGameLayout> Root;
  23:     UPROPERTY(Transient) TObjectPtr<UHodgeGameplayUIDataSource> GameplayData;
  25:     UPROPERTY(Transient) TWeakObjectPtr<APlayerController> Controller;
  27:     FDelegateHandle ControllerDelegate;
  29:     FDelegateHandle InputModeDelegate;
  31:     TSet<FName> InputTokens;
  33:     TSharedPtr<FStreamableHandle> RootLoad;
  34: };
  37: UCLASS(Config=Game)
  38: class HODGEPODGE_API UHodgeUIManagerSubsystem : public UGameInstanceSubsystem
  39: {
  40:     GENERATED_BODY()
  41: public:
  46:     virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
  50:     virtual void Initialize(FSubsystemCollectionBase& Collection) override;
  52:     virtual void Deinitialize() override;
  56:     void PlayerAdded(ULocalPlayer* Player);
  60:     void PlayerRemoved(ULocalPlayer* Player);
  65:     UFUNCTION(BlueprintPure, Category="Hodge|UI") UHodgePrimaryGameLayout* GetRootLayout(ULocalPlayer* Player) const;
  70:     UFUNCTION(BlueprintPure, Category="Hodge|UI") bool IsGameInputAllowed(ULocalPlayer* Player) const;
  75:     UFUNCTION(BlueprintPure, Category="Hodge|UI", meta=(WorldContext="WorldContextObject"))
  76:     static UHodgeUIManagerSubsystem* GetUIManager(const UObject* WorldContextObject);
  81:     UFUNCTION(BlueprintPure, Category="Hodge|UI") static UHodgePrimaryGameLayout* GetRootLayoutForController(APlayerController* Controller);
  86:     UFUNCTION(BlueprintPure, Category="Hodge|UI") static UHodgeGameplayUIDataSource* GetGameplayDataForController(APlayerController* Controller);
  91:     static bool AllowsGameplayInput(const APlayerController* Controller);
  96:     void SuspendInput(ULocalPlayer* Player, FName Token);
 101:     void ResumeInput(ULocalPlayer* Player, FName Token);
 105:     void RefreshInputState(ULocalPlayer* Player);
 107:     FHodgeRootLayoutChanged OnRootLayoutChanged;
 111:     int32 GetPlayerLayoutCount() const { return Players.Num(); }
 112: protected:
 114:     UPROPERTY(Config, EditDefaultsOnly, Category="UI") TSoftClassPtr<UHodgePrimaryGameLayout> RootLayoutClass;
 115: private:
 117:     UPROPERTY(Transient) TMap<TObjectPtr<ULocalPlayer>, FHodgePlayerUILayout> Players;
 122:     void ControllerChanged(ULocalPlayer* Player, APlayerController* Controller);
 127:     void CreateRoot(ULocalPlayer* Player, TWeakObjectPtr<APlayerController> Controller);
 131:     void DestroyRoot(ULocalPlayer* Player);
 135:     void ClearGameplayInput(ULocalPlayer* Player);
 136: };
```

## HodgeUIMessaging.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Subsystem/HodgeUIMessaging.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIMessaging.h)

**全部为注释或空白；无有效声明/实现。**

## HodgeLockOnIndicatorWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Targeting/HodgeLockOnIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/Targeting/HodgeLockOnIndicatorWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Blueprint/UserWidget.h"
   4: #include "HodgeLockOnIndicatorWidget.generated.h"
   6: UCLASS()
   7: class HODGEPODGE_API UHodgeLockOnIndicatorWidget : public UUserWidget
   8: {
   9: 	GENERATED_BODY()
  10: protected:
  11: 	virtual TSharedRef<SWidget> RebuildWidget() override;
  12: };
```

## CircumferenceMarkerWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Weapons/CircumferenceMarkerWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/CircumferenceMarkerWidget.h)

项目内直接 include（不是运行调用关系）：[UI/Weapons/SCircumferenceMarkerWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/SCircumferenceMarkerWidget.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "Components/Widget.h"
   9: #include "UI/Weapons/SCircumferenceMarkerWidget.h"
  11: #include "CircumferenceMarkerWidget.generated.h"
  13: class SWidget;
  14: class UObject;
  15: struct FFrame;
  18: UCLASS()
  19: class UCircumferenceMarkerWidget : public UWidget
  20: {
  21: 	GENERATED_BODY()
  23: public:
  27: 	UCircumferenceMarkerWidget(const FObjectInitializer& ObjectInitializer);
  30: public:
  32: 	virtual void SynchronizeProperties() override;
  33: protected:
  37: 	virtual TSharedRef<SWidget> RebuildWidget() override;
  41: public:
  45: 	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
  48: public:
  51: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
  52: 	TArray<FCircumferenceMarkerEntry> MarkerList;
  56: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=(ClampMin=0.0))
  57: 	float Radius = 48.0f;
  61: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
  62: 	FSlateBrush MarkerImage;
  67: 	UPROPERTY(EditAnywhere, Category=Corner)
  68: 	uint8 bReticleCornerOutsideSpreadRadius : 1;
  70: public:
  75: 	UFUNCTION(BlueprintCallable, Category = "Appearance")
  76: 	void SetRadius(float InRadius);
  78: private:
  81: 	TSharedPtr<SCircumferenceMarkerWidget> MyMarkerWidget;
  82: };
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
   6: #pragma once
   8: #include "Styling/CoreStyle.h"
   9: #include "Widgets/Accessibility/SlateWidgetAccessibleTypes.h"
  10: #include "Widgets/DeclarativeSyntaxSupport.h"
  11: #include "Widgets/SLeafWidget.h"
  13: #include "SCircumferenceMarkerWidget.generated.h"
  15: class FPaintArgs;
  16: class FSlateRect;
  17: class FSlateWindowElementList;
  18: class FWidgetStyle;
  19: struct FGeometry;
  20: struct FSlateBrush;
  23: USTRUCT(BlueprintType)
  24: struct FCircumferenceMarkerEntry
  25: {
  26: 	GENERATED_BODY()
  30: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ForceUnits=deg))
  31: 	float PositionAngle = 0.0f;
  35: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ForceUnits=deg))
  36: 	float ImageRotationAngle = 0.0f;
  37: };
  40: class SCircumferenceMarkerWidget : public SLeafWidget
  41: {
  42: 	SLATE_BEGIN_ARGS(SCircumferenceMarkerWidget)
  43: 		: _MarkerBrush(FCoreStyle::Get().GetBrush("Throbber.CircleChunk"))
  44: 		, _Radius(48.0f)
  45: 	{
  46: 	}
  48: 		SLATE_ARGUMENT(const FSlateBrush*, MarkerBrush)
  50: 		SLATE_ARGUMENT(TArray<FCircumferenceMarkerEntry>, MarkerList)
  52: 		SLATE_ATTRIBUTE(float, Radius)
  54: 		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
  55: 	SLATE_END_ARGS()
  57: public:
  61: 	void Construct(const FArguments& InArgs);
  64: 	SCircumferenceMarkerWidget();
  77: 	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
  82: 	virtual FVector2D ComputeDesiredSize(float) const override;
  86: 	virtual bool ComputeVolatility() const override { return true; }
  92: 	void SetRadius(float NewRadius);
  96: 	void SetMarkerList(TArray<FCircumferenceMarkerEntry>& NewMarkerList);
  98: private:
 105: 	FSlateRenderTransform GetMarkerRenderTransform(const FCircumferenceMarkerEntry& Marker, const float BaseRadius, const float HUDScale) const;
 107: private:
 110: 	const FSlateBrush* MarkerBrush;
 114: 	TArray<FCircumferenceMarkerEntry> MarkerList;
 118: 	TAttribute<float> Radius;
 122: 	TAttribute<FSlateColor> ColorAndOpacity;
 124: 	bool bColorAndOpacitySet;
 129: 	uint8 bReticleCornerOutsideSpreadRadius : 1;
 130: };
```

## SHitMarkerConfirmationWidget.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/UI/Weapons/SHitMarkerConfirmationWidget.h](../../../Source/Hodgepodge/Public/UI/Weapons/SHitMarkerConfirmationWidget.h)

**全部为注释或空白；无有效声明/实现。**
