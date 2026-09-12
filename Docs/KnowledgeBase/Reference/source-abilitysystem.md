# AbilitySystem 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeGameplayAbility.cpp

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。

源码：[Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/Abilities/HodgeGameplayAbility.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[Interface/HodgeAbilitySourceInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h)

定义候选（多行签名仅展示首行）：

- L34: `UHodgeGameplayAbility::UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer)`
- L63: `UHodgeAbilitySystemComponent* UHodgeGameplayAbility::GetHodgeAbilitySystemComponentFromActorInfo() const`
- L72: `AHodgePlayerControllerBase* UHodgeGameplayAbility::GetHodgePlayerControllerFromActorInfo() const`
- L79: `AController* UHodgeGameplayAbility::GetControllerFromActorInfo() const`
- L117: `AHodgeCombatCharacter* UHodgeGameplayAbility::GetHodgeCharacterFromActorInfo() const`
- L129: `void UHodgeGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const`
- L182: `bool UHodgeGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,`
- L223: `void UHodgeGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)`
- L242: `void UHodgeGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)`
- L255: `void UHodgeGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,`
- L266: `void UHodgeGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,`
- L276: `void UHodgeGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,`
- L289: `bool UHodgeGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,`
- L319: `void UHodgeGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,`
- L402: `FGameplayEffectContextHandle UHodgeGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle,`
- L449: `void UHodgeGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,`
- L468: `bool UHodgeGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,`
- L624: `void UHodgeGameplayAbility::OnPawnAvatarSet()`
- L631: `void UHodgeGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle,`
- L657: `void UHodgeGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,`
- L699: `bool UHodgeGameplayAbility::CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const`
- L737: `bool UHodgeGameplayAbility::ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup)`
- L770: `void UHodgeGameplayAbility::SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode)`
- L784: `void UHodgeGameplayAbility::ClearCameraMode()`

## HodgeAttributeSet.cpp

项目 AttributeSet 基础和 ASC 访问。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeAttributeSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

定义候选（多行签名仅展示首行）：

- L18: `UHodgeAttributeSet::UHodgeAttributeSet()`
- L22: `UWorld* UHodgeAttributeSet::GetWorld() const`
- L29: `UHodgeAbilitySystemComponent* UHodgeAttributeSet::GetHodgeAbilitySystemComponent() const`

## HodgeHealthSet.cpp

Health、MaxHealth、BaseDamage、BaseHeal；复制与夹取有效，GE 结算主体注释。

源码：[Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/AttributeSet/HodgeHealthSet.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

定义候选（多行签名仅展示首行）：

- L12: `UHodgeHealthSet::UHodgeHealthSet() : Health(100.0f)`
- L22: `void UHodgeHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L32: `void UHodgeHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)`
- L54: `void UHodgeHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)`
- L64: `void UHodgeHealthSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)`
- L69: `void UHodgeHealthSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)`
- L74: `bool UHodgeHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)`
- L115: `void UHodgeHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)`
- L192: `void UHodgeHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const`
- L199: `void UHodgeHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)`
- L206: `void UHodgeHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)`
- L228: `void UHodgeHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const`

## GameplayTagStack.cpp

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

源码：[Source/Hodgepodge/Private/AbilitySystem/GameplayTagStack.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/GameplayTagStack.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

定义候选（多行签名仅展示首行）：

- L13: `FString FGameplayTagStack::GetDebugString() const`
- L22: `void FGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)`
- L69: `void FGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)`
- L124: `void FGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)`
- L138: `void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)`
- L152: `void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)`

## HodgeAbilitySystemComponent.cpp

Tag 输入缓存、激活组、关系映射、全局注册、失败通知与动态 Tag GE。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilitySystemComponent.cpp)

定义候选（多行签名仅展示首行）：


## HodgeAbilityTagRelationshipMapping.cpp

数据驱动的能力阻断、取消与激活条件关系。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeAbilityTagRelationshipMapping.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)

定义候选（多行签名仅展示首行）：

- L9: `void UHodgeAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(`
- L39: `void UHodgeAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(`
- L69: `bool UHodgeAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags,`

## HodgeGameplayCueManager.cpp

项目 Cue 资源加载/预加载管理实现；默认管理器配置尚未接通。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayCueManager.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeGameplayCueManager.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h)

定义候选（多行签名仅展示首行）：

- L68: `UHodgeGameplayCueManager::UHodgeGameplayCueManager(const FObjectInitializer& ObjectInitializer)`
- L74: `UHodgeGameplayCueManager* UHodgeGameplayCueManager::Get()`
- L80: `void UHodgeGameplayCueManager::OnCreated()`
- L90: `void UHodgeGameplayCueManager::LoadAlwaysLoadedCues()`
- L124: `bool UHodgeGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const`
- L153: `bool UHodgeGameplayCueManager::ShouldSyncLoadMissingGameplayCues() const`
- L160: `bool UHodgeGameplayCueManager::ShouldAsyncLoadMissingGameplayCues() const`
- L167: `void UHodgeGameplayCueManager::DumpGameplayCues(const TArray<FString>& Args)`
- L262: `void UHodgeGameplayCueManager::OnGameplayTagLoaded(const FGameplayTag& Tag)`
- L309: `void UHodgeGameplayCueManager::HandlePostGarbageCollect()`
- L322: `void UHodgeGameplayCueManager::ProcessLoadedTags()`
- L373: `void UHodgeGameplayCueManager::ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject)`
- L438: `void UHodgeGameplayCueManager::OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject,`
- L454: `void UHodgeGameplayCueManager::RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject)`
- L490: `void UHodgeGameplayCueManager::HandlePostLoadMap(UWorld* NewWorld)`
- L537: `void UHodgeGameplayCueManager::UpdateDelayLoadDelegateListeners()`
- L581: `bool UHodgeGameplayCueManager::ShouldDelayLoadGameplayCues() const`
- L600: `void UHodgeGameplayCueManager::RefreshGameplayCuePrimaryAsset()`

## HodgeGameplayEffectContext.cpp

项目 GE 上下文与序列化扩展；需要 Globals 分配入口配套。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayEffectContext.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)

定义候选（多行签名仅展示首行）：

- L18: `FHodgeGameplayEffectContext* FHodgeGameplayEffectContext::ExtractEffectContext(`
- L37: `bool FHodgeGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)`
- L61: `void FHodgeGameplayEffectContext::SetAbilitySource(const IHodgeAbilitySourceInterface* InObject, float InSourceLevel)`
- L71: `const IHodgeAbilitySourceInterface* FHodgeGameplayEffectContext::GetAbilitySource() const`
- L78: `const UPhysicalMaterial* FHodgeGameplayEffectContext::GetPhysicalMaterial() const`

## HodgeGameplayTags.cpp

原生 GameplayTag 注册和移动状态标签映射。标签存在不等于对应玩法实现。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：


## HodgeGlobalAbilitySystem.cpp

世界级全局能力/效果授予及 ASC 注册表。

源码：[Source/Hodgepodge/Private/AbilitySystem/HodgeGlobalAbilitySystem.cpp](../../../Source/Hodgepodge/Private/AbilitySystem/HodgeGlobalAbilitySystem.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeGlobalAbilitySystem.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

定义候选（多行签名仅展示首行）：

- L10: `void FGlobalAppliedAbilityList::AddToASC(TSubclassOf<UGameplayAbility> Ability, UHodgeAbilitySystemComponent* ASC)`
- L32: `void FGlobalAppliedAbilityList::RemoveFromASC(UHodgeAbilitySystemComponent* ASC)`
- L46: `void FGlobalAppliedAbilityList::RemoveFromAll()`
- L65: `void FGlobalAppliedEffectList::AddToASC(TSubclassOf<UGameplayEffect> Effect, UHodgeAbilitySystemComponent* ASC)`
- L85: `void FGlobalAppliedEffectList::RemoveFromASC(UHodgeAbilitySystemComponent* ASC)`
- L99: `void FGlobalAppliedEffectList::RemoveFromAll()`
- L116: `UHodgeGlobalAbilitySystem::UHodgeGlobalAbilitySystem()`
- L121: `void UHodgeGlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)`
- L138: `void UHodgeGlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)`
- L155: `void UHodgeGlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)`
- L172: `void UHodgeGlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)`
- L189: `void UHodgeGlobalAbilitySystem::RegisterASC(UHodgeAbilitySystemComponent* ASC)`
- L210: `void UHodgeGlobalAbilitySystem::UnregisterASC(UHodgeAbilitySystemComponent* ASC)`

## HodgeAbilityCost.h

自定义额外能力消耗的扩展契约。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeAbilityCost.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "GameplayAbilitySpec.h"
   7: #include "Abilities/GameplayAbility.h"
   8: #include "HodgeAbilityCost.generated.h"
  10: class UHodgeGameplayAbility;
  25: UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
  26: class HODGEPODGE_API UHodgeAbilityCost : public UObject
  27: {
  28: 	GENERATED_BODY()
  30: public:
  32: 	UHodgeAbilityCost()
  33: 	{
  34: 	}
  50: 	virtual bool CheckCost(const UHodgeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
  51: 	                       const FGameplayAbilityActorInfo* ActorInfo,
  52: 	                       FGameplayTagContainer* OptionalRelevantTags) const
  53: 	{
  55: 		return true;
  56: 	}
  71: 	virtual void ApplyCost(const UHodgeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
  72: 	                       const FGameplayAbilityActorInfo* ActorInfo,
  73: 	                       const FGameplayAbilityActivationInfo ActivationInfo)
  74: 	{
  76: 	}
  79: 	bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }
  81: protected:
  83: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
  84: 	bool bOnlyApplyCostOnHit = false;
  85: };
```

