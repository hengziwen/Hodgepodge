/** @file HodgeEquipmentInstance.h
 * @brief 声明装备运行实例相关的类型、接口与现有处理流程。
 */
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
	/** 构造装备运行实例并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface

	// 表示该 UObject 支持网络复制，可作为网络同步的运行时装备对象。
	/** 判断Supported关联Networking。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool IsSupportedForNetworking() const override { return true; }

	// 获取该装备实例所属的 UWorld。
	/** 获取世界。
	 * @return 世界的当前值或引用。
	 */
	virtual UWorld* GetWorld() const override final;

	//~End of UObject interface

	// 获取创建或赋予这件装备的来源对象。
	/** 获取Instigator。
	 * @return Instigator的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category=Equipment)
	UObject* GetInstigator() const { return Instigator; }

	// 设置创建或赋予这件装备的来源对象。
	/** 设置Instigator。
	 * @param InInstigator 发起本次行为的对象。
	 */
	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	// 获取当前装备实例所属的 Pawn。
	/** 获取Pawn角色。
	 * @return Pawn角色的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category=Equipment)
	APawn* GetPawn() const;
	/** 获取装备等级。
	 * @return 装备等级的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") int32 GetEquipmentLevel() const { return EquipmentLevel; }
	/** 获取装备标识。
	 * @return 装备标识的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Attributes") FGuid GetEquipmentId() const { return EquipmentId; }
	/** 设置该装备实例的身份、等级与属性配置。
	 * @param Id 当前记录的唯一标识。
	 * @param Level 配置或运行实例的等级。
	 * @param Profile 当前操作使用的配置对象。
	 */
	void SetStatIdentity(FGuid Id, int32 Level, const UHodgeEquipmentStatProfile* Profile);
	/** 获取数值配置。
	 * @return 数值配置的当前值或引用。
	 */
	const UHodgeEquipmentStatProfile* GetStatProfile() const { return StatProfile; }

	// 获取指定 Pawn 类型的所属 Pawn，DeterminesOutputType 让蓝图输出类型跟随 PawnType。
	/** 获取TypedPawn角色。
	 * @param PawnType Pawn角色类型，由调用方传入。
	 * @return TypedPawn角色的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	// 获取这件装备在 Pawn 身上生成的所有 Actor。
	/** 获取SpawnedActors。
	 * @return SpawnedActors的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category=Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	// 根据装备定义中的 Actor 配置生成并挂接装备 Actor。
	/** 根据装备配置创建并挂接表现Actor。
	 * @param ActorsToSpawn Actors目标生成，由调用方传入。
	 */
	virtual void SpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn);
	/** 按装备配置生成与挂接Actor，并报告创建是否成功。
	 * @param ActorsToSpawn Actors目标生成，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool TrySpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn);

	// 销毁当前装备实例生成的所有装备 Actor。
	/** 销毁装备Actors。 */
	virtual void DestroyEquipmentActors();

	// 装备生效时调用的生命周期函数。
	/** 处理Equipped。 */
	virtual void OnEquipped();

	// 装备被卸下时调用的生命周期函数。
	/** 处理Unequipped。 */
	virtual void OnUnequipped();

protected:
	/** 处理SpawnedActors变化。 */
	virtual void OnSpawnedActorsChanged() {}
#if UE_WITH_IRIS

	/** Register all replication fragments */

	// Iris 网络复制启用时，为该装备实例注册复制 Fragment。
	/** 为Iris复制登记本对象的属性片段。
	 * @param Context 本次操作的上下文。
	 * @param RegistrationFlags 注册标记，由调用方传入。
	 */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
	                                          UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;

#endif // UE_WITH_IRIS

	// 提供给蓝图实现的装备完成事件，由 C++ OnEquipped 调用。
	/** 触发蓝图Equipped。 */
	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
	void K2_OnEquipped();

	// 提供给蓝图实现的卸装事件，由 C++ OnUnequipped 调用。
	/** 触发蓝图Unequipped。 */
	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
	void K2_OnUnequipped();

private:
	// Instigator 通过网络复制发生变化时调用的 RepNotify。
	/** 收到Instigator复制后更新本地状态。 */
	UFUNCTION()
	void OnRep_Instigator();
	/** 收到SpawnedActors复制后更新本地状态。 */
	UFUNCTION()
	void OnRep_SpawnedActors();

private:
	// 创建或赋予这件装备的来源对象，并通过网络复制给客户端。
	UPROPERTY(ReplicatedUsing=OnRep_Instigator)
	TObjectPtr<UObject> Instigator;
	// 装备标识的数据或对象引用。
	UPROPERTY(Replicated) FGuid EquipmentId;
	// 装备等级的数值或阈值。
	UPROPERTY(Replicated) int32 EquipmentLevel = 1;
	// 属性曲线与初始化效果配置。
	UPROPERTY(Replicated) TObjectPtr<const UHodgeEquipmentStatProfile> StatProfile;

	// 当前装备实例生成的装备 Actor 列表，并通过网络进行复制。
	UPROPERTY(ReplicatedUsing=OnRep_SpawnedActors)
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
