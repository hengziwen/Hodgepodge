# Equipment 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeEquipmentDefinition.cpp

装备实例类、Actor 挂接与 AbilitySets 配置。

源码：[Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)

定义候选（多行签名仅展示首行）：

- L14: `UHodgeEquipmentDefinition::UHodgeEquipmentDefinition(const FObjectInitializer& ObjectInitializer)`

## HodgeEquipmentInstance.cpp

Pawn 所属复制 UObject、SpawnedActors RepNotify 和装备/卸装生命周期。

源码：[Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Data/HodgeEquipmentStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)

定义候选（多行签名仅展示首行）：

- L35: `UHodgeEquipmentInstance::UHodgeEquipmentInstance(const FObjectInitializer& ObjectInitializer)`
- L41: `UWorld* UHodgeEquipmentInstance::GetWorld() const`
- L57: `void UHodgeEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L75: `void UHodgeEquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,`
- L89: `APawn* UHodgeEquipmentInstance::GetPawn() const`
- L95: `APawn* UHodgeEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const`
- L116: `void UHodgeEquipmentInstance::SpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn)`
- L122: `bool UHodgeEquipmentInstance::TrySpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn)`
- L168: `void UHodgeEquipmentInstance::DestroyEquipmentActors()`
- L184: `void UHodgeEquipmentInstance::OnRep_SpawnedActors()`
- L190: `void UHodgeEquipmentInstance::OnEquipped()`
- L197: `void UHodgeEquipmentInstance::OnUnequipped()`
- L204: `void UHodgeEquipmentInstance::OnRep_Instigator()`
- L209: `void UHodgeEquipmentInstance::SetStatIdentity(FGuid Id, int32 Level, const UHodgeEquipmentStatProfile* Profile)`

## HodgeEquipmentManagerComponent.cpp

Experience 注入 Pawn，ASC 就绪后装备默认剑，来源授予句柄精确撤销与客户端迟到绑定。

源码：[Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Data/HodgeEquipmentStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

定义候选（多行签名仅展示首行）：

- L63: `FString FHodgeAppliedEquipmentEntry::GetDebugString() const`
- L72: `void FHodgeEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)`
- L89: `void FHodgeEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)`
- L106: `void FHodgeEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)`
- L121: `UHodgeAbilitySystemComponent* FHodgeEquipmentList::GetAbilitySystemComponent() const`
- L134: `UHodgeEquipmentInstance* FHodgeEquipmentList::AddEntry(TSubclassOf<UHodgeEquipmentDefinition> Definition, FGuid Id, int32 Level)`
- L195: `void FHodgeEquipmentList::RemoveEntry(UHodgeEquipmentInstance* Instance)`
- L231: `UHodgeEquipmentManagerComponent::UHodgeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)`
- L243: `void UHodgeEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L253: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::EquipItem(TSubclassOf<UHodgeEquipmentDefinition> EquipmentClass)`
- L262: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::EquipItemWithState(TSubclassOf<UHodgeEquipmentDefinition> EquipmentClass, FGuid Id, int32 Level)`
- L310: `void UHodgeEquipmentManagerComponent::UnequipItem(UHodgeEquipmentInstance* ItemInstance)`
- L344: `bool UHodgeEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch,`
- L369: `void UHodgeEquipmentManagerComponent::InitializeComponent()`
- L386: `void UHodgeEquipmentManagerComponent::UninitializeComponent()`
- L411: `void UHodgeEquipmentManagerComponent::ReadyForReplication()`
- L438: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::GetFirstInstanceOfType(`
- L461: `TArray<UHodgeEquipmentInstance*> UHodgeEquipmentManagerComponent::GetEquipmentInstancesOfType(`
- L487: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::FindInstanceOfDefinition(TSubclassOf<UHodgeEquipmentDefinition> Definition) const`
- L494: `bool UHodgeEquipmentManagerComponent::SetEquipmentLevel(UHodgeEquipmentInstance* Instance, int32 Level)`
- L549: `bool UHodgeEquipmentManagerComponent::ValidateEquipmentForGrant(TSubclassOf<UHodgeEquipmentDefinition> Definition,`
- L577: `bool UHodgeEquipmentManagerComponent::IsEquipmentReady(const UHodgeEquipmentInstance* Instance) const`

## HodgeWeaponInstance.cpp

手持请求、显现阶段、计时器、复制/拥有者预测及拒绝恢复，不新增角色常驻武器表现组件。

源码：[Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Equipment/HodgeWeaponPresentationActor.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h)、[Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L46: `UHodgeWeaponInstance::UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer)`
- L71: `void UHodgeWeaponInstance::OnEquipped()`
- L93: `void UHodgeWeaponInstance::OnUnequipped()`
- L104: `void UHodgeWeaponInstance::UpdateFiringTime()`
- L117: `float UHodgeWeaponInstance::GetTimeSinceLastInteractedWith() const`
- L146: `TSubclassOf<UAnimInstance> UHodgeWeaponInstance::PickBestAnimLayer(bool bEquipped,`
- L157: `const FPlatformUserId UHodgeWeaponInstance::GetOwningUserId() const`
- L171: `void UHodgeWeaponInstance::ApplyDeviceProperties()`
- L209: `void UHodgeWeaponInstance::RemoveDeviceProperties()`
- L232: `void UHodgeWeaponInstance::OnDeathStarted(AActor* OwningActor)`
- L248: `void UHodgeWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L255: `double UHodgeWeaponInstance::GetPresentationTime() const`
- L264: `bool UHodgeWeaponInstance::CanDrivePresentation() const`
- L274: `UHodgeWeaponInstance* UHodgeWeaponInstance::ResolvePresentationWeapon(APawn* Pawn, UObject* SourceObject)`
- L302: `void UHodgeWeaponInstance::InitializePresentation()`
- L325: `void UHodgeWeaponInstance::ClearPresentationTimer()`
- L331: `void UHodgeWeaponInstance::ShutdownPresentation()`
- L348: `FGuid UHodgeWeaponInstance::AcquireHandUse(FGuid ExecutionId, int32 OccurrenceId, int32 ActivationKey, EHodgeWeaponHandEndBehavior EndBehavior)`
- L381: `void UHodgeWeaponInstance::ReleaseHandUse(FGuid Handle)`
- L390: `void UHodgeWeaponInstance::ReleaseHandUsesForExecution(const FGuid& ExecutionId)`
- L399: `void UHodgeWeaponInstance::BeginIdlePresentation(EHodgeWeaponHandEndBehavior EndBehavior)`
- L410: `void UHodgeWeaponInstance::SchedulePresentationPhase(EHodgeWeaponPresentationPhase Phase, float Seconds, EHodgeWeaponHandEndBehavior EndBehavior)`
- L429: `void UHodgeWeaponInstance::SetPresentationPhase(EHodgeWeaponPresentationPhase Phase, int32 ActivationKey, EHodgeWeaponHandEndBehavior EndBehavior)`
- L485: `FHodgeWeaponPresentationState UHodgeWeaponInstance::GetPresentationState() const`
- L494: `void UHodgeWeaponInstance::OnRep_PresentationState()`
- L516: `void UHodgeWeaponInstance::RejectPredictedHandUse(int32 ActivationKey)`
- L536: `void UHodgeWeaponInstance::RefreshPresentationActors()`
- L546: `void UHodgeWeaponInstance::OnSpawnedActorsChanged()`
- L553: `void UHodgeWeaponInstance::BeginDestroy()`
- L561: `void UHodgeWeaponInstance::UpdateOwnerPosePolicy()`

## HodgeWeaponPresentationActor.cpp

检测 Mesh 留手，可见 Mesh 回背/悬浮/消隐挂 BackSocket，手持时挂回检测 Mesh。

源码：[Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationActor.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationActor.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponPresentationActor.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L18: `AHodgeWeaponPresentationActor::AHodgeWeaponPresentationActor()`
- L41: `void AHodgeWeaponPresentationActor::BeginPlay()`
- L51: `void AHodgeWeaponPresentationActor::TryBindWeapon()`
- L65: `void AHodgeWeaponPresentationActor::ConfigureProfile(const UHodgeWeaponPresentationProfile* Profile)`
- L83: `void AHodgeWeaponPresentationActor::BindWeapon(UHodgeWeaponInstance* Instance)`
- L108: `void AHodgeWeaponPresentationActor::RefreshPresentation()`
- L118: `FTransform AHodgeWeaponPresentationActor::GetVisualTransform() const`
- L124: `void AHodgeWeaponPresentationActor::EvaluatePresentation()`
- L200: `void AHodgeWeaponPresentationActor::Tick(float DeltaSeconds)`
- L207: `void AHodgeWeaponPresentationActor::EndPlay(const EEndPlayReason::Type Reason)`

## HodgeWeaponPresentationProfile.cpp

模型/材质、手背插槽/偏移、曲线/时间；BackSocket 默认 WeaponOnBack，BackTransform 为插槽内偏移。

源码：[Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationProfile.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationProfile.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)

定义候选（多行签名仅展示首行）：

- L12: `bool UHodgeWeaponPresentationProfile::Validate(TArray<FText>& Errors) const`
- L30: `EDataValidationResult UHodgeWeaponPresentationProfile::IsDataValid(FDataValidationContext& Context) const`

## HodgeEquipmentDefinition.h

装备实例类、Actor 挂接与 AbilitySets 配置。

源码：[Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "Templates/SubclassOf.h"
  11: #include "HodgeEquipmentDefinition.generated.h"
  13: class AActor;
  14: class UHodgeAbilitySet;
  15: class UHodgeEquipmentInstance;
  16: class UHodgeEquipmentStatProfile;
  22: USTRUCT()
  23: struct FHodgeEquipmentActorToSpawn
  24: {
  25: 	GENERATED_BODY()
  29: 	FHodgeEquipmentActorToSpawn()
  30: 	{
  31: 	}
  34: 	UPROPERTY(EditAnywhere, Category=Equipment)
  35: 	TSubclassOf<AActor> ActorToSpawn;
  38: 	UPROPERTY(EditAnywhere, Category=Equipment)
  39: 	FName AttachSocket;
  42: 	UPROPERTY(EditAnywhere, Category=Equipment)
  43: 	FTransform AttachTransform;
  44: };
  55: UCLASS(Blueprintable, Const, Abstract, BlueprintType)
  56: class UHodgeEquipmentDefinition : public UObject
  57: {
  58: 	GENERATED_BODY()
  60: public:
  65: 	UHodgeEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  70: 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
  71: 	TSubclassOf<UHodgeEquipmentInstance> InstanceType;
  76: 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
  77: 	TArray<TObjectPtr<const UHodgeAbilitySet>> AbilitySetsToGrant;
  79: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Attributes")
  80: 	TObjectPtr<UHodgeEquipmentStatProfile> StatProfile;
  85: 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
  86: 	TArray<FHodgeEquipmentActorToSpawn> ActorsToSpawn;
  87: };
```

## HodgeEquipmentInstance.h

Pawn 所属复制 UObject、SpawnedActors RepNotify 和装备/卸装生命周期。

源码：[Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "Engine/World.h"
  11: #include "HodgeEquipmentInstance.generated.h"
  13: class AActor;
  14: class APawn;
  15: struct FFrame;
  16: struct FHodgeEquipmentActorToSpawn;
  17: class UHodgeEquipmentStatProfile;
  27: UCLASS(BlueprintType, Blueprintable)
  28: class UHodgeEquipmentInstance : public UObject
  29: {
  30: 	GENERATED_BODY()
  32: public:
  37: 	UHodgeEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  45: 	virtual bool IsSupportedForNetworking() const override { return true; }
  51: 	virtual UWorld* GetWorld() const override final;
  59: 	UFUNCTION(BlueprintPure, Category=Equipment)
  60: 	UObject* GetInstigator() const { return Instigator; }
  66: 	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }
  72: 	UFUNCTION(BlueprintPure, Category=Equipment)
  73: 	APawn* GetPawn() const;
  77: 	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") int32 GetEquipmentLevel() const { return EquipmentLevel; }
  81: 	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") FGuid GetEquipmentId() const { return EquipmentId; }
  87: 	void SetStatIdentity(FGuid Id, int32 Level, const UHodgeEquipmentStatProfile* Profile);
  91: 	const UHodgeEquipmentStatProfile* GetStatProfile() const { return StatProfile; }
  98: 	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
  99: 	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;
 105: 	UFUNCTION(BlueprintPure, Category=Equipment)
 106: 	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }
 112: 	virtual void SpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn);
 117: 	virtual bool TrySpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn);
 121: 	virtual void DestroyEquipmentActors();
 125: 	virtual void OnEquipped();
 129: 	virtual void OnUnequipped();
 131: protected:
 133: 	virtual void OnSpawnedActorsChanged() {}
 134: #if UE_WITH_IRIS
 143: 	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
 144: 	                                          UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
 146: #endif
 150: 	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
 151: 	void K2_OnEquipped();
 155: 	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
 156: 	void K2_OnUnequipped();
 158: private:
 161: 	UFUNCTION()
 162: 	void OnRep_Instigator();
 164: 	UFUNCTION()
 165: 	void OnRep_SpawnedActors();
 167: private:
 169: 	UPROPERTY(ReplicatedUsing=OnRep_Instigator)
 170: 	TObjectPtr<UObject> Instigator;
 172: 	UPROPERTY(Replicated) FGuid EquipmentId;
 174: 	UPROPERTY(Replicated) int32 EquipmentLevel = 1;
 176: 	UPROPERTY(Replicated) TObjectPtr<const UHodgeEquipmentStatProfile> StatProfile;
 179: 	UPROPERTY(ReplicatedUsing=OnRep_SpawnedActors)
 180: 	TArray<TObjectPtr<AActor>> SpawnedActors;
 181: };
