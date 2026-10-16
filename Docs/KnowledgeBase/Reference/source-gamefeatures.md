# GameFeatures 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## GameFeatureAction_AddAbilities.cpp

面向配置 Actor 授予能力、属性与 AbilitySet，维护撤销句柄。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddAbilities.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)

定义候选（多行签名仅展示首行）：

- L51: `void UGameFeatureAction_AddAbilities::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)`
- L62: `void UGameFeatureAction_AddAbilities::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`
- L73: `EDataValidationResult UGameFeatureAction_AddAbilities::IsDataValid(FDataValidationContext& Context) const`
- L190: `void UGameFeatureAction_AddAbilities::AddToWorld(const FWorldContext& WorldContext,`
- L217: `void UGameFeatureAction_AddAbilities::Reset(FPerContextData& ActiveData)`
- L227: `void UGameFeatureAction_AddAbilities::HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex,`
- L243: `void UGameFeatureAction_AddAbilities::AddActorAbilities(AActor* Actor, int32 EntryIndex,`
- L404: `void UGameFeatureAction_AddAbilities::RemoveActorAbilities(AActor* Actor, int32 EntryIndex, FPerContextData& ActiveData, bool bCancelDirect)`
- L415: `void UGameFeatureAction_AddAbilities::RetireExtension(const TSharedRef<FActorExtensions>& Entry, bool bCancelDirect)`
- L426: `void UGameFeatureAction_AddAbilities::RevokeExtension(FActorExtensions& Entry, bool bCancelDirectAbilities)`
- L442: `UActorComponent* UGameFeatureAction_AddAbilities::FindOrAddComponentForActor(`

## GameFeatureAction_AddGameplayCuePath.cpp

声明和校验 Cue 路径；Policy 添加路径主体已有，但观察者注册和注销清理仍缺。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddGameplayCuePath.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddGameplayCuePath.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddGameplayCuePath.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h)

定义候选（多行签名仅展示首行）：

- L21: `UGameFeatureAction_AddGameplayCuePath::UGameFeatureAction_AddGameplayCuePath()`
- L30: `EDataValidationResult UGameFeatureAction_AddGameplayCuePath::IsDataValid(FDataValidationContext& Context) const`

## GameFeatureAction_AddInputBinding.cpp

有效扩展事件添加/移除额外 InputConfig；Hero 侧移除已实现并与 EndPlay 清理配套。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputBinding.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputBinding.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddInputBinding.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputBinding.h)、[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)、[Input/HodgeInputConfig.h](../../../Source/Hodgepodge/Public/Input/HodgeInputConfig.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

定义候选（多行签名仅展示首行）：

- L55: `void UGameFeatureAction_AddInputBinding::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)`
- L73: `void UGameFeatureAction_AddInputBinding::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`
- L91: `EDataValidationResult UGameFeatureAction_AddInputBinding::IsDataValid(FDataValidationContext& Context) const`
- L123: `void UGameFeatureAction_AddInputBinding::AddToWorld(const FWorldContext& WorldContext,`
- L158: `void UGameFeatureAction_AddInputBinding::Reset(FPerContextData& ActiveData)`
- L184: `void UGameFeatureAction_AddInputBinding::HandlePawnExtension(AActor* Actor, FName EventName,`
- L210: `void UGameFeatureAction_AddInputBinding::AddInputMappingForPlayer(APawn* Pawn, FPerContextData& ActiveData)`
- L255: `void UGameFeatureAction_AddInputBinding::RemoveInputMapping(APawn* Pawn, FPerContextData& ActiveData)`

## GameFeatureAction_AddInputContextMapping.cpp

有效 Controller 扩展添加 IMC，包含设置注册与诊断日志；记录/撤销需验收。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputContextMapping.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputContextMapping.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddInputContextMapping.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h)、[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)

定义候选（多行签名仅展示首行）：

