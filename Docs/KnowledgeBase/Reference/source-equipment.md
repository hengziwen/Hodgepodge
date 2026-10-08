# Equipment 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeEquipmentDefinition.cpp

装备实例类、Actor 挂接与 AbilitySets 配置。

源码：[Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)

定义候选（多行签名仅展示首行）：

- L11: `UHodgeEquipmentDefinition::UHodgeEquipmentDefinition(const FObjectInitializer& ObjectInitializer)`

## HodgeEquipmentInstance.cpp

Pawn 所属复制 UObject、SpawnedActors RepNotify 和装备/卸装生命周期。

源码：[Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentInstance.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Data/HodgeEquipmentStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)

定义候选（多行签名仅展示首行）：

- L32: `UHodgeEquipmentInstance::UHodgeEquipmentInstance(const FObjectInitializer& ObjectInitializer)`
- L38: `UWorld* UHodgeEquipmentInstance::GetWorld() const`
- L54: `void UHodgeEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L72: `void UHodgeEquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,`
- L86: `APawn* UHodgeEquipmentInstance::GetPawn() const`
- L92: `APawn* UHodgeEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const`
- L113: `void UHodgeEquipmentInstance::SpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn)`
- L152: `void UHodgeEquipmentInstance::DestroyEquipmentActors()`
- L167: `void UHodgeEquipmentInstance::OnRep_SpawnedActors()`
- L173: `void UHodgeEquipmentInstance::OnEquipped()`
- L180: `void UHodgeEquipmentInstance::OnUnequipped()`
- L187: `void UHodgeEquipmentInstance::OnRep_Instigator()`
- L191: `void UHodgeEquipmentInstance::SetStatIdentity(FGuid Id, int32 Level, const UHodgeEquipmentStatProfile* Profile)`

## HodgeEquipmentManagerComponent.cpp

Experience 注入 Pawn，ASC 就绪后装备默认剑，来源授予句柄精确撤销与客户端迟到绑定。

源码：[Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[AbilitySystem/Stats/HodgeAttributeCoordinator.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeAttributeCoordinator.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Core/PlayState/HodgePlayerState.h](../../../Source/Hodgepodge/Public/Core/PlayState/HodgePlayerState.h)、[Data/HodgeEquipmentStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h)、[Character/HodgeCombatCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeCombatCharacter.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

定义候选（多行签名仅展示首行）：

- L52: `FString FHodgeAppliedEquipmentEntry::GetDebugString() const`
- L61: `void FHodgeEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)`
- L78: `void FHodgeEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)`
- L95: `void FHodgeEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)`
- L110: `UHodgeAbilitySystemComponent* FHodgeEquipmentList::GetAbilitySystemComponent() const`
- L123: `UHodgeEquipmentInstance* FHodgeEquipmentList::AddEntry(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition, FGuid Id, int32 Level)`
- L212: `void FHodgeEquipmentList::RemoveEntry(UHodgeEquipmentInstance* Instance)`
- L248: `UHodgeEquipmentManagerComponent::UHodgeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)`
- L260: `void UHodgeEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L270: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::EquipItem(TSubclassOf<UHodgeEquipmentDefinition> EquipmentClass)`
- L277: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::EquipItemWithState(TSubclassOf<UHodgeEquipmentDefinition> EquipmentClass, FGuid Id, int32 Level)`
- L321: `void UHodgeEquipmentManagerComponent::UnequipItem(UHodgeEquipmentInstance* ItemInstance)`
- L352: `bool UHodgeEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch,`
- L377: `void UHodgeEquipmentManagerComponent::InitializeComponent()`
- L394: `void UHodgeEquipmentManagerComponent::UninitializeComponent()`
- L419: `void UHodgeEquipmentManagerComponent::ReadyForReplication()`
- L446: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::GetFirstInstanceOfType(`
- L469: `TArray<UHodgeEquipmentInstance*> UHodgeEquipmentManagerComponent::GetEquipmentInstancesOfType(`
- L494: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::FindInstanceOfDefinition(TSubclassOf<UHodgeEquipmentDefinition> Definition) const`
- L500: `bool UHodgeEquipmentManagerComponent::SetEquipmentLevel(UHodgeEquipmentInstance* Instance, int32 Level)`

## HodgeWeaponInstance.cpp

手持请求、显现阶段、计时器、复制/拥有者预测及拒绝恢复，不新增角色常驻武器表现组件。

源码：[Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Equipment/HodgeWeaponPresentationActor.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h)、[Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L43: `UHodgeWeaponInstance::UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer)`
- L68: `void UHodgeWeaponInstance::OnEquipped()`
- L90: `void UHodgeWeaponInstance::OnUnequipped()`
- L101: `void UHodgeWeaponInstance::UpdateFiringTime()`
- L114: `float UHodgeWeaponInstance::GetTimeSinceLastInteractedWith() const`
- L143: `TSubclassOf<UAnimInstance> UHodgeWeaponInstance::PickBestAnimLayer(bool bEquipped,`
- L154: `const FPlatformUserId UHodgeWeaponInstance::GetOwningUserId() const`
- L168: `void UHodgeWeaponInstance::ApplyDeviceProperties()`
- L206: `void UHodgeWeaponInstance::RemoveDeviceProperties()`
- L229: `void UHodgeWeaponInstance::OnDeathStarted(AActor* OwningActor)`
- L244: `void UHodgeWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L250: `double UHodgeWeaponInstance::GetPresentationTime() const`
- L257: `bool UHodgeWeaponInstance::CanDrivePresentation() const`
- L265: `UHodgeWeaponInstance* UHodgeWeaponInstance::ResolvePresentationWeapon(APawn* Pawn, UObject* SourceObject)`
- L291: `void UHodgeWeaponInstance::InitializePresentation()`
- L311: `void UHodgeWeaponInstance::ClearPresentationTimer()`
- L316: `void UHodgeWeaponInstance::ShutdownPresentation()`
- L331: `FGuid UHodgeWeaponInstance::AcquireHandUse(FGuid ExecutionId, int32 OccurrenceId, int32 ActivationKey)`
- L361: `void UHodgeWeaponInstance::ReleaseHandUse(FGuid Handle)`
- L368: `void UHodgeWeaponInstance::ReleaseHandUsesForExecution(const FGuid& ExecutionId)`
- L375: `void UHodgeWeaponInstance::BeginIdlePresentation()`
- L381: `void UHodgeWeaponInstance::SchedulePresentationPhase(EHodgeWeaponPresentationPhase Phase, float Seconds)`
- L399: `void UHodgeWeaponInstance::SetPresentationPhase(EHodgeWeaponPresentationPhase Phase, int32 ActivationKey)`
- L443: `FHodgeWeaponPresentationState UHodgeWeaponInstance::GetPresentationState() const`
- L451: `void UHodgeWeaponInstance::OnRep_PresentationState()`
- L472: `void UHodgeWeaponInstance::RejectPredictedHandUse(int32 ActivationKey)`
- L490: `void UHodgeWeaponInstance::RefreshPresentationActors()`
- L499: `void UHodgeWeaponInstance::OnSpawnedActorsChanged()`
- L505: `void UHodgeWeaponInstance::BeginDestroy()`
- L512: `void UHodgeWeaponInstance::UpdateOwnerPosePolicy()`

## HodgeWeaponPresentationActor.cpp

检测 Mesh 留手，可见 Mesh 回背/悬浮/消隐挂 BackSocket，手持时挂回检测 Mesh。

源码：[Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationActor.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationActor.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponPresentationActor.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L14: `AHodgeWeaponPresentationActor::AHodgeWeaponPresentationActor()`
- L36: `void AHodgeWeaponPresentationActor::BeginPlay()`
- L44: `void AHodgeWeaponPresentationActor::TryBindWeapon()`
- L56: `void AHodgeWeaponPresentationActor::ConfigureProfile(const UHodgeWeaponPresentationProfile* Profile)`
- L73: `void AHodgeWeaponPresentationActor::BindWeapon(UHodgeWeaponInstance* Instance)`
- L97: `void AHodgeWeaponPresentationActor::RefreshPresentation()`
- L105: `FTransform AHodgeWeaponPresentationActor::GetVisualTransform() const`
- L110: `void AHodgeWeaponPresentationActor::EvaluatePresentation()`
- L182: `void AHodgeWeaponPresentationActor::Tick(float DeltaSeconds)`
- L188: `void AHodgeWeaponPresentationActor::EndPlay(const EEndPlayReason::Type Reason)`

## HodgeWeaponPresentationProfile.cpp

模型/材质、手背插槽/偏移、曲线/时间；BackSocket 默认 WeaponOnBack，BackTransform 为插槽内偏移。

源码：[Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationProfile.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponPresentationProfile.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)

定义候选（多行签名仅展示首行）：

- L8: `bool UHodgeWeaponPresentationProfile::Validate(TArray<FText>& Errors) const`
- L25: `EDataValidationResult UHodgeWeaponPresentationProfile::IsDataValid(FDataValidationContext& Context) const`

## HodgeEquipmentDefinition.h

装备实例类、Actor 挂接与 AbilitySets 配置。

源码：[Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "Templates/SubclassOf.h"
   8: #include "HodgeEquipmentDefinition.generated.h"
  10: class AActor;
  11: class UHodgeAbilitySet;
  12: class UHodgeEquipmentInstance;
  13: class UHodgeEquipmentStatProfile;
  19: USTRUCT()
  20: struct FHodgeEquipmentActorToSpawn
  21: {
  22: 	GENERATED_BODY()
  25: 	FHodgeEquipmentActorToSpawn()
  26: 	{
  27: 	}
  30: 	UPROPERTY(EditAnywhere, Category=Equipment)
  31: 	TSubclassOf<AActor> ActorToSpawn;
  34: 	UPROPERTY(EditAnywhere, Category=Equipment)
  35: 	FName AttachSocket;
  38: 	UPROPERTY(EditAnywhere, Category=Equipment)
  39: 	FTransform AttachTransform;
  40: };
  51: UCLASS(Blueprintable, Const, Abstract, BlueprintType)
  52: class UHodgeEquipmentDefinition : public UObject
  53: {
  54: 	GENERATED_BODY()
  56: public:
  58: 	UHodgeEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  63: 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
  64: 	TSubclassOf<UHodgeEquipmentInstance> InstanceType;
  69: 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
  70: 	TArray<TObjectPtr<const UHodgeAbilitySet>> AbilitySetsToGrant;
  71: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Attributes")
  72: 	TObjectPtr<UHodgeEquipmentStatProfile> StatProfile;
  77: 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
  78: 	TArray<FHodgeEquipmentActorToSpawn> ActorsToSpawn;
  79: };
```

## HodgeEquipmentInstance.h

Pawn 所属复制 UObject、SpawnedActors RepNotify 和装备/卸装生命周期。

源码：[Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "Engine/World.h"
   8: #include "HodgeEquipmentInstance.generated.h"
  10: class AActor;
  11: class APawn;
  12: struct FFrame;
  13: struct FHodgeEquipmentActorToSpawn;
  14: class UHodgeEquipmentStatProfile;
  24: UCLASS(BlueprintType, Blueprintable)
  25: class UHodgeEquipmentInstance : public UObject
  26: {
  27: 	GENERATED_BODY()
  29: public:
  31: 	UHodgeEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  36: 	virtual bool IsSupportedForNetworking() const override { return true; }
  39: 	virtual UWorld* GetWorld() const override final;
  44: 	UFUNCTION(BlueprintPure, Category=Equipment)
  45: 	UObject* GetInstigator() const { return Instigator; }
  48: 	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }
  51: 	UFUNCTION(BlueprintPure, Category=Equipment)
  52: 	APawn* GetPawn() const;
  53: 	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") int32 GetEquipmentLevel() const { return EquipmentLevel; }
  54: 	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") FGuid GetEquipmentId() const { return EquipmentId; }
  55: 	void SetStatIdentity(FGuid Id, int32 Level, const UHodgeEquipmentStatProfile* Profile);
  56: 	const UHodgeEquipmentStatProfile* GetStatProfile() const { return StatProfile; }
  59: 	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
  60: 	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;
  63: 	UFUNCTION(BlueprintPure, Category=Equipment)
  64: 	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }
  67: 	virtual void SpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn);
  70: 	virtual void DestroyEquipmentActors();
  73: 	virtual void OnEquipped();
  76: 	virtual void OnUnequipped();
  78: protected:
  79: 	virtual void OnSpawnedActorsChanged() {}
  80: #if UE_WITH_IRIS
  85: 	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
  86: 	                                          UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
  88: #endif
  91: 	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
  92: 	void K2_OnEquipped();
  95: 	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
  96: 	void K2_OnUnequipped();
  98: private:
 100: 	UFUNCTION()
 101: 	void OnRep_Instigator();
 102: 	UFUNCTION()
 103: 	void OnRep_SpawnedActors();
 105: private:
 107: 	UPROPERTY(ReplicatedUsing=OnRep_Instigator)
 108: 	TObjectPtr<UObject> Instigator;
 109: 	UPROPERTY(Replicated) FGuid EquipmentId;
 110: 	UPROPERTY(Replicated) int32 EquipmentLevel = 1;
 111: 	UPROPERTY(Replicated) TObjectPtr<const UHodgeEquipmentStatProfile> StatProfile;
 114: 	UPROPERTY(ReplicatedUsing=OnRep_SpawnedActors)
 115: 	TArray<TObjectPtr<AActor>> SpawnedActors;
 116: };
```

## HodgeEquipmentManagerComponent.h

Experience 注入 Pawn，ASC 就绪后装备默认剑，来源授予句柄精确撤销与客户端迟到绑定。

源码：[Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "Data/HodgeAbilitySet.h"
   9: #include "Components/PawnComponent.h"
  12: #include "Net/Serialization/FastArraySerializer.h"
  14: #include "HodgeEquipmentManagerComponent.generated.h"
  16: class UActorComponent;
  17: class UHodgeAbilitySystemComponent;
  18: class UHodgeEquipmentDefinition;
  19: class UHodgeEquipmentInstance;
  20: class UHodgeEquipmentManagerComponent;
  21: class UObject;
  22: struct FFrame;
  23: struct FHodgeEquipmentList;
  24: struct FNetDeltaSerializeInfo;
  25: struct FReplicationFlags;
  30: USTRUCT(BlueprintType)
  31: struct FHodgeAppliedEquipmentEntry : public FFastArraySerializerItem
  32: {
  33: 	GENERATED_BODY()
  36: 	FHodgeAppliedEquipmentEntry()
  37: 	{
  38: 	}
  41: 	FString GetDebugString() const;
  43: private:
  45: 	friend FHodgeEquipmentList;
  48: 	friend UHodgeEquipmentManagerComponent;
  53: 	UPROPERTY()
  54: 	TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition;
  57: 	UPROPERTY()
  58: 	TObjectPtr<UHodgeEquipmentInstance> Instance = nullptr;
  63: 	UPROPERTY(NotReplicated)
  64: 	FHodgeAbilitySet_GrantedHandles GrantedHandles;
  67: 	UPROPERTY(NotReplicated)
  68: 	TWeakObjectPtr<UHodgeAbilitySystemComponent> GrantedAbilitySystem;
  69: 	UPROPERTY(NotReplicated) FActiveGameplayEffectHandle AttributeHandle;
  70: };
  75: USTRUCT(BlueprintType)
  76: struct FHodgeEquipmentList : public FFastArraySerializer
  77: {
  78: 	GENERATED_BODY()
  81: 	FHodgeEquipmentList()
  82: 		: OwnerComponent(nullptr)
  83: 	{
  84: 	}
  87: 	FHodgeEquipmentList(UActorComponent* InOwnerComponent)
  88: 		: OwnerComponent(InOwnerComponent)
  89: 	{
  90: 	}
  92: public:
  96: 	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
  99: 	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
 102: 	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
 107: 	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
 108: 	{
 109: 		return FFastArraySerializer::FastArrayDeltaSerialize<FHodgeAppliedEquipmentEntry, FHodgeEquipmentList>(
 110: 			Entries, DeltaParms, *this);
 111: 	}
 114: 	UHodgeEquipmentInstance* AddEntry(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition, FGuid Id, int32 Level);
 117: 	void RemoveEntry(UHodgeEquipmentInstance* Instance);
 119: private:
 121: 	UHodgeAbilitySystemComponent* GetAbilitySystemComponent() const;
 124: 	friend UHodgeEquipmentManagerComponent;
 126: private:
 130: 	UPROPERTY()
 131: 	TArray<FHodgeAppliedEquipmentEntry> Entries;
 134: 	UPROPERTY(NotReplicated)
 135: 	TObjectPtr<UActorComponent> OwnerComponent;
 136: };
 139: template <>
 140: struct TStructOpsTypeTraits<FHodgeEquipmentList> : public TStructOpsTypeTraitsBase2<FHodgeEquipmentList>
 141: {
 142: 	enum { WithNetDeltaSerializer = true };
 143: };
 152: UCLASS(BlueprintType, Const)
 153: class HODGEPODGE_API UHodgeEquipmentManagerComponent : public UPawnComponent
 154: {
 155: 	GENERATED_BODY()
 157: public:
 159: 	UHodgeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 162: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
 163: 	UHodgeEquipmentInstance* EquipItem(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition);
 164: 	UHodgeEquipmentInstance* EquipItemWithState(TSubclassOf<UHodgeEquipmentDefinition> Definition, FGuid Id, int32 Level);
 165: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Attributes") bool SetEquipmentLevel(UHodgeEquipmentInstance* Instance, int32 Level);
 166: 	UHodgeEquipmentInstance* FindInstanceOfDefinition(TSubclassOf<UHodgeEquipmentDefinition> Definition) const;
 169: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
 170: 	void UnequipItem(UHodgeEquipmentInstance* ItemInstance);
 175: 	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
 176: 	                                 FReplicationFlags* RepFlags) override;
 185: 	virtual void InitializeComponent() override;
 188: 	virtual void UninitializeComponent() override;
 191: 	virtual void ReadyForReplication() override;
 198: 	UFUNCTION(BlueprintCallable, BlueprintPure)
 199: 	UHodgeEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UHodgeEquipmentInstance> InstanceType);
 204: 	UFUNCTION(BlueprintCallable, BlueprintPure)
 205: 	TArray<UHodgeEquipmentInstance*> GetEquipmentInstancesOfType(
 206: 		TSubclassOf<UHodgeEquipmentInstance> InstanceType) const;
 209: 	template <typename T>
 210: 	T* GetFirstInstanceOfType()
 211: 	{
 212: 		return (T*)GetFirstInstanceOfType(T::StaticClass());
 213: 	}
 215: private:
 217: 	friend struct FHodgeEquipmentMutationScope;
 218: 	bool bEquipmentMutation = false;
 219: 	bool bPendingUninitialize = false;
 220: 	UPROPERTY(Replicated)
 221: 	FHodgeEquipmentList EquipmentList;
 222: };
```

## HodgeWeaponInstance.h

手持请求、显现阶段、计时器、复制/拥有者预测及拒绝恢复，不新增角色常驻武器表现组件。

源码：[Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)、[Equipment/HodgeWeaponPresentationTypes.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   9: #include "Equipment/HodgeEquipmentInstance.h"
  10: #include "Combat/HodgeHitDetection.h"
  11: #include "Equipment/HodgeWeaponPresentationTypes.h"
  14: #include "GameFramework/InputDevicePropertyHandle.h"
  16: #include "HodgeWeaponInstance.generated.h"
  18: class UAnimInstance;
  19: class UObject;
  20: struct FFrame;
  21: struct FGameplayTagContainer;
  22: class UInputDeviceProperty;
  23: class UHodgeWeaponPresentationProfile;
  24: class USkeletalMeshComponent;
  34: class UHodgeCombatComponentBase;
  36: UCLASS()
  37: class HODGEPODGE_API UHodgeWeaponInstance : public UHodgeEquipmentInstance
  38: {
  39: 	GENERATED_BODY()
  41: public:
  43: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="SourceTag"))
  44: 	TArray<FHodgeHitSource> HitSources;
  46: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Presentation")
  47: 	TObjectPtr<UHodgeWeaponPresentationProfile> PresentationProfile;
  48: 	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
  49: 	UHodgeWeaponPresentationProfile* GetPresentationProfile() const { return PresentationProfile; }
  50: 	UFUNCTION(BlueprintCallable, Category="Hodge|Presentation")
  51: 	FGuid AcquireHandUse(FGuid ExecutionId, int32 OccurrenceId, int32 ActivationKey = 0);
  52: 	UFUNCTION(BlueprintCallable, Category="Hodge|Presentation")
  53: 	void ReleaseHandUse(FGuid Handle);
  54: 	void ReleaseHandUsesForExecution(const FGuid& ExecutionId);
  55: 	void RejectPredictedHandUse(int32 ActivationKey);
  56: 	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
  57: 	int32 GetHandUseCount() const { return HandRequests.Num(); }
  58: 	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
  59: 	FHodgeWeaponPresentationState GetPresentationState() const;
  60: 	double GetPresentationTime() const;
  61: 	void RefreshPresentationActors();
  62: 	static UHodgeWeaponInstance* ResolvePresentationWeapon(APawn* Pawn, UObject* SourceObject = nullptr);
  65: 	UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  66: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
  71: 	virtual void OnEquipped() override;
  74: 	virtual void OnUnequipped() override;
  79: 	UFUNCTION(BlueprintCallable)
  80: 	void UpdateFiringTime();
  85: 	UFUNCTION(BlueprintPure)
  86: 	float GetTimeSinceLastInteractedWith() const;
  88: protected:
  89: 	virtual void OnSpawnedActorsChanged() override;
  90: 	virtual void BeginDestroy() override;
 106: 	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Input Devices")
 107: 	TArray<TObjectPtr<UInputDeviceProperty>> ApplicableDeviceProperties;
 112: 	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
 113: 	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;
 118: 	UFUNCTION(BlueprintCallable)
 119: 	const FPlatformUserId GetOwningUserId() const;
 124: 	UFUNCTION()
 125: 	void OnDeathStarted(AActor* OwningActor);
 135: 	void ApplyDeviceProperties();
 140: 	void RemoveDeviceProperties();
 142: private:
 143: 	friend struct FHodgeWeaponPresentationTestAccess;
 144: 	struct FHandRequest { FGuid ExecutionId; int32 OccurrenceId = INDEX_NONE; int32 ActivationKey = 0; };
 145: 	TMap<FGuid, FHandRequest> HandRequests;
 146: 	FTimerHandle PresentationTimer;
 147: 	bool bPresentationEquipped = false;
 148: 	bool bPresentationDisabled = false;
 149: 	bool bPredictedPresentation = false;
 150: 	FHodgeWeaponPresentationState LocalPresentation;
 151: 	UPROPERTY(ReplicatedUsing=OnRep_PresentationState)
 152: 	FHodgeWeaponPresentationState ReplicatedPresentation;
 153: 	UFUNCTION() void OnRep_PresentationState();
 154: 	void InitializePresentation();
 155: 	void ShutdownPresentation();
 156: 	void SetPresentationPhase(EHodgeWeaponPresentationPhase Phase, int32 ActivationKey);
 157: 	void SchedulePresentationPhase(EHodgeWeaponPresentationPhase Phase, float Seconds);
 158: 	void BeginIdlePresentation();
 159: 	bool CanDrivePresentation() const;
 160: 	void ClearPresentationTimer();
 161: 	FGuid CombatPoseLease;
 162: 	TWeakObjectPtr<UHodgeCombatComponentBase> PoseCombat;
 163: 	void UpdateOwnerPosePolicy();
 167: 	UPROPERTY(Transient)
 168: 	TSet<FInputDevicePropertyHandle> DevicePropertyHandles;
 171: 	double TimeLastEquipped = 0.0;
 174: 	double TimeLastFired = 0.0;
 175: };