## HodgeGameplayAbility.h

项目技能基类：激活策略、互斥组、额外 Cost、失败与 EffectContext 扩展。

源码：[Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)

项目内直接 include（不是运行调用关系）：[Core/PlayerController/HodgePlayerControllerBase.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerControllerBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Abilities/GameplayAbility.h"
   7: #include "Core/PlayerController/HodgePlayerControllerBase.h"
   8: #include "HodgeGameplayAbility.generated.h"
  11: class UHodgeAbilityCost;
  12: class UHodgeCameraMode;
  13: class IHodgeAbilitySourceInterface;
  14: class AHodgeCombatCharacter;
  20: UENUM(BlueprintType)
  21: enum class EHodgeAbilityActivationPolicy : uint8
  22: {
  24: 	OnInputTriggered,
  27: 	WhileInputActive,
  30: 	OnSpawn
  31: };
  39: UENUM(BlueprintType)
  40: enum class EHodgeAbilityActivationGroup : uint8
  41: {
  43: 	Independent,
  46: 	Exclusive_Replaceable,
  49: 	Exclusive_Blocking,
  52: 	MAX UMETA(Hidden)
  53: };
  56: USTRUCT(BlueprintType)
  57: struct FHodgeAbilityMontageFailureMessage
  58: {
  59: 	GENERATED_BODY()
  61: public:
  63: 	UPROPERTY(BlueprintReadWrite)
  64: 	TObjectPtr<APlayerController> PlayerController = nullptr;
  67: 	UPROPERTY(BlueprintReadWrite)
  68: 	TObjectPtr<AActor> AvatarActor = nullptr;
  71: 	UPROPERTY(BlueprintReadWrite)
  72: 	FGameplayTagContainer FailureTags;
  75: 	UPROPERTY(BlueprintReadWrite)
  76: 	TObjectPtr<UAnimMontage> FailureMontage = nullptr;
  77: };
  91: UCLASS(Abstract, HideCategories = Input,
  92: 	Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
  93: class HODGEPODGE_API UHodgeGameplayAbility : public UGameplayAbility
  94: {
  95: 	GENERATED_BODY()
  98: 	friend class UHodgeAbilitySystemComponent;
 100: public:
 102: 	UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 105: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 106: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponentFromActorInfo() const;
 109: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 110: 	AHodgePlayerControllerBase* GetHodgePlayerControllerFromActorInfo() const;
 113: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 114: 	AController* GetControllerFromActorInfo() const;
 117: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 118: 	AHodgeCombatCharacter* GetHodgeCharacterFromActorInfo() const;
 121: 	EHodgeAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
 124: 	EHodgeAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }
 127: 	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
 131: 	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
 132: 		Meta = (ExpandBoolAsExecs = "ReturnValue"))
 133: 	bool CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const;
 137: 	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
 138: 		Meta = (ExpandBoolAsExecs = "ReturnValue"))
 139: 	bool ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup);
 142: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 143: 	void SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode);
 146: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
 147: 	void ClearCameraMode();
 150: 	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
 151: 	{
 153: 		NativeOnAbilityFailedToActivate(FailedReason);
 156: 		ScriptOnAbilityFailedToActivate(FailedReason);
 157: 	}
 159: protected:
 161: 	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
 164: 	UFUNCTION(BlueprintImplementableEvent)
 165: 	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
 169: 	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 170: 	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
 171: 	                                FGameplayTagContainer* OptionalRelevantTags) const override;
 174: 	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
 177: 	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
 180: 	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
 183: 	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 184: 	                             const FGameplayAbilityActivationInfo ActivationInfo,
 185: 	                             const FGameplayEventData* TriggerEventData) override;
 188: 	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 189: 	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
 190: 	                        bool bWasCancelled) override;
 193: 	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 194: 	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
 197: 	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 198: 	                       const FGameplayAbilityActivationInfo ActivationInfo) const override;
 201: 	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
 202: 	                                                       const FGameplayAbilityActorInfo* ActorInfo) const override;
 205: 	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
 206: 	                                                  FGameplayAbilitySpec* AbilitySpec) const override;
 209: 	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
 210: 	                                               const FGameplayTagContainer* SourceTags = nullptr,
 211: 	                                               const FGameplayTagContainer* TargetTags = nullptr,
 212: 	                                               OUT FGameplayTagContainer* OptionalRelevantTags = nullptr)
 213: 	const override;
 217: 	virtual void OnPawnAvatarSet();
 220: 	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
 221: 	                              float& OutSourceLevel, const IHodgeAbilitySourceInterface*& OutAbilitySource,
 222: 	                              AActor*& OutEffectCauser) const;
 226: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
 227: 	void K2_OnAbilityAdded();
 231: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
 232: 	void K2_OnAbilityRemoved();
 236: 	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
 237: 	void K2_OnPawnAvatarSet();
 239: protected:
 241: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Ability Activation")
 242: 	EHodgeAbilityActivationPolicy ActivationPolicy;
 245: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Ability Activation")
 246: 	EHodgeAbilityActivationGroup ActivationGroup;
 249: 	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
 250: 	TArray<TObjectPtr<UHodgeAbilityCost>> AdditionalCosts;
 253: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 254: 	TMap<FGameplayTag, FText> FailureTagToUserFacingMessages;
 257: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 258: 	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;
 261: 	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
 262: 	bool bLogCancelation;
 265: 	TSubclassOf<UHodgeCameraMode> ActiveCameraMode;
 266: };
