/** @file HodgeEnemyInitComponent.cpp
 * @brief 实现怪物初始化协调组件相关的类型、接口与现有处理流程。
 */
#include "Component/HodgeEnemyInitComponent.h"
#include "Character/HodgeEnemyCharacter.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Component/HodgeExperienceManagerComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Equipment/HodgeEquipmentDefinition.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEnemyInitComponent)

const FName UHodgeEnemyInitComponent::NAME_ActorFeatureName(TEXT("EnemyInit"));

/** 构造怪物初始化协调组件并初始化其默认状态。 */
UHodgeEnemyInitComponent::UHodgeEnemyInitComponent(const FObjectInitializer& Initializer) : Super(Initializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

/** 组件注册后接入当前Owner的依赖。 */
void UHodgeEnemyInitComponent::OnRegister()
{
	Super::OnRegister();
	RegisterInitStateFeature();
}

/** 组件注销时解除依赖并清理本组件资源。 */
void UHodgeEnemyInitComponent::OnUnregister()
{
	bClosed = true; ++Generation; Result = EHodgeEnemyInitializationResult::Invalidated;
	Cleanup();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Extension.IsValid()) { Extension->UnregisterAbilitySystemDelegates(this); Extension->OnPawnDataChanged.RemoveAll(this); }
	if (GetWorld()) { GetWorld()->GameStateSetEvent.Remove(GameStateHandle); }
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	OnReadinessChanged.Broadcast(); OnReadinessChanged.Clear(); UnregisterInitStateFeature();
	Super::OnUnregister();
}

/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
void UHodgeEnemyInitComponent::BeginPlay()
{
	Super::BeginPlay();
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	// 按当前对象生命周期建立状态监听。
	if (auto* World = GetWorld()) { GameStateHandle = World->GameStateSetEvent.AddUObject(this, &ThisClass::HandleGameStateSet); }
	BindDependencies();
	TryInitialize();
}

/** 绑定Dependencies。 */
void UHodgeEnemyInitComponent::BindDependencies()
{
	if (bClosed) { return; }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Extension.IsValid())
	{
		Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(GetOwner());
		if (Extension.IsValid())
		{
			Extension->OnPawnDataChanged.AddUObject(this, &ThisClass::CheckDefaultInitialization);
			Extension->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::HandleASCUninitialized));
		}
	}
	if (!bRequireExperience) { bExperienceReady = true; return; }
	if (bExperienceBound || !GetWorld() || !GetWorld()->GetGameState()) { return; }
	if (auto* Experience = GetWorld()->GetGameState()->FindComponentByClass<UHodgeExperienceManagerComponent>())
	{
		bExperienceBound = true;
		Experience->CallOrRegister_OnExperienceLoaded(FOnHodgeExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::HandleExperienceLoaded));
	}
}

/** 处理游戏状态设置。 */
void UHodgeEnemyInitComponent::HandleGameStateSet(AGameStateBase*) { BindDependencies(); TryInitialize(); }
/** 处理Experience配置已加载。 */
void UHodgeEnemyInitComponent::HandleExperienceLoaded(const UHodgeExperienceDefinition*)
{
	if (bClosed) { return; }
	bExperienceReady = true; TryInitialize();
}
/** 尝试推进当前组件及相关Feature的初始化链。 */
void UHodgeEnemyInitComponent::CheckDefaultInitialization() { TryInitialize(); }
/** 处理Actor对象Init状态变化。 */
void UHodgeEnemyInitComponent::OnActorInitStateChanged(const FActorInitStateChangedParams&) { TryInitialize(); }

/** 只判断初始化阶段能否推进，不承担阶段副作用。 */
bool UHodgeEnemyInitComponent::CanChangeInitState(UGameFrameworkComponentManager*, FGameplayTag Current, FGameplayTag Desired) const
{
	const auto* Enemy = Cast<AHodgeEnemyCharacter>(GetOwner());
	if (!Enemy || bClosed || Result == EHodgeEnemyInitializationResult::Failed || Result == EHodgeEnemyInitializationResult::Invalidated) { return false; }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Current.IsValid() && Desired == HodgeGameplayTags::InitState_Spawned) { return Enemy->GetNativeEnemyAbilitySystem() != nullptr; }
	// 按服务器权限区分本次处理分支。
	if (Current == HodgeGameplayTags::InitState_Spawned && Desired == HodgeGameplayTags::InitState_DataAvailable)
	{ return bExperienceReady && Enemy->EnemyPawnData && Extension.IsValid() && (!Enemy->HasAuthority() || Enemy->GetController()); }
	if (Current == HodgeGameplayTags::InitState_DataAvailable && Desired == HodgeGameplayTags::InitState_DataInitialized)
	{ return IsBindingCurrent() && (!Enemy->HasAuthority() || (bStatsApplied && bBaseGranted && bEquipmentPrepared && HasValidCombatDependencies())); }
	return Current == HodgeGameplayTags::InitState_DataInitialized && Desired == HodgeGameplayTags::InitState_GameplayReady && IsBindingCurrent();
}