```

## HodgeEquipmentManagerComponent.h

Experience 注入 Pawn，ASC 就绪后装备默认剑，来源授予句柄精确撤销与客户端迟到绑定。

源码：[Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "Data/HodgeAbilitySet.h"
  12: #include "Components/PawnComponent.h"
  15: #include "Net/Serialization/FastArraySerializer.h"
  17: #include "HodgeEquipmentManagerComponent.generated.h"
  19: class UActorComponent;
  20: class UHodgeAbilitySystemComponent;
  21: class UHodgeEquipmentDefinition;
  22: class UHodgeEquipmentInstance;
  23: class UHodgeEquipmentManagerComponent;
  24: class UObject;
  25: struct FFrame;
  26: struct FHodgeEquipmentList;
  27: struct FNetDeltaSerializeInfo;
  28: struct FReplicationFlags;
  33: USTRUCT(BlueprintType)
  34: struct FHodgeAppliedEquipmentEntry : public FFastArraySerializerItem
  35: {
  36: 	GENERATED_BODY()
  40: 	FHodgeAppliedEquipmentEntry()
  41: 	{
  42: 	}
  48: 	FString GetDebugString() const;
  50: private:
  52: 	friend FHodgeEquipmentList;
  55: 	friend UHodgeEquipmentManagerComponent;
  60: 	UPROPERTY()
  61: 	TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition;
  64: 	UPROPERTY()
  65: 	TObjectPtr<UHodgeEquipmentInstance> Instance = nullptr;
  70: 	UPROPERTY(NotReplicated)
  71: 	FHodgeAbilitySet_GrantedHandles GrantedHandles;
  74: 	UPROPERTY(NotReplicated)
  75: 	TWeakObjectPtr<UHodgeAbilitySystemComponent> GrantedAbilitySystem;
  77: 	UPROPERTY(NotReplicated) FActiveGameplayEffectHandle AttributeHandle;
  78: };
  83: USTRUCT(BlueprintType)
  84: struct FHodgeEquipmentList : public FFastArraySerializer
  85: {
  86: 	GENERATED_BODY()
  90: 	FHodgeEquipmentList()
  91: 		: OwnerComponent(nullptr)
  92: 	{
  93: 	}
  99: 	FHodgeEquipmentList(UActorComponent* InOwnerComponent)
 100: 		: OwnerComponent(InOwnerComponent)
 101: 	{
 102: 	}
 104: public:
 112: 	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
 119: 	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
 126: 	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
 135: 	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
 136: 	{
 137: 		return FFastArraySerializer::FastArrayDeltaSerialize<FHodgeAppliedEquipmentEntry, FHodgeEquipmentList>(
 138: 			Entries, DeltaParms, *this);
 139: 	}
 148: 	UHodgeEquipmentInstance* AddEntry(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition, FGuid Id, int32 Level);
 154: 	void RemoveEntry(UHodgeEquipmentInstance* Instance);
 156: private:
 161: 	UHodgeAbilitySystemComponent* GetAbilitySystemComponent() const;
 164: 	friend UHodgeEquipmentManagerComponent;
 166: private:
 170: 	UPROPERTY()
 171: 	TArray<FHodgeAppliedEquipmentEntry> Entries;
 174: 	UPROPERTY(NotReplicated)
 175: 	TObjectPtr<UActorComponent> OwnerComponent;
 176: };
 179: template <>
 181: struct TStructOpsTypeTraits<FHodgeEquipmentList> : public TStructOpsTypeTraitsBase2<FHodgeEquipmentList>
 182: {
 183: 	enum { WithNetDeltaSerializer = true };
 184: };
 193: UCLASS(BlueprintType, Const)
 194: class HODGEPODGE_API UHodgeEquipmentManagerComponent : public UPawnComponent
 195: {
 196: 	GENERATED_BODY()
 198: public:
 203: 	UHodgeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 210: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
 211: 	UHodgeEquipmentInstance* EquipItem(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition);
 218: 	UHodgeEquipmentInstance* EquipItemWithState(TSubclassOf<UHodgeEquipmentDefinition> Definition, FGuid Id, int32 Level);
 224: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Attributes") bool SetEquipmentLevel(UHodgeEquipmentInstance* Instance, int32 Level);
 229: 	UHodgeEquipmentInstance* FindInstanceOfDefinition(TSubclassOf<UHodgeEquipmentDefinition> Definition) const;
 238: 	static bool ValidateEquipmentForGrant(TSubclassOf<UHodgeEquipmentDefinition> Definition,
 239: 		const UHodgeAbilitySystemComponent* ASC, int32 Level, TArray<FText>& Errors, bool bForInitialization = false);
 244: 	bool IsEquipmentReady(const UHodgeEquipmentInstance* Instance) const;
 248: 	UFUNCTION(BlueprintPure, Category="Hodge|Equipment") FString GetLastEquipError() const { return LastEquipError; }
 252: 	bool DidEquipHaveIrreversibleEffects() const { return bEquipIrreversibleEffects; }
 258: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
 259: 	void UnequipItem(UHodgeEquipmentInstance* ItemInstance);
 270: 	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
 271: 	                                 FReplicationFlags* RepFlags) override;
 281: 	virtual void InitializeComponent() override;
 285: 	virtual void UninitializeComponent() override;
 289: 	virtual void ReadyForReplication() override;
 300: 	UFUNCTION(BlueprintCallable, BlueprintPure)
 301: 	UHodgeEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UHodgeEquipmentInstance> InstanceType);
 310: 	UFUNCTION(BlueprintCallable, BlueprintPure)
 311: 	TArray<UHodgeEquipmentInstance*> GetEquipmentInstancesOfType(
 312: 		TSubclassOf<UHodgeEquipmentInstance> InstanceType) const;
 315: 	template <typename T>
 319: 	T* GetFirstInstanceOfType()
 320: 	{
 321: 		return (T*)GetFirstInstanceOfType(T::StaticClass());
 322: 	}
 324: private:
 326: 	friend struct FHodgeEquipmentMutationScope;
 327: 	friend struct FHodgeEquipmentList;
 329: 	UPROPERTY(Transient) FString LastEquipError;
 331: 	bool bEquipIrreversibleEffects = false;
 333: 	bool bEquipmentMutation = false;
 335: 	bool bPendingUninitialize = false;
 337: 	UPROPERTY(Replicated)
 338: 	FHodgeEquipmentList EquipmentList;
 339: };
```

