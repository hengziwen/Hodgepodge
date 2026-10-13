/** @file HodgeWingTests.cpp
 * @brief 实现翅膀自动化测试相关的类型、接口与现有处理流程。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Animation/HodgeWingProfile.h"
#include "Component/HodgeWingComponent.h"
#include "Engine/SkeletalMesh.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWingEnvelopeTest, "Hodge.Wing.Envelope",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeWingEnvelopeTest::RunTest(const FString& Parameters)
{
	// 当前操作使用的配置对象。
	auto* Profile = NewObject<UHodgeWingProfile>();
	Profile->WindupPose.Root = FRotator(-30.f, 0.f, 0.f);
	Profile->StrikePose.Root = FRotator(65.f, 0.f, 0.f);
	Profile->WindupPose.Mid = FRotator(0.f, -12.f, 0.f);
	Profile->StrikePose.Tip = FRotator(0.f, 20.f, 0.f);
	TestTrue(TEXT("Ordered source times accepted"), Profile->HasValidTiming());
	TestTrue(TEXT("Before windup stays at reference"), Profile->Evaluate(0.f).Root.IsNearlyZero());
	TestTrue(TEXT("Windup key reached exactly"), Profile->Evaluate(Profile->StrikeStart).Root.Equals(Profile->WindupPose.Root));
	TestTrue(TEXT("Strike key reached exactly"), Profile->Evaluate(Profile->StrikePeak).Root.Equals(Profile->StrikePose.Root));
	TestTrue(TEXT("Natural recovery returns to reference"), Profile->Evaluate(Profile->RecoveryEnd).Root.IsNearlyZero());
	const FRotator Before = Profile->Evaluate(Profile->StrikeStart - .00001f).Root;
	const FRotator After = Profile->Evaluate(Profile->StrikeStart + .00001f).Root;
	TestTrue(TEXT("Key transition has no angular jump"), Before.Equals(After, .01f));
	Profile->StrikePeak = Profile->StrikeStart;
	TestFalse(TEXT("Zero interval rejected"), Profile->HasValidTiming());
	TestTrue(TEXT("Invalid timing fails closed"), Profile->Evaluate(.3f).Root.IsNearlyZero());
	Profile->StrikePeak = NAN;
	TestFalse(TEXT("Nonfinite times rejected"), Profile->HasValidTiming());
	TArray<FText> Errors;
	TestFalse(TEXT("Missing resources rejected"), Profile->Validate(Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWingCancellationTest, "Hodge.Wing.CancellationAndClock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeWingCancellationTest::RunTest(const FString& Parameters)
{
	auto* Wing = NewObject<UHodgeWingComponent>();
	Wing->Profile = NewObject<UHodgeWingProfile>(Wing);
	Wing->Profile->StrikePose.Root = FRotator(65.f, 0.f, 0.f);
	Wing->UpdatePose(Wing->Profile->StrikePeak, 10, true, .016f);
	TestEqual(TEXT("Montage instance identity retained"), Wing->SourceInstanceId, 10);
	const FRotator Peak = Wing->Pose.Root;
	Wing->UpdatePose(Wing->Profile->StrikePeak, 10, true, .5f);
	TestTrue(TEXT("Paused source clock does not advance pose"), Wing->Pose.Root.Equals(Peak));
	Wing->UpdatePose(0.f, INDEX_NONE, false, 0.f);
	TestTrue(TEXT("Cancel begins from current pose without snapping"), Wing->Pose.Root.Equals(Peak));
	Wing->UpdatePose(0.f, INDEX_NONE, false, Wing->Profile->CancelReturnSeconds * .5f);
	TestTrue(TEXT("Cancel interpolates through intermediate pose"), Wing->Pose.Root.Pitch > 0.f && Wing->Pose.Root.Pitch < Peak.Pitch);
	Wing->UpdatePose(0.f, INDEX_NONE, false, Wing->Profile->CancelReturnSeconds);
	TestTrue(TEXT("Canceled pose reaches reference"), Wing->Pose.Root.IsNearlyZero());
	TestEqual(TEXT("Ended source identity is invalid"), Wing->SourceInstanceId, INDEX_NONE);
	Wing->UpdatePose(Wing->Profile->StrikePeak, 11, true, .016f);
	TestEqual(TEXT("Repeated attack has a new identity"), Wing->SourceInstanceId, 11);
	Wing->UpdatePose(NAN, 11, true, Wing->Profile->CancelReturnSeconds);
	TestTrue(TEXT("Invalid source position recovers safely"), Wing->Pose.Root.IsNearlyZero());
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWingAssetContractTest, "Hodge.Wing.AssetContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeWingAssetContractTest::RunTest(const FString& Parameters)
{
	const auto* Asset = LoadObject<UHodgeWingProfile>(nullptr, TEXT("/Game/Main/Character/Hero/Wing/DA_PoverWing.DA_PoverWing"));
	if (!TestNotNull(TEXT("Formal wing configuration exists"), Asset)) { return false; }
	TArray<FText> Errors;
	TestTrue(TEXT("Formal mesh, blueprint, montage and timing validate"), Asset->Validate(Errors));
	auto* Invalid = DuplicateObject<UHodgeWingProfile>(Asset, GetTransientPackage());
	Invalid->Mesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab.SKM_Pover_LyraLab"));
	Errors.Reset();
	TestFalse(TEXT("Hero skeleton cannot replace the independent wing skeleton"), Invalid->Validate(Errors));
	Invalid->Mesh = Asset->Mesh;
	Invalid->MeshOffset.SetScale3D(FVector(2.f));
	Errors.Reset();
	TestFalse(TEXT("Attachment cannot silently multiply imported root scale"), Invalid->Validate(Errors));
	auto* Unowned = NewObject<UHodgeWingComponent>();
	Unowned->Profile = const_cast<UHodgeWingProfile*>(Asset);
	AddExpectedError(TEXT("invalid profile/owner/socket"), EAutomationExpectedErrorFlags::Contains, 1);
	TestFalse(TEXT("Missing Character owner fails explicitly"), Unowned->InitializeWing());
	TestNull(TEXT("Failure leaves no partial mesh"), Unowned->GetWingMesh());
	return true;
}
#endif