/** 按已有依赖与状态尝试推进初始化。 */
EHodgeEnemyInitializationResult UHodgeEnemyInitComponent::TryInitialize()
{
	if (bClosed) { return EHodgeEnemyInitializationResult::Invalidated; }
	if (Result == EHodgeEnemyInitializationResult::Failed || Result == EHodgeEnemyInitializationResult::Invalidated) { return Result; }
	if (bTrying) { bRecheck = true; return Result; }
	TGuardValue<bool> Guard(bTrying, true);
	// 状态通知可同步重入，只允许有限的追加检查，外部事件仍可再次推进。
	for (int32 Pass = 0; Pass < 4; ++Pass)
	{
		bRecheck = false;
		Result = InitializeStep();
		if (!bRecheck || bClosed || Result == EHodgeEnemyInitializationResult::Failed || Result == EHodgeEnemyInitializationResult::Invalidated) { break; }
	}
	return Result;
}

/** 初始化步骤。 */
EHodgeEnemyInitializationResult UHodgeEnemyInitComponent::InitializeStep()
{
	BindDependencies();
	auto* Enemy = Cast<AHodgeEnemyCharacter>(GetOwner());
	if (!Enemy || Enemy->IsActorBeingDestroyed() || !IsRegistered()) { return EHodgeEnemyInitializationResult::Invalidated; }
	auto* NativeASC = Enemy->GetNativeEnemyAbilitySystem();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Extension.IsValid() || !NativeASC) { return Wait(TEXT("Waiting for PawnExtension/ASC registration")); }
	if (auto* Existing = Extension->GetHodgeAbilitySystemComponent(); Existing && Existing != NativeASC)
	{ return Fail(TEXT("ASC identity conflict: PawnExtension is bound to another ASC")); }
	if (BoundASC.IsValid() && !IsBindingCurrent()) { Cleanup(); return EHodgeEnemyInitializationResult::Invalidated; }
	if (IsGameplayReady()) { return EHodgeEnemyInitializationResult::Ready; }
	if (!GetComponentManager()) { return Wait(TEXT("Waiting for GameFrameworkComponentManager")); }
	TryToChangeInitState(HodgeGameplayTags::InitState_Spawned);
	if (!bExperienceReady) { return Wait(TEXT("Waiting for Experience Loaded")); }
	const UHodgePawnData* Data = Enemy->EnemyPawnData;
	// 按服务器权限区分本次处理分支。
	if (!Data) { return Enemy->HasAuthority() ? Fail(TEXT("Missing required EnemyPawnData")) : Wait(TEXT("Waiting for replicated EnemyPawnData")); }
	if (!Data->StatProfile) { return Fail(TEXT("Missing required StatProfile")); }
	if (BoundData && BoundData != Data) { return Fail(TEXT("PawnData changed during the initialization cycle")); }
	if (Enemy->HasAuthority())
	{
		if (const auto* Existing = Extension->GetPawnData<UHodgePawnData>(); Existing && Existing != Data)
		{ return Fail(TEXT("PawnData conflict: refusing Hero or other configuration")); }
		Extension->SetPawnData(Data);
		if (!Enemy->GetController()) { return Wait(TEXT("Waiting for Possess")); }
	}
	if (Extension->GetPawnData<UHodgePawnData>() != Data) { return Wait(TEXT("Waiting for matching replicated PawnData")); }
	BoundData = Data;
	TryToChangeInitState(HodgeGameplayTags::InitState_DataAvailable);
	Extension->CheckDefaultInitialization();
	if (!Extension->HasReachedInitState(HodgeGameplayTags::InitState_DataInitialized)) { return Wait(TEXT("Waiting for PawnExtension DataInitialized")); }
	if (!NativeASC->IsRegistered()) { return Wait(TEXT("Waiting for ASC registration")); }
	if (Enemy->HasAuthority())
	{
		TArray<FText> Errors;
		float Health, Damage;
		if (!Data->StatProfile->Validate(Errors) || !Data->StatProfile->Evaluate(Enemy->InitialLevel, Health, Damage))
		{ return Fail(Errors.IsEmpty() ? TEXT("Invalid stat level") : Errors[0].ToString()); }
		TArray<const UHodgeAbilitySet*> Sets;
		if (!bBaseGranted)
		{
			for (const UHodgeAbilitySet* Set : Data->AbilitySets) { Sets.Add(Set); }
			if (Data->DefaultWeaponDefinition)
			{
				const auto* Weapon = GetDefault<UHodgeEquipmentDefinition>(Data->DefaultWeaponDefinition);
				for (const UHodgeAbilitySet* Set : Weapon->AbilitySetsToGrant) { Sets.Add(Set); }
			}
			if (!UHodgeAbilitySet::ValidateBatchForGrant(Sets, NativeASC, Errors, true)) { return Fail(Errors[0].ToString()); }
		}
		if (Data->DefaultWeaponDefinition && !bEquipmentPrepared &&
			!UHodgeEquipmentManagerComponent::ValidateEquipmentForGrant(Data->DefaultWeaponDefinition, NativeASC, 1, Errors, true))
		{ return Fail(Errors[0].ToString()); }
	}
	if (!NativeASC->GetSet<UHodgeHealthSet>()) { NativeASC->AddAttributeSetSubobject(Enemy->EnemyHealthSet.Get()); }
	if (!NativeASC->GetSet<UHodgeCombatSet>()) { NativeASC->AddAttributeSetSubobject(Enemy->EnemyCombatSet.Get()); }
	BoundASC = NativeASC;
	Extension->InitializeAbilitySystem(NativeASC, Enemy);
	if (!IsBindingCurrent() || bClosed) { return EHodgeEnemyInitializationResult::Invalidated; }
	if (Enemy->HasAuthority())
	{
		if (!bStatsApplied)
		{
			float Health, Damage; Data->StatProfile->Evaluate(Enemy->InitialLevel, Health, Damage);
			auto Spec = NativeASC->MakeOutgoingSpec(Data->StatProfile->InitializationEffect, Enemy->InitialLevel, NativeASC->MakeEffectContext());
			if (!Spec.IsValid()) { return Fail(TEXT("Invalid initial attribute spec")); }
			Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Stat_MaxHealth, Health);
			Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Stat_BaseDamage, Damage);
			if (!NativeASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()).WasSuccessfullyApplied()) { return Fail(TEXT("Initial attribute effect failed")); }
			if (!IsBindingCurrent() || bClosed) { return EHodgeEnemyInitializationResult::Invalidated; }
			NativeASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), Health);
			bStatsApplied = true;
		}
		if (!bBaseGranted)
		{
			for (const UHodgeAbilitySet* Set : Data->AbilitySets)
			{
				const auto Grant = Set->TryGiveToAbilitySystem(NativeASC, BaseHandles, const_cast<UHodgePawnData*>(Data), true);
				if (!Grant.bSucceeded) { return Fail(Grant.Error); }
				if (!IsBindingCurrent() || bClosed) { Cleanup(); return EHodgeEnemyInitializationResult::Invalidated; }
			}
			bBaseGranted = true;
		}
		if (!bEquipmentPrepared)
		{
			if (Data->DefaultWeaponDefinition && !Enemy->FindComponentByClass<UHodgeEquipmentManagerComponent>())
			{ return Wait(TEXT("Waiting for required EquipmentManager")); }
			if (!Enemy->PrepareDefaultEquipment())
			{
				const auto* Equipment = Enemy->FindComponentByClass<UHodgeEquipmentManagerComponent>();
				return Fail(Equipment && !Equipment->GetLastEquipError().IsEmpty() ? Equipment->GetLastEquipError() : TEXT("Default equipment preparation failed"));
			}
			if (!IsBindingCurrent() || bClosed) { Cleanup(); return EHodgeEnemyInitializationResult::Invalidated; }
			bEquipmentPrepared = true;
		}
		if (!Enemy->FindComponentByClass<UHodgeCombatComponentBase>()) { return Wait(TEXT("Waiting for required CombatComponent")); }
	}
	TryToChangeInitState(HodgeGameplayTags::InitState_DataInitialized);
	TryToChangeInitState(HodgeGameplayTags::InitState_GameplayReady);
	if (!IsGameplayReady()) { return Wait(TEXT("Waiting for remaining combat dependencies")); }
	Diagnostic.Reset(); Result = EHodgeEnemyInitializationResult::Ready;
	// 向当前监听者发布状态变化，调用方继续负责后续处理。
	OnReadinessChanged.Broadcast();
	return IsBindingCurrent() && !bClosed ? Result : EHodgeEnemyInitializationResult::Invalidated;
}

