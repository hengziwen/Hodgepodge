/** @file GameplayTagStack.h
 * @brief 声明GameplayTag计数容器相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "GameplayTagStack.generated.h"

// 前置声明：GameplayTagStackContainer 是 TagStack 的所属容器。
struct FGameplayTagStackContainer;

// 前置声明：FastArray 网络增量序列化所需的网络参数。
struct FNetDeltaSerializeInfo;

/**
 * @brief 表示一个 GameplayTag 堆栈，由 Tag 和对应的堆叠数量组成。
 * @note 继承 FFastArraySerializerItem，使单个 TagStack 可以参与 FastArray 增量同步。
 */
USTRUCT(BlueprintType)
struct FGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	// 默认构造函数，Tag 默认为无效 Tag，StackCount 默认为 0。
	/** 构造玩法标签堆叠并初始化其默认状态。 */
	FGameplayTagStack()
	{
	}

	// 使用指定的 Tag 和堆叠数量创建 TagStack。
	/** 构造玩法标签堆叠并初始化其默认状态。
	 * @param InTag 标签，用于对应标签查询或匹配。
	 * @param InStackCount 标签或条目的堆叠数量。
	 */
	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		  , StackCount(InStackCount)
	{
	}

	// 获取当前 TagStack 的调试信息字符串。
	/** 获取调试String。
	 * @return 调试String的当前值或引用。
	 */
	FString GetDebugString() const;

private:
	// 允许 FGameplayTagStackContainer 访问 TagStack 的私有成员。
	friend FGameplayTagStackContainer;

	// 当前堆栈对应的 GameplayTag。
	UPROPERTY()
	FGameplayTag Tag;

	// 当前 GameplayTag 的堆叠数量。
	UPROPERTY()
	int32 StackCount = 0;
};

/**
 * @brief GameplayTagStack 的容器，负责管理多个 Tag 及其堆叠数量。
 * @note 继承 FFastArraySerializer，用于对 TagStack 数组进行高效的网络增量同步。
 */
USTRUCT(BlueprintType)
struct FGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	// 默认构造函数。
	/** 构造玩法标签堆叠Container并初始化其默认状态。 */
	FGameplayTagStackContainer()
	// : Owner(nullptr)
	{
	}

public:
	// 为指定 GameplayTag 增加指定数量的堆叠，StackCount 小于 1 时不执行任何操作。
	/** 添加堆叠。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param StackCount 标签或条目的堆叠数量。
	 */
	void AddStack(FGameplayTag Tag, int32 StackCount);

	// 为指定 GameplayTag 移除指定数量的堆叠，StackCount 小于 1 时不执行任何操作。
	/** 移除堆叠。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param StackCount 标签或条目的堆叠数量。
	 */
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	// 获取指定 GameplayTag 当前的堆叠数量，不存在时返回 0。
	/** 获取堆叠数量。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return 堆叠数量的当前值或引用。
	 */
	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	// 判断指定 GameplayTag 当前是否至少存在一个堆叠。
	/** 查询计数容器中是否存在该Tag。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}

	//~FFastArraySerializer contract

	// 客户端收到网络数据并删除数组元素后调用。
	/** 在条目复制移除前解除该条目的本地依赖。
	 * @param RemovedIndices RemovedIndices，由调用方传入。
	 * @param FinalSize 最终大小，由调用方传入。
	 */
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);

	// 客户端收到网络数据并新增数组元素后调用。
	/** 在新增条目复制到达后接入该条目。
	 * @param AddedIndices AddedIndices，由调用方传入。
	 * @param FinalSize 最终大小，由调用方传入。
	 */
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);

	// 客户端收到网络数据并修改数组元素后调用。
	/** 在条目复制变化到达后更新本地表现或状态。
	 * @param ChangedIndices 变化Indices，由调用方传入。
	 * @param FinalSize 最终大小，由调用方传入。
	 */
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	//~End of FFastArraySerializer contract

	// 使用 FastArrayDeltaSerialize 对 Stacks 执行网络增量序列化。
	/** 通过FastArray机制读取或写入条目增量。
	 * @param DeltaParms 增量Parms，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(
			Stacks, DeltaParms, *this);
	}

private:
	// 实际参与网络复制的 GameplayTagStack 数组。
	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;

	// 用于快速查询 GameplayTag 对应堆叠数量的缓存 Map，不直接参与网络复制。
	TMap<FGameplayTag, int32> TagToCountMap;
};

// 告诉 UE 该结构体使用自定义的网络增量序列化方式。
template <>
/** 保存TStructOps类型TraitsF玩法标签堆叠Container的数据和接口契约。 */
struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
{
	enum
	{
		// 启用 FGameplayTagStackContainer::NetDeltaSerialize() 进行网络序列化。
		WithNetDeltaSerializer = true,
	};
};
