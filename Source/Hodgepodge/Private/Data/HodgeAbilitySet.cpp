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
#include "UObject/StrongObjectPtr.h"

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
	if (!IsValid(HodgeASC)) { return; }

	// AbilitySet 的授予和移除必须由服务器执行
	if (!HodgeASC->GetOwner() || !HodgeASC->GetOwner()->HasAuthority())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}
	TStrongObjectPtr<UHodgeAbilitySystemComponent> KeepASC(HodgeASC);

	// 撤销回调可能重入，先清空当前来源的公开账本。
	auto Abilities = MoveTemp(AbilitySpecHandles);
	auto Effects = MoveTemp(GameplayEffectHandles);
	auto Attributes = MoveTemp(GrantedAttributeSets);

	// 遍历并移除之前授予的 GameplayAbility
	for (const FGameplayAbilitySpecHandle& Handle : Abilities)
	{
		if (Handle.IsValid())
		{
			HodgeASC->ClearAbility(Handle);
		}
	}

	// 遍历并移除之前应用的 GameplayEffect
	for (const FActiveGameplayEffectHandle& Handle : Effects)
	{
		if (Handle.IsValid())
		{
			HodgeASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	// 遍历并移除之前添加到 ASC 的 AttributeSet
	for (UAttributeSet* Set : Attributes)
	{
		HodgeASC->RemoveSpawnedAttribute(Set);
	}

	// 原账本已在回调前取走；重入后新增的资源不由本次撤销处理。
}

/** 构造能力/属性/效果授予包并初始化其默认状态。 */
UHodgeAbilitySet::UHodgeAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

namespace
{
    /** 固定本次提交的 Owner/Avatar 身份，空 Avatar 仅允许玩家持久授予。 */
    struct FGrantBinding
    {
        TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
        TWeakObjectPtr<AActor> Owner;
        TWeakObjectPtr<AActor> Avatar;
        bool bHadAvatar;
        EHodgeAbilityGrantContext Context;

        FGrantBinding(UHodgeAbilitySystemComponent* InASC, EHodgeAbilityGrantContext InContext)
            : ASC(InASC), Owner(InASC ? InASC->GetOwnerActor() : nullptr),
              Avatar(InASC ? InASC->GetAvatarActor() : nullptr), bHadAvatar(Avatar.IsValid()), Context(InContext) {}

        /** 同步回调更换或销毁绑定对象后，本批授予必须停止。 */
        bool IsCurrent() const
        {
            return ASC.IsValid() && ASC->IsRegistered() && Owner.IsValid() && !Owner->IsActorBeingDestroyed() &&
                ASC->GetOwnerActor() == Owner.Get() && ASC->GetAvatarActor() == Avatar.Get() &&
                (bHadAvatar ? Avatar.IsValid() && !Avatar->IsActorBeingDestroyed()
                            : Context == EHodgeAbilityGrantContext::OwnerInitialized);
        }
    };
}

/** 执行唯一授予循环，任何失败均由批次调用者清理 Pending 句柄。 */
FHodgeAbilityGrantResult UHodgeAbilitySet::GrantInternal(UHodgeAbilitySystemComponent* ASC,
    FHodgeAbilitySet_GrantedHandles& OutHandles, UObject* SourceObject, EHodgeAbilityGrantContext Context) const
{
    FHodgeAbilityGrantResult Result;
    const FGrantBinding Binding(ASC, Context);
    auto Fail = [&](const TCHAR* Message) { Result.Error = Message; return Result; };
    if (!Binding.IsCurrent() || !ASC->IsOwnerActorAuthoritative())
    { return Fail(TEXT("Ability grants require a registered authoritative ASC and current binding")); }

    for (const auto& Entry : GrantedAttributes)
    {
        if (!IsValid(Entry.AttributeSet)) { return Fail(TEXT("Invalid AttributeSet")); }
        auto* Set = NewObject<UAttributeSet>(ASC->GetOwner(), Entry.AttributeSet);
        ASC->AddAttributeSetSubobject(Set);
        OutHandles.AddAttributeSet(Set);
        if (!Binding.IsCurrent()) { return Fail(TEXT("Binding changed during AttributeSet grant")); }
    }
    for (const auto& Entry : GrantedGameplayAbilities)
    {
        if (!IsValid(Entry.Ability)) { return Fail(TEXT("Invalid ability")); }
        auto* CDO = Entry.Ability->GetDefaultObject<UHodgeGameplayAbility>();
        // OnGiveAbility 可能同步激活 OnSpawn，失败时不能承诺撤回全部外部副作用。
        Result.bHasIrreversibleEffects |= CDO->GetActivationPolicy() == EHodgeAbilityActivationPolicy::OnSpawn;
        FGameplayAbilitySpec Spec(CDO, Entry.AbilityLevel);
        Spec.SourceObject = SourceObject;
        if (Entry.InputTag.IsValid()) { Spec.GetDynamicSpecSourceTags().AddTag(Entry.InputTag); }
        const auto Handle = ASC->GiveAbility(Spec);
        OutHandles.AddAbilitySpecHandle(Handle);
        if (!Binding.IsCurrent() || !Handle.IsValid() || !ASC->FindAbilitySpecFromHandle(Handle))
        { return Fail(TEXT("Ability grant failed or binding changed")); }
    }
    for (const auto& Entry : GrantedAbilityDefinitions)
    {
        const auto Handle = ASC->GiveAbilityDefinition(Entry.Definition, Entry.AbilityLevel, SourceObject);
        OutHandles.AddAbilitySpecHandle(Handle);
        if (!Binding.IsCurrent() || !Handle.IsValid() || !ASC->FindAbilitySpecFromHandle(Handle))
        { return Fail(TEXT("Definition grant failed or binding changed")); }
    }
    for (const auto& Entry : GrantedGameplayEffects)
    {
        if (!IsValid(Entry.GameplayEffect)) { return Fail(TEXT("Invalid GameplayEffect")); }
        const auto* Effect = Entry.GameplayEffect.GetDefaultObject();
        const auto Handle = ASC->ApplyGameplayEffectToSelf(Effect, Entry.EffectLevel, ASC->MakeEffectContext());
        OutHandles.AddGameplayEffectHandle(Handle);
        Result.bHasIrreversibleEffects |= Handle.WasSuccessfullyApplied() &&
            Effect->DurationPolicy == EGameplayEffectDurationType::Instant;
        if (!Binding.IsCurrent() || !Handle.WasSuccessfullyApplied())
        { return Fail(TEXT("GameplayEffect grant failed or binding changed")); }
    }
    Result.bSucceeded = Binding.IsCurrent() && OutHandles.AreValidFor(ASC);
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

/** 兼容旧签名，但不再保留宽松执行语义；生产调用者使用返回结果的接口。 */
void UHodgeAbilitySet::GiveToAbilitySystem(UHodgeAbilitySystemComponent* ASC,
    FHodgeAbilitySet_GrantedHandles* OutHandles, UObject* SourceObject) const
{
    FHodgeAbilitySet_GrantedHandles UntrackedHandles;
    const auto Result = TryGiveToAbilitySystem(ASC, OutHandles ? *OutHandles : UntrackedHandles,
        SourceObject, false, EHodgeAbilityGrantContext::OwnerInitialized);
    if (!Result.bSucceeded)
    { UE_LOG(LogTemp, Error, TEXT("AbilitySet grant failed: %s (%s)"), *GetPathName(), *Result.Error); }
}

/** 单资产与整批提交复用同一预校验、执行及失败清理路径。 */
FHodgeAbilityGrantResult UHodgeAbilitySet::TryGiveToAbilitySystem(UHodgeAbilitySystemComponent* ASC,
    FHodgeAbilitySet_GrantedHandles& OutHandles, UObject* SourceObject, bool bForInitialization,
    EHodgeAbilityGrantContext Context) const
{
    return TryGiveBatchToAbilitySystem({this}, ASC, OutHandles, SourceObject, bForInitialization, Context);
}

/** 整批配置先验证，成功后才把来源句柄发布给调用者。 */
FHodgeAbilityGrantResult UHodgeAbilitySet::TryGiveBatchToAbilitySystem(const TArray<const UHodgeAbilitySet*>& Sets,
    UHodgeAbilitySystemComponent* ASC, FHodgeAbilitySet_GrantedHandles& OutHandles, UObject* SourceObject,
    bool bForInitialization, EHodgeAbilityGrantContext Context)
{
    FHodgeAbilityGrantResult Result;
    const FGrantBinding Binding(ASC, Context);
    if (!Binding.IsCurrent() || !ASC->IsOwnerActorAuthoritative())
    { Result.Error = TEXT("ASC grant context is not ready or lacks authority"); return Result; }
    TArray<FText> Errors;
    if (!ValidateBatchForGrant(Sets, ASC, Errors, bForInitialization))
    { Result.Error = Errors[0].ToString(); return Result; }

    // 同步授予回调可以触发 GC；提交结束前持有 ASC 和本批配置。
    TStrongObjectPtr<UHodgeAbilitySystemComponent> KeepASC(ASC);
    TArray<TStrongObjectPtr<const UHodgeAbilitySet>> KeepSets;
    for (const auto* Set : Sets) { KeepSets.Emplace(Set); }
    FHodgeAbilitySet_GrantedHandles Pending;
    for (const auto* Set : Sets)
    {
        const auto Step = Set->GrantInternal(ASC, Pending, SourceObject, Context);
        Result.bHasIrreversibleEffects |= Step.bHasIrreversibleEffects;
        if (!Step.bSucceeded) { Result.Error = Step.Error; break; }
    }
    Result.bSucceeded = Result.Error.IsEmpty() && Binding.IsCurrent() && Pending.AreValidFor(ASC);
    if (Result.bSucceeded) { OutHandles.Append(MoveTemp(Pending)); }
    else
    {
        if (Result.Error.IsEmpty()) { Result.Error = TEXT("Batch binding or resources changed before commit"); }
        Pending.TakeFromAbilitySystem(ASC);
    }
    return Result;
}

/** 核心属性集同族或普通属性集继承重叠均不可重复驻留。 */
bool UHodgeAbilitySet::AreAttributeSetTypesConflicting(const UClass* First, const UClass* Second)
{
    if (!First || !Second) { return false; }
    return First->IsChildOf(Second) || Second->IsChildOf(First) ||
        (First->IsChildOf(UHodgeHealthSet::StaticClass()) && Second->IsChildOf(UHodgeHealthSet::StaticClass())) ||
        (First->IsChildOf(UHodgeCombatSet::StaticClass()) && Second->IsChildOf(UHodgeCombatSet::StaticClass()));
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
			if (Existing && AreAttributeSetTypesConflicting(Type, Existing->GetClass()))
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
			for (const UClass* Type : Attributes)
			{ if (AreAttributeSetTypesConflicting(Type, Entry.AttributeSet.Get())) { Errors.Add(FText::FromString(TEXT("Duplicate AttributeSet across AbilitySets"))); break; } }
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