## HodgeWeaponInstance.h

手持请求、显现阶段、计时器、复制/拥有者预测及拒绝恢复，不新增角色常驻武器表现组件。

源码：[Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)、[Equipment/HodgeWeaponPresentationTypes.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
  12: #include "Equipment/HodgeEquipmentInstance.h"
  13: #include "Combat/HodgeHitDetection.h"
  14: #include "Equipment/HodgeWeaponPresentationTypes.h"
  17: #include "GameFramework/InputDevicePropertyHandle.h"
  19: #include "HodgeWeaponInstance.generated.h"
  21: class UAnimInstance;
  22: class UObject;
  23: struct FFrame;
  24: struct FGameplayTagContainer;
  25: class UInputDeviceProperty;
  26: class UHodgeWeaponPresentationProfile;
  27: class USkeletalMeshComponent;
  37: class UHodgeCombatComponentBase;
  40: UCLASS()
  41: class HODGEPODGE_API UHodgeWeaponInstance : public UHodgeEquipmentInstance
  42: {
  43: 	GENERATED_BODY()
  45: public:
  47: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="SourceTag"))
  48: 	TArray<FHodgeHitSource> HitSources;
  51: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Presentation")
  52: 	TObjectPtr<UHodgeWeaponPresentationProfile> PresentationProfile;
  56: 	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
  57: 	UHodgeWeaponPresentationProfile* GetPresentationProfile() const { return PresentationProfile; }
  65: 	UFUNCTION(BlueprintCallable, Category="Hodge|Presentation")
  66: 	FGuid AcquireHandUse(FGuid ExecutionId, int32 OccurrenceId, int32 ActivationKey = 0, EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack);
  70: 	UFUNCTION(BlueprintCallable, Category="Hodge|Presentation")
  71: 	void ReleaseHandUse(FGuid Handle);
  75: 	void ReleaseHandUsesForExecution(const FGuid& ExecutionId);
  79: 	void RejectPredictedHandUse(int32 ActivationKey);
  83: 	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
  84: 	int32 GetHandUseCount() const { return HandRequests.Num(); }
  88: 	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
  89: 	FHodgeWeaponPresentationState GetPresentationState() const;
  93: 	double GetPresentationTime() const;
  95: 	void RefreshPresentationActors();
 101: 	static UHodgeWeaponInstance* ResolvePresentationWeapon(APawn* Pawn, UObject* SourceObject = nullptr);
 107: 	UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 111: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 117: 	virtual void OnEquipped() override;
 121: 	virtual void OnUnequipped() override;
 127: 	UFUNCTION(BlueprintCallable)
 128: 	void UpdateFiringTime();
 136: 	UFUNCTION(BlueprintPure)
 137: 	float GetTimeSinceLastInteractedWith() const;
 139: protected:
 141: 	virtual void OnSpawnedActorsChanged() override;
 143: 	virtual void BeginDestroy() override;
 159: 	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Input Devices")
 160: 	TArray<TObjectPtr<UInputDeviceProperty>> ApplicableDeviceProperties;
 170: 	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
 171: 	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;
 179: 	UFUNCTION(BlueprintCallable)
 180: 	const FPlatformUserId GetOwningUserId() const;
 188: 	UFUNCTION()
 189: 	void OnDeathStarted(AActor* OwningActor);
 200: 	void ApplyDeviceProperties();
 206: 	void RemoveDeviceProperties();
 208: private:
 209: 	friend struct FHodgeWeaponPresentationTestAccess;
 211: 	struct FHandRequest
 212: 	{
 214: 		FGuid ExecutionId;
 216: 		int32 OccurrenceId = INDEX_NONE;
 218: 		int32 ActivationKey = 0;
 220: 		EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack;
 222: 		uint64 Order = 0;
 223: 	};
 225: 	uint64 NextHandRequestOrder = 0;
 227: 	TMap<FGuid, FHandRequest> HandRequests;
 229: 	FTimerHandle PresentationTimer;
 231: 	bool bPresentationEquipped = false;
 233: 	bool bPresentationDisabled = false;
 235: 	bool bPredictedPresentation = false;
 237: 	FHodgeWeaponPresentationState LocalPresentation;
 239: 	UPROPERTY(ReplicatedUsing=OnRep_PresentationState)
 240: 	FHodgeWeaponPresentationState ReplicatedPresentation;
 242: 	UFUNCTION() void OnRep_PresentationState();
 244: 	void InitializePresentation();
 246: 	void ShutdownPresentation();
 251: 	void SetPresentationPhase(EHodgeWeaponPresentationPhase Phase, int32 ActivationKey, EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack);
 256: 	void SchedulePresentationPhase(EHodgeWeaponPresentationPhase Phase, float Seconds, EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack);
 258: 	void BeginIdlePresentation(EHodgeWeaponHandEndBehavior EndBehavior);
 262: 	bool CanDrivePresentation() const;
 264: 	void ClearPresentationTimer();
 266: 	FGuid CombatPoseLease;
 268: 	TWeakObjectPtr<UHodgeCombatComponentBase> PoseCombat;
 270: 	void UpdateOwnerPosePolicy();
 274: 	UPROPERTY(Transient)
 275: 	TSet<FInputDevicePropertyHandle> DevicePropertyHandles;
 278: 	double TimeLastEquipped = 0.0;
 281: 	double TimeLastFired = 0.0;
 282: };
