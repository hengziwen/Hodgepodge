/** @file HodgeCharacterFacingTests.cpp
 * @brief 实现角色朝向自动化测试相关的类型、接口与现有处理流程。
 */
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "HodgeRelationshipTestAbility.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Dash.h"
#include "Animation/HodgeAnimInstance.h"
#include "Character/HodgeCombatCharacter.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "Data/HodgeSprintAbilityProfile.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "UObject/UnrealType.h"
#include <limits>

/** 保存朝向Lifecycle测试Access的数据和接口契约。 */
struct FHodgeFacingLifecycleTestAccess
{
	/** 检查当前配置或场景的契约条件。
	 * @param Rotation 朝向组件或旋转数据。
	 * @param Handle 对应操作或能力的句柄。
	 * @param Key 键，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	static bool Check(UHodgeCharacterRotationComponent* Rotation, FGameplayAbilitySpecHandle Handle, int32 Key)
	{
		Rotation->ReplicatedState.ActionAbilityHandle = Handle; Rotation->ReplicatedState.ActionPredictionKey = Key;
		return Rotation->IsAuthorityActionCurrent();
	}
};

namespace HodgeFacingTests
{
	/** 保存测试场景的数据和接口契约。 */
	struct FFixture
	{
		// 本次操作所在的世界。
		UWorld* World = nullptr;
		// 角色的数据或对象引用。
		AHodgeCombatCharacter* Character = nullptr;
		// 当前角色使用的控制器。
		APlayerController* Controller = nullptr;
		// 后续GAS操作使用的能力系统组件。
		UHodgeAbilitySystemComponent* ASC = nullptr;
		// 朝向组件或旋转数据。
		UHodgeCharacterRotationComponent* Rotation = nullptr;
		// 角色移动组件或移动数据。
		UHodgeCharacterMovementComponent* Movement = nullptr;
		/** 构造测试场景并初始化其默认状态。 */
		FFixture()
		{
			const UWorld::InitializationValues Values = UWorld::InitializationValues().AllowAudioPlayback(false).RequiresHitProxies(false)
				.CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
			World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			Character = World->SpawnActor<AHodgeCombatCharacter>();
			Controller = World->SpawnActor<APlayerController>(); Controller->Possess(Character);
			ASC = NewObject<UHodgeAbilitySystemComponent>(Character); ASC->RegisterComponent();
			ASC->InitAbilityActorInfo(Character, Character);
			Rotation = Character->GetCharacterRotationComponent();
			Movement = CastChecked<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement());
			Rotation->InitializeWithAbilitySystem(ASC);
		}
		/** 销毁该对象，执行当前类型的析构收尾。 */
		~FFixture() { Rotation->UninitializeFromAbilitySystem(); World->DestroyWorld(false); }
		/** 推进当前用例或实验流程的一个步骤。
		 * @param Seconds 秒，由调用方传入。
		 */
		void Step(float Seconds = 1.f / 60.f) { Movement->PhysicsRotation(Seconds); static_cast<UActorComponent*>(Rotation)->TickComponent(Seconds, LEVELTICK_All, nullptr); }
	};
	/** 按反射类型写入当前蓝图的布尔配置字段。
	 * @param Object 对象，由调用方传入。
	 * @param Name 名称，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool BoolProperty(UObject* Object, FName Name)
	{ return FindFProperty<FBoolProperty>(Object->GetClass(), Name)->GetPropertyValue_InContainer(Object); }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingAuthorityActionTest, "Hodge.Facing.AuthorityActionLifecycle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeFacingAuthorityActionTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	const auto Dash = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_Dash::StaticClass(), 1));
	TestFalse(TEXT("Ended or inactive predicted Dash cannot restore authority action facing"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, Dash, 1));
	const auto Handle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeFacingPredictionTestAbility::StaticClass(), 1));
	// 通过ASC发起激活，保留GAS自身的条件与消耗校验。
	TestTrue(TEXT("Prediction lifecycle fixture activates"), F.ASC->TryActivateAbility(Handle));
	const auto* Spec = F.ASC->FindAbilitySpecFromHandle(Handle);
	const auto* Instance = Spec ? Spec->GetPrimaryInstance() : nullptr;
	// 按预测状态或网络角色选择对应处理。
	if (!Instance) { AddError(TEXT("Prediction fixture instance missing")); return false; }
	const int32 Key = Instance->GetCurrentActivationInfo().GetActivationPredictionKey().Current;
	TestTrue(TEXT("Live matching predicted action remains authoritative"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, Handle, Key));
	TestFalse(TEXT("Previous activation cannot overwrite a newer one"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, Handle, Key + 1));
	F.ASC->CancelAbilityHandle(Handle);
	TestFalse(TEXT("Late matching packet after local end is rejected"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, Handle, Key));
	const auto ServerOnly = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeRelationshipTestAbility::StaticClass(), 1));
	TestTrue(TEXT("Server-only actions retain their authority path"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, ServerOnly, 0));
	TestTrue(TEXT("Component action sources retain their authority path"), FHodgeFacingLifecycleTestAccess::Check(F.Rotation, {}, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingInputTest, "Hodge.Facing.InputSnapshots", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeFacingInputTest::RunTest(const FString& Parameters)
{
	const auto North = HodgeFacing::MakeMoveIntent(FVector2D(0.f, 1.f), 90.f, 1., 3);
	TestTrue(TEXT("Input remains raw control-relative"), North.RawInput2D.Equals(FVector2D(0.f, 1.f)));
	TestTrue(TEXT("Forward input rotates into world Y"), North.DesiredDirectionWorld.Equals(FVector::RightVector, .001));
	TestEqual(TEXT("Avatar identity retained"), North.AvatarGeneration, 3);
	const auto Analog = HodgeFacing::MakeMoveIntent(FVector2D(.3f, .4f), 0.f, 2., 3);
	TestEqual(TEXT("Analog magnitude retained"), Analog.InputMagnitude, .5f);
	TestTrue(TEXT("World direction is horizontal unit"), Analog.DesiredDirectionWorld.Equals(FVector(.8, .6, 0), .001));
	TestTrue(TEXT("No input does not invent forward intent"), HodgeFacing::MakeMoveIntent({}, 0.f, 0., 0).DesiredDirectionWorld.IsZero());
	FVector Output;
	TestFalse(TEXT("Vertical-only action rejected"), HodgeFacing::NormalizeDirection(FVector(0, 0, 20), Output));
	TestFalse(TEXT("NaN direction rejected"), HodgeFacing::NormalizeDirection(FVector(std::numeric_limits<float>::quiet_NaN(), 0, 0), Output));
	TestTrue(TEXT("Invalid control angle sanitized"), HodgeFacing::MakeMoveIntent(FVector2D(1, 1), std::numeric_limits<float>::infinity(), 0., 1).DesiredDirectionWorld.IsZero());
	TestFalse(TEXT("Action driver cannot be used as base"), HodgeFacing::IsBaseDriver(EHodgeCharacterFacingDriver::ActionDirection));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingModesTest, "Hodge.Facing.ModesAndPriority", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeFacingModesTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	TestTrue(TEXT("Free is default"), F.Movement->bOrientRotationToMovement && !F.Character->bUseControllerRotationYaw);
	F.Controller->SetControlRotation(FRotator(0, 90, 0));
	F.Character->FaceRotation(F.Controller->GetControlRotation(), .1f);
	TestTrue(TEXT("Free idle does not turn with camera"), FMath::Abs(F.Character->GetActorRotation().Yaw) < .01f);
	auto Strafe = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, F.Character);
	TestTrue(TEXT("Strafe request applied"), Strafe.IsValid() && F.Character->bUseControllerRotationYaw && !F.Movement->bOrientRotationToMovement);
	TestEqual(TEXT("Animation style follows base"), F.Rotation->GetResolvedState().Style, EHodgeLocomotionStyle::ReservedStrafe);
	F.Character->FaceRotation(F.Controller->GetControlRotation(), .01f);
	TestTrue(TEXT("Controller transition is bounded"), F.Character->GetActorRotation().Yaw > 0.f && F.Character->GetActorRotation().Yaw < 5.f);
	auto Free = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, F.Character);
	TestTrue(TEXT("Latest base request wins"), F.Movement->bOrientRotationToMovement);
	TestTrue(TEXT("Release restores remaining source"), F.Rotation->ReleaseFacingRequest(Free) && F.Character->bUseControllerRotationYaw);
	TestTrue(TEXT("Release last base restores Free"), F.Rotation->ReleaseFacingRequest(Strafe) && F.Movement->bOrientRotationToMovement);
	TestFalse(TEXT("Double release is harmless"), F.Rotation->ReleaseFacingRequest(Strafe));
	TestEqual(TEXT("Released status retained"), F.Rotation->GetFacingRequestStatus(Strafe), EHodgeFacingRequestStatus::Released);
	TestFalse(TEXT("Invalid source rejected"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, GetTransientPackage()).IsValid());
	TestFalse(TEXT("Invalid base enum rejected"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::ActionDirection, F.Character).IsValid());
	F.Controller->UnPossess();
	TestFalse(TEXT("Controller mode requires a controller"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, F.Character).IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingActionTest, "Hodge.Facing.ActionsAndConstraints", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeFacingActionTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	auto Base = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, F.Character);
	auto Action = F.Rotation->RequestActionFacing(FVector::RightVector, F.Character, FGuid::NewGuid());
	TestTrue(TEXT("Action starts pending"), Action.IsValid() && F.Rotation->GetFacingRequestStatus(Action) == EHodgeFacingRequestStatus::Pending);
	TestTrue(TEXT("Action disables competing drivers"), !F.Character->bUseControllerRotationYaw && !F.Movement->bOrientRotationToMovement);
	TestEqual(TEXT("Underlying Strafe is retained"), F.Rotation->GetResolvedState().Style, EHodgeLocomotionStyle::ReservedStrafe);
	F.Step(.01f);
	TestTrue(TEXT("Action preparation turns gradually"), F.Character->GetActorRotation().Yaw > 0 && F.Character->GetActorRotation().Yaw < 10);
	for (int32 I = 0; I < 30; ++I) { F.Step(); }
	TestEqual(TEXT("Applied only after alignment"), F.Rotation->GetFacingRequestStatus(Action), EHodgeFacingRequestStatus::Applied);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	const float Locked = F.Character->GetActorRotation().Yaw;
	TestFalse(TEXT("Strong constraint denies a new action"), F.Rotation->RequestActionFacing(FVector::BackwardVector, F.Character, FGuid::NewGuid(), true).IsValid());
	F.Movement->MoveUpdatedComponent(FVector::ZeroVector, FRotator(0, 180, 0), false);
	TestTrue(TEXT("Final movement respects frozen yaw"), FMath::Abs(FMath::FindDeltaAngleDegrees(Locked, F.Character->GetActorRotation().Yaw)) < .01);
	F.Rotation->ReleaseFacingRequest(Base);
	F.Rotation->ReleaseFacingRequest(Action);
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	TestTrue(TEXT("Other source continues locking"), F.Rotation->IsYawLocked());
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	TestEqual(TEXT("Release restores latest base, not old strafe"), F.Rotation->GetResolvedState().Driver, EHodgeCharacterFacingDriver::Movement);
	auto Instant = F.Rotation->RequestActionFacing(FVector::BackwardVector, F.Character, FGuid::NewGuid(), true);
	TestTrue(TEXT("Explicit instant action applied"), Instant.IsValid() && F.Rotation->GetFacingRequestStatus(Instant) == EHodgeFacingRequestStatus::Applied);
	F.Rotation->ReleaseRequestsForSource(F.Character);
	TestFalse(TEXT("Zero direction rejected"), F.Rotation->RequestActionFacing({}, F.Character, FGuid::NewGuid()).IsValid());
	TestFalse(TEXT("Missing execution identity rejected"), F.Rotation->RequestActionFacing(FVector::ForwardVector, F.Character, {}).IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingLifecycleTest, "Hodge.Facing.LifecycleAndExceptions", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeFacingLifecycleTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	const int32 Generation = F.Rotation->GetAvatarGeneration();
	F.Rotation->InitializeWithAbilitySystem(F.ASC);
	TestEqual(TEXT("Duplicate bind does not create an epoch"), F.Rotation->GetAvatarGeneration(), Generation);
	// 本次操作的来源。
	UObject* Source = NewObject<USkeletalMeshComponent>(F.Character);
	auto Handle = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, Source);
	Source->MarkAsGarbage(); F.Step();
	TestEqual(TEXT("Destroyed source restored Free"), F.Rotation->GetResolvedState().Driver, EHodgeCharacterFacingDriver::Movement);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	TestTrue(TEXT("Death freezes yaw"), F.Rotation->IsYawLocked());
	TestFalse(TEXT("Death denies new base requests"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, F.Character).IsValid());
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Death);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	F.Rotation->UninitializeFromAbilitySystem();
	TestEqual(TEXT("Unbind retains external tag"), F.ASC->GetTagCount(HodgeGameplayTags::Status_Rotation_Locked), 1);
	TestFalse(TEXT("Unbound API refuses requests"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, F.Character).IsValid());
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	F.Rotation->InitializeWithAbilitySystem(F.ASC);
	TestFalse(TEXT("Old handle cannot affect new epoch"), F.Rotation->ReleaseFacingRequest(Handle));
	TArray<FHodgeFacingRequestHandle> Handles;
	for (int32 I = 0; I < 32; ++I) { Handles.Add(F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, F.Character)); }
	TestFalse(TEXT("Request capacity bounds abuse"), F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement, F.Character).IsValid());
	F.Rotation->ReleaseRequestsForSource(F.Character);
	F.ASC->InitAbilityActorInfo(F.Character, nullptr); F.Step();
	TestFalse(TEXT("Lost avatar unbinds"), F.Rotation->IsFacingSystemReady());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingActivationPairTest, "Hodge.Facing.ActivationPair", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeFacingActivationPairTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	const FGuid Execution = FGuid::NewGuid();
	const auto Authority = F.Rotation->RequestActionFacing(FVector::ForwardVector, F.Character, Execution);
	const auto Prediction = F.Rotation->RequestActionFacing(FVector::ForwardVector, F.Character, Execution);
	F.Step();
	TestTrue(TEXT("Paired activation confirms both handles"), F.Rotation->IsActionFacingApplied(Authority) && F.Rotation->IsActionFacingApplied(Prediction));
	const auto NewExecution = F.Rotation->RequestActionFacing(FVector::ForwardVector, F.Character, FGuid::NewGuid()); F.Step();
	TestFalse(TEXT("Different execution cannot confirm the previous action"), F.Rotation->IsActionFacingApplied(Authority));
	F.Rotation->ReleaseFacingRequest(NewExecution); F.Step();
	auto* OtherSource = NewObject<USkeletalMeshComponent>(F.Character); OtherSource->RegisterComponent();
	const auto Other = F.Rotation->RequestActionFacing(FVector::ForwardVector, OtherSource, Execution); F.Step();
	TestFalse(TEXT("Another source cannot authorize the previous action"), F.Rotation->IsActionFacingApplied(Authority));
	F.Rotation->ReleaseFacingRequest(Other); F.Rotation->ReleaseFacingRequest(Prediction); F.Step();
	TestTrue(TEXT("Releasing prediction preserves own authority request"), F.Rotation->IsActionFacingApplied(Authority));
	F.Rotation->ReleaseFacingRequest(Authority);
	TestFalse(TEXT("Released action cannot remain applied"), F.Rotation->IsActionFacingApplied(Authority));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingReplayTest, "Hodge.Facing.ReplayAndNetworkData", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeFacingReplayTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	auto* Prediction = F.Movement->GetPredictionData_Client_Character();
	FSavedMovePtr Free = Prediction->AllocateNewMove(); Free->SetMoveFor(F.Character, .016f, {}, *Prediction);
	auto Handle = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, F.Character);
	FSavedMovePtr Strafe = Prediction->AllocateNewMove(); Strafe->SetMoveFor(F.Character, .016f, {}, *Prediction);
	TestFalse(TEXT("Different modes do not combine"), Free->CanCombineWith(Strafe, F.Character, .125f));
	F.Character->bClientUpdating = true; Free->PrepMoveFor(F.Character);
	TestTrue(TEXT("Replay applies historical flags"), F.Movement->bOrientRotationToMovement && !F.Character->bUseControllerRotationYaw);
	F.Character->bClientUpdating = false; F.Rotation->ClearMoveReplayState();
	TestTrue(TEXT("Replay exit restores current flags"), F.Character->bUseControllerRotationYaw && !F.Movement->bOrientRotationToMovement);
	FHodgeCharacterRotationState State = F.Rotation->GetResolvedState(); State.RequestSequence = 17;
	F.Character->bClientUpdating = true; F.Rotation->SetMoveReplayState(State);
	FSavedMovePtr Saved = Prediction->AllocateNewMove(); Saved->SetMoveFor(F.Character, .016f, {}, *Prediction);
	F.Character->bClientUpdating = false; F.Rotation->ClearMoveReplayState();
	auto& Container = F.Movement->GetFacingMoveDataForTests();
	Container.ClientFillNetworkMoveData(Saved.Get(), nullptr, nullptr);
	TArray<uint8> Bytes; FMemoryWriter Writer(Bytes);
	TestTrue(TEXT("Custom packed move serializes"), Container.Serialize(*F.Movement, Writer, nullptr));
	FMemoryReader Reader(Bytes);
	TestTrue(TEXT("Custom packed move deserializes"), Container.Serialize(*F.Movement, Reader, nullptr));
	TestEqual(TEXT("Packet consumes exactly its payload"), Reader.Tell(), Writer.Tell());
	TestFalse(TEXT("Unknown sequence is not authorization"), F.Rotation->BeginServerMove(999));
	F.Rotation->EndServerMove();
	TestTrue(TEXT("Current mode survives rejected move"), F.Character->bUseControllerRotationYaw);
	F.Rotation->ReleaseFacingRequest(Handle);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeFacingAnimationTest, "Hodge.Facing.AnimationSnapshot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeFacingAnimationTest::RunTest(const FString& Parameters)
{
	HodgeFacingTests::FFixture F;
	// 当前网格使用的动画实例。
	auto* Anim = NewObject<UHodgeAnimInstance>(F.Character->GetMesh());
	static_cast<UAnimInstance*>(Anim)->NativeUpdateAnimation(.12f);
	TestFalse(TEXT("Free style is default animation branch"), HodgeFacingTests::BoolProperty(Anim, TEXT("bUseStrafeLocomotion")));
	TestTrue(TEXT("Free suppresses controller turn compensation"), HodgeFacingTests::BoolProperty(Anim, TEXT("bSuppressLocomotionYaw")));
	auto Strafe = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller, F.Character);
	F.Step(); static_cast<UAnimInstance*>(Anim)->NativeUpdateAnimation(.12f);
	TestTrue(TEXT("Strafe snapshot selects preserved branch"), HodgeFacingTests::BoolProperty(Anim, TEXT("bUseStrafeLocomotion")));
	F.Rotation->ReleaseFacingRequest(Strafe); F.Step(); static_cast<UAnimInstance*>(Anim)->NativeUpdateAnimation(.12f);
	TestFalse(TEXT("Release clears strafe snapshot"), HodgeFacingTests::BoolProperty(Anim, TEXT("bUseStrafeLocomotion")));
	TestTrue(TEXT("Offset only resets after blend weight zero"), HodgeFacingTests::BoolProperty(Anim, TEXT("bResetLocomotionYaw")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeDashInstantFacingTest,"Hodge.Facing.DashInstantStart",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeDashInstantFacingTest::RunTest(const FString&)
{
	HodgeFacingTests::FFixture F; const auto* Profile = NewObject<UHodgeSprintAbilityProfile>();
	TestTrue(TEXT("Dash 默认立即对齐朝向"),Profile->bInstantFacingAtStart);
	const auto LockBase = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Controller,F.Character);
	for (float Angle : {0.f,45.f,90.f,130.f,135.f,180.f,-135.f,-90.f,-45.f})
	{
		F.Character->SetActorRotation(FRotator::ZeroRotator);
		FHodgeDashDirections Directions;
		TestTrue(TEXT("各方向判定有效"),FHodgeDashDirections::Select(FRotator(0.f,Angle,0.f).Vector(),FVector::ForwardVector,Profile->bEnableBackwardVariant,Profile->BackwardConeAngle,Directions));
		const auto DashBase = F.Rotation->AcquireBaseFacingMode(EHodgeCharacterFacingDriver::Movement,F.Character);
		const auto Action = F.Rotation->RequestActionFacing(Directions.FacingDirection,F.Character,FGuid::NewGuid(),Profile->bInstantFacingAtStart);
		TestTrue(FString::Printf(TEXT("%.0f度在返回句柄时已满足 Dash 提交条件"),Angle),F.Rotation->IsActionFacingApplied(Action));
		TestTrue(TEXT("实际角色已朝向蒙太奇对应方向"),FMath::Abs(FMath::FindDeltaAngleDegrees(F.Character->GetActorRotation().Yaw,Directions.FacingDirection.Rotation().Yaw))<.1f);
		F.Rotation->ReleaseFacingRequest(Action);F.Rotation->ReleaseFacingRequest(DashBase);
	}
	F.Rotation->ReleaseFacingRequest(LockBase);return true;
}
#endif
