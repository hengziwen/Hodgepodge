// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "HodgeInventoryItemDefinition.generated.h"

class UHodgeEquipmentDefinition;
class UHodgeInventoryItemInstance;

/** 沿用 Lyra 内联静态片段；初始化只写物品自身，不授予 GAS 或触发外部效果。 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class HODGEPODGE_API UHodgeInventoryItemFragment : public UObject
{
	GENERATED_BODY()
public:
	virtual void OnInstanceCreated(UHodgeInventoryItemInstance* Instance) const {}
};

/** 只映射装备定义，执行仍由 Pawn 的 EquipmentManager 负责。 */
UCLASS()
class HODGEPODGE_API UHodgeInventoryFragment_EquippableItem : public UHodgeInventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition;
};

/** 物品整数状态的初始值，不与库存数量混用。 */
UCLASS()
class HODGEPODGE_API UHodgeInventoryFragment_SetStats : public UHodgeInventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly) TMap<FGameplayTag, int32> InitialStats;
	virtual void OnInstanceCreated(UHodgeInventoryItemInstance* Instance) const override;
};

/** 复用 Lyra 的蓝图类／CDO 定义形式，实例不保存可变的配置副本。 */
UCLASS(Blueprintable, Const, Abstract)
class HODGEPODGE_API UHodgeInventoryItemDefinition : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FText DisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1")) int32 MaxStackCount = 1;
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly) TArray<TObjectPtr<UHodgeInventoryItemFragment>> Fragments;
	const UHodgeInventoryItemFragment* FindFragmentByClass(TSubclassOf<UHodgeInventoryItemFragment> Type) const;
	template<class T> const T* FindFragment() const { return Cast<T>(FindFragmentByClass(T::StaticClass())); }
	/** 运行时与编辑器共用定义校验，装备等级范围由既有配置决定。 */
	bool Validate(int32 Level, FString& Error) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