- L44: `void UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering()`
- L54: `void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)`
- L76: `void UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`
- L93: `void UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()`
- L103: `void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts()`
- L122: `void UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)`
- L146: `void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)`
- L184: `void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts()`
- L205: `void UGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance(`
- L228: `void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)`
- L266: `EDataValidationResult UGameFeatureAction_AddInputContextMapping::IsDataValid(FDataValidationContext& Context) const`
- L298: `void UGameFeatureAction_AddInputContextMapping::AddToWorld(const FWorldContext& WorldContext,`
- L333: `void UGameFeatureAction_AddInputContextMapping::Reset(FPerContextData& ActiveData)`
- L358: `void UGameFeatureAction_AddInputContextMapping::HandleControllerExtension(`
- L390: `void UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData)`
- L438: `void UGameFeatureAction_AddInputContextMapping::RemoveInputMapping(APlayerController* PlayerController,`

## GameFeatureAction_AddWidget.cpp

Widget 注入迁移草稿，当前实现停用。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddWidget.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h)、[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)、[Core/HUD/HodgeHUD.h](../../../Source/Hodgepodge/Public/Core/HUD/HodgeHUD.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)、[UI/Foundation/HodgePrimaryGameLayout.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h)

定义候选（多行签名仅展示首行）：

- L54: `void UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`
- L73: `void UGameFeatureAction_AddWidgets::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)`
- L88: `EDataValidationResult UGameFeatureAction_AddWidgets::IsDataValid(FDataValidationContext& Context) const`
- L163: `void UGameFeatureAction_AddWidgets::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)`
- L197: `void UGameFeatureAction_AddWidgets::ClearActorContents(FPerActorData& Data)`
- L210: `void UGameFeatureAction_AddWidgets::Reset(FPerContextData& ActiveData)`
- L223: `void UGameFeatureAction_AddWidgets::HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)`
- L242: `void UGameFeatureAction_AddWidgets::AddWidgets(AActor* Actor, FPerContextData& ActiveData)`
- L293: `void UGameFeatureAction_AddWidgets::RemoveWidgets(AActor* Actor, FPerContextData& ActiveData)`

## GameFeatureAction_SplitscreenConfig.cpp

GameFeature 激活期间的分屏策略调整。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_SplitscreenConfig.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_SplitscreenConfig.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_SplitscreenConfig.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_SplitscreenConfig.h)、[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)

定义候选（多行签名仅展示首行）：

- L24: `void UGameFeatureAction_SplitscreenConfig::OnGameFeatureDeactivating(`
- L81: `void UGameFeatureAction_SplitscreenConfig::AddToWorld(`

## GameFeatureAction_WorldActionBase.cpp

按游戏世界和激活上下文组织 Action 生命周期。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_WorldActionBase.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_WorldActionBase.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)

定义候选（多行签名仅展示首行）：

- L14: `void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)`
- L36: `void UGameFeatureAction_WorldActionBase::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`
- L50: `void UGameFeatureAction_WorldActionBase::HandleGameInstanceStart(`

## HodgeGameFeaturePolicy.cpp

已配置的 GameFeature 策略；Hotfix 观察者注册，Cue 路径观察者创建仍注释。

源码：[Source/Hodgepodge/Private/GameFeatures/HodgeGameFeaturePolicy.cpp](../../../Source/Hodgepodge/Private/GameFeatures/HodgeGameFeaturePolicy.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/HodgeGameFeaturePolicy.h](../../../Source/Hodgepodge/Public/GameFeatures/HodgeGameFeaturePolicy.h)、[AbilitySystem/HodgeGameplayCueManager.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h)、[GameFeatures/GameFeatureAction_AddGameplayCuePath.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h)

定义候选（多行签名仅展示首行）：

