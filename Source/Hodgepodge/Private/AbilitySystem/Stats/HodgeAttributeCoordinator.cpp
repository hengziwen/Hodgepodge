/** @file HodgeAttributeCoordinator.cpp
 * @brief 实现PlayerState属性协调器相关的类型、接口与现有处理流程。
 */
#include "AbilitySystem/Stats/HodgeAttributeCoordinator.h"
#include "AbilitySystem/HodgeGameplayTags.h"

#include "Core/PlayState/HodgePlayerState.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "Component/HodgeHealthComponent.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeEquipmentStatProfile.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Equipment/HodgeEquipmentDefinition.h"
#include "Inventory/HodgeInventoryManagerComponent.h"
#include "Inventory/HodgeInventoryItemInstance.h"
#include "Inventory/HodgeInventoryItemDefinition.h"
#include "Inventory/HodgeEquipmentSelectionComponent.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAttributeCoordinator)

AHodgePlayerState* UHodgeAttributeCoordinator::GetPlayerState() const { return GetTypedOuter<AHodgePlayerState>(); }
/** 判断已绑定目标。 */
bool UHodgeAttributeCoordinator::IsBoundTo(const APawn* Avatar) const { return Avatar && BoundAvatar.Get() == Avatar; }
/** 判断当前表现角色。 */
bool UHodgeAttributeCoordinator::IsCurrentAvatar() const { return BoundAvatar.IsValid() && AbilitySystem.IsValid() && AbilitySystem->GetAvatarActor() == BoundAvatar.Get(); }
/** 判断就绪关联。 */
bool UHodgeAttributeCoordinator::IsReadyFor(const APawn* Avatar) const
{
	// 玩家状态对象。
	const auto* PS = GetPlayerState();
	return PS && IsBoundTo(Avatar) && IsCurrentAvatar() && PS->AttributeReadyState.bReady;
}

/** 校验CoreSets。 */
bool UHodgeAttributeCoordinator::ValidateCoreSets() const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!AbilitySystem.IsValid()) { return false; }
	int32 HealthCount = 0, CombatCount = 0;
	for (const UAttributeSet* Set : AbilitySystem->GetSpawnedAttributes())
	{
		if (Set && Set->IsA<UHodgeHealthSet>()) { ++HealthCount; }
		if (Set && Set->IsA<UHodgeCombatSet>()) { ++CombatCount; }
	}
	return HealthCount == 1 && CombatCount == 1;
}

/** 发布当前Avatar的属性就绪状态。 */
void UHodgeAttributeCoordinator::PublishReady(bool bReady)
{
	// 玩家状态对象。
	auto* PS = GetPlayerState();
	// 按服务器权限区分本次处理分支。
	if (!PS || !PS->HasAuthority()) { return; }
	PS->AttributeReadyState.Avatar = BoundAvatar.Get();
	PS->AttributeReadyState.LifeGeneration = LifeGeneration;
	PS->AttributeReadyState.bReady = bReady;
	PS->AttributeReadyState.Revision = PS->AttributeReadyState.Revision == MAX_int32 ? 1 : PS->AttributeReadyState.Revision + 1;
	PS->ForceNetUpdate();
	if (auto* World = PS->GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(PS, [PS]() { PS->NotifyAttributeReadiness(); }));
	}
}

/** 开始更新。 */
bool UHodgeAttributeCoordinator::BeginUpdate(EHodgeAttributeUpdateReason Reason)
{
	// 按服务器权限区分本次处理分支。
	if (bUpdating || !GetPlayerState() || !GetPlayerState()->HasAuthority() || !IsCurrentAvatar() || !HealthSet.IsValid()) { return false; }
	bWasReady = GetPlayerState()->AttributeReadyState.bReady;
	bUpdating = true;
	UpdateReason = Reason;
	OldHealth = HealthSet->GetHealth();
	OldMaxHealth = FMath::Max(1.f, HealthSet->GetMaxHealth());
	OldBaseHealth = AbilitySystem->GetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute());
	OldBaseDamage = AbilitySystem->GetNumericAttributeBase(UHodgeCombatSet::GetBaseDamageAttribute());
	HealthSet->BeginAttributeRebuild();
	PublishReady(false);
	return true;
}

