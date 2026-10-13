/** @file HodgeAttributeGrowthTests.cpp
 * @brief 实现属性Growth自动化测试相关的类型、接口与现有处理流程。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Character/HodgeCombatCharacter.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Equipment/HodgeEquipmentDefinition.h"
#include "Equipment/HodgeEquipmentInstance.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Stats/HodgeAttributeCoordinator.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Data/HodgeEquipmentStatProfile.h"
#include "Engine/World.h"
#include "Engine/CurveTable.h"

namespace HodgeAttributeGrowthTests
{
	/** 保存测试场景的数据和接口契约。 */
	struct FFixture
	{
		// 本次操作所在的世界。
		UWorld* World;
		// 玩家状态对象。
		AHodgePlayerState* PS;
		// 当前操作关联的Pawn角色。
		AHodgeCombatCharacter* Pawn;
		// 后续GAS操作使用的能力系统组件。
		UHodgeAbilitySystemComponent* ASC;
		// 装备的数据或对象引用。
		UHodgeEquipmentManagerComponent* Equipment;
		// 当前操作使用的数据。
		UHodgePawnData* Data;
		// Sword的数据或对象引用。
		TSubclassOf<UHodgeEquipmentDefinition> Sword;
		/** 构造测试场景并初始化其默认状态。 */
		FFixture()
		{
			const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
			World = UWorld::CreateWorld(EWorldType::EditorPreview, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			PS = World->SpawnActor<AHodgePlayerState>();
			Pawn = World->SpawnActor<AHodgeCombatCharacter>();
			ASC = PS->GetHodgeAbilitySystemComponent();
			// 先检查对象与上下文，避免在依赖失效后继续处理。
			if (!ASC->GetSet<UHodgeHealthSet>()) { ASC->AddAttributeSetSubobject(CastChecked<UHodgeHealthSet>(PS->GetDefaultSubobjectByName(TEXT("HealthSet")))); }
			if (!ASC->GetSet<UHodgeCombatSet>()) { ASC->AddAttributeSetSubobject(CastChecked<UHodgeCombatSet>(PS->GetDefaultSubobjectByName(TEXT("CombatSet")))); }
			Equipment = NewObject<UHodgeEquipmentManagerComponent>(Pawn);
			Pawn->AddInstanceComponent(Equipment);
			Equipment->RegisterComponent();
			if (!Equipment->HasBeenInitialized()) { Equipment->InitializeComponent(); }
			Data = NewObject<UHodgePawnData>(PS);
			Data->StatProfile = NewObject<UHodgeCharacterStatProfile>(Data);
			Data->StatProfile->MaxLevel = 2;
			auto* Curves = NewObject<UCurveTable>(Data->StatProfile);
			auto& Health = Curves->AddRichCurve(TEXT("MaxHealth"));
			Health.AddKey(1.f, 100.f); Health.AddKey(2.f, 120.f);
			auto& Damage = Curves->AddRichCurve(TEXT("BaseDamage"));
			Damage.AddKey(1.f, 20.f); Damage.AddKey(2.f, 22.f);
			Data->StatProfile->MaxHealth = FScalableFloat(1.f);
			Data->StatProfile->MaxHealth.Curve.CurveTable = Curves;
			Data->StatProfile->MaxHealth.Curve.RowName = TEXT("MaxHealth");
			Data->StatProfile->BaseDamage = FScalableFloat(1.f);
			Data->StatProfile->BaseDamage.Curve.CurveTable = Curves;
			Data->StatProfile->BaseDamage.Curve.RowName = TEXT("BaseDamage");
			Sword = LoadClass<UHodgeEquipmentDefinition>(nullptr, TEXT("/Game/Main/Data/Equipments/BP_Equipment_Sword.BP_Equipment_Sword_C"));
			ASC->InitAbilityActorInfo(PS, Pawn);
		}
		/** 初始化ialize。
		 * @param bSword 是否Sword的控制标记。
		 * @return true表示本次检查或操作成功，false表示失败。
		 */
		bool Initialize(bool bSword)
		{
			Data->DefaultWeaponDefinition = bSword ? Sword : nullptr;
			const auto State = PS->GetAttributeCoordinator()->GetOrCreateDefaultEquipment(Sword);
			if (!PS->GetAttributeCoordinator()->PrepareAvatar(Pawn, Data)) { return false; }
			if (bSword && !Equipment->EquipItemWithState(Sword, State.InstanceId, State.Level)) { return false; }
			return PS->GetAttributeCoordinator()->CompleteAvatarInitialization();
		}
		/** 将装备管理逻辑接入当前可用ASC。 */
		void ConnectEquipmentASC()
		{
			Data->DefaultWeaponDefinition = Sword;
			auto* Extension = Pawn->FindComponentByClass<UHodgePawnExtensionComponent>();
			Extension->SetPawnData(Data);
			Extension->InitializeAbilitySystem(ASC, PS);
		}
		/** 销毁该对象，执行当前类型的析构收尾。 */
		~FFixture()
		{
			PS->GetAttributeCoordinator()->DetachAvatar(Pawn);
			Equipment->UninitializeComponent();
			World->DestroyWorld(false);
		}
	};

	/** 应用伤害。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 * @param Value 值，由调用方传入。
	 */
	void ApplyDamage(UHodgeAbilitySystemComponent* ASC, float Value)
	{
		auto* Effect = NewObject<UGameplayEffect>();
		Effect->DurationPolicy = EGameplayEffectDurationType::Instant;
		FGameplayModifierInfo Mod;
		Mod.Attribute = UHodgeHealthSet::GetDamageAttribute();
		Mod.ModifierOp = EGameplayModOp::Additive;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Value));
		Effect->Modifiers.Add(Mod);
		ASC->ApplyGameplayEffectToSelf(Effect, 1.f, ASC->MakeEffectContext());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeStatProfileTest, "Hodge.Attributes.ProfileValidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeStatProfileTest::RunTest(const FString& Parameters)
{
	// 当前操作使用的配置对象。
	auto* Profile = NewObject<UHodgeCharacterStatProfile>();
	TArray<FText> Errors;
	TestTrue(TEXT("Default character profile contract is valid"), Profile->Validate(Errors));
	float Health, Damage;
	TestFalse(TEXT("Level below range rejected"), Profile->Evaluate(0, Health, Damage));
	Profile->MaxHealth = FScalableFloat(-10.f);
	Errors.Reset();
	TestFalse(TEXT("Negative maximum rejected"), Profile->Validate(Errors));
	auto* Gear = NewObject<UHodgeEquipmentStatProfile>();
	Errors.Reset();
	TestTrue(TEXT("Independent equipment effect contract is valid"), Gear->Validate(Errors));
	Gear->BaseDamageBonus = FScalableFloat(-1.f);
	Errors.Reset();
	TestFalse(TEXT("Negative equipment contribution rejected"), Gear->Validate(Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeStatGrowthTest, "Hodge.Attributes.GrowthEquipmentLifecycle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeStatGrowthTest::RunTest(const FString& Parameters)
{
	HodgeAttributeGrowthTests::FFixture F;
	if (!TestNotNull(TEXT("Default sword fixture exists"), F.Sword.Get())) { return false; }
	F.ConnectEquipmentASC();
	if (!TestTrue(TEXT("Bootstrap waits for and includes default sword"), F.Initialize(true))) { return false; }
	const auto* Health = F.ASC->GetSet<UHodgeHealthSet>();
	// 统一战斗组件。
	const auto* Combat = F.ASC->GetSet<UHodgeCombatSet>();
	TestEqual(TEXT("Spawn maximum contains sword"), Health->GetMaxHealth(), 150.f);
	TestEqual(TEXT("Spawn fills after sword, not before"), Health->GetHealth(), 150.f);
	TestEqual(TEXT("Attack includes weapon contribution"), Combat->GetBaseDamage(), 30.f);
	TestTrue(TEXT("Restore injured state"), F.PS->RestoreCharacterHealth(75.f));
	TestTrue(TEXT("Repeated bootstrap is idempotent"), F.PS->GetAttributeCoordinator()->PrepareAvatar(F.Pawn, F.Data));
	TestTrue(TEXT("Repeated completion remains ready"), F.PS->GetAttributeCoordinator()->CompleteAvatarInitialization());
	TestEqual(TEXT("Rebinding notification cannot heal"), Health->GetHealth(), 75.f);
	TestTrue(TEXT("Level two updates only character base"), F.PS->SetCharacterLevel(2));
	TestEqual(TEXT("Weapon remains after level change"), Health->GetMaxHealth(), 170.f);
	TestEqual(TEXT("Level change keeps resource ratio"), Health->GetHealth(), 85.f);
	TestEqual(TEXT("Attack recalculates base plus weapon"), Combat->GetBaseDamage(), 32.f);
	auto* Sword = F.Equipment->FindInstanceOfDefinition(F.Sword);
	const FGuid Identity = Sword->GetEquipmentId();
	TestTrue(TEXT("Weapon level refresh succeeds"), F.Equipment->SetEquipmentLevel(Sword, 2));
	TestEqual(TEXT("Weapon growth updates its contribution"), Health->GetMaxHealth(), 180.f);
	TestEqual(TEXT("Weapon upgrade does not heal"), Health->GetHealth(), 85.f);
	TestTrue(TEXT("Weapon identity preserved"), Sword->GetEquipmentId() == Identity);
	TestTrue(TEXT("Repeated weapon level update is idempotent"), F.Equipment->SetEquipmentLevel(Sword, 2));
	F.Equipment->UnequipItem(Sword);
	TestEqual(TEXT("Unequip removes only weapon contribution"), Health->GetMaxHealth(), 120.f);
	TestEqual(TEXT("Unequip keeps absolute resource"), Health->GetHealth(), 85.f);
	TestEqual(TEXT("Character attack base remains"), Combat->GetBaseDamage(), 22.f);
	TestFalse(TEXT("Invalid character level is rejected"), F.PS->SetCharacterLevel(100));
	TestTrue(TEXT("Invalid level leaves character ready"), F.PS->AreAttributesReadyFor(F.Pawn));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeStatReentryTest, "Hodge.Attributes.ReentryAndFailure", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeStatReentryTest::RunTest(const FString& Parameters)
{
	HodgeAttributeGrowthTests::FFixture F;
	if (!TestTrue(TEXT("Constant bootstrap"), F.Initialize(false))) { return false; }
	const auto* Health = F.ASC->GetSet<UHodgeHealthSet>();
	F.PS->RestoreCharacterHealth(50.f);
	bool bInjected = false;
	const auto Handle = F.ASC->GetGameplayAttributeValueChangeDelegate(UHodgeHealthSet::GetMaxHealthAttribute()).AddLambda([&F, &bInjected](const FOnAttributeChangeData&)
	{
		if (!bInjected && F.PS->GetAttributeCoordinator()->IsUpdating()) { bInjected = true; HodgeAttributeGrowthTests::ApplyDamage(F.ASC, 10.f); }
	});
	TestTrue(TEXT("Upgrade with reentrant damage succeeds"), F.PS->SetCharacterLevel(2));
	TestTrue(TEXT("Reentrant resource execution was observed"), bInjected);
	TestEqual(TEXT("Real damage is merged, not overwritten"), Health->GetHealth(), 50.f);
	F.ASC->GetGameplayAttributeValueChangeDelegate(UHodgeHealthSet::GetMaxHealthAttribute()).Remove(Handle);
	HodgeAttributeGrowthTests::ApplyDamage(F.ASC, 1000.f);
	TestEqual(TEXT("Lethal damage leaves zero resource"), Health->GetHealth(), 0.f);
	TestTrue(TEXT("Dead progression may update its base"), F.PS->SetCharacterLevel(1));
	TestEqual(TEXT("Upgrade cannot revive"), Health->GetHealth(), 0.f);
	TestFalse(TEXT("Ordinary restore cannot revive"), F.PS->RestoreCharacterHealth(50.f));
	F.PS->GetAttributeCoordinator()->DetachAvatar(F.Pawn);
	TestFalse(TEXT("Detached avatar cannot level up"), F.PS->SetCharacterLevel(2));
	F.Data->StatProfile = nullptr;
	AddExpectedError(TEXT("Attribute initialization rejected"), EAutomationExpectedErrorFlags::Contains, 1);
	TestFalse(TEXT("Missing data never publishes ready"), F.PS->GetAttributeCoordinator()->PrepareAvatar(F.Pawn, F.Data));
	TestFalse(TEXT("Missing profile remains unready"), F.PS->AreAttributesReadyFor(F.Pawn));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeStatAvatarTest, "Hodge.Attributes.AvatarAndBootstrapFailure", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeStatAvatarTest::RunTest(const FString& Parameters)
{
	HodgeAttributeGrowthTests::FFixture F;
	F.Data->DefaultWeaponDefinition = F.Sword;
	TestTrue(TEXT("Missing equipment starts a pending bootstrap"), F.PS->GetAttributeCoordinator()->PrepareAvatar(F.Pawn, F.Data));
	TestFalse(TEXT("No default equipment cannot publish ready"), F.PS->GetAttributeCoordinator()->CompleteAvatarInitialization());
	TestFalse(TEXT("Incomplete bootstrap cannot upgrade"), F.PS->SetCharacterLevel(2));
	TestEqual(TEXT("Rejected bootstrap restores attack base"), F.ASC->GetSet<UHodgeCombatSet>()->GetBaseDamage(), 0.f);
	F.ConnectEquipmentASC();
	if (!TestTrue(TEXT("Late completion retries successfully"), F.PS->AreAttributesReadyFor(F.Pawn))) { return false; }
	F.PS->RestoreCharacterHealth(40.f);
	F.PS->GetAttributeCoordinator()->DetachAvatar(F.Pawn);
	F.Equipment->UnequipItem(F.Equipment->FindInstanceOfDefinition(F.Sword));
	auto* NewPawn = F.World->SpawnActor<AHodgeCombatCharacter>();
	F.ASC->InitAbilityActorInfo(F.PS, NewPawn);
	F.Data->DefaultWeaponDefinition = nullptr;
	TestTrue(TEXT("New avatar rebind prepares"), F.PS->GetAttributeCoordinator()->PrepareAvatar(NewPawn, F.Data));
	TestTrue(TEXT("Rebind completes"), F.PS->GetAttributeCoordinator()->CompleteAvatarInitialization());
	TestEqual(TEXT("Rebind preserves injury"), F.ASC->GetSet<UHodgeHealthSet>()->GetHealth(), 40.f);
	TestFalse(TEXT("Old avatar no longer ready"), F.PS->AreAttributesReadyFor(F.Pawn));
	F.PS->GetAttributeCoordinator()->ExpectRespawn();
	TestTrue(TEXT("Explicit respawn prepares"), F.PS->GetAttributeCoordinator()->PrepareAvatar(NewPawn, F.Data));
	TestTrue(TEXT("Explicit respawn completes"), F.PS->GetAttributeCoordinator()->CompleteAvatarInitialization());
	TestEqual(TEXT("Explicit respawn fills final maximum"), F.ASC->GetSet<UHodgeHealthSet>()->GetHealth(), 100.f);
	F.PS->GetAttributeCoordinator()->DetachAvatar(NewPawn);
	return true;
}
#endif