/** 判断绑定当前。 */
bool UHodgeEnemyInitComponent::IsBindingCurrent() const
{
	return BoundASC.IsValid() && Extension.IsValid() && BoundASC->IsRegistered() &&
		Extension->GetHodgeAbilitySystemComponent() == BoundASC.Get() &&
		BoundASC->GetOwnerActor() == GetOwner() && BoundASC->GetAvatarActor() == GetOwner();
}

/** 查询本Feature是否已完成当前初始化链。 */
bool UHodgeEnemyInitComponent::IsGameplayReady() const
{
	return !bClosed && IsRegistered() && GetOwner() && !GetOwner()->IsActorBeingDestroyed() &&
		Result != EHodgeEnemyInitializationResult::Failed && Result != EHodgeEnemyInitializationResult::Invalidated &&
		HasReachedInitState(HodgeGameplayTags::InitState_GameplayReady);
}

/** 检查当前怪物的ASC、基础授予和必要装备依赖。 */
bool UHodgeEnemyInitComponent::HasValidCombatDependencies() const
{
	const auto* Enemy = Cast<AHodgeEnemyCharacter>(GetOwner());
	// 统一战斗组件。
	const auto* Combat = GetOwner() ? GetOwner()->FindComponentByClass<UHodgeCombatComponentBase>() : nullptr;
	return Enemy && !bClosed && IsBindingCurrent() && BoundData == Enemy->EnemyPawnData && Combat && Combat->CanExecuteAbilities() &&
		(!Enemy->HasAuthority() || (bStatsApplied && bBaseGranted && BaseHandles.AreValidFor(BoundASC.Get()) && Enemy->IsDefaultEquipmentReady()));
}