- L18: `UHodgeGameFeaturePolicy::UHodgeGameFeaturePolicy(const FObjectInitializer& ObjectInitializer)`
- L27: `UHodgeGameFeaturePolicy& UHodgeGameFeaturePolicy::Get()`
- L37: `void UHodgeGameFeaturePolicy::InitGameFeatureManager()`
- L62: `void UHodgeGameFeaturePolicy::ShutdownGameFeatureManager()`
- L82: `TArray<FPrimaryAssetId> UHodgeGameFeaturePolicy::GetPreloadAssetListForGameFeature(`
- L95: `const TArray<FName> UHodgeGameFeaturePolicy::GetPreloadBundleStateForGameFeature() const`
- L105: `void UHodgeGameFeaturePolicy::GetGameFeatureLoadingMode(`
- L119: `bool UHodgeGameFeaturePolicy::IsPluginAllowed(const FString& PluginURL) const`
- L131: `void UHodgeGameFeature_HotfixManager::OnGameFeatureLoading(`
- L159: `void UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(`
- L243: `void UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(`

## GameFeatureAction_AddAbilities.h

面向配置 Actor 授予能力、属性与 AbilitySet，维护撤销句柄。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   8: #pragma once
  11: #include "GameFeatureAction_WorldActionBase.h"
  14: #include "Abilities/GameplayAbility.h"
  17: #include "Data/HodgeAbilitySet.h"
  20: #include "GameFeatureAction_AddAbilities.generated.h"
  23: struct FWorldContext;
  26: class UInputAction;
  29: class UAttributeSet;
  32: class UDataTable;
  35: struct FComponentRequestHandle;
  38: class UHodgeAbilitySet;
  39: class UGameFrameworkComponentManager;
  42: USTRUCT(BlueprintType)
  43: struct FHodgeAbilityGrant
  44: {
  45: 	GENERATED_BODY()
  49: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
  50: 	TSoftClassPtr<UGameplayAbility> AbilityType;
  57: };
  60: USTRUCT(BlueprintType)
  61: struct FHodgeAttributeSetGrant
  62: {
  63: 	GENERATED_BODY()
  67: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
  68: 	TSoftClassPtr<UAttributeSet> AttributeSetType;
  72: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
  73: 	TSoftObjectPtr<UDataTable> InitializationData;
  74: };
  77: USTRUCT()
  78: struct FGameFeatureAbilitiesEntry
  79: {
  80: 	GENERATED_BODY()
  84: 	UPROPERTY(EditAnywhere, Category="Abilities")
  85: 	TSoftClassPtr<AActor> ActorClass;
  89: 	UPROPERTY(EditAnywhere, Category="Abilities")
  90: 	TArray<FHodgeAbilityGrant> GrantedAbilities;
  94: 	UPROPERTY(EditAnywhere, Category="Attributes")
  95: 	TArray<FHodgeAttributeSetGrant> GrantedAttributes;
  99: 	UPROPERTY(EditAnywhere, Category="Attributes", meta=(AssetBundles="Client,Server"))
 100: 	TArray<TSoftObjectPtr<const UHodgeAbilitySet>> GrantedAbilitySets;
 101: };
 110: UCLASS(MinimalAPI, meta = (DisplayName = "Add Abilities"))
 111: class UGameFeatureAction_AddAbilities final : public UGameFeatureAction_WorldActionBase
 112: {
 113: 	GENERATED_BODY()
 115: public:
 121: 	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
 127: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
 131: #if WITH_EDITOR
 137: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
 138: #endif
 142: 	UPROPERTY(EditAnywhere, Category="Abilities", meta=(TitleProperty="ActorClass", ShowOnlyInnerProperties))
 143: 	TArray<FGameFeatureAbilitiesEntry> AbilitiesList;
 145: private:
 146: 	friend struct FHodgeGameFeatureGrantTestAccess;
 148: 	enum class EGrantState : uint8 { Waiting, Applying, Applied, Failed, Removed };
 150: 	struct FActorExtensions
 151: 	{
 152: 		TWeakObjectPtr<AActor> Actor;
 153: 		TWeakObjectPtr<UHodgeAbilitySystemComponent> GrantedASC;
 154: 		TWeakObjectPtr<UGameFrameworkComponentManager> InitManager;
 155: 		FDelegateHandle InitStateHandle;
 156: 		TArray<FGameplayAbilitySpecHandle> DirectAbilities;
 157: 		FHodgeAbilitySet_GrantedHandles GrantedHandles;
 158: 		EGrantState State = EGrantState::Waiting;
 159: 		bool bApplying = false;
 160: 		bool bCommitted = false;
 161: 		bool bHasIrreversibleEffects = false;
 162: 		bool bCancelDirectOnRemoval = false;
 163: 	};
 166: 	struct FPerContextData
 167: 	{
 169: 		TMap<TWeakObjectPtr<AActor>, TMap<int32, TSharedPtr<FActorExtensions>>> ActiveExtensions;
 170: 		bool bActive = true;
 173: 		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
 174: 	};
 177: 	TMap<FGameFeatureStateChangeContext, TSharedPtr<FPerContextData>> ContextData;
 185: 	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
 192: 	void Reset(FPerContextData& ActiveData);
 201: 	void HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext);
 209: 	void AddActorAbilities(AActor* Actor, int32 EntryIndex, const TSharedRef<FPerContextData>& ActiveData);
 216: 	void RemoveActorAbilities(AActor* Actor, int32 EntryIndex, FPerContextData& ActiveData, bool bCancelDirect = false);
 218: 	void RetireExtension(const TSharedRef<FActorExtensions>& Extension, bool bCancelDirect = false);
 220: 	void RevokeExtension(FActorExtensions& Extension, bool bCancelDirectAbilities);
 223: 	template<class ComponentType>
 230: 	ComponentType* FindOrAddComponentForActor(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData)
 231: 	{
 234: 		return Cast<ComponentType>(FindOrAddComponentForActor(ComponentType::StaticClass(), Actor, AbilitiesEntry, ActiveData));
 235: 	}
 245: 	UActorComponent* FindOrAddComponentForActor(UClass* ComponentType, AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData);
 246: };
