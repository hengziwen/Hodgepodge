// Copyright Epic Games, Inc. All Rights Reserved.

// GameFeature：动态给指定 Actor 添加 Ability、AttributeSet、AbilitySet 的实现。
#include "GameFeatures/GameFeatureAction_AddAbilities.h"

// GameInstance：用于获取 GameFrameworkComponentManager 子系统。
#include "Engine/GameInstance.h"

// GameFrameworkComponentManager：负责监听 Actor 扩展生命周期，并动态添加组件。
#include "Components/GameFrameworkComponentManager.h"

// 项目自定义的 AbilitySystemComponent。
#include "AbilitySystem/HodgeAbilitySystemComponent.h"

// World：用于判断当前是否为有效的游戏世界。
#include "Engine/World.h"

// PlayerState：用于使用项目自定义的 AbilityReady 扩展事件名。
#include "Core/PlayState/HodgePlayerState.h" //@TODO: For the fname

// GameFeatureAction_WorldActionBase：当前 Action 的父类实现。
#include "GameFeatures/GameFeatureAction_WorldActionBase.h"

#if WITH_EDITOR
// 编辑器下的数据验证功能。
#include "Misc/DataValidation.h"
#endif

// 使用当前 CPP 对应的内联生成代码。
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddAbilities)

// GameFeature 模块使用的本地化文本命名空间。
#define LOCTEXT_NAMESPACE "GameFeatures"

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddAbilities

// GameFeature 开始激活时调用。
void UGameFeatureAction_AddAbilities::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	// 获取当前 GameFeature 激活上下文对应的运行时数据。
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);

	// 如果当前上下文已经残留运行时数据，说明上一次状态没有正常清理。
	if (!ensureAlways(ActiveData.ActiveExtensions.IsEmpty()) ||
		!ensureAlways(ActiveData.ComponentRequests.IsEmpty()))
	{
		// 清理残留的 Ability、AttributeSet、AbilitySet 和组件请求。
		Reset(ActiveData);
	}

	// 调用父类，继续执行 WorldActionBase 的激活流程。
	Super::OnGameFeatureActivating(Context);
}

// GameFeature 开始停用时调用。
void UGameFeatureAction_AddAbilities::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	// 先调用父类的停用逻辑。
	Super::OnGameFeatureDeactivating(Context);

	// 查找当前 GameFeature 激活上下文对应的运行时数据。
	FPerContextData* ActiveData = ContextData.Find(Context);

	// 如果找到了对应的数据，则清理当前 GameFeature 添加的所有内容。
	if (ensure(ActiveData))
	{
		// 移除当前上下文创建的 Ability、AttributeSet、AbilitySet 和组件请求。
		Reset(*ActiveData);
	}
}

#if WITH_EDITOR

