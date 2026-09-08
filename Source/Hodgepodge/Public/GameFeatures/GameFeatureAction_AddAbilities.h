
// Copyright Epic Games, Inc. All Rights Reserved.

// 防止头文件被重复包含。
#pragma once

// 引入 GameFeature 世界行为基类。
#include "GameFeatureAction_WorldActionBase.h"

// 引入 GameplayAbility 基础类型。
#include "Abilities/GameplayAbility.h"

// 引入项目自定义的 AbilitySet。
#include "Data/HodgeAbilitySet.h"

// 引入当前类的反射生成代码。
#include "GameFeatureAction_AddAbilities.generated.h"

// 前向声明 WorldContext。
struct FWorldContext;

// 前向声明 InputAction。
class UInputAction;

// 前向声明 AttributeSet。
class UAttributeSet;

// 前向声明 DataTable。
class UDataTable;

// 前向声明 GameFrameworkComponentManager 的组件请求句柄。
struct FComponentRequestHandle;

// 前向声明项目自定义的 AbilitySet。
class UHodgeAbilitySet;

// 描述一个需要授予 Actor 的 GameplayAbility。
USTRUCT(BlueprintType)
struct FHodgeAbilityGrant
{
	GENERATED_BODY()

	// Type of ability to grant
	// 要授予 Actor 的 GameplayAbility 类型。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
	TSoftClassPtr<UGameplayAbility> AbilityType;

	// Input action to bind the ability to, if any (can be left unset)
	// 如果需要，可以指定该 Ability 对应的输入动作。
	// 当前项目暂时没有启用这部分 InputAction 配置。
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
// 	TSoftObjectPtr<UInputAction> InputAction;
};

// 描述一个需要授予 Actor 的 AttributeSet。
USTRUCT(BlueprintType)
struct FHodgeAttributeSetGrant
{
	GENERATED_BODY()

	// Ability set to grant
	// 要授予 Actor 的 AttributeSet 类型。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
	TSoftClassPtr<UAttributeSet> AttributeSetType;

	// Data table referent to initialize the attributes with, if any (can be left unset)
	// 用于初始化 AttributeSet 属性值的 DataTable，可以不设置。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client,Server"))
	TSoftObjectPtr<UDataTable> InitializationData;
};

// 描述一组需要应用到指定 Actor 类型上的 Gameplay 能力配置。
USTRUCT()
struct FGameFeatureAbilitiesEntry
{
	GENERATED_BODY()

	// The base actor class to add to
	// 指定需要添加这些能力的 Actor 基类。
	UPROPERTY(EditAnywhere, Category="Abilities")
	TSoftClassPtr<AActor> ActorClass;

	// List of abilities to grant to actors of the specified class
	// 授予指定 Actor 类型的 GameplayAbility 列表。
	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<FHodgeAbilityGrant> GrantedAbilities;

	// List of attribute sets to grant to actors of the specified class 
	// 授予指定 Actor 类型的 AttributeSet 列表。
	UPROPERTY(EditAnywhere, Category="Attributes")
	TArray<FHodgeAttributeSetGrant> GrantedAttributes;

	// List of ability sets to grant to actors of the specified class
	// 授予指定 Actor 类型的 AbilitySet 列表。
	UPROPERTY(EditAnywhere, Category="Attributes", meta=(AssetBundles="Client,Server"))
	TArray<TSoftObjectPtr<const UHodgeAbilitySet>> GrantedAbilitySets;
};

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddAbilities

/**
 * GameFeatureAction responsible for granting abilities (and attributes) to actors of a specified type.
 */
// 负责向指定类型 Actor 动态授予 GameplayAbility、AttributeSet 和 AbilitySet 的 GameFeature Action。
UCLASS(MinimalAPI, meta = (DisplayName = "Add Abilities"))
class UGameFeatureAction_AddAbilities final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	// GameFeature 激活时开始向目标 Actor 添加能力。
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;

	// GameFeature 停用时撤销之前添加的能力。
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	// 编辑器下验证当前 Ability 配置是否有效。
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface

	/** 记录当前 GameFeature 要应用到哪些 Actor 类型以及授予哪些能力。 */
	UPROPERTY(EditAnywhere, Category="Abilities", meta=(TitleProperty="ActorClass", ShowOnlyInnerProperties))
	TArray<FGameFeatureAbilitiesEntry> AbilitiesList;

private:
	// 记录单个 Actor 当前被本 GameFeature 添加的所有能力资源。
	struct FActorExtensions
	{
		// 记录通过 GiveAbility 授予的 GameplayAbility Spec Handle。
		TArray<FGameplayAbilitySpecHandle> Abilities;

		// 记录通过本 GameFeature 创建并添加到 Actor 的 AttributeSet。
		TArray<UAttributeSet*> Attributes;

		// 记录通过 AbilitySet 授予的所有能力句柄，方便后续完整撤销。
		TArray<FHodgeAbilitySet_GrantedHandles> AbilitySetHandles;
	};

	// 记录一次 GameFeature 激活上下文对应的所有运行时资源。
	struct FPerContextData
	{
		// Actor → 本 GameFeature 为该 Actor 添加的能力资源。
		TMap<AActor*, FActorExtensions> ActiveExtensions;

		// 记录向 GameFrameworkComponentManager 注册的 Actor 扩展监听句柄。
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
	};
	
	// 按 GameFeature 激活上下文保存对应的运行时数据。
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;	

	//~ Begin UGameFeatureAction_WorldActionBase interface
	// 将当前 GameFeature Action 添加到指定 World。
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

	// 清理当前 GameFeature 激活过程中添加的所有能力和组件。
	void Reset(FPerContextData& ActiveData);

	// 处理指定 Actor 的 GameFrameworkComponentManager 扩展事件。
	void HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext);

	// 根据配置向指定 Actor 添加 GameplayAbility、AttributeSet 和 AbilitySet。
	void AddActorAbilities(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData);

	// 移除指定 Actor 上由当前 GameFeature 添加的所有能力。
	void RemoveActorAbilities(AActor* Actor, FPerContextData& ActiveData);

	// 根据指定配置查找或创建 Actor 上需要的组件。
	template<class ComponentType>
	ComponentType* FindOrAddComponentForActor(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData)
	{
		//@TODO: Just find, no add?
		// 根据具体组件类型查找或创建组件，并转换为目标类型。
		return Cast<ComponentType>(FindOrAddComponentForActor(ComponentType::StaticClass(), Actor, AbilitiesEntry, ActiveData));
	}

	// 根据组件类型查找 Actor 上已有的组件，必要时负责创建组件。
	UActorComponent* FindOrAddComponentForActor(UClass* ComponentType, AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData);
};
