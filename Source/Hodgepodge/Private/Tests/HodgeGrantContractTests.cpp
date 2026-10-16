/** @file HodgeGrantContractTests.cpp @brief 验证配置来源、分阶段授予和 GameFeature 来源账本。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "HodgeEnemyAITestSupport.h"
#include "HodgeEnemyAITestTypes.h"
#include "GameFeatures/GameFeatureAction_AddAbilities.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Data/HodgeAssetManager.h"
#include "Data/HodgeGameData.h"
#include "Data/HodgePawnData.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "AbilitySystem/AttributeSet/HodgeResonanceSet.h"
#include "Component/HodgeHealthComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "GameFramework/Pawn.h"
#include "UObject/StrongObjectPtr.h"

/** 仅接入扩展事件与观察提交状态；资源操作始终执行生产函数。 */
struct FHodgeGameFeatureGrantTestAccess
{
	static void Prepare(UGameFeatureAction_AddAbilities* Action)
	{ Action->ContextData.Add(FGameFeatureStateChangeContext(), MakeShared<UGameFeatureAction_AddAbilities::FPerContextData>()); }
	static void Add(UGameFeatureAction_AddAbilities* Action, AActor* Actor, int32 Index)
	{ Action->HandleActorExtension(Actor, UGameFrameworkComponentManager::NAME_ExtensionAdded, Index, FGameFeatureStateChangeContext()); }
	static void Remove(UGameFeatureAction_AddAbilities* Action, AActor* Actor, int32 Index)
	{ Action->HandleActorExtension(Actor, UGameFrameworkComponentManager::NAME_ExtensionRemoved, Index, FGameFeatureStateChangeContext()); }
	static bool Applied(UGameFeatureAction_AddAbilities* Action, AActor* Actor, int32 Index)
	{
		const auto Data = Action->ContextData.FindRef(FGameFeatureStateChangeContext());
		const auto* Entries = Data ? Data->ActiveExtensions.Find(Actor) : nullptr;
		const auto Entry = Entries ? Entries->FindRef(Index) : nullptr;
		return Entry && Entry->State == UGameFeatureAction_AddAbilities::EGrantState::Applied;
	}
	static void Reset(UGameFeatureAction_AddAbilities* Action)
	{
		const auto Data = Action->ContextData.FindRef(FGameFeatureStateChangeContext());
		if (Data) { Action->Reset(*Data); }
	}
};

