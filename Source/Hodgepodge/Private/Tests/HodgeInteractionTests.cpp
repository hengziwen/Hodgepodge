/** @file HodgeInteractionTests.cpp @brief 交互底座的真实物理查询、权威身份与过期提交回归；不代替联机测试。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "HodgeInteractionTestTypes.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Interact.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "Character/HodgeHeroCharacter.h"
#include "Component/HodgeHealthComponent.h"
#include "Component/HodgeInteractionComponentBase.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "Data/HodgeCharacterStatProfile.h"
#include "Data/HodgePawnData.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include <limits>

/** 仅模拟请求已经到达服务器，仍执行生产实现的全部权威检查。 */
struct FHodgeInteractionTestAccess
{
	static void Submit(UHodgeInteractionComponentBase* Interaction, AActor* Target, APawn* ExpectedPawn,
		const FGuid& ExpectedRunId, int32 ExpectedRevision)
	{
		Interaction->ServerRequestInteraction_Implementation(Target, ExpectedPawn, ExpectedRunId, ExpectedRevision);
	}
	/** 只注入迟到反馈；保存及读取结果仍经过生产的 Pawn 退休规则。 */
	static void DeliverResult(UHodgeInteractionComponentBase* Interaction, APawn* ExpectedPawn, const FHodgeInteractionResult& Result)
	{
		Interaction->ClientInteractionResult_Implementation(ExpectedPawn, Result);
	}
};

namespace HodgeInteractionTests
{
	/** 最小真实玩家绑定；未加载 Experience，不宣称完整 Hero GameplayReady。 */
	struct FFixture
	{
		UWorld* World = nullptr;
		AHodgePlayerState* PS = nullptr;
		AHodgePlayerController* PC = nullptr;
		AHodgeInteractionTestHero* Pawn = nullptr;
		UHodgeAbilitySystemComponent* ASC = nullptr;
		UHodgePawnData* Data = nullptr;
		UHodgeHealthComponent* Health = nullptr;
		UHodgeInteractionComponentBase* Interaction = nullptr;

