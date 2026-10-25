/** @file HodgeAutoRespawnTests.cpp @brief 验证真实被动能力的死亡门槛、换 Avatar 与异步退休；不代替 PIE。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_AutoRespawn.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "Character/HodgeHeroCharacter.h"
#include "Component/HodgeHealthComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include <limits>

/** 只注入生命周期事件与历史回调，生成和复制由集成用例另行验证。 */
struct FHodgeAutoRespawnTestAccess
{
	static uint64 Generation(const UHodgeGameplayAbility_AutoRespawn* GA) { return GA->Generation; }
	static FTimerHandle Timer(const UHodgeGameplayAbility_AutoRespawn* GA) { return GA->RespawnTimer; }
	static APawn* Avatar(const UHodgeGameplayAbility_AutoRespawn* GA) { return GA->BoundAvatar.Get(); }
	static void Detached(UHodgeGameplayAbility_AutoRespawn* GA, APawn* Pawn) { GA->OnAvatarUninitialized(Pawn); }
	static void Finished(UHodgeGameplayAbility_AutoRespawn* GA, APawn* Pawn) { GA->OnDeathFinished(Pawn); }
	static void OldCallback(UHodgeGameplayAbility_AutoRespawn* GA, uint64 Generation) { GA->AttemptRespawn(Generation); }
	static void Leaving(UHodgeGameplayAbility_AutoRespawn* GA, APawn* Pawn)
	{ GA->OnAvatarEndPlay(Pawn, EEndPlayReason::LevelTransition); }
};

