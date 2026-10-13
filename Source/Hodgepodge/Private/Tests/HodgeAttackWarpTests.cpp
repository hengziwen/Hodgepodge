/** @file HodgeAttackWarpTests.cpp @brief AW-01～07 配置、许可、根运动执行和协议边界测试。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "HodgeAttackWarpTestTypes.h"
#include "HodgeEnemyAITestSupport.h"
#include "Combat/HodgeAttackFacingTypes.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgeAttackMotionWarpingComponent.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Character/HodgeCombatCharacter.h"
#include "Data/HodgeAbilityDefinition.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Animation/HodgeRootMotionModifier_AttackFacing.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "AnimNotifyState_MotionWarping.h"
#include "GameFramework/PlayerController.h"
#include <limits>

/** 测试活动身份由 GAS 建立，只补充本次执行的本地 GUID。 */
struct FHodgeAttackWarpTestAccess
{
	static bool AuthorityCurrent(UHodgeCharacterRotationComponent* Rotation, const FHodgeCharacterRotationState& State)
	{ Rotation->ReplicatedState=State; return Rotation->IsAuthorityActionCurrent(); }
	static void Release(UHodgeGameplayAbility_Definition* Ability, FHodgeFacingRequestHandle Handle)
	{ Ability->AttackFacingHandle=Handle; Ability->ReleaseAttackFacing(); }
	static bool Apply(UHodgeGameplayAbility_Definition* Ability, const FHodgeAttackFacingTargetData& Data) { return Ability->ApplyAttackFacing(Data); }
	static FGuid SetIdentity(UHodgeGameplayAbility_Definition* Ability)
	{ Ability->ExecutionId=FGuid::NewGuid(); return Ability->ExecutionId; }
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
		Data->FacingWarp.bEnabled=true; return Data;
	}
	/** 临时权威角色无需编辑器地图，不写入正式资产。 */
	struct FFixture
	{
		UGameInstance* Instance=nullptr; UWorld* World;
		AHodgeCombatCharacter* Character; APlayerController* Controller; UHodgeAbilitySystemComponent* ASC;
		UHodgeCharacterRotationComponent* Rotation; UHodgeAttackMotionWarpingComponent* Warp;
		UHodgeAbilityDefinition* Data; UHodgeGameplayAbility_Definition* Ability=nullptr; FGameplayAbilitySpecHandle Spec;
		FGuid Execution;
		FFixture()
		{
			World=FHodgeEnemyInitTestAccess::CreateWorld(Instance); Character=World->SpawnActor<AHodgeCombatCharacter>();
			Controller=World->SpawnActor<APlayerController>(); Controller->Possess(Character);
			ASC=NewObject<UHodgeAbilitySystemComponent>(Character); ASC->RegisterComponent(); ASC->InitAbilityActorInfo(Character,Character);
			Rotation=Character->GetCharacterRotationComponent(); Rotation->InitializeWithAbilitySystem(ASC);
			Warp=Character->FindComponentByClass<UHodgeAttackMotionWarpingComponent>(); if (!Warp->HasBeenInitialized()) { Warp->InitializeComponent(); }
			Data=Definition(); Spec=ASC->GiveAbilityDefinition(Data,1,Data);
			if (!Spec.IsValid() || !ASC->TryActivateAbility(Spec)) { return; }
			const auto* Granted=ASC->FindAbilitySpecFromHandle(Spec); Ability=Granted ? Cast<UHodgeGameplayAbility_Definition>(Granted->GetPrimaryInstance()) : nullptr;
			if (Ability) { Execution=FHodgeAttackWarpTestAccess::SetIdentity(Ability); }
		}
		~FFixture() { if (Ability && Ability->IsActive()) { Ability->FinishExecution(true,false); } World->DestroyWorld(false); }
	};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeAttackWarpConfigTest,"Hodge.AttackWarp.ConfigBounds",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeAttackWarpConfigTest::RunTest(const FString&)
{
	FHodgeAttackFacingConfig Config; FString Error; TestTrue(TEXT("默认关闭不改变既有技能"),Config.Validate(Error));
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
	const auto Handle=F.Rotation->RequestMontageWarpFacing(45.f,F.Ability,F.Execution); const auto OldAuthority=F.Rotation->GetResolvedState();
	TestTrue(TEXT("活动执行允许权威回退"),FHodgeAttackWarpTestAccess::AuthorityCurrent(F.Rotation,OldAuthority));
	FHodgeAttackWarpTestAccess::Release(F.Ability,Handle);
	TestFalse(TEXT("Recovery之后旧权威占用不复活"),FHodgeAttackWarpTestAccess::AuthorityCurrent(F.Rotation,OldAuthority));
	return true;
}
#endif