```

## HodgeWeaponPresentationActor.h

检测 Mesh 留手，可见 Mesh 回背/悬浮/消隐挂 BackSocket，手持时挂回检测 Mesh。

源码：[Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponPresentationTypes.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "GameFramework/Actor.h"
   5: #include "Equipment/HodgeWeaponPresentationTypes.h"
   6: #include "HodgeWeaponPresentationActor.generated.h"
   8: class UHodgeWeaponInstance;
   9: class UHodgeWeaponPresentationProfile;
  10: class USkeletalMeshComponent;
  11: class UMaterialInstanceDynamic;
  12: class APawn;
  15: UCLASS(Blueprintable)
  16: class HODGEPODGE_API AHodgeWeaponPresentationActor : public AActor
  17: {
  18: 	GENERATED_BODY()
  19: public:
  20: 	AHodgeWeaponPresentationActor();
  21: 	void BindWeapon(UHodgeWeaponInstance* Instance);
  22: 	void RefreshPresentation();
  23: 	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetDetectionMesh() const { return SkeletalMesh; }
  24: 	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetVisualMesh() const { return WeaponVisualMesh; }
  25: 	UFUNCTION(BlueprintPure) float GetVisibilityAmount() const { return VisibilityAmount; }
  26: 	UFUNCTION(BlueprintPure) EHodgeWeaponPresentationPhase GetVisualPhase() const { return VisualPhase; }
  27: 	FTransform GetVisualTransform() const;
  28: 	virtual void Tick(float DeltaSeconds) override;
  29: protected:
  30: 	virtual void BeginPlay() override;
  31: 	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
  32: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
  33: 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> WeaponVisualMesh;
  34: 	UFUNCTION(BlueprintImplementableEvent) void OnPresentationPhaseChanged(EHodgeWeaponPresentationPhase Phase);
  35: private:
  36: 	void TryBindWeapon();
  37: 	void ConfigureProfile(const UHodgeWeaponPresentationProfile* Profile);
  38: 	void EvaluatePresentation();
  39: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
  40: 	UPROPERTY(Transient) TObjectPtr<const UHodgeWeaponPresentationProfile> AppliedProfile;
  41: 	UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;
  42: 	FTimerHandle BindRetryTimer;
  43: 	int32 BindAttempts = 0;
  44: 	float VisibilityAmount = 0.f;
  45: 	EHodgeWeaponPresentationPhase VisualPhase = EHodgeWeaponPresentationPhase::Hidden;
  46: };
```

