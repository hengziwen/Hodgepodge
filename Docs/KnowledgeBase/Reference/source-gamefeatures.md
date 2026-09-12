# GameFeatures 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## GameFeatureAction_AddAbilities.cpp

面向配置 Actor 授予能力、属性与 AbilitySet，维护撤销句柄。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddAbilities.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddAbilities.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)

定义候选（多行签名仅展示首行）：

- L39: `void UGameFeatureAction_AddAbilities::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)`
- L57: `void UGameFeatureAction_AddAbilities::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`
- L76: `EDataValidationResult UGameFeatureAction_AddAbilities::IsDataValid(FDataValidationContext& Context) const`
- L195: `void UGameFeatureAction_AddAbilities::AddToWorld(const FWorldContext& WorldContext,`
- L244: `void UGameFeatureAction_AddAbilities::Reset(FPerContextData& ActiveData)`
- L261: `void UGameFeatureAction_AddAbilities::HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex,`
- L292: `void UGameFeatureAction_AddAbilities::AddActorAbilities(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry,`
- L410: `void UGameFeatureAction_AddAbilities::RemoveActorAbilities(AActor* Actor, FPerContextData& ActiveData)`
- L449: `UActorComponent* UGameFeatureAction_AddAbilities::FindOrAddComponentForActor(`

## GameFeatureAction_AddGameplayCuePath.cpp

声明和校验 Cue 路径配置；Policy 内实际增删路径仍注释。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddGameplayCuePath.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddGameplayCuePath.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddGameplayCuePath.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h)

定义候选（多行签名仅展示首行）：

- L18: `UGameFeatureAction_AddGameplayCuePath::UGameFeatureAction_AddGameplayCuePath()`
- L27: `EDataValidationResult UGameFeatureAction_AddGameplayCuePath::IsDataValid(FDataValidationContext& Context) const`

## GameFeatureAction_AddInputBinding.cpp

额外 InputConfig 扩展；依赖 Hero 的主体和添加分支仍注释。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputBinding.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputBinding.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddInputBinding.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputBinding.h)、[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)、[Input/HodgeInputConfig.h](../../../Source/Hodgepodge/Public/Input/HodgeInputConfig.h)

定义候选（多行签名仅展示首行）：

- L50: `void UGameFeatureAction_AddInputBinding::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)`
- L68: `void UGameFeatureAction_AddInputBinding::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`
- L86: `EDataValidationResult UGameFeatureAction_AddInputBinding::IsDataValid(FDataValidationContext& Context) const`
- L118: `void UGameFeatureAction_AddInputBinding::AddToWorld(const FWorldContext& WorldContext,`
- L153: `void UGameFeatureAction_AddInputBinding::Reset(FPerContextData& ActiveData)`
- L179: `void UGameFeatureAction_AddInputBinding::HandlePawnExtension(AActor* Actor, FName EventName,`
- L204: `void UGameFeatureAction_AddInputBinding::AddInputMappingForPlayer(APawn* Pawn, FPerContextData& ActiveData)`
- L249: `void UGameFeatureAction_AddInputBinding::RemoveInputMapping(APawn* Pawn, FPerContextData& ActiveData)`

## GameFeatureAction_AddInputContextMapping.cpp

IMC 与本地玩家/设置集成，Controller 扩展添加分支仍注释。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputContextMapping.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddInputContextMapping.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_AddInputContextMapping.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h)、[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)

定义候选（多行签名仅展示首行）：

- L39: `void UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering()`
- L49: `void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)`
- L67: `void UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`
- L84: `void UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()`
- L94: `void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts()`
- L113: `void UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)`
- L137: `void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)`
- L175: `void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts()`
- L196: `void UGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance(`
- L219: `void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)`
- L257: `EDataValidationResult UGameFeatureAction_AddInputContextMapping::IsDataValid(FDataValidationContext& Context) const`
- L289: `void UGameFeatureAction_AddInputContextMapping::AddToWorld(const FWorldContext& WorldContext,`
- L324: `void UGameFeatureAction_AddInputContextMapping::Reset(FPerContextData& ActiveData)`
- L349: `void UGameFeatureAction_AddInputContextMapping::HandleControllerExtension(`
- L375: `void UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData)`
- L407: `void UGameFeatureAction_AddInputContextMapping::RemoveInputMapping(APlayerController* PlayerController,`