/** 应用角色基类。 */
bool UHodgeAttributeCoordinator::ApplyCharacterBase(int32 Level)
{
	float Health, Damage;
	if (!Profile || !Profile->Evaluate(Level, Health, Damage) || !IsCurrentAvatar()) { return false; }
	auto Context = AbilitySystem->MakeEffectContext();
	Context.AddSourceObject(Profile);
	auto Spec = AbilitySystem->MakeOutgoingSpec(Profile->InitializationEffect, Level, Context);
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Spec.IsValid()) { return false; }
	Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Stat_MaxHealth, Health);
	Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Stat_BaseDamage, Damage);
	// 将构造的效果规格提交给GAS执行属性更新。
	const bool bApplied = AbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()).WasSuccessfullyApplied();
	return bApplied;
}

/** 准备表现角色。 */
bool UHodgeAttributeCoordinator::PrepareAvatar(APawn* Avatar, const UHodgePawnData* Data)
{
	// 玩家状态对象。
	auto* PS = GetPlayerState();
	// 按服务器权限区分本次处理分支。
	if (!PS || !PS->HasAuthority() || !IsValid(Avatar) || !Data || bUpdating) { return false; }
	if (IsReadyFor(Avatar) && !bExpectRespawn && Profile == Data->StatProfile && AppliedProfileVersion == Profile->ConfigurationVersion) { return true; }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (IsReadyFor(Avatar) && HealthSet.IsValid()) { SavedHealth = HealthSet->GetHealth(); }
	if (BoundAvatar.IsValid() && BoundAvatar.Get() != Avatar) { DetachAvatar(BoundAvatar.Get()); }
	const bool bNewLife = BoundAvatar.Get() != Avatar || bExpectRespawn;
	BoundAvatar = Avatar;
	AbilitySystem = PS->GetHodgeAbilitySystemComponent();
	PawnData = Data;
	Profile = Data->StatProfile;
	HealthSet = const_cast<UHodgeHealthSet*>(AbilitySystem->GetSet<UHodgeHealthSet>());
	if (bNewLife) { LifeGeneration = LifeGeneration == MAX_int32 ? 1 : LifeGeneration + 1; }
	PublishReady(false);
	TArray<FText> Errors;
	float Health, Damage;
	if (!IsCurrentAvatar() || !ValidateCoreSets() || !Profile || !Profile->Validate(Errors) || !Profile->Evaluate(PS->CharacterProgression.Level, Health, Damage))
	{
		UE_LOG(LogTemp, Error, TEXT("Attribute initialization rejected for %s: invalid profile, core sets or level"), *GetNameSafe(Avatar));
		return false;
	}
	if (!PS->CharacterProgression.CharacterId.IsValid()) { PS->CharacterProgression.CharacterId = FGuid::NewGuid(); }
	const auto Reason = bExpectRespawn ? EHodgeAttributeUpdateReason::Respawn
		: (bHasSavedHealth ? EHodgeAttributeUpdateReason::Rebind : EHodgeAttributeUpdateReason::FirstSpawn);
	if (!BeginUpdate(Reason)) { return false; }
	bBootstrap = true;
	if (!ApplyCharacterBase(PS->CharacterProgression.Level)) { CommitUpdate(false); return false; }
	return true;
}

/** 完成本Avatar的属性和装备准备并发布结果。 */
bool UHodgeAttributeCoordinator::CompleteAvatarInitialization()
{
	const auto* PS = GetPlayerState();
	if (!bUpdating) { return IsReadyFor(BoundAvatar.Get()); }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!bBootstrap || !PawnData || !IsCurrentAvatar()) { return CommitUpdate(false); }
	bool bHasEquipment = true;
	if (PawnData->bUseInventory)
	{
		const auto* Selection = PS->FindComponentByClass<UHodgeEquipmentSelectionComponent>();
		bHasEquipment = Selection && Selection->IsSelectionApplied();
	}
	else if (PawnData->DefaultWeaponDefinition)
	{
		const auto* Manager = BoundAvatar->FindComponentByClass<UHodgeEquipmentManagerComponent>();
		bHasEquipment = Manager && Manager->FindInstanceOfDefinition(PawnData->DefaultWeaponDefinition);
	}
	return CommitUpdate(bHasEquipment);
}

