/** @file HodgeAbilitySet.cpp
 * @brief 实现能力/属性/效果授予包相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data/HodgeAbilitySet.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"

#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Data/HodgeAbilityDefinition.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilitySet)

// 添加一个有效的 GameplayAbility SpecHandle，用于之后移除对应 Ability
void FHodgeAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

// 添加一个有效的 GameplayEffect Handle，用于之后移除对应 GameplayEffect
void FHodgeAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

// 保存授予的 AttributeSet 指针，用于之后从 ASC 中移除
void FHodgeAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

// 移除当前 GrantedHandles 所记录的所有 Ability、GameplayEffect 和 AttributeSet
void FHodgeAbilitySet_GrantedHandles::TakeFromAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC)
{
	check(HodgeASC);

	// AbilitySet 的授予和移除必须由服务器执行
	if (!HodgeASC->GetOwner() || !HodgeASC->GetOwner()->HasAuthority())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// 遍历并移除之前授予的 GameplayAbility
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			HodgeASC->ClearAbility(Handle);
		}
	}

	// 遍历并移除之前应用的 GameplayEffect
	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			HodgeASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	// 遍历并移除之前添加到 ASC 的 AttributeSet
	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		HodgeASC->RemoveSpawnedAttribute(Set);
	}

	// 清空所有记录，避免重复移除或持有无效 Handle
	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

/** 构造能力/属性/效果授予包并初始化其默认状态。 */
UHodgeAbilitySet::UHodgeAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// 将当前 AbilitySet 中配置的 Ability、Effect 和 AttributeSet 全部授予指定 ASC
FHodgeAbilityGrantResult UHodgeAbilitySet::GrantInternal(UHodgeAbilitySystemComponent* HodgeASC,
                                           FHodgeAbilitySet_GrantedHandles* OutGrantedHandles,
	                                       UObject* SourceObject, bool bChecked) const
{
	check(HodgeASC);
	FHodgeAbilityGrantResult Result;
	// 当前操作关联的所属对象。
	TWeakObjectPtr<AActor> Owner = HodgeASC->GetOwnerActor();
	TWeakObjectPtr<AActor> Avatar = HodgeASC->GetAvatarActor();
	auto Current = [&]() { return Owner.IsValid() && Avatar.IsValid() && HodgeASC->IsRegistered() &&
		!Owner->IsActorBeingDestroyed() && !Avatar->IsActorBeingDestroyed() &&
		HodgeASC->GetOwnerActor() == Owner.Get() && HodgeASC->GetAvatarActor() == Avatar.Get(); };
	auto Fail = [&](const TCHAR* Message) { Result.Error = Message; return Result; };
	if (bChecked && !Current()) { return Fail(TEXT("ASC ActorInfo is not bound")); }

	// AbilitySet 的授予必须由服务器执行
	if (!HodgeASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return Fail(TEXT("Ability grants require authority"));
	}

	// 授予 AttributeSet。
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		// 获取当前要授予的 AttributeSet 配置
		const FHodgeAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		// 检查配置的 AttributeSet 类型是否有效
		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"),
			       SetIndex, *GetNameSafe(this));
			if (bChecked) { return Fail(TEXT("Invalid AttributeSet")); }
			continue;
		}

        const bool bCoreSet = SetToGrant.AttributeSet->IsChildOf(UHodgeHealthSet::StaticClass()) || SetToGrant.AttributeSet->IsChildOf(UHodgeCombatSet::StaticClass());
        if (bCoreSet)
        {
            bool bExists = false;
            for (const UAttributeSet* Existing : HodgeASC->GetSpawnedAttributes())
            {
                bExists |= Existing && ((SetToGrant.AttributeSet->IsChildOf(UHodgeHealthSet::StaticClass()) && Existing->IsA<UHodgeHealthSet>())
                    || (SetToGrant.AttributeSet->IsChildOf(UHodgeCombatSet::StaticClass()) && Existing->IsA<UHodgeCombatSet>()));
            }
            if (bExists) { UE_LOG(LogTemp, Error, TEXT("AbilitySet %s cannot duplicate a core AttributeSet"), *GetName()); continue; }
        }
		// 根据配置创建 AttributeSet 实例，Owner 设置为 ASC 的 OwnerActor
		UAttributeSet* NewSet = NewObject<UAttributeSet>(HodgeASC->GetOwner(), SetToGrant.AttributeSet);

		// 将新创建的 AttributeSet 添加到 ASC
		HodgeASC->AddAttributeSetSubobject(NewSet);

		// 如果调用者需要回收信息，则记录这个 AttributeSet
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
		if (bChecked && !Current()) { return Fail(TEXT("Binding changed during AttributeSet grant")); }
	}

	// 授予 GameplayAbility。
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		// 获取当前要授予的 GameplayAbility 配置
		const FHodgeAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		// 检查 GameplayAbility 类型是否有效
		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."),
			       AbilityIndex, *GetNameSafe(this));
			if (bChecked) { return Fail(TEXT("Invalid ability")); }
			continue;
		}

		// 获取 GameplayAbility 的 CDO，用于构造 GameplayAbilitySpec
		UHodgeGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UHodgeGameplayAbility>();
		Result.bHasIrreversibleEffects |= AbilityCDO->GetActivationPolicy() == EHodgeAbilityActivationPolicy::OnSpawn;

		// 根据 Ability 类和等级创建 AbilitySpec
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);

		// 设置 Ability 的 SourceObject，例如可以指向授予该 Ability 的武器、装备或 AbilitySet 来源
		AbilitySpec.SourceObject = SourceObject;

		// 将配置的 InputTag 写入 AbilitySpec 的动态 Source Tags，用于输入系统识别该 Ability
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		// 将 AbilitySpec 授予 ASC，并获得唯一的 SpecHandle
		const FGameplayAbilitySpecHandle AbilitySpecHandle = HodgeASC->GiveAbility(AbilitySpec);

		// 如果调用者需要回收信息，则记录这个 AbilitySpecHandle
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
		if (bChecked && (!Current() || !AbilitySpecHandle.IsValid() || !HodgeASC->FindAbilitySpecFromHandle(AbilitySpecHandle)))
		{ return Fail(TEXT("Ability grant failed or binding changed")); }
	}

	for (const FHodgeAbilitySet_Definition& Entry : GrantedAbilityDefinitions)
	{
		const FGameplayAbilitySpecHandle Handle = HodgeASC->GiveAbilityDefinition(
			Entry.Definition, Entry.AbilityLevel, SourceObject);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(Handle);
		}
		if (bChecked && (!Current() || !Handle.IsValid() || !HodgeASC->FindAbilitySpecFromHandle(Handle)))
		{ return Fail(TEXT("Definition grant failed or binding changed")); }
	}

	// 授予 GameplayEffect。
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		// 获取当前要授予的 GameplayEffect 配置
		const FHodgeAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		// 检查 GameplayEffect 类型是否有效
		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"),
			       EffectIndex, *GetNameSafe(this));
			if (bChecked) { return Fail(TEXT("Invalid GameplayEffect")); }
			continue;
		}

		// 获取 GameplayEffect 的 CDO，因为应用 GameplayEffect 时使用的是 GE 定义对象
		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();

		// 将 GameplayEffect 直接应用到 ASC 自己身上，并获得对应的 ActiveGameplayEffectHandle
		const FActiveGameplayEffectHandle GameplayEffectHandle = HodgeASC->ApplyGameplayEffectToSelf(
			GameplayEffect, EffectToGrant.EffectLevel, HodgeASC->MakeEffectContext());

		// 如果调用者需要回收信息，则记录这个 GameplayEffectHandle
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
		Result.bHasIrreversibleEffects |= GameplayEffectHandle.WasSuccessfullyApplied() && GameplayEffect->DurationPolicy == EGameplayEffectDurationType::Instant;
		if (bChecked && (!Current() || !GameplayEffectHandle.WasSuccessfullyApplied()))
		{ return Fail(TEXT("GameplayEffect grant failed or binding changed")); }
	}
	Result.bSucceeded = !bChecked || (Current() && OutGrantedHandles && OutGrantedHandles->AreValidFor(HodgeASC));
	if (!Result.bSucceeded) { Result.Error = TEXT("Granted resources changed before commit"); }
	return Result;
}