## GameFeatureAction_AddWidget.cpp

Widget 注入迁移草稿，当前实现停用。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_AddWidget.cpp)

**全部为注释或空白；无有效声明/实现。**

## GameFeatureAction_SplitscreenConfig.cpp

GameFeature 激活期间的分屏策略调整。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_SplitscreenConfig.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_SplitscreenConfig.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_SplitscreenConfig.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_SplitscreenConfig.h)、[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)

定义候选（多行签名仅展示首行）：

- L20: `void UGameFeatureAction_SplitscreenConfig::OnGameFeatureDeactivating(`
- L76: `void UGameFeatureAction_SplitscreenConfig::AddToWorld(`

## GameFeatureAction_WorldActionBase.cpp

按游戏世界和激活上下文组织 Action 生命周期。

源码：[Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_WorldActionBase.cpp](../../../Source/Hodgepodge/Private/GameFeatures/GameFeatureAction_WorldActionBase.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)

定义候选（多行签名仅展示首行）：

- L10: `void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)`
- L31: `void UGameFeatureAction_WorldActionBase::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)`
- L44: `void UGameFeatureAction_WorldActionBase::HandleGameInstanceStart(`

## HodgeGameFeaturePolicy.cpp

GameFeature 项目策略和资源加载观察扩展。

源码：[Source/Hodgepodge/Private/GameFeatures/HodgeGameFeaturePolicy.cpp](../../../Source/Hodgepodge/Private/GameFeatures/HodgeGameFeaturePolicy.cpp)

项目内直接 include（不是运行调用关系）：[GameFeatures/HodgeGameFeaturePolicy.h](../../../Source/Hodgepodge/Public/GameFeatures/HodgeGameFeaturePolicy.h)、[GameFeatures/GameFeatureAction_AddGameplayCuePath.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h)

定义候选（多行签名仅展示首行）：

- L14: `UHodgeGameFeaturePolicy::UHodgeGameFeaturePolicy(const FObjectInitializer& ObjectInitializer)`
- L23: `UHodgeGameFeaturePolicy& UHodgeGameFeaturePolicy::Get()`
- L33: `void UHodgeGameFeaturePolicy::InitGameFeatureManager()`
- L58: `void UHodgeGameFeaturePolicy::ShutdownGameFeatureManager()`
- L78: `TArray<FPrimaryAssetId> UHodgeGameFeaturePolicy::GetPreloadAssetListForGameFeature(`
- L91: `const TArray<FName> UHodgeGameFeaturePolicy::GetPreloadBundleStateForGameFeature() const`
- L101: `void UHodgeGameFeaturePolicy::GetGameFeatureLoadingMode(`
- L115: `bool UHodgeGameFeaturePolicy::IsPluginAllowed(const FString& PluginURL) const`
- L127: `void UHodgeGameFeature_HotfixManager::OnGameFeatureLoading(`
- L155: `void UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(`
- L239: `void UHodgeGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(`

## GameFeatureAction_AddAbilities.h

