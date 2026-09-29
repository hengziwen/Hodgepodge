# Equipment 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeEquipmentDefinition.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentDefinition.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)

定义候选（多行签名仅展示首行）：

- L11: `UHodgeEquipmentDefinition::UHodgeEquipmentDefinition(const FObjectInitializer& ObjectInitializer)`

## HodgeEquipmentInstance.cpp

模块或基础类型入口。

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
- L147: `void UHodgeEquipmentInstance::DestroyEquipmentActors()`
- L161: `void UHodgeEquipmentInstance::OnEquipped()`
- L168: `void UHodgeEquipmentInstance::OnUnequipped()`
- L175: `void UHodgeEquipmentInstance::OnRep_Instigator()`

## HodgeEquipmentManagerComponent.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeEquipmentManagerComponent.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Equipment/HodgeEquipmentDefinition.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentDefinition.h)、[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)

定义候选（多行签名仅展示首行）：

- L32: `FString FHodgeAppliedEquipmentEntry::GetDebugString() const`
- L41: `void FHodgeEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)`
- L58: `void FHodgeEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)`
- L75: `void FHodgeEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)`
- L85: `UHodgeAbilitySystemComponent* FHodgeEquipmentList::GetAbilitySystemComponent() const`
- L98: `UHodgeEquipmentInstance* FHodgeEquipmentList::AddEntry(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition)`
- L165: `void FHodgeEquipmentList::RemoveEntry(UHodgeEquipmentInstance* Instance)`
- L200: `UHodgeEquipmentManagerComponent::UHodgeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)`
- L212: `void UHodgeEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
- L222: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::EquipItem(`
- L253: `void UHodgeEquipmentManagerComponent::UnequipItem(UHodgeEquipmentInstance* ItemInstance)`
- L273: `bool UHodgeEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch,`
- L298: `void UHodgeEquipmentManagerComponent::InitializeComponent()`
- L305: `void UHodgeEquipmentManagerComponent::UninitializeComponent()`
- L329: `void UHodgeEquipmentManagerComponent::ReadyForReplication()`
- L356: `UHodgeEquipmentInstance* UHodgeEquipmentManagerComponent::GetFirstInstanceOfType(`
- L379: `TArray<UHodgeEquipmentInstance*> UHodgeEquipmentManagerComponent::GetEquipmentInstancesOfType(`

## HodgeWeaponInstance.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp](../../../Source/Hodgepodge/Private/Equipment/HodgeWeaponInstance.cpp)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)

定义候选（多行签名仅展示首行）：

- L32: `UHodgeWeaponInstance::UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer)`
- L57: `void UHodgeWeaponInstance::OnEquipped()`
- L76: `void UHodgeWeaponInstance::OnUnequipped()`
- L86: `void UHodgeWeaponInstance::UpdateFiringTime()`
- L99: `float UHodgeWeaponInstance::GetTimeSinceLastInteractedWith() const`
- L128: `TSubclassOf<UAnimInstance> UHodgeWeaponInstance::PickBestAnimLayer(bool bEquipped,`
- L139: `const FPlatformUserId UHodgeWeaponInstance::GetOwningUserId() const`
- L153: `void UHodgeWeaponInstance::ApplyDeviceProperties()`
- L191: `void UHodgeWeaponInstance::RemoveDeviceProperties()`
- L214: `void UHodgeWeaponInstance::OnDeathStarted(AActor* OwningActor)`

## HodgeEquipmentDefinition.h

模块或基础类型入口。

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

模块或基础类型入口。

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
  74: #if UE_WITH_IRIS
  79: 	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
  80: 	                                          UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
  82: #endif
  85: 	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
  86: 	void K2_OnEquipped();
  89: 	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
  90: 	void K2_OnUnequipped();
  92: private:
  94: 	UFUNCTION()
  95: 	void OnRep_Instigator();
  97: private:
  99: 	UPROPERTY(ReplicatedUsing=OnRep_Instigator)
 100: 	TObjectPtr<UObject> Instigator;
 103: 	UPROPERTY(Replicated)
 104: 	TArray<TObjectPtr<AActor>> SpawnedActors;
 105: };