/** 追加当前记录或授予结果到目标集合。 */
void FHodgeAbilitySet_GrantedHandles::Append(FHodgeAbilitySet_GrantedHandles&& Other)
{
	AbilitySpecHandles.Append(Other.AbilitySpecHandles);
	GameplayEffectHandles.Append(Other.GameplayEffectHandles);
	GrantedAttributeSets.Append(Other.GrantedAttributeSets);
	Other.AbilitySpecHandles.Reset(); Other.GameplayEffectHandles.Reset(); Other.GrantedAttributeSets.Reset();
}

/** 检查记录的授予句柄是否仍属于指定ASC。 */
bool FHodgeAbilitySet_GrantedHandles::AreValidFor(const UHodgeAbilitySystemComponent* ASC) const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsValid(ASC)) { return false; }
	for (const auto Handle : AbilitySpecHandles) { if (!ASC->FindAbilitySpecFromHandle(Handle)) { return false; } }
	for (const auto Handle : GameplayEffectHandles) { if (!ASC->GetActiveGameplayEffect(Handle)) { return false; } }
	for (const UAttributeSet* Set : GrantedAttributeSets) { if (!ASC->GetSpawnedAttributes().Contains(Set)) { return false; } }
	return true;
}

/** 将配置中的能力、属性与效果授予目标ASC。 */
void UHodgeAbilitySet::GiveToAbilitySystem(UHodgeAbilitySystemComponent* ASC,
	FHodgeAbilitySet_GrantedHandles* OutHandles, UObject* SourceObject) const
{
	// 旧入口保留宽松配置语义，受检调用复用同一执行循环。
	GrantInternal(ASC, OutHandles, SourceObject, false);
}

