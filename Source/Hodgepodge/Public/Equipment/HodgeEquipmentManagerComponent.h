/** @file HodgeEquipmentManagerComponent.h
 * @brief 声明装备管理组件相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// 装备系统通过 AbilitySet 给 Pawn 的 ASC 授予能力，并使用 GrantedHandles 记录授予结果。
#include "Data/HodgeAbilitySet.h"

// 装备管理器属于 Pawn 的模块化组件。
#include "Components/PawnComponent.h"

// 提供 FastArray 增量复制，用于高效同步装备列表的新增、修改和删除。
#include "Net/Serialization/FastArraySerializer.h"

#include "HodgeEquipmentManagerComponent.generated.h"

class UActorComponent;
class UHodgeAbilitySystemComponent;
class UHodgeEquipmentDefinition;
class UHodgeEquipmentInstance;
class UHodgeEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FHodgeEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment */

// 表示装备列表中的一条已装备记录，并作为 FastArray 的单个复制元素。
USTRUCT(BlueprintType)
struct FHodgeAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	// 默认构造一条装备记录。
	/** 构造已应用装备条目并初始化其默认状态。 */
	FHodgeAppliedEquipmentEntry()
	{
	}

	// 返回当前装备记录的调试字符串。
	/** 获取调试String。
	 * @return 调试String的当前值或引用。
	 */
	FString GetDebugString() const;

private:
	// 允许装备列表直接访问当前 Entry 的私有运行时数据。
	friend FHodgeEquipmentList;

	// 允许装备管理组件直接访问当前 Entry 的私有运行时数据。
	friend UHodgeEquipmentManagerComponent;

	// The equipment class that got equipped

	// 当前装备记录对应的 EquipmentDefinition 类型。
	UPROPERTY()
	TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition;

	// 当前装备记录实际创建出来的运行时 EquipmentInstance。
	UPROPERTY()
	TObjectPtr<UHodgeEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles

	// 仅服务器保存的 AbilitySet 授予句柄，用于卸装时精确撤销这件装备授予的 GAS 内容。
	UPROPERTY(NotReplicated)
	FHodgeAbilitySet_GrantedHandles GrantedHandles;

	// 仅服务器记录原始 ASC，Pawn 解绑后仍可准确撤销授予。
	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UHodgeAbilitySystemComponent> GrantedAbilitySystem;
	// 本来源应用的属性效果句柄。
	UPROPERTY(NotReplicated) FActiveGameplayEffectHandle AttributeHandle;
};

/** List of applied equipment */

// 保存 Pawn 当前所有已装备项目，并通过 FastArray 实现增量网络复制。
USTRUCT(BlueprintType)
struct FHodgeEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	// 默认构造装备列表，此时还没有绑定所属组件。
	/** 构造装备列表并初始化其默认状态。 */
	FHodgeEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	// 构造装备列表并记录负责管理该列表的组件。
	/** 构造装备列表并初始化其默认状态。
	 * @param InOwnerComponent 所属对象组件，由调用方传入。
	 */
	FHodgeEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract

	// 客户端收到装备条目删除复制通知时调用，用于处理删除前的装备生命周期。
	/** 在条目复制移除前解除该条目的本地依赖。
	 * @param RemovedIndices RemovedIndices，由调用方传入。
	 * @param FinalSize 最终大小，由调用方传入。
	 */
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);

	// 客户端收到新装备条目复制完成后调用，用于处理新增装备生命周期。
	/** 在新增条目复制到达后接入该条目。
	 * @param AddedIndices AddedIndices，由调用方传入。
	 * @param FinalSize 最终大小，由调用方传入。
	 */
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);

	// 客户端收到已有装备条目发生变化后调用。
	/** 在条目复制变化到达后更新本地表现或状态。
	 * @param ChangedIndices 变化Indices，由调用方传入。
	 * @param FinalSize 最终大小，由调用方传入。
	 */
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	//~End of FFastArraySerializer contract

	// 使用 FastArrayDeltaSerialize 对装备列表执行增量网络序列化。
	/** 通过FastArray机制读取或写入条目增量。
	 * @param DeltaParms 增量Parms，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FHodgeAppliedEquipmentEntry, FHodgeEquipmentList>(
			Entries, DeltaParms, *this);
	}

	// 根据 EquipmentDefinition 创建一条新的已装备记录，并返回对应运行时 EquipmentInstance。
	/** 添加条目。
	 * @param EquipmentDefinition 装备静态定义，由调用方传入。
	 * @param Id 当前记录的唯一标识。
	 * @param Level 配置或运行实例的等级。
	 * @return 本次计算、查询或创建得到的装备运行实例。
	 */
	UHodgeEquipmentInstance* AddEntry(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition, FGuid Id, int32 Level);

	// 根据运行时 EquipmentInstance 找到并移除对应的装备记录。
	/** 移除条目。
	 * @param Instance Instance，由调用方传入。
	 */
	void RemoveEntry(UHodgeEquipmentInstance* Instance);