面向配置 Actor 授予能力、属性与 AbilitySet，维护撤销句柄。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddAbilities.h)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   5: #pragma once
   8: #include "GameFeatureAction_WorldActionBase.h"
  11: #include "Abilities/GameplayAbility.h"
  14: #include "Data/HodgeAbilitySet.h"
  17: #include "GameFeatureAction_AddAbilities.generated.h"
  20: struct FWorldContext;
  23: class UInputAction;
  26: class UAttributeSet;
  29: class UDataTable;
  32: struct FComponentRequestHandle;
  35: class UHodgeAbilitySet;
  38: USTRUCT(BlueprintType)
  39: struct FHodgeAbilityGrant
  40: {
  41: 	GENERATED_BODY()
  45: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
  46: 	TSoftClassPtr<UGameplayAbility> AbilityType;
  53: };
  56: USTRUCT(BlueprintType)
  57: struct FHodgeAttributeSetGrant
  58: {
  59: 	GENERATED_BODY()
  63: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
  64: 	TSoftClassPtr<UAttributeSet> AttributeSetType;
  68: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
  69: 	TSoftObjectPtr<UDataTable> InitializationData;
  70: };
  73: USTRUCT()
  74: struct FGameFeatureAbilitiesEntry
  75: {
  76: 	GENERATED_BODY()
  80: 	UPROPERTY(EditAnywhere, Category="Abilities")
  81: 	TSoftClassPtr<AActor> ActorClass;
  85: 	UPROPERTY(EditAnywhere, Category="Abilities")
  86: 	TArray<FHodgeAbilityGrant> GrantedAbilities;
  90: 	UPROPERTY(EditAnywhere, Category="Attributes")
  91: 	TArray<FHodgeAttributeSetGrant> GrantedAttributes;
  95: 	UPROPERTY(EditAnywhere, Category="Attributes", meta=(AssetBundles="Client,Server"))
  96: 	TArray<TSoftObjectPtr<const UHodgeAbilitySet>> GrantedAbilitySets;
  97: };
 106: UCLASS(MinimalAPI, meta = (DisplayName = "Add Abilities"))
 107: class UGameFeatureAction_AddAbilities final : public UGameFeatureAction_WorldActionBase
 108: {
 109: 	GENERATED_BODY()
 111: public:
 114: 	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
 117: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
 121: #if WITH_EDITOR
 123: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
 124: #endif
 128: 	UPROPERTY(EditAnywhere, Category="Abilities", meta=(TitleProperty="ActorClass", ShowOnlyInnerProperties))
 129: 	TArray<FGameFeatureAbilitiesEntry> AbilitiesList;
 131: private:
 133: 	struct FActorExtensions
 134: 	{
 136: 		TArray<FGameplayAbilitySpecHandle> Abilities;
 139: 		TArray<UAttributeSet*> Attributes;
 142: 		TArray<FHodgeAbilitySet_GrantedHandles> AbilitySetHandles;
 143: 	};
 146: 	struct FPerContextData
 147: 	{
 149: 		TMap<AActor*, FActorExtensions> ActiveExtensions;
 152: 		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
 153: 	};
 156: 	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
 160: 	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
 164: 	void Reset(FPerContextData& ActiveData);
 167: 	void HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext);
 170: 	void AddActorAbilities(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData);
 173: 	void RemoveActorAbilities(AActor* Actor, FPerContextData& ActiveData);
 176: 	template<class ComponentType>
 177: 	ComponentType* FindOrAddComponentForActor(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData)
 178: 	{
 181: 		return Cast<ComponentType>(FindOrAddComponentForActor(ComponentType::StaticClass(), Actor, AbilitiesEntry, ActiveData));
 182: 	}
 185: 	UActorComponent* FindOrAddComponentForActor(UClass* ComponentType, AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData);
 186: };
```

## GameFeatureAction_AddGameplayCuePath.h

声明和校验 Cue 路径配置；Policy 内实际增删路径仍注释。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddGameplayCuePath.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   7: #include "GameFeatureAction.h"
  10: #include "UObject/SoftObjectPath.h"
  13: #include "GameFeatureAction_AddGameplayCuePath.generated.h"
  22: UCLASS(MinimalAPI, meta = (DisplayName = "Add Gameplay Cue Path"))
  23: class UGameFeatureAction_AddGameplayCuePath final : public UGameFeatureAction
  24: {
  25: 	GENERATED_BODY()
  27: public:
  29: 	UGameFeatureAction_AddGameplayCuePath();
  32: #if WITH_EDITOR
  34: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  35: #endif
  39: 	const TArray<FDirectoryPath>& GetDirectoryPathsToAdd() const { return DirectoryPathsToAdd; }
  41: private:
  45: 	UPROPERTY(EditAnywhere, Category = "Game Feature | Gameplay Cues",
  46: 		meta = (RelativeToGameContentDir, LongPackageName))
  47: 	TArray<FDirectoryPath> DirectoryPathsToAdd;
  48: };
```

## GameFeatureAction_AddInputBinding.h