// 编辑器中用于检查 GameFeature 配置是否合法。
EDataValidationResult UGameFeatureAction_AddAbilities::IsDataValid(FDataValidationContext& Context) const
{
	// 先执行父类的数据验证，再合并当前 Action 自己的验证结果。
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context),
	                                                            EDataValidationResult::Valid);

	// 当前遍历到的 AbilitiesList 下标。
	int32 EntryIndex = 0;

	// 遍历所有 Actor → Ability/Attribute/AbilitySet 配置。
	for (const FGameFeatureAbilitiesEntry& Entry : AbilitiesList)
	{
		// 每一项必须指定目标 Actor 类型。
		if (Entry.ActorClass.IsNull())
		{
			// 当前配置无效。
			Result = EDataValidationResult::Invalid;

			// 告诉编辑器具体是哪一个 Entry 缺少 ActorClass。
			Context.AddError(FText::Format(
				LOCTEXT("EntryHasNullActor", "Null ActorClass at index {0} in AbilitiesList"),
				FText::AsNumber(EntryIndex)));
		}

		// 如果 Ability、Attribute 和 AbilitySet 全部为空，则这条配置实际上什么都不会做。
		if (Entry.GrantedAbilities.IsEmpty() && Entry.GrantedAttributes.IsEmpty() && Entry.GrantedAbilitySets.IsEmpty())
		{
			// 当前配置无效。
			Result = EDataValidationResult::Invalid;

			// 告诉编辑器具体是哪一个 Entry 没有任何可添加内容。
			Context.AddError(FText::Format(LOCTEXT("EntryHasNoAddOns",
			                                       "Index {0} in AbilitiesList will do nothing (no granted abilities, attributes, or ability sets)"), FText::AsNumber(
				                               EntryIndex)));
		}

		// 当前 Entry 中遍历到的 Ability 下标。
		int32 AbilityIndex = 0;

		// 检查当前 Entry 中配置的所有 GameplayAbility。
		for (const FHodgeAbilityGrant& Ability : Entry.GrantedAbilities)
		{
			// Ability 必须指定具体的 Ability 类。
			if (Ability.AbilityType.IsNull())
			{
				// 当前配置无效。
				Result = EDataValidationResult::Invalid;

				// 告诉编辑器具体是哪一个 Ability 配置为空。
				Context.AddError(FText::Format(
					LOCTEXT("EntryHasNullAbility",
					        "Null AbilityType at index {0} in AbilitiesList[{1}].GrantedAbilities"),
					FText::AsNumber(AbilityIndex), FText::AsNumber(EntryIndex)));
			}

			// 移动到下一个 Ability 配置。
			++AbilityIndex;
		}

		// 当前 Entry 中遍历到的 AttributeSet 下标。
		int32 AttributesIndex = 0;

		// 检查当前 Entry 中配置的所有 AttributeSet。
		for (const FHodgeAttributeSetGrant& Attributes : Entry.GrantedAttributes)
		{
			// AttributeSet 必须指定具体的类。
			if (Attributes.AttributeSetType.IsNull())
			{
				// 当前配置无效。
				Result = EDataValidationResult::Invalid;

				// 告诉编辑器具体是哪一个 AttributeSet 配置为空。
				Context.AddError(FText::Format(
					LOCTEXT("EntryHasNullAttributeSet",
					        "Null AttributeSetType at index {0} in AbilitiesList[{1}].GrantedAttributes"),
					FText::AsNumber(AttributesIndex), FText::AsNumber(EntryIndex)));
			}

			// 移动到下一个 AttributeSet 配置。
			++AttributesIndex;
		}

		// 当前 Entry 中遍历到的 AbilitySet 下标。
		int32 AttributeSetIndex = 0;

		// 检查当前 Entry 中配置的所有 AbilitySet。
		for (const TSoftObjectPtr<const UHodgeAbilitySet>& AttributeSetPtr : Entry.GrantedAbilitySets)
		{
			// AbilitySet 必须指定有效的资源引用。
			if (AttributeSetPtr.IsNull())
			{
				// 当前配置无效。
				Result = EDataValidationResult::Invalid;

				// 告诉编辑器具体是哪一个 AbilitySet 配置为空。
				Context.AddError(FText::Format(
					LOCTEXT("EntryHasNullAttributeSet",
					        "Null AbilitySet at index {0} in AbilitiesList[{1}].GrantedAbilitySets"),
					FText::AsNumber(AttributeSetIndex), FText::AsNumber(EntryIndex)));
			}

			// 移动到下一个 AbilitySet 配置。
			++AttributeSetIndex;
		}

		// 移动到下一个 Actor 配置。
		++EntryIndex;
	}

	// 返回最终的数据验证结果。
	return Result;

	// 这行实际上不会执行，因为上面的 return Result 已经返回。
	return EDataValidationResult::NotValidated;
}
#endif

// GameFeature 被加入某个 World 时调用。
// 这里主要负责向 GameFrameworkComponentManager 注册 Actor 扩展监听。
void UGameFeatureAction_AddAbilities::AddToWorld(const FWorldContext& WorldContext,
                                                 const FGameFeatureStateChangeContext& ChangeContext)
{
	// 获取当前 World。
	UWorld* World = WorldContext.World();

	// 获取当前 World 所属的 GameInstance。
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	// 获取当前 GameFeature 上下文对应的运行时数据。
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// 只在有效 GameInstance、有效 World 且当前是 GameWorld 时执行。
	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		// 获取 GameFrameworkComponentManager 子系统。
		if (UGameFrameworkComponentManager* ComponentMan = UGameInstance::GetSubsystem<
			UGameFrameworkComponentManager>(GameInstance))
		{
			// 当前遍历到的 AbilitiesList 配置下标。
			int32 EntryIndex = 0;

			// 遍历所有 Actor → Ability/Attribute/AbilitySet 配置。
			for (const FGameFeatureAbilitiesEntry& Entry : AbilitiesList)
			{
				// 只有指定了目标 ActorClass 的配置才需要注册监听。
				if (!Entry.ActorClass.IsNull())
				{
					// 创建 Actor 扩展事件回调。
					UGameFrameworkComponentManager::FExtensionHandlerDelegate AddAbilitiesDelegate =
						UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
							this, &UGameFeatureAction_AddAbilities::HandleActorExtension, EntryIndex, ChangeContext);

					// 监听指定 ActorClass 的扩展生命周期。
					TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentMan->AddExtensionHandler(
						Entry.ActorClass, AddAbilitiesDelegate);

					// 保存监听请求，GameFeature 停用时需要撤销。
					ActiveData.ComponentRequests.Add(ExtensionRequestHandle);

					// 移动到下一条 AbilitiesList 配置。
					EntryIndex++;
				}
			}
		}
	}
}