```

## HodgeWeaponPresentationActor.h

检测 Mesh 留手，可见 Mesh 回背/悬浮/消隐挂 BackSocket，手持时挂回检测 Mesh。

源码：[Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponPresentationTypes.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "GameFramework/Actor.h"
   8: #include "Equipment/HodgeWeaponPresentationTypes.h"
   9: #include "HodgeWeaponPresentationActor.generated.h"
  11: class UHodgeWeaponInstance;
  12: class UHodgeWeaponPresentationProfile;
  13: class USkeletalMeshComponent;
  14: class UMaterialInstanceDynamic;
  15: class APawn;
  18: UCLASS(Blueprintable)
  19: class HODGEPODGE_API AHodgeWeaponPresentationActor : public AActor
  20: {
  21: 	GENERATED_BODY()
  22: public:
  24: 	AHodgeWeaponPresentationActor();
  28: 	void BindWeapon(UHodgeWeaponInstance* Instance);
  30: 	void RefreshPresentation();
  34: 	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetDetectionMesh() const { return SkeletalMesh; }
  38: 	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetVisualMesh() const { return WeaponVisualMesh; }
  42: 	UFUNCTION(BlueprintPure) float GetVisibilityAmount() const { return VisibilityAmount; }
  46: 	UFUNCTION(BlueprintPure) EHodgeWeaponPresentationPhase GetVisualPhase() const { return VisualPhase; }
  50: 	FTransform GetVisualTransform() const;
  54: 	virtual void Tick(float DeltaSeconds) override;
  55: protected:
  57: 	virtual void BeginPlay() override;
  61: 	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
  63: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
  65: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> WeaponVisualMesh;
  69: 	UFUNCTION(BlueprintImplementableEvent) void OnPresentationPhaseChanged(EHodgeWeaponPresentationPhase Phase);
  70: private:
  72: 	void TryBindWeapon();
  76: 	void ConfigureProfile(const UHodgeWeaponPresentationProfile* Profile);
  78: 	void EvaluatePresentation();
  80: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
  82: 	UPROPERTY(Transient) TObjectPtr<const UHodgeWeaponPresentationProfile> AppliedProfile;
  84: 	UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;
  86: 	FTimerHandle BindRetryTimer;
  88: 	int32 BindAttempts = 0;
  90: 	float VisibilityAmount = 0.f;
  92: 	EHodgeWeaponPresentationPhase VisualPhase = EHodgeWeaponPresentationPhase::Hidden;
  93: };
```

