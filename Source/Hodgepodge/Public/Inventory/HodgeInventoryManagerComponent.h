// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inventory/HodgeInventoryTypes.h"
#include "HodgeInventoryManagerComponent.generated.h"

class UHodgePawnData;
class UHodgeInventoryManagerComponent;

/** 容器条目只保存实例与数量，实例身份和状态由网络 UObject 保存。 */
USTRUCT()
struct FHodgeInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY() TObjectPtr<UHodgeInventoryItemInstance> Instance;
	UPROPERTY() int32 StackCount = 0;
};

/** 沿用 Lyra FastArray；字段延迟到达由实例 PostNetReceive 补发。 */
USTRUCT()
struct FHodgeInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY() TArray<FHodgeInventoryEntry> Entries;
	UPROPERTY(NotReplicated) TObjectPtr<UHodgeInventoryManagerComponent> Owner;
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& Info) { return FastArrayDeltaSerialize<FHodgeInventoryEntry, FHodgeInventoryList>(Entries, Info, *this); }
	void PostReplicatedReceive(const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters);
};
template<> struct TStructOpsTypeTraits<FHodgeInventoryList> : TStructOpsTypeTraitsBase2<FHodgeInventoryList>
{ enum { WithNetDeltaSerializer = true }; };

/** PlayerState 私有容器，无 Tick；所有写操作在服务器预检后提交。 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UHodgeInventoryManagerComponent(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
	virtual bool ReplicateSubobjects(UActorChannel*, FOutBunch*, FReplicationFlags*) override;
	virtual void ReadyForReplication() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	/** 默认批次原子提交，已初始化时不重复发放；旧身份只能在此内部接续。 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) FHodgeInventoryOperation InitializeFromPawnData(const UHodgePawnData* Data);
	UFUNCTION(BlueprintPure) bool IsInitialized() const { return bInitialized; }
	UFUNCTION(BlueprintPure) FGuid GetDefaultSelectedItemId() const { return DefaultSelectedItemId; }
	UFUNCTION(BlueprintPure) TArray<FHodgeInventoryItemView> GetItems() const;
	UFUNCTION(BlueprintPure) UHodgeInventoryItemInstance* FindItemById(FGuid Id) const;
	UFUNCTION(BlueprintPure) int64 GetTotalItemCountByDefinition(TSubclassOf<UHodgeInventoryItemDefinition> Definition) const;
	UFUNCTION(BlueprintPure) int64 GetRevision() const { return Revision; }
	UFUNCTION(BlueprintPure) FHodgeInventoryOperation CanAddItemDefinition(TSubclassOf<UHodgeInventoryItemDefinition> Definition, int32 Count, int32 Level = 1) const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) FHodgeInventoryOperation AddItemDefinition(TSubclassOf<UHodgeInventoryItemDefinition> Definition, int32 Count, int32 Level = 1);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) FHodgeInventoryOperation RemoveItemById(FGuid Id);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) FHodgeInventoryOperation ConsumeItemsByDefinition(TSubclassOf<UHodgeInventoryItemDefinition> Definition, int32 Count);
	/** 复制收敛后通知本地消费者；不允许通知回调重入写入。 */
	void RefreshSnapshot();
	FHodgeInventoryChanged OnInventoryChanged;
private:
	struct FGrant
	{
		TSubclassOf<UHodgeInventoryItemDefinition> Definition;
		int32 Count = 1;
		int32 Level = 1;
		FGuid ExistingId;
		bool bSelect = false;
	};
	FHodgeInventoryOperation CheckWrite() const;
	FHodgeInventoryOperation BuildPlan(const TArray<FGrant>& Grants, int32 Capacity, TArray<FHodgeInventoryEntry>& Plan);
	FHodgeInventoryOperation CommitPlan(TArray<FHodgeInventoryEntry>&& Plan);
	void RegisterItem(UHodgeInventoryItemInstance* Item);
	void UnregisterItem(UHodgeInventoryItemInstance* Item);
	UPROPERTY(Replicated) FHodgeInventoryList InventoryList;
	UPROPERTY(Replicated) bool bInitialized = false;
	UPROPERTY(Replicated) FGuid DefaultSelectedItemId;
	UPROPERTY(Transient) TObjectPtr<const UHodgePawnData> InitializationData;
	UPROPERTY(Transient) TArray<FHodgeInventoryItemView> LastSnapshot;
	/** 构造和片段初始化期间保活候选物品，失败时整批释放。 */
	UPROPERTY(Transient) TArray<TObjectPtr<UHodgeInventoryItemInstance>> PendingItems;
	int32 MaxEntries = 0;
	int64 Revision = 0;
	bool bMutating = false;
	bool bPublishing = false;
};