// 清理当前 GameFeature 上下文创建的所有运行时资源。
void UGameFeatureAction_AddAbilities::Reset(FPerContextData& ActiveData)
{
	// 持续移除所有已经添加 Ability 扩展的 Actor。
	while (!ActiveData.ActiveExtensions.IsEmpty())
	{
		// 获取当前 ActiveExtensions 中的一个 Actor。
		auto ExtensionIt = ActiveData.ActiveExtensions.CreateIterator();

		// 移除这个 Actor 身上的 Ability、AttributeSet 和 AbilitySet。
		RemoveActorAbilities(ExtensionIt->Key, ActiveData);
	}

	// 清除所有 GameFrameworkComponentManager 的组件请求。
	ActiveData.ComponentRequests.Empty();
}

// 处理指定 Actor 的扩展生命周期事件。
void UGameFeatureAction_AddAbilities::HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex,
                                                           FGameFeatureStateChangeContext ChangeContext)
{
	// 找到当前 GameFeature 上下文对应的运行时数据。
	FPerContextData* ActiveData = ContextData.Find(ChangeContext);

	// 确保 EntryIndex 有效，并且运行时数据存在。
	if (AbilitiesList.IsValidIndex(EntryIndex) && ActiveData)
	{
		// 获取当前 Actor 对应的 Ability 配置。
		const FGameFeatureAbilitiesEntry& Entry = AbilitiesList[EntryIndex];

		// Actor 扩展被移除，或者 Actor 本身不再是监听对象。
		if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName ==
			UGameFrameworkComponentManager::NAME_ReceiverRemoved))
		{
			// 移除这个 Actor 身上由当前 GameFeature 添加的所有内容。
			RemoveActorAbilities(Actor, *ActiveData);
		}

		// Actor 扩展已经添加，或者项目自定义的 AbilityReady 事件触发。
		else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName ==
			AHodgePlayerState::NAME_HodgeAbilityReady))
		{
			// 给这个 Actor 添加配置中的 Ability、AttributeSet 和 AbilitySet。
			AddActorAbilities(Actor, Entry, *ActiveData);
		}
	}
}