/** 按受检授予流程执行配置并返回授予结果。 */
FHodgeAbilityGrantResult UHodgeAbilitySet::TryGiveToAbilitySystem(UHodgeAbilitySystemComponent* ASC,
	FHodgeAbilitySet_GrantedHandles& OutHandles, UObject* SourceObject, bool bForInitialization) const
{
	TArray<FText> Errors;
	FHodgeAbilityGrantResult Result;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsValid(ASC) || !ValidateForGrant(ASC, Errors, bForInitialization))
	{ Result.Error = Errors.IsEmpty() ? TEXT("Invalid ASC") : Errors[0].ToString(); return Result; }
	FHodgeAbilitySet_GrantedHandles Pending;
	Result = GrantInternal(ASC, &Pending, SourceObject, true);
	if (Result.bSucceeded) { OutHandles.Append(MoveTemp(Pending)); }
	else { Pending.TakeFromAbilitySystem(ASC); }
	return Result;
}

/** 校验关联授予。 */
bool UHodgeAbilitySet::ValidateForGrant(const UHodgeAbilitySystemComponent* ASC, TArray<FText>& Errors, bool bForInitialization) const
{
	const int32 Before = Errors.Num();
	auto Fail = [&Errors, this](const TCHAR* Reason) { Errors.Add(FText::FromString(GetPathName() + TEXT(": ") + Reason)); };
	TSet<UClass*> Attributes;
	for (const auto& Entry : GrantedAttributes)
	{
		UClass* Type = Entry.AttributeSet;
		if (!Type || Type->HasAnyClassFlags(CLASS_Abstract) || Attributes.Contains(Type)) { Fail(TEXT("Invalid or duplicate AttributeSet")); continue; }
		Attributes.Add(Type);
		if (!ASC) { continue; }
		for (const UAttributeSet* Existing : ASC->GetSpawnedAttributes())
		{
			if (Existing && (Existing->IsA(Type) ||
				(Type->IsChildOf(UHodgeHealthSet::StaticClass()) && Existing->IsA<UHodgeHealthSet>()) ||
				(Type->IsChildOf(UHodgeCombatSet::StaticClass()) && Existing->IsA<UHodgeCombatSet>())))
			{ Fail(TEXT("AttributeSet already exists on ASC")); break; }
		}
	}
	TSet<UClass*> Abilities;
	for (const auto& Entry : GrantedGameplayAbilities)
	{
		UClass* Type = Entry.Ability;
		if (!Type || Type->HasAnyClassFlags(CLASS_Abstract) || Entry.AbilityLevel < 1 || Abilities.Contains(Type))
		{ Fail(TEXT("Invalid or duplicate ability")); continue; }
		Abilities.Add(Type);
		if (bForInitialization && Type->GetDefaultObject<UHodgeGameplayAbility>()->GetActivationPolicy() == EHodgeAbilityActivationPolicy::OnSpawn)
		{ Fail(TEXT("Bootstrap abilities cannot activate OnSpawn")); }
	}
	TSet<FGameplayTag> Definitions;
	for (const auto& Entry : GrantedAbilityDefinitions)
	{
		if (!Entry.Definition || Entry.AbilityLevel < 1) { Fail(TEXT("Missing Definition or invalid level")); continue; }
		Entry.Definition->ValidateDefinition(Errors);
		if (Definitions.Contains(Entry.Definition->AbilityTag) || (ASC && ASC->FindDefinitionAbility(Entry.Definition->AbilityTag).IsValid()))
		{ Fail(TEXT("Definition identity is already granted")); }
		Definitions.Add(Entry.Definition->AbilityTag);
	}
	for (const auto& Entry : GrantedGameplayEffects)
	{
		UClass* Type = Entry.GameplayEffect;
		if (!Type || Type->HasAnyClassFlags(CLASS_Abstract) || !FMath::IsFinite(Entry.EffectLevel) || Entry.EffectLevel <= 0.f)
		{ Fail(TEXT("Invalid GameplayEffect")); }
	}
	return Before == Errors.Num();
}

