/** @file HodgeHitReactionFacingMovementTests.cpp
 * @brief 用实际CMC写入、SavedMove和原生重放验证已提交受击段的有限补转；不替代PIE联机验收。
 */
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "Combat/HodgeHitReactionMotion.h"
#include "Character/HodgeCombatCharacter.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeHitReactionComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/HodgeHitReactionProfile.h"
#include "Data/HodgePawnData.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/RootMotionSource.h"
#include "Misc/OutputDevice.h"
#include "Misc/OutputDeviceRedirector.h"
#include "Templates/Atomic.h"

/** 只构造提交后的测试事实并访问生产移动入口；不伪造BeginHit或联机成功证据。 */
struct FHodgeHitReactionFacingMovementTestAccess
{
	using FSnapshot = UHodgeCharacterMovementComponent::FHitReactionFacingMoveSnapshot;
	/** 安装已成功提交的受击段，生命、ASC与Actor身份仍由真实组件核验。 */
	static void Install(UHodgeHitReactionComponent* Reaction, const FHodgeHitReactionFacingState& Facing)
	{
		Reaction->State = {};
		Reaction->State.Sequence = 1;
		Reaction->State.Phase = EHodgeHitReactionPhase::Controlled;
		Reaction->State.Type = EHodgeImpactType::HitStun;
		Reaction->State.EndsAt = 100.f;
		Reaction->FacingState = Facing;
		Reaction->SetControl(true);
	}
	/** 测试只提供时钟采样点，生产算法自行计算有效窗口交集和限速。 */
	static FSnapshot Snapshot(UHodgeCharacterMovementComponent* Movement, float At, float From, bool bPaused = false)
	{
		FSnapshot Result = Movement->CaptureHitReactionFacing(At - From);
		Result.EvaluatedAt = At;
		Result.EvaluationStartedAt = From;
		Result.bPaused = bPaused;
		return Result;
	}
	/** 运行真实补转算法，Actor旋转仍通过CMC的MoveUpdatedComponent写入。 */
	static void Step(UHodgeCharacterMovementComponent* Movement, const FSnapshot& Snapshot, float DeltaTime)
	{ Movement->ApplyHitReactionFacing(Snapshot, DeltaTime); }
	/** 调用生产校正入口，恢复标记只模拟原生重放结束的调用位置。 */
	static void Correct(UHodgeCharacterMovementComponent* Movement, bool bRestore = false)
	{ Movement->ApplyHitReactionFacingCorrection(bRestore); }
	/** 查询是否已经消费当前最终修订，不能以最终角度代替一次性语义。 */
	static bool Consumed(UHodgeCharacterMovementComponent* Movement, const FHodgeHitReactionFacingState& Facing)
	{
		return Movement->CorrectedFacingChainId == Facing.ChainId && Movement->CorrectedFacingSegment == Facing.Segment &&
			Movement->CorrectedFacingRevision == Facing.Revision;
	}
	/** 观察PrepMoveFor实际安装的历史快照，不依赖SavedMove私有实现布局。 */
	static TOptional<FSnapshot> Replay(UHodgeCharacterMovementComponent* Movement) { return Movement->HitReactionReplay; }
	/** 检查原生回放中的控制裁决，不能只看当前ASC标签而遗漏旧历史覆盖。 */
	static bool Controlled(UHodgeCharacterMovementComponent* Movement) { return Movement->IsHitReactionControlled(); }
	/** 检查原生纠正携带Rotation时不会恢复纠正前的当前驱动锚点。 */
	static bool MayRestore(UHodgeCharacterMovementComponent* Movement)
	{ return Movement->HitReactionReplayAnchor.IsSet() && Movement->HitReactionReplayAnchor->bMayRestore; }
	/** 仅替换已提交计划的运动来源，真实提交/升级由独立Lifecycle用例覆盖。 */
	static void SetMotionSource(UHodgeHitReactionComponent* Reaction, EHodgeReactionMotionSource Source)
	{ Reaction->State.MotionSource = Source; }
	/** 模拟同Pawn复制先于ASC依赖到达，仅写复制数据并执行其真实OnRep入口。 */
	static void ReceiveBeforeFirstBinding(UHodgeHitReactionComponent* Reaction, const FHodgeHitReactionFacingState& Facing)
	{
		Install(Reaction, Facing);
		Reaction->Profile = Reaction->OverrideProfile;
		Reaction->OnRep_Profile();
		Reaction->OnRep_FacingState();
	}
	/** 退休测试段保持当前朝向，不制造额外恢复转身。 */
	static void Retire(UHodgeHitReactionComponent* Reaction)
	{
		Reaction->ClearFacing();
		Reaction->State.Phase = EHodgeHitReactionPhase::None;
		Reaction->SetControl(false);
	}
};