```

## GameFeatureAction_AddGameplayCuePath.h

声明和校验 Cue 路径；Policy 添加路径主体已有，但观察者注册和注销清理仍缺。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   7: #pragma once
  10: #include "GameFeatureAction.h"
  13: #include "UObject/SoftObjectPath.h"
  16: #include "GameFeatureAction_AddGameplayCuePath.generated.h"
  25: UCLASS(MinimalAPI, meta = (DisplayName = "Add Gameplay Cue Path"))
  26: class UGameFeatureAction_AddGameplayCuePath final : public UGameFeatureAction
  27: {
  28: 	GENERATED_BODY()
  30: public:
  33: 	UGameFeatureAction_AddGameplayCuePath();
  36: #if WITH_EDITOR
  42: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  43: #endif
  50: 	const TArray<FDirectoryPath>& GetDirectoryPathsToAdd() const { return DirectoryPathsToAdd; }
  52: private:
  56: 	UPROPERTY(EditAnywhere, Category = "Game Feature | Gameplay Cues",
  57: 		meta = (RelativeToGameContentDir, LongPackageName))
  59: 	TArray<FDirectoryPath> DirectoryPathsToAdd;
  60: };
```

## GameFeatureAction_AddInputBinding.h

有效扩展事件添加/移除额外 InputConfig；Hero 侧移除已实现并与 EndPlay 清理配套。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputBinding.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputBinding.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   7: #pragma once
  10: #include "GameFeatureAction_WorldActionBase.h"
  13: #include "UObject/SoftObjectPtr.h"
  16: #include "GameFeatureAction_AddInputBinding.generated.h"
  19: class AActor;
  22: class UInputMappingContext;
  25: class UPlayer;
  28: class APlayerController;
  31: struct FComponentRequestHandle;
  34: class UHodgeInputConfig;
  42: UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Binds"))
  43: class UGameFeatureAction_AddInputBinding final : public UGameFeatureAction_WorldActionBase
  44: {
  45: 	GENERATED_BODY()
  47: public:
  53: 	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
  59: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
  63: #if WITH_EDITOR
  69: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  70: #endif
  75: 	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
  76: 	TArray<TSoftObjectPtr<const UHodgeInputConfig>> InputConfigs;
  78: private:
  80: 	struct FPerContextData
  81: 	{
  83: 		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
  86: 		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
  87: 	};
  90: 	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
  98: 	virtual void AddToWorld(const FWorldContext& WorldContext,
  99: 	                        const FGameFeatureStateChangeContext& ChangeContext) override;
 106: 	void Reset(FPerContextData& ActiveData);
 114: 	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
 121: 	void AddInputMappingForPlayer(APawn* Pawn, FPerContextData& ActiveData);
 128: 	void RemoveInputMapping(APawn* Pawn, FPerContextData& ActiveData);
 129: };