```

## HodgeAttributeSet.h

项目 AttributeSet 基础和 ASC 访问。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   9: #pragma once
  11: #include "CoreMinimal.h"
  12: #include "AttributeSet.h"
  13: #include "HodgeAttributeSet.generated.h"
  58: #define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
  59: GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
  60: GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
  61: GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
  62: GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
  64: struct FGameplayEffectSpec;
  65: class UHodgeAbilitySystemComponent;
  75: DECLARE_MULTICAST_DELEGATE_SixParams(FHodgeAttributeEvent, AActor*                     , AActor*                 ,
  76:                                      const FGameplayEffectSpec*               , float                    ,
  77:                                      float             , float             );
  84: UCLASS()
  85: class HODGEPODGE_API UHodgeAttributeSet : public UAttributeSet
  86: {
  87: 	GENERATED_BODY()
  89: public:
  90: 	UHodgeAttributeSet();
  92: 	virtual UWorld* GetWorld() const override;
  94: 	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const;
  95: };
```

## HodgeHealthSet.h

Health、MaxHealth、BaseDamage、BaseHeal；复制与夹取有效，GE 结算主体注释。

源码：[Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

项目内直接 include（不是运行调用关系）：[AbilitySystem/AttributeSet/HodgeAttributeSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeAttributeSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "AbilitySystemComponent.h"
   7: #include "AbilitySystem/AttributeSet/HodgeAttributeSet.h"
   8: #include "HodgeHealthSet.generated.h"
  13: UCLASS()
  14: class HODGEPODGE_API UHodgeHealthSet : public UHodgeAttributeSet
  15: {
  16: 	GENERATED_BODY()
  18: public:
  19: 	UHodgeHealthSet();
  21: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Health);
  22: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, MaxHealth);
  23: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Healing);
  24: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, Damage);
  25: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, BaseDamage);
  26: 	ATTRIBUTE_ACCESSORS(UHodgeHealthSet, BaseHeal);
  29: 	mutable FHodgeAttributeEvent OnHealthChanged;
  32: 	mutable FHodgeAttributeEvent OnMaxHealthChanged;
  35: 	mutable FHodgeAttributeEvent OnOutOfHealth;
  37: protected:
  38: 	UFUNCTION()
  39: 	void OnRep_Health(const FGameplayAttributeData& OldValue);
  41: 	UFUNCTION()
  42: 	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
  44: 	UFUNCTION()
  45: 	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);
  47: 	UFUNCTION()
  48: 	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);
  50: 	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
  51: 	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
  53: 	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
  54: 	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
  55: 	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
  57: 	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
  59: private:
  61: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Hodge|Health",
  62: 		Meta = (HideFromModifiers, AllowPrivateAccess = true))
  63: 	FGameplayAttributeData Health;
  66: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Hodge|Health",
  67: 		Meta = (AllowPrivateAccess = true))
  68: 	FGameplayAttributeData MaxHealth;
  71: 	bool bOutOfHealth;
  74: 	float MaxHealthBeforeAttributeChange;
  75: 	float HealthBeforeAttributeChange;
  82: 	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(AllowPrivateAccess=true))
  83: 	FGameplayAttributeData Healing;
  86: 	UPROPERTY(BlueprintReadOnly, Category="Hodge|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
  87: 	FGameplayAttributeData Damage;
  90: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Hodge|Combat",
  91: 		Meta = (AllowPrivateAccess = true))
  92: 	FGameplayAttributeData BaseDamage;
  94: 	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Hodge|Combat",
  95: 		Meta = (AllowPrivateAccess = true))
  96: 	FGameplayAttributeData BaseHeal;
  97: };
```

## GameplayTagStack.h

带计数的标签栈及复制数据结构，区别于只判断有无的 TagContainer。

源码：[Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h](../../../Source/Hodgepodge/Public/AbilitySystem/GameplayTagStack.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameplayTagContainer.h"
   6: #include "Net/Serialization/FastArraySerializer.h"
   8: #include "GameplayTagStack.generated.h"
  11: struct FGameplayTagStackContainer;
  14: struct FNetDeltaSerializeInfo;
  20: USTRUCT(BlueprintType)
  21: struct FGameplayTagStack : public FFastArraySerializerItem
  22: {
  23: 	GENERATED_BODY()
  26: 	FGameplayTagStack()
  27: 	{
  28: 	}
  31: 	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
  32: 		: Tag(InTag)
  33: 		  , StackCount(InStackCount)
  34: 	{
  35: 	}
  38: 	FString GetDebugString() const;
  40: private:
  42: 	friend FGameplayTagStackContainer;
  45: 	UPROPERTY()
  46: 	FGameplayTag Tag;
  49: 	UPROPERTY()
  50: 	int32 StackCount = 0;
  51: };
  57: USTRUCT(BlueprintType)
  58: struct FGameplayTagStackContainer : public FFastArraySerializer
  59: {
  60: 	GENERATED_BODY()
  63: 	FGameplayTagStackContainer()
  65: 	{
  66: 	}
  68: public:
  70: 	void AddStack(FGameplayTag Tag, int32 StackCount);
  73: 	void RemoveStack(FGameplayTag Tag, int32 StackCount);
  76: 	int32 GetStackCount(FGameplayTag Tag) const
  77: 	{
  78: 		return TagToCountMap.FindRef(Tag);
  79: 	}
  82: 	bool ContainsTag(FGameplayTag Tag) const
  83: 	{
  84: 		return TagToCountMap.Contains(Tag);
  85: 	}
  90: 	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
  93: 	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
  96: 	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
 101: 	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
 102: 	{
 103: 		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(
 104: 			Stacks, DeltaParms, *this);
 105: 	}
 107: private:
 109: 	UPROPERTY()
 110: 	TArray<FGameplayTagStack> Stacks;
 113: 	TMap<FGameplayTag, int32> TagToCountMap;
 114: };
 117: template <>
 118: struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
 119: {
 120: 	enum
 121: 	{
 123: 		WithNetDeltaSerializer = true,
 124: 	};
 125: };
```

## HodgeAbilitySystemComponent.h