namespace HodgeHitReactionFacingMovementTests
{
	/** 按完整Avatar路径统计真实校正日志，避免终点相同掩盖同修订重复消费。 */
	class FCorrectionCounter final : public FOutputDevice
	{
	public:
		/** 同步等待真实日志线程处理完已有样本，再读取一次性消费计数。 */
		int32 GetCount() const { GLog->FlushThreadedLogs(); return Count.Load(); }
		/** 原生响应与已消费Final恢复分别计数，不能借重复包生成新授权。 */
		int32 GetNativeCount() const { GLog->FlushThreadedLogs(); return NativeCount.Load(); }
		/** 冻结恢复不等于第二次Final消费。 */
		int32 GetFrozenRestoreCount() const { GLog->FlushThreadedLogs(); return FrozenRestoreCount.Load(); }
		/** 拒绝恢复只证明净零变化，不能计为原生新ACK或Final授权。 */
		int32 GetRejectedRestoreCount() const { GLog->FlushThreadedLogs(); return RejectedRestoreCount.Load(); }
		/** 日志监听限于用例作用域，保留引擎原有的全部输出设备。 */
		explicit FCorrectionCounter(const AActor* Avatar) : AvatarToken(TEXT("Avatar=") + Avatar->GetPathName() + TEXT(" "))
		{ GLog->AddOutputDevice(this); }
		/** 清理本用例监听，不影响其他自动化用例。 */
		virtual ~FCorrectionCounter() override { GLog->RemoveOutputDevice(this); }
		/** 按事件独立计数，历史或冻结恢复不能算成第二次Final消费。 */
		virtual void Serialize(const TCHAR* Text, ELogVerbosity::Type Verbosity, const FName& Category) override
		{
			const FString Message(Text);
			if (Category == TEXT("LogHodgeHitReactionFacing") && Message.Contains(AvatarToken))
			{
				if (Message.StartsWith(TEXT("Event=Correction "))) { ++Count; }
				else if (Message.StartsWith(TEXT("Event=NativeCorrection "))) { ++NativeCount; }
				else if (Message.StartsWith(TEXT("Event=FrozenRestore "))) { ++FrozenRestoreCount; }
				else if (Message.StartsWith(TEXT("Event=NativeRejectedRestore "))) { ++RejectedRestoreCount; }
			}
		}
	private:
		FString AvatarToken;
		TAtomic<int32> Count{0};
		TAtomic<int32> NativeCount{0};
		TAtomic<int32> FrozenRestoreCount{0};
		TAtomic<int32> RejectedRestoreCount{0};
	};
	/** 独立世界只验证运行时代码；Profile和段均为瞬态，不改正式资产。 */
	struct FFixture
	{
		UWorld* World;
		AHodgeCombatCharacter* Character;
		APlayerController* Controller;
		UHodgeAbilitySystemComponent* ASC;
		UHodgeCharacterMovementComponent* Movement;
		UHodgeCharacterRotationComponent* Rotation;
		UHodgeHitReactionComponent* Reaction;
		/** 复用PawnExtension初始化ASC，确保角色接口和各组件读取同一ASC。 */
		explicit FFixture(bool bDeferFirstBinding = false)
		{
			const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).RequiresHitProxies(false)
				.CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
			World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
			Character = World->SpawnActor<AHodgeCombatCharacter>();
			Controller = World->SpawnActor<APlayerController>();
			Controller->Possess(Character);
			ASC = NewObject<UHodgeAbilitySystemComponent>(Character);
			Character->AddInstanceComponent(ASC);
			ASC->RegisterComponent();
			ASC->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(Character));
			ASC->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), 100.f);
			ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
			Reaction = Character->GetHitReactionComponent();
			Reaction->OverrideProfile = NewObject<UHodgeHitReactionProfile>(Character);
			auto* Data = NewObject<UHodgePawnData>(Character);
			Data->HitReactionProfile = Reaction->OverrideProfile;
			auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(Character);
			Extension->SetPawnData(Data);
			if (!bDeferFirstBinding) { Extension->InitializeAbilitySystem(ASC, Character); }
			Rotation = Character->GetCharacterRotationComponent();
			Movement = CastChecked<UHodgeCharacterMovementComponent>(Character->GetCharacterMovement());
			Movement->SetUpdatedComponent(Character->GetCapsuleComponent());
			Movement->Activate();
			Movement->SetMovementMode(MOVE_Flying);
			Movement->GravityScale = 0.f;
			Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			FHodgeHitReactionFacingState Facing;
			Facing.ChainId = FGuid::NewGuid(); Facing.Segment = Facing.Revision = 1;
			Facing.Preset = EHodgeHitReactionFacingPreset::Heavy;
			Facing.StartedAt = -.02f; Facing.EndsAt = .08f;
			Facing.TargetYaw = 90.f; Facing.MaxYawSpeed = 720.f; Facing.bHasTarget = true;
			FHodgeHitReactionFacingMovementTestAccess::Install(Reaction, Facing);
		}
		/** 恢复测试网络角色后按真实解绑路径清理委托和控制占用。 */
		~FFixture()
		{
			Character->SetRole(ROLE_Authority);
			UHodgePawnExtensionComponent::FindPawnExtensionComponent(Character)->UninitializeAbilitySystem();
			World->DestroyWorld(false);
		}
		/** 修改提交后的权威快照，限于网络收包或段升级的测试准备。 */
		void SetFacing(const FHodgeHitReactionFacingState& Facing)
		{ FHodgeHitReactionFacingMovementTestAccess::Install(Reaction, Facing); }
		/** 显式模拟本地拥有者角色；本用例不声明建立真实网络连接。 */
		void Owner() { Character->SetRole(ROLE_AutonomousProxy); }
		/** 通过瞬移准备纠正前姿态，不把它计作普通有限补转。 */
		void Pose(float Yaw)
		{ Character->SetActorRotation(FRotator(0.f, Yaw, 0.f), ETeleportType::TeleportPhysics); }
		/** 录制实际项目SavedMove，交给引擎原生回放循环消费。 */
		FSavedMovePtr Record(float TimeStamp, float DeltaTime = .016f)
		{
			auto* Prediction = Movement->GetPredictionData_Client_Character();
			FSavedMovePtr Move = Prediction->AllocateNewMove();
			Move->Clear();
			Move->SetMoveFor(Character, DeltaTime, FVector::ZeroVector, *Prediction);
			Move->TimeStamp = TimeStamp;
			return Move;
		}
		/** 真实纠正先确认首个移动，再重放第二个移动，覆盖LastAckedMove回退入口。 */
		bool CorrectAndReplay(bool bHasRotation = false, float AuthorityYaw = 0.f)
		{
			FCharacterMoveResponseDataContainer Response;
			Response.ClientAdjustment.TimeStamp = .016f;
			Response.ClientAdjustment.NewLoc = Character->GetActorLocation();
			Response.ClientAdjustment.NewVel = FVector::ZeroVector;
			Response.ClientAdjustment.MovementMode = Movement->PackNetworkMovementMode();
			Response.bHasRotation = bHasRotation;
			Response.ClientAdjustment.NewRot = FRotator(0.f, AuthorityYaw, 0.f);
			Movement->ClientHandleMoveResponse(Response);
			return Movement->ClientUpdatePositionAfterServerUpdate();
		}
		/** 为原生确认和重放各保留一个移动，避免仅调用自定义Prep冒充真实重放。 */
		void QueueHistory()
		{
			Movement->GetPredictionData_Client_Character()->SavedMoves.Add(Record(.016f));
			Movement->GetPredictionData_Client_Character()->SavedMoves.Add(Record(.032f));
		}
		/** 进入真实legacy RMS或Packed Rotation路径，不直接写入原生ACK与锚点缓存。 */
		void NativeCorrection(bool bRMS, float TimeStamp, float Yaw, bool bUnresolvedRelativeBase = false, bool bAnimRootMotion = false)
		{
			if (bRMS)
			{
				Movement->ClientAdjustRootMotionSourcePosition_Implementation(TimeStamp, FRootMotionSourceGroup(), bAnimRootMotion,
					0.f, Character->GetActorLocation(), FVector(0.f, Yaw / 180.f, 0.f), 0.f, nullptr, NAME_None,
					bUnresolvedRelativeBase, bUnresolvedRelativeBase, Movement->PackNetworkMovementMode());
			}
			else
			{
				FCharacterMoveResponseDataContainer Response;
				Response.ClientAdjustment.TimeStamp = TimeStamp; Response.ClientAdjustment.NewLoc = Character->GetActorLocation();
				Response.ClientAdjustment.MovementMode = Movement->PackNetworkMovementMode();
				Response.ClientAdjustment.NewRot = FRotator(0.f, Yaw, 0.f); Response.bHasRotation = true;
				Response.bHasBase = Response.ClientAdjustment.bBaseRelativePosition = bUnresolvedRelativeBase;
				Movement->ClientHandleMoveResponse(Response);
			}
		}
	};
	/** 断言水平角差，允许浮点误差而不放宽生产限速。 */
	bool NearYaw(float Actual, float Expected)
	{ return FMath::Abs(FMath::FindDeltaAngleDegrees(Actual, Expected)) <= .01f; }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingRateWindowTest, "Hodge.HitReaction.FacingMovement.RateAndWorldWindow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 步长抖动只能消费有效世界时窗，过期只记录实际Yaw，不能硬补到目标。 */
bool FHodgeHitFacingRateWindowTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	FFixture F;
	auto Facing = F.Reaction->GetFacingState(); Facing.StartedAt = 0.f; Facing.EndsAt = .04f; F.SetFacing(Facing);
	F.Movement->Velocity = FVector(75.f, -20.f, 12.f);
	const FVector BeforeVelocity = F.Movement->Velocity;
	for (const FVector Sample : {FVector(.01, 0, .01), FVector(.02, .01, .01), FVector(.06, .02, .04)})
	{
		const float BeforeYaw = F.Character->GetActorRotation().Yaw;
		const auto Snapshot = FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, Sample.X, Sample.Y);
		FHodgeHitReactionFacingMovementTestAccess::Step(F.Movement, Snapshot, Sample.Z);
		TestTrue(TEXT("每步受限于有效CMC步长且不越过目标"), F.Character->GetActorRotation().Yaw >= BeforeYaw &&
			F.Character->GetActorRotation().Yaw - BeforeYaw <= 720.f * Sample.Z + .05f && F.Character->GetActorRotation().Yaw <= 90.f);
	}
	TestTrue(TEXT("40毫秒窗口只允许28.8度实际累计转动"), NearYaw(F.Character->GetActorRotation().Yaw, 28.8f));
	TestTrue(TEXT("到期公布实际结束朝向"), F.Reaction->GetFacingState().bFinal && NearYaw(F.Reaction->GetFacingState().FinalYaw, 28.8f));
	TestTrue(TEXT("补转不旋转或重算世界运动速度"), F.Movement->Velocity.Equals(BeforeVelocity));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingPauseTest, "Hodge.HitReaction.FacingMovement.PauseConsumesWindow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 局部暂停不能积攒可补播时长，过期后恢复也不重开普通转向。 */
bool FHodgeHitFacingPauseTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	FFixture F;
	FHodgeHitReactionFacingMovementTestAccess::Step(F.Movement,
		FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, .04f, 0.f, true), .04f);
	TestTrue(TEXT("暂停时身体不转"), NearYaw(F.Character->GetActorRotation().Yaw, 0.f));
	FHodgeHitReactionFacingMovementTestAccess::Step(F.Movement,
		FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, .05f, .04f), .01f);
	TestTrue(TEXT("恢复只消费剩余的一步，不追补暂停时间"), NearYaw(F.Character->GetActorRotation().Yaw, 7.2f));
	FHodgeHitReactionFacingMovementTestAccess::Step(F.Movement,
		FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, .1f, .05f, true), .05f);
	TestTrue(TEXT("暂停期间世界窗口仍到期并冻结实际结果"), F.Reaction->GetFacingState().bFinal);
	FHodgeHitReactionFacingMovementTestAccess::Step(F.Movement,
		FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, .12f, .1f), .02f);
	TestTrue(TEXT("过期恢复不补播"), NearYaw(F.Character->GetActorRotation().Yaw, 7.2f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingHardLockTest, "Hodge.HitReaction.FacingMovement.HardLockPermanent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 真实ASC标签回调在硬锁建立时永久结束本段，解除标签也不恢复旧目标。 */
bool FHodgeHitFacingHardLockTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	FFixture F;
	FHodgeHitReactionFacingMovementTestAccess::Step(F.Movement,
		FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, .01f, 0.f), .01f);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	const auto Frozen = F.Reaction->GetFacingState();
	TestTrue(TEXT("硬锁回调冻结同段实际朝向并提升修订"), Frozen.bFinal && Frozen.bHardLocked && Frozen.Revision == 2);
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	FHodgeHitReactionFacingMovementTestAccess::Step(F.Movement,
		FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, .06f, .01f), .05f);
	TestTrue(TEXT("解锁不能重开本段转向"), NearYaw(F.Character->GetActorRotation().Yaw, Frozen.FinalYaw));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingHardLockCorrectionTest, "Hodge.HitReaction.FacingMovement.HardLockKeepsAuthoritativeFinal",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 客户端先冻结不同Yaw时，同段Final校正必须成为持续硬锁和解锁后的唯一冻结参考。 */
