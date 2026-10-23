/** @file HodgeAbilitySet.h
 * @brief 声明能力/属性/效果授予包相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "Engine/DataAsset.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"

#include "GameplayAbilitySpecHandle.h"
#include "HodgeAbilitySet.generated.h"

class UAttributeSet;
class UGameplayEffect;
class UHodgeAbilitySystemComponent;
class UHodgeGameplayAbility;
class UObject;
class UHodgeAbilityDefinition;

/** 玩家持久授予可先于 Pawn；装备和怪物仍要求本次 Avatar 有效。 */
enum class EHodgeAbilityGrantContext : uint8
{
	AvatarBound,
	OwnerInitialized
};

/** 受检授予的结果；失败清理不能撤销已经发生的 Instant 或自动激活副作用。 */
USTRUCT(BlueprintType)
struct FHodgeAbilityGrantResult
{
	GENERATED_BODY()
	// 控制是否Succeeded的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bSucceeded = false;
	// 控制是否存在判断IrreversibleEffects的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bHasIrreversibleEffects = false;
	// 错误的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FString Error;
};

/** 保存能力设置定义的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgeAbilitySet_Definition
{
 GENERATED_BODY()
 // 授予的动作 Definition，不能把其 GA 再重复加入普通能力列表。
 UPROPERTY(EditDefaultsOnly) TObjectPtr<UHodgeAbilityDefinition> Definition;
 // 授予到 AbilitySpec 的等级，Definition 的等级必须至少为 1。
 UPROPERTY(EditDefaultsOnly, meta=(ClampMin="1")) int32 AbilityLevel = 1;
};


/**
 * FHodgeAbilitySet_GameplayAbility
 *
 * AbilitySet 中用于配置要授予的 GameplayAbility 的数据。
 */
USTRUCT(BlueprintType)
struct FHodgeAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	// 要授予的 GameplayAbility 类型。
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHodgeGameplayAbility> Ability = nullptr;

	// 授予 GameplayAbility 时使用的等级。
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// 用于将输入映射到该 Ability 的 GameplayTag。
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


/**
 * FHodgeAbilitySet_GameplayEffect
 *
 * AbilitySet 中用于配置要授予的 GameplayEffect 的数据。
 */
USTRUCT(BlueprintType)
struct FHodgeAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	// 要授予的 GameplayEffect 类型。
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// 授予 GameplayEffect 时使用的等级。
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 * FHodgeAbilitySet_AttributeSet
 *
 * AbilitySet 中用于配置要授予的 AttributeSet 的数据。
 */
USTRUCT(BlueprintType)
struct FHodgeAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// 要添加到 AbilitySystemComponent 中的 AttributeSet 类型。
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

/**
 * FHodgeAbilitySet_GrantedHandles
 *
 * 用于保存 AbilitySet 授予的 Ability、GameplayEffect 和 AttributeSet，
 * 方便之后通过这些引用将已经授予的内容从 AbilitySystem 中移除。
 */
USTRUCT(BlueprintType)
struct FHodgeAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	// 保存授予的 GameplayAbility 对应的 SpecHandle。
	/** 添加能力规格句柄。
	 * @param Handle 对应操作或能力的句柄。
	 */
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);

	// 保存授予的 GameplayEffect 对应的 ActiveGameplayEffectHandle。
	/** 添加玩法效果句柄。
	 * @param Handle 对应操作或能力的句柄。
	 */
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);

	// 保存授予的 AttributeSet 对象指针。
	/** 添加属性集基类。
	 * @param Set 设置，由调用方传入。
	 */
	void AddAttributeSet(UAttributeSet* Set);

	// 根据保存的 Handles 和 AttributeSet，将 AbilitySet 授予的内容从 ASC 中全部移除。
	/** 撤销该句柄集合拥有的能力、效果和属性。
	 * @param HodgeASC ASC，由调用方传入。
	 */
	void TakeFromAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC);
	/** 追加当前记录或授予结果到目标集合。
	 * @param Other Other，由调用方传入。
	 */
	void Append(FHodgeAbilitySet_GrantedHandles&& Other);
	/** 检查记录的授予句柄是否仍属于指定ASC。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool AreValidFor(const UHodgeAbilitySystemComponent* ASC) const;
	/** 获取能力数量。
	 * @return 能力数量的当前值或引用。
	 */
	int32 GetAbilityCount() const { return AbilitySpecHandles.Num(); }

protected:
	// 已授予 GameplayAbility 的 SpecHandle 列表。
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// 已应用 GameplayEffect 的 ActiveGameplayEffectHandle 列表。
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// 已添加到 AbilitySystemComponent 的 AttributeSet 对象列表。
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