额外 InputConfig 扩展；依赖 Hero 的主体和添加分支仍注释。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputBinding.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputBinding.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   7: #include "GameFeatureAction_WorldActionBase.h"
  10: #include "UObject/SoftObjectPtr.h"
  13: #include "GameFeatureAction_AddInputBinding.generated.h"
  16: class AActor;
  19: class UInputMappingContext;
  22: class UPlayer;
  25: class APlayerController;
  28: struct FComponentRequestHandle;
  31: class UHodgeInputConfig;
  39: UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Binds"))
  40: class UGameFeatureAction_AddInputBinding final : public UGameFeatureAction_WorldActionBase
  41: {
  42: 	GENERATED_BODY()
  44: public:
  47: 	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
  50: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
  54: #if WITH_EDITOR
  56: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  57: #endif
  62: 	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
  63: 	TArray<TSoftObjectPtr<const UHodgeInputConfig>> InputConfigs;
  65: private:
  67: 	struct FPerContextData
  68: 	{
  70: 		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
  73: 		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
  74: 	};
  77: 	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
  81: 	virtual void AddToWorld(const FWorldContext& WorldContext,
  82: 	                        const FGameFeatureStateChangeContext& ChangeContext) override;
  86: 	void Reset(FPerContextData& ActiveData);
  89: 	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
  92: 	void AddInputMappingForPlayer(APawn* Pawn, FPerContextData& ActiveData);
  95: 	void RemoveInputMapping(APawn* Pawn, FPerContextData& ActiveData);
  96: };
```

## GameFeatureAction_AddInputContextMapping.h

IMC 与本地玩家/设置集成，Controller 扩展添加分支仍注释。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddInputContextMapping.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   7: #include "GameFeatureAction_WorldActionBase.h"
  10: #include "UObject/SoftObjectPtr.h"
  13: #include "GameFeatureAction_AddInputContextMapping.generated.h"
  16: class AActor;
  19: class UInputMappingContext;
  22: class UPlayer;
  25: class APlayerController;
  28: struct FComponentRequestHandle;
  31: USTRUCT()
  32: struct FInputMappingContextAndPriority
  33: {
  35: 	GENERATED_BODY()
  38: 	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
  39: 	TSoftObjectPtr<UInputMappingContext> InputMapping;
  43: 	UPROPERTY(EditAnywhere, Category="Input")
  44: 	int32 Priority = 0;
  48: 	UPROPERTY(EditAnywhere, Category="Input")
  49: 	bool bRegisterWithSettings = true;
  50: };
  58: UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Mapping"))
  59: class UGameFeatureAction_AddInputContextMapping final : public UGameFeatureAction_WorldActionBase
  60: {
  62: 	GENERATED_BODY()
  64: public:
  67: 	virtual void OnGameFeatureRegistering() override;
  70: 	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
  73: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
  76: 	virtual void OnGameFeatureUnregistering() override;
  80: #if WITH_EDITOR
  82: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  83: #endif
  87: 	UPROPERTY(EditAnywhere, Category="Input")
  88: 	TArray<FInputMappingContextAndPriority> InputMappings;
  90: private:
  92: 	struct FPerContextData
  93: 	{
  95: 		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
  98: 		TArray<TWeakObjectPtr<APlayerController>> ControllersAddedTo;
  99: 	};
 102: 	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
 106: 	FDelegateHandle RegisterInputContextMappingsForGameInstanceHandle;
 111: 	void RegisterInputMappingContexts();
 116: 	void RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);
 121: 	void RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);
 126: 	void UnregisterInputMappingContexts();
 130: 	void UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);
 135: 	void UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);
 139: 	virtual void AddToWorld(const FWorldContext& WorldContext,
 140: 	                        const FGameFeatureStateChangeContext& ChangeContext) override;
 144: 	void Reset(FPerContextData& ActiveData);
 147: 	void HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
 150: 	void AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData);
 153: 	void RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData);
 154: };
```

## GameFeatureAction_AddWidget.h

Widget 注入迁移草稿，当前实现停用。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h)

**全部为注释或空白；无有效声明/实现。**

## GameFeatureAction_SplitscreenConfig.h

GameFeature 激活期间的分屏策略调整。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_SplitscreenConfig.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_SplitscreenConfig.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameFeatureAction_WorldActionBase.h"
   6: #include "UObject/ObjectKey.h"
   8: #include "GameFeatureAction_SplitscreenConfig.generated.h"
  10: class UObject;
  11: struct FGameFeatureDeactivatingContext;
  12: struct FGameFeatureStateChangeContext;
  13: struct FWorldContext;
  22: UCLASS(MinimalAPI, meta = (DisplayName = "Splitscreen Config"))
  23: class UGameFeatureAction_SplitscreenConfig final : public UGameFeatureAction_WorldActionBase
  24: {
  25: 	GENERATED_BODY()
  27: public:
  31: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
  38: 	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
  42: public:
  44: 	UPROPERTY(EditAnywhere, Category=Action)
  45: 	bool bDisableSplitscreen = true;
  47: private:
  49: 	TArray<FObjectKey> LocalDisableVotes;
  52: 	static TMap<FObjectKey, int32> GlobalDisableVotes;
  53: };