```

## GameFeatureAction_AddInputContextMapping.h

有效 Controller 扩展添加 IMC，包含设置注册与诊断日志；记录/撤销需验收。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   7: #pragma once
  10: #include "GameFeatureAction_WorldActionBase.h"
  13: #include "UObject/SoftObjectPtr.h"
  16: #include "GameFeatureAction_AddInputContextMapping.generated.h"
  19: class AActor;
  22: class UInputMappingContext;
  25: class UPlayer;
  28: class APlayerController;
  31: struct FComponentRequestHandle;
  34: USTRUCT()
  35: struct FInputMappingContextAndPriority
  36: {
  38: 	GENERATED_BODY()
  41: 	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
  42: 	TSoftObjectPtr<UInputMappingContext> InputMapping;
  46: 	UPROPERTY(EditAnywhere, Category="Input")
  47: 	int32 Priority = 0;
  51: 	UPROPERTY(EditAnywhere, Category="Input")
  52: 	bool bRegisterWithSettings = true;
  53: };
  61: UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Mapping"))
  62: class UGameFeatureAction_AddInputContextMapping final : public UGameFeatureAction_WorldActionBase
  63: {
  65: 	GENERATED_BODY()
  67: public:
  71: 	virtual void OnGameFeatureRegistering() override;
  77: 	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
  83: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
  87: 	virtual void OnGameFeatureUnregistering() override;
  91: #if WITH_EDITOR
  97: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  98: #endif
 102: 	UPROPERTY(EditAnywhere, Category="Input")
 103: 	TArray<FInputMappingContextAndPriority> InputMappings;
 105: private:
 107: 	struct FPerContextData
 108: 	{
 110: 		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
 113: 		TArray<TWeakObjectPtr<APlayerController>> ControllersAddedTo;
 114: 	};
 117: 	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
 121: 	FDelegateHandle RegisterInputContextMappingsForGameInstanceHandle;
 127: 	void RegisterInputMappingContexts();
 135: 	void RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);
 143: 	void RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);
 149: 	void UnregisterInputMappingContexts();
 156: 	void UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);
 164: 	void UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);
 172: 	virtual void AddToWorld(const FWorldContext& WorldContext,
 173: 	                        const FGameFeatureStateChangeContext& ChangeContext) override;
 180: 	void Reset(FPerContextData& ActiveData);
 188: 	void HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
 195: 	void AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData);
 202: 	void RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData);
 203: };
```

## GameFeatureAction_AddWidget.h

Widget 注入迁移草稿，当前实现停用。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h)