/**
 * UHodgeAbilitySet
 *
 * 不可修改的 PrimaryDataAsset，用于统一配置一组 GameplayAbility、GameplayEffect 和 AttributeSet。
 *
 * 可以把它理解成一个“能力包”：
 * 一个 AbilitySet 可以一次性给角色安装一整套战斗能力、属性和效果。
 */
UCLASS(BlueprintType, Const)
class UHodgeAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 构造能力/属性/效果授予包并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/** 获取GrantedDefinitions。
	 * @return GrantedDefinitions的当前值或引用。
	 */
	const TArray<FHodgeAbilitySet_Definition>& GetGrantedDefinitions() const { return GrantedAbilityDefinitions; }
	/** 供跨来源预校验读取配置，不允许调用者修改资产。 */
	const TArray<FHodgeAbilitySet_GameplayAbility>& GetGrantedAbilities() const { return GrantedGameplayAbilities; }
	/** 供 GameFeature 检查直接属性与 AbilitySet 的重复声明。 */
	const TArray<FHodgeAbilitySet_AttributeSet>& GetGrantedAttributes() const { return GrantedAttributes; }
	/** 同族属性集不能重复驻留，避免 GAS 按基类查找时产生歧义。 */
	static bool AreAttributeSetTypesConflicting(const UClass* First, const UClass* Second);

	// 将当前 AbilitySet 中配置的所有内容授予指定的 AbilitySystemComponent，并可通过返回的 Handles 在之后移除这些内容。
	/** 将配置中的能力、属性与效果授予目标ASC。
	 * @param HodgeASC ASC，由调用方传入。
	 * @param OutGrantedHandles 本来源授予的能力/效果/属性句柄集合。
	 * @param SourceObject 能力或效果的来源对象。
	 */
	void GiveToAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC, FHodgeAbilitySet_GrantedHandles* OutGrantedHandles,
	                         UObject* SourceObject = nullptr) const;
	/** 校验关联授予。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @param bForInitialization 是否关联初始化的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool ValidateForGrant(const UHodgeAbilitySystemComponent* ASC, TArray<FText>& Errors, bool bForInitialization = false) const;
	/** 在授予前校验整批配置与目标ASC的兼容性。
	 * @param Sets Sets，由调用方传入。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @param bForInitialization 是否关联初始化的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	static bool ValidateBatchForGrant(const TArray<const UHodgeAbilitySet*>& Sets, const UHodgeAbilitySystemComponent* ASC,
		TArray<FText>& Errors, bool bForInitialization = false);
	/** 按受检授予流程执行配置并返回授予结果。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 * @param OutHandles 输出Handles。
	 * @param SourceObject 能力或效果的来源对象。
	 * @param bForInitialization 是否关联初始化的控制标记。
	 * @return 本次计算、查询或创建得到的能力授予结果。
	 */
	FHodgeAbilityGrantResult TryGiveToAbilitySystem(UHodgeAbilitySystemComponent* ASC,
		FHodgeAbilitySet_GrantedHandles& OutHandles, UObject* SourceObject = nullptr, bool bForInitialization = false,
		EHodgeAbilityGrantContext Context = EHodgeAbilityGrantContext::AvatarBound) const;
	/** 整批预校验并提交；失败仅清理本批可撤销资源，保留不可逆副作用标记。 */
	static FHodgeAbilityGrantResult TryGiveBatchToAbilitySystem(const TArray<const UHodgeAbilitySet*>& Sets,
		UHodgeAbilitySystemComponent* ASC, FHodgeAbilitySet_GrantedHandles& OutHandles,
		UObject* SourceObject = nullptr, bool bForInitialization = false,
		EHodgeAbilityGrantContext Context = EHodgeAbilityGrantContext::AvatarBound);

protected:
	// 授予 AbilitySet 时需要添加的 GameplayAbility 列表。
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FHodgeAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// 授予动作配置并建立 Spec 与 Definition 的关联，普攻和独立动作技能使用此列表。
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities", meta=(TitleProperty="Definition"))
	TArray<FHodgeAbilitySet_Definition> GrantedAbilityDefinitions;

	// 授予 AbilitySet 时需要应用的 GameplayEffect 列表。
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FHodgeAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// 授予 AbilitySet 时需要添加的 AttributeSet 列表。
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FHodgeAbilitySet_AttributeSet> GrantedAttributes;
private:
	/** 唯一授予循环；调用前完成预校验，调用后仍验证同步回调中的绑定变化。 */
	FHodgeAbilityGrantResult GrantInternal(UHodgeAbilitySystemComponent* ASC,
		FHodgeAbilitySet_GrantedHandles& OutHandles, UObject* SourceObject, EHodgeAbilityGrantContext Context) const;
};