```

## GameFeatureAction_WorldActionBase.h

按游戏世界和激活上下文组织 Action 生命周期。

源码：[Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_WorldActionBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameFeatureAction.h"
   6: #include "GameFeaturesSubsystem.h"
   8: #include "GameFeatureAction_WorldActionBase.generated.h"
  10: class FDelegateHandle;
  11: class UGameInstance;
  12: class UObject;
  13: struct FGameFeatureActivatingContext;
  14: struct FGameFeatureDeactivatingContext;
  15: struct FGameFeatureStateChangeContext;
  16: struct FWorldContext;
  21: UCLASS(Abstract)
  22: class UGameFeatureAction_WorldActionBase : public UGameFeatureAction
  23: {
  24: 	GENERATED_BODY()
  26: public:
  30: 	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
  33: 	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
  37: private:
  39: 	void HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext);
  42: 	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
  43: 		PURE_VIRTUAL(UGameFeatureAction_WorldActionBase::AddToWorld,);
  45: private:
  47: 	TMap<FGameFeatureStateChangeContext, FDelegateHandle> GameInstanceStartHandles;
  48: };
```

## HodgeGameFeaturePolicy.h

GameFeature 项目策略和资源加载观察扩展。

源码：[Source/Hodgepodge/Public/GameFeatures/HodgeGameFeaturePolicy.h](../../../Source/Hodgepodge/Public/GameFeatures/HodgeGameFeaturePolicy.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameFeatureStateChangeObserver.h"
   6: #include "GameFeaturesProjectPolicies.h"
   8: #include "HodgeGameFeaturePolicy.generated.h"
  10: class FName;
  11: class UGameFeatureData;
  12: struct FPrimaryAssetId;
  24: UCLASS(MinimalAPI, Config = Game)
  25: class UHodgeGameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
  26: {
  27:     GENERATED_BODY()
  29: public:
  31:     HODGEPODGE_API static UHodgeGameFeaturePolicy& Get();
  34:     UHodgeGameFeaturePolicy(const FObjectInitializer& ObjectInitializer);
  40:     virtual void InitGameFeatureManager() override;
  43:     virtual void ShutdownGameFeatureManager() override;
  47:     virtual TArray<FPrimaryAssetId> GetPreloadAssetListForGameFeature(
  48:         const UGameFeatureData* GameFeatureToLoad,
  49:         bool bIncludeLoadedAssets = false
  50:     ) const override;
  53:     virtual bool IsPluginAllowed(const FString& PluginURL) const override;
  56:     virtual const TArray<FName> GetPreloadBundleStateForGameFeature() const override;
  60:     virtual void GetGameFeatureLoadingMode(
  61:         bool& bLoadClientData,
  62:         bool& bLoadServerData
  63:     ) const override;
  67: private:
  72:     UPROPERTY(Transient)
  73:     TArray<TObjectPtr<UObject>> Observers;
  74: };
  79: UCLASS()
  80: class UHodgeGameFeature_HotfixManager
  81:     : public UObject
  82:     , public IGameFeatureStateChangeObserver
  83: {
  84:     GENERATED_BODY()
  86: public:
  89:     virtual void OnGameFeatureLoading(
  90:         const UGameFeatureData* GameFeatureData,
  91:         const FString& PluginURL
  92:     ) override;
  93: };
  98: UCLASS()
  99: class UHodgeGameFeature_AddGameplayCuePaths
 100:     : public UObject
 101:     , public IGameFeatureStateChangeObserver
 102: {
 103:     GENERATED_BODY()
 105: public:
 108:     virtual void OnGameFeatureRegistering(
 109:         const UGameFeatureData* GameFeatureData,
 110:         const FString& PluginName,
 111:         const FString& PluginURL
 112:     ) override;
 115:     virtual void OnGameFeatureUnregistering(
 116:         const UGameFeatureData* GameFeatureData,
 117:         const FString& PluginName,
 118:         const FString& PluginURL
 119:     ) override;
 120: };
```
