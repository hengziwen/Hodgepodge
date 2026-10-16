// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "AbilitySystem/GameplayTagStack.h"
#include "HodgeInventoryItemInstance.generated.h"

class UHodgeInventoryItemDefinition;
class UHodgeInventoryManagerComponent;

/** 玩家持有的网络 UObject，Outer 为 PlayerState；装备只是该物品的临时执行对象。 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeInventoryItemInstance : public UObject
{
	GENERATED_BODY()
public:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
	virtual void PostNetReceive() override;
#if UE_WITH_IRIS
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext&, UE::Net::EFragmentRegistrationFlags) override;
#endif
	UFUNCTION(BlueprintPure) FGuid GetItemId() const { return ItemId; }
	UFUNCTION(BlueprintPure) int32 GetItemLevel() const { return ItemLevel; }
	UFUNCTION(BlueprintPure) TSubclassOf<UHodgeInventoryItemDefinition> GetItemDef() const { return ItemDefinition; }
	UFUNCTION(BlueprintPure) int32 GetStatTagStackCount(FGameplayTag Tag) const { return StatTags.GetStackCount(Tag); }
	/** 仅未提交的新实例允许片段写入初始 Tag，提交后没有任意改写入口。 */
	void InitializeStatTag(FGameplayTag Tag, int32 Count);
private:
	friend class UHodgeInventoryManagerComponent;
	void Initialize(TSubclassOf<UHodgeInventoryItemDefinition> Definition, FGuid Id, int32 Level);
	UPROPERTY(Replicated) FGuid ItemId;
	UPROPERTY(Replicated) TSubclassOf<UHodgeInventoryItemDefinition> ItemDefinition;
	UPROPERTY(Replicated) int32 ItemLevel = 1;
	UPROPERTY(Replicated) FGameplayTagStackContainer StatTags;
	bool bInitializing = false;
};