/** 在授予前校验整批配置与目标ASC的兼容性。 */
bool UHodgeAbilitySet::ValidateBatchForGrant(const TArray<const UHodgeAbilitySet*>& Sets,
	const UHodgeAbilitySystemComponent* ASC, TArray<FText>& Errors, bool bForInitialization)
{
	const int32 Before = Errors.Num();
	TSet<const UHodgeAbilitySet*> UniqueSets;
	TSet<FGameplayTag> Definitions;
	TSet<UClass*> Attributes;
	TSet<UClass*> Abilities;
	for (const auto* Set : Sets)
	{
		if (!IsValid(Set) || UniqueSets.Contains(Set)) { Errors.Add(FText::FromString(TEXT("Null or duplicate AbilitySet in batch"))); continue; }
		UniqueSets.Add(Set); Set->ValidateForGrant(ASC, Errors, bForInitialization);
		for (const auto& Entry : Set->GrantedAbilityDefinitions)
		{
			if (!Entry.Definition) { continue; }
			if (Definitions.Contains(Entry.Definition->AbilityTag)) { Errors.Add(FText::FromString(TEXT("Duplicate Definition identity across AbilitySets"))); }
			Definitions.Add(Entry.Definition->AbilityTag);
		}
		for (const auto& Entry : Set->GrantedAttributes)
		{
			if (Attributes.Contains(Entry.AttributeSet.Get())) { Errors.Add(FText::FromString(TEXT("Duplicate AttributeSet across AbilitySets"))); }
			Attributes.Add(Entry.AttributeSet.Get());
		}
		for (const auto& Entry : Set->GrantedGameplayAbilities)
		{
			if (Abilities.Contains(Entry.Ability.Get())) { Errors.Add(FText::FromString(TEXT("Duplicate ability across AbilitySets"))); }
			Abilities.Add(Entry.Ability.Get());
		}
	}
	return Before == Errors.Num();
}
