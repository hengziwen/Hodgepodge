/** @file HodgeDashAirTests.cpp @brief 验证地面起手、离地续冲和 RMS 垂直速度收尾。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "HodgeEnemyAITestSupport.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Sprint.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Character/HodgeCombatCharacter.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeLocomotionPolicyComponent.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/RootMotionSource.h"
#include <limits>

/** 测试只建立最小上下文，不播放动画、不写入正式配置资产。 */
struct FHodgeDashAirTestAccess
{
	/** 使用真实 ASC ActorInfo，提供模式和公共约束检查所需依赖。 */
	static void Bind(UHodgeGameplayAbility_MovementAction* Ability, UHodgeAbilitySystemComponent* ASC,
		UHodgeLocomotionPolicyComponent* Policy, UHodgeCharacterRotationComponent* Rotation, bool bCommitted)
	{
		Ability->CurrentActorInfo = ASC->AbilityActorInfo.Get();
		Ability->Policy = Policy;
		Ability->Rotation = Rotation;
		Ability->bCommitted = bCommitted;
	}
	/** 调用实际公共合法性入口，保留派生 Dash 的模式分派。 */
	static bool Valid(UHodgeGameplayAbility_MovementAction* Ability) { return Ability->IsMovementValid(); }
};

namespace HodgeDashAirTests
{
	/** 临时角色和物理世界在用例结束时统一销毁。 */
	struct FFixture
	{
		UGameInstance* Instance = nullptr;
		UWorld* World = nullptr;
		AHodgeCombatCharacter* Character = nullptr;
		UHodgeAbilitySystemComponent* ASC = nullptr;
		UHodgeCharacterMovementComponent* Movement = nullptr;
		UHodgeCharacterRotationComponent* Rotation = nullptr;
		UHodgeLocomotionPolicyComponent* Policy = nullptr;
		/** 原生角色用于测试 CMC 的实际 RMS 更新与清理。 */
		FFixture()
		{
			World = FHodgeEnemyInitTestAccess::CreateWorld(Instance);
			Character = World->SpawnActor<AHodgeCombatCharacter>();
			World->SpawnActor<APlayerController>()->Possess(Character);
			ASC = NewObject<UHodgeAbilitySystemComponent>(Character);
			ASC->RegisterComponent();
			ASC->InitAbilityActorInfo(Character, Character);
			Movement = CastChecked<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement());
			Rotation = Character->GetCharacterRotationComponent();
			Rotation->InitializeWithAbilitySystem(ASC);
			Policy = Character->FindComponentByClass<UHodgeLocomotionPolicyComponent>();
			Policy->InitializeWithAbilitySystem(ASC);
		}
		/** 配对关闭 GameInstance 和世界，避免残留对象影响下一用例。 */
		~FFixture() { FHodgeEnemyInitTestAccess::DestroyWorld(Instance, World); }
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDashAirEligibilityTest, "Hodge.Movement.DashAir.Eligibility", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeDashAirEligibilityTest::RunTest(const FString&)
{
	HodgeDashAirTests::FFixture F;
	auto* Dash = NewObject<UHodgeGameplayAbility_Dash>(F.Character);
	auto* Sprint = NewObject<UHodgeGameplayAbility_Sprint>(F.Character);
	FHodgeDashAirTestAccess::Bind(Dash, F.ASC, F.Policy, F.Rotation, false);
	FHodgeDashAirTestAccess::Bind(Sprint, F.ASC, F.Policy, F.Rotation, true);
	F.Movement->SetMovementMode(MOVE_Walking);
	TestTrue(TEXT("地面准备 Dash 合法"), FHodgeDashAirTestAccess::Valid(Dash));
	F.Movement->SetMovementMode(MOVE_Falling);
	TestFalse(TEXT("准备阶段离地不开放空中起手"), FHodgeDashAirTestAccess::Valid(Dash));
	FHodgeDashAirTestAccess::Bind(Dash, F.ASC, F.Policy, F.Rotation, true);
	TestTrue(TEXT("已提交 Dash 离地继续"), FHodgeDashAirTestAccess::Valid(Dash));
	TestFalse(TEXT("Sprint 仍不能空中执行"), FHodgeDashAirTestAccess::Valid(Sprint));
	for (EMovementMode Mode : {MOVE_None, MOVE_Flying, MOVE_Swimming})
	{
		F.Movement->SetMovementMode(Mode);
		TestFalse(TEXT("其他模式不被空中 Dash 放宽"), FHodgeDashAirTestAccess::Valid(Dash));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDashAirConstraintsTest, "Hodge.Movement.DashAir.Constraints", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeDashAirConstraintsTest::RunTest(const FString&)
{
	HodgeDashAirTests::FFixture F;
	auto* Dash = NewObject<UHodgeGameplayAbility_Dash>(F.Character);
	FHodgeDashAirTestAccess::Bind(Dash, F.ASC, F.Policy, F.Rotation, true);
	F.Movement->SetMovementMode(MOVE_Falling);
	const FGameplayTag Constraints[] = {HodgeGameplayTags::Status_Death, HodgeHitReactionTags::Controlled, TAG_Gameplay_MovementStopped};
	for (FGameplayTag Tag : Constraints)
	{
		F.ASC->AddLooseGameplayTag(Tag);
		TestFalse(TEXT("离地续冲不能绕过强约束"), FHodgeDashAirTestAccess::Valid(Dash));
		F.ASC->RemoveLooseGameplayTag(Tag);
	}
	TestTrue(TEXT("约束移除后当前上下文仍合法"), FHodgeDashAirTestAccess::Valid(Dash));
	F.ASC->SetAvatarActor(nullptr);
	TestFalse(TEXT("Avatar 解绑使旧 Dash 失效"), FHodgeDashAirTestAccess::Valid(Dash));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDashAirFinishPolicyTest, "Hodge.Movement.DashAir.FinishPolicy", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeDashAirFinishPolicyTest::RunTest(const FString&)
{
	HodgeDashAirTests::FFixture F;
	const FName Source(TEXT("DashAirTest"));
	float Speed = 0.f;
	bool bClamp = false;
	F.Policy->RecordDashMotionVelocity(Source, 600.f, 2.f);
	TestTrue(TEXT("活动来源有地面收尾记录"), F.Policy->GetDashFinishVelocity(Source, false, Speed, bClamp));
	TestFalse(TEXT("地面保持原停止策略"), bClamp);
	F.Policy->GetDashFinishVelocity(Source, true, Speed, bClamp);
	TestTrue(TEXT("Falling 使用限速而非全速度清零"), bClamp);
	TestEqual(TEXT("限速采用记录的移动上限"), Speed, 600.f);
	F.Policy->GetDashFinishVelocity(Source, false, Speed, bClamp);
	TestFalse(TEXT("落地后恢复地面收尾"), bClamp);
	F.Policy->RecordDashExitVelocity(Source, 900.f);
	F.Policy->GetDashFinishVelocity(Source, false, Speed, bClamp);
	TestTrue(TEXT("显式移动交接覆盖活动来源策略"), bClamp);
	TestFalse(TEXT("旧来源不能影响新来源"), F.Policy->GetDashFinishVelocity(TEXT("OtherDash"), true, Speed, bClamp));
	F.Policy->RecordDashMotionVelocity(TEXT("Invalid"), 600.f, std::numeric_limits<float>::quiet_NaN());
	TestFalse(TEXT("非法时长不建立记录"), F.Policy->GetDashFinishVelocity(TEXT("Invalid"), true, Speed, bClamp));
	F.ASC->SetAvatarActor(nullptr);
	TestFalse(TEXT("解绑后记录不能使用"), F.Policy->GetDashFinishVelocity(Source, true, Speed, bClamp));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDashAirPhysicsTest, "Hodge.Movement.DashAir.Physics", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHodgeDashAirPhysicsTest::RunTest(const FString&)
{
	HodgeDashAirTests::FFixture F;
	const FName Source(TEXT("DashAirPhysics"));
	F.Movement->SetMovementMode(MOVE_Falling);
	F.Movement->Velocity = FVector(0.f, 0.f, -400.f);
	F.Policy->RecordDashMotionVelocity(Source, 600.f, .35f);
	auto Force = MakeShared<FRootMotionSource_ConstantForce>();
	Force->InstanceName = Source;
	Force->AccumulateMode = ERootMotionAccumulateMode::Override;
	Force->Priority = 5;
	Force->Force = FVector(800.f, 0.f, 0.f);
	Force->Duration = .35f;
	Force->Settings.SetFlag(ERootMotionSourceSettingsFlags::IgnoreZAccumulate);
	Force->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
	Force->FinishVelocityParams.SetVelocity = FVector::ZeroVector;
	const uint16 Id = F.Movement->ApplyRootMotionSource(Force);
	F.Movement->PerformMovement(.01f);
	const auto LiveSource = F.Movement->GetRootMotionSource(Source);
	TestTrue(TEXT("CMC 对实际活动源选择空中收尾"), LiveSource && LiveSource->FinishVelocityParams.Mode == ERootMotionFinishVelocityMode::ClampVelocity);
	TestTrue(TEXT("覆盖式 RMS 保持水平冲刺"), F.Movement->Velocity.X > 750.f);
	TestTrue(TEXT("重力继续累计下落速度"), F.Movement->Velocity.Z < -400.f);
	// 实际引擎清理必须先应用 CMC 按来源选择的收尾模式。
	F.Movement->RemoveRootMotionSourceByID(Id);
	const float FallingZ = F.Movement->Velocity.Z;
	F.Movement->PerformMovement(.01f);
	AddInfo(FString::Printf(TEXT("清理前 Z=%.3f，清理后 Z=%.3f，模式=%d"), FallingZ, F.Movement->Velocity.Z, static_cast<int32>(F.Movement->MovementMode)));
	TestTrue(TEXT("RMS 清理后下落仍连续加速"), F.Movement->Velocity.Z < FallingZ);
	TestTrue(TEXT("RMS 清理后水平速度受到上限约束"), F.Movement->Velocity.Size2D() <= 601.f);
	return true;
}
#endif
