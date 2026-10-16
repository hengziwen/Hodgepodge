/** @file GameFeatureAction_AddAbilities.h
 * @brief 声明GameFeature能力注入动作相关的类型、接口与现有处理流程。
 */

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
class UGameFrameworkComponentManager;

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
	/** 处理游戏功能扩展Activating。
	 * @param Context 本次操作的上下文。
	 */
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;

	// GameFeature 停用时撤销之前添加的能力。
	/** 处理游戏功能扩展Deactivating。
	 * @param Context 本次操作的上下文。
	 */
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	// 编辑器下验证当前 Ability 配置是否有效。
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface

	/** 记录当前 GameFeature 要应用到哪些 Actor 类型以及授予哪些能力。 */
	UPROPERTY(EditAnywhere, Category="Abilities", meta=(TitleProperty="ActorClass", ShowOnlyInnerProperties))
	TArray<FGameFeatureAbilitiesEntry> AbilitiesList;

private:
	friend struct FHodgeGameFeatureGrantTestAccess;
	/** 条目状态区分依赖等待、正在提交、成功和失败，失败不自动重复副作用。 */
	enum class EGrantState : uint8 { Waiting, Applying, Applied, Failed, Removed };
	// 每个 Actor/EntryIndex 独立持有来源账本，回调重入时由共享记录保持地址稳定。
	struct FActorExtensions
	{
		TWeakObjectPtr<AActor> Actor;
		TWeakObjectPtr<UHodgeAbilitySystemComponent> GrantedASC;
		TWeakObjectPtr<UGameFrameworkComponentManager> InitManager;
		FDelegateHandle InitStateHandle;
		TArray<FGameplayAbilitySpecHandle> DirectAbilities;
		FHodgeAbilitySet_GrantedHandles GrantedHandles;
		EGrantState State = EGrantState::Waiting;
		bool bApplying = false;
		bool bCommitted = false;
		bool bHasIrreversibleEffects = false;
		bool bCancelDirectOnRemoval = false;
	};

	// 记录一次 GameFeature 激活上下文对应的所有运行时资源。
	struct FPerContextData
	{
		// 不同条目互不覆盖，失效 Actor 的撤销仍使用原授予 ASC。
		TMap<TWeakObjectPtr<AActor>, TMap<int32, TSharedPtr<FActorExtensions>>> ActiveExtensions;
		bool bActive = true;

		// 记录向 GameFrameworkComponentManager 注册的 Actor 扩展监听句柄。
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
	};
	
	// 按 GameFeature 激活上下文保存对应的运行时数据。
	TMap<FGameFeatureStateChangeContext, TSharedPtr<FPerContextData>> ContextData;

	//~ Begin UGameFeatureAction_WorldActionBase interface
	// 将当前 GameFeature Action 添加到指定 World。
	/** 添加目标世界。
	 * @param WorldContext 世界上下文，由调用方传入。
	 * @param ChangeContext 变更上下文，由调用方传入。
	 */
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

	// 清理当前 GameFeature 激活过程中添加的所有能力和组件。
	/** 恢复本对象的初始状态。
	 * @param ActiveData 活动数据，由调用方传入。
	 */
	void Reset(FPerContextData& ActiveData);

	// 处理指定 Actor 的 GameFrameworkComponentManager 扩展事件。
	/** 消费GameFramework扩展事件并配对管理注入内容。
	 * @param Actor 本次操作关联的Actor。
	 * @param EventName 事件名称，由调用方传入。
	 * @param EntryIndex 条目索引，由调用方传入。
	 * @param ChangeContext 变更上下文，由调用方传入。
	 */
	void HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext);

	// 根据配置向指定 Actor 添加 GameplayAbility、AttributeSet 和 AbilitySet。
	/** 添加Actor对象Abilities。
	 * @param Actor 本次操作关联的Actor。
	 * @param AbilitiesEntry Abilities条目，由调用方传入。
	 * @param ActiveData 活动数据，由调用方传入。
	 */
	void AddActorAbilities(AActor* Actor, int32 EntryIndex, const TSharedRef<FPerContextData>& ActiveData);

	// 移除指定 Actor 上由当前 GameFeature 添加的所有能力。
	/** 移除Actor对象Abilities。
	 * @param Actor 本次操作关联的Actor。
	 * @param ActiveData 活动数据，由调用方传入。
	 */
	void RemoveActorAbilities(AActor* Actor, int32 EntryIndex, FPerContextData& ActiveData, bool bCancelDirect = false);
	/** 先使来源失效再撤销；提交中的记录由当前调用栈完成失败清理。 */
	void RetireExtension(const TSharedRef<FActorExtensions>& Extension, bool bCancelDirect = false);
	/** 普通停用保留直接能力的结束后移除语义；未提交资源立即清理。 */
	void RevokeExtension(FActorExtensions& Extension, bool bCancelDirectAbilities);

	// 根据指定配置查找或创建 Actor 上需要的组件。
	template<class ComponentType>
	/** 查找Or添加组件关联Actor对象。
	 * @param Actor 本次操作关联的Actor。
	 * @param AbilitiesEntry Abilities条目，由调用方传入。
	 * @param ActiveData 活动数据，由调用方传入。
	 * @return Or添加组件关联Actor对象的当前值或引用。
	 */
	ComponentType* FindOrAddComponentForActor(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData)
	{
		//@TODO: Just find, no add?
		// 根据具体组件类型查找或创建组件，并转换为目标类型。
		return Cast<ComponentType>(FindOrAddComponentForActor(ComponentType::StaticClass(), Actor, AbilitiesEntry, ActiveData));
	}

	// 根据组件类型查找 Actor 上已有的组件，必要时负责创建组件。
	/** 查找Or添加组件关联Actor对象。
	 * @param ComponentType 组件类型，由调用方传入。
	 * @param Actor 本次操作关联的Actor。
	 * @param AbilitiesEntry Abilities条目，由调用方传入。
	 * @param ActiveData 活动数据，由调用方传入。
	 * @return Or添加组件关联Actor对象的当前值或引用。
	 */
	UActorComponent* FindOrAddComponentForActor(UClass* ComponentType, AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData);
};
