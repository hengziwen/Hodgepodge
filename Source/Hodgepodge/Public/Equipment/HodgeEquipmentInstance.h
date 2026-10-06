// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// 提供 UWorld 定义，用于让装备实例能够通过 GetWorld() 获取所属世界。
#include "Engine/World.h"

#include "HodgeEquipmentInstance.generated.h"

class AActor;
class APawn;
struct FFrame;
struct FHodgeEquipmentActorToSpawn;
class UHodgeEquipmentStatProfile;

/**
 * UHodgeEquipmentInstance
 *
 * A piece of equipment spawned and applied to a pawn
 *
 * 一件已经创建并应用到 Pawn 上的运行时装备实例。
 * 负责保存装备来源、所属 Pawn、生成的装备 Actor，并提供装备和卸装生命周期。
 */
UCLASS(BlueprintType, Blueprintable)
class UHodgeEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	// 构造运行时装备实例。
	UHodgeEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface

	// 表示该 UObject 支持网络复制，可作为网络同步的运行时装备对象。
	virtual bool IsSupportedForNetworking() const override { return true; }

	// 获取该装备实例所属的 UWorld。
	virtual UWorld* GetWorld() const override final;

	//~End of UObject interface

	// 获取创建或赋予这件装备的来源对象。
	UFUNCTION(BlueprintPure, Category=Equipment)
	UObject* GetInstigator() const { return Instigator; }

	// 设置创建或赋予这件装备的来源对象。
	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	// 获取当前装备实例所属的 Pawn。
	UFUNCTION(BlueprintPure, Category=Equipment)
	APawn* GetPawn() const;
	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") int32 GetEquipmentLevel() const { return EquipmentLevel; }
	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") FGuid GetEquipmentId() const { return EquipmentId; }
	void SetStatIdentity(FGuid Id, int32 Level, const UHodgeEquipmentStatProfile* Profile);
	const UHodgeEquipmentStatProfile* GetStatProfile() const { return StatProfile; }

	// 获取指定 Pawn 类型的所属 Pawn，DeterminesOutputType 让蓝图输出类型跟随 PawnType。
	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	// 获取这件装备在 Pawn 身上生成的所有 Actor。
	UFUNCTION(BlueprintPure, Category=Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	// 根据装备定义中的 Actor 配置生成并挂接装备 Actor。
	virtual void SpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn);

	// 销毁当前装备实例生成的所有装备 Actor。
	virtual void DestroyEquipmentActors();

	// 装备生效时调用的生命周期函数。
	virtual void OnEquipped();

	// 装备被卸下时调用的生命周期函数。
	virtual void OnUnequipped();

protected:
	virtual void OnSpawnedActorsChanged() {}
#if UE_WITH_IRIS

	/** Register all replication fragments */

	// Iris 网络复制启用时，为该装备实例注册复制 Fragment。
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
	                                          UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;

#endif // UE_WITH_IRIS

	// 提供给蓝图实现的装备完成事件，由 C++ OnEquipped 调用。
	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
	void K2_OnEquipped();

	// 提供给蓝图实现的卸装事件，由 C++ OnUnequipped 调用。
	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
	void K2_OnUnequipped();

private:
	// Instigator 通过网络复制发生变化时调用的 RepNotify。
	UFUNCTION()
	void OnRep_Instigator();
	UFUNCTION()
	void OnRep_SpawnedActors();

private:
	// 创建或赋予这件装备的来源对象，并通过网络复制给客户端。
	UPROPERTY(ReplicatedUsing=OnRep_Instigator)
	TObjectPtr<UObject> Instigator;
	UPROPERTY(Replicated) FGuid EquipmentId;
	UPROPERTY(Replicated) int32 EquipmentLevel = 1;
	UPROPERTY(Replicated) TObjectPtr<const UHodgeEquipmentStatProfile> StatProfile;

	// 当前装备实例生成的装备 Actor 列表，并通过网络进行复制。
	UPROPERTY(ReplicatedUsing=OnRep_SpawnedActors)
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