Tag 输入缓存、激活组、关系映射、全局注册、失败通知与动态 Tag GE。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: ��# p r a g m a   o n c e 
   2:  
   3:  
   4:  
   5:  # i n c l u d e   " C o r e M i n i m a l . h " 
   6:  
   7:  # i n c l u d e   " A b i l i t y S y s t e m C o m p o n e n t . h " 
   8:  
   9:  # i n c l u d e   " N a t i v e G a m e p l a y T a g s . h " 
  10:  
  11:  # i n c l u d e   " A b i l i t i e s / H o d g e G a m e p l a y A b i l i t y . h " 
  12:  
  13:  # i n c l u d e   " H o d g e A b i l i t y S y s t e m C o m p o n e n t . g e n e r a t e d . h " 
  14:  
  15:  
  16:  
  17:  / /   A b i l i t y   h~{sQ�| f\h�
  18: �(u�NƖ-N�{t  A b i l i t y   KN���v�o;m0;�^X�T�S�msQ�|0
  19:  
  20:  c l a s s   U H o d g e A b i l i t y T a g R e l a t i o n s h i p M a p p i n g ; 
  21:  
  22:  
  23:  
  24:  / /   h�:y  A b i l i t y   ��eQS_MR��;�^X�v  G a m e p l a y T a g 0
  25:  
  26:  H O D G E P O D G E _ A P I   U E _ D E C L A R E _ G A M E P L A Y _ T A G _ E X T E R N ( T A G _ G a m e p l a y _ A b i l i t y I n p u t B l o c k e d ) ; 
  27:  
  28:  
  29:  
  30:  / * * 
  31:  
  32:    *   U H o d g e A b i l i t y S y s t e m C o m p o n e n t 
  33:  
  34:    * 
  35:  
  36:    *   y��vꁚ[IN�v  A b i l i t y S y s t e m C o m p o n e n t   �W{|0
  37:  
  38:    * 
  39:  
  40:    *   (W  U A b i l i t y S y s t e m C o m p o n e n t   �v�W@x
  41: N;N��ibU\�
  42:  
  43:    *   A b i l i t y   ��eQYt0A c t i v a t i o n G r o u p   �{t0T a g R e l a t i o n s h i p 0A b i l i t y   �o;m1Y%���wI{y��v;���0
  44:  
  45:    * 
  46:  
  47:    *   U H o d g e A b i l i t y S y s t e m C o m p o n e n t 
  48:  
  49:    * 
  50:  
  51:    *     B a s e   a b i l i t y   s y s t e m   c o m p o n e n t   c l a s s   u s e d   b y   t h i s   p r o j e c t . 
  52:  
  53:    * / 
  54:  
  55:  U C L A S S ( ) 
  56:  
  57:  c l a s s   H O D G E P O D G E _ A P I   U H o d g e A b i l i t y S y s t e m C o m p o n e n t   :   p u b l i c   U A b i l i t y S y s t e m C o m p o n e n t 
  58:  
  59:  { 
  60:  
  61:  	 G E N E R A T E D _ B O D Y ( ) 
  62:  
  63:  
  64:  
  65:  p u b l i c : 
  66:  
  67:  	 / /   
  68: R�YSy��vꁚ[IN  A b i l i t y S y s t e m C o m p o n e n t 0
  69:  
  70:  	 U H o d g e A b i l i t y S y s t e m C o m p o n e n t ( c o n s t   F O b j e c t I n i t i a l i z e r &   O b j e c t I n i t i a l i z e r   =   F O b j e c t I n i t i a l i z e r : : G e t ( ) ) ; 
  71:  
  72:  
  73:  
  74:  	 / / ~ U A c t o r C o m p o n e n t   i n t e r f a c e 
  75:  
  76:  	 / /   A c t o r C o m p o n e n t   sS\�~_g�d>e�eۏL�nt0
  77:  
  78:  	 v i r t u a l   v o i d   E n d P l a y ( c o n s t   E E n d P l a y R e a s o n : : T y p e   E n d P l a y R e a s o n )   o v e r r i d e ; 
  79:  
  80:  	 / / ~ E n d   o f   U A c t o r C o m p o n e n t   i n t e r f a c e 
  81:  
  82:  
  83:  
  84:  	 / /   
  85: R�YS  A S C   N  O w n e r A c t o r 0A v a t a r A c t o r   KN���vsQ�|0
  86:  
  87:  	 v i r t u a l   v o i d   I n i t A b i l i t y A c t o r I n f o ( A A c t o r *   I n O w n e r A c t o r ,   A A c t o r *   I n A v a t a r A c t o r )   o v e r r i d e ; 
  88:  
  89:  
  90:  
  91:  	 / /   (u�N$R�e�g*N  A b i l i t y   /f&T�^勫��S�m�v�Qpe�[a�{|�W0
  92:  
  93:  	 t y p e d e f   T F u n c t i o n R e f < b o o l ( c o n s t   U H o d g e G a m e p l a y A b i l i t y *   H o d g e A b i l i t y ,   F G a m e p l a y A b i l i t y S p e c H a n d l e   H a n d l e ) > 
  94:  
  95:  	 T S h o u l d C a n c e l A b i l i t y F u n c ; 
  96:  
  97:  
  98:  
  99:  	 / /   9hncꁚ[INag�N�S�mS_MR  A S C   -N&{Tag�N�v  A b i l i t y 0
 100:  
 101:  	 v o i d   C a n c e l A b i l i t i e s B y F u n c ( T S h o u l d C a n c e l A b i l i t y F u n c   S h o u l d C a n c e l F u n c ,   b o o l   b R e p l i c a t e C a n c e l A b i l i t y ) ; 
 102:  
 103:  
 104:  
 105:  	 / /   �S�m@b	g1u��eQ�o;m�v  A b i l i t y 0
 106:  
 107:  	 v o i d   C a n c e l I n p u t A c t i v a t e d A b i l i t i e s ( b o o l   b R e p l i c a t e C a n c e l A b i l i t y ) ; 
 108:  
 109:  
 110:  
 111:  	 / /   ��U_�g*N  A b i l i t y   ��eQh~{��	c
 112: N0
 113:  
 114:  	 v o i d   A b i l i t y I n p u t T a g P r e s s e d ( c o n s t   F G a m e p l a y T a g &   I n p u t T a g ) ; 
 115:  
 116:  
 117:  
 118:  	 / /   ��U_�g*N  A b i l i t y   ��eQh~{��ʑ>e0
 119:  
 120:  	 v o i d   A b i l i t y I n p u t T a g R e l e a s e d ( c o n s t   F G a m e p l a y T a g &   I n p u t T a g ) ; 
 121:  
 122:  
 123:  
 124:  	 / /   �k'^Yt  A b i l i t y   ��eQ�r`v^
 125: \Ջ�o;m�[�^  A b i l i t y 0
 126:  
 127:  	 v o i d   P r o c e s s A b i l i t y I n p u t ( f l o a t   D e l t a T i m e ,   b o o l   b G a m e P a u s e d ) ; 
 128:  
 129:  
 130:  
 131:  	 / /   nzzS_MR'^�y/}�v  A b i l i t y   ��eQ�r`0
 132:  
 133:  	 v o i d   C l e a r A b i l i t y I n p u t ( ) ; 
 134:  
 135:  
 136:  
 137:  	 / /   $R�ec�[  A c t i v a t i o n G r o u p   S_MR/f&T��vQ�N  A b i l i t y   ;�^X0
 138:  
 139:  	 b o o l   I s A c t i v a t i o n G r o u p B l o c k e d ( E H o d g e A b i l i t y A c t i v a t i o n G r o u p   G r o u p )   c o n s t ; 
 140:  
 141:  
 142:  
 143:  	 / /   \  A b i l i t y   �ReQc�[�v  A c t i v a t i o n G r o u p 0
 144:  
 145:  	 v o i d   A d d A b i l i t y T o A c t i v a t i o n G r o u p ( E H o d g e A b i l i t y A c t i v a t i o n G r o u p   G r o u p ,   U H o d g e G a m e p l a y A b i l i t y *   H o d g e A b i l i t y ) ; 
 146:  
 147:  
 148:  
 149:  	 / /   \  A b i l i t y   �Nc�[�v  A c t i v a t i o n G r o u p   -N�yd�0
 150:  
 151:  	 v o i d   R e m o v e A b i l i t y F r o m A c t i v a t i o n G r o u p ( E H o d g e A b i l i t y A c t i v a t i o n G r o u p   G r o u p ,   U H o d g e G a m e p l a y A b i l i t y *   H o d g e A b i l i t y ) ; 
 152:  
 153:  
 154:  
 155:  	 / /   �S�mc�[  A c t i v a t i o n G r o u p   -N�v  A b i l i t y 
 156: �v^�S�N�_eu N*Nc�[  A b i l i t y 0
 157:  
 158:  	 v o i d   C a n c e l A c t i v a t i o n G r o u p A b i l i t i e s ( E H o d g e A b i l i t y A c t i v a t i o n G r o u p   G r o u p ,   U H o d g e G a m e p l a y A b i l i t y *   I g n o r e H o d g e A b i l i t y , 
 159:  
 160:  	                                                                         b o o l   b R e p l i c a t e C a n c e l A b i l i t y ) ; 
 161:  
 162:  
 163:  
 164:  	 / /   U s e s   a   g a m e p l a y   e f f e c t   t o   a d d   t h e   s p e c i f i e d   d y n a m i c   g r a n t e d   t a g . 
 165:  
 166:  	 / /   O(u  G a m e p l a y E f f e c t   �R`�c�Nc�[�v  G a m e p l a y T a g 0
 167:  
 168:  	 v o i d   A d d D y n a m i c T a g G a m e p l a y E f f e c t ( c o n s t   F G a m e p l a y T a g &   T a g ) ; 
 169:  
 170:  
 171:  
 172:  	 / /   R e m o v e s   a l l   a c t i v e   i n s t a n c e s   o f   t h e   g a m e p l a y   e f f e c t   t h a t   w a s   u s e d   t o   a d d   t h e   s p e c i f i e d   d y n a m i c   g r a n t e d   t a g . 
 173:  
 174:  	 / /   �yd�@b	g(u�N�c�Nc�[  G a m e p l a y T a g   �v�R`  G a m e p l a y E f f e c t   �[�O0
 175:  
 176:  	 v o i d   R e m o v e D y n a m i c T a g G a m e p l a y E f f e c t ( c o n s t   F G a m e p l a y T a g &   T a g ) ; 
 177:  
 178:  
 179:  
 180:  	 / * *   G e t s   t h e   a b i l i t y   t a r g e t   d a t a   a s s o c i a t e d   w i t h   t h e   g i v e n   a b i l i t y   h a n d l e   a n d   a c t i v a t i o n   i n f o   * / 
 181:  
 182:  	 / /   ���Sc�[  A b i l i t y H a n d l e   �T  A c t i v a t i o n I n f o   �[�^�v  T a r g e t D a t a 0
 183:  
 184:  	 v o i d   G e t A b i l i t y T a r g e t D a t a ( c o n s t   F G a m e p l a y A b i l i t y S p e c H a n d l e   A b i l i t y H a n d l e , 
 185:  
 186:  	                                                     F G a m e p l a y A b i l i t y A c t i v a t i o n I n f o   A c t i v a t i o n I n f o , 
 187:  
 188:  	                                                     F G a m e p l a y A b i l i t y T a r g e t D a t a H a n d l e &   O u t T a r g e t D a t a H a n d l e ) ; 
 189:  
 190:  
 191:  
 192:  	 / * *   S e t s   t h e   c u r r e n t   t a g   r e l a t i o n s h i p   m a p p i n g ,   i f   n u l l   i t   w i l l   c l e a r   i t   o u t   * / 
 193:  
 194:  	 / /   ��nS_MRO(u�v  A b i l i t y T a g R e l a t i o n s h i p M a p p i n g 
 195: � OeQ  n u l l p t r   �end�S_MR f\0
 196:  
 197:  	 v o i d   S e t T a g R e l a t i o n s h i p M a p p i n g ( U H o d g e A b i l i t y T a g R e l a t i o n s h i p M a p p i n g *   N e w M a p p i n g ) ; 
 198:  
 199:  
 200:  
 201:  	 / * *   L o o k s   a t   a b i l i t y   t a g s   a n d   g a t h e r s   a d d i t i o n a l   r e q u i r e d   a n d   b l o c k i n g   t a g s   * / 
 202:  
 203:  	 / /   9hnc  A b i l i t y   ꁫ��vh~{sQ�|
 204: ����S��Y�v  R e q u i r e d T a g s   �T  B l o c k e d T a g s 0
 205:  
 206:  	 v o i d   G e t A d d i t i o n a l A c t i v a t i o n T a g R e q u i r e m e n t s ( c o n s t   F G a m e p l a y T a g C o n t a i n e r &   A b i l i t y T a g s , 
 207:  
 208:  	                                                                                         F G a m e p l a y T a g C o n t a i n e r &   O u t A c t i v a t i o n R e q u i r e d , 
 209:  
 210:  	                                                                                         F G a m e p l a y T a g C o n t a i n e r &   O u t A c t i v a t i o n B l o c k e d )   c o n s t ; 
 211:  
 212:  
 213:  
 214:  p r o t e c t e d : 
 215:  
 216:  	 / /   �g~bv^
 217: \Ջ�o;m@b	gM�n:N  O n S p a w n   �v  A b i l i t y 0
 218:  
 219:  	 v o i d   T r y A c t i v a t e A b i l i t i e s O n S p a w n ( ) ; 
 220:  
 221:  
 222:  
 223:  	 / /   A b i l i t y   ��eQ	c
 224: N�e�f�e�[�^  A b i l i t y S p e c   �v��eQ�r`0
 225:  
 226:  	 v i r t u a l   v o i d   A b i l i t y S p e c I n p u t P r e s s e d ( F G a m e p l a y A b i l i t y S p e c &   S p e c )   o v e r r i d e ; 
 227:  
 228:  
 229:  
 230:  	 / /   A b i l i t y   ��eQʑ>e�e�f�e�[�^  A b i l i t y S p e c   �v��eQ�r`0
 231:  
 232:  	 v i r t u a l   v o i d   A b i l i t y S p e c I n p u t R e l e a s e d ( F G a m e p l a y A b i l i t y S p e c &   S p e c )   o v e r r i d e ; 
 233:  
 234:  
 235:  
 236:  	 / /   A b i l i t y   b�R�o;mT��w  A S C 0
 237:  
 238:  	 v i r t u a l   v o i d   N o t i f y A b i l i t y A c t i v a t e d ( c o n s t   F G a m e p l a y A b i l i t y S p e c H a n d l e   H a n d l e ,   U G a m e p l a y A b i l i t y *   A b i l i t y )   o v e r r i d e ; 
 239:  
 240:  
 241:  
 242:  	 / /   A b i l i t y   �o;m1Y%�T��w  A S C 0
 243:  
 244:  	 v i r t u a l   v o i d   N o t i f y A b i l i t y F a i l e d ( c o n s t   F G a m e p l a y A b i l i t y S p e c H a n d l e   H a n d l e ,   U G a m e p l a y A b i l i t y *   A b i l i t y , 
 245:  
 246:  	                                                                   c o n s t   F G a m e p l a y T a g C o n t a i n e r &   F a i l u r e R e a s o n )   o v e r r i d e ; 
 247:  
 248:  
 249:  
 250:  	 / /   A b i l i t y   �~_gT��w  A S C 
 251: �v^�c�O/f&T���S�m�v�Oo`0
 252:  
 253:  	 v i r t u a l   v o i d   N o t i f y A b i l i t y E n d e d ( F G a m e p l a y A b i l i t y S p e c H a n d l e   H a n d l e ,   U G a m e p l a y A b i l i t y *   A b i l i t y , 
 254:  
 255:  	                                                                 b o o l   b W a s C a n c e l l e d )   o v e r r i d e ; 
 256:  
 257:  
 258:  
 259:  	 / /   9hnc  A b i l i t y   �v  B l o c k   /   C a n c e l   T a g s 
 260: ��[vQ�N  A b i l i t y   gbL�;�^X�T�S�mYt0
 261:  
 262:  	 v i r t u a l   v o i d   A p p l y A b i l i t y B l o c k A n d C a n c e l T a g s ( c o n s t   F G a m e p l a y T a g C o n t a i n e r &   A b i l i t y T a g s , 
 263:  
 264:  	                                                                                         U G a m e p l a y A b i l i t y *   R e q u e s t i n g A b i l i t y ,   b o o l   b E n a b l e B l o c k T a g s , 
 265:  
 266:  	                                                                                         c o n s t   F G a m e p l a y T a g C o n t a i n e r &   B l o c k T a g s ,   b o o l   b E x e c u t e C a n c e l T a g s , 
 267:  
 268:  	                                                                                         c o n s t   F G a m e p l a y T a g C o n t a i n e r &   C a n c e l T a g s )   o v e r r i d e ; 
 269:  
 270:  
 271:  
 272:  	 / /   S_  A b i l i t y   �v�S�S�m�r`�Su�SS�e
 273: ���wvQ�N�vsQ  A b i l i t y 0
 274:  
 275:  	 v i r t u a l   v o i d   H a n d l e C h a n g e A b i l i t y C a n B e C a n c e l e d ( c o n s t   F G a m e p l a y T a g C o n t a i n e r &   A b i l i t y T a g s , 
 276:  
 277:  	                                                                                             U G a m e p l a y A b i l i t y *   R e q u e s t i n g A b i l i t y ,   b o o l   b C a n B e C a n c e l e d )   o v e r r i d e ; 
 278:  
 279:  
 280:  
 281:  	 / * *   N o t i f y   c l i e n t   t h a t   a n   a b i l i t y   f a i l e d   t o   a c t i v a t e   * / 
 282:  
 283:  	 / /   (W
 284: g�RhV�hKm0R  A b i l i t y   �o;m1Y%��e��w�[7b�z0
 285:  
 286:  	 U F U N C T I O N ( C l i e n t ,   U n r e l i a b l e ) 
 287:  
 288:  	 v o i d   C l i e n t N o t i f y A b i l i t y F a i l e d ( c o n s t   U G a m e p l a y A b i l i t y *   A b i l i t y ,   c o n s t   F G a m e p l a y T a g C o n t a i n e r &   F a i l u r e R e a s o n ) ; 
 289:  
 290:  
 291:  
 292:  	 / /   (W,g0W�~ NYt  A b i l i t y   �o;m1Y%�0
 293:  
 294:  	 v o i d   H a n d l e A b i l i t y F a i l e d ( c o n s t   U G a m e p l a y A b i l i t y *   A b i l i t y ,   c o n s t   F G a m e p l a y T a g C o n t a i n e r &   F a i l u r e R e a s o n ) ; 
 295:  
 296:  
 297:  
 298:  p r o t e c t e d : 
 299:  
 300:  	 / /   S_MR  A S C   O(u�v  A b i l i t y T a g R e l a t i o n s h i p M a p p i n g 0
 301:  
 302:  	 / /   (u�N9hnc  A b i l i t y T a g s   �R`ibU\  R e q u i r e d T a g s 0B l o c k e d T a g s 0C a n c e l T a g s   �T  B l o c k T a g s 0
 303:  
 304:  	 / /   I f   s e t ,   t h i s   t a b l e   i s   u s e d   t o   l o o k   u p   t a g   r e l a t i o n s h i p s   f o r   a c t i v a t e   a n d   c a n c e l 
 305:  
 306:  	 U P R O P E R T Y ( ) 
 307:  
 308:  	 T O b j e c t P t r < U H o d g e A b i l i t y T a g R e l a t i o n s h i p M a p p i n g >   T a g R e l a t i o n s h i p M a p p i n g ; 
 309:  
 310:  
 311:  
 312:  	 / /   S_MR'^�Su��eQ	c
 313: N�v  A b i l i t y S p e c H a n d l e 0
 314:  
 315:  	 / /   H a n d l e s   t o   a b i l i t i e s   t h a t   h a d   t h e i r   i n p u t   p r e s s e d   t h i s   f r a m e . 
 316:  
 317:  	 T A r r a y < F G a m e p l a y A b i l i t y S p e c H a n d l e >   I n p u t P r e s s e d S p e c H a n d l e s ; 
 318:  
 319:  
 320:  
 321:  	 / /   S_MR'^�Su��eQʑ>e�v  A b i l i t y S p e c H a n d l e 0
 322:  
 323:  	 / /   H a n d l e s   t o   a b i l i t i e s   t h a t   h a d   t h e i r   i n p u t   r e l e a s e d   t h i s   f r a m e . 
 324:  
 325:  	 T A r r a y < F G a m e p l a y A b i l i t y S p e c H a n d l e >   I n p u t R e l e a s e d S p e c H a n d l e s ; 
 326:  
 327:  
 328:  
 329:  	 / /   S_MR�N6qY�N��eQ�Oc�r`�v  A b i l i t y S p e c H a n d l e 0
 330:  
 331:  	 / /   H a n d l e s   t o   a b i l i t i e s   t h a t   h a v e   t h e i r   i n p u t   h e l d . 
 332:  
 333:  	 T A r r a y < F G a m e p l a y A b i l i t y S p e c H a n d l e >   I n p u t H e l d S p e c H a n d l e s ; 
 334:  
 335:  
 336:  
 337:  	 / /   �k*N  A c t i v a t i o n G r o u p   S_MRck(WЏL��v  A b i l i t y   peϑ0
 338:  
 339:  	 / /   N u m b e r   o f   a b i l i t i e s   r u n n i n g   i n   e a c h   a c t i v a t i o n   g r o u p . 
 340:  
 341:  	 i n t 3 2   A c t i v a t i o n G r o u p C o u n t s [ ( u i n t 8 ) E H o d g e A b i l i t y A c t i v a t i o n G r o u p : : M A X ] ; 
 342:  
 343:  } ; 
 344:  
 345:  
```

## HodgeAbilityTagRelationshipMapping.h

数据驱动的能力阻断、取消与激活条件关系。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilityTagRelationshipMapping.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "Engine/DataAsset.h"
   6: #include "GameplayTagContainer.h"
   8: #include "HodgeAbilityTagRelationshipMapping.generated.h"
  10: class UObject;
  18: USTRUCT()
  19: struct FHodgeAbilityTagRelationship
  20: {
  21: 	GENERATED_BODY()
  24: 	UPROPERTY(EditAnywhere, Category = Ability, meta = (Categories = "Gameplay.Action"))
  25: 	FGameplayTag AbilityTag;
  28: 	UPROPERTY(EditAnywhere, Category = Ability)
  29: 	FGameplayTagContainer AbilityTagsToBlock;
  32: 	UPROPERTY(EditAnywhere, Category = Ability)
  33: 	FGameplayTagContainer AbilityTagsToCancel;
  36: 	UPROPERTY(EditAnywhere, Category = Ability)
  37: 	FGameplayTagContainer ActivationRequiredTags;
  40: 	UPROPERTY(EditAnywhere, Category = Ability)
  41: 	FGameplayTagContainer ActivationBlockedTags;
  42: };
  51: UCLASS()
  52: class UHodgeAbilityTagRelationshipMapping : public UDataAsset
  53: {
  54: 	GENERATED_BODY()
  56: private:
  58: 	UPROPERTY(EditAnywhere, Category = Ability, meta=(TitleProperty="AbilityTag"))
  59: 	TArray<FHodgeAbilityTagRelationship> AbilityTagRelationships;
  61: public:
  63: 	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock,
  64: 	                                    FGameplayTagContainer* OutTagsToCancel) const;
  67: 	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
  68: 	                                         FGameplayTagContainer* OutActivationRequired,
  69: 	                                         FGameplayTagContainer* OutActivationBlocked) const;
  72: 	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
  73: };
```

## HodgeGameplayCueManager.h

项目 Cue 资源加载/预加载管理实现；默认管理器配置尚未接通。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayCueManager.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameplayCueManager.h"
   7: #include "HodgeGameplayCueManager.generated.h"
   9: class FString;
  10: class UClass;
  11: class UObject;
  12: class UWorld;
  13: struct FObjectKey;
  21: UCLASS()
  22: class UHodgeGameplayCueManager : public UGameplayCueManager
  23: {
  24: 	GENERATED_BODY()
  26: public:
  28: 	UHodgeGameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  31: 	static UHodgeGameplayCueManager* Get();
  36: 	virtual void OnCreated() override;
  39: 	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;
  42: 	virtual bool ShouldSyncLoadMissingGameplayCues() const override;
  45: 	virtual bool ShouldAsyncLoadMissingGameplayCues() const override;
  50: 	static void DumpGameplayCues(const TArray<FString>& Args);
  53: 	void LoadAlwaysLoadedCues();
  56: 	void RefreshGameplayCuePrimaryAsset();
  58: private:
  60: 	void OnGameplayTagLoaded(const FGameplayTag& Tag);
  63: 	void HandlePostGarbageCollect();
  66: 	void ProcessLoadedTags();
  69: 	void ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject);
  72: 	void OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject, bool bAlwaysLoadedCue);
  75: 	void RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject);
  78: 	void HandlePostLoadMap(UWorld* NewWorld);
  81: 	void UpdateDelayLoadDelegateListeners();
  84: 	bool ShouldDelayLoadGameplayCues() const;
  86: private:
  88: 	struct FLoadedGameplayTagToProcessData
  89: 	{
  91: 		FGameplayTag Tag;
  94: 		TWeakObjectPtr<UObject> WeakOwner;
  97: 		FLoadedGameplayTagToProcessData()
  98: 		{
  99: 		}
 102: 		FLoadedGameplayTagToProcessData(const FGameplayTag& InTag,
 103: 		                                const TWeakObjectPtr<UObject>& InWeakOwner) : Tag(InTag), WeakOwner(InWeakOwner)
 104: 		{
 105: 		}
 106: 	};
 108: private:
 110: 	UPROPERTY(transient)
 111: 	TSet<TObjectPtr<UClass>> PreloadedCues;
 114: 	TMap<FObjectKey, TSet<FObjectKey>> PreloadedCueReferencers;
 118: 	UPROPERTY(transient)
 119: 	TSet<TObjectPtr<UClass>> AlwaysLoadedCues;
 122: 	TArray<FLoadedGameplayTagToProcessData> LoadedGameplayTagsToProcess;
 125: 	FCriticalSection LoadedGameplayTagsToProcessCS;
 128: 	bool bProcessLoadedTagsAfterGC = false;
 129: };
```

## HodgeGameplayEffectContext.h

项目 GE 上下文与序列化扩展；需要 Globals 分配入口配套。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayEffectContext.h)

项目内直接 include（不是运行调用关系）：[Interface/HodgeAbilitySourceInterface.h](../../../Source/Hodgepodge/Public/Interface/HodgeAbilitySourceInterface.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "GameplayEffectTypes.h"
   6: #include "Interface/HodgeAbilitySourceInterface.h"
   8: #include "HodgeGameplayEffectContext.generated.h"
  10: class AActor;
  11: class FArchive;
  13: class UObject;
  14: class UPhysicalMaterial;
  26: USTRUCT()
  27: struct FHodgeGameplayEffectContext : public FGameplayEffectContext
  28: {
  29: 	GENERATED_BODY()
  32: 	FHodgeGameplayEffectContext()
  33: 		: FGameplayEffectContext()
  34: 	{
  35: 	}
  38: 	FHodgeGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
  39: 		: FGameplayEffectContext(InInstigator, InEffectCauser)
  40: 	{
  41: 	}
  44: 	static HODGEPODGE_API FHodgeGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);
  47: 	void SetAbilitySource(const IHodgeAbilitySourceInterface* InObject, float InSourceLevel);
  50: 	const IHodgeAbilitySourceInterface* GetAbilitySource() const;
  53: 	virtual FGameplayEffectContext* Duplicate() const override
  54: 	{
  55: 		FHodgeGameplayEffectContext* NewContext = new FHodgeGameplayEffectContext();
  56: 		*NewContext = *this;
  57: 		if (GetHitResult())
  58: 		{
  60: 			NewContext->AddHitResult(*GetHitResult(), true);
  61: 		}
  62: 		return NewContext;
  63: 	}
  66: 	virtual UScriptStruct* GetScriptStruct() const override
  67: 	{
  68: 		return FHodgeGameplayEffectContext::StaticStruct();
  69: 	}
  72: 	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
  75: 	const UPhysicalMaterial* GetPhysicalMaterial() const;
  77: public:
  79: 	UPROPERTY()
  80: 	int32 CartridgeID = -1;
  82: protected:
  85: 	UPROPERTY()
  86: 	TWeakObjectPtr<const UObject> AbilitySourceObject;
  87: };
  90: template <>
  91: struct TStructOpsTypeTraits<FHodgeGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FHodgeGameplayEffectContext>
  92: {
  93: 	enum
  94: 	{
  96: 		WithNetSerializer = true,
  99: 		WithCopy = true
 100: 	};
 101: };
```

## HodgeGameplayTags.h

原生 GameplayTag 注册和移动状态标签映射。标签存在不等于对应玩法实现。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "NativeGameplayTags.h"
   5: namespace HodgeGameplayTags
   6: {
   7: 	HODGEPODGE_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
  12: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
  13: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
  14: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
  15: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
  16: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
  17: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
  18: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);
  23: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);
  28: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dash_Duration_Message);
  29: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Grenade_Duration_Message);
  30: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interaction_Activate);
  31: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Respawn_Completed_Message);
  32: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Respawn_Duration_Message);
  37: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type);
  38: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action);
  39: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_ADS);
  40: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Dash);
  41: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Drop);
  42: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Emote);
  43: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Grenade);
  44: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Jump);
  45: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Melee);
  46: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Reload);
  47: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_WeaponFire);
  48: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Info);
  49: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Info_ShowLeaderboard);
  50: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive);
  51: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_AutoReload);
  52: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_AutoRespawn);
  53: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive_ChangeQuickbarSlot);
  54: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange);
  55: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange_Death);
  56: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_StatusChange_Spawning);
  61: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic);
  62: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_AnimationStyle_Feminine);
  63: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_AnimationStyle_Masculine);
  64: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cosmetic_BodyStyle_Medium);
  69: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_Zone_WeakSpot);
  74: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_DamageTaken);
  75: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Dash);
  76: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Dash_Cooldown);
  77: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Death);
  78: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Heal);
  79: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Melee_Cooldown);
  84: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_Burst);
  85: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_BurstLatent);
  86: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Test_Looping);
  91: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Grenade_Cooldown);
  92: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Grenade_Detonate);
  93: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Melee_Hit);
  94: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Melee_Impact);
  95: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Pistol_Fire);
  96: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Rifle_Fire);
  97: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Rifle_Impact);
  98: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Shotgun_Fire);
 103: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_World_Launcher_Activate);
 104: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_World_Teleporter_Activate);
 109: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageTrait_Instant);
 110: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageTrait_Periodic);
 111: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Basic);
 112: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Grenade);
 113: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Melee);
 114: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Pistol);
 115: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Rifle);
 116: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_DamageType_Shotgun);
 117: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_Instant);
 118: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_Periodic);
 123: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_MeleeHit);
 124: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
 125: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
 126: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);
 131: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameSettings_Action_EditSafeZone);
 136: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_ExtraEquipment);
 137: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_InfrequentAbilities);
 138: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_LeftSideTouchInputs);
 139: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_LeftSideTouchRegion);
 140: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RespawnTimer);
 141: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RightSideTouchInputs);
 142: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HUD_Slot_RightSideTouchRegion);
 147: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Dash);
 148: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Heal);
 149: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Melee);
 150: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Quickslot_Drop);
 151: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);
 156: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_ADS);
 157: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Fire);
 158: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_FireAuto);
 159: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Grenade);
 160: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Reload);
 165: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
 166: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
 167: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
 168: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);
 176: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint);
 177: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Walk);
 178: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Aim);
 179: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ragdoll);
 180: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Roll);
 181: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_RotationMode);
 182: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_ViewMode);
 183: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SwitchShoulder);
 188: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
 189: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
 190: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
 191: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);
 196: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lyra_Damage_Taken_Message);
 197: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lyra_HUD_PlayerHUD);
 198: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lyra_HUD_TempTopWidgets);
 199: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lyra_Player);
 204: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_BinauralSettingControlledByOS);
 205: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_CanExitApplication);
 206: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_PrimarlyController);
 207: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_HasStrictControllerPairing);
 208: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_PrimarlyTouchScreen);
 209: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_SupportsMouseAndKeyboard);
 210: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_Input_HardwareCursor);
 211: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsBackgroundAudio);
 212: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsChangingAudioOutputDevice);
 213: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Platform_Trait_SupportsWindowedMode);
 218: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShooterGame_GamePhase_MatchBeginCountdown);
 223: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_SpawningIn);
 224: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
 225: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
 226: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
 227: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
 228: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);
 233: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
 234: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);
 239: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
 240: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);
 245: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Back);
 246: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Game);
 247: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_GameMenu);
 248: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Menu);
 249: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Modal);
 254: 	HODGEPODGE_API extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
 255: 	HODGEPODGE_API extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;
 257: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
 258: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
 259: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
 260: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
 261: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);
 262: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
 263: };