namespace
{
	/** 创建带真实 ASC、属性和 Init State 子系统的隔离世界。 */
	struct FGrantWorld
	{
		UGameInstance* Instance;
		UWorld* World;
		AActor* Owner;
		UHodgeAbilitySystemComponent* ASC;
		FGrantWorld(bool bPawn = false)
		{
			World = FHodgeEnemyInitTestAccess::CreateWorld(Instance);
			Owner = bPawn ? World->SpawnActor<APawn>() : World->SpawnActor<AActor>();
			ASC = NewObject<UHodgeAbilitySystemComponent>(Owner);
			ASC->RegisterComponent(); ASC->InitAbilityActorInfo(Owner, Owner);
			ASC->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(Owner));
			ASC->AddAttributeSetSubobject(NewObject<UHodgeCombatSet>(Owner));
			ASC->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), 100.f);
			ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
		}
		~FGrantWorld() { FHodgeEnemyInitTestAccess::DestroyWorld(Instance, World); }
	};

	/** 每个测试只构造自己的 Action 配置，禁止写入正式资产。 */
	TStrongObjectPtr<UGameFeatureAction_AddAbilities> MakeAction()
	{
		TStrongObjectPtr<UGameFeatureAction_AddAbilities> Action(NewObject<UGameFeatureAction_AddAbilities>());
		FHodgeGameFeatureGrantTestAccess::Prepare(Action.Get());
		return Action;
	}

	/** 直接能力配置复用原有公开属性入口。 */
	FGameFeatureAbilitiesEntry DirectAbility(UClass* Type)
	{
		FGameFeatureAbilitiesEntry Entry; Entry.ActorClass = AActor::StaticClass();
		FHodgeAbilityGrant Grant; Grant.AbilityType = Type; Entry.GrantedAbilities.Add(Grant);
		return Entry;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeGameDataLifetimeTest, "Hodge.GameData.ConfiguredAssetAndGC", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 配置资产身份和 GC 持有必须来自 AssetManager。 */
bool FHodgeGameDataLifetimeTest::RunTest(const FString&)
{
	const auto* Data = &UHodgeGameData::Get();
	TestTrue(TEXT("Getter returns configured asset"), Data == &UHodgeAssetManager::Get().GetGameData());
	TestNotNull(TEXT("Configured damage GE resolves"), UHodgeAssetManager::GetSubclass(Data->DamageGameplayEffect_SetByCaller).Get());
	TestNotNull(TEXT("Configured heal GE resolves"), UHodgeAssetManager::GetSubclass(Data->HealGameplayEffect_SetByCaller).Get());
	TestNotNull(TEXT("Configured tag GE resolves"), UHodgeAssetManager::GetSubclass(Data->DynamicTagGameplayEffect).Get());
	TWeakObjectPtr<const UHodgeGameData> WeakData(Data);
	CollectGarbage(RF_NoFlags);
	TestTrue(TEXT("Asset remains held after GC"), WeakData.IsValid() && WeakData.Get() == &UHodgeGameData::Get());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDynamicTagContractTest, "Hodge.GameData.DynamicTagPair", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 使用正式配置验证动态 GE 的增加、移除，不替代为 Loose Tag。 */
bool FHodgeDynamicTagContractTest::RunTest(const FString&)
{
	FGrantWorld F;
	F.ASC->AddDynamicTagGameplayEffect(HodgeGameplayTags::Status_Attack);
	TestTrue(TEXT("Dynamic GE adds tag"), F.ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Attack));
	F.ASC->RemoveDynamicTagGameplayEffect(HodgeGameplayTags::Status_Attack);
	TestFalse(TEXT("Matching GE removal clears tag"), F.ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Attack));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeSelfDestructContractTest, "Hodge.GameData.SelfDestructAuthorityAndAvatar", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 正式 GE 允许明确的自毁，普通自伤和旧 Avatar 回调仍被拒绝。 */
bool FHodgeSelfDestructContractTest::RunTest(const FString&)
{
	FGrantWorld F;
	auto* Health = NewObject<UHodgeHealthComponent>(F.Owner); Health->RegisterComponent();
	Health->InitializeWithAbilitySystem(F.ASC);
	const auto Type = UHodgeAssetManager::GetSubclass(UHodgeGameData::Get().DamageGameplayEffect_SetByCaller);
	auto Spec = F.ASC->MakeOutgoingSpec(Type, 1.f, F.ASC->MakeEffectContext());
	if (!TestTrue(TEXT("Configured damage spec builds"), Spec.IsValid())) { return false; }
	Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Damage, 100.f);
	F.ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	TestEqual(TEXT("Ordinary self damage remains blocked"), Health->GetHealth(), 100.f);
	auto* NewAvatar = F.World->SpawnActor<AActor>();
	F.ASC->InitAbilityActorInfo(F.Owner, NewAvatar);
	Health->DamageSelfDestruct(true);
	TestEqual(TEXT("Old Avatar cannot self-destruct new binding"), Health->GetHealth(), 100.f);
	F.ASC->InitAbilityActorInfo(F.Owner, F.Owner);
	F.ASC->AddLooseGameplayTag(TAG_Gameplay_DamageImmunity);
	Health->DamageSelfDestruct(true);
	TestEqual(TEXT("Authoritative self-destruct bypasses immunity"), Health->GetHealth(), 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeOwnerOnlyGrantTest, "Hodge.Grants.OwnerBeforePawnAndOnSpawn", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 两个授予阶段有不同前置条件，Avatar 到达不能重授基础资源。 */
bool FHodgeOwnerOnlyGrantTest::RunTest(const FString&)
{
	FGrantWorld F;
	F.ASC->InitAbilityActorInfo(F.Owner, nullptr);
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddAbility(UHodgeOwnerOnlyOnSpawnTestAbility::StaticClass());
	FHodgeAbilitySet_GrantedHandles Handles;
	TestFalse(TEXT("Bound-avatar grant refuses missing Avatar"), Set->TryGiveToAbilitySystem(F.ASC, Handles).bSucceeded);
	const auto Result = Set->TryGiveToAbilitySystem(F.ASC, Handles, nullptr, false, EHodgeAbilityGrantContext::OwnerInitialized);
	TestTrue(TEXT("Owner phase grants before Pawn"), Result.bSucceeded);
	if (!Result.bSucceeded) { return false; }
	const auto Handle = F.ASC->FindAbilitySpecFromClass(UHodgeOwnerOnlyOnSpawnTestAbility::StaticClass())->Handle;
	TestFalse(TEXT("OnSpawn waits for Avatar"), F.ASC->FindAbilitySpecFromHandle(Handle)->IsActive());
	auto* Pawn = F.World->SpawnActor<APawn>(); F.ASC->InitAbilityActorInfo(F.Owner, Pawn);
	TestTrue(TEXT("New Pawn activates existing OnSpawn spec"), F.ASC->FindAbilitySpecFromHandle(Handle)->IsActive());
	TestEqual(TEXT("Exactly one spec remains"), F.ASC->GetActivatableAbilities().Num(), 1);
	Handles.TakeFromAbilitySystem(F.ASC);
	TestEqual(TEXT("Source handles revoke after rebind"), F.ASC->GetActivatableAbilities().Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeGrantBatchAtomicTest, "Hodge.Grants.BatchPreflightAndIrreversibleFailure", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 整批错误在副作用前失败，执行中失败只清理本批可撤销句柄。 */
bool FHodgeGrantBatchAtomicTest::RunTest(const FString&)
{
	FGrantWorld F;
	auto* First = NewObject<UHodgeEnemyTestAbilitySet>(); First->AddAbility(UHodgeRelationshipTestAbility::StaticClass());
	auto* Bad = NewObject<UHodgeEnemyTestAbilitySet>(); Bad->AddAbility(nullptr);
	FHodgeAbilitySet_GrantedHandles Handles;
	TestFalse(TEXT("Invalid later set fails whole preflight"), UHodgeAbilitySet::TryGiveBatchToAbilitySystem({First, Bad}, F.ASC, Handles).bSucceeded);
	TestEqual(TEXT("No first-set partial grant"), F.ASC->GetActivatableAbilities().Num(), 0);
	auto* Effect = NewObject<UHodgeEnemyTestAbilitySet>(); Effect->AddEffect(UHodgeEnemyInstantTestEffect::StaticClass());
	const float Before = F.ASC->GetNumericAttribute(UHodgeCombatSet::GetBaseDamageAttribute());
	const auto Listener = F.ASC->OnGameplayEffectAppliedDelegateToSelf.AddLambda([&](UAbilitySystemComponent*, const FGameplayEffectSpec&, FActiveGameplayEffectHandle)
	{ F.ASC->SetAvatarActor(nullptr); });
	const auto Result = UHodgeAbilitySet::TryGiveBatchToAbilitySystem({First, Effect}, F.ASC, Handles);
	F.ASC->OnGameplayEffectAppliedDelegateToSelf.Remove(Listener);
	TestFalse(TEXT("Synchronous binding loss rejects commit"), Result.bSucceeded);
	TestTrue(TEXT("Irreversible result is explicit"), Result.bHasIrreversibleEffects);
	TestEqual(TEXT("Earlier ability is cleaned"), F.ASC->GetActivatableAbilities().Num(), 0);
	TestEqual(TEXT("Instant change is not falsely rolled back"), F.ASC->GetNumericAttribute(UHodgeCombatSet::GetBaseDamageAttribute()), Before + 1.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgePlayerBaseGrantTest, "Hodge.Grants.PlayerStateCommitAndFailure", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 生产 PlayerState 只发布已提交 PawnData，重复设置不重授，失败不可报就绪。 */
bool FHodgePlayerBaseGrantTest::RunTest(const FString&)
{
	FGrantWorld F; F.World->WorldType = EWorldType::EditorPreview;
	auto* PS = F.World->SpawnActor<AHodgePlayerState>();
	PS->PreInitializeComponents(); PS->InitializeComponents(); PS->PostInitializeComponents();
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddAbility(UHodgeOwnerOnlyOnSpawnTestAbility::StaticClass());
	auto* Data = NewObject<UHodgePawnData>(); Data->AbilitySets.Add(Set);
	PS->SetPawnData(Data);
	TestTrue(TEXT("PawnData published after batch success"), PS->GetPawnData<UHodgePawnData>() == Data);
	PS->SetPawnData(Data);
	TestEqual(TEXT("Repeated configuration is idempotent"), PS->GetHodgeAbilitySystemComponent()->GetActivatableAbilities().Num(), 1);
	// 预览世界跳过自动 Receiver 注册，手动结束前显式配对注册。
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(PS, false);
	PS->EndPlay(EEndPlayReason::Destroyed);
	TestEqual(TEXT("Persistent owner revokes its own source"), PS->GetHodgeAbilitySystemComponent()->GetActivatableAbilities().Num(), 0);
	auto* Failed = F.World->SpawnActor<AHodgePlayerState>();
	Failed->PreInitializeComponents(); Failed->InitializeComponents(); Failed->PostInitializeComponents();
	auto* Bad = NewObject<UHodgePawnData>(); Bad->AbilitySets.Add(Set); Bad->AbilitySets.Add(nullptr);
	AddExpectedError(TEXT("base grant failed"), EAutomationExpectedErrorFlags::Contains, 1);
	Failed->SetPawnData(Bad);
	TestNull(TEXT("Failed config is not published"), Failed->GetPawnData<UHodgePawnData>());
	TestEqual(TEXT("No partial base abilities"), Failed->GetHodgeAbilitySystemComponent()->GetActivatableAbilities().Num(), 0);
	Failed->SetPawnData(Data);
	TestNull(TEXT("Failed owner does not blindly retry"), Failed->GetPawnData<UHodgePawnData>());
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(Failed, false);
	Failed->EndPlay(EEndPlayReason::Destroyed);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeGFOverlappingEntriesTest, "Hodge.Grants.GameFeature.OverlappingEntriesAndSources", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 同 Actor 的两个条目各提交一次，移除一项不影响另一项及基础来源。 */
bool FHodgeGFOverlappingEntriesTest::RunTest(const FString&)
{
	FGrantWorld F; auto Action = MakeAction();
	const auto Outside = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeRelationshipTestAbility::StaticClass()));
	Action->AbilitiesList.Add(DirectAbility(UHodgeRelationshipTestAbility::StaticClass()));
	Action->AbilitiesList.Add(DirectAbility(UHodgeFacingPredictionTestAbility::StaticClass()));
	FHodgeGameFeatureGrantTestAccess::Add(Action.Get(), F.Owner, 0);
	FHodgeGameFeatureGrantTestAccess::Add(Action.Get(), F.Owner, 1);
	FHodgeGameFeatureGrantTestAccess::Add(Action.Get(), F.Owner, 0);
	TestTrue(TEXT("First entry applied"), FHodgeGameFeatureGrantTestAccess::Applied(Action.Get(), F.Owner, 0));
	TestTrue(TEXT("Second entry applied"), FHodgeGameFeatureGrantTestAccess::Applied(Action.Get(), F.Owner, 1));
	TestEqual(TEXT("Two sources plus base, no duplicate event grant"), F.ASC->GetActivatableAbilities().Num(), 3);
	FHodgeGameFeatureGrantTestAccess::Remove(Action.Get(), F.Owner, 0);
	TestEqual(TEXT("Only matching entry revoked"), F.ASC->GetActivatableAbilities().Num(), 2);
	TestNotNull(TEXT("Unrelated base spec survives"), F.ASC->FindAbilitySpecFromHandle(Outside));
	FHodgeGameFeatureGrantTestAccess::Reset(Action.Get());
	TestEqual(TEXT("Context reset preserves outside source"), F.ASC->GetActivatableAbilities().Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeGFPreflightTest, "Hodge.Grants.GameFeature.PreflightAllBranches", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 不驻留引用、跨分支重复属性和非法直接能力均不能产生部分授予。 */
bool FHodgeGFPreflightTest::RunTest(const FString&)
{
	FGrantWorld F; auto Action = MakeAction();
	auto Missing = DirectAbility(UHodgeRelationshipTestAbility::StaticClass());
	Missing.GrantedAbilitySets.Add(TSoftObjectPtr<const UHodgeAbilitySet>(FSoftObjectPath(TEXT("/Game/NotResident/AS_Missing.AS_Missing"))));
	Action->AbilitiesList.Add(Missing);
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddAttribute(UHodgeResonanceSet::StaticClass());
	FGameFeatureAbilitiesEntry Duplicate; Duplicate.ActorClass = AActor::StaticClass(); Duplicate.GrantedAbilitySets.Add(Set);
	FHodgeAttributeSetGrant Attribute; Attribute.AttributeSetType = UHodgeResonanceSet::StaticClass(); Duplicate.GrantedAttributes.Add(Attribute);
	Action->AbilitiesList.Add(Duplicate);
	Action->AbilitiesList.Add(DirectAbility(nullptr));
	AddExpectedError(TEXT("Ability extension failed"), EAutomationExpectedErrorFlags::Contains, 3);
	for (int32 Index = 0; Index < 3; ++Index)
	{
		FHodgeGameFeatureGrantTestAccess::Add(Action.Get(), F.Owner, Index);
		TestFalse(TEXT("Invalid entry never committed"), FHodgeGameFeatureGrantTestAccess::Applied(Action.Get(), F.Owner, Index));
		FHodgeGameFeatureGrantTestAccess::Add(Action.Get(), F.Owner, Index);
	}
	TestEqual(TEXT("Direct ability not partially granted"), F.ASC->GetActivatableAbilities().Num(), 0);
	TestNull(TEXT("No partial resource set"), F.ASC->GetSet<UHodgeResonanceSet>());
	FHodgeGameFeatureGrantTestAccess::Reset(Action.Get());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeGFWithdrawalTest, "Hodge.Grants.GameFeature.WithdrawDuringCommit", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** GE 同步回调停用来源时，提交栈必须失效并清理所有 Pending 资源。 */
bool FHodgeGFWithdrawalTest::RunTest(const FString&)
{
	FGrantWorld F; auto Action = MakeAction();
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddEffect(UHodgeEnemyInstantTestEffect::StaticClass());
	auto Config = DirectAbility(UHodgeRelationshipTestAbility::StaticClass()); Config.GrantedAbilitySets.Add(Set);
	FHodgeAttributeSetGrant Attribute; Attribute.AttributeSetType = UHodgeResonanceSet::StaticClass(); Config.GrantedAttributes.Add(Attribute);
	Action->AbilitiesList.Add(Config);
	const auto Listener = F.ASC->OnGameplayEffectAppliedDelegateToSelf.AddLambda([&](UAbilitySystemComponent*, const FGameplayEffectSpec&, FActiveGameplayEffectHandle)
	{ FHodgeGameFeatureGrantTestAccess::Reset(Action.Get()); });
	FHodgeGameFeatureGrantTestAccess::Add(Action.Get(), F.Owner, 0);
	F.ASC->OnGameplayEffectAppliedDelegateToSelf.Remove(Listener);
	TestFalse(TEXT("Withdrawn entry cannot publish success"), FHodgeGameFeatureGrantTestAccess::Applied(Action.Get(), F.Owner, 0));
	TestNull(TEXT("Pending attribute revoked"), F.ASC->GetSet<UHodgeResonanceSet>());
	TestEqual(TEXT("No ability from withdrawn entry"), F.ASC->GetActivatableAbilities().Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeGFLateBindingTest, "Hodge.Grants.GameFeature.LateInitAndOriginalASC", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 真实 Init State 通知推进等待条目；换绑后的撤销仍指向原 ASC。 */
bool FHodgeGFLateBindingTest::RunTest(const FString&)
{
	FGrantWorld F(true); auto Action = MakeAction();
	Action->AbilitiesList.Add(DirectAbility(UHodgeRelationshipTestAbility::StaticClass()));
	F.ASC->InitAbilityActorInfo(F.Owner, nullptr);
	FHodgeGameFeatureGrantTestAccess::Add(Action.Get(), F.Owner, 0);
	TestFalse(TEXT("No Avatar means waiting"), FHodgeGameFeatureGrantTestAccess::Applied(Action.Get(), F.Owner, 0));
	F.ASC->InitAbilityActorInfo(F.Owner, F.Owner);
	auto* Manager = F.Instance->GetSubsystem<UGameFrameworkComponentManager>();
	Manager->RegisterFeatureImplementer(F.Owner, TEXT("GrantTest"), Action.Get());
	Manager->ChangeFeatureInitState(F.Owner, TEXT("GrantTest"), Action.Get(), HodgeGameplayTags::InitState_DataInitialized);
	TestTrue(TEXT("Init State callback applies entry"), FHodgeGameFeatureGrantTestAccess::Applied(Action.Get(), F.Owner, 0));
	F.ASC->InitAbilityActorInfo(F.Owner, F.World->SpawnActor<AActor>());
	FHodgeGameFeatureGrantTestAccess::Remove(Action.Get(), F.Owner, 0);
	TestEqual(TEXT("Original ASC source is revoked after rebind"), F.ASC->GetActivatableAbilities().Num(), 0);
	Manager->RemoveFeatureImplementer(F.Owner, Action.Get());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeGFBindingLifecycleTest, "Hodge.Grants.GameFeature.RealExtensionBindingLifecycle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 真实激活、Actor 扩展监听及 PawnExtension 绑定事件验证三类授予与配对撤销。 */
bool FHodgeGFBindingLifecycleTest::RunTest(const FString&)
{
	FGrantWorld F(true);
	auto* Pawn = CastChecked<APawn>(F.Owner);
	Pawn->PreInitializeComponents(); Pawn->InitializeComponents(); Pawn->PostInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(Pawn);
	auto* Extension = NewObject<UHodgePawnExtensionComponent>(Pawn); Extension->RegisterComponent();
	Extension->SetPawnData(NewObject<UHodgePawnData>());
	F.ASC->InitAbilityActorInfo(Pawn, nullptr);
	TStrongObjectPtr<UGameFeatureAction_AddAbilities> Action(NewObject<UGameFeatureAction_AddAbilities>());
	auto Config = DirectAbility(UHodgeRelationshipTestAbility::StaticClass()); Config.ActorClass = APawn::StaticClass();
	auto* Set = NewObject<UHodgeEnemyTestAbilitySet>(); Set->AddAbility(UHodgeFacingPredictionTestAbility::StaticClass());
	Config.GrantedAbilitySets.Add(Set);
	FHodgeAttributeSetGrant Attribute; Attribute.AttributeSetType = UHodgeResonanceSet::StaticClass(); Config.GrantedAttributes.Add(Attribute);
	// 空配置项只跳过自身，后面的真实条目仍使用原数组下标。
	Action->AbilitiesList.AddDefaulted(); Action->AbilitiesList.Add(Config);
	FGameFeatureActivatingContext Activating;
	const FName WorldHandle = GEngine->GetWorldContextFromWorldChecked(F.World).ContextHandle;
	Activating.SetRequiredWorldContextHandle(WorldHandle);
	Action->OnGameFeatureActivating(Activating);
	TestEqual(TEXT("Real handler waits for Avatar"), F.ASC->GetActivatableAbilities().Num(), 0);
	Extension->InitializeAbilitySystem(F.ASC, Pawn);
	TestEqual(TEXT("Binding event grants both ability branches"), F.ASC->GetActivatableAbilities().Num(), 2);
	TestNotNull(TEXT("Direct AttributeSet committed"), F.ASC->GetSet<UHodgeResonanceSet>());
	Extension->InitializeAbilitySystem(F.ASC, Pawn);
	TestEqual(TEXT("Repeated bind does not duplicate"), F.ASC->GetActivatableAbilities().Num(), 2);
	Extension->UninitializeAbilitySystem();
	TestEqual(TEXT("Unbind revokes owned abilities"), F.ASC->GetActivatableAbilities().Num(), 0);
	TestNull(TEXT("Unbind revokes owned AttributeSet"), F.ASC->GetSet<UHodgeResonanceSet>());
	Extension->InitializeAbilitySystem(F.ASC, Pawn);
	TestEqual(TEXT("Fresh binding can grant again"), F.ASC->GetActivatableAbilities().Num(), 2);
	FGameFeatureDeactivatingContext Deactivating(TEXTVIEW("GrantContractTest"), [](FStringView) {});
	Deactivating.SetRequiredWorldContextHandle(WorldHandle);
	Action->OnGameFeatureDeactivating(Deactivating);
	TestEqual(TEXT("Real deactivation revokes abilities"), F.ASC->GetActivatableAbilities().Num(), 0);
	TestNull(TEXT("Real deactivation revokes AttributeSet"), F.ASC->GetSet<UHodgeResonanceSet>());
	Extension->UninitializeAbilitySystem();
	F.Instance->GetSubsystem<UGameFrameworkComponentManager>()->RemoveFeatureImplementer(Pawn, Extension);
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(Pawn);
	return true;
}
#endif
