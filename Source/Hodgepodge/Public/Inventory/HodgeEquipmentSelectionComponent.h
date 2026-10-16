// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/HodgeInventoryTypes.h"
#include "HodgeEquipmentSelectionComponent.generated.h"

class UHodgePawnData;
class UHodgeEquipmentInstance;
class UHodgeInventoryManagerComponent;

/** PlayerState 保存单武器选择；实际授予与 Actor 始终归 Pawn EquipmentManager。 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeEquipmentSelectionComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UHodgeEquipmentSelectionComponent(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	/** 初始化只设置一次默认选择，重生不覆盖手动卸装。 */
	FHodgeInventoryOperation InitializeFromPawnData(const UHodgePawnData* Data);
	FHodgeInventoryOperation BindAvatar(APawn* Pawn);
	void UnbindAvatar(APawn* Pawn);
	FHodgeInventoryOperation CanEquipItem(FGuid Id) const;
	FHodgeInventoryOperation RequestEquipItem(FGuid Id);
	FHodgeInventoryOperation RequestUnequip();
	UFUNCTION(BlueprintPure) FGuid GetSelectedItemId() const { return SelectedItemId; }
	UFUNCTION(BlueprintPure) bool IsSelectionApplied() const;
	UFUNCTION(BlueprintPure) APawn* GetBoundAvatar() const { return BoundAvatar.Get(); }
	/** 库存复制晚到时再次发布已经可解析的选择。 */
	void NotifySelectionChanged();
	FHodgeEquipmentSelectionChanged OnEquipmentSelectionChanged;
private:
	UHodgeInventoryManagerComponent* Inventory() const;
	FHodgeInventoryOperation CheckState(bool bManual) const;
	FHodgeInventoryOperation ApplySelection(FGuid Id, bool bManual);
	bool RemoveApplied();
	UFUNCTION() void OnRep_Selection();
	UPROPERTY(ReplicatedUsing=OnRep_Selection) FGuid SelectedItemId;
	UPROPERTY(ReplicatedUsing=OnRep_Selection) bool bApplied = false;
	UPROPERTY(Transient) TObjectPtr<UHodgeEquipmentInstance> AppliedInstance;
	UPROPERTY(Transient) TWeakObjectPtr<APawn> BoundAvatar;
	bool bInitialized = false;
	bool bChanging = false;
	bool bUnbindPending = false;
};