## HodgeWeaponPresentationProfile.h

模型/材质、手背插槽/偏移、曲线/时间；BackSocket 默认 WeaponOnBack，BackTransform 为插槽内偏移。

源码：[Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Engine/DataAsset.h"
   8: #include "HodgeWeaponPresentationProfile.generated.h"
  10: class USkeletalMesh;
  11: class UMaterialInterface;
  12: class UCurveFloat;
  15: UCLASS(BlueprintType, Const)
  16: class HODGEPODGE_API UHodgeWeaponPresentationProfile : public UDataAsset
  17: {
  18: 	GENERATED_BODY()
  19: public:
  21: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<USkeletalMesh> WeaponMesh;
  23: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<TObjectPtr<UMaterialInterface>> Materials;
  25: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName HandSocket = TEXT("WeaponOnHand");
  27: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FTransform HandOffset = FTransform::Identity;
  29: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName BackSocket = TEXT("WeaponOnBack");
  31: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ToolTip="Offset relative to the character Mesh BackSocket, not the Pawn root."))
  32: 	FTransform BackTransform = FTransform::Identity;
  34: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FVector ReturnArcOffset = FVector(0.f, 15.f, 10.f);
  36: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UCurveFloat> ReturnCurve;
  38: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float DrawSeconds = .08f;
  40: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float ReturnGraceSeconds = .06f;
  42: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float ReturnSeconds = .3f;
  44: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverSeconds = 2.f;
  46: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float FadeSeconds = .35f;
  48: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverAmplitude = 2.f;
  50: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverFrequency = .8f;
  52: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName VisibilityParameter = TEXT("WeaponVisibility");
  57: 	bool Validate(TArray<FText>& Errors) const;
  58: #if WITH_EDITOR
  63: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  64: #endif
  65: };