项目内直接 include（不是运行调用关系）：[UI/Extension/UIExtensionSystem.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionSystem.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  10: #include "CommonActivatableWidget.h"
  14: #include "GameFeatureAction_WorldActionBase.h"
  18: #include "GameplayTagContainer.h"
  22: #include "UI/Extension/UIExtensionSystem.h"
  24: #include "GameFeatureAction_AddWidget.generated.h"
  28: struct FWorldContext;
  32: struct FComponentRequestHandle;
  41: USTRUCT()
  42: struct FHodgeHUDLayoutRequest
  43: {
  44: 	GENERATED_BODY()
  53: 	UPROPERTY(EditAnywhere, Category=UI, meta=(AssetBundles="Client"))
  54: 	TSoftClassPtr<UCommonActivatableWidget> LayoutClass;
  62: 	UPROPERTY(EditAnywhere, Category=UI, meta=(Categories="UI.Layer"))
  63: 	FGameplayTag LayerID;
  64: };
  75: USTRUCT()
  76: struct FHodgeHUDElementEntry
  77: {
  78: 	GENERATED_BODY()
  82: 	UPROPERTY(EditAnywhere, Category=UI, meta=(AssetBundles="Client"))
  83: 	TSoftClassPtr<UUserWidget> WidgetClass;
  88: 	UPROPERTY(EditAnywhere, Category = UI)
  89: 	FGameplayTag SlotID;
  90: };
 109: UCLASS(MinimalAPI, meta = (DisplayName = "Add Widgets"))
 110: class UGameFeatureAction_AddWidgets final : public UGameFeatureAction_WorldActionBase
 111: {
 112: 	GENERATED_BODY()
 113: #if WITH_EDITOR
 114:     friend class UHodgeUIAuthoringLibrary;
 115: #endif
 117: public:
 125: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
 127: #if WITH_EDITORONLY_DATA
 134: 	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) override;
 136: #endif
 142: #if WITH_EDITOR
 150: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
 152: #endif
 156: private:
 162: 	UPROPERTY(EditAnywhere, Category=UI, meta=(TitleProperty="{LayerID} -> {LayoutClass}"))
 164: 	TArray<FHodgeHUDLayoutRequest> Layout;
 171: 	UPROPERTY(EditAnywhere, Category=UI, meta=(TitleProperty="{SlotID} -> {WidgetClass}"))
 173: 	TArray<FHodgeHUDElementEntry> Widgets;
 175: private:
 181: 	struct FPerActorData
 182: 	{
 184: 		TWeakObjectPtr<AActor> Actor;
 186: 		TWeakObjectPtr<ULocalPlayer> Player;
 188: 		TWeakObjectPtr<class UHodgePrimaryGameLayout> Root;
 190: 		TWeakObjectPtr<class UHodgeUIManagerSubsystem> Manager;
 192: 		FDelegateHandle RootChangedDelegate;
 194: 		bool bAdded = false;
 196: 		TArray<TWeakObjectPtr<UCommonActivatableWidget>> LayoutsAdded;
 199: 		TArray<FUIExtensionHandle> ExtensionHandles;
 200: 	};
 209: 	struct FPerContextData
 210: 	{
 212: 		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
 215: 		TMap<FObjectKey, FPerActorData> ActorData;
 216: 	};
 219: 	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
 230: 	virtual void AddToWorld(const FWorldContext& WorldContext,
 231: 	                        const FGameFeatureStateChangeContext& ChangeContext) override;
 240: 	void Reset(FPerContextData& ActiveData);
 244: 	void ClearActorContents(FPerActorData& Data);
 253: 	void HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
 260: 	void AddWidgets(AActor* Actor, FPerContextData& ActiveData);
 267: 	void RemoveWidgets(AActor* Actor, FPerContextData& ActiveData);
 268: };
```

## GameFeatureAction_SplitscreenConfig.h

GameFeature 激活期间的分屏策略调整。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_SplitscreenConfig.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_SplitscreenConfig.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "GameFeatureAction_WorldActionBase.h"
   9: #include "UObject/ObjectKey.h"
  11: #include "GameFeatureAction_SplitscreenConfig.generated.h"
  13: class UObject;
  14: struct FGameFeatureDeactivatingContext;
  15: struct FGameFeatureStateChangeContext;
  16: struct FWorldContext;
  25: UCLASS(MinimalAPI, meta = (DisplayName = "Splitscreen Config"))
  26: class UGameFeatureAction_SplitscreenConfig final : public UGameFeatureAction_WorldActionBase
  27: {
  28: 	GENERATED_BODY()
  30: public:
  37: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
  48: 	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
  52: public:
  54: 	UPROPERTY(EditAnywhere, Category=Action)
  55: 	bool bDisableSplitscreen = true;
  57: private:
  59: 	TArray<FObjectKey> LocalDisableVotes;
  62: 	static TMap<FObjectKey, int32> GlobalDisableVotes;
  63: };
```