		FFixture()
		{
			const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
				.CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
			World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			PS = World->SpawnActor<AHodgePlayerState>();
			PC = World->SpawnActor<AHodgePlayerController>();
			PC->PlayerState = PS;
			PS->SetOwner(PC);
			// 沿用重生测试的持久 Owner 夹具，只装配交互实际依赖的 ASC 与生命绑定。
			ASC = PS->GetHodgeAbilitySystemComponent();
			if (!ASC->GetSet<UHodgeHealthSet>()) { ASC->AddAttributeSetSubobject(CastChecked<UHodgeHealthSet>(PS->GetDefaultSubobjectByName(TEXT("HealthSet")))); }
			if (!ASC->GetSet<UHodgeCombatSet>()) { ASC->AddAttributeSetSubobject(CastChecked<UHodgeCombatSet>(PS->GetDefaultSubobjectByName(TEXT("CombatSet")))); }
			ASC->InitAbilityActorInfo(PS, nullptr);
			Data = NewObject<UHodgePawnData>(PS);
			Data->StatProfile = NewObject<UHodgeCharacterStatProfile>(Data);
			PS->SetPawnData(Data);
			Pawn = World->SpawnActor<AHodgeInteractionTestHero>();
			Pawn->FindComponentByClass<UHodgePawnExtensionComponent>()->SetPawnData(Data);
			PC->Possess(Pawn);
			Pawn->FindComponentByClass<UHodgePawnExtensionComponent>()->InitializeAbilitySystem(ASC, PS);
			ASC->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), 100.f);
			ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
			Health = Pawn->FindComponentByClass<UHodgeHealthComponent>();
			// 使用生产 Controller 的默认组件，覆盖真实组件装配且避免重复实例。
			Interaction = PC->FindComponentByClass<UHodgeInteractionComponentBase>();
		}

		~FFixture()
		{
			World->DestroyWorld(false);
		}

		/** 查询目标由真实碰撞组件进入候选集合，不注入交互组件的内部缓存。 */
		AHodgeInteractionTestTarget* NewTarget(FVector Location = FVector(150.f, 0.f, 0.f)) const
		{
			return World->SpawnActor<AHodgeInteractionTestTarget>(Location, FRotator::ZeroRotator);
		}

		/** 宽遮挡箱覆盖眼睛到目标的整条测试视线，避免依赖某一角色眼高。 */
		UBoxComponent* NewOccluder() const
		{
			auto* Actor = World->SpawnActor<AActor>();
			auto* Box = NewObject<UBoxComponent>(Actor);
			Actor->AddInstanceComponent(Box);
			Actor->SetRootComponent(Box);
			Box->InitBoxExtent(FVector(8.f, 100.f, 200.f));
			Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Box->SetCollisionObjectType(ECC_WorldStatic);
			Box->SetCollisionResponseToAllChannels(ECR_Ignore);
			Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			Box->RegisterComponent();
			Actor->SetActorLocation(FVector(75.f, 0.f, 0.f));
			return Box;
		}

		/** 使用调用前保存的目标选项模拟真正的客户端请求快照。 */
		void Submit(AHodgeInteractionTestTarget* Target, const FHodgeInteractionOption& Option) const
		{
			FHodgeInteractionTestAccess::Submit(Interaction, Target, Pawn, Option.RunId, Option.Revision);
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeInteractionIdentityTest, "Hodge.Interaction.AuthorityIdentity", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeInteractionIdentityTest::RunTest(const FString&)
{
	HodgeInteractionTests::FFixture F;
	auto* Target = F.NewTarget();
	TestTrue(TEXT("夹具生命绑定可用"), F.Health->GetHealth() > 0.f && !F.Health->IsDeadOrDying());
	// 请求身份必须在到达时重新核对，不能信任调用者提供的旧身体。
	FHodgeInteractionTestAccess::Submit(F.Interaction, Target, nullptr, Target->Option.RunId, Target->Option.Revision);
	TestFalse(TEXT("空 Pawn 请求被拒绝"), F.Interaction->GetLastResult().bSucceeded);
	auto* OtherPawn = F.World->SpawnActor<APawn>();
	FHodgeInteractionTestAccess::Submit(F.Interaction, Target, OtherPawn, Target->Option.RunId, Target->Option.Revision);
	TestFalse(TEXT("未占有 Pawn 请求被拒绝"), F.Interaction->GetLastResult().bSucceeded);
	F.ASC->InitAbilityActorInfo(F.PS, OtherPawn);
	F.Submit(Target, Target->Option);
	TestFalse(TEXT("ASC 已换 Avatar 时拒绝旧身体"), F.Interaction->GetLastResult().bSucceeded);
	F.ASC->InitAbilityActorInfo(F.PS, F.Pawn);
	F.PC->PlayerState = nullptr;
	F.Submit(Target, Target->Option);
	TestFalse(TEXT("失去 PlayerState 后拒绝请求"), F.Interaction->GetLastResult().bSucceeded);
	F.PC->PlayerState = F.PS;
	F.ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 0.f);
	F.Submit(Target, Target->Option);
	TestFalse(TEXT("零生命且尚未死亡通知时仍拒绝"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("非法身份没有到达目标执行"), Target->ExecuteAttempts, 0);
	F.ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
	F.Submit(Target, Target->Option);
	TestTrue(TEXT("身份恢复后允许真实提交"), F.Interaction->GetLastResult().bSucceeded);
	TestTrue(TEXT("目标收到当前被占有身体"), Target->LastExecutingPawn.Get() == F.Pawn);
	FHodgeInteractionTestAccess::Submit(F.Interaction, Target, OtherPawn, Target->Option.RunId, Target->Option.Revision);
	TestTrue(TEXT("其他身体的迟到拒绝不覆盖当前成功结果"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("迟到其他身体请求没有额外执行"), Target->ExecuteAttempts, 1);
	F.Health->StartDeath();
	F.Submit(Target, Target->Option);
	TestFalse(TEXT("死亡开始后即使生命非零也拒绝"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("死亡请求不产生额外执行"), Target->ExecuteAttempts, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeInteractionRevisionTest, "Hodge.Interaction.RunRevisionAndDuplicateRequest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeInteractionRevisionTest::RunTest(const FString&)
{
	HodgeInteractionTests::FFixture F;
	auto* Target = F.NewTarget();
	const auto Initial = Target->Option;
	auto Stale = Initial;
	Stale.RunId = FGuid::NewGuid();
	F.Submit(Target, Stale);
	TestFalse(TEXT("旧轮次不能提交"), F.Interaction->GetLastResult().bSucceeded);
	Stale = Initial;
	--Stale.Revision;
	F.Submit(Target, Stale);
	TestFalse(TEXT("旧修订不能提交"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("过期请求在组件中被拒绝"), Target->ExecuteAttempts, 0);
	F.Submit(Target, Initial);
	TestTrue(TEXT("有效快照首次提交成功"), F.Interaction->GetLastResult().bSucceeded);
	F.Submit(Target, Initial);
	TestFalse(TEXT("重复快照已失效"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("重复请求没有再次进入目标提交"), Target->ExecuteAttempts, 1);
	TestEqual(TEXT("目标副作用只发生一次"), Target->SuccessfulExecutions, 1);
	F.Submit(Target, Target->Option);
	TestTrue(TEXT("新的合法选项仍可执行"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("新修订可产生下一次业务提交"), Target->SuccessfulExecutions, 2);
	Target->bOptionAvailable = false;
	F.Submit(Target, Target->Option);
	TestFalse(TEXT("目标已撤销选项则拒绝"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("不可用选项不进入提交"), Target->ExecuteAttempts, 2);
	Target->bOptionAvailable = true;
	Target->bRejectExecution = true;
	F.Submit(Target, Target->Option);
	TestFalse(TEXT("目标业务失败原样返回"), F.Interaction->GetLastResult().bSucceeded);
	TestFalse(TEXT("业务失败保留可定位原因"), F.Interaction->GetLastResult().Reason.IsEmpty());
	TestEqual(TEXT("业务失败不伪装成功副作用"), Target->SuccessfulExecutions, 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeInteractionSpatialTest, "Hodge.Interaction.AuthorityRangeAndVisibility", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeInteractionSpatialTest::RunTest(const FString&)
{
	HodgeInteractionTests::FFixture F;
	auto* Target = F.NewTarget(FVector(350.f, 0.f, 0.f));
	F.Submit(Target, Target->Option);
	TestFalse(TEXT("直接传入超距目标也会拒绝"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("超距目标没有执行"), Target->ExecuteAttempts, 0);
	Target->SetActorLocation(FVector(150.f, 0.f, 0.f));
	auto* Occluder = F.NewOccluder();
	F.Submit(Target, Target->Option);
	TestFalse(TEXT("服务器重新检测实际遮挡"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("有遮挡时未进入目标提交"), Target->ExecuteAttempts, 0);
	TestFalse(TEXT("本地候选查询也过滤遮挡"), F.Interaction->RequestNearestInteraction());
	Occluder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TestTrue(TEXT("移除遮挡后真实物理候选可交互"), F.Interaction->RequestNearestInteraction());
	TestTrue(TEXT("权威交互结果成功"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("通过物理查询只执行一次"), Target->SuccessfulExecutions, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeInteractionCandidateTest, "Hodge.Interaction.NearestPhysicalCandidate", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeInteractionCandidateTest::RunTest(const FString&)
{
	HodgeInteractionTests::FFixture F;
	auto* Far = F.NewTarget(FVector(180.f, -100.f, 0.f));
	auto* Near = F.NewTarget(FVector(100.f, 100.f, 0.f));
	auto* NoCollision = F.NewTarget(FVector(30.f, 0.f, 0.f));
	NoCollision->InteractionShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TestTrue(TEXT("真实候选扫描能够发起交互"), F.Interaction->RequestNearestInteraction());
	TestEqual(TEXT("选择最近的物理候选"), Near->SuccessfulExecutions, 1);
	TestEqual(TEXT("更远候选没有执行"), Far->ExecuteAttempts, 0);
	TestEqual(TEXT("无查询碰撞对象不会混入候选"), NoCollision->ExecuteAttempts, 0);
	Near->bOptionAvailable = false;
	TestTrue(TEXT("最近目标不可用后选择次近合法项"), F.Interaction->RequestNearestInteraction());
	TestEqual(TEXT("次近候选提交成功"), Far->SuccessfulExecutions, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeInteractionGeometryAndRetirementTest, "Hodge.Interaction.FiniteGeometryAndResultRetirement", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeInteractionGeometryAndRetirementTest::RunTest(const FString&)
{
	HodgeInteractionTests::FFixture F;
	auto* Target = F.NewTarget();
	F.Submit(Target, Target->Option);
	TestTrue(TEXT("先建立当前身体的成功结果"), F.Interaction->GetLastResult().bSucceeded);
	F.Pawn->bInvalidViewLocation = true;
	F.Submit(Target, Target->Option);
	TestFalse(TEXT("NaN 视线端点的服务器请求被拒绝"), F.Interaction->GetLastResult().bSucceeded);
	TestFalse(TEXT("NaN 视线端点不会成为本地候选"), F.Interaction->RequestNearestInteraction());
	TestEqual(TEXT("非法几何没有进入物理目标提交"), Target->ExecuteAttempts, 1);
	F.Pawn->bInvalidViewLocation = false;
	F.Submit(Target, Target->Option);
	const auto Success = F.Interaction->GetLastResult();
	TestTrue(TEXT("几何恢复后可再次成功"), Success.bSucceeded);
	F.PC->UnPossess();
	TestFalse(TEXT("无当前 Pawn 时不暴露旧成功"), F.Interaction->GetLastResult().bSucceeded);
	FHodgeInteractionTestAccess::DeliverResult(F.Interaction, F.Pawn, Success);
	TestFalse(TEXT("旧身体迟到反馈不能恢复成功"), F.Interaction->GetLastResult().bSucceeded);
	const TWeakObjectPtr<APawn> OldPawn = F.Pawn;
	F.Pawn->Destroy();
	TestFalse(TEXT("销毁后旧身体弱引用失效"), OldPawn.IsValid());
	TestFalse(TEXT("当前和旧弱引用均为空时也不返回旧成功"), F.Interaction->GetLastResult().bSucceeded);
	FHodgeInteractionTestAccess::DeliverResult(F.Interaction, nullptr, Success);
	TestFalse(TEXT("无身体反馈不能制造有效成功结果"), F.Interaction->GetLastResult().bSucceeded);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeInteractionGASTest, "Hodge.Interaction.GASShortCommand", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeInteractionGASTest::RunTest(const FString&)
{
	HodgeInteractionTests::FFixture F;
	auto* Target = F.NewTarget();
	// 真实授予与激活能力，不直接调用 ActivateAbility 或假造 AbilityActorInfo。
	const auto Handle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_Interact::StaticClass(), 1));
	const bool bActivated = F.ASC->TryActivateAbility(Handle);
	TestTrue(TEXT("本地交互短能力可由 ASC 激活"), bActivated);
	TestEqual(TEXT("能力通过默认 Controller 组件实际提交一次"), Target->SuccessfulExecutions, 1);
	const auto* Spec = F.ASC->FindAbilitySpecFromHandle(Handle);
	TestTrue(TEXT("交互结束不残留活跃能力"), Spec && !Spec->IsActive());
	F.ASC->ClearAbility(Handle);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeInteractionConfiguredDistanceTest, "Hodge.Interaction.PawnDataDistanceConfiguration", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeInteractionConfiguredDistanceTest::RunTest(const FString&)
{
	HodgeInteractionTests::FFixture F;
	auto* Target = F.NewTarget();
	const FVector PawnLocation = F.Pawn->GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();
	TestEqual(TEXT("旧配置默认交互距离仍为二百五十厘米"), F.Data->InteractionDistance, 250.f);
	TestEqual(TEXT("组件从当前 PlayerState PawnData 读取距离"), F.Interaction->GetInteractionDistance(), 250.f);
	// 只改变真实配置，固定同一目标的一百五十厘米物理距离。
	F.Data->InteractionDistance = 100.f;
	TestFalse(TEXT("配置一百厘米时本地查询不接受该目标"), F.Interaction->RequestNearestInteraction());
	F.Submit(Target, Target->Option);
	TestFalse(TEXT("直接服务器请求也使用同一百厘米配置"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("配置超距请求未进入目标执行"), Target->ExecuteAttempts, 0);
	F.Data->InteractionDistance = 200.f;
	TestTrue(TEXT("配置二百厘米后真实查询接受同一目标"), F.Interaction->RequestNearestInteraction());
	TestTrue(TEXT("配置扩大后真实权威提交成功"), F.Interaction->GetLastResult().bSucceeded);
	TestEqual(TEXT("合法距离只执行一次"), Target->SuccessfulExecutions, 1);
	for (const float Invalid : {-1.f, 0.f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity()})
	{
		F.Data->InteractionDistance = Invalid;
		TestFalse(TEXT("负数、零或非有限配置在物理查询前被拒绝"), F.Interaction->RequestNearestInteraction());
		F.Submit(Target, Target->Option);
		TestFalse(TEXT("非法距离的服务器请求同样失败"), F.Interaction->GetLastResult().bSucceeded);
		TestFalse(TEXT("非法距离保留可定位诊断"), F.Interaction->GetLastResult().Reason.IsEmpty());
		TestEqual(TEXT("非法配置不能到达目标执行"), Target->ExecuteAttempts, 1);
	}
	TestTrue(TEXT("配置测试没有移动玩家"), F.Pawn->GetActorLocation().Equals(PawnLocation));
	TestTrue(TEXT("配置测试没有移动目标"), Target->GetActorLocation().Equals(TargetLocation));
	F.Data->InteractionDistance = 200.f;
	TestTrue(TEXT("修复配置后可重新通过真实交互路径"), F.Interaction->RequestNearestInteraction());
	TestEqual(TEXT("恢复后只新增一次合法执行"), Target->SuccessfulExecutions, 2);
	return true;
}
#endif
