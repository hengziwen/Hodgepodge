// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/GameplayTagStack.h"

#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagStack)

//////////////////////////////////////////////////////////////////////
// FGameplayTagStack

// 返回 TagStack 的调试字符串，例如 "State.Buff.Attackx3"。
FString FGameplayTagStack::GetDebugString() const
{
	return FString::Printf(TEXT("%sx%d"), *Tag.ToString(), StackCount);
}

//////////////////////////////////////////////////////////////////////
// FGameplayTagStackContainer

// 增加指定 GameplayTag 的堆叠数量，如果 Tag 不存在则创建新的 TagStack。
void FGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
	// 无效 Tag 不能添加到容器中。
	if (!Tag.IsValid())
	{
		// 向 Blueprint/Kismet 输出警告，提示调用者传入了无效 Tag。
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to AddStack"), ELogVerbosity::Warning);
		return;
	}

	// 只处理大于 0 的堆叠数量。
	if (StackCount > 0)
	{
		// 遍历已有的 TagStack，查找是否已经存在相同 Tag。
		for (FGameplayTagStack& Stack : Stacks)
		{
			// 找到相同 Tag 时直接增加已有堆叠数量。
			if (Stack.Tag == Tag)
			{
				// 计算增加后的新堆叠数量。
				const int32 NewCount = Stack.StackCount + StackCount;

				// 更新 TagStack 中实际保存的堆叠数量。
				Stack.StackCount = NewCount;

				// 同步更新本地快速查询缓存。
				TagToCountMap[Tag] = NewCount;

				// 标记当前数组元素发生变化，FastArray 下次复制时只同步该元素。
				MarkItemDirty(Stack);

				return;
			}
		}

		// 没有找到相同 Tag 时，在数组中创建新的 TagStack。
		FGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);

		// 标记新添加的数组元素发生变化，使其参与 FastArray 网络复制。
		MarkItemDirty(NewStack);

		// 将新 Tag 和堆叠数量加入本地快速查询缓存。
		TagToCountMap.Add(Tag, StackCount);
	}
}

// 移除指定 GameplayTag 的指定堆叠数量，数量归零后删除整个 TagStack。
void FGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	// 无效 Tag 不能从容器中移除。
	if (!Tag.IsValid())
	{
		// 向 Blueprint/Kismet 输出警告，提示调用者传入了无效 Tag。
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to RemoveStack"), ELogVerbosity::Warning);
		return;
	}

	// TODO：尝试移除不存在的 Tag 或移除数量超过当前数量时是否应该输出错误。
	if (StackCount > 0)
	{
		// 使用迭代器遍历数组，因为找到目标后可能需要直接删除数组元素。
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FGameplayTagStack& Stack = *It;

			// 找到目标 GameplayTag。
			if (Stack.Tag == Tag)
			{
				// 当前堆叠数量小于等于要移除的数量时，直接删除整个 TagStack。
				if (Stack.StackCount <= StackCount)
				{
					// 从 Stacks 数组中移除当前元素。
					It.RemoveCurrent();

					// 从本地快速查询缓存中移除该 Tag。
					TagToCountMap.Remove(Tag);

					// 标记整个数组发生结构变化，因为数组元素被删除了。
					MarkArrayDirty();
				}
				else
				{
					// 当前堆叠数量仍然大于 0，因此只减少对应数量。
					const int32 NewCount = Stack.StackCount - StackCount;

					// 更新 TagStack 中实际保存的堆叠数量。
					Stack.StackCount = NewCount;

					// 同步更新本地快速查询缓存。
					TagToCountMap[Tag] = NewCount;

					// 标记当前数组元素发生变化，只需要增量同步该元素。
					MarkItemDirty(Stack);
				}

				return;
			}
		}
	}
}

// 客户端收到网络复制数据并删除 TagStack 后调用，用于同步本地查询缓存。
void FGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	// 遍历本次网络同步中被删除的所有数组元素。
	for (int32 Index : RemovedIndices)
	{
		// 获取即将被移除的 Tag。
		const FGameplayTag Tag = Stacks[Index].Tag;

		// 从本地快速查询缓存中移除对应 Tag。
		TagToCountMap.Remove(Tag);
	}
}

// 客户端收到网络复制数据并新增 TagStack 后调用，用于同步本地查询缓存。
void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	// 遍历本次网络同步中新增的所有数组元素。
	for (int32 Index : AddedIndices)
	{
		// 获取刚刚同步到客户端的 TagStack。
		const FGameplayTagStack& Stack = Stacks[Index];

		// 将新 Tag 和堆叠数量加入本地快速查询缓存。
		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
	}
}

// 客户端收到网络复制数据并修改 TagStack 后调用，用于同步本地查询缓存。
void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	// 遍历本次网络同步中发生变化的所有数组元素。
	for (int32 Index : ChangedIndices)
	{
		// 获取同步后的 TagStack。
		const FGameplayTagStack& Stack = Stacks[Index];

		// 使用最新的堆叠数量更新本地快速查询缓存。
		TagToCountMap[Stack.Tag] = Stack.StackCount;
	}
}