```

## HodgeWeaponPresentationTypes.h

表现阶段与复制快照：服务器时间、起始变换/可见度、版本和激活身份。

源码：[Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "HodgeWeaponPresentationTypes.generated.h"
  10: UENUM(BlueprintType)
  11: enum class EHodgeWeaponHandEndBehavior : uint8
  12: {
  13: 	ReturnToBack UMETA(DisplayName="回背部后延迟淡出"),
  14: 	FadeAtHand UMETA(DisplayName="在手部直接淡出")
  15: };
  18: UENUM(BlueprintType)
  19: enum class EHodgeWeaponPresentationPhase : uint8
  20: {
  22: 	Hidden,
  24: 	Hand,
  26: 	Returning,
  28: 	Hovering,
  30: 	Fading
  31: };
  34: USTRUCT(BlueprintType)
  35: struct FHodgeWeaponPresentationState
  36: {
  37: 	GENERATED_BODY()
  39: 	UPROPERTY(BlueprintReadOnly) EHodgeWeaponPresentationPhase Phase = EHodgeWeaponPresentationPhase::Hidden;
  41: 	UPROPERTY(BlueprintReadOnly) EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack;
  43: 	UPROPERTY(BlueprintReadOnly) double StartTime = 0;
  45: 	UPROPERTY(BlueprintReadOnly) FTransform StartRelativeTransform = FTransform::Identity;
  47: 	UPROPERTY(BlueprintReadOnly) float StartVisibility = 0.f;
  49: 	UPROPERTY(BlueprintReadOnly) int32 Revision = 0;
  51: 	UPROPERTY(BlueprintReadOnly) int32 ActivationKey = 0;
  52: };
```