## HodgeWeaponPresentationProfile.h

模型/材质、手背插槽/偏移、曲线/时间；BackSocket 默认 WeaponOnBack，BackTransform 为插槽内偏移。

源码：[Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Engine/DataAsset.h"
   5: #include "HodgeWeaponPresentationProfile.generated.h"
   7: class USkeletalMesh;
   8: class UMaterialInterface;
   9: class UCurveFloat;
  12: UCLASS(BlueprintType, Const)
  13: class HODGEPODGE_API UHodgeWeaponPresentationProfile : public UDataAsset
  14: {
  15: 	GENERATED_BODY()
  16: public:
  17: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<USkeletalMesh> WeaponMesh;
  18: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<TObjectPtr<UMaterialInterface>> Materials;
  19: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName HandSocket = TEXT("WeaponOnHand");
  20: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FTransform HandOffset = FTransform::Identity;
  21: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName BackSocket = TEXT("WeaponOnBack");
  22: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ToolTip="Offset relative to the character Mesh BackSocket, not the Pawn root."))
  23: 	FTransform BackTransform = FTransform::Identity;
  24: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FVector ReturnArcOffset = FVector(0.f, 15.f, 10.f);
  25: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UCurveFloat> ReturnCurve;
  26: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float DrawSeconds = .08f;
  27: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float ReturnGraceSeconds = .06f;
  28: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float ReturnSeconds = .3f;
  29: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverSeconds = 2.f;
  30: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float FadeSeconds = .35f;
  31: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverAmplitude = 2.f;
  32: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverFrequency = .8f;
  33: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName VisibilityParameter = TEXT("WeaponVisibility");
  34: 	bool Validate(TArray<FText>& Errors) const;
  35: #if WITH_EDITOR
  36: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  37: #endif
  38: };
