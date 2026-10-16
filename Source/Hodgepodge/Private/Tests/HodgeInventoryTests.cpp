// Copyright Epic Games, Inc. All Rights Reserved.
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Inventory/HodgeInventoryManagerComponent.h"
#include "Inventory/HodgeInventoryItemDefinition.h"
#include "Inventory/HodgeInventoryItemInstance.h"
#include "Inventory/HodgeEquipmentSelectionComponent.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Character/HodgeCombatCharacter.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "AbilitySystem/Stats/HodgeAttributeCoordinator.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "Equipment/HodgeEquipmentInstance.h"
#include "Engine/World.h"

namespace HodgeInventoryTests
{
	/** 隔离世界执行真实库存 API 和装备授予链，不依赖 UI 或视觉检查。 */
	struct FFixture
	{
		UWorld* World;
		AHodgePlayerState* PS;
		AHodgeCombatCharacter* Pawn;
		UHodgeInventoryManagerComponent* Inventory;
		UHodgeEquipmentSelectionComponent* Selection;
		UHodgeEquipmentManagerComponent* Equipment;
		UHodgePawnData* Data;
		TSubclassOf<UHodgeInventoryItemDefinition> Sword, Material;
		FFixture()
		{
			const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
			World = UWorld::CreateWorld(EWorldType::EditorPreview, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			PS = World->SpawnActor<AHodgePlayerState>(); Pawn = World->SpawnActor<AHodgeCombatCharacter>();
			Inventory = NewObject<UHodgeInventoryManagerComponent>(PS); PS->AddInstanceComponent(Inventory); Inventory->RegisterComponent();
			Selection = NewObject<UHodgeEquipmentSelectionComponent>(PS); PS->AddInstanceComponent(Selection); Selection->RegisterComponent();
			Equipment = NewObject<UHodgeEquipmentManagerComponent>(Pawn); Pawn->AddInstanceComponent(Equipment); Equipment->RegisterComponent();
			Data = NewObject<UHodgePawnData>(PS); Data->bUseInventory = true; Data->InventoryMaxEntries = 4;
			Data->StatProfile = NewObject<UHodgeCharacterStatProfile>(Data);
			Sword = LoadClass<UHodgeInventoryItemDefinition>(nullptr, TEXT("/Game/Main/Data/Inventory/ID_Sword.ID_Sword_C"));
			Material = LoadClass<UHodgeInventoryItemDefinition>(nullptr, TEXT("/Game/Main/Data/Inventory/ID_TestMaterial.ID_TestMaterial_C"));
		}
		bool Bind()
		{
			auto* ASC = PS->GetHodgeAbilitySystemComponent();
			if (!ASC->GetSet<UHodgeHealthSet>()) { ASC->AddAttributeSetSubobject(CastChecked<UHodgeHealthSet>(PS->GetDefaultSubobjectByName(TEXT("HealthSet")))); }
			if (!ASC->GetSet<UHodgeCombatSet>()) { ASC->AddAttributeSetSubobject(CastChecked<UHodgeCombatSet>(PS->GetDefaultSubobjectByName(TEXT("CombatSet")))); }
			auto* Extension = Pawn->FindComponentByClass<UHodgePawnExtensionComponent>();
			Extension->SetPawnData(Data); Extension->InitializeAbilitySystem(ASC, PS);
			return PS->AreAttributesReadyFor(Pawn) && Selection->IsSelectionApplied();
		}
		~FFixture()
		{
			Pawn->FindComponentByClass<UHodgePawnExtensionComponent>()->UninitializeAbilitySystem();
			Selection->UnbindAvatar(Pawn); PS->GetAttributeCoordinator()->DetachAvatar(Pawn);
			World->DestroyWorld(false);
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeInventoryStackTest, "Hodge.Inventory.StacksCapacityAndNotifications", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeInventoryStackTest::RunTest(const FString& Parameters)
{
	HodgeInventoryTests::FFixture F;
	if (!TestNotNull(TEXT("Material definition exists"), F.Material.Get())) { return false; }
	F.Data->InventoryMaxEntries = 2;
	TestTrue(TEXT("Empty container initializes"), F.Inventory->InitializeFromPawnData(F.Data).Succeeded());
	bool bReentryBlocked = false;
	const auto Handle = F.Inventory->OnInventoryChanged.AddLambda([&](const TArray<FHodgeInventoryChange>& Changes, int64 Revision)
	{ bReentryBlocked = F.Inventory->AddItemDefinition(F.Material, 1).Code == EHodgeInventoryResult::Busy; });
	const auto Added = F.Inventory->AddItemDefinition(F.Material, 37);
	TestTrue(TEXT("Multi-stack add succeeds"), Added.Succeeded());
	TestEqual(TEXT("All affected stacks are returned"), Added.Changes.Num(), 2);
	TestEqual(TEXT("Total counts quantities, not entries"), F.Inventory->GetTotalItemCountByDefinition(F.Material), int64(37));
	TestTrue(TEXT("Notification cannot reenter mutation"), bReentryBlocked);
	F.Inventory->OnInventoryChanged.Remove(Handle);
	TestTrue(TEXT("Full entry capacity still allows filling existing stack"), F.Inventory->AddItemDefinition(F.Material, 3).Succeeded());
	TestTrue(TEXT("Overflowing entry capacity rejected"), F.Inventory->AddItemDefinition(F.Material, 1).Code == EHodgeInventoryResult::CapacityExceeded);
	TestFalse(TEXT("Insufficient consumption fails"), F.Inventory->ConsumeItemsByDefinition(F.Material, 41).Succeeded());
	TestEqual(TEXT("Failed consumption is atomic"), F.Inventory->GetTotalItemCountByDefinition(F.Material), int64(40));
	TestTrue(TEXT("Consumption across stacks succeeds"), F.Inventory->ConsumeItemsByDefinition(F.Material, 22).Succeeded());
	TestEqual(TEXT("Empty stack removed"), F.Inventory->GetItems().Num(), 1);
	TestEqual(TEXT("Remaining quantity correct"), F.Inventory->GetTotalItemCountByDefinition(F.Material), int64(18));
	for (const int32 Count : {0, -1, MAX_int32}) { TestFalse(TEXT("Invalid/overflow quantity rejected"), F.Inventory->AddItemDefinition(F.Material, Count).Succeeded()); }
	TestFalse(TEXT("Abstract definition rejected"), F.Inventory->AddItemDefinition(UHodgeInventoryItemDefinition::StaticClass(), 1).Succeeded());
	const auto Id = F.Inventory->GetItems()[0].ItemId;
	const TWeakObjectPtr<UHodgeInventoryItemInstance> Removed = F.Inventory->FindItemById(Id);
	TestTrue(TEXT("Remove by identity succeeds"), F.Inventory->RemoveItemById(Id).Succeeded());
	TestNull(TEXT("Deleted identity no longer resolves"), F.Inventory->FindItemById(Id));
	TestTrue(TEXT("Late consumer sees empty snapshot"), F.Inventory->GetItems().IsEmpty());
	CollectGarbage(RF_NoFlags);
	TestFalse(TEXT("Removed inventory instance is not retained after GC"), Removed.IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeInventoryInitializationTest, "Hodge.Inventory.AtomicDefaultsAndValidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeInventoryInitializationTest::RunTest(const FString& Parameters)
{
	HodgeInventoryTests::FFixture F;
	if (!TestNotNull(TEXT("Sword definition exists"), F.Sword.Get())) { return false; }
	FHodgeInventoryDefaultItem Good; Good.Definition = F.Sword;
	FHodgeInventoryDefaultItem Bad; Bad.Definition = UHodgeInventoryItemDefinition::StaticClass();
	F.Data->DefaultInventoryItems = {Good, Bad};
	TestFalse(TEXT("Invalid batch fails"), F.Inventory->InitializeFromPawnData(F.Data).Succeeded());
	TestFalse(TEXT("Failure does not set initialized"), F.Inventory->IsInitialized());
	TestTrue(TEXT("Failure does not leave partial items"), F.Inventory->GetItems().IsEmpty());
	F.Data->DefaultInventoryItems = {Good};
	TestTrue(TEXT("Corrected batch succeeds"), F.Inventory->InitializeFromPawnData(F.Data).Succeeded());
	const auto Id = F.Inventory->GetItems()[0].ItemId;
	TestTrue(TEXT("Repeated initialization succeeds"), F.Inventory->InitializeFromPawnData(F.Data).Succeeded());
	TestEqual(TEXT("Default issued only once"), F.Inventory->GetItems().Num(), 1);
	TestTrue(TEXT("Identity remains stable"), F.Inventory->GetItems()[0].ItemId == Id);
	TestTrue(TEXT("Second sword added separately"), F.Inventory->AddItemDefinition(F.Sword, 1, 2).Succeeded());
	const auto Items = F.Inventory->GetItems();
	TestTrue(TEXT("Same definition, independent identities"), Items[0].ItemId != Items[1].ItemId);
	TestEqual(TEXT("Independent second level"), Items[1].Instance->GetItemLevel(), 2);
	auto* Definition = GetMutableDefault<UHodgeInventoryItemDefinition>(F.Sword);
	TGuardValue<int32> Limit(Definition->MaxStackCount, 2);
	TestFalse(TEXT("Equippable item cannot stack"), F.Inventory->CanAddItemDefinition(F.Sword, 1).Succeeded());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeInventoryEquipmentTest, "Hodge.Inventory.EquipmentIdentityLifecycleAndGuards", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeInventoryEquipmentTest::RunTest(const FString& Parameters)
{
	HodgeInventoryTests::FFixture F;
	if (!TestNotNull(TEXT("Sword definition exists"), F.Sword.Get())) { return false; }
	const auto* Fragment = GetDefault<UHodgeInventoryItemDefinition>(F.Sword)->FindFragment<UHodgeInventoryFragment_EquippableItem>();
	if (!TestNotNull(TEXT("Sword equippable fragment exists"), Fragment)) { return false; }
	FHodgeOwnedEquipmentState Old; Old.InstanceId = FGuid::NewGuid(); Old.Definition = Fragment->EquipmentDefinition; Old.Level = 2;
	F.PS->GetAttributeCoordinator()->RememberEquipment(Old.InstanceId, Old.Definition, Old.Level);
	FHodgeInventoryDefaultItem Default; Default.Definition = F.Sword; Default.bEquip = true;
	F.Data->DefaultInventoryItems = {Default};
	if (!TestTrue(TEXT("Full attribute/inventory/equipment bootstrap"), F.Bind())) { return false; }
	TestEqual(TEXT("Legacy default not duplicated"), F.Inventory->GetItems().Num(), 1);
	TestTrue(TEXT("Legacy identity retained"), F.Selection->GetSelectedItemId() == Old.InstanceId);
	TestEqual(TEXT("Legacy level retained"), F.Inventory->FindItemById(Old.InstanceId)->GetItemLevel(), 2);
	auto* Equipment = F.Equipment->GetFirstInstanceOfType(UHodgeEquipmentInstance::StaticClass());
	if (!TestNotNull(TEXT("Actual equipment exists"), Equipment)) { return false; }
	TestTrue(TEXT("Source established before callbacks"), Equipment->GetInstigator() == F.Inventory->FindItemById(Old.InstanceId));
	TestFalse(TEXT("Legacy level writer cannot bypass inventory"), F.Equipment->SetEquipmentLevel(Equipment, 1));
	TestTrue(TEXT("Repeated equip is idempotent"), F.Selection->RequestEquipItem(Old.InstanceId).Succeeded());
	TestEqual(TEXT("Only one actual equipment instance"), F.Equipment->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).Num(), 1);
	TestFalse(TEXT("Selected weapon protected from generic consumption"), F.Inventory->ConsumeItemsByDefinition(F.Sword, 1).Succeeded());
	TestTrue(TEXT("Second weapon added"), F.Inventory->AddItemDefinition(F.Sword, 1).Succeeded());
	const FGuid OtherId = F.Inventory->GetItems()[1].ItemId;
	auto* ASC = F.PS->GetHodgeAbilitySystemComponent();
	ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Attack);
	TestTrue(TEXT("Attack blocks switching"), F.Selection->RequestEquipItem(OtherId).Code == EHodgeInventoryResult::ActionBlocked);
	ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Attack);
	TestTrue(TEXT("Idle switch succeeds"), F.Selection->RequestEquipItem(OtherId).Succeeded());
	TestEqual(TEXT("Switch leaves both items in inventory"), F.Inventory->GetItems().Num(), 2);
	TestTrue(TEXT("Unknown item rejected"), F.Selection->RequestEquipItem(FGuid::NewGuid()).Code == EHodgeInventoryResult::NotOwned);
	F.Selection->UnbindAvatar(F.Pawn);
	TestTrue(TEXT("Detach retains selected identity"), F.Selection->GetSelectedItemId() == OtherId);
	TestTrue(TEXT("Detach removes actual equipment"), F.Equipment->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).IsEmpty());
	TestTrue(TEXT("Rebind restores actual equipment"), F.Selection->BindAvatar(F.Pawn).Succeeded());
	// 同步通知解绑不能把已经撤销的装备报告为成功，也不能泄漏候选实例。
	bool bInjected = false;
	const auto ReentryHandle = F.Selection->OnEquipmentSelectionChanged.AddLambda([&]()
	{ if (!bInjected && F.Selection->GetSelectedItemId() == Old.InstanceId) { bInjected = true; F.Selection->UnbindAvatar(F.Pawn); } });
	TestTrue(TEXT("Synchronous detach returns waiting, not false success"), F.Selection->RequestEquipItem(Old.InstanceId).Code == EHodgeInventoryResult::Waiting);
	TestTrue(TEXT("Injected detach occurred"), bInjected);
	TestTrue(TEXT("Detached candidate cleaned"), F.Equipment->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).IsEmpty());
	F.Selection->OnEquipmentSelectionChanged.Remove(ReentryHandle);
	TestTrue(TEXT("Lifecycle restore after callback detach"), F.Selection->BindAvatar(F.Pawn).Succeeded());
	TestTrue(TEXT("Explicit unequip succeeds"), F.Selection->RequestUnequip().Succeeded());
	TestFalse(TEXT("Explicit unequip clears selection"), F.Selection->GetSelectedItemId().IsValid());
	TestTrue(TEXT("Rebind after unequip stays empty"), F.Selection->BindAvatar(F.Pawn).Succeeded());
	TestTrue(TEXT("No default silently returns"), F.Equipment->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).IsEmpty());
	// 更换真实 Pawn 后保留库存和显式卸装，旧 Pawn 的迟到解绑无效。
	auto* OldPawn = F.Pawn;
	OldPawn->FindComponentByClass<UHodgePawnExtensionComponent>()->UninitializeAbilitySystem();
	F.Pawn = F.World->SpawnActor<AHodgeCombatCharacter>();
	F.Equipment = NewObject<UHodgeEquipmentManagerComponent>(F.Pawn); F.Pawn->AddInstanceComponent(F.Equipment); F.Equipment->RegisterComponent();
	TestTrue(TEXT("New avatar initializes without granting defaults again"), F.Bind());
	F.Selection->UnbindAvatar(OldPawn);
	TestTrue(TEXT("Stale unbind cannot detach new avatar"), F.Selection->GetBoundAvatar() == F.Pawn);
	TestEqual(TEXT("Pawn change retains exact inventory"), F.Inventory->GetItems().Num(), 2);
	TestTrue(TEXT("Explicit unequip survives pawn change"), F.Equipment->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).IsEmpty());
	TestTrue(TEXT("New avatar can equip same held identity"), F.Selection->RequestEquipItem(OtherId).Succeeded());
	TestTrue(TEXT("Deleting selected weapon first unequips"), F.Inventory->RemoveItemById(OtherId).Succeeded());
	TestTrue(TEXT("Deletion removes compatibility mirror"), !F.PS->GetOwnedEquipmentStates().ContainsByPredicate([&](const auto& State) { return State.InstanceId == OtherId; }));
	TestTrue(TEXT("Deletion leaves no actual equipment"), F.Equipment->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).IsEmpty());
	TestTrue(TEXT("Remaining weapon can equip before component removal"), F.Selection->RequestEquipItem(Old.InstanceId).Succeeded());
	F.Inventory->UnregisterComponent();
	TestTrue(TEXT("Inventory deactivation cleans actual equipment"), F.Equipment->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).IsEmpty());
	TestTrue(TEXT("Inactive inventory cannot equip"), F.Selection->RequestEquipItem(Old.InstanceId).Code == EHodgeInventoryResult::Waiting);
	F.Inventory->RegisterComponent();
	TestTrue(TEXT("Reregistered container can restore held selection"), F.Selection->BindAvatar(F.Pawn).Succeeded());
	F.Selection->UnregisterComponent();
	TestTrue(TEXT("Selection removal revokes its equipment"), F.Equipment->GetEquipmentInstancesOfType(UHodgeEquipmentInstance::StaticClass()).IsEmpty());
	return true;
}
#endif
