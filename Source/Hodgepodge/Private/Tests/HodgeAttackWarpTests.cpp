/** @file HodgeAttackWarpTests.cpp @brief AW-01～07 配置、许可、根运动执行和协议边界测试。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "HodgeAttackWarpTestTypes.h"
#include "HodgeEnemyAITestSupport.h"
#include "Combat/HodgeAttackFacingTypes.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeAttackMotionWarpingComponent.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgeLockOnComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Component/HodgeHealthComponent.h"
#include "Character/HodgeCombatCharacter.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeLockOnProfile.h"
#include "Data/HodgePawnData.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Animation/HodgeRootMotionModifier_AttackFacing.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "AnimNotifyState_MotionWarping.h"
#include "GameFramework/PlayerController.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/UnrealType.h"
#include <limits>

/** 测试活动身份由 GAS 建立，只补充本次执行的本地 GUID。 */
struct FHodgeAttackWarpTestAccess
{
	static bool AuthorityCurrent(UHodgeCharacterRotationComponent* Rotation, const FHodgeCharacterRotationState& State)
	{ Rotation->ReplicatedState=State; return Rotation->IsAuthorityActionCurrent(); }
	static void Release(UHodgeGameplayAbility_Definition* Ability, FHodgeFacingRequestHandle Handle)
	{ Ability->AttackFacingHandle=Handle; Ability->ReleaseAttackFacing(); }
	/** 使用实际OnRep入口验证已退役执行不会再接回服务器旧租约。 */
	static void ReplayAuthority(UHodgeCharacterRotationComponent* Rotation, const FHodgeCharacterRotationState& State)
	{ Rotation->ReplicatedState=State; Rotation->OnRep_RotationState(); }
	static bool Apply(UHodgeGameplayAbility_Definition* Ability, const FHodgeAttackFacingTargetData& Data) { return Ability->ApplyAttackFacing(Data); }
	static FGuid SetIdentity(UHodgeGameplayAbility_Definition* Ability)
	{ Ability->ExecutionId=FGuid::NewGuid(); return Ability->ExecutionId; }
	/** 夹具只建立初始化和激活快照，追踪与有效性判断仍调用生产入口。 */
	static void InitializeLock(UHodgeLockOnComponent* Lock)
	{
		Lock->Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(Lock->GetOwner());
		Lock->CheckDefaultInitialization();
	}
	static void Confirm(UHodgeLockOnComponent* Lock, AActor* Target, FName Point = NAME_None)
	{
		Lock->ConfirmedState.Target = Target;
		Lock->ConfirmedState.bHasTarget = Target != nullptr;
		Lock->ConfirmedState.LockPointId = Point;
		++Lock->ConfirmedState.StateVersion;
		Lock->OnLockStateChanged.Broadcast(Lock->ConfirmedState, Lock->GetPresentationTarget(), Lock->GetPresentationRevision());
	}
	static void Predict(UHodgeLockOnComponent* Lock, AActor* Target, FName Point, bool bQueued)
	{
		auto& Intent = bQueued ? Lock->Queued : Lock->Pending;
		Intent.CandidateActor = Target;
		Intent.LockPointId = Point;
		if (bQueued) { Lock->bHasQueued = true; } else { Lock->bHasPending = true; }
	}
	static void SetExecutionTarget(UHodgeGameplayAbility_Definition* Ability, AActor* Target)
	{
		Ability->ExecutionLockTarget = Target;
		Ability->BindExecutionLockTracking();
	}
	static void StartMontage(UHodgeGameplayAbility_Definition* Ability) { Ability->StartExecutionMontage(); }
	static bool IsTracking(const UHodgeGameplayAbility_Definition* Ability) { return Ability->bTrackExecutionLockTarget; }
	static bool IsFrozen(const UHodgeGameplayAbility_Definition* Ability) { return Ability->bExecutionLockTrackingFrozen; }
	static FHodgeFacingRequestHandle FacingHandle(const UHodgeGameplayAbility_Definition* Ability) { return Ability->AttackFacingHandle; }
	static void SetPredictionSuspended(UHodgeGameplayAbility_Definition* Ability, bool bSuspended)
	{ Ability->bPredictedMoveCancel = bSuspended; }
	/** 复现SetMoveFor与PostUpdate之间的原生求值作用域，不复制回填算法。 */
	static void BeginEvaluation(UHodgeCharacterMovementComponent* Movement) { Movement->BeginAttackWarpEvaluation(); }
	static void EndEvaluation(UHodgeCharacterMovementComponent* Movement) { Movement->EndAttackWarpEvaluation(); }
	/** 使用CMC原生Adapter读取当前SavedMove，再走实际MotionWarping更新与求值顺序。 */
	static FTransform ConvertReplay(UHodgeCharacterMovementComponent* Movement, FSavedMove_Character& Move)
	{
		Movement->SetCurrentReplayedSavedMove(&Move);
		ON_SCOPE_EXIT { Movement->SetCurrentReplayedSavedMove(nullptr); };
		return Movement->ConvertLocalRootMotionToWorld(Move.RootMotionMovement.GetRootMotionTransform(), Move.DeltaTime);
	}
	static void SetServerView(UHodgeCharacterRotationComponent* Rotation, const FHodgeCharacterRotationState& State)
	{ Rotation->bServerMoveOverride = true; Rotation->ServerMoveState = State; }
	/** 仅接入正式回调，分别模拟数据先到与能力先订阅的 GAS 缓存顺序。 */
	static void BindFacingData(UHodgeGameplayAbility_Definition* Ability, UHodgeAbilitySystemComponent* ASC,
		FGameplayAbilitySpecHandle Spec, const FPredictionKey& Key)
	{
		Ability->GetCurrentActivationInfoRef().ServerSetActivationPredictionKey(Key);
		Ability->bFacingDataWaiting = true;
		Ability->FacingDataDelegate = ASC->AbilityTargetDataSetDelegate(Spec, Key).AddUObject(
			Ability, &UHodgeGameplayAbility_Definition::OnAttackFacingData, Ability->ExecutionId);
	}
};
namespace HodgeAttackWarpTests
{
	UHodgeAbilityDefinition* Definition()
	{
		auto* OriginalData=LoadObject<UHodgeAbilityDefinition>(nullptr,TEXT("/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1.DA_Attack_1"));
		auto* Data=DuplicateObject<UHodgeAbilityDefinition>(OriginalData,GetTransientPackage()); Data->AbilityTag=HodgeGameplayTags::Status_Attack;
		Data->AbilityClass=UHodgeAttackWarpTestAbility::StaticClass(); Data->ExecutionRoute=EHodgeAbilityExecutionRoute::Standalone;
		auto* Original=LoadObject<UAnimMontage>(nullptr,TEXT("/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage.AM_Attack01_Montage"));
		Data->ExecutionConfig.Montage=DuplicateObject<UAnimMontage>(Original,Data); auto* Montage=Data->ExecutionConfig.Montage.Get();
		Montage->Notifies.RemoveAll([](const FAnimNotifyEvent& E) { return E.NotifyStateClass && E.NotifyStateClass->IsA<UAnimNotifyState_MotionWarping>(); });
		auto* Notify=NewObject<UAnimNotifyState_MotionWarping>(Montage); auto* Config=NewObject<UHodgeRootMotionModifier_AttackFacing>(Notify);
		Config->WarpTargetName=Data->FacingWarp.WarpTargetName; Notify->RootMotionModifier=Config;
		auto& Event=Montage->Notifies.AddDefaulted_GetRef(); Event.NotifyStateClass=Notify; Event.Link(Montage,0.f); Event.SetDuration(.1f); Event.EndLink.Link(Montage,.1f);
		Data->FacingWarp.bEnabled=true;
		// 基础夹具固定旧行为，正式五段资产的作者开关不应改变历史用例前提。
		Data->FacingWarp.bTrackExecutionLockTargetDuringWarp=false; return Data;
	}
	/** 临时权威角色无需编辑器地图，不写入正式资产。 */
	struct FFixture
	{
		UGameInstance* Instance=nullptr; UWorld* World;
		AHodgeCombatCharacter* Character; APlayerController* Controller; UHodgeAbilitySystemComponent* ASC;
		UHodgeCharacterRotationComponent* Rotation; UHodgeAttackMotionWarpingComponent* Warp;
		UHodgeAbilityDefinition* Data; UHodgeGameplayAbility_Definition* Ability=nullptr; FGameplayAbilitySpecHandle Spec;
		FGuid Execution;
		explicit FFixture(bool bWithAnimation = false)
		{
			World=FHodgeEnemyInitTestAccess::CreateWorld(Instance); Character=World->SpawnActor<AHodgeCombatCharacter>();
			Controller=World->SpawnActor<APlayerController>(); Controller->Possess(Character);
			ASC=NewObject<UHodgeAbilitySystemComponent>(Character); ASC->RegisterComponent(); ASC->InitAbilityActorInfo(Character,Character);
			if (bWithAnimation)
			{
				// 真实 Montage 实例验证数据处理确实到达播放入口，不改正式资产。
				Character->GetMesh()->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr,
					TEXT("/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab.SKM_Pover_LyraLab")));
				Character->GetMesh()->SetAnimInstanceClass(UAnimInstance::StaticClass());
				ASC->RefreshAbilityActorInfo();
			}
			Rotation=Character->GetCharacterRotationComponent(); Rotation->InitializeWithAbilitySystem(ASC);
			Warp=Character->FindComponentByClass<UHodgeAttackMotionWarpingComponent>(); if (!Warp->HasBeenInitialized()) { Warp->InitializeComponent(); }
			Data=Definition(); Spec=ASC->GiveAbilityDefinition(Data,1,Data);
			if (!Spec.IsValid() || !ASC->TryActivateAbility(Spec)) { return; }
			const auto* Granted=ASC->FindAbilitySpecFromHandle(Spec); Ability=Granted ? Cast<UHodgeGameplayAbility_Definition>(Granted->GetPrimaryInstance()) : nullptr;
			if (Ability) { Execution=FHodgeAttackWarpTestAccess::SetIdentity(Ability); }
		}
		~FFixture() { if (Ability && Ability->IsActive()) { Ability->FinishExecution(true,false); } World->DestroyWorld(false); }
	};
	/** 临时锁定资源使用真实PawnExtension、GAS、Montage和目标接口，不保存正式资产。 */
	struct FTrackingFixture : FFixture
	{
		TGuardValue<bool> ScriptGuard{GAllowActorScriptExecutionInEditor, true};
		UHodgeLockOnComponent* Lock = nullptr;
		AHodgeAttackWarpTestTarget* Target = nullptr;
		AHodgeAttackWarpTestTarget* OtherTarget = nullptr;
		explicit FTrackingFixture(bool bTrack = true) : FFixture(true)
		{
			Data->FacingWarp.bTrackExecutionLockTargetDuringWarp = bTrack;
			Data->FacingWarp.MaxCorrectionAngle = 60.f;
			// PawnExtension 会接入真实健康组件，源角色也必须具备所需属性集。
			if (!ASC->GetSet<UHodgeHealthSet>()) { ASC->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(Character)); }
			ASC->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), 100.f);
			ASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
			auto* PawnData = NewObject<UHodgePawnData>(Character);
			PawnData->LockOnProfile = NewObject<UHodgeLockOnProfile>(PawnData);
			auto* Extension = Character->FindComponentByClass<UHodgePawnExtensionComponent>();
			Extension->SetPawnData(PawnData);
			Extension->InitializeAbilitySystem(ASC, Character);
			Lock = NewObject<UHodgeLockOnComponent>(Character);
			Lock->RegisterComponent();
			FHodgeAttackWarpTestAccess::InitializeLock(Lock);
			Target = SpawnTarget(FVector(500.f, 0.f, 0.f));
			OtherTarget = SpawnTarget(FVector(0.f, -500.f, 0.f));
			FHodgeAttackWarpTestAccess::Confirm(Lock, Target);
			if (Ability) { FHodgeAttackWarpTestAccess::SetExecutionTarget(Ability, Target); }
		}
		~FTrackingFixture() { Lock->Uninitialize(); }
		AHodgeAttackWarpTestTarget* SpawnTarget(FVector Location)
		{
			auto* Result = World->SpawnActor<AHodgeAttackWarpTestTarget>(Location, FRotator::ZeroRotator);
			auto* TargetASC = Result->GetNativeEnemyAbilitySystem();
			TargetASC->InitAbilityActorInfo(Result, Result);
			if (!TargetASC->GetSet<UHodgeHealthSet>()) { TargetASC->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(Result)); }
			TargetASC->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), 100.f);
			TargetASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100.f);
			Result->FindComponentByClass<UHodgeHealthComponent>()->InitializeWithAbilitySystem(TargetASC);
			return Result;
		}
		/** Apply与播放均用正式入口；只补充无完整Activate的单测所需起手快照。 */
		bool Start(EHodgeAttackFacingSource Source = EHodgeAttackFacingSource::LockedTarget)
		{
			if (!Ability || !Lock->IsReady() || !Lock->IsConfirmedTargetValid()) { return false; }
			FHodgeAttackFacingTargetData Payload;
			if (!UHodgeGameplayAbility_Definition::BuildAttackFacingData(ASC->AbilityActorInfo.Get(), Payload,
				Data->FacingWarp.bTrackExecutionLockTargetDuringWarp)) { return false; }
			Payload.Source = Source;
			if (Source != EHodgeAttackFacingSource::LockedTarget) { Payload.LockedTarget.Reset(); }
			if (!FHodgeAttackWarpTestAccess::Apply(Ability, Payload)) { return false; }
			FHodgeAttackWarpTestAccess::StartMontage(Ability);
			return Ability->IsActive() && Instance() != nullptr;
		}
		FAnimMontageInstance* Instance() const
		{ return Character->GetMesh()->GetAnimInstance()->GetActiveInstanceForMontage(Data->ExecutionConfig.Montage); }
		void SetPosition(float Position) { if (auto* Playing = Instance()) { Playing->SetPosition(Position); } }
		void MoveTarget(float Yaw)
		{ Target->SetActorLocation(Character->GetActorLocation() + FRotator(0.f, Yaw, 0.f).Vector() * 500.f); }
	};
	/** 读取真实SavedMove恢复的状态，避免在测试翻刻其私有布局。 */
	FHodgeCharacterRotationState SavedRotation(FSavedMove_Character& Move, FFixture& Fixture)
	{
		// 引擎位域不能绑定引用，按作用域配对恢复回放标志。
		const bool bWasClientUpdating = Fixture.Character->bClientUpdating;
		Fixture.Character->bClientUpdating = true;
		ON_SCOPE_EXIT { Fixture.Character->bClientUpdating = bWasClientUpdating; };
		Move.PrepMoveFor(Fixture.Character);
		const auto State = Fixture.Rotation->GetResolvedState();
		Fixture.Rotation->ClearMoveReplayState();
		return State;
	}
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpConfigTest,"Hodge.AttackWarp.ConfigBounds",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAttackWarpConfigTest::RunTest(const FString&)
{
	FHodgeAttackFacingConfig Config; FString Error; TestTrue(TEXT("默认关闭不改变既有技能"),Config.Validate(Error));
	TestFalse(TEXT("锁定持续追踪默认关闭"), Config.bTrackExecutionLockTargetDuringWarp);
	Config.bEnabled=true; TestTrue(TEXT("默认启用参数合法"),Config.Validate(Error));
	Config.MaxCorrectionAngle=180.f; TestFalse(TEXT("整圈方向歧义拒绝"),Config.Validate(Error)); Config.MaxCorrectionAngle=90.f;
	Config.MaxRotationRate=std::numeric_limits<float>::infinity(); TestFalse(TEXT("无限速度拒绝"),Config.Validate(Error)); Config.MaxRotationRate=1080.f;
	Config.TargetDataTimeout=0.f; TestFalse(TEXT("零等待上限拒绝"),Config.Validate(Error)); Config.TargetDataTimeout=1.f;
	Config.WarpTargetName=NAME_None; TestFalse(TEXT("空目标名拒绝"),Config.Validate(Error)); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpYawTest,"Hodge.AttackWarp.FixedYaw",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAttackWarpYawTest::RunTest(const FString&)
{
	float Out=0.f; TestTrue(TEXT("正常方向"),HodgeAttackFacing::ResolveYaw(0.f,45.f,90.f,Out)); TestEqual(TEXT("固定世界方向"),Out,45.f);
	HodgeAttackFacing::ResolveYaw(0.f,150.f,90.f,Out); TestEqual(TEXT("过大角度只做有限修正"),Out,90.f);
	HodgeAttackFacing::ResolveYaw(179.f,-179.f,90.f,Out); TestTrue(TEXT("跨180度最短圆弧"),FMath::IsNearlyEqual(Out,-179.f));
	TestFalse(TEXT("非法方向拒绝"),HodgeAttackFacing::ResolveYaw(0.f,std::numeric_limits<float>::quiet_NaN(),90.f,Out)); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpWindowTest,"Hodge.AttackWarp.WindowContract",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAttackWarpWindowTest::RunTest(const FString&)
{
	auto* Data=HodgeAttackWarpTests::Definition(); TArray<FText> Errors; TestTrue(TEXT("唯一窗口在首刀前"),Data->ValidateDefinition(Errors));
	float Start=0.f,End=0.f; TestTrue(TEXT("窗口只来自原生通知"),Data->GetFacingWarpWindow(Start,End)); TestTrue(TEXT("源时间正确"),FMath::IsNearlyEqual(End,.1f));
	auto& Event=Data->ExecutionConfig.Montage->Notifies.Last(); Event.SetDuration(.2f); Event.EndLink.Link(Data->ExecutionConfig.Montage,.2f);
	Errors.Reset(); TestFalse(TEXT("跨越首刀拒绝"),Data->ValidateDefinition(Errors)); Event.SetDuration(.1f); Event.EndLink.Link(Data->ExecutionConfig.Montage,.1f);
	const FAnimNotifyEvent Duplicate=Event; Data->ExecutionConfig.Montage->Notifies.Add(Duplicate); TestFalse(TEXT("重复窗口拒绝"),Data->GetFacingWarpWindow(Start,End)); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpLeaseTest,"Hodge.AttackWarp.LeaseAndSuspension",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAttackWarpLeaseTest::RunTest(const FString&)
{
	HodgeAttackWarpTests::FFixture F; if (!F.Ability) { AddError(TEXT("Test fixture grant/activation failed")); return false; } const auto Handle=F.Rotation->RequestMontageWarpFacing(45.f,F.Ability,F.Execution);
	TestTrue(TEXT("获准后可以播放，不等Applied"),F.Rotation->IsFacingRequestSelected(Handle)); TestFalse(TEXT("许可不等于已转到目标"),F.Rotation->IsActionFacingApplied(Handle));
	TestEqual(TEXT("执行方式进入快照"),F.Rotation->GetResolvedState().ExecutionMode,EHodgeFacingExecutionMode::MontageWarp);
	F.Rotation->SetFacingRequestSuspended(Handle,true); TestFalse(TEXT("预测取消暂停执行"),F.Rotation->IsFacingRequestSelected(Handle));
	F.Rotation->SetFacingRequestSuspended(Handle,false); TestTrue(TEXT("拒绝取消恢复原请求"),F.Rotation->IsFacingRequestSelected(Handle));
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked); TestFalse(TEXT("强约束优先"),F.Rotation->IsFacingRequestSelected(Handle));
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked); F.Rotation->ReleaseFacingRequest(Handle);
	TestEqual(TEXT("释放后恢复物理策略"),F.Rotation->GetResolvedState().ExecutionMode,EHodgeFacingExecutionMode::PhysicalTurn); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpRootTest,"Hodge.AttackWarp.RootExecutor",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAttackWarpRootTest::RunTest(const FString&)
{
	HodgeAttackWarpTests::FFixture F; if (!F.Ability) { AddError(TEXT("Test fixture grant/activation failed")); return false; } F.Rotation->RequestMontageWarpFacing(45.f,F.Ability,F.Execution);
	F.Warp->AddOrUpdateWarpTargetFromLocationAndRotation(F.Data->FacingWarp.WarpTargetName,F.Character->GetActorLocation(),FRotator(0.f,45.f,0.f));
	auto* Template=NewObject<UHodgeRootMotionModifier_AttackFacing>(); Template->WarpTargetName=F.Data->FacingWarp.WarpTargetName;
	auto* Modifier=Cast<UHodgeRootMotionModifier_AttackFacing>(F.Warp->AddModifierFromTemplate(Template,F.Data->ExecutionConfig.Montage,0.f,.1f));
	FMotionWarpingUpdateContext Context; Context.Animation=F.Data->ExecutionConfig.Montage; Context.PreviousPosition=0.f; Context.CurrentPosition=.02f; Context.PlayRate=2.f; Context.DeltaSeconds=.01f; Context.Weight=1.f;
	Modifier->Update(Context); TestTrue(TEXT("实际倍率调整旋转剩余时间"),FMath::IsNearlyEqual(Modifier->WarpRotationTimeMultiplier,.5f));
	const FVector Translation(3.f,4.f,5.f); const auto Result=Modifier->ProcessRootMotion(FTransform(FQuat::Identity,Translation),.01f);
	TestTrue(TEXT("原动画位移保持"),Result.GetTranslation().Equals(Translation)); TestTrue(TEXT("根运动执行旋转"),Result.GetRotation().GetAngle()>KINDA_SMALL_NUMBER);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked); Modifier->Update(Context); TestEqual(TEXT("约束禁用Warp修正器"),Modifier->GetState(),ERootMotionModifierState::Disabled); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpReplayWindowTest, "Hodge.AttackWarp.ReplayWindowRebuild",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 回放从窗后倒回最后一个合法Warp步，必须当步重建且窗外不能继续旋转。 */
bool FHodgeAttackWarpReplayWindowTest::RunTest(const FString&)
{
	HodgeAttackWarpTests::FFixture F(true);
	if (!TestNotNull(TEXT("回放夹具活动能力"),F.Ability)) { return false; }
	F.Rotation->RequestMontageWarpFacing(135.f,F.Ability,F.Execution);
	const auto Historical=F.Rotation->GetResolvedState();
	auto* Movement=CastChecked<UHodgeCharacterMovementComponent>(F.Character->GetCharacterMovement());
	auto* Client=static_cast<FNetworkPredictionData_Client_Character*>(Movement->GetPredictionData_Client());
	const auto Move=Client->AllocateNewMove(); Move->Clear();
	Move->SetMoveFor(F.Character,.04f,FVector::ZeroVector,*Client);
	Move->RootMotionMontage=F.Data->ExecutionConfig.Montage;
	Move->RootMotionPreviousTrackPosition=.08f; Move->RootMotionTrackPosition=.12f; Move->RootMotionPlayRateWithScale=1.f;
	Move->RootMotionMovement.Set(FTransform(FQuat::Identity,FVector(16.64f,0.f,0.f)));
	// 原动画参数与服务端校正Yaw固定；两次只改变项目缓存是否已经位于窗后。
	F.Character->SetActorRotation(FRotator(0.f,74.6159f,0.f));
	const bool bWasClientUpdating=F.Character->bClientUpdating;
	ON_SCOPE_EXIT { F.Character->bClientUpdating=bWasClientUpdating; F.Rotation->ClearMoveReplayState(); };
	float ReferenceYaw=0.f;
	for (bool bRewind : {false,true})
	{
		F.Character->bClientUpdating=false; F.Rotation->ClearMoveReplayState();
		F.Warp->ClearAttackWarp();
		F.Warp->PrepareAttackTarget(Historical,F.Data->ExecutionConfig.Montage,bRewind ? .3f : .08f);
		F.Character->bClientUpdating=true; Move->PrepMoveFor(F.Character);
		const auto Root=FHodgeAttackWarpTestAccess::ConvertReplay(Movement,*Move);
		const float Yaw=Root.GetRotation().Rotator().Yaw;
		TestTrue(TEXT("旋转保持原速率上限"),Yaw>1.f && Yaw<=F.Data->FacingWarp.MaxRotationRate*Move->DeltaTime+.01f);
		TestTrue(TEXT("重建不改变原动画位移长度"),FMath::IsNearlyEqual(Root.GetTranslation().Size(),16.64,.01));
		if (!bRewind) { ReferenceYaw=Yaw; }
		else { TestTrue(TEXT("从窗后倒回的首步完整保留旋转"),FMath::IsNearlyEqual(Yaw,ReferenceYaw,.01f)); }
	}
	Move->RootMotionPreviousTrackPosition=.12f; Move->RootMotionTrackPosition=.16f;
	const auto Outside=FHodgeAttackWarpTestAccess::ConvertReplay(Movement,*Move);
	TestTrue(TEXT("历史窗口结束后不补额外旋转"),Outside.GetRotation().Equals(FQuat::Identity,.0001f));
	// 无效配置与传入动画同时为空时仍须安全清理，不能在重建扫描中解引用空动画。
	auto Invalid=Historical;
	auto* MissingMontage=NewObject<UHodgeAbilityDefinition>(); MissingMontage->FacingWarp.bEnabled=true;
	Invalid.WarpDefinition=MissingMontage;
	F.Warp->PrepareAttackTarget(Invalid,nullptr,.08f);
	TestNull(TEXT("空动画不能遗留攻击Warp目标"),F.Warp->FindWarpTarget(F.Data->FacingWarp.WarpTargetName));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpProtocolTest,"Hodge.AttackWarp.TargetDataRetirement",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAttackWarpProtocolTest::RunTest(const FString&)
{
	HodgeAttackWarpTests::FFixture F; if (!F.Ability) { AddError(TEXT("Test fixture grant/activation failed")); return false; } FPredictionKey Key; Key.Current=27;
	FHodgeAttackFacingTargetData Data; Data.SourceAvatar=F.Character; Data.DesiredYaw=45.f;
	F.ASC->ServerSetReplicatedTargetData_Implementation(F.Spec,Key,FGameplayAbilityTargetDataHandle(new FHodgeAttackFacingTargetData(Data)),{},Key);
	FGameplayAbilityTargetDataHandle Cached; FGameplayAbilityActivationInfo Info; Info.ServerSetActivationPredictionKey(Key); F.ASC->GetAbilityTargetData(F.Spec,Info,Cached);
	TestEqual(TEXT("先到数据进入GAS缓存"),Cached.Num(),1); F.ASC->ConsumeClientReplicatedTargetData(F.Spec,Key); F.ASC->RetireAttackFacingData(F.Spec,Key);
	F.ASC->ServerSetReplicatedTargetData_Implementation(F.Spec,Key,FGameplayAbilityTargetDataHandle(new FHodgeAttackFacingTargetData(Data)),{},Key);
	F.ASC->GetAbilityTargetData(F.Spec,Info,Cached); TestEqual(TEXT("迟到重复数据不复活缓存"),Cached.Num(),0); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpReplayTest,"Hodge.AttackWarp.ReplayAndOwnership",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAttackWarpReplayTest::RunTest(const FString&)
{
	HodgeAttackWarpTests::FFixture F; if (!F.Ability) { AddError(TEXT("Test fixture grant/activation failed")); return false; } const auto Old=F.Rotation->RequestMontageWarpFacing(45.f,F.Ability,F.Execution);
	const auto Historical=F.Rotation->GetResolvedState(); F.Execution=FHodgeAttackWarpTestAccess::SetIdentity(F.Ability);
	const auto Current=F.Rotation->RequestMontageWarpFacing(30.f,F.Ability,F.Execution); const auto Live=F.Rotation->GetResolvedState();
	F.Warp->PrepareAttackTarget(Live,F.Data->ExecutionConfig.Montage,.02f); F.Warp->ClearAttackWarpForRequest(Old.Id);
	TestNotNull(TEXT("旧执行清理不删除新目标"),F.Warp->FindWarpTarget(F.Data->FacingWarp.WarpTargetName));
	F.Warp->PrepareAttackTarget(Historical,F.Data->ExecutionConfig.Montage,.01f);
	const auto* Target=F.Warp->FindWarpTarget(F.Data->FacingWarp.WarpTargetName);
	TestTrue(TEXT("回放使用历史Yaw而非当前刀"),Target && FMath::IsNearlyEqual(Target->Rotator().Yaw,45.,.01));
	F.Rotation->ReleaseFacingRequest(Old); TestTrue(TEXT("释放旧请求不抢走新动作"),F.Rotation->IsFacingRequestSelected(Current)); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpValidationTest,"Hodge.AttackWarp.PayloadValidation",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAttackWarpValidationTest::RunTest(const FString&)
{
	HodgeAttackWarpTests::FFixture F; if (!F.Ability) { AddError(TEXT("Test fixture grant/activation failed")); return false; } FHodgeAttackFacingTargetData Data; Data.SourceAvatar=F.Character; Data.Source=EHodgeAttackFacingSource::Camera; Data.DesiredYaw=45.f;
	TestTrue(TEXT("合法方向取得动作占用"),FHodgeAttackWarpTestAccess::Apply(F.Ability,Data));
	Data.SourceAvatar=F.Controller; TestFalse(TEXT("旧或其他Avatar拒绝"),FHodgeAttackWarpTestAccess::Apply(F.Ability,Data)); Data.SourceAvatar=F.Character;
	Data.StartYaw=100.f; TestFalse(TEXT("伪造起手朝向拒绝"),FHodgeAttackWarpTestAccess::Apply(F.Ability,Data)); Data.StartYaw=0.f;
	Data.DesiredYaw=std::numeric_limits<float>::quiet_NaN(); TestFalse(TEXT("非法方向拒绝"),FHodgeAttackWarpTestAccess::Apply(F.Ability,Data)); Data.DesiredYaw=45.f;
	Data.Source=EHodgeAttackFacingSource::LockedTarget; TestFalse(TEXT("未授权锁定目标拒绝"),FHodgeAttackWarpTestAccess::Apply(F.Ability,Data)); Data.Source=EHodgeAttackFacingSource::Camera;
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death); TestFalse(TEXT("死亡约束不可绕过"),FHodgeAttackWarpTestAccess::Apply(F.Ability,Data)); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpRetiredStateTest,"Hodge.AttackWarp.LateAuthorityDoesNotRevive",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAttackWarpRetiredStateTest::RunTest(const FString&)
{
	HodgeAttackWarpTests::FFixture F; if (!F.Ability) { AddError(TEXT("Test fixture grant/activation failed")); return false; }
	const auto Handle=F.Rotation->RequestMontageWarpFacing(45.f,F.Ability,F.Execution);
	auto OldAuthority=F.Rotation->GetResolvedState();
	OldAuthority.ActionRequestId=FGuid::NewGuid();
	// 仅将夹具角色切到拥有者分支；真实GAS活动身份与请求释放/同步解析均沿用生产入口。
	F.Character->SetRole(ROLE_AutonomousProxy);
	ON_SCOPE_EXIT { F.Character->SetRole(ROLE_Authority); };
	TestTrue(TEXT("夹具为本地拥有者且保留有效服务器租约"),F.Character->IsLocallyControlled() && OldAuthority.StateVersion>0);
	TestTrue(TEXT("活动执行允许权威回退"),FHodgeAttackWarpTestAccess::AuthorityCurrent(F.Rotation,OldAuthority));
	FHodgeAttackWarpTestAccess::Release(F.Ability,Handle);
	// 必须当次同步解析就退出Warp，不能等下一帧或第二次OnRep才修正旧占用。
	TestEqual(TEXT("Recovery释放返回时已退出根运动占用"),F.Rotation->GetResolvedState().ExecutionMode,EHodgeFacingExecutionMode::PhysicalTurn);
	TestFalse(TEXT("Recovery释放不能回填服务器旧请求"),F.Rotation->GetResolvedState().ActionRequestId.IsValid());
	TestFalse(TEXT("Recovery之后旧权威占用不复活"),FHodgeAttackWarpTestAccess::AuthorityCurrent(F.Rotation,OldAuthority));
	FHodgeAttackWarpTestAccess::ReplayAuthority(F.Rotation,OldAuthority);
	TestEqual(TEXT("迟到OnRep仍不能重建根运动占用"),F.Rotation->GetResolvedState().ExecutionMode,EHodgeFacingExecutionMode::PhysicalTurn);
	TestTrue(TEXT("朝向退役不提前结束当前能力"),F.Ability->IsActive());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpDeliveryOrderTest, "Hodge.AttackWarp.TargetDataDeliveryOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 两种真实委托顺序都必须启动一次 Montage，重复与取消后的数据不能复活执行。 */
bool FHodgeAttackWarpDeliveryOrderTest::RunTest(const FString&)
{
	for (bool bDataFirst : {true, false})
	{
		HodgeAttackWarpTests::FFixture F(true);
		if (!TestNotNull(TEXT("Activated facing ability exists"), F.Ability) ||
			!TestNotNull(TEXT("Real target anim instance exists"), F.Character->GetMesh()->GetAnimInstance())) { return false; }
		FPredictionKey Key;
		Key.Current = 41;
		FHodgeAttackFacingTargetData Data;
		Data.SourceAvatar = F.Character;
		Data.DesiredYaw = 45.f;
		FGameplayAbilityTargetDataHandle Input(new FHodgeAttackFacingTargetData(Data));
		if (!bDataFirst) { FHodgeAttackWarpTestAccess::BindFacingData(F.Ability, F.ASC, F.Spec, Key); }
		F.ASC->ServerSetReplicatedTargetData_Implementation(F.Spec, Key, Input, {}, Key);
		if (bDataFirst)
		{
			FHodgeAttackWarpTestAccess::BindFacingData(F.Ability, F.ASC, F.Spec, Key);
			TestTrue(TEXT("Cached data reaches the production callback"), F.ASC->CallReplicatedTargetDataDelegatesIfSet(F.Spec, Key));
		}
		TestTrue(TEXT("Valid facing data keeps the execution active"), F.Ability->IsActive());
		TestEqual(TEXT("Facing uses the received snapshot"), F.Ability->GetAttackFacingYaw(), 45.f);
		auto* Anim = F.Character->GetMesh()->GetAnimInstance();
		const auto* Played = Anim->GetActiveInstanceForMontage(F.Data->ExecutionConfig.Montage);
		if (!TestNotNull(TEXT("Facing callback starts a real montage instance"), Played)) { return false; }
		const int32 InstanceId = Played->GetInstanceID();
		FGameplayAbilityTargetDataHandle Cached;
		F.ASC->GetAbilityTargetData(F.Spec, F.Ability->GetCurrentActivationInfo(), Cached);
		TestEqual(TEXT("Processed data is consumed"), Cached.Num(), 0);
		F.ASC->ServerSetReplicatedTargetData_Implementation(F.Spec, Key, Input, {}, Key);
		Played = Anim->GetActiveInstanceForMontage(F.Data->ExecutionConfig.Montage);
		TestTrue(TEXT("Duplicate delivery cannot restart the montage"), Played && Played->GetInstanceID() == InstanceId);
		F.Ability->FinishExecution(true, false);
		F.ASC->ServerSetReplicatedTargetData_Implementation(F.Spec, Key, Input, {}, Key);
		TestFalse(TEXT("Cancelled execution is not revived by late data"), F.Ability->IsActive());
		TestFalse(TEXT("Retired data cannot broadcast again"), F.ASC->CallReplicatedTargetDataDelegatesIfSet(F.Spec, Key));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpTrackingOptInTest, "Hodge.AttackWarp.LockTracking.OptInAndConfirmedPoint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 默认关闭保持固定方向；开启后只读确认目标与确认点，排队输入不改本刀事实。 */
bool FHodgeAttackWarpTrackingOptInTest::RunTest(const FString&)
{
	{
		HodgeAttackWarpTests::FTrackingFixture F(false);
		if (!TestTrue(TEXT("关闭追踪仍能沿原链路播放"), F.Start())) { return false; }
		F.MoveTarget(40.f); F.SetPosition(.03f); F.Ability->UpdateAttackFacingLifecycle();
		TestEqual(TEXT("关闭追踪保持起手Yaw"), F.Ability->GetAttackFacingYaw(), 0.f);
		TestFalse(TEXT("关闭开关不建立追踪"), FHodgeAttackWarpTestAccess::IsTracking(F.Ability));
	}
	{
		HodgeAttackWarpTests::FTrackingFixture F;
		FHodgeAttackWarpTestAccess::Predict(F.Lock, F.Target, TEXT("Alternate"), false);
		FVector PredictedPoint;
		TestTrue(TEXT("表现点确实与确认点不同"), F.Lock->GetPresentationLockPoint(PredictedPoint) && PredictedPoint.Y > 400.f);
		if (!TestTrue(TEXT("确认点载荷不受Pending点位影响"), F.Start())) { return false; }
		TestTrue(TEXT("已确认同一目标允许追踪"), FHodgeAttackWarpTestAccess::IsTracking(F.Ability));
		F.MoveTarget(30.f); F.SetPosition(.03f); F.Ability->UpdateAttackFacingLifecycle();
		TestTrue(TEXT("持续采样使用确认点而非同对象预测点"), FMath::IsNearlyEqual(F.Ability->GetAttackFacingYaw(), 30.f, .01f));
		FHodgeAttackWarpTestAccess::Predict(F.Lock, F.OtherTarget, NAME_None, true);
		TestEqual(TEXT("夹具确有待确认切换"), F.Lock->GetPresentationTarget(), static_cast<AActor*>(F.OtherTarget));
		F.MoveTarget(45.f); F.SetPosition(.04f); F.Ability->UpdateAttackFacingLifecycle();
		TestTrue(TEXT("Queued切换继续原已确认目标"), FMath::IsNearlyEqual(F.Ability->GetAttackFacingYaw(), 45.f, .01f));
		TestFalse(TEXT("Pending或Queued本身不会冻结本刀"), FHodgeAttackWarpTestAccess::IsFrozen(F.Ability));
	}
	{
		HodgeAttackWarpTests::FTrackingFixture F;
		if (!TestTrue(TEXT("非锁定来源仍可申请固定方向"), F.Start(EHodgeAttackFacingSource::Camera))) { return false; }
		TestFalse(TEXT("等待或镜头载荷不能后来改绑为追踪"), FHodgeAttackWarpTestAccess::IsTracking(F.Ability));
		F.MoveTarget(40.f); F.SetPosition(.03f); F.Ability->UpdateAttackFacingLifecycle();
		TestEqual(TEXT("非锁定来源保持原方向"), F.Ability->GetAttackFacingYaw(), 0.f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpTrackingBoundsTest, "Hodge.AttackWarp.LockTracking.StartYawAndWindowBounds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 多次更新只能消费同一起手角度额度，低帧率越尾不能补一次追踪。 */
bool FHodgeAttackWarpTrackingBoundsTest::RunTest(const FString&)
{
	{
		HodgeAttackWarpTests::FTrackingFixture F;
		if (!TestTrue(TEXT("有界追踪夹具启动"), F.Start())) { return false; }
		const auto Request = FHodgeAttackWarpTestAccess::FacingHandle(F.Ability);
		F.MoveTarget(40.f); F.SetPosition(.02f); F.Ability->UpdateAttackFacingLifecycle();
		TestTrue(TEXT("第一步追踪当前同目标"), FMath::IsNearlyEqual(F.Ability->GetAttackFacingYaw(), 40.f, .01f));
		F.Character->SetActorRotation(FRotator(0.f, 40.f, 0.f)); F.MoveTarget(110.f); F.SetPosition(.04f);
		F.Ability->UpdateAttackFacingLifecycle();
		TestTrue(TEXT("实际角色已转动也不能重置起手额度"), FMath::IsNearlyEqual(F.Ability->GetAttackFacingYaw(), 60.f, .01f));
		TestEqual(TEXT("更新沿用同一请求身份"), F.Rotation->GetResolvedState().ActionRequestId, Request.Id);
		F.MoveTarget(-120.f); F.SetPosition(.06f); F.Ability->UpdateAttackFacingLifecycle();
		TestTrue(TEXT("反向目标仍受原起手负向上限"), FMath::IsNearlyEqual(F.Ability->GetAttackFacingYaw(), -60.f, .01f));
	}
	for (float CrossedPosition : {.10f, .12f})
	{
		HodgeAttackWarpTests::FTrackingFixture F;
		if (!TestTrue(TEXT("低帧率越尾夹具启动"), F.Start())) { return false; }
		F.MoveTarget(35.f); F.SetPosition(CrossedPosition); F.Ability->UpdateAttackFacingLifecycle();
		TestEqual(TEXT("窗口尾或跨尾帧不补追踪"), F.Ability->GetAttackFacingYaw(), 0.f);
		TestTrue(TEXT("越尾永久冻结"), FHodgeAttackWarpTestAccess::IsFrozen(F.Ability));
		F.SetPosition(.03f); F.Ability->UpdateAttackFacingLifecycle();
		TestEqual(TEXT("位置校正回窗口也不恢复追踪"), F.Ability->GetAttackFacingYaw(), 0.f);
	}
	{
		HodgeAttackWarpTests::FTrackingFixture F;
		// 人为提前首伤害通知，单独验证运行边界；该非法作者配置不写回资产。
		for (auto& Notify : F.Data->ExecutionConfig.Montage->Notifies)
		{
			if ((Notify.NotifyStateClass && Notify.NotifyStateClass->IsA<UHodgeAnimNotifyState_HitCheck>()) ||
				(Notify.Notify && Notify.Notify->IsA<UHodgeAnimNotify_Hit>())) { Notify.Link(F.Data->ExecutionConfig.Montage, .05f); }
		}
		if (!TestTrue(TEXT("首伤害边界夹具启动"), F.Start())) { return false; }
		F.MoveTarget(35.f); F.SetPosition(.06f); F.Ability->UpdateAttackFacingLifecycle();
		TestEqual(TEXT("即使Warp尚未结束也不跨首伤害追踪"), F.Ability->GetAttackFacingYaw(), 0.f);
		TestTrue(TEXT("首伤害通知开始即冻结"), FHodgeAttackWarpTestAccess::IsFrozen(F.Ability));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpTrackingWaitingDataTest, "Hodge.AttackWarp.LockTracking.WaitingTargetDataCannotRebind",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 夹具接入生产候选绑定与GAS委托；本地投递只验证等待时序，不冒充真实网络RPC。 */
bool FHodgeAttackWarpTrackingWaitingDataTest::RunTest(const FString&)
{
	for (bool bChangedWhileWaiting : {false, true})
	{
		HodgeAttackWarpTests::FTrackingFixture F;
		if (!TestNotNull(TEXT("等待载荷的活动能力"), F.Ability)) { return false; }
		FHodgeAttackFacingTargetData Data;
		if (!TestTrue(TEXT("保留起手确认TargetData"), UHodgeGameplayAbility_Definition::BuildAttackFacingData(F.ASC->AbilityActorInfo.Get(), Data, true))) { return false; }
		FPredictionKey Key; Key.Current = bChangedWhileWaiting ? 81 : 82;
		FHodgeAttackWarpTestAccess::BindFacingData(F.Ability, F.ASC, F.Spec, Key);
		TestTrue(TEXT("等待期间尚未启动Montage"), F.Ability->IsAwaitingFacingData() && !F.Instance());
		if (bChangedWhileWaiting)
		{
			FHodgeAttackWarpTestAccess::Confirm(F.Lock, F.OtherTarget);
			FHodgeAttackWarpTestAccess::Confirm(F.Lock, F.Target);
			TestTrue(TEXT("等待时B确认再A已经永久冻结"), FHodgeAttackWarpTestAccess::IsFrozen(F.Ability));
		}
		F.ASC->ServerSetReplicatedTargetData_Implementation(F.Spec, Key,
			FGameplayAbilityTargetDataHandle(new FHodgeAttackFacingTargetData(Data)), {}, Key);
		if (!TestTrue(TEXT("原合法载荷依然沿正式回调播放一次"), F.Ability->IsActive() && F.Instance())) { return false; }
		F.MoveTarget(40.f); F.SetPosition(.03f); F.Ability->UpdateAttackFacingLifecycle();
		TestTrue(TEXT("等待期间的冻结事实经过Apply和旧请求释放仍保留"),
			FMath::IsNearlyEqual(F.Ability->GetAttackFacingYaw(), bChangedWhileWaiting ? 0.f : 40.f, .01f));
		TestEqual(TEXT("载荷等待不替换本刀起手对象"), F.Ability->GetExecutionLockTarget(), static_cast<AActor*>(F.Target));
	}
	{
		HodgeAttackWarpTests::FTrackingFixture F;
		FHodgeAttackWarpTestAccess::SetExecutionTarget(F.Ability, F.OtherTarget);
		if (!TestTrue(TEXT("对象不一致时保留合法固定方向播放"), F.Start())) { return false; }
		TestFalse(TEXT("当前确认对象不能重绑起手快照"), FHodgeAttackWarpTestAccess::IsTracking(F.Ability));
		TestEqual(TEXT("起手快照仍是原对象"), F.Ability->GetExecutionLockTarget(), static_cast<AActor*>(F.OtherTarget));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpTrackingFreezeTest, "Hodge.AttackWarp.LockTracking.PermanentFreeze",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 停止条件消失后也不得恢复；确认B后又确认A不能因两次求值间隙漏掉冻结。 */
bool FHodgeAttackWarpTrackingFreezeTest::RunTest(const FString&)
{
	const TCHAR* Cases[] = {TEXT("Switch"), TEXT("Clear"), TEXT("InvalidRange"), TEXT("YawLocked"), TEXT("PredictionSuspend"), TEXT("OldExecution"), TEXT("Release")};
	for (const TCHAR* Case : Cases)
	{
		HodgeAttackWarpTests::FTrackingFixture F;
		if (!TestTrue(FString(Case) + TEXT("夹具启动"), F.Start())) { return false; }
		F.MoveTarget(20.f); F.SetPosition(.02f); F.Ability->UpdateAttackFacingLifecycle();
		const float LastYaw = F.Ability->GetAttackFacingYaw();
		const FGuid Execution = F.Ability->GetExecutionId();
		if (FCString::Strcmp(Case, TEXT("Switch")) == 0)
		{
			FHodgeAttackWarpTestAccess::Confirm(F.Lock, F.OtherTarget);
			FHodgeAttackWarpTestAccess::Confirm(F.Lock, F.Target);
		}
		else if (FCString::Strcmp(Case, TEXT("Clear")) == 0)
		{
			FHodgeAttackWarpTestAccess::Confirm(F.Lock, nullptr);
			FHodgeAttackWarpTestAccess::Confirm(F.Lock, F.Target);
		}
		else if (FCString::Strcmp(Case, TEXT("InvalidRange")) == 0) { F.Target->SetActorLocation(FVector(10000.f, 0.f, 0.f)); }
		else if (FCString::Strcmp(Case, TEXT("YawLocked")) == 0) { F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked); }
		else if (FCString::Strcmp(Case, TEXT("PredictionSuspend")) == 0) { FHodgeAttackWarpTestAccess::SetPredictionSuspended(F.Ability, true); }
		else if (FCString::Strcmp(Case, TEXT("OldExecution")) == 0) { FHodgeAttackWarpTestAccess::SetIdentity(F.Ability); }
		else { FHodgeAttackWarpTestAccess::Release(F.Ability, FHodgeAttackWarpTestAccess::FacingHandle(F.Ability)); }
		F.Ability->UpdateAttackFacingLifecycle();
		TestTrue(FString(Case) + TEXT("触发永久冻结"), FHodgeAttackWarpTestAccess::IsFrozen(F.Ability));
		F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Rotation_Locked);
		FHodgeAttackWarpTestAccess::SetPredictionSuspended(F.Ability, false);
		F.MoveTarget(45.f); F.SetPosition(.04f); F.Ability->UpdateAttackFacingLifecycle();
		TestTrue(FString(Case) + TEXT("解除条件后保留最后合法Yaw"), FMath::IsNearlyEqual(F.Ability->GetAttackFacingYaw(), LastYaw, .01f));
		TestEqual(FString(Case) + TEXT("不改变本刀锁定快照"), F.Ability->GetExecutionLockTarget(), static_cast<AActor*>(F.Target));
		if (FCString::Strcmp(Case, TEXT("OldExecution")) != 0) { TestEqual(TEXT("冻结不创建下一执行"), F.Ability->GetExecutionId(), Execution); }
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpTrackingServerViewTest, "Hodge.AttackWarp.LockTracking.ServerMoveViewAndIdentity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 同步当前移动视图的Yaw单字段，旧请求、旧代次、回放或不同执行不能污染当前占用。 */
bool FHodgeAttackWarpTrackingServerViewTest::RunTest(const FString&)
{
	HodgeAttackWarpTests::FTrackingFixture F;
	if (!TestTrue(TEXT("服务器视图夹具启动"), F.Start())) { return false; }
	const auto Handle = FHodgeAttackWarpTestAccess::FacingHandle(F.Ability);
	const auto Initial = F.Rotation->GetResolvedState();
	F.Rotation->BeginServerMove(Initial.RequestSequence, Initial.ActionPredictionKey);
	F.MoveTarget(35.f); F.SetPosition(.03f); F.Ability->UpdateAttackFacingLifecycle();
	const auto Updated = F.Rotation->GetResolvedState();
	TestTrue(TEXT("同帧服务器移动视图即消费新Yaw"), FMath::IsNearlyEqual(Updated.ActionYaw, 35.f, .01f));
	auto Expected = Initial; Expected.ActionYaw = Updated.ActionYaw;
	TestTrue(TEXT("服务器视图只改Yaw"), FHodgeCharacterRotationState::StaticStruct()->CompareScriptStruct(&Expected, &Updated, PPF_None));
	F.Rotation->EndServerMove();
	const auto LocalBefore = F.Rotation->GetResolvedState();
	auto StaleHandle = Handle; --StaleHandle.AvatarGeneration;
	TestFalse(TEXT("旧Avatar代次不能更新"), F.Rotation->UpdateMontageWarpFacing(StaleHandle, F.Ability, F.Execution, 50.f));
	TestFalse(TEXT("旧执行不能更新"), F.Rotation->UpdateMontageWarpFacing(Handle, F.Ability, FGuid::NewGuid(), 50.f));
	TestFalse(TEXT("非法Yaw不能更新"), F.Rotation->UpdateMontageWarpFacing(Handle, F.Ability, F.Execution, std::numeric_limits<float>::quiet_NaN()));
	F.Rotation->SetFacingRequestSuspended(Handle, true);
	TestFalse(TEXT("已暂停请求不能更新"), F.Rotation->UpdateMontageWarpFacing(Handle, F.Ability, F.Execution, 50.f));
	F.Rotation->SetFacingRequestSuspended(Handle, false);
	{
		const bool bWasClientUpdating = F.Character->bClientUpdating;
		F.Character->bClientUpdating = true;
		ON_SCOPE_EXIT { F.Character->bClientUpdating = bWasClientUpdating; };
		TestFalse(TEXT("移动回放即使没有ReplayState也拒绝更新"), F.Rotation->UpdateMontageWarpFacing(Handle, F.Ability, F.Execution, 50.f));
	}
	TestTrue(TEXT("拒绝输入保持当前Yaw"), FMath::IsNearlyEqual(F.Rotation->GetResolvedState().ActionYaw, LocalBefore.ActionYaw));
	for (int32 Mismatch = 0; Mismatch < 4; ++Mismatch)
	{
		auto View = F.Rotation->GetResolvedState(); View.ActionYaw = -20.f;
		if (Mismatch == 0) { View.ActionRequestId = FGuid::NewGuid(); }
		else if (Mismatch == 1) { ++View.ActionPredictionKey; }
		else if (Mismatch == 2) { View.Driver = EHodgeCharacterFacingDriver::Movement; }
		else { View.WarpDefinition = HodgeAttackWarpTests::Definition(); }
		FHodgeAttackWarpTestAccess::SetServerView(F.Rotation, View);
		TestTrue(TEXT("当前请求仍可更新自己的本地Yaw"), F.Rotation->UpdateMontageWarpFacing(Handle, F.Ability, F.Execution, 40.f + Mismatch));
		const auto Protected = F.Rotation->GetResolvedState();
		TestTrue(TEXT("不同身份服务器视图不被整份覆盖"), FHodgeCharacterRotationState::StaticStruct()->CompareScriptStruct(&View, &Protected, PPF_None));
		F.Rotation->EndServerMove();
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpSavedMoveYawTest, "Hodge.AttackWarp.LockTracking.SavedMoveConsumesEvaluatedYaw",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 真实SavedMove捕获起始状态，在本次Warp求值之后只补录实际采用Yaw。 */
bool FHodgeAttackWarpSavedMoveYawTest::RunTest(const FString&)
{
	HodgeAttackWarpTests::FTrackingFixture F;
	if (!TestTrue(TEXT("SavedMove夹具启动"), F.Start())) { return false; }
	auto* Movement = CastChecked<UHodgeCharacterMovementComponent>(F.Character->GetCharacterMovement());
	auto* Client = static_cast<FNetworkPredictionData_Client_Character*>(Movement->GetPredictionData_Client());
	const auto Move = Client->AllocateNewMove(); Move->Clear();
	const auto Initial = F.Rotation->GetResolvedState();
	Move->SetMoveFor(F.Character, .02f, FVector(12.f, 34.f, 0.f), *Client);
	FHodgeAttackWarpTestAccess::BeginEvaluation(Movement);
	F.MoveTarget(40.f); F.SetPosition(.03f); F.Ability->UpdateAttackFacingLifecycle();
	F.Warp->PrepareAttackTarget(F.Rotation->GetResolvedState(), F.Data->ExecutionConfig.Montage, .03f);
	FHodgeAttackWarpTestAccess::EndEvaluation(Movement);
	Move->PostUpdate(F.Character, FSavedMove_Character::PostUpdate_Record);
	TestNotNull(TEXT("PostUpdate捕获真实根运动Montage"), Move->RootMotionMontage.Get());
	const auto Saved = HodgeAttackWarpTests::SavedRotation(*Move, F);
	TestTrue(TEXT("保存本次Warp实际采用Yaw而非SetMoveFor旧Yaw"), FMath::IsNearlyEqual(Saved.ActionYaw, 40.f, .01f));
	auto Expected = Initial; Expected.ActionYaw = Saved.ActionYaw;
	TestTrue(TEXT("回填未覆盖起始状态其他字段"), FHodgeCharacterRotationState::StaticStruct()->CompareScriptStruct(&Expected, &Saved, PPF_None));
	const auto Acceleration = Move->Acceleration;
	F.MoveTarget(-45.f); F.SetPosition(.04f); F.Ability->UpdateAttackFacingLifecycle();
	Move->PostUpdate(F.Character, FSavedMove_Character::PostUpdate_Replay);
	const auto Replayed = HodgeAttackWarpTests::SavedRotation(*Move, F);
	TestTrue(TEXT("Replay不重录当前目标方向"), FHodgeCharacterRotationState::StaticStruct()->CompareScriptStruct(&Saved, &Replayed, PPF_None));
	TestTrue(TEXT("Yaw回填不改移动加速度"), Move->Acceleration.Equals(Acceleration));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpSavedMoveGuardsTest, "Hodge.AttackWarp.LockTracking.SavedMoveIdentityGuards",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 无同一步匹配求值时保留原快照，旧刀与回放不能借PostUpdate写入新刀输入。 */
bool FHodgeAttackWarpSavedMoveGuardsTest::RunTest(const FString&)
{
	const TCHAR* Cases[] = {TEXT("NoEvaluation"), TEXT("Disabled"), TEXT("WrongRequest"), TEXT("WrongSpec"), TEXT("WrongKey"),
		TEXT("WrongDefinition"), TEXT("WrongMontage"), TEXT("WrongStep"), TEXT("OldAvatar"), TEXT("Replay"), TEXT("Ambiguous")};
	for (const TCHAR* Case : Cases)
	{
		HodgeAttackWarpTests::FTrackingFixture F;
		if (!TestTrue(FString(Case) + TEXT("保护夹具启动"), F.Start())) { return false; }
		auto* Movement = CastChecked<UHodgeCharacterMovementComponent>(F.Character->GetCharacterMovement());
		auto* Client = static_cast<FNetworkPredictionData_Client_Character*>(Movement->GetPredictionData_Client());
		const auto Move = Client->AllocateNewMove(); Move->Clear();
		const auto Initial = F.Rotation->GetResolvedState();
		Move->SetMoveFor(F.Character, .02f, FVector::ZeroVector, *Client);
		auto Evaluated = Initial; Evaluated.ActionYaw = 35.f;
		const UAnimMontage* EvaluatedMontage = F.Data->ExecutionConfig.Montage;
		if (FCString::Strcmp(Case, TEXT("Disabled")) == 0) { F.Data->FacingWarp.bTrackExecutionLockTargetDuringWarp = false; }
		else if (FCString::Strcmp(Case, TEXT("WrongRequest")) == 0) { Evaluated.ActionRequestId = FGuid::NewGuid(); }
		else if (FCString::Strcmp(Case, TEXT("WrongSpec")) == 0)
		{
			// 通过真实 ASC 生成另一份 Spec，避免调用本机未导出的句柄生成函数。
			Evaluated.ActionAbilityHandle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeAttackWarpTestAbility::StaticClass(), 1));
		}
		else if (FCString::Strcmp(Case, TEXT("WrongKey")) == 0) { ++Evaluated.ActionPredictionKey; }
		else if (FCString::Strcmp(Case, TEXT("WrongDefinition")) == 0)
		{
			auto* OtherDefinition = HodgeAttackWarpTests::Definition(); OtherDefinition->FacingWarp.bTrackExecutionLockTargetDuringWarp = true;
			Evaluated.WarpDefinition = OtherDefinition; EvaluatedMontage = OtherDefinition->ExecutionConfig.Montage;
		}
		else if (FCString::Strcmp(Case, TEXT("WrongMontage")) == 0) { EvaluatedMontage = NewObject<UAnimMontage>(); }
		if (FCString::Strcmp(Case, TEXT("Replay")) == 0) { F.Character->bClientUpdating = true; }
		FHodgeAttackWarpTestAccess::BeginEvaluation(Movement);
		if (FCString::Strcmp(Case, TEXT("NoEvaluation")) != 0) { Movement->RecordAttackWarpEvaluation(Evaluated, EvaluatedMontage); }
		if (FCString::Strcmp(Case, TEXT("Ambiguous")) == 0)
		{ Evaluated.ActionRequestId = FGuid::NewGuid(); Movement->RecordAttackWarpEvaluation(Evaluated, EvaluatedMontage); }
		FHodgeAttackWarpTestAccess::EndEvaluation(Movement);
		F.Character->bClientUpdating = false;
		if (FCString::Strcmp(Case, TEXT("WrongStep")) == 0)
		{ FHodgeAttackWarpTestAccess::BeginEvaluation(Movement); Movement->RecordAttackWarpEvaluation(Evaluated, EvaluatedMontage); FHodgeAttackWarpTestAccess::EndEvaluation(Movement); }
		if (FCString::Strcmp(Case, TEXT("OldAvatar")) == 0)
		{ F.Rotation->UninitializeFromAbilitySystem(); F.Rotation->InitializeWithAbilitySystem(F.ASC); }
		Move->PostUpdate(F.Character, FSavedMove_Character::PostUpdate_Record);
		const auto Saved = HodgeAttackWarpTests::SavedRotation(*Move, F);
		TestTrue(FString(Case) + TEXT("保留原SavedMove全部朝向字段"), FHodgeCharacterRotationState::StaticStruct()->CompareScriptStruct(&Initial, &Saved, PPF_None));
	}
	return true;
}
#endif