```

## HodgeGlobalAbilitySystem.h

世界级全局能力/效果授予及 ASC 注册表。

源码：[Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGlobalAbilitySystem.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "ActiveGameplayEffectHandle.h"
   6: #include "Subsystems/WorldSubsystem.h"
   7: #include "GameplayAbilitySpecHandle.h"
   8: #include "Templates/SubclassOf.h"
  10: #include "HodgeGlobalAbilitySystem.generated.h"
  12: class UGameplayAbility;
  13: class UGameplayEffect;
  14: class UHodgeAbilitySystemComponent;
  15: class UObject;
  16: struct FActiveGameplayEffectHandle;
  17: struct FFrame;
  18: struct FGameplayAbilitySpecHandle;
  24: USTRUCT()
  25: struct FGlobalAppliedAbilityList
  26: {
  27: 	GENERATED_BODY()
  30: 	UPROPERTY()
  31: 	TMap<TObjectPtr<UHodgeAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;
  34: 	void AddToASC(TSubclassOf<UGameplayAbility> Ability, UHodgeAbilitySystemComponent* ASC);
  37: 	void RemoveFromASC(UHodgeAbilitySystemComponent* ASC);
  40: 	void RemoveFromAll();
  41: };
  47: USTRUCT()
  48: struct FGlobalAppliedEffectList
  49: {
  50: 	GENERATED_BODY()
  53: 	UPROPERTY()
  54: 	TMap<TObjectPtr<UHodgeAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;
  57: 	void AddToASC(TSubclassOf<UGameplayEffect> Effect, UHodgeAbilitySystemComponent* ASC);
  60: 	void RemoveFromASC(UHodgeAbilitySystemComponent* ASC);
  63: 	void RemoveFromAll();
  64: };
  72: UCLASS()
  73: class UHodgeGlobalAbilitySystem : public UWorldSubsystem
  74: {
  75: 	GENERATED_BODY()
  77: public:
  78: 	UHodgeGlobalAbilitySystem();
  81: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
  82: 	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);
  85: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
  86: 	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);
  89: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
  90: 	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);
  93: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge")
  94: 	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);
  97: 	void RegisterASC(UHodgeAbilitySystemComponent* ASC);
 100: 	void UnregisterASC(UHodgeAbilitySystemComponent* ASC);
 102: private:
 104: 	UPROPERTY()
 105: 	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;
 108: 	UPROPERTY()
 109: 	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;
 112: 	UPROPERTY()
 113: 	TArray<TObjectPtr<UHodgeAbilitySystemComponent>> RegisteredASCs;
 114: };
```