```

## HodgeEquipmentManagerComponent.h

模块或基础类型入口。

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
  65: };
  70: USTRUCT(BlueprintType)
  71: struct FHodgeEquipmentList : public FFastArraySerializer
  72: {
  73: 	GENERATED_BODY()
  76: 	FHodgeEquipmentList()
  77: 		: OwnerComponent(nullptr)
  78: 	{
  79: 	}
  82: 	FHodgeEquipmentList(UActorComponent* InOwnerComponent)
  83: 		: OwnerComponent(InOwnerComponent)
  84: 	{
  85: 	}
  87: public:
  91: 	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
  94: 	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
  97: 	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
 102: 	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
 103: 	{
 104: 		return FFastArraySerializer::FastArrayDeltaSerialize<FHodgeAppliedEquipmentEntry, FHodgeEquipmentList>(
 105: 			Entries, DeltaParms, *this);
 106: 	}
 109: 	UHodgeEquipmentInstance* AddEntry(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition);
 112: 	void RemoveEntry(UHodgeEquipmentInstance* Instance);
 114: private:
 116: 	UHodgeAbilitySystemComponent* GetAbilitySystemComponent() const;
 119: 	friend UHodgeEquipmentManagerComponent;
 121: private:
 125: 	UPROPERTY()
 126: 	TArray<FHodgeAppliedEquipmentEntry> Entries;
 129: 	UPROPERTY(NotReplicated)
 130: 	TObjectPtr<UActorComponent> OwnerComponent;
 131: };
 134: template <>
 135: struct TStructOpsTypeTraits<FHodgeEquipmentList> : public TStructOpsTypeTraitsBase2<FHodgeEquipmentList>
 136: {
 137: 	enum { WithNetDeltaSerializer = true };
 138: };
 147: UCLASS(BlueprintType, Const)
 148: class HODGEPODGE_API UHodgeEquipmentManagerComponent : public UPawnComponent
 149: {
 150: 	GENERATED_BODY()
 152: public:
 154: 	UHodgeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 157: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
 158: 	UHodgeEquipmentInstance* EquipItem(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition);
 161: 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
 162: 	void UnequipItem(UHodgeEquipmentInstance* ItemInstance);
 167: 	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
 168: 	                                 FReplicationFlags* RepFlags) override;
 177: 	virtual void InitializeComponent() override;
 180: 	virtual void UninitializeComponent() override;
 183: 	virtual void ReadyForReplication() override;
 190: 	UFUNCTION(BlueprintCallable, BlueprintPure)
 191: 	UHodgeEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UHodgeEquipmentInstance> InstanceType);
 196: 	UFUNCTION(BlueprintCallable, BlueprintPure)
 197: 	TArray<UHodgeEquipmentInstance*> GetEquipmentInstancesOfType(
 198: 		TSubclassOf<UHodgeEquipmentInstance> InstanceType) const;
 201: 	template <typename T>
 202: 	T* GetFirstInstanceOfType()
 203: 	{
 204: 		return (T*)GetFirstInstanceOfType(T::StaticClass());
 205: 	}
 207: private:
 209: 	UPROPERTY(Replicated)
 210: 	FHodgeEquipmentList EquipmentList;
 211: };
```

## HodgeWeaponInstance.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeWeaponInstance.h)

项目内直接 include（不是运行调用关系）：[Equipment/HodgeEquipmentInstance.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentInstance.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   9: #include "Equipment/HodgeEquipmentInstance.h"
  12: #include "GameFramework/InputDevicePropertyHandle.h"
  14: #include "HodgeWeaponInstance.generated.h"
  16: class UAnimInstance;
  17: class UObject;
  18: struct FFrame;
  19: struct FGameplayTagContainer;
  20: class UInputDeviceProperty;
  30: UCLASS()
  31: class HODGEPODGE_API UHodgeWeaponInstance : public UHodgeEquipmentInstance
  32: {
  33: 	GENERATED_BODY()
  35: public:
  37: 	UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  42: 	virtual void OnEquipped() override;
  45: 	virtual void OnUnequipped() override;
  50: 	UFUNCTION(BlueprintCallable)
  51: 	void UpdateFiringTime();
  56: 	UFUNCTION(BlueprintPure)
  57: 	float GetTimeSinceLastInteractedWith() const;
  59: protected:
  75: 	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Input Devices")
  76: 	TArray<TObjectPtr<UInputDeviceProperty>> ApplicableDeviceProperties;
  81: 	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
  82: 	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;
  87: 	UFUNCTION(BlueprintCallable)
  88: 	const FPlatformUserId GetOwningUserId() const;
  93: 	UFUNCTION()
  94: 	void OnDeathStarted(AActor* OwningActor);
 104: 	void ApplyDeviceProperties();
 109: 	void RemoveDeviceProperties();
 111: private:
 115: 	UPROPERTY(Transient)
 116: 	TSet<FInputDevicePropertyHandle> DevicePropertyHandles;
 119: 	double TimeLastEquipped = 0.0;
 122: 	double TimeLastFired = 0.0;
 123: };
```
