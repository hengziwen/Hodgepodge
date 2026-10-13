/** @file HodgeLockOnTests.cpp @brief LOCK-01～07 的输入、权威校验、纠正及生命周期自动化。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "HodgeEnemyAITestSupport.h"
#include "HodgeLockOnTestTypes.h"
#include "Targeting/HodgeLockOnTypes.h"
#include "Component/HodgeLockOnComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Component/HodgeHealthComponent.h"
#include "Data/HodgeLockOnProfile.h"
#include "Data/HodgePawnData.h"
#include "Character/HodgeHeroCharacter.h"
#include "Character/HodgeEnemyCharacter.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "Combat/HodgeDamageRules.h"
#include "GameFramework/PlayerController.h"
#include "Camera/HodgeCameraComponent.h"
#include "Camera/HodgeCameraMode_LockOn.h"
#include "Camera/HodgeCameraMode_ThirdPerson.h"
#include "Component/HodgeHeroComponent.h"
#include <limits>

/** 测试仅绕过资产装配；真实状态推进、资格和资源方法仍使用生产代码。 */
struct FHodgeLockOnTestAccess
{
	static void Bind(UHodgeLockOnComponent* Lock, UHodgeAbilitySystemComponent* ASC, UHodgeLockOnProfile* Profile)
	{
		Lock->Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(Lock->GetOwner());
		Lock->Rotation = Lock->GetOwner()->FindComponentByClass<UHodgeCharacterRotationComponent>();
		Lock->Rotation->InitializeWithAbilitySystem(ASC);
		Lock->InitializeFromExtension();
		// 使用真实协调入口建立 Spawned，避免测试手动推进状态而掩盖生产缺口。
		Lock->CheckDefaultInitialization();
	}
	static void Pending(UHodgeLockOnComponent* Lock, AActor* Target, int32 Seq)
	{ Lock->bHasPending = true; Lock->Pending.CandidateActor = Target; Lock->Pending.RequestSequence = Seq; }
	static bool HasPending(UHodgeLockOnComponent* Lock) { return Lock->bHasPending; }
	static void Result(UHodgeLockOnComponent* Lock, FHodgeLockOnState State) { Lock->ApplyResult(State); }
	static void Check(UHodgeLockOnComponent* Lock, double Now) { Lock->CheckTargetValidity(Now); }
	/** 模拟 Timer 在截止帧先于组件的清理检查结束命令。 */
	static void CancelAfterDeadline(UHodgeLockOnComponent* Lock)
	{
		Lock->PendingAt = Lock->GetWorld()->GetTimeSeconds()-Lock->Profile->CommandTimeout-.1;
		Lock->NotifyCommandEnded(Lock->Pending.RequestSequence,true);
	}
};
namespace HodgeLockOnTests
{
	/** 临时世界中的真实 Hero、Enemy ASC 与控制器，所有资源随场景销毁。 */
	struct FFixture
	{
		// 无 BeginPlay 的单元世界仍需执行目标的 BlueprintNativeEvent；退出夹具后恢复全局开关。
		TGuardValue<bool> ScriptGuard{GAllowActorScriptExecutionInEditor,true};
		UGameInstance* Instance = nullptr;
		UWorld* World;
		AHodgeHeroCharacter* Hero;
		AHodgeEnemyCharacter* Enemy;
		APlayerController* PC;
		UHodgeAbilitySystemComponent* ASC;
		UHodgeLockOnComponent* Lock;
		UHodgeLockOnProfile* Profile;
		FFixture()
		{
			World = FHodgeEnemyInitTestAccess::CreateWorld(Instance);
			Hero = World->SpawnActor<AHodgeHeroCharacter>(); PC = World->SpawnActor<APlayerController>(); PC->Possess(Hero);
			ASC = NewObject<UHodgeAbilitySystemComponent>(Hero); ASC->RegisterComponent();
			auto* Health = NewObject<UHodgeHealthSet>(Hero); ASC->AddAttributeSetSubobject(Health); Health->InitMaxHealth(100.f); Health->InitHealth(100.f);
			auto* Data = NewObject<UHodgePawnData>(); Profile = NewObject<UHodgeLockOnProfile>(Data); Data->LockOnProfile = Profile;
			auto* Extension = Hero->FindComponentByClass<UHodgePawnExtensionComponent>(); Extension->SetPawnData(Data); Extension->InitializeAbilitySystem(ASC,Hero);
			Lock = Hero->FindComponentByClass<UHodgeLockOnComponent>();
			FHodgeLockOnTestAccess::Bind(Lock,ASC,Profile);
			Enemy = World->SpawnActor<AHodgeEnemyCharacter>(FVector(500.f,0.f,100.f),FRotator::ZeroRotator);
			auto* EnemyASC = Enemy->GetNativeEnemyAbilitySystem(); EnemyASC->InitAbilityActorInfo(Enemy,Enemy);
			if (!EnemyASC->GetSet<UHodgeHealthSet>()) { EnemyASC->AddAttributeSetSubobject(NewObject<UHodgeHealthSet>(Enemy)); }
			EnemyASC->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(),100.f);
			EnemyASC->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(),100.f);
			Enemy->FindComponentByClass<UHodgeHealthComponent>()->InitializeWithAbilitySystem(EnemyASC);
		}
		~FFixture() { Lock->Uninitialize(); FHodgeEnemyInitTestAccess::DestroyWorld(Instance,World); }
		FHodgeLockOnTargetData Command(int32 Sequence, AActor* Target = nullptr)
		{
			FHodgeLockOnTargetData Data; Data.SourceAvatar = Hero; Data.CandidateActor = Target ? Target : Enemy;
			Data.RequestSequence = Sequence; Data.BaseStateVersion = Lock->GetConfirmedState().StateVersion; return Data;
		}
	};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnGestureTest,"Hodge.LockOn.MouseGesture",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnGestureTest::RunTest(const FString&)
{
	const auto* Profile = GetDefault<UHodgeLockOnProfile>(); FHodgeLockOnGesture Gesture;
	TestEqual(TEXT("小幅鼠标不切换"),Gesture.Sample({2.,0.},0.,*Profile),0);
	TestEqual(TEXT("快速向右甩动"),Gesture.Sample({20.,0.},.06,*Profile),1);
	TestEqual(TEXT("同一持续甩动不能连续切"),Gesture.Sample({20.,0.},.30,*Profile),0);
	TestEqual(TEXT("无输入使手势重新武装"),Gesture.Sample({},.40,*Profile),0);
	TestEqual(TEXT("反向甩动"),Gesture.Sample({-20.,0.},.45,*Profile),-1);
	Gesture.Reset(); Gesture.Sample({14.,0.},0.,*Profile); Gesture.Sample({-14.,0.},.04,*Profile);
	TestEqual(TEXT("来回晃动不满足方向一致率"),Gesture.Sample({18.,0.},.08,*Profile),0);
	Gesture.Reset(); Gesture.Sample({10.,0.},0.,*Profile);
	TestEqual(TEXT("低速累计不越过滚动窗口"),Gesture.Sample({10.,0.},.2,*Profile),0);
	TestEqual(TEXT("非法输入拒绝"),Gesture.Sample({std::numeric_limits<double>::quiet_NaN(),0.},.3,*Profile),0);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnProfileTest,"Hodge.LockOn.ProfileValidation",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnProfileTest::RunTest(const FString&)
{
	auto* Profile = NewObject<UHodgeLockOnProfile>(); FString Error;
	TestTrue(TEXT("原型配置完整"),Profile->Validate(Error));
	Profile->KeepDistance = 100.f; TestFalse(TEXT("保持范围小于获取范围拒绝"),Profile->Validate(Error)); Profile->KeepDistance = 2000.f;
	Profile->GestureWindow = 0.f; TestFalse(TEXT("零手势窗口拒绝"),Profile->Validate(Error)); Profile->GestureWindow = .12f;
	Profile->CameraPitchSpeed = std::numeric_limits<float>::infinity(); TestFalse(TEXT("无限速度拒绝"),Profile->Validate(Error)); Profile->CameraPitchSpeed = 120.f;
	Profile->PitchLimits = {60.,-50.}; TestFalse(TEXT("颠倒镜头范围拒绝"),Profile->Validate(Error)); Profile->PitchLimits = {-55.,55.};
	Profile->CameraManualHold = -1.f; TestFalse(TEXT("负停手宽限拒绝"),Profile->Validate(Error)); Profile->CameraManualHold = .75f;
	Profile->CameraScreenSoftZone = {1.1,.65}; TestFalse(TEXT("屏幕软区越界拒绝"),Profile->Validate(Error)); Profile->CameraScreenSoftZone = {.6,.65};
	Profile->CameraAssistRecoveryTime = 0.f; TestFalse(TEXT("零恢复时间拒绝"),Profile->Validate(Error)); Profile->CameraAssistRecoveryTime = .35f;
	Profile->CameraRecenterSharpness = 0.f; Profile->CameraYawDeadZone = 81.f; TestTrue(TEXT("退休字段不影响新配置资格"),Profile->Validate(Error));
	Profile->CameraScreenDeadZone = {.7,.16}; TestFalse(TEXT("软区小于死区拒绝"),Profile->Validate(Error)); Profile->CameraScreenDeadZone = {.12,.16};
	Profile->CameraAcquireDuration = 0.f; TestFalse(TEXT("零获取时长拒绝"),Profile->Validate(Error)); Profile->CameraAcquireDuration=.35f;
	Profile->CameraMouseSuppression = 1.1f; TestFalse(TEXT("抑制系数越界拒绝"),Profile->Validate(Error)); Profile->CameraMouseSuppression=1.f;
	Profile->CameraMode = UHodgeCameraMode_ThirdPerson::StaticClass(); TestFalse(TEXT("不支持的抽象镜头配置拒绝"),Profile->Validate(Error));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnAuthorityTest,"Hodge.LockOn.AuthorityIdentityAndVersions",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnAuthorityTest::RunTest(const FString&)
{
	HodgeLockOnTests::FFixture F; TestTrue(TEXT("独立 Feature 就绪"),F.Lock->IsReady());
	TestTrue(TEXT("敌人接口资格"),F.Enemy->CanBeLockedBy_Implementation(F.Hero));
	TestTrue(TEXT("反射接口存在"),F.Enemy->Implements<UHodgeLockOnTargetInterface>());
	TestTrue(TEXT("反射资格存在"),IHodgeLockOnTargetInterface::Execute_CanBeLockedBy(F.Enemy,F.Hero));
	TestTrue(TEXT("真实伤害资格"),FHodgeDamageRules::CanDamage(F.Hero,F.Enemy,false));
	FVector Anchor; TestTrue(TEXT("默认锚点有有效 Bounds"),F.Enemy->GetLockPoint_Implementation(NAME_None,Anchor));
	TestTrue(TEXT("反射锚点存在"),IHodgeLockOnTargetInterface::Execute_GetLockPoint(F.Enemy,NAME_None,Anchor));
	auto Data = F.Command(1); F.Lock->ProcessAuthorityCommand(Data);
	TestEqual(TEXT("合法目标确认"),F.Lock->GetConfirmedTarget(),static_cast<AActor*>(F.Enemy));
	TestTrue(TEXT("服务器 GE 授予状态"),F.ASC->HasMatchingGameplayTag(HodgeLockOnTags::Locked));
	const int32 Version = F.Lock->GetConfirmedState().StateVersion;
	const int32 Revision = F.Lock->GetPresentationRevision();
	F.Lock->ProcessAuthorityCommand(Data); TestEqual(TEXT("重复数据不推进版本"),F.Lock->GetConfirmedState().StateVersion,Version);
	TestEqual(TEXT("重复回执不触发表现修订"),F.Lock->GetPresentationRevision(),Revision);
	Data = F.Command(2); Data.SourceAvatar = F.Enemy; F.Lock->ProcessAuthorityCommand(Data);
	TestEqual(TEXT("错误 Avatar 不修改序号"),F.Lock->GetConfirmedState().RequestSequence,1);
	Data = F.Command(2); Data.BaseStateVersion = 0; F.Lock->ProcessAuthorityCommand(Data);
	TestEqual(TEXT("业务拒绝仍确认序号"),F.Lock->GetConfirmedState().RequestSequence,2);
	TestEqual(TEXT("版本拒绝原因"),F.Lock->GetConfirmedState().Reason,EHodgeLockOnReason::StaleVersion);
	TestEqual(TEXT("拒绝保留原目标"),F.Lock->GetConfirmedTarget(),static_cast<AActor*>(F.Enemy));
	Data = F.Command(3); Data.Operation = EHodgeLockOnOperation::Clear; Data.CandidateActor.Reset(); F.Lock->ProcessAuthorityCommand(Data);
	TestNull(TEXT("显式解除"),F.Lock->GetConfirmedTarget()); TestFalse(TEXT("解除只撤销自身 GE"),F.ASC->HasMatchingGameplayTag(HodgeLockOnTags::Locked));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnRecoveryTest,"Hodge.LockOn.PredictionReconciliation",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnRecoveryTest::RunTest(const FString&)
{
	HodgeLockOnTests::FFixture F;
	FHodgeLockOnTestAccess::Pending(F.Lock,F.Enemy,5);
	FHodgeLockOnState Old; Old.StateVersion = 1; Old.RequestSequence = 4; Old.Reason = EHodgeLockOnReason::InvalidTarget;
	FHodgeLockOnTestAccess::Result(F.Lock,Old); TestTrue(TEXT("旧拒绝保留新预测"),FHodgeLockOnTestAccess::HasPending(F.Lock));
	F.Lock->RejectPrediction(5,F.Lock->GetGeneration()+1); TestTrue(TEXT("旧初始化代拒绝忽略"),FHodgeLockOnTestAccess::HasPending(F.Lock));
	Old.StateVersion = 2; Old.RequestSequence = 5; FHodgeLockOnTestAccess::Result(F.Lock,Old);
	TestFalse(TEXT("匹配回执结束预测"),FHodgeLockOnTestAccess::HasPending(F.Lock));
	FHodgeLockOnTestAccess::Pending(F.Lock,F.Enemy,6); F.Lock->RejectPrediction(6,F.Lock->GetGeneration());
	TestFalse(TEXT("GAS 拒绝回收匹配资源"),FHodgeLockOnTestAccess::HasPending(F.Lock));
	TestEqual(TEXT("GAS 拒绝诊断"),F.Lock->GetLastLocalReason(),EHodgeLockOnReason::ActivationFailed);
	F.Lock->ProcessAuthorityCommand(F.Command(7));
	FHodgeLockOnTestAccess::Pending(F.Lock,F.Enemy,8); FHodgeLockOnTestAccess::CancelAfterDeadline(F.Lock);
	TestFalse(TEXT("截止帧结束回收预测"),FHodgeLockOnTestAccess::HasPending(F.Lock));
	TestEqual(TEXT("Timer 截止帧保持超时诊断"),F.Lock->GetLastLocalReason(),EHodgeLockOnReason::Timeout);
	TestEqual(TEXT("超时不清此前确认目标"),F.Lock->GetConfirmedTarget(),static_cast<AActor*>(F.Enemy));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnMotionTest,"Hodge.LockOn.ControlAndMotionLifecycle",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnMotionTest::RunTest(const FString&)
{
	HodgeLockOnTests::FFixture F; F.Lock->ProcessAuthorityCommand(F.Command(1));
	auto* Rotation = F.Hero->GetCharacterRotationComponent();
	TestEqual(TEXT("普通锁定进入八向基础风格"),Rotation->GetResolvedState().Style,EHodgeLocomotionStyle::ReservedStrafe);
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Controlled); F.Lock->RefreshFacing();
	TestEqual(TEXT("强控制保留目标"),F.Lock->GetConfirmedTarget(),static_cast<AActor*>(F.Enemy));
	TestEqual(TEXT("强控制让出 Controller"),Rotation->GetResolvedState().BaseDriver,EHodgeCharacterFacingDriver::Movement);
	F.ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Controlled); F.Lock->RefreshFacing();
	F.ASC->AddLooseGameplayTag(HodgeMovementTags::Sprinting); F.Lock->RefreshFacing();
	TestEqual(TEXT("Sprint 自由风格"),Rotation->GetResolvedState().Style,EHodgeLocomotionStyle::FreeDirectional);
	F.ASC->RemoveLooseGameplayTag(HodgeMovementTags::Sprinting); F.Lock->RefreshFacing();
	TestEqual(TEXT("运动结束恢复八向"),Rotation->GetResolvedState().Style,EHodgeLocomotionStyle::ReservedStrafe);
	F.Lock->Uninitialize(); F.Lock->Uninitialize();
	TestFalse(TEXT("重复解绑不保留就绪"),F.Lock->IsReady()); TestNull(TEXT("解绑清目标"),F.Lock->GetConfirmedTarget());
	TestFalse(TEXT("解绑移除自己的状态 GE"),F.ASC->HasMatchingGameplayTag(HodgeLockOnTags::Locked));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnRangeTest,"Hodge.LockOn.RangeGraceAndInvalidTarget",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnRangeTest::RunTest(const FString&)
{
	HodgeLockOnTests::FFixture F; F.Enemy->SetActorLocation({2500.f,0.f,100.f});
	F.Lock->ProcessAuthorityCommand(F.Command(1)); TestFalse(TEXT("超获取范围拒绝"),F.Lock->GetConfirmedState().bAccepted);
	F.Enemy->SetActorLocation({500.f,0.f,100.f}); F.Lock->ProcessAuthorityCommand(F.Command(2));
	F.Enemy->SetActorLocation({2200.f,0.f,100.f}); FHodgeLockOnTestAccess::Check(F.Lock,0.);
	TestNotNull(TEXT("保持超距宽限内不解除"),F.Lock->GetConfirmedTarget()); FHodgeLockOnTestAccess::Check(F.Lock,.2);
	TestNotNull(TEXT("短超距保留"),F.Lock->GetConfirmedTarget()); F.Enemy->SetActorLocation({500.f,0.f,100.f}); FHodgeLockOnTestAccess::Check(F.Lock,.25);
	F.Enemy->SetActorLocation({2200.f,0.f,100.f}); FHodgeLockOnTestAccess::Check(F.Lock,.3); FHodgeLockOnTestAccess::Check(F.Lock,.7);
	TestNull(TEXT("持续超距解除"),F.Lock->GetConfirmedTarget());
	F.Enemy->SetActorLocation({500.f,0.f,100.f}); F.Enemy->bAllowPlayerLockOn = false;
	F.Lock->ProcessAuthorityCommand(F.Command(3)); TestNull(TEXT("目标契约拒绝"),F.Lock->GetConfirmedTarget());
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnGASTest,"Hodge.LockOn.GASShortCommand",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnGASTest::RunTest(const FString&)
{
	HodgeLockOnTests::FFixture F; const auto Handle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_LockOn::StaticClass(),1));
	TestTrue(TEXT("获取通过真实 GAS 短命令"),F.Lock->TryLockTarget(F.Enemy));
	TestEqual(TEXT("命令处理后目标持久"),F.Lock->GetConfirmedTarget(),static_cast<AActor*>(F.Enemy));
	TestFalse(TEXT("短命令已经结束"),F.ASC->FindAbilitySpecFromHandle(Handle)->IsActive());
	TestTrue(TEXT("再次按中键的解除语义"),F.Lock->ToggleLock()); TestNull(TEXT("解除完成"),F.Lock->GetConfirmedTarget());
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnDeathTest,"Hodge.LockOn.DeathClearsAndDoesNotReacquire",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnDeathTest::RunTest(const FString&)
{
	HodgeLockOnTests::FFixture F; F.Lock->ProcessAuthorityCommand(F.Command(1));
	F.Enemy->FindComponentByClass<UHodgeHealthComponent>()->StartDeath();
	TestNull(TEXT("目标死亡同步解除"),F.Lock->GetConfirmedTarget());
	TestFalse(TEXT("目标死亡回收锁定 GE"),F.ASC->HasMatchingGameplayTag(HodgeLockOnTags::Locked));
	TestEqual(TEXT("目标死亡原因"),F.Lock->GetConfirmedState().Reason,EHodgeLockOnReason::Death);
	F.Lock->ProcessAuthorityCommand(F.Command(2)); TestFalse(TEXT("不允许重新获取死亡目标"),F.Lock->GetConfirmedState().bAccepted);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	TestNull(TEXT("玩家死亡保持解除"),F.Lock->GetConfirmedTarget()); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnCacheTest,"Hodge.LockOn.TargetDataEarlyAndRetired",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnCacheTest::RunTest(const FString&)
{
	HodgeLockOnTests::FFixture F; const auto Handle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_LockOn::StaticClass(),1));
	FPredictionKey Key; Key.Current = 4; auto Data = FGameplayAbilityTargetDataHandle(new FHodgeLockOnTargetData(F.Command(1)));
	F.ASC->ServerSetReplicatedTargetData_Implementation(Handle,Key,Data,{},Key);
	TestTrue(TEXT("数据先到可补偿消费"),F.ASC->CallReplicatedTargetDataDelegatesIfSet(Handle,Key));
	F.ASC->ConsumeClientReplicatedTargetData(Handle,Key); F.ASC->RetireLockOnCommand(Handle,Key);
	F.ASC->ServerSetReplicatedTargetData_Implementation(Handle,Key,Data,{},Key);
	TestFalse(TEXT("退役激活的迟到数据不重新入缓存"),F.ASC->CallReplicatedTargetDataDelegatesIfSet(Handle,Key));
	FPredictionKey Other; Other.Current = 5; auto Invalid = F.Command(2); Invalid.SourceAvatar = F.Enemy;
	F.ASC->ServerSetReplicatedTargetData_Implementation(Handle,Other,FGameplayAbilityTargetDataHandle(new FHodgeLockOnTargetData(Invalid)),{},Other);
	TestFalse(TEXT("旧 Avatar 载荷不进入缓存"),F.ASC->CallReplicatedTargetDataDelegatesIfSet(Handle,Other));
	return true;
}
namespace HodgeLockCameraTests
{
	/** 用纯几何帧验证算法，不依赖视口 Tick 或网络时间。 */
	FHodgeLockCameraFrame Frame(FVector Target = FVector(1000.,0.,0.), float Aspect = 16.f/9.f)
	{ FHodgeLockCameraFrame Result; Result.TargetLocation = Target; Result.AspectRatio = Aspect; return Result; }
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeSoftLockCameraTest,"Hodge.LockOn.SoftCameraManualAndAssist",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeSoftLockCameraTest::RunTest(const FString&)
{
	auto* Profile = NewObject<UHodgeLockOnProfile>(); FHodgeSoftLockCamera Camera;
	auto Frame = HodgeLockCameraTests::Frame(); Frame.ViewRotation = FRotator(0.,170.,0.);
	Profile->CameraYawSpeed = 90.f; Profile->CameraPitchSpeed = 60.f;
	Camera.Seed(FRotator(0.,170.,0.),0.); Camera.BeginAcquire(); Camera.AddLook({40.,15.},.01);
	FRotator View = Camera.Update(Frame,1.f/60.f,.01,*Profile,true);
	TestTrue(TEXT("输入直接生效并中止获取"),FMath::IsNearlyEqual(View.Yaw,-150.,.001) && !Camera.IsAcquiring());
	TestTrue(TEXT("俯仰直接应用输入"),FMath::IsNearlyEqual(View.Pitch,15.,.001));
	View = Camera.Update(Frame,.05f,.1,*Profile,true);
	TestTrue(TEXT("手动宽限不自动拉回"),FMath::IsNearlyEqual(View.Yaw,-150.,.001));
	View = Camera.Update(Frame,.05f,1.,*Profile,true);
	TestTrue(TEXT("自动修正受角速度上限约束"),View.Yaw>-150. && View.Yaw<=-145.5+.001);
	Profile->bEnableSoftCameraAssist = false; Camera.AddLook({-130.,500.},1.);
	View = Camera.Update(Frame,.05f,2.,*Profile,true);
	TestTrue(TEXT("关闭辅助仍可自由旋转"),View.Yaw>79. && View.Yaw<=84.51);
	TestTrue(TEXT("俯仰遵守安全范围"),FMath::IsNearlyEqual(View.Pitch,Profile->PitchLimits.Y,.001));
	Camera.AddLook({std::numeric_limits<double>::infinity(),0.},3.);
	TestFalse(TEXT("非法输入不污染状态"),Camera.Update(Frame,.05f,3.,*Profile,true).ContainsNaN());
	Profile->bEnableSoftCameraAssist = true; Camera.Seed(FRotator(0.,100.,0.),0.);
	TestTrue(TEXT("临时模式不自动构图"),FMath::IsNearlyEqual(Camera.Update(Frame,.05f,1.,*Profile,false).Yaw,100.,.001));
	Camera.AddLook({25.,0.},2.); Camera.Seed(FRotator(0.,125.,0.),3.);
	TestTrue(TEXT("晋升不重复应用旧输入"),FMath::IsNearlyEqual(Camera.Update(Frame,.01f,3.,*Profile,false).Yaw,125.,.001));
	Camera.AddLook({6.,0.},4.); Camera.Seed(FRotator(0.,125.,0.),4.);
	TestTrue(TEXT("晋升保留同帧输入"),FMath::IsNearlyEqual(Camera.Update(Frame,.01f,4.,*Profile,false).Yaw,131.,.001));
	Camera.BeginAcquire(); Camera.Update(Frame,.01f,5.,*Profile,true); Camera.Seed(FRotator(0.,60.,0.),6.);
	TestTrue(TEXT("中途技能镜头返回不续播旧获取"),!Camera.IsAcquiring() && FMath::IsNearlyEqual(Camera.Update(Frame,.01f,6.,*Profile,true).Yaw,60.,.001));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockCameraAcquireTest,"Hodge.LockOn.CameraAcquireEase",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockCameraAcquireTest::RunTest(const FString&)
{
	auto* Profile = NewObject<UHodgeLockOnProfile>(); FHodgeSoftLockCamera Camera;
	Camera.Seed(FRotator(0.,150.,0.),0.); Camera.BeginAcquire();
	auto Frame = HodgeLockCameraTests::Frame(); Frame.ViewRotation = FRotator(0.,150.,0.);
	Frame.CameraLocation = Frame.ViewRotation.RotateVector(FVector(-350.,40.,50.));
	TArray<double> Steps; double Before=150.;
	for (int32 I=0; I<42; ++I)
	{
		const auto View=Camera.Update(Frame,1.f/120.f,(I+1)/120.,*Profile,true);
		Steps.Add(FMath::Abs(FMath::FindDeltaAngleDegrees(Before,View.Yaw))); Before=View.Yaw;
	}
	TestTrue(TEXT("获取在配置时间内结束"),!Camera.IsAcquiring());
	TestTrue(TEXT("缓入缓出而非匀速"),Steps[0]<Steps[20]*.2 && Steps.Last()<Steps[20]*.2);
	const FVector Offset=Frame.ViewRotation.UnrotateVector(Frame.CameraLocation);
	const FVector Ray=Frame.TargetLocation-Camera.GetRotation().RotateVector(Offset);
	TestTrue(TEXT("实际轨道镜头瞄准身体点"),FMath::Abs(FMath::FindDeltaAngleDegrees(Camera.GetRotation().Yaw,Ray.Rotation().Yaw))<.1);
	Camera.Seed(FRotator(0.,179.,0.),0.); Camera.BeginAcquire(); Frame=HodgeLockCameraTests::Frame(FRotator(0.,-179.,0.).Vector()*1000.);
	const auto View=Camera.Update(Frame,.01f,.01,*Profile,true);
	TestTrue(TEXT("跨180度取最短弧"),FMath::Abs(FMath::FindDeltaAngleDegrees(179.,View.Yaw))<.1);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockCameraTrackingTest,"Hodge.LockOn.CameraScreenZones",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockCameraTrackingTest::RunTest(const FString&)
{
	auto* Profile = NewObject<UHodgeLockOnProfile>(); FHodgeSoftLockCamera Camera;
	Camera.Seed({},0.); auto Frame=HodgeLockCameraTests::Frame(FVector(1000.,40.,20.));
	TestTrue(TEXT("中心区稳定不微调"),Camera.Update(Frame,.016f,1.,*Profile,true).IsNearlyZero());
	Frame.TargetLocation=FVector(1000.,250.,0.); const double Moderate=Camera.Update(Frame,.016f,1.,*Profile,true).Yaw;
	Camera.Seed({},0.); Frame.TargetLocation=FVector(1000.,900.,0.); const double Strong=Camera.Update(Frame,.016f,1.,*Profile,true).Yaw;
	TestTrue(TEXT("偏离越大辅助越强"),Strong>Moderate && Moderate>0.);
	Camera.Seed({},0.); Frame.TargetLocation=FVector(1000.,0.,100.); Frame.AspectRatio=1.f;
	TestTrue(TEXT("方形视口垂直偏差在死区"),Camera.Update(Frame,.016f,1.,*Profile,true).IsNearlyZero());
	Camera.Seed({},0.); Frame.AspectRatio=2.f;
	TestTrue(TEXT("宽视口同一世界高度离开垂直死区"),Camera.Update(Frame,.016f,1.,*Profile,true).Pitch>0.);
	Camera.Seed({},0.); Frame.TargetLocation=FVector(0.,0.,500.);
	TestTrue(TEXT("目标重叠不旋转"),Camera.Update(Frame,.016f,1.,*Profile,true).IsNearlyZero());
	Frame.TargetLocation=FVector(1000.,900.,10000.);
	const auto Before=Camera.GetRotation(); const auto After=Camera.Update(Frame,2.f,2.,*Profile,true);
	TestTrue(TEXT("大时间步限制单帧速度且不翻转"),FMath::Abs(After.Yaw-Before.Yaw)<=Profile->CameraYawSpeed*.05+.01 && After.Pitch<=Profile->PitchLimits.Y);
	Frame.TargetLocation.X=std::numeric_limits<double>::quiet_NaN();
	TestTrue(TEXT("无效几何保持原视角"),Camera.Update(Frame,.016f,3.,*Profile,true).Equals(After));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockCameraRecoveryTest,"Hodge.LockOn.CameraInputRecoveryAndFrameRate",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockCameraRecoveryTest::RunTest(const FString&)
{
	auto* Profile=NewObject<UHodgeLockOnProfile>(); const auto Frame=HodgeLockCameraTests::Frame();
	FHodgeSoftLockCamera Early,Late; Early.Seed(FRotator(0.,45.,0.),0.); Late.Seed(FRotator(0.,45.,0.),0.);
	const double EarlyStep=45.-Early.Update(Frame,.01f,Profile->CameraManualHold+.02,*Profile,true).Yaw;
	const double LateStep=45.-Late.Update(Frame,.01f,1.,*Profile,true).Yaw;
	TestTrue(TEXT("停手后渐进恢复而非突然开启"),EarlyStep>0. && EarlyStep<LateStep*.1);
	Profile->CameraMouseSuppression=.5f; Early.Seed(FRotator(0.,45.,0.),0.);
	TestTrue(TEXT("抑制系数可调"),Early.Update(Frame,.01f,.01,*Profile,true).Yaw<45.);
	Profile->CameraMouseSuppression=1.f;
	FHodgeSoftLockCamera At30,At120; At30.Seed(FRotator(0.,45.,0.),0.); At120.Seed(FRotator(0.,45.,0.),0.);
	for(int32 I=0;I<30;++I) { At30.Update(Frame,1.f/30.f,1.+I/30.,*Profile,true); }
	for(int32 I=0;I<120;++I) { At120.Update(Frame,1.f/120.f,1.+I/120.,*Profile,true); }
	TestTrue(TEXT("不同帧率的跟随结果接近"),FMath::IsNearlyEqual(At30.GetRotation().Yaw,At120.GetRotation().Yaw,.5));
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeLockOnCameraStackTest,"Hodge.LockOn.CameraModePriorityAndContinuity",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeLockOnCameraStackTest::RunTest(const FString&)
{
	HodgeLockOnTests::FFixture F;
	auto* Data = const_cast<UHodgePawnData*>(UHodgePawnExtensionComponent::FindPawnExtensionComponent(F.Hero)->GetPawnData<UHodgePawnData>());
	Data->DefaultCameraMode = UHodgeCameraMode_LockOnTestFree::StaticClass(); F.Profile->CameraBlendTime = .4f;
	auto* Hero = F.Hero->FindComponentByClass<UHodgeHeroComponent>();
	auto* Camera = UHodgeCameraComponent::FindCameraComponent(F.Hero);
	TestNotNull(TEXT("正式 Hero 自带 CameraMode 组件"),Camera); if (!Camera || !Hero) { return false; }
	Camera->DetermineCameraModeDelegate.BindUObject(Hero,&UHodgeHeroComponent::GetDesiredCameraMode);
	F.PC->SetControlRotation(FRotator(5.,120.,0.)); FMinimalViewInfo View;
	static_cast<UCameraComponent*>(Camera)->GetCameraView(0.f,View);
	TestEqual(TEXT("自由状态选择默认模式"),Camera->GetActiveCameraModeClass().Get(),UHodgeCameraMode_LockOnTestFree::StaticClass());
	F.Lock->ProcessAuthorityCommand(F.Command(1));
	static_cast<UCameraComponent*>(Camera)->GetCameraView(0.f,View);
	TestEqual(TEXT("目标选择经实际模式栈进入锁定模式"),Camera->GetActiveCameraModeClass().Get(),UHodgeCameraMode_LockOn::StaticClass());
	TestTrue(TEXT("获取继承原画面而非角色朝向"),FMath::IsNearlyEqual(View.Rotation.Yaw,120.,.01));
	float Weight; FGameplayTag Tag; Camera->GetBlendInfo(Weight,Tag);
	TestTrue(TEXT("诊断读取真正栈顶初始混合权重"),FMath::IsNearlyZero(Weight));
	static_cast<UCameraComponent*>(Camera)->GetCameraView(.1f,View); Camera->GetBlendInfo(Weight,Tag);
	TestTrue(TEXT("配置的切换时间实际产生混合"),Weight>0.f && Weight<1.f);
	const auto Handle = F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_LockOn::StaticClass(),1));
	Hero->SetAbilityCameraMode(UHodgeCameraMode_LockOnTestFree::StaticClass(),Handle);
	static_cast<UCameraComponent*>(Camera)->GetCameraView(0.f,View);
	TestEqual(TEXT("技能临时模式高于锁定模式"),Camera->GetActiveCameraModeClass().Get(),UHodgeCameraMode_LockOnTestFree::StaticClass());
	const double BeforeResume = View.Rotation.Yaw; Hero->ClearAbilityCameraMode(Handle);
	static_cast<UCameraComponent*>(Camera)->GetCameraView(0.f,View);
	TestEqual(TEXT("结束技能恢复仍有效的锁定模式"),Camera->GetActiveCameraModeClass().Get(),UHodgeCameraMode_LockOn::StaticClass());
	TestTrue(TEXT("栈内再次晋升仍保持输出连续"),FMath::Abs(FMath::FindDeltaAngleDegrees(BeforeResume,View.Rotation.Yaw))<.01);
	const double ActorYaw = F.Hero->GetActorRotation().Yaw;
	F.Hero->FaceRotation(FRotator(0.,ActorYaw+90.,0.),.016f);
	TestTrue(TEXT("手动镜头不提前抢写锁定角色朝向"),FMath::IsNearlyEqual(F.Hero->GetActorRotation().Yaw,ActorYaw,.001));
	F.Lock->ClearLock(EHodgeLockOnReason::InvalidTarget); const double BeforeClear=View.Rotation.Yaw;
	static_cast<UCameraComponent*>(Camera)->GetCameraView(0.f,View);
	TestEqual(TEXT("解除选择恢复默认模式"),Camera->GetActiveCameraModeClass().Get(),UHodgeCameraMode_LockOnTestFree::StaticClass());
	TestTrue(TEXT("解除不跳回敌人方位"),FMath::Abs(FMath::FindDeltaAngleDegrees(BeforeClear,View.Rotation.Yaw))<.01);
	return true;
}
#endif