bool FHodgeHitFacingHardLockCorrectionTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	FFixture F; F.Owner(); F.Pose(60.f);
	FCorrectionCounter Counter(F.Character);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	const auto LocalSnapshot = FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, .016f, 0.f);
	TestTrue(TEXT("真实硬锁回调先冻结拥有者的60度姿态"), LocalSnapshot.bLocallyFrozen &&
		NearYaw(F.Character->GetActorRotation().Yaw, 60.f));
	auto Facing = F.Reaction->GetFacingState();
	Facing.bFinal = Facing.bHardLocked = true; Facing.FinalYaw = 0.f; ++Facing.Revision; F.SetFacing(Facing);
	F.Movement->PerformMovement(.016f);
	TestTrue(TEXT("首个允许的真实移动消费同段权威0度Final"), NearYaw(F.Character->GetActorRotation().Yaw, 0.f) &&
		FHodgeHitReactionFacingMovementTestAccess::Consumed(F.Movement, Facing));
	for (int32 Step = 0; Step < 3; ++Step)
	{
		F.Movement->PerformMovement(.016f);
		TestTrue(TEXT("硬锁持续求值不能回填校正前的60度冻结参考"), NearYaw(F.Character->GetActorRotation().Yaw, 0.f));
	}
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
	F.Movement->PerformMovement(.016f);
	const auto FinalSnapshot = FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, .08f, .064f);
	TestTrue(TEXT("解锁后保持同一最终段且不重新补转"), NearYaw(F.Character->GetActorRotation().Yaw, 0.f) &&
		FinalSnapshot.bLocallyFrozen && FinalSnapshot.Facing.bFinal && FinalSnapshot.Facing.bHardLocked &&
		FinalSnapshot.Facing.ChainId == Facing.ChainId && FinalSnapshot.Facing.Segment == Facing.Segment &&
		FinalSnapshot.Facing.Revision == Facing.Revision);
	TestEqual(TEXT("缓存同步不新增Final校正次数"), Counter.GetCount(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingCorrectionTest, "Hodge.HitReaction.FacingMovement.FinalCorrectionOnceAndPause",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 最终修订只消费一次，暂停延后消费，无源合法受击不会被可选转向校正。 */
bool FHodgeHitFacingCorrectionTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	FFixture F; F.Owner();
	FCorrectionCounter Counter(F.Character);
	auto Facing = F.Reaction->GetFacingState(); Facing.bFinal = true; Facing.FinalYaw = 30.f; ++Facing.Revision; F.SetFacing(Facing);
	F.Character->GetMesh()->bPauseAnims = true;
	FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
	TestFalse(TEXT("暂停不消费最终修订"), FHodgeHitReactionFacingMovementTestAccess::Consumed(F.Movement, Facing));
	TestTrue(TEXT("暂停中无瞬转"), NearYaw(F.Character->GetActorRotation().Yaw, 0.f));
	F.Character->GetMesh()->bPauseAnims = false;
	FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
	TestTrue(TEXT("首个允许入口校正至权威朝向"), NearYaw(F.Character->GetActorRotation().Yaw, 30.f));
	TestTrue(TEXT("修订已消费"), FHodgeHitReactionFacingMovementTestAccess::Consumed(F.Movement, Facing));
	F.Movement->PerformMovement(.016f);
	TestEqual(TEXT("迟到Final首次普通移动不重复消费同一最终修订"), Counter.GetCount(), 1);
	F.Pose(10.f);
	FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
	TestTrue(TEXT("普通入口不重复消费同一修订"), NearYaw(F.Character->GetActorRotation().Yaw, 10.f));
	FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement, true);
	TestTrue(TEXT("原生回放收尾恢复已消费约束而不新建校正"), NearYaw(F.Character->GetActorRotation().Yaw, 30.f));
	TestEqual(TEXT("FrozenRestore不会排入新Final校正"), Counter.GetCount(), 1);
	Facing.bHasTarget = false; ++Facing.Segment; ++Facing.Revision; Facing.FinalYaw = 80.f; F.SetFacing(Facing);
	FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
	TestTrue(TEXT("无源最终段不写入可选转向"), NearYaw(F.Character->GetActorRotation().Yaw, 30.f));
	FHodgeHitReactionFacingMovementTestAccess::Retire(F.Reaction);
	FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement, true);
	TestFalse(TEXT("退休清除旧校正身份"), FHodgeHitReactionFacingMovementTestAccess::Consumed(F.Movement, Facing));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingLegacyRotationTest, "Hodge.HitReaction.FacingMovement.LegacyNativeRotationProtection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 三种直接legacy纠正仍由引擎确认及重放，真实Rotation不被可选恢复锚点覆盖。 */
bool FHodgeHitFacingLegacyRotationTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	for (int32 Case = 0; Case < 4; ++Case)
	{
		FFixture F; F.Owner(); F.QueueHistory(); F.Pose(20.f); F.Rotation->UpdateHitReactionLockedYaw(20.f);
		const auto Location = F.Character->GetActorLocation();
		const uint8 Mode = F.Movement->PackNetworkMovementMode();
		const FVector_NetQuantizeNormal Rotation(FVector(0.f, 80.f / 180.f, 0.f));
		if (Case <= 1)
		{
			const TOptional<FRotator> Optional = Case == 1 ? TOptional<FRotator>(FRotator(0.f, 80.f, 0.f)) : TOptional<FRotator>();
			F.Movement->ClientAdjustPosition_Implementation(.016f, Location, FVector::ZeroVector, nullptr, NAME_None,
				false, false, Mode, Optional);
		}
		else if (Case == 2)
		{
			F.Movement->ClientAdjustRootMotionPosition_Implementation(.016f, 0.f, Location, Rotation, 0.f,
				nullptr, NAME_None, false, false, Mode);
		}
		else
		{
			F.Movement->ClientAdjustRootMotionSourcePosition_Implementation(.016f, FRootMotionSourceGroup(), false,
				0.f, Location, Rotation, 0.f, nullptr, NAME_None, false, false, Mode);
		}
		TestEqual(FString::Printf(TEXT("legacy案例%d仅Montage保持原生历史末态，其他合法响应保留对应当前锚点"), Case),
			FHodgeHitReactionFacingMovementTestAccess::MayRestore(F.Movement), Case != 2);
		if (Case != 0) { TestTrue(TEXT("legacy真实权威Rotation已由Native应用"), NearYaw(F.Character->GetActorRotation().Yaw, 80.f)); }
		TestTrue(TEXT("legacy真实Ack后执行剩余SavedMove重放"), F.Movement->ClientUpdatePositionAfterServerUpdate());
		TestTrue(TEXT("legacy末态不被回包前旧锚点覆盖"), Case == 0 ? NearYaw(F.Character->GetActorRotation().Yaw, 20.f) :
			Case == 2 ? !NearYaw(F.Character->GetActorRotation().Yaw, 20.f) : NearYaw(F.Character->GetActorRotation().Yaw, 80.f));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingMixedReplayTest, "Hodge.HitReaction.FacingMovement.MixedNewActionAndGoodMoveReplay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 用等价原生Rotation夹具作对照，检查混合队列与后续GoodMove是否留下陈旧锚点。 */
bool FHodgeHitFacingMixedReplayTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	for (const bool bAckAllOldFacing : {false, true})
	{
		float Results[2] = {};
		for (int32 Reference = 0; Reference < 2; ++Reference)
		{
			FFixture F; F.Owner(); F.QueueHistory(); F.Pose(20.f); F.Rotation->UpdateHitReactionLockedYaw(20.f);
			FHodgeHitReactionFacingMovementTestAccess::Retire(F.Reaction);
			F.Character->SetRole(ROLE_Authority);
			const auto Request = F.Rotation->RequestActionFacing(FVector::RightVector, F.Character, FGuid::NewGuid(), false);
			F.Owner();
			TestTrue(TEXT("混合队列的新动作已获权威授权"), Request.IsValid());
			const auto NewActionMove = F.Record(.048f);
			F.Movement->PerformMovement(.016f);
			NewActionMove->PostUpdate(F.Character, FSavedMove_Character::PostUpdate_Record);
			F.Movement->GetPredictionData_Client_Character()->SavedMoves.Add(NewActionMove);
			// ActionDirection关闭这两种基础驱动，位置回包本身保留当前Yaw；显式相同Yaw完全等价。
			TestFalse(TEXT("对照夹具无需LastAckedMove朝向回退"), F.Movement->bOrientRotationToMovement || F.Movement->bUseControllerDesiredRotation);
			FCharacterMoveResponseDataContainer Response;
			Response.ClientAdjustment.TimeStamp = .016f;
			Response.ClientAdjustment.NewLoc = F.Character->GetActorLocation();
			Response.ClientAdjustment.MovementMode = F.Movement->PackNetworkMovementMode();
			Response.ClientAdjustment.NewRot = F.Character->GetActorRotation();
			Response.bHasRotation = Reference == 1;
			F.Movement->ClientHandleMoveResponse(Response);
			if (bAckAllOldFacing)
			{
				FCharacterMoveResponseDataContainer Good;
				Good.ClientAdjustment.TimeStamp = .032f; Good.ClientAdjustment.bAckGoodMove = true;
				F.Movement->ClientHandleMoveResponse(Good);
				TestEqual(TEXT("GoodMove已确认移除全部旧受击，仅余新动作"), F.Movement->GetPredictionData_Client_Character()->SavedMoves.Num(), 1);
			}
			TestTrue(TEXT("混合/GoodMove案例真实执行原生重放"), F.Movement->ClientUpdatePositionAfterServerUpdate());
			Results[Reference] = F.Character->GetActorRotation().Yaw;
			AddInfo(FString::Printf(TEXT("MixedReplay AckAllOldFacing=%d Reference=%d NewAction=%s FinalYaw=%.9f"),
				bAckAllOldFacing, Reference, *Request.Id.ToString(), Results[Reference]));
		}
		TestTrue(FString::Printf(TEXT("AckAllOldFacing=%d当前锚点不能覆盖新动作末步Native合法输出(实际%.6f 对照%.6f)"),
			bAckAllOldFacing, Results[0], Results[1]), NearYaw(Results[0], Results[1]));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingRootMotionIsolationTest, "Hodge.HitReaction.FacingMovement.RootMotionRetirementCorrectionAndNativeReplay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 根运动接管和后续同级Impact只保留旧Final记录，不能重新消费或覆盖Native末态。 */
bool FHodgeHitFacingRootMotionIsolationTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	{
		FFixture F; F.Owner(); FCorrectionCounter Counter(F.Character);
		auto Facing = F.Reaction->GetFacingState(); Facing.bFinal = true; Facing.FinalYaw = 30.f; ++Facing.Revision; F.SetFacing(Facing);
		FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
		TestEqual(TEXT("接管前同段Final已消费一次"), Counter.GetCount(), 1);
		Facing.bRetired = true; ++Facing.Revision; F.SetFacing(Facing);
		FHodgeHitReactionFacingMovementTestAccess::SetMotionSource(F.Reaction, EHodgeReactionMotionSource::AnimationRootMotion);
		F.Pose(80.f); F.Rotation->UpdateHitReactionLockedYaw(80.f);
		FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
		FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement, true);
		TestTrue(TEXT("RootMotion接管后旧Final和FrozenRestore均不写Yaw"), NearYaw(F.Character->GetActorRotation().Yaw, 80.f));
		TestFalse(TEXT("退役记录不再具有当前执行资格"), F.Reaction->IsFacingStateCurrent(Facing.ChainId, Facing.Segment));
		FHodgeHitReactionFacingMovementTestAccess::SetMotionSource(F.Reaction, EHodgeReactionMotionSource::Impact);
		F.Movement->PerformMovement(.016f);
		FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement, true);
		TestTrue(TEXT("同链同级恢复Impact不会把已退役旧Final重新校正"), NearYaw(F.Character->GetActorRotation().Yaw, 80.f));
		TestEqual(TEXT("记录保留期间不会再次消费旧Final修订"), Counter.GetCount(), 1);
	}
	for (const bool bRetiredImpact : {false, true})
	{
		float Results[2] = {};
		float NativeStartYaw = 0.f;
		for (int32 Reference = 0; Reference < 2; ++Reference)
		{
			FFixture F; F.Owner(); F.QueueHistory(); FCorrectionCounter Counter(F.Character);
			auto Facing = F.Reaction->GetFacingState(); Facing.bFinal = true; Facing.bRetired = bRetiredImpact;
			Facing.FinalYaw = 30.f; ++Facing.Revision; F.SetFacing(Facing);
			FHodgeHitReactionFacingMovementTestAccess::SetMotionSource(F.Reaction, bRetiredImpact ?
				EHodgeReactionMotionSource::Impact : EHodgeReactionMotionSource::AnimationRootMotion);
			F.Pose(80.f); F.Rotation->UpdateHitReactionLockedYaw(80.f);
			FCharacterMoveResponseDataContainer Response;
			Response.ClientAdjustment.TimeStamp = .016f;
			Response.ClientAdjustment.NewLoc = F.Character->GetActorLocation();
			Response.ClientAdjustment.MovementMode = F.Movement->PackNetworkMovementMode();
			Response.bHasRotation = Reference == 1;
			Response.ClientAdjustment.NewRot = FRotator(0.f, NativeStartYaw, 0.f);
			F.Movement->ClientHandleMoveResponse(Response);
			if (Reference == 0) { NativeStartYaw = F.Character->GetActorRotation().Yaw; }
			else { TestTrue(TEXT("显式原生对照与无Rotation响应的实际重放起点相同"), NearYaw(F.Character->GetActorRotation().Yaw, NativeStartYaw)); }
			TestTrue(TEXT("RootMotion/退休Impact均真实经过Native Ack及历史重放"), F.Movement->ClientUpdatePositionAfterServerUpdate());
			Results[Reference] = F.Character->GetActorRotation().Yaw;
			TestEqual(TEXT("历史重放不消费已排除的旧Final"), Counter.GetCount(), 0);
			TestFalse(TEXT("当前计划不可执行时清理历史覆盖而不新建资格"), FHodgeHitReactionFacingMovementTestAccess::Replay(F.Movement).IsSet());
		}
		TestTrue(FString::Printf(TEXT("RetiredImpact=%d旧锚点不能覆盖原生合法末态(实际%.6f 对照%.6f)"),
			bRetiredImpact, Results[0], Results[1]), NearYaw(Results[0], Results[1]));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingFirstBindingTest, "Hodge.HitReaction.FacingMovement.ReplicatedFinalBeforeFirstASCBinding",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 复制早于ASC依赖时保留合法Final；真实首绑、同ASC重复初始化及旧ASC切换分别验证。 */
bool FHodgeHitFacingFirstBindingTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	FFixture F(true); F.Owner(); FCorrectionCounter Counter(F.Character);
	auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(F.Character);
	TestNull(TEXT("延迟夹具从未建立PawnExtension ASC绑定"), Extension->GetHodgeAbilitySystemComponent());
	TestFalse(TEXT("首绑前朝向依赖尚未就绪"), F.Rotation->IsFacingSystemReady());
	auto Facing = F.Reaction->GetFacingState(); Facing.bFinal = true; Facing.FinalYaw = 30.f; ++Facing.Revision;
	FHodgeHitReactionFacingMovementTestAccess::ReceiveBeforeFirstBinding(F.Reaction, Facing);
	TestFalse(TEXT("仅收到复制数据时不提前授予Yaw执行资格"), F.Reaction->IsFacingStateCurrent(Facing.ChainId, Facing.Segment));
	FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
	TestEqual(TEXT("依赖未就绪时不消费Final"), Counter.GetCount(), 0);
	Extension->InitializeAbilitySystem(F.ASC, F.Character);
	const auto Bound = F.Reaction->GetFacingState();
	TestTrue(TEXT("真实首次绑定保留同Pawn已复制的链段"), Bound.ChainId == Facing.ChainId && Bound.Segment == Facing.Segment);
	TestEqual(TEXT("首绑不修改已复制最终修订"), Bound.Revision, Facing.Revision);
	TestTrue(TEXT("首绑后存活同段恢复执行资格"), F.Reaction->IsFacingStateCurrent(Facing.ChainId, Facing.Segment));
	F.Movement->PerformMovement(.016f);
	TestTrue(TEXT("首个真实移动消费已收到的权威Final"), NearYaw(F.Character->GetActorRotation().Yaw, 30.f));
	TestEqual(TEXT("首绑后只消费一次Final"), Counter.GetCount(), 1);
	Extension->InitializeAbilitySystem(F.ASC, F.Character);
	F.Reaction->InitializeWithAbilitySystem(F.ASC);
	F.Movement->PerformMovement(.016f);
	TestTrue(TEXT("同ASC重复初始化保留同一链段"), F.Reaction->GetFacingState().ChainId == Facing.ChainId &&
		F.Reaction->GetFacingState().Segment == Facing.Segment);
	TestEqual(TEXT("同ASC重复初始化不修改修订"), F.Reaction->GetFacingState().Revision, Facing.Revision);
	TestEqual(TEXT("同ASC重复初始化不重复消费Final"), Counter.GetCount(), 1);
	// 通过真实Extension切换撤销旧绑定，不直接改写受击组件的私有ASC指针。
	auto* Replacement = NewObject<UHodgeAbilitySystemComponent>(F.Character);
	F.Character->AddInstanceComponent(Replacement); Replacement->RegisterComponent();
	Replacement->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(F.Character));
	Replacement->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), 100.f);
	Replacement->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
	Extension->InitializeAbilitySystem(Replacement, F.Character);
	TestFalse(TEXT("真实旧ASC切换仍完整清除旧Face"), F.Reaction->GetFacingState().HasIdentity());
	TestFalse(TEXT("旧段不因新ASC就绪而重获资格"), F.Reaction->IsFacingStateCurrent(Facing.ChainId, Facing.Segment));
	F.Pose(70.f); F.Rotation->UpdateHitReactionLockedYaw(70.f);
	F.Movement->PerformMovement(.016f);
	TestTrue(TEXT("新ASC移动不恢复旧Final朝向"), NearYaw(F.Character->GetActorRotation().Yaw, 70.f));
	TestEqual(TEXT("切换ASC后旧最终修订没有再次消费"), Counter.GetCount(), 1);
	Extension->UninitializeAbilitySystem();
	TestFalse(TEXT("真实解绑仍清除全部Face"), F.Reaction->GetFacingState().HasIdentity());
	TestFalse(TEXT("真实解绑后不能执行旧补转"), F.Rotation->IsFacingSystemReady());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingSavedMoveTest, "Hodge.HitReaction.FacingMovement.HistoricalSnapshotAndCombineBoundary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** SetMoveFor保存历史时钟和暂停事实；当前暂停或新段不能污染旧移动输入。 */
bool FHodgeHitFacingSavedMoveTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	FFixture F; F.Owner();
	const auto Move = F.Record(.016f);
	const auto Next = F.Record(.032f);
	TestFalse(TEXT("活动补转移动禁止合并"), Move->CanCombineWith(Next, F.Character, .125f));
	const auto Original = F.Reaction->GetFacingState();
	auto NewSegment = Original; ++NewSegment.Segment; NewSegment.TargetYaw = -90.f; F.SetFacing(NewSegment);
	F.Character->GetMesh()->bPauseAnims = true;
	F.Character->bClientUpdating = true;
	Move->PrepMoveFor(F.Character);
	const auto Replay = FHodgeHitReactionFacingMovementTestAccess::Replay(F.Movement);
	if (TestTrue(TEXT("原生Prep安装历史快照"), Replay.IsSet()))
	{
		TestEqual(TEXT("仍为历史段"), Replay->Facing.Segment, Original.Segment);
		TestEqual(TEXT("仍为历史目标"), Replay->Facing.TargetYaw, Original.TargetYaw);
		TestFalse(TEXT("当前暂停不改写历史暂停事实"), Replay->bPaused);
		TestEqual(TEXT("历史求值时刻保持录制时世界时间"), Replay->EvaluatedAt, 0.f);
	}
	F.Character->bClientUpdating = false;
	F.Rotation->ClearMoveReplayState(); F.Movement->ResetHitReactionFacing();
	F.Character->GetMesh()->bPauseAnims = false;
	NewSegment.bFinal = true; NewSegment.FinalYaw = 30.f; ++NewSegment.Revision; F.SetFacing(NewSegment);
	const auto Pending = F.Record(.048f);
	TestFalse(TEXT("最终修订待校正时禁止合并"), Pending->CanCombineWith(F.Record(.064f), F.Character, .125f));
	FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
	// 在实际静止移动后执行原生PostUpdate，排除未完成移动自身的禁止合并条件。
	const auto Stable = F.Record(.08f);
	F.Movement->PerformMovement(.016f);
	Stable->PostUpdate(F.Character, FSavedMove_Character::PostUpdate_Record);
	const auto StableNext = F.Record(.096f);
	F.Movement->PerformMovement(.016f);
	StableNext->PostUpdate(F.Character, FSavedMove_Character::PostUpdate_Record);
	TestTrue(TEXT("已消费的稳定Final沿用原生可合并条件"), Stable->CanCombineWith(StableNext, F.Character, .125f));
	F.Character->GetMesh()->bPauseAnims = true;
	const auto Paused = F.Record(.112f);
	TestFalse(TEXT("稳定Final跨暂停边界仍禁止合并"), StableNext->CanCombineWith(Paused, F.Character, .125f));
	F.Character->GetMesh()->bPauseAnims = false;
	FHodgeHitReactionFacingMovementTestAccess::Retire(F.Reaction);
	TestFalse(TEXT("结束受击身份边界禁止合并"), StableNext->CanCombineWith(F.Record(.128f), F.Character, .125f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingNativeReplayTest, "Hodge.HitReaction.FacingMovement.NativeCorrectionReplayCurrentIdentity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 真正通过Native纠正、Ack和SavedMove循环验证当前Final、非Final、新段及新动作的可见末态。 */
bool FHodgeHitFacingNativeReplayTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	for (int32 Case = 0; Case < 4; ++Case)
	{
		FFixture F; F.Owner(); F.QueueHistory();
		auto Facing = F.Reaction->GetFacingState();
		F.Pose(20.f); F.Rotation->UpdateHitReactionLockedYaw(20.f);
		float Expected = 20.f;
		if (Case == 0) { Facing.bFinal = true; Facing.FinalYaw = 30.f; ++Facing.Revision; Expected = 30.f; }
		if (Case == 2) { ++Facing.Segment; Facing.TargetYaw = -90.f; }
		if (Case == 3)
		{
			FHodgeHitReactionFacingMovementTestAccess::Retire(F.Reaction);
			// 在权威侧建立已授权的新动作，再以拥有者角色验证旧移动不能覆盖它。
			F.Character->SetRole(ROLE_Authority);
			const auto Request = F.Rotation->RequestActionFacing(FVector::RightVector, F.Character, FGuid::NewGuid(), true);
			F.Owner();
			TestTrue(TEXT("新动作成功申请实际朝向"), Request.IsValid());
			Expected = F.Character->GetActorRotation().Yaw;
		}
		else { F.SetFacing(Facing); }
		TestTrue(FString::Printf(TEXT("原生纠正实际重放案例%d"), Case), F.CorrectAndReplay());
		TestTrue(FString::Printf(TEXT("案例%d收尾使用当前占用，无旧段Yaw遗留"), Case), NearYaw(F.Character->GetActorRotation().Yaw, Expected));
		TestFalse(TEXT("回放覆盖已清理"), FHodgeHitReactionFacingMovementTestAccess::Replay(F.Movement).IsSet());
		TestFalse(TEXT("原生回放标记已退出"), F.Character->bClientUpdating);
		if (Case == 0) { TestTrue(TEXT("当前最终修订已消费"), FHodgeHitReactionFacingMovementTestAccess::Consumed(F.Movement, Facing)); }
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingNativeActiveReplayTest, "Hodge.HitReaction.FacingMovement.NativeRotationActiveMultiMoveZeroStep",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 三个真实历史步复现到达目标后零补角回填旧锁，显式原生回包必须成为当前锚点。 */
bool FHodgeHitFacingNativeActiveReplayTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	for (const bool bRMS : {true, false})
	{
		FFixture F; F.Owner(); FCorrectionCounter Counter(F.Character);
		auto Facing = F.Reaction->GetFacingState();
		Facing.StartedAt = 0.f; Facing.EndsAt = .12f; Facing.TargetYaw = 60.f; F.SetFacing(Facing);
		auto Force = MakeShared<FHodgeRootMotionSource_Knockback>();
		Force->InstanceName = TEXT("NativeActiveReplay"); Force->Duration = .3f; Force->Priority = 500;
		Force->AccumulateMode = ERootMotionAccumulateMode::Override; Force->Force = FVector(80.f, 0.f, 0.f);
		F.Movement->ApplyRootMotionSource(Force);
		auto* Prediction = F.Movement->GetPredictionData_Client_Character();
		const auto Ack = F.Record(.016f);
		F.Movement->PerformMovement(.016f); Ack->PostUpdate(F.Character, FSavedMove_Character::PostUpdate_Record);
		Prediction->SavedMoves.Add(Ack);
		float MoveTime = .016f;
		// 仅推进独立测试世界时钟，历史快照仍由真实SetMoveFor和CMC录制。
		for (const FVector Sample : {FVector(.002550125, .023221016, 0.), FVector(.029655456, .027105331, 0.),
			FVector(.058664798, .029009342, 0.)})
		{
			F.World->TimeSeconds = Sample.X; MoveTime += Sample.Y;
			const auto Move = F.Record(MoveTime, Sample.Y);
			F.Movement->PerformMovement(Sample.Y); Move->PostUpdate(F.Character, FSavedMove_Character::PostUpdate_Record);
			Prediction->SavedMoves.Add(Move);
		}
		const float VisibleBefore = F.Character->GetActorRotation().Yaw;
		TestTrue(TEXT("真实录制已从0度按有限步长推进到约42度"), FMath::Abs(VisibleBefore - 42.238312f) < .01f);
		TestTrue(TEXT("Ack帧包含真实击退RMS"), Ack->SavedRootMotion.HasActiveRootMotionSources());
		const float AuthorityYaw = 54.120304f;
		if (bRMS)
		{
			F.Movement->ClientAdjustRootMotionSourcePosition_Implementation(.016f, Ack->SavedRootMotion, false, 0.f,
				F.Character->GetActorLocation(), FVector(0.f, AuthorityYaw / 180.f, 0.f), 0.f,
				nullptr, NAME_None, false, false, F.Movement->PackNetworkMovementMode());
		}
		else
		{
			FCharacterMoveResponseDataContainer Response;
			Response.ClientAdjustment.TimeStamp = .016f; Response.ClientAdjustment.NewLoc = F.Character->GetActorLocation();
			Response.ClientAdjustment.MovementMode = F.Movement->PackNetworkMovementMode();
			Response.ClientAdjustment.NewRot = FRotator(0.f, AuthorityYaw, 0.f); Response.bHasRotation = true;
			F.Movement->ClientHandleMoveResponse(Response);
		}
		const float AppliedNativeYaw = F.Character->GetActorRotation().Yaw;
		TestTrue(TEXT("真实原生ACK已移除首帧并保留三个未确认移动"), Prediction->LastAckedMove == Ack && Prediction->SavedMoves.Num() == 3);
		TestTrue(TEXT("原生响应已实际应用54度权威朝向"), NearYaw(AppliedNativeYaw, AuthorityYaw));
		TestTrue(TEXT("实际执行三个SavedMove的原生重放循环"), F.Movement->ClientUpdatePositionAfterServerUpdate());
		const float AfterReplay = F.Character->GetActorRotation().Yaw;
		AddInfo(FString::Printf(TEXT("NativeActiveReplay RMS=%d Before=%.9f AppliedNative=%.9f AfterReplay=%.9f LockedYaw=%.9f"),
			bRMS, VisibleBefore, AppliedNativeYaw, AfterReplay, F.Rotation->GetLockedYaw()));
		TestTrue(TEXT("非Final回放收尾保留本次原生权威锚点，不回旧历史锁"), NearYaw(AfterReplay, AppliedNativeYaw));
		TestTrue(TEXT("当前锁参考同步本次实际原生Yaw"), NearYaw(F.Rotation->GetLockedYaw(), AppliedNativeYaw));
		F.World->TimeSeconds = .083883762;
		F.Movement->PerformMovement(.025218964f);
		const float VisibleAfter = F.Character->GetActorRotation().Yaw;
		TestTrue(TEXT("后续正常移动承接原生锚点且不倒退或越过目标"), VisibleAfter >= VisibleBefore && VisibleAfter <= 60.01f &&
			FMath::Abs(FMath::FindDeltaAngleDegrees(AppliedNativeYaw, VisibleAfter)) <= 720.f * .025218964f + .05f);
		TestEqual(TEXT("活跃非Final回包不冒充一次Final校正"), Counter.GetCount(), 0);
		TestEqual(TEXT("新ACK只发布一次实际原生朝向事件"), Counter.GetNativeCount(), 1);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingNativeFinalPauseTest, "Hodge.HitReaction.FacingMovement.NativeFinalWithoutHistoryAndPause",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 原生新ACK后没有历史移动时，暂停保留待恢复；恢复只能FrozenRestore，不能重新消费Final。 */
bool FHodgeHitFacingNativeFinalPauseTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	for (const bool bRMS : {false, true})
	{
		for (const bool bPaused : {false, true})
		{
			FFixture F; F.Owner(); FCorrectionCounter Counter(F.Character);
			auto Facing = F.Reaction->GetFacingState(); Facing.bFinal = true; Facing.FinalYaw = 30.f; ++Facing.Revision; F.SetFacing(Facing);
			FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
			F.Movement->GetPredictionData_Client_Character()->SavedMoves.Add(F.Record(.016f));
			F.Character->GetMesh()->bPauseAnims = bPaused;
			F.NativeCorrection(bRMS, .016f, 54.f);
			TestEqual(TEXT("原生新ACK真实留证，暂停不伪装成拒绝响应"), Counter.GetNativeCount(), 1);
			TestTrue(TEXT("已消费Final的待恢复边界仍禁止历史移动合并"),
				FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, .016f, 0.f).bCorrectionPending);
			TestTrue(TEXT("原生响应保留实际54度并确认最后一个移动"), NearYaw(F.Character->GetActorRotation().Yaw, 54.f) &&
				F.Movement->GetPredictionData_Client_Character()->SavedMoves.IsEmpty());
			TestFalse(TEXT("无未确认移动时原生入口不伪造重放成功"), F.Movement->ClientUpdatePositionAfterServerUpdate());
			TestFalse(TEXT("无历史入口消费后不遗留下一次回放锚点"), FHodgeHitReactionFacingMovementTestAccess::MayRestore(F.Movement));
			if (bPaused)
			{
				TestTrue(TEXT("暂停保持原生姿态，不提前消费可选Final恢复"), NearYaw(F.Character->GetActorRotation().Yaw, 54.f));
				TestEqual(TEXT("暂停期间不记录FrozenRestore"), Counter.GetFrozenRestoreCount(), 0);
				F.Character->GetMesh()->bPauseAnims = false;
				F.Movement->PerformMovement(.016f);
			}
			TestTrue(TEXT("首个允许入口恢复同段已消费的30度Final"), NearYaw(F.Character->GetActorRotation().Yaw, 30.f));
			TestEqual(TEXT("原生响应不重排Final消费"), Counter.GetCount(), 1);
			TestEqual(TEXT("待恢复只执行一次FrozenRestore"), Counter.GetFrozenRestoreCount(), 1);
			F.Movement->PerformMovement(.016f);
			TestEqual(TEXT("后续普通移动不重复恢复已清理待办"), Counter.GetFrozenRestoreCount(), 1);
			TestFalse(TEXT("已恢复边界不遗留待校正标记"),
				FHodgeHitReactionFacingMovementTestAccess::Snapshot(F.Movement, .032f, .016f).bCorrectionPending);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingReplayZeroStepTest, "Hodge.HitReaction.FacingMovement.ReplayZeroStepSynchronizesActualLockedYaw",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 直接核对每个真实历史移动的Super末态，不能靠之后锚点恢复掩盖零步长回填。 */
bool FHodgeHitFacingReplayZeroStepTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	for (const bool bAtTarget : {false, true})
	{
		FFixture F; F.Owner();
		auto Facing = F.Reaction->GetFacingState(); Facing.TargetYaw = 60.f;
		Facing.StartedAt = bAtTarget ? -.02f : .04f; Facing.EndsAt = .12f; F.SetFacing(Facing);
		F.Pose(21.f); F.Rotation->UpdateHitReactionLockedYaw(21.f);
		const auto Move = F.Record(.016f);
		const float ActualBefore = bAtTarget ? 60.f : 54.f;
		F.Pose(ActualBefore); F.Rotation->UpdateHitReactionLockedYaw(ActualBefore);
		F.Character->bClientUpdating = true; Move->PrepMoveFor(F.Character);
		F.Movement->PerformMovement(.016f); Move->PostUpdate(F.Character, FSavedMove_Character::PostUpdate_Replay);
		const float ActualAfterSuper = F.Character->GetActorRotation().Yaw;
		AddInfo(FString::Printf(TEXT("ReplayZeroStep AtTarget=%d Before=%.9f AfterSuper=%.9f LockedYaw=%.9f"),
			bAtTarget, ActualBefore, ActualAfterSuper, F.Rotation->GetLockedYaw()));
		TestTrue(TEXT("Super真实末态不回填历史录制前的21度"), NearYaw(ActualAfterSuper, ActualBefore));
		TestTrue(TEXT("零步进时历史锁参考已同步当前实际身体姿态"), NearYaw(F.Rotation->GetLockedYaw(), ActualBefore));
		F.Character->bClientUpdating = false; F.Rotation->ClearMoveReplayState(); F.Movement->ResetHitReactionFacing();
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingNativeAckTest, "Hodge.HitReaction.FacingMovement.NativeAckIdentityRejectAndLatestAnchor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 新ACK对象而非相等时间戳决定授权，同帧多响应只保留最新实际锚点。 */
bool FHodgeHitFacingNativeAckTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	for (const bool bRMS : {false, true})
	{
		FFixture F; F.Owner(); F.QueueHistory(); FCorrectionCounter Counter(F.Character);
		F.NativeCorrection(bRMS, .016f, 20.f);
		TestEqual(TEXT("首个真实新ACK发布一次原生事件"), Counter.GetNativeCount(), 1);
		F.NativeCorrection(bRMS, .016f, 80.f);
		F.NativeCorrection(bRMS, .008f, 80.f);
		TestEqual(TEXT("重复与过期时间戳不能因LastAcked相等而取得新授权"), Counter.GetNativeCount(), 1);
		TestTrue(TEXT("拒绝包不更新当前锁参考或擦除合法待回放锚点"), NearYaw(F.Rotation->GetLockedYaw(), 20.f) &&
			FHodgeHitReactionFacingMovementTestAccess::MayRestore(F.Movement));
		F.NativeCorrection(bRMS, .032f, 80.f, true);
		TestEqual(TEXT("未解析相对基座拒绝新候选ACK，仍不发布事件"), Counter.GetNativeCount(), 1);
		TestTrue(TEXT("候选存在但Super拒绝时仍保留前一合法锚点"), FHodgeHitReactionFacingMovementTestAccess::MayRestore(F.Movement));
		F.NativeCorrection(bRMS, .032f, 40.f);
		TestEqual(TEXT("同帧第二个实际新ACK独立留证"), Counter.GetNativeCount(), 2);
		TestFalse(TEXT("全部移动已确认，无历史重放"), F.Movement->ClientUpdatePositionAfterServerUpdate());
		TestTrue(TEXT("无历史入口保留最新40度，不回旧20度"), NearYaw(F.Character->GetActorRotation().Yaw, 40.f) &&
			NearYaw(F.Rotation->GetLockedYaw(), 40.f));
		TestFalse(TEXT("锚点消费后不遗留到下一次普通步"), FHodgeHitReactionFacingMovementTestAccess::MayRestore(F.Movement));
		TestEqual(TEXT("全程不产生Final校正"), Counter.GetCount(), 0);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingNativeExclusionTest, "Hodge.HitReaction.FacingMovement.NativeCorrectionExcludesRetiredRootMotionAndAvatar",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 原生应用行为保持不变，退休、根运动、新动作及Avatar失效不取得可选受击锚点授权。 */
bool FHodgeHitFacingNativeExclusionTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	for (int32 Case = 0; Case < 5; ++Case)
	{
		FFixture F; F.Owner(); F.QueueHistory(); FCorrectionCounter Counter(F.Character);
		if (Case == 0) { UHodgePawnExtensionComponent::FindPawnExtensionComponent(F.Character)->UninitializeAbilitySystem(); }
		else if (Case == 1)
		{
			auto Facing = F.Reaction->GetFacingState(); Facing.bRetired = true; F.SetFacing(Facing);
		}
		else if (Case == 2)
		{ FHodgeHitReactionFacingMovementTestAccess::SetMotionSource(F.Reaction, EHodgeReactionMotionSource::AnimationRootMotion); }
		else if (Case == 3)
		{
			FHodgeHitReactionFacingMovementTestAccess::Retire(F.Reaction); F.Character->SetRole(ROLE_Authority);
			TestTrue(TEXT("新动作真实申请成功"), F.Rotation->RequestActionFacing(FVector::RightVector, F.Character, FGuid::NewGuid(), false).IsValid());
			F.Owner();
		}
		F.NativeCorrection(true, .016f, 54.f, false, Case == 4);
		TestTrue(TEXT("排除范围不阻止原生ACK与实际Rotation应用"), NearYaw(F.Character->GetActorRotation().Yaw, 54.f));
		TestEqual(TEXT("排除范围不生成NativeCorrection授权事件"), Counter.GetNativeCount(), 0);
		TestFalse(TEXT("排除范围不安装可选原生Yaw恢复锚点"), FHodgeHitReactionFacingMovementTestAccess::MayRestore(F.Movement));
		F.NativeCorrection(true, .016f, 80.f, false, Case == 4);
		TestTrue(TEXT("排除范围的重复RMS仍保留原生机制，不扩展局部恢复"), NearYaw(F.Character->GetActorRotation().Yaw, 80.f));
		TestEqual(TEXT("排除范围不产生受击拒绝恢复事件"), Counter.GetRejectedRestoreCount(), 0);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingNativeTailDuplicateTest, "Hodge.HitReaction.FacingMovement.NativeTailDuplicateAfterNoHistoryCannotTurnBody",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 合法锚点已消费后，下一帧尾重复RMS也不能写身体Yaw；无后续合法ACK掩盖故障。 */
bool FHodgeHitFacingNativeTailDuplicateTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	for (const bool bFinalConsumed : {false, true})
	{
		FFixture F; F.Owner(); FCorrectionCounter Counter(F.Character);
		if (bFinalConsumed)
		{
			auto Facing = F.Reaction->GetFacingState(); Facing.bFinal = true; Facing.FinalYaw = 30.f; ++Facing.Revision; F.SetFacing(Facing);
			FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
		}
		F.Movement->GetPredictionData_Client_Character()->SavedMoves.Add(F.Record(.016f));
		F.NativeCorrection(true, .016f, 20.f);
		TestFalse(TEXT("先真实消费无历史原生入口"), F.Movement->ClientUpdatePositionAfterServerUpdate());
		const float BeforeDuplicate = F.Character->GetActorRotation().Yaw;
		TestTrue(TEXT("基线为合法20度锚点或已消费30度Final"), NearYaw(BeforeDuplicate, bFinalConsumed ? 30.f : 20.f));
		TestFalse(TEXT("尾重复到来前已经没有可用的待重放锚点"), FHodgeHitReactionFacingMovementTestAccess::MayRestore(F.Movement));
		F.World->TimeSeconds = .016f;
		F.NativeCorrection(true, .016f, 80.f);
		const float AfterDuplicate = F.Character->GetActorRotation().Yaw;
		AddInfo(FString::Printf(TEXT("NativeTailDuplicate Final=%d Before=%.9f After=%.9f LockedYaw=%.9f"),
			bFinalConsumed, BeforeDuplicate, AfterDuplicate, F.Rotation->GetLockedYaw()));
		TestTrue(TEXT("同LastAck时间戳的尾重复包不能暴露80度未授权跳转"), NearYaw(AfterDuplicate, BeforeDuplicate));
		TestTrue(TEXT("拒绝恢复同步当前锁但不重建待回放锚点"), NearYaw(F.Rotation->GetLockedYaw(), BeforeDuplicate) &&
			!FHodgeHitReactionFacingMovementTestAccess::MayRestore(F.Movement));
		TestEqual(TEXT("没有后续新ACK，NativeCorrection始终只有最初一次"), Counter.GetNativeCount(), 1);
		TestEqual(TEXT("身体的未授权写入单独记录一次净零拒绝恢复"), Counter.GetRejectedRestoreCount(), 1);
		F.Movement->PerformMovement(.016f);
		const float AfterMove = F.Character->GetActorRotation().Yaw;
		TestTrue(TEXT("下一普通步从合法姿态按原速率前进，不从80度重新起算"), AfterMove >= BeforeDuplicate &&
			FMath::Abs(FMath::FindDeltaAngleDegrees(BeforeDuplicate, AfterMove)) <= 720.f * .016f + .05f);
		TestEqual(TEXT("拒绝包不新建或重排Final消费"), Counter.GetCount(), bFinalConsumed ? 1 : 0);
		TestEqual(TEXT("拒绝包不制造新的已消费Final待恢复"), Counter.GetFrozenRestoreCount(), bFinalConsumed ? 1 : 0);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingNativeRetirementTest, "Hodge.HitReaction.FacingMovement.NativePendingRetiresOnNewSegmentAndAvatar",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 已接受的旧段原生锚点与Final待恢复也必须随退休或Avatar换代失效。 */
bool FHodgeHitFacingNativeRetirementTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	for (const bool bNewAvatarGeneration : {false, true})
	{
		FFixture F; F.Owner(); F.QueueHistory(); FCorrectionCounter Counter(F.Character);
		auto Facing = F.Reaction->GetFacingState(); Facing.bFinal = true; Facing.FinalYaw = 30.f; ++Facing.Revision; F.SetFacing(Facing);
		FHodgeHitReactionFacingMovementTestAccess::Correct(F.Movement);
		F.NativeCorrection(true, .016f, 54.f);
		TestEqual(TEXT("旧段确已接受一次原生Yaw"), Counter.GetNativeCount(), 1);
		if (bNewAvatarGeneration)
		{
			auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(F.Character);
			const int32 OldGeneration = F.Rotation->GetAvatarGeneration();
			Extension->UninitializeAbilitySystem(); Extension->InitializeAbilitySystem(F.ASC, F.Character);
			TestTrue(TEXT("真实解绑重绑已提升Avatar代次"), F.Rotation->GetAvatarGeneration() > OldGeneration);
		}
		else { FHodgeHitReactionFacingMovementTestAccess::Retire(F.Reaction); }
		TestFalse(TEXT("退休或换代立即清理旧原生锚点"), FHodgeHitReactionFacingMovementTestAccess::MayRestore(F.Movement));
		Facing.ChainId = FGuid::NewGuid(); Facing.Segment = Facing.Revision = 1; Facing.FinalYaw = -30.f; F.SetFacing(Facing);
		TestTrue(TEXT("仍执行原生未确认历史循环"), F.Movement->ClientUpdatePositionAfterServerUpdate());
		TestTrue(TEXT("旧54度锚点或30度Final不能覆盖新段负30度"), NearYaw(F.Character->GetActorRotation().Yaw, -30.f));
		TestEqual(TEXT("新旧Final各消费一次，不重新消费旧修订"), Counter.GetCount(), 2);
		TestEqual(TEXT("旧原生待办不触发旧Final冻结恢复"), Counter.GetFrozenRestoreCount(), 0);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingNewPawnReplayTest, "Hodge.HitReaction.FacingMovement.OldHitSavedMoveCannotRestoreOnReadyNewPawn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 同世界真实新Pawn已绑定后仍隔离旧Hit历史；普通瞬态ASC不替代PlayerState联机验证。 */
bool FHodgeHitFacingNewPawnReplayTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	FFixture F; F.Owner();
	auto OldFacing = F.Reaction->GetFacingState(); OldFacing.bFinal = true; OldFacing.FinalYaw = 90.f; ++OldFacing.Revision;
	F.SetFacing(OldFacing);
	const auto OldMove = F.Record(.016f);
	F.Character->bClientUpdating = true;
	OldMove->PrepMoveFor(F.Character);
	const auto Recorded = FHodgeHitReactionFacingMovementTestAccess::Replay(F.Movement);
	TestTrue(TEXT("旧真实SavedMove确实携带受击身份和控制"), Recorded.IsSet() && Recorded->bCurrent &&
		Recorded->Facing.ChainId == OldFacing.ChainId && FHodgeHitReactionFacingMovementTestAccess::Controlled(F.Movement));
	const int32 OldGeneration = F.Rotation->GetAvatarGeneration();
	F.Character->bClientUpdating = false;
	UHodgePawnExtensionComponent::FindPawnExtensionComponent(F.Character)->UninitializeAbilitySystem();
	// 保留旧Actor和移动对象，在同世界通过真实Possess及Extension创建独立新Avatar。
	auto* NewPawn = F.World->SpawnActor<AHodgeCombatCharacter>();
	F.Controller->Possess(NewPawn);
	auto* NewASC = NewObject<UHodgeAbilitySystemComponent>(NewPawn);
	NewPawn->AddInstanceComponent(NewASC); NewASC->RegisterComponent();
	NewASC->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(NewPawn));
	NewASC->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), 100.f);
	NewASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
	auto* NewReaction = NewPawn->GetHitReactionComponent();
	NewReaction->OverrideProfile = NewObject<UHodgeHitReactionProfile>(NewPawn);
	auto* Data = NewObject<UHodgePawnData>(NewPawn); Data->HitReactionProfile = NewReaction->OverrideProfile;
	auto* NewExtension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(NewPawn);
	NewExtension->SetPawnData(Data); NewExtension->InitializeAbilitySystem(NewASC, NewPawn);
	auto* NewRotation = NewPawn->GetCharacterRotationComponent();
	auto* NewMovement = CastChecked<UHodgeCharacterMovementComponent>(NewPawn->GetCharacterMovement());
	NewMovement->SetUpdatedComponent(NewPawn->GetCapsuleComponent()); NewMovement->Activate();
	NewMovement->SetMovementMode(MOVE_Flying); NewMovement->GravityScale = 0.f;
	NewPawn->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewPawn->SetRole(ROLE_AutonomousProxy);
	TestTrue(TEXT("新Pawn确为同世界不同Actor"), NewPawn != F.Character && NewPawn->GetWorld() == F.World);
	TestTrue(TEXT("新Pawn通过真实Extension完成ASC依赖"), NewExtension->GetHodgeAbilitySystemComponent() == NewASC && NewRotation->IsFacingSystemReady());
	TestEqual(TEXT("相同首次代次仍必须由Avatar指针隔离旧历史"), NewRotation->GetAvatarGeneration(), OldGeneration);
	FCorrectionCounter Counter(NewPawn);
	NewPawn->SetActorRotation(FRotator(0.f, 40.f, 0.f), ETeleportType::TeleportPhysics);
	NewPawn->bClientUpdating = true;
	OldMove->PrepMoveFor(NewPawn);
	TestFalse(TEXT("已Ready新CMC不安装旧Hit快照"), FHodgeHitReactionFacingMovementTestAccess::Replay(NewMovement).IsSet());
	TestFalse(TEXT("新Pawn无受击时不恢复旧历史控制"), FHodgeHitReactionFacingMovementTestAccess::Controlled(NewMovement));
	TestFalse(TEXT("新Pawn无受击时不恢复旧历史锁"), NewRotation->IsYawLocked());
	NewMovement->PerformMovement(.016f);
	NewPawn->bClientUpdating = false;
	TestTrue(TEXT("真实移动不转回旧Pawn的Yaw"), NearYaw(NewPawn->GetActorRotation().Yaw, 40.f));
	TestFalse(TEXT("旧历史不能在新Pawn建立受击身份"), NewReaction->GetFacingState().HasIdentity());
	TestEqual(TEXT("无受击新Pawn不消费旧Final"), Counter.GetCount(), 0);
	// 新Pawn自有Final仍可执行；拒绝旧Prep不能顺带撤销合法的新段或其一次性记录。
	auto NewFacing = OldFacing; NewFacing.ChainId = FGuid::NewGuid(); NewFacing.Segment = NewFacing.Revision = 1;
	NewFacing.TargetYaw = NewFacing.FinalYaw = -30.f;
	FHodgeHitReactionFacingMovementTestAccess::Install(NewReaction, NewFacing);
	NewPawn->bClientUpdating = true; OldMove->PrepMoveFor(NewPawn);
	NewMovement->PerformMovement(.016f); NewPawn->bClientUpdating = false;
	TestFalse(TEXT("新段期间也不安装旧Hit快照"), FHodgeHitReactionFacingMovementTestAccess::Replay(NewMovement).IsSet());
	TestEqual(TEXT("历史移动中不提前消费新Final"), Counter.GetCount(), 0);
	NewMovement->PerformMovement(.016f);
	TestTrue(TEXT("新Pawn只校正到自己的独立Final"), NearYaw(NewPawn->GetActorRotation().Yaw, -30.f));
	TestTrue(TEXT("新Final已由真实移动消费"), FHodgeHitReactionFacingMovementTestAccess::Consumed(NewMovement, NewFacing));
	TestFalse(TEXT("旧Final从未取得新CMC消费身份"), FHodgeHitReactionFacingMovementTestAccess::Consumed(NewMovement, OldFacing));
	NewPawn->bClientUpdating = true; OldMove->PrepMoveFor(NewPawn);
	NewMovement->PerformMovement(.016f); NewPawn->bClientUpdating = false;
	NewMovement->PerformMovement(.016f);
	TestEqual(TEXT("重复旧Prep及新移动仍只消费新Final一次"), Counter.GetCount(), 1);
	TestTrue(TEXT("重复旧历史不留下旧Yaw"), NearYaw(NewPawn->GetActorRotation().Yaw, -30.f));
	TestTrue(TEXT("旧历史不替换新链和修订"), NewReaction->GetFacingState().ChainId == NewFacing.ChainId &&
		NewReaction->GetFacingState().Revision == NewFacing.Revision);
	NewPawn->SetRole(ROLE_Authority); NewExtension->UninitializeAbilitySystem();
	TestFalse(TEXT("额外新Pawn明确配对解绑"), NewRotation->IsFacingSystemReady());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitFacingReplayEpochTest, "Hodge.HitReaction.FacingMovement.ReplayUnboundAndNativeAuthorityRotation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 解绑使旧自定义回放失效；真正携带Rotation的原生回包不能被当前驱动锚点覆盖。 */
bool FHodgeHitFacingReplayEpochTest::RunTest(const FString& Parameters)
{
	using namespace HodgeHitReactionFacingMovementTests;
	{
		FFixture F; F.Owner();
		const auto OldMove = F.Record(.016f);
		UHodgePawnExtensionComponent::FindPawnExtensionComponent(F.Character)->UninitializeAbilitySystem();
		F.Pose(40.f);
		F.Character->bClientUpdating = true;
		OldMove->PrepMoveFor(F.Character);
		F.Movement->PerformMovement(.016f);
		F.Character->bClientUpdating = false;
		TestFalse(TEXT("解绑后Prep不能重建旧受击覆盖"), FHodgeHitReactionFacingMovementTestAccess::Replay(F.Movement).IsSet());
		TestFalse(TEXT("解绑后不能重建旧冻结约束"), F.Rotation->IsYawLocked());
		TestTrue(TEXT("解绑后无旧补转Yaw遗留"), NearYaw(F.Character->GetActorRotation().Yaw, 40.f));
	}
	{
		FFixture F; F.Owner(); F.QueueHistory(); F.Pose(20.f); F.Rotation->UpdateHitReactionLockedYaw(20.f);
		FCharacterMoveResponseDataContainer Response;
		Response.ClientAdjustment.TimeStamp = .016f;
		Response.ClientAdjustment.NewLoc = F.Character->GetActorLocation();
		Response.ClientAdjustment.MovementMode = F.Movement->PackNetworkMovementMode();
		Response.ClientAdjustment.NewRot = FRotator(0.f, 80.f, 0.f);
		Response.bHasRotation = true;
		F.Movement->ClientHandleMoveResponse(Response);
		TestTrue(TEXT("真实原生Rotation只允许纠正后的当前锚点恢复"), FHodgeHitReactionFacingMovementTestAccess::MayRestore(F.Movement));
		TestTrue(TEXT("原生真实权威Rotation已应用"), NearYaw(F.Character->GetActorRotation().Yaw, 80.f));
		TestTrue(TEXT("仍执行原生历史重放"), F.Movement->ClientUpdatePositionAfterServerUpdate());
		TestTrue(TEXT("保留实际80度原生权威锚点，不回包前20度"), NearYaw(F.Character->GetActorRotation().Yaw, 80.f));
	}
	return true;
}

#endif