```

## HodgeWeaponPresentationTypes.h

表现阶段与复制快照：服务器时间、起始变换/可见度、版本和激活身份。

源码：[Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "HodgeWeaponPresentationTypes.generated.h"
   6: UENUM(BlueprintType)
   7: enum class EHodgeWeaponPresentationPhase : uint8
   8: {
   9: 	Hidden,
  10: 	Hand,
  11: 	Returning,
  12: 	Hovering,
  13: 	Fading
  14: };
  17: USTRUCT(BlueprintType)
  18: struct FHodgeWeaponPresentationState
  19: {
  20: 	GENERATED_BODY()
  21: 	UPROPERTY(BlueprintReadOnly) EHodgeWeaponPresentationPhase Phase = EHodgeWeaponPresentationPhase::Hidden;
  22: 	UPROPERTY(BlueprintReadOnly) double StartTime = 0;
  23: 	UPROPERTY(BlueprintReadOnly) FTransform StartRelativeTransform = FTransform::Identity;
  24: 	UPROPERTY(BlueprintReadOnly) float StartVisibility = 0.f;
  25: 	UPROPERTY(BlueprintReadOnly) int32 Revision = 0;
  26: 	UPROPERTY(BlueprintReadOnly) int32 ActivationKey = 0;
  27: };
```