// 给指定 Actor 添加 Ability、AttributeSet 和 AbilitySet。
void UGameFeatureAction_AddAbilities::AddActorAbilities(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry,
                                                        FPerContextData& ActiveData)
{
	// Actor 必须有效。
	check(Actor);

	// 当前逻辑只允许服务器授予 GameplayAbility 等能力资源。
	if (!Actor->HasAuthority())
	{
		// 客户端直接退出，不执行授予逻辑。
		return;
	}

	// early out if Actor already has ability extensions applied
	// 如果这个 Actor 已经添加过当前 GameFeature 的能力扩展，则直接退出，避免重复添加。
	if (ActiveData.ActiveExtensions.Find(Actor) != nullptr)
	{
		return;
	}

	// 查找 Actor 上的 AbilitySystemComponent；如果没有则尝试通过 ComponentManager 添加。
	if (UAbilitySystemComponent* AbilitySystemComponent = FindOrAddComponentForActor<UAbilitySystemComponent>(
		Actor, AbilitiesEntry, ActiveData))
	{
		// 记录本次实际添加到 Actor 身上的所有资源，用于之后精确移除。
		FActorExtensions AddedExtensions;

		// 预留 GameplayAbility Handle 数组容量。
		AddedExtensions.Abilities.Reserve(AbilitiesEntry.GrantedAbilities.Num());

		// 预留 AttributeSet 数组容量。
		AddedExtensions.Attributes.Reserve(AbilitiesEntry.GrantedAttributes.Num());

		// 预留 AbilitySet Handle 数组容量。
		AddedExtensions.AbilitySetHandles.Reserve(AbilitiesEntry.GrantedAbilitySets.Num());

		// 遍历当前配置中的所有 GameplayAbility。
		for (const FHodgeAbilityGrant& Ability : AbilitiesEntry.GrantedAbilities)
		{
			// 确保 Ability 引用有效。
			if (!Ability.AbilityType.IsNull())
			{
				// 同步加载 GameplayAbility 类，并创建对应的 AbilitySpec。
				FGameplayAbilitySpec NewAbilitySpec(Ability.AbilityType.LoadSynchronous());

				// 将 AbilitySpec 授予 ASC，并获得唯一 Handle。
				FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(NewAbilitySpec);

				// 保存这个 Handle，之后移除时使用同一个 Handle。
				AddedExtensions.Abilities.Add(AbilityHandle);
			}
		}

		// 遍历当前配置中的所有 AttributeSet。
		for (const FHodgeAttributeSetGrant& Attributes : AbilitiesEntry.GrantedAttributes)
		{
			// 确保 AttributeSet 类型有效。
			if (!Attributes.AttributeSetType.IsNull())
			{
				// 同步加载 AttributeSet 类。
				TSubclassOf<UAttributeSet> SetType = Attributes.AttributeSetType.LoadSynchronous();

				// 确保最终得到的是有效的 AttributeSet 类。
				if (SetType)
				{
					// 创建 AttributeSet 对象，并让 ASC 的 Owner 作为其 Outer。
					UAttributeSet* NewSet = NewObject<UAttributeSet>(AbilitySystemComponent->GetOwner(), SetType);

					// 如果配置了 Attribute 初始化 DataTable，则继续进行初始化。
					if (!Attributes.InitializationData.IsNull())
					{
						// 同步加载 Attribute 初始化数据表。
						UDataTable* InitData = Attributes.InitializationData.LoadSynchronous();

						// 数据表有效时，从 MetaDataTable 初始化 AttributeSet。
						if (InitData)
						{
							NewSet->InitFromMetaDataTable(InitData);
						}
					}

					// 保存创建出来的 AttributeSet，用于之后移除。
					AddedExtensions.Attributes.Add(NewSet);

					// 将 AttributeSet 注册到 ASC，使 GAS 可以管理和访问它。
					AbilitySystemComponent->AddAttributeSetSubobject(NewSet);
				}
			}
		}

		// 当前项目使用自定义 HodgeAbilitySystemComponent，因此这里进行类型转换。
		UHodgeAbilitySystemComponent* HodgeASC = CastChecked<UHodgeAbilitySystemComponent>(AbilitySystemComponent);

		// 遍历当前配置中的所有 AbilitySet。
		for (const TSoftObjectPtr<const UHodgeAbilitySet>& SetPtr : AbilitiesEntry.GrantedAbilitySets)
		{
			// 获取已经加载的 AbilitySet 资源。
			if (const UHodgeAbilitySet* Set = SetPtr.Get())
			{
				// 将 AbilitySet 中定义的所有 Ability、Effect、AttributeSet 等资源授予 ASC，
				// 并把本次授予产生的所有 Handle 记录到 AddedExtensions 中。
				Set->GiveToAbilitySystem(HodgeASC, &AddedExtensions.AbilitySetHandles.AddDefaulted_GetRef());
			}
		}

		// 记录当前 Actor 已经由这个 GameFeature 添加了哪些资源。
		ActiveData.ActiveExtensions.Add(Actor, AddedExtensions);
	}
	else
	{
		// 找不到或无法创建 ASC 时，无法继续授予 Ability。
		UE_LOG(LogGameFeatures, Error,
		       TEXT("Failed to find/add an ability component to '%s'. Abilities will not be granted."),
		       *Actor->GetPathName());
	}
}