## GameFeatureAction_WorldActionBase.h

按游戏世界和激活上下文组织 Action 生命周期。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "GameFeatureAction.h"
   9: #include "GameFeaturesSubsystem.h"
  11: #include "GameFeatureAction_WorldActionBase.generated.h"
  13: class FDelegateHandle;
  14: class UGameInstance;
  15: class UObject;
  16: struct FGameFeatureActivatingContext;
  17: struct FGameFeatureDeactivatingContext;
  18: struct FGameFeatureStateChangeContext;
  19: struct FWorldContext;
  24: UCLASS(Abstract)
  25: class UGameFeatureAction_WorldActionBase : public UGameFeatureAction
  26: {
  27: 	GENERATED_BODY()
  29: public:
  36: 	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
  42: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
  46: private:
  52: 	void HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext);
  59: 	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
  60: 		PURE_VIRTUAL(UGameFeatureAction_WorldActionBase::AddToWorld,);
  62: private:
  64: 	TMap<FGameFeatureStateChangeContext, FDelegateHandle> GameInstanceStartHandles;
  65: };
```

## HodgeGameFeaturePolicy.h

已配置的 GameFeature 策略；Hotfix 观察者注册，Cue 路径观察者创建仍注释。

源码：[Source/Hodgepodge/Public/GameFeatures/HodgeGameFeaturePolicy.h](../../../Source/Hodgepodge/Public/GameFeatures/HodgeGameFeaturePolicy.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "GameFeatureStateChangeObserver.h"
   9: #include "GameFeaturesProjectPolicies.h"
  11: #include "HodgeGameFeaturePolicy.generated.h"
  13: class FName;
  14: class UGameFeatureData;
  15: struct FPrimaryAssetId;
  27: UCLASS(MinimalAPI, Config = Game)
  28: class UHodgeGameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
  29: {
  30:     GENERATED_BODY()
  32: public:
  37:     HODGEPODGE_API static UHodgeGameFeaturePolicy& Get();
  43:     UHodgeGameFeaturePolicy(const FObjectInitializer& ObjectInitializer);
  50:     virtual void InitGameFeatureManager() override;
  54:     virtual void ShutdownGameFeatureManager() override;
  63:     virtual TArray<FPrimaryAssetId> GetPreloadAssetListForGameFeature(
  64:         const UGameFeatureData* GameFeatureToLoad,
  65:         bool bIncludeLoadedAssets = false
  66:     ) const override;
  73:     virtual bool IsPluginAllowed(const FString& PluginURL) const override;
  79:     virtual const TArray<FName> GetPreloadBundleStateForGameFeature() const override;
  87:     virtual void GetGameFeatureLoadingMode(
  88:         bool& bLoadClientData,
  89:         bool& bLoadServerData
  90:     ) const override;
  94: private:
  99:     UPROPERTY(Transient)
 100:     TArray<TObjectPtr<UObject>> Observers;
 101: };
 106: UCLASS()
 107: class UHodgeGameFeature_HotfixManager
 108:     : public UObject
 109:     , public IGameFeatureStateChangeObserver
 110: {
 111:     GENERATED_BODY()
 113: public:
 120:     virtual void OnGameFeatureLoading(
 121:         const UGameFeatureData* GameFeatureData,
 122:         const FString& PluginURL
 123:     ) override;
 124: };
 129: UCLASS()
 130: class UHodgeGameFeature_AddGameplayCuePaths
 131:     : public UObject
 132:     , public IGameFeatureStateChangeObserver
 133: {
 134:     GENERATED_BODY()
 136: public:
 144:     virtual void OnGameFeatureRegistering(
 145:         const UGameFeatureData* GameFeatureData,
 146:         const FString& PluginName,
 147:         const FString& PluginURL
 148:     ) override;
 156:     virtual void OnGameFeatureUnregistering(
 157:         const UGameFeatureData* GameFeatureData,
 158:         const FString& PluginName,
 159:         const FString& PluginURL
 160:     ) override;
 161: };
```