private:
	// 获取当前 Pawn 使用的 Hodge AbilitySystemComponent。
	/** 获取能力系统组件。
	 * @return 能力系统组件的当前值或引用。
	 */
	UHodgeAbilitySystemComponent* GetAbilitySystemComponent() const;

	// 允许装备管理组件直接访问装备列表内部数据。
	friend UHodgeEquipmentManagerComponent;

private:
	// Replicated list of equipment entries

	// 当前 Pawn 已装备的所有装备记录，由 FastArray 负责增量复制。
	UPROPERTY()
	TArray<FHodgeAppliedEquipmentEntry> Entries;

	// 当前装备列表所属的管理组件，仅用于本地运行时访问，不参与网络复制。
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

// 告诉 UE FHodgeEquipmentList 提供了自定义 NetDeltaSerialize，用于启用 FastArray 增量复制。
template <>
/** 保存TStructOps类型TraitsFHodge装备列表的数据和接口契约。 */
struct TStructOpsTypeTraits<FHodgeEquipmentList> : public TStructOpsTypeTraitsBase2<FHodgeEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};


/**
 * Manages equipment applied to a pawn
 *
 * 管理应用到 Pawn 身上的所有装备。
 * 负责装备和卸装、装备列表复制、EquipmentInstance 子对象复制以及装备实例查询。
 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	// 构造 Pawn 的装备管理组件。
	/** 构造装备管理组件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 在服务器上根据 EquipmentDefinition 为 Pawn 装备一件物品，并返回创建的运行时实例。
	/** 装备Item。
	 * @param EquipmentDefinition 装备静态定义，由调用方传入。
	 * @return 本次计算、查询或创建得到的装备运行实例。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UHodgeEquipmentInstance* EquipItem(TSubclassOf<UHodgeEquipmentDefinition> EquipmentDefinition);
	/** 按已有物品身份和等级创建装备并提交其授予。
	 * @param Definition 当前操作使用的定义配置。
	 * @param Id 当前记录的唯一标识。
	 * @param Level 配置或运行实例的等级。
	 * @return 本次计算、查询或创建得到的装备运行实例。
	 */
	UHodgeEquipmentInstance* EquipItemWithState(TSubclassOf<UHodgeEquipmentDefinition> Definition, FGuid Id, int32 Level);
	/** 设置装备等级。
	 * @param Instance Instance，由调用方传入。
	 * @param Level 配置或运行实例的等级。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Hodge|Attributes") bool SetEquipmentLevel(UHodgeEquipmentInstance* Instance, int32 Level);
	/** 查找InstanceOf定义。
	 * @param Definition 当前操作使用的定义配置。
	 * @return InstanceOf定义的当前值或引用。
	 */
	UHodgeEquipmentInstance* FindInstanceOfDefinition(TSubclassOf<UHodgeEquipmentDefinition> Definition) const;
	/** 校验装备关联授予。
	 * @param Definition 当前操作使用的定义配置。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 * @param Level 配置或运行实例的等级。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @param bForInitialization 是否关联初始化的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	static bool ValidateEquipmentForGrant(TSubclassOf<UHodgeEquipmentDefinition> Definition,
		const UHodgeAbilitySystemComponent* ASC, int32 Level, TArray<FText>& Errors, bool bForInitialization = false);
	/** 判断装备就绪。
	 * @param Instance Instance，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsEquipmentReady(const UHodgeEquipmentInstance* Instance) const;
	/** 获取上次装备错误。
	 * @return 上次装备错误的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Equipment") FString GetLastEquipError() const { return LastEquipError; }
	/** 查询本次失败授予是否已经产生不可撤销副作用。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool DidEquipHaveIrreversibleEffects() const { return bEquipIrreversibleEffects; }

	// 在服务器上卸下指定的运行时装备实例。
	/** 撤销该装备实例的能力与效果并清理表现Actor。
	 * @param ItemInstance ItemInstance，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UHodgeEquipmentInstance* ItemInstance);

	//~UObject interface

	// 通过 ActorChannel 手动复制当前装备列表中的 EquipmentInstance 子对象。
	/** 通过传统ActorChannel复制本对象持有的网络子对象。
	 * @param Channel 当前Actor的网络复制通道。
	 * @param Bunch 本次复制写入的网络数据包。
	 * @param RepFlags 本次子对象复制使用的网络标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
	                                 FReplicationFlags* RepFlags) override;

	//~End of UObject interface

	//~UActorComponent interface

	//virtual void EndPlay() override;

	// 组件初始化时执行装备管理器需要的初始化工作。
	/** 初始化组件。 */
	virtual void InitializeComponent() override;

	// 组件反初始化时执行装备管理器的清理工作。
	/** 解除初始化组件。 */
	virtual void UninitializeComponent() override;

	// 组件已经具备网络复制条件时调用，用于注册需要复制的装备子对象。
	/** 在组件可复制时注册已有网络子对象。 */
	virtual void ReadyForReplication() override;

	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */

	// 返回第一个属于指定 EquipmentInstance 类型的已装备实例，没有找到时返回 nullptr。
	/** 获取FirstInstanceOf类型。
	 * @param InstanceType Instance类型，由调用方传入。
	 * @return FirstInstanceOf类型的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UHodgeEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UHodgeEquipmentInstance> InstanceType);

	/** Returns all equipped instances of a given type, or an empty array if none are found */

	// 返回所有属于指定 EquipmentInstance 类型的已装备实例，没有找到时返回空数组。
	/** 获取装备InstancesOf类型。
	 * @param InstanceType Instance类型，由调用方传入。
	 * @return 装备InstancesOf类型的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UHodgeEquipmentInstance*> GetEquipmentInstancesOfType(
		TSubclassOf<UHodgeEquipmentInstance> InstanceType) const;

	// C++ 模板版本的实例查询接口，通过 T::StaticClass() 自动确定需要查询的类型。
	template <typename T>
	/** 获取FirstInstanceOf类型。
	 * @return FirstInstanceOf类型的当前值或引用。
	 */
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	// 当前 Pawn 的已装备列表，通过 FastArray 增量复制到客户端。
	friend struct FHodgeEquipmentMutationScope;
	friend struct FHodgeEquipmentList;
	// 上次装备错误的数据或对象引用。
	UPROPERTY(Transient) FString LastEquipError;
	// 控制是否装备IrreversibleEffects的状态标记。
	bool bEquipIrreversibleEffects = false;
	// 控制是否装备Mutation的状态标记。
	bool bEquipmentMutation = false;
	// 控制是否待处理Uninitialize的状态标记。
	bool bPendingUninitialize = false;
	// 装备列表的数据或对象引用。
	UPROPERTY(Replicated)
	FHodgeEquipmentList EquipmentList;
};
