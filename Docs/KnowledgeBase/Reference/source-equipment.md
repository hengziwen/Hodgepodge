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

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)

定义候选（多行签名仅展示首行）：

- L31: `UHodgeEquipmentInstance::UHodgeEquipmentInstance(const FObjectInitializer& ObjectInitializer)`
- L37: `UWorld* UHodgeEquipmentInstance::GetWorld() const`
- L53: `void UHodgeEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L68: `void UHodgeEquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,`
- L82: `APawn* UHodgeEquipmentInstance::GetPawn() const`
- L88: `APawn* UHodgeEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const`
- L109: `void UHodgeEquipmentInstance::SpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn)`
- L148: `void UHodgeEquipmentInstance::DestroyEquipmentActors()`
- L163: `void UHodgeEquipmentInstance::OnRep_SpawnedActors()`
- L169: `void UHodgeEquipmentInstance::OnEquipped()`
- L176: `void UHodgeEquipmentInstance::OnUnequipped()`
- L183: `void UHodgeEquipmentInstance::OnRep_Instigator()`

## HodgeEquipmentManagerComponent.cpp

Experience 注入 Pawn，ASC 就绪后装备默认剑，来源授予句柄精确撤销与客户端迟到绑定。

源码：[Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)、[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

定义候选（多行签名仅展示首行）：

- L36: `FString FHodgeAppliedEquipmentEntry::GetDebugString() const`
- L45: `void FHodgeEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)`
- L62: `void FHodgeEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)`
- L79: `void FHodgeEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)`
- L94: `UHodgeAbilitySystemComponent* FHodgeEquipmentList::GetAbilitySystemComponent() const`
- L107: `UHodgeEquipmentInstance* FHodgeEquipmentList::AddEntry(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition)`
- L176: `void FHodgeEquipmentList::RemoveEntry(UHodgeEquipmentInstance* Instance)`
- L211: `UHodgeEquipmentManagerComponent::UHodgeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)`
- L223: `void UHodgeEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L233: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::EquipItem(`
- L264: `void UHodgeEquipmentManagerComponent::UnequipItem(UHodgeEquipmentInstance* ItemInstance)`
- L284: `bool UHodgeEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch,`
- L309: `void UHodgeEquipmentManagerComponent::InitializeComponent()`
- L325: `void UHodgeEquipmentManagerComponent::UninitializeComponent()`
- L349: `void UHodgeEquipmentManagerComponent::ReadyForReplication()`
- L376: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::GetFirstInstanceOfType(`
- L399: `TArray<UHodgeEquipmentInstance*> UHodgeEquipmentManagerComponent::GetEquipmentInstancesOfType(`

## HodgeWeaponInstance.cpp

手持请求、显现阶段、计时器、复制/拥有者预测及拒绝恢复，不新增角色常驻武器表现组件。

源码：[Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Equipment/HodgeWeaponPresentationActor.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationActor.h)、[Equipment/HodgeWeaponPresentationProfile.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)

定义候选（多行签名仅展示首行）：

- L42: `UHodgeWeaponInstance::UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer)`
- L67: `void UHodgeWeaponInstance::OnEquipped()`
- L89: `void UHodgeWeaponInstance::OnUnequipped()`
- L100: `void UHodgeWeaponInstance::UpdateFiringTime()`
- L113: `float UHodgeWeaponInstance::GetTimeSinceLastInteractedWith() const`
- L142: `TSubclassOf<UAnimInstance> UHodgeWeaponInstance::PickBestAnimLayer(bool bEquipped,`
- L153: `const FPlatformUserId UHodgeWeaponInstance::GetOwningUserId() const`
- L167: `void UHodgeWeaponInstance::ApplyDeviceProperties()`
- L205: `void UHodgeWeaponInstance::RemoveDeviceProperties()`
- L228: `void UHodgeWeaponInstance::OnDeathStarted(AActor* OwningActor)`
- L243: `void UHodgeWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L249: `double UHodgeWeaponInstance::GetPresentationTime() const`
- L256: `bool UHodgeWeaponInstance::CanDrivePresentation() const`
- L264: `UHodgeWeaponInstance* UHodgeWeaponInstance::ResolvePresentationWeapon(APawn* Pawn, UObject* SourceObject)`
- L284: `void UHodgeWeaponInstance::InitializePresentation()`
- L300: `void UHodgeWeaponInstance::ClearPresentationTimer()`
- L305: `void UHodgeWeaponInstance::ShutdownPresentation()`
- L317: `FGuid UHodgeWeaponInstance::AcquireHandUse(FGuid ExecutionId, int32 EventIndex, int32 ActivationKey)`
- L343: `void UHodgeWeaponInstance::ReleaseHandUse(FGuid Handle)`
- L350: `void UHodgeWeaponInstance::ReleaseHandUsesForExecution(const FGuid& ExecutionId)`
- L357: `void UHodgeWeaponInstance::BeginIdlePresentation()`
- L363: `void UHodgeWeaponInstance::SchedulePresentationPhase(EHodgeWeaponPresentationPhase Phase, float Seconds)`
- L378: `void UHodgeWeaponInstance::SetPresentationPhase(EHodgeWeaponPresentationPhase Phase, int32 ActivationKey)`
- L406: `FHodgeWeaponPresentationState UHodgeWeaponInstance::GetPresentationState() const`
- L412: `void UHodgeWeaponInstance::OnRep_PresentationState()`
- L426: `void UHodgeWeaponInstance::RejectPredictedHandUse(int32 ActivationKey)`
- L441: `void UHodgeWeaponInstance::RefreshPresentationActors()`
- L447: `void UHodgeWeaponInstance::OnSpawnedActorsChanged()`
- L453: `void UHodgeWeaponInstance::BeginDestroy()`
- L459: `void UHodgeWeaponInstance::UpdateOwnerPosePolicy()`

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
  18: USTRUCT()
  19: struct FHodgeEquipmentActorToSpawn
  20: {
  21: 	GENERATED_BODY()
  24: 	FHodgeEquipmentActorToSpawn()
  25: 	{
  26: 	}
  29: 	UPROPERTY(EditAnywhere, Category=Equipment)
  30: 	TSubclassOf<AActor> ActorToSpawn;
  33: 	UPROPERTY(EditAnywhere, Category=Equipment)
  34: 	FName AttachSocket;
  37: 	UPROPERTY(EditAnywhere, Category=Equipment)
  38: 	FTransform AttachTransform;
  39: };
  50: UCLASS(Blueprintable, Const, Abstract, BlueprintType)
  51: class UHodgeEquipmentDefinition : public UObject
  52: {
  53: 	GENERATED_BODY()
  55: public:
  57: 	UHodgeEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  62: 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
  63: 	TSubclassOf<UHodgeEquipmentInstance> InstanceType;
  68: 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
  69: 	TArray<TObjectPtr<const UHodgeAbilitySet>> AbilitySetsToGrant;
  74: 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
  75: 	TArray<FHodgeEquipmentActorToSpawn> ActorsToSpawn;
  76: };
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
  23: UCLASS(BlueprintType, Blueprintable)
  24: class UHodgeEquipmentInstance : public UObject
  25: {
  26: 	GENERATED_BODY()
  28: public:
  30: 	UHodgeEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  35: 	virtual bool IsSupportedForNetworking() const override { return true; }
  38: 	virtual UWorld* GetWorld() const override final;
  43: 	UFUNCTION(BlueprintPure, Category=Equipment)
  44: 	UObject* GetInstigator() const { return Instigator; }
  47: 	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }
  50: 	UFUNCTION(BlueprintPure, Category=Equipment)
  51: 	APawn* GetPawn() const;
  54: 	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
  55: 	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;
  58: 	UFUNCTION(BlueprintPure, Category=Equipment)
  59: 	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }
  62: 	virtual void SpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn);
  65: 	virtual void DestroyEquipmentActors();
  68: 	virtual void OnEquipped();
  71: 	virtual void OnUnequipped();
  73: protected:
  74: 	virtual void OnSpawnedActorsChanged() {}
  75: #if UE_WITH_IRIS
  80: 	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
  81: 	                                          UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
  83: #endif
  86: 	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
  87: 	void K2_OnEquipped();
  90: 	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
  91: 	void K2_OnUnequipped();
  93: private:
  95: 	UFUNCTION()
  96: 	void OnRep_Instigator();
  97: 	UFUNCTION()
  98: 	void OnRep_SpawnedActors();
 100: private:
 102: 	UPROPERTY(ReplicatedUsing=OnRep_Instigator)
 103: 	TObjectPtr<UObject> Instigator;
 106: 	UPROPERTY(ReplicatedUsing=OnRep_SpawnedActors)
 107: 	TArray<TObjectPtr<AActor>> SpawnedActors;
 108: };
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
  69: };
  74: USTRUCT(BlueprintType)
  75: struct FHodgeEquipmentList : public FFastArraySerializer
  76: {
  77: 	GENERATED_BODY()
  80: 	FHodgeEquipmentList()
  81: 		: OwnerComponent(nullptr)
  82: 	{
  83: 	}
  86: 	FHodgeEquipmentList(UActorComponent* InOwnerComponent)
  87: 		: OwnerComponent(InOwnerComponent)
  88: 	{
  89: 	}
  91: public:
  95: 	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
  98: 	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
 101: 	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
 106: 	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
 107: 	{
 108: 		return FFastArraySerializer::FastArrayDeltaSerialize<FHodgeAppliedEquipmentEntry, FHodgeEquipmentList>(
 109: 			Entries, DeltaParms, *this);
 110: 	}
 113: 	UHodgeEquipmentInstance* AddEntry(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition);
 116: 	void RemoveEntry(UHodgeEquipmentInstance* Instance);
 118: private:
 120: 	UHodgeAbilitySystemComponent* GetAbilitySystemComponent() const;
 123: 	friend UHodgeEquipmentManagerComponent;
 125: private:
 129: 	UPROPERTY()
 130: 	TArray<FHodgeAppliedEquipmentEntry> Entries;
 133: 	UPROPERTY(NotReplicated)
 134: 	TObjectPtr<UActorComponent> OwnerComponent;
 135: };
 138: template <>
 139: struct TStructOpsTypeTraits<FHodgeEquipmentList> : public TStructOpsTypeTraitsBase2<FHodgeEquipmentList>
 140: {
 141: 	enum { WithNetDeltaSerializer = true };
 142: };
 151: UCLASS(BlueprintType, Const)
 152: class HODGEPODGE_API UHodgeEquipmentManagerComponent : public UPawnComponent
 153: {
 154: 	GENERATED_BODY()
 156: public:
 158: 	UHodgeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 161: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
 162: 	UHodgeEquipmentInstance* EquipItem(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition);
 165: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
 166: 	void UnequipItem(UHodgeEquipmentInstance* ItemInstance);
 171: 	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
 172: 	                                 FReplicationFlags* RepFlags) override;
 181: 	virtual void InitializeComponent() override;
 184: 	virtual void UninitializeComponent() override;
 187: 	virtual void ReadyForReplication() override;
 194: 	UFUNCTION(BlueprintCallable, BlueprintPure)
 195: 	UHodgeEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UHodgeEquipmentInstance> InstanceType);
 200: 	UFUNCTION(BlueprintCallable, BlueprintPure)
 201: 	TArray<UHodgeEquipmentInstance*> GetEquipmentInstancesOfType(
 202: 		TSubclassOf<UHodgeEquipmentInstance> InstanceType) const;
 205: 	template <typename T>
 206: 	T* GetFirstInstanceOfType()
 207: 	{
 208: 		return (T*)GetFirstInstanceOfType(T::StaticClass());
 209: 	}
 211: private:
 213: 	UPROPERTY(Replicated)
 214: 	FHodgeEquipmentList EquipmentList;
 215: };
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
  34: UCLASS()
  35: class HODGEPODGE_API UHodgeWeaponInstance : public UHodgeEquipmentInstance
  36: {
  37: 	GENERATED_BODY()
  39: public:
  41: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="SourceTag"))
  42: 	TArray<FHodgeHitSource> HitSources;
  44: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Presentation")
  45: 	TObjectPtr<UHodgeWeaponPresentationProfile> PresentationProfile;
  46: 	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
  47: 	UHodgeWeaponPresentationProfile* GetPresentationProfile() const { return PresentationProfile; }
  48: 	UFUNCTION(BlueprintCallable, Category="Hodge|Presentation")
  49: 	FGuid AcquireHandUse(FGuid ExecutionId, int32 EventIndex, int32 ActivationKey = 0);
  50: 	UFUNCTION(BlueprintCallable, Category="Hodge|Presentation")
  51: 	void ReleaseHandUse(FGuid Handle);
  52: 	void ReleaseHandUsesForExecution(const FGuid& ExecutionId);
  53: 	void RejectPredictedHandUse(int32 ActivationKey);
  54: 	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
  55: 	int32 GetHandUseCount() const { return HandRequests.Num(); }
  56: 	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
  57: 	FHodgeWeaponPresentationState GetPresentationState() const;
  58: 	double GetPresentationTime() const;
  59: 	void RefreshPresentationActors();
  60: 	static UHodgeWeaponInstance* ResolvePresentationWeapon(APawn* Pawn, UObject* SourceObject = nullptr);
  63: 	UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  64: 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
  69: 	virtual void OnEquipped() override;
  72: 	virtual void OnUnequipped() override;
  77: 	UFUNCTION(BlueprintCallable)
  78: 	void UpdateFiringTime();
  83: 	UFUNCTION(BlueprintPure)
  84: 	float GetTimeSinceLastInteractedWith() const;
  86: protected:
  87: 	virtual void OnSpawnedActorsChanged() override;
  88: 	virtual void BeginDestroy() override;
 104: 	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Input Devices")
 105: 	TArray<TObjectPtr<UInputDeviceProperty>> ApplicableDeviceProperties;
 110: 	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
 111: 	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;
 116: 	UFUNCTION(BlueprintCallable)
 117: 	const FPlatformUserId GetOwningUserId() const;
 122: 	UFUNCTION()
 123: 	void OnDeathStarted(AActor* OwningActor);
 133: 	void ApplyDeviceProperties();
 138: 	void RemoveDeviceProperties();
 140: private:
 141: 	friend struct FHodgeWeaponPresentationTestAccess;
 142: 	struct FHandRequest { FGuid ExecutionId; int32 EventIndex = INDEX_NONE; int32 ActivationKey = 0; };
 143: 	TMap<FGuid, FHandRequest> HandRequests;
 144: 	FTimerHandle PresentationTimer;
 145: 	bool bPresentationEquipped = false;
 146: 	bool bPresentationDisabled = false;
 147: 	bool bPredictedPresentation = false;
 148: 	FHodgeWeaponPresentationState LocalPresentation;
 149: 	UPROPERTY(ReplicatedUsing=OnRep_PresentationState)
 150: 	FHodgeWeaponPresentationState ReplicatedPresentation;
 151: 	UFUNCTION() void OnRep_PresentationState();
 152: 	void InitializePresentation();
 153: 	void ShutdownPresentation();
 154: 	void SetPresentationPhase(EHodgeWeaponPresentationPhase Phase, int32 ActivationKey);
 155: 	void SchedulePresentationPhase(EHodgeWeaponPresentationPhase Phase, float Seconds);
 156: 	void BeginIdlePresentation();
 157: 	bool CanDrivePresentation() const;
 158: 	void ClearPresentationTimer();
 159: 	void UpdateOwnerPosePolicy();
 160: 	TWeakObjectPtr<USkeletalMeshComponent> PoseMesh;
 161: 	uint8 SavedPosePolicy = 0;
 162: 	bool bPosePolicyOverridden = false;
 166: 	UPROPERTY(Transient)
 167: 	TSet<FInputDevicePropertyHandle> DevicePropertyHandles;
 170: 	double TimeLastEquipped = 0.0;
 173: 	double TimeLastFired = 0.0;
 174: };
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