namespace HodgeAutoRespawnTests
{
	/** 真实 ASC 授予并激活能力，配置与角色只存在于隔离世界。 */
	struct FFixture
	{
		UWorld* World;
		AHodgePlayerState* PS;
		APlayerController* PC;
		AHodgeHeroCharacter* Pawn;
		UHodgeAbilitySystemComponent* ASC;
		UHodgePawnData* Data;
		FGameplayAbilitySpecHandle Handle;
		UHodgeGameplayAbility_AutoRespawn* GA;
		explicit FFixture(bool bEnabled = true)
		{
			const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
				.CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
			World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			PS = World->SpawnActor<AHodgePlayerState>();
			PC = World->SpawnActor<APlayerController>();
			PC->PlayerState = PS;
			PS->SetOwner(PC);
			// 隔离世界尚未走 BeginPlay，显式完成与现有属性测试一致的 ASC 核心依赖。
			ASC = PS->GetHodgeAbilitySystemComponent();
			if (!ASC->GetSet<UHodgeHealthSet>()) { ASC->AddAttributeSetSubobject(CastChecked<UHodgeHealthSet>(PS->GetDefaultSubobjectByName(TEXT("HealthSet")))); }
			if (!ASC->GetSet<UHodgeCombatSet>()) { ASC->AddAttributeSetSubobject(CastChecked<UHodgeCombatSet>(PS->GetDefaultSubobjectByName(TEXT("CombatSet")))); }
			ASC->InitAbilityActorInfo(PS, nullptr);
			Data = NewObject<UHodgePawnData>(PS);
			Data->bEnableAutoRespawn = bEnabled;
			Data->StatProfile = NewObject<UHodgeCharacterStatProfile>(Data);
			PS->SetPawnData(Data);
			Pawn = NewPawn();
			Handle = ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_AutoRespawn::StaticClass(), 1));
			GA = Cast<UHodgeGameplayAbility_AutoRespawn>(ASC->FindAbilitySpecFromHandle(Handle)->GetPrimaryInstance());
		}
		/** 换身体仍复用同一 Controller、PlayerState 与 ASC。 */
		AHodgeHeroCharacter* NewPawn()
		{
			// 服务器换身体先完成原扩展解绑；普通 APlayerController 不替 Hodge 清 ASC Avatar。
			if (auto* Previous = PC->GetPawn())
			{
				Previous->FindComponentByClass<UHodgePawnExtensionComponent>()->UninitializeAbilitySystem();
			}
			auto* Result = World->SpawnActor<AHodgeHeroCharacter>();
			auto* Extension = Result->FindComponentByClass<UHodgePawnExtensionComponent>();
			Extension->SetPawnData(Data);
			PC->Possess(Result);
			Extension->InitializeAbilitySystem(ASC, PS);
			return Result;
		}
		~FFixture()
		{
			ASC->ClearAbility(Handle);
			World->DestroyWorld(false);
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeRespawnConfigTest, "Hodge.Respawn.Configuration", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeRespawnConfigTest::RunTest(const FString& Parameters)
{
	// 运行时不能信任仅由 Details 元数据限制的数值。
	auto* Data = NewObject<UHodgePawnData>();
	FString Error;
	TestFalse(TEXT("Old assets default to disabled"), Data->bEnableAutoRespawn);
	Data->bEnableAutoRespawn = true;
	TestTrue(TEXT("Approved defaults are valid"), Data->ValidateAutoRespawn(Error));
	TestEqual(TEXT("Total attempts include the first"), Data->AutoRespawnMaxAttempts, 3);
	for (const float Invalid : {-1.f, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()})
	{
		Data->AutoRespawnDelay = Invalid;
		TestFalse(TEXT("Invalid delay rejected"), Data->ValidateAutoRespawn(Error));
	}
	Data->AutoRespawnDelay = 0.f;
	Data->AutoRespawnMaxAttempts = 0;
	TestFalse(TEXT("Zero attempts rejected"), Data->ValidateAutoRespawn(Error));
	Data->AutoRespawnMaxAttempts = 3;
	Data->AutoRespawnRetryInterval = 0.f;
	TestFalse(TEXT("Zero retry cannot create a next-frame loop"), Data->ValidateAutoRespawn(Error));
	Data->bEnableAutoRespawn = false;
	TestTrue(TEXT("Disabled settings do not break old loading"), Data->ValidateAutoRespawn(Error));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeRespawnLifecycleTest, "Hodge.Respawn.DeathGateAndDeduplication", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeRespawnLifecycleTest::RunTest(const FString& Parameters)
{
	HodgeAutoRespawnTests::FFixture F;
	if (!TestNotNull(TEXT("Actual instanced ability granted"), F.GA)) { return false; }
	TestTrue(TEXT("OnSpawn active on authority"), F.GA->IsActive());
	TestTrue(TEXT("Formal Hero binding established"), FHodgeAutoRespawnTestAccess::Avatar(F.GA) == F.Pawn);
	FHodgeAutoRespawnTestAccess::Finished(F.GA, F.Pawn);
	TestFalse(TEXT("Death finish alone cannot start timer"), FHodgeAutoRespawnTestAccess::Timer(F.GA).IsValid());
	FHodgeAutoRespawnTestAccess::Detached(F.GA, F.Pawn);
	const auto Timer = FHodgeAutoRespawnTestAccess::Timer(F.GA);
	TestTrue(TEXT("Both gates queue next tick even with zero delay"), Timer.IsValid());
	FHodgeAutoRespawnTestAccess::Finished(F.GA, F.Pawn);
	FHodgeAutoRespawnTestAccess::Detached(F.GA, F.Pawn);
	TestTrue(TEXT("Duplicate callbacks retain one timer"), Timer == FHodgeAutoRespawnTestAccess::Timer(F.GA));
	F.ASC->ClearAbility(F.Handle);
	TestFalse(TEXT("Removal cancels the actual timer"), F.World->GetTimerManager().TimerExists(Timer));
	TestNull(TEXT("Removal clears binding"), FHodgeAutoRespawnTestAccess::Avatar(F.GA));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeRespawnRetirementTest, "Hodge.Respawn.NewAvatarRetiresOldCallback", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeRespawnRetirementTest::RunTest(const FString& Parameters)
{
	HodgeAutoRespawnTests::FFixture F;
	if (!TestNotNull(TEXT("Actual ability instance"), F.GA)) { return false; }
	const uint64 OldGeneration = FHodgeAutoRespawnTestAccess::Generation(F.GA);
	FHodgeAutoRespawnTestAccess::Detached(F.GA, F.Pawn);
	TestFalse(TEXT("Teardown alone cannot respawn"), FHodgeAutoRespawnTestAccess::Timer(F.GA).IsValid());
	FHodgeAutoRespawnTestAccess::Finished(F.GA, F.Pawn);
	const auto OldTimer = FHodgeAutoRespawnTestAccess::Timer(F.GA);
	auto* NewPawn = F.NewPawn();
	TestTrue(TEXT("Persistent instance rebound to new pawn"), FHodgeAutoRespawnTestAccess::Avatar(F.GA) == NewPawn);
	TestFalse(TEXT("New avatar cancels old timer"), F.World->GetTimerManager().TimerExists(OldTimer));
	FHodgeAutoRespawnTestAccess::Finished(F.GA, NewPawn);
	FHodgeAutoRespawnTestAccess::Detached(F.GA, NewPawn);
	const auto NewTimer = FHodgeAutoRespawnTestAccess::Timer(F.GA);
	FHodgeAutoRespawnTestAccess::OldCallback(F.GA, OldGeneration);
	TestTrue(TEXT("Old stack cannot clear new timer"), NewTimer == FHodgeAutoRespawnTestAccess::Timer(F.GA));
	FHodgeAutoRespawnTestAccess::Finished(F.GA, F.Pawn);
	TestTrue(TEXT("Late old death cannot alter new request"), FHodgeAutoRespawnTestAccess::Avatar(F.GA) == NewPawn);
	FHodgeAutoRespawnTestAccess::Leaving(F.GA, NewPawn);
	TestFalse(TEXT("World transition cancels pending respawn"), F.World->GetTimerManager().TimerExists(NewTimer));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeRespawnDisabledTest, "Hodge.Respawn.DisabledDoesNotListen", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeRespawnDisabledTest::RunTest(const FString& Parameters)
{
	HodgeAutoRespawnTests::FFixture F(false);
	if (!TestNotNull(TEXT("Granted passive instance"), F.GA)) { return false; }
	TestNull(TEXT("Disabled profile never binds"), FHodgeAutoRespawnTestAccess::Avatar(F.GA));
	FHodgeAutoRespawnTestAccess::Finished(F.GA, F.Pawn);
	FHodgeAutoRespawnTestAccess::Detached(F.GA, F.Pawn);
	TestFalse(TEXT("Disabled callbacks cannot schedule"), FHodgeAutoRespawnTestAccess::Timer(F.GA).IsValid());
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeRespawnClientDeathTagsTest, "Hodge.Respawn.ClientDeathTagsFollowCurrentAvatar", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeRespawnClientDeathTagsTest::RunTest(const FString& Parameters)
{
	HodgeAutoRespawnTests::FFixture F;
	auto* OldHealth = F.Pawn->FindComponentByClass<UHodgeHealthComponent>();
	OldHealth->StartDeath();
	TestTrue(TEXT("Previous body establishes a death tag"), F.ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death));
	auto* NewPawn = F.NewPawn();
	auto* NewHealth = NewPawn->FindComponentByClass<UHodgeHealthComponent>();
	F.ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 40.f);
	NewHealth->UninitializeFromAbilitySystem();
	// Role 注入只验证客户端本地清理代码，真实复制由 Dedicated 双玩家专项覆盖。
	NewPawn->SetRole(ROLE_AutonomousProxy);
	NewHealth->InitializeWithAbilitySystem(F.ASC);
	TestFalse(TEXT("New client body clears previous local death tags"), F.ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death));
	TestEqual(TEXT("Binding never refills health"), NewHealth->GetHealth(), 40.f);
	OldHealth->InitializeWithAbilitySystem(F.ASC);
	OldHealth->FinishDeath();
	TestFalse(TEXT("Late old finish cannot mark new body dead"), F.ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death));
	NewHealth->StartDeath();
	NewHealth->FinishDeath();
	TestTrue(TEXT("Current client body still applies its own death tag"), F.ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death_Dead));
	NewHealth->UninitializeFromAbilitySystem();
	NewHealth->InitializeWithAbilitySystem(F.ASC);
	TestTrue(TEXT("Binding an already dead current body preserves death"), F.ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death_Dead));
	OldHealth->UninitializeFromAbilitySystem();
	TestTrue(TEXT("Late old unbind cannot clear current death"), F.ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death_Dead));
	NewPawn->SetRole(ROLE_Authority);
	return true;
}
#endif