/** 提交更新。 */
bool UHodgeAttributeCoordinator::CommitUpdate(bool bSuccess)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!bUpdating || !HealthSet.IsValid()) { return false; }
	bSuccess &= IsCurrentAvatar();
	if (!bSuccess && AbilitySystem.IsValid())
	{
		AbilitySystem->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), OldBaseHealth);
		AbilitySystem->SetNumericAttributeBase(UHodgeCombatSet::GetBaseDamageAttribute(), OldBaseDamage);
	}
	bool bReachedZero = false;
	const float Delta = HealthSet->EndAttributeRebuild(bReachedZero);
	// 本次操作的目标。
	float Target = OldHealth;
	if (bSuccess)
	{
		if (UpdateReason == EHodgeAttributeUpdateReason::FirstSpawn || UpdateReason == EHodgeAttributeUpdateReason::Respawn)
		{
			Target = HealthSet->GetMaxHealth();
			if (!bReachedZero)
			{
				if (auto* Component = UHodgeHealthComponent::FindHealthComponent(BoundAvatar.Get())) { Component->ResetForSpawn(); }
			}
		}
		else if (UpdateReason == EHodgeAttributeUpdateReason::LevelUp) { Target = OldHealth / OldMaxHealth * HealthSet->GetMaxHealth(); }
		else if (UpdateReason == EHodgeAttributeUpdateReason::Rebind && bHasSavedHealth) { Target = SavedHealth; }
	}
	Target += Delta;
	if (bReachedZero || ((UpdateReason != EHodgeAttributeUpdateReason::FirstSpawn && UpdateReason != EHodgeAttributeUpdateReason::Respawn) && OldHealth <= 0.f)) { Target = 0.f; }
	if (IsCurrentAvatar()) { bSuccess &= HealthSet->SetHealthForAttributeCommit(Target); }
	const bool bRestoreReady = !bSuccess && !bBootstrap && bWasReady && IsCurrentAvatar();
	bUpdating = false;
	bBootstrap = false;
	if (bSuccess)
	{
		bHasSavedHealth = true;
		SavedHealth = HealthSet->GetHealth();
		bExpectRespawn = false;
		if (Profile && UpdateReason != EHodgeAttributeUpdateReason::EquipmentChange) { AppliedProfileVersion = Profile->ConfigurationVersion; }
	}
	PublishReady(bSuccess || bRestoreReady);
	return bSuccess;
}

/** 解除当前Avatar相关的属性与监听绑定。 */
void UHodgeAttributeCoordinator::DetachAvatar(APawn* Avatar)
{
	if (!IsBoundTo(Avatar)) { return; }
	if (bUpdating) { CommitUpdate(false); }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (HealthSet.IsValid()) { SavedHealth = HealthSet->GetHealth(); bHasSavedHealth = true; }
	PublishReady(false);
	BoundAvatar.Reset();
	AbilitySystem.Reset();
	HealthSet.Reset();
	PawnData = nullptr;
	Profile = nullptr;
}

/** 设置角色等级。 */
bool UHodgeAttributeCoordinator::SetCharacterLevel(int32 Level)
{
	// 玩家状态对象。
	auto* PS = GetPlayerState();
	float Health, Damage;
	// 按服务器权限区分本次处理分支。
	if (!PS || !PS->HasAuthority() || !IsReadyFor(BoundAvatar.Get()) || !Profile || !Profile->Evaluate(Level, Health, Damage)) { return false; }
	if (PS->CharacterProgression.Level == Level) { return true; }
	if (!BeginUpdate(EHodgeAttributeUpdateReason::LevelUp)) { return false; }
	const int32 PreviousLevel = PS->CharacterProgression.Level;
	const bool bApplied = ApplyCharacterBase(Level);
	if (bApplied) { PS->CharacterProgression.Level = Level; }
	if (!CommitUpdate(bApplied)) { PS->CharacterProgression.Level = PreviousLevel; return false; }
	PS->CharacterProgression.Revision = PS->CharacterProgression.Revision == MAX_int32 ? 1 : PS->CharacterProgression.Revision + 1;
	PS->ForceNetUpdate();
	return true;
}

/** 按当前属性范围恢复或设置角色生命。 */
bool UHodgeAttributeCoordinator::RestoreHealth(float Health)
{
	// 按服务器权限区分本次处理分支。
	if (!GetPlayerState() || !GetPlayerState()->HasAuthority() || !IsReadyFor(BoundAvatar.Get()) || bUpdating || !FMath::IsFinite(Health) || Health < 0.f) { return false; }
	if (HealthSet->GetHealth() <= 0.f && Health > 0.f) { return false; }
	const bool bResult = HealthSet->SetHealthForAttributeCommit(Health);
	SavedHealth = HealthSet->GetHealth();
	return bResult;
}