// 移除当前 GameFeature 之前给 Actor 添加的所有能力资源。
void UGameFeatureAction_AddAbilities::RemoveActorAbilities(AActor* Actor, FPerContextData& ActiveData)
{
	// 查找这个 Actor 对应的运行时资源记录。
	if (FActorExtensions* ActorExtensions = ActiveData.ActiveExtensions.Find(Actor))
	{
		// 查找 Actor 身上的 AbilitySystemComponent。
		if (UAbilitySystemComponent* AbilitySystemComponent = Actor->FindComponentByClass<UAbilitySystemComponent>())
		{
			// 移除本次 GameFeature 创建的 AttributeSet。
			for (UAttributeSet* AttribSetInstance : ActorExtensions->Attributes)
			{
				// 从 ASC 中移除指定的 AttributeSet 实例。
				AbilitySystemComponent->RemoveSpawnedAttribute(AttribSetInstance);
			}

			// 遍历本次 GameFeature 授予的所有 GameplayAbility。
			for (FGameplayAbilitySpecHandle AbilityHandle : ActorExtensions->Abilities)
			{
				// 标记 Ability，在当前 Ability 结束后移除。
				AbilitySystemComponent->SetRemoveAbilityOnEnd(AbilityHandle);
			}

			// 转换为项目自定义的 HodgeAbilitySystemComponent。
			UHodgeAbilitySystemComponent* HodgeASC = CastChecked<UHodgeAbilitySystemComponent>(AbilitySystemComponent);

			// 遍历本次授予的所有 AbilitySet。
			for (FHodgeAbilitySet_GrantedHandles& SetHandle : ActorExtensions->AbilitySetHandles)
			{
				// 使用 AbilitySet 授予时保存的精确 Handle，将整个 AbilitySet 完整撤销。
				SetHandle.TakeFromAbilitySystem(HodgeASC);
			}
		}

		// 从当前 GameFeature 的运行时记录中移除 Actor。
		ActiveData.ActiveExtensions.Remove(Actor);
	}
}

// 查找 Actor 上指定类型的组件；如果没有，则通过 GameFrameworkComponentManager 请求创建。
UActorComponent* UGameFeatureAction_AddAbilities::FindOrAddComponentForActor(
	UClass* ComponentType, AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData)
{
	// 首先直接查找 Actor 上是否已经存在目标组件。
	UActorComponent* Component = Actor->FindComponentByClass(ComponentType);

	// 默认情况下，如果没有组件，就需要向 ComponentManager 请求创建。
	bool bMakeComponentRequest = (Component == nullptr);

	// 如果 Actor 已经存在这个组件，则进一步判断它的来源。
	if (Component)
	{
		// Check to see if this component was created from a different `UGameFrameworkComponentManager` request.
		// 检查这个组件是否可能是由其他 GameFrameworkComponentManager 请求创建的。

		// `Native` is what `CreationMethod` defaults to for dynamically added components.
		// 动态创建组件时 CreationMethod 默认可能为 Native，因此不能单纯通过 CreationMethod 判断来源。
		if (Component->CreationMethod == EComponentCreationMethod::Native)
		{
			// Attempt to tell the difference between a true native component and one created by the GameFrameworkComponent system.
			// 尝试区分真正的 Native 组件和 GameFrameworkComponent 系统创建出来的组件。

			// 获取当前组件对应的 Archetype。
			UObject* ComponentArchetype = Component->GetArchetype();

			// 如果 Archetype 是类默认对象，则认为它可能来自 GameFrameworkComponentManager，
			// 因此需要再次创建一个 ComponentRequest；ComponentRequest 本身是引用计数的。
			bMakeComponentRequest = ComponentArchetype->HasAnyFlags(RF_ClassDefaultObject);
		}
	}

	// 如果需要创建组件请求，则向 GameFrameworkComponentManager 请求指定组件。
	if (bMakeComponentRequest)
	{
		// 获取 Actor 所在的 World。
		UWorld* World = Actor->GetWorld();

		// 获取当前 World 对应的 GameInstance。
		UGameInstance* GameInstance = World->GetGameInstance();

		// 获取 GameFrameworkComponentManager 子系统。
		if (UGameFrameworkComponentManager* ComponentMan = UGameInstance::GetSubsystem<
			UGameFrameworkComponentManager>(GameInstance))
		{
			// 请求给 AbilitiesEntry.ActorClass 类型的 Actor 添加指定类型的组件。
			TSharedPtr<FComponentRequestHandle> RequestHandle = ComponentMan->AddComponentRequest(
				AbilitiesEntry.ActorClass, ComponentType);

			// 保存组件请求句柄，之后 GameFeature 停用时进行统一清理。
			ActiveData.ComponentRequests.Add(RequestHandle);
		}

		// 如果之前没有组件，则请求创建后重新查找一次。
		if (!Component)
		{
			// 从 Actor 上重新获取刚刚创建的组件。
			Component = Actor->FindComponentByClass(ComponentType);

			// 确保组件最终成功创建。
			ensureAlways(Component);
		}
	}

	// 返回找到或创建出来的组件。
	return Component;
}

#undef LOCTEXT_NAMESPACE