/** 检查怪物是否已就绪且当前未被死亡或控制限制。 */
bool UHodgeEnemyInitComponent::CanExecuteAIAction() const
{
	const auto* Health = BoundASC.IsValid() ? BoundASC->GetSet<UHodgeHealthSet>() : nullptr;
	return GetOwner() && GetOwner()->HasAuthority() && IsGameplayReady() && HasValidCombatDependencies() &&
		Health && Health->GetHealth() > 0.f &&
		CastChecked<APawn>(GetOwner())->GetController() && !BoundASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death) &&
		!BoundASC->HasMatchingGameplayTag(HodgeHitReactionTags::Controlled);
}

/** 等待当前阶段满足完成条件。 */
EHodgeEnemyInitializationResult UHodgeEnemyInitComponent::Wait(const TCHAR* Reason)
{ Diagnostic = Reason; return EHodgeEnemyInitializationResult::Waiting; }

/** 记录当前初始化或执行失败并完成必要清理。 */
EHodgeEnemyInitializationResult UHodgeEnemyInitComponent::Fail(const FString& Reason)
{
	Diagnostic = Reason; Result = EHodgeEnemyInitializationResult::Failed;
	UE_LOG(LogTemp, Warning, TEXT("EnemyInit failed [%s]: %s"), *GetNameSafe(GetOwner()), *Reason);
	Cleanup(); OnReadinessChanged.Broadcast();
	return Result;
}

/** 处理ASCUninitialized。 */
void UHodgeEnemyInitComponent::HandleASCUninitialized()
{
	if (bCleaning || bClosed) { return; }
	Result = EHodgeEnemyInitializationResult::Invalidated;
	Diagnostic = TEXT("ASC binding was removed"); Cleanup(); OnReadinessChanged.Broadcast();
}

/** 按当前对象的来源归属解除监听并清理资源。 */
void UHodgeEnemyInitComponent::Cleanup()
{
	if (bCleaning) { return; }
	TGuardValue<bool> Guard(bCleaning, true);
	// 按服务器权限区分本次处理分支。
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (auto* Enemy = Cast<AHodgeEnemyCharacter>(GetOwner())) { Enemy->UninitializeDefaultEquipment(); }
		if (BoundASC.IsValid()) { BaseHandles.TakeFromAbilitySystem(BoundASC.Get()); }
	}
}

/** 离开世界运行阶段并解除本对象持有的监听或资源。 */
void UHodgeEnemyInitComponent::EndPlay(EEndPlayReason::Type Reason)
{
	bClosed = true; ++Generation; Cleanup();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Extension.IsValid()) { Extension->UnregisterAbilitySystemDelegates(this); Extension->OnPawnDataChanged.RemoveAll(this); }
	if (GetWorld()) { GetWorld()->GameStateSetEvent.Remove(GameStateHandle); }
	OnReadinessChanged.Clear(); UnregisterInitStateFeature();
	Super::EndPlay(Reason);
}