/** 开始装备更新。 */
bool UHodgeAttributeCoordinator::BeginEquipmentUpdate() { return IsReadyFor(BoundAvatar.Get()) && BeginUpdate(EHodgeAttributeUpdateReason::EquipmentChange); }
/** 结束装备更新。 */
void UHodgeAttributeCoordinator::FinishEquipmentUpdate(bool bSuccess) { CommitUpdate(bSuccess); }
/** 记录下一次Avatar绑定应按重生策略初始化。 */
void UHodgeAttributeCoordinator::ExpectRespawn() { if (GetPlayerState() && GetPlayerState()->HasAuthority()) { bExpectRespawn = LifeGeneration > 0; } }

/** 设置初始Saved生命值。 */
bool UHodgeAttributeCoordinator::SetInitialSavedHealth(float Health)
{
	// 按服务器权限区分本次处理分支。
	if (LifeGeneration != 0 || bUpdating || !GetPlayerState() || !GetPlayerState()->HasAuthority() || !FMath::IsFinite(Health) || Health < 0.f) { return false; }
	SavedHealth = Health;
	bHasSavedHealth = true;
	return true;
}

/** 获取Or创建默认装备。 */
FHodgeOwnedEquipmentState UHodgeAttributeCoordinator::GetOrCreateDefaultEquipment(TSubclassOf<UHodgeEquipmentDefinition> Definition)
{
	// 玩家状态对象。
	auto* PS = GetPlayerState();
	// 按服务器权限区分本次处理分支。
	if (!PS || !PS->HasAuthority() || !Definition) { return {}; }
	if (const auto* Inventory = PS->FindComponentByClass<UHodgeInventoryManagerComponent>(); Inventory && Inventory->IsInitialized())
	{
		for (const auto& View : Inventory->GetItems())
		{
			const auto* Fragment = GetDefault<UHodgeInventoryItemDefinition>(View.Instance->GetItemDef())->FindFragment<UHodgeInventoryFragment_EquippableItem>();
			if (Fragment && Fragment->EquipmentDefinition == Definition)
			{ FHodgeOwnedEquipmentState State; State.InstanceId = View.ItemId; State.Level = View.Instance->GetItemLevel(); State.Definition = Definition; return State; }
		}
		return {};
	}
	if (const auto* Existing = PS->OwnedEquipmentStates.FindByPredicate([Definition](const auto& State) { return State.Definition == Definition; })) { return *Existing; }
	FHodgeOwnedEquipmentState State;
	State.InstanceId = FGuid::NewGuid();
	State.Definition = Definition;
	const UHodgeEquipmentStatProfile* ProfileData = Definition->GetDefaultObject<UHodgeEquipmentDefinition>()->StatProfile.Get();
	State.Level = ProfileData ? ProfileData->MinLevel : 1;
	PS->OwnedEquipmentStates.Add(State);
	PS->ForceNetUpdate();
	return State;
}

/** 记录装备身份与等级以供后续属性协调。 */
void UHodgeAttributeCoordinator::RememberEquipment(FGuid Id, TSubclassOf<UHodgeEquipmentDefinition> Definition, int32 Level)
{
	// 玩家状态对象。
	auto* PS = GetPlayerState();
	// 按服务器权限区分本次处理分支。
	if (!PS || !PS->HasAuthority() || !Id.IsValid()) { return; }
	// 兼容记录只能镜像库存事实，不能成为第二个物品／等级写入口。
	if (const auto* Inventory = PS->FindComponentByClass<UHodgeInventoryManagerComponent>(); Inventory && Inventory->IsInitialized())
	{
		const auto* Item = Inventory->FindItemById(Id);
		const auto* Fragment = Item ? GetDefault<UHodgeInventoryItemDefinition>(Item->GetItemDef())->FindFragment<UHodgeInventoryFragment_EquippableItem>() : nullptr;
		if (!Fragment || Fragment->EquipmentDefinition != Definition || Item->GetItemLevel() != Level) { return; }
	}
	auto* State = PS->OwnedEquipmentStates.FindByPredicate([Id](const auto& Item) { return Item.InstanceId == Id; });
	if (!State) { State = &PS->OwnedEquipmentStates.AddDefaulted_GetRef(); State->InstanceId = Id; }
	State->Definition = Definition;
	State->Level = Level;
	PS->ForceNetUpdate();
}
