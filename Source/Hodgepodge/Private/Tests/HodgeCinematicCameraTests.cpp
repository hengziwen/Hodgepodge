/** @file HodgeCinematicCameraTests.cpp @brief 覆盖CIN-01配置、源时间采样及CIN-03旧会话隔离。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Data/HodgeCinematicCameraProfile.h"
#include "Curves/CurveFloat.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "Component/HodgeHeroComponent.h"
#include "Camera/HodgeCameraMode_Cinematic.h"
#include "Camera/HodgeCameraMode_ThirdPerson.h"
#include <limits>

namespace
{
	/** 使用三个连续分镜作为测试配置，不加载生产资产。 */
	UHodgeCinematicCameraProfile* MakeProfile()
	{
		auto* Profile = NewObject<UHodgeCinematicCameraProfile>();
		for (int32 Index=0; Index<3; ++Index)
		{
			FHodgeCinematicCameraShot Shot;
			Shot.ShotId = FName(*FString::Printf(TEXT("Shot%d"), Index));
			Shot.Start = Index/3.f; Shot.End = (Index+1)/3.f;
			Shot.StartOffset = FVector(180.-Index*30., 30., 0.); Shot.EndOffset = FVector(120.-Index*30., 15., 0.);
			Profile->Shots.Add(Shot);
		}
		return Profile;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeCinematicSchemaTest, "Hodge.Cinematic.Profile.Schema", EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeCinematicSchemaTest::RunTest(const FString& Parameters)
{
	auto* Profile=MakeProfile(); TArray<FText> Errors;
	TestTrue(TEXT("CIN-01 连续三分镜有效"), Profile->Validate(Errors));
	Profile->Shots[1].Start += .02f; Errors.Reset();
	TestFalse(TEXT("区间空洞必须拒绝"), Profile->Validate(Errors));
	Profile=MakeProfile(); Profile->Shots[1].ShotId=Profile->Shots[0].ShotId; Errors.Reset();
	TestFalse(TEXT("分镜身份不能重复"), Profile->Validate(Errors));
	Profile=MakeProfile(); Profile->Shots.Last().End=.9f; Errors.Reset();
	TestFalse(TEXT("窗口末尾不能漏覆盖"), Profile->Validate(Errors));
	Profile=MakeProfile(); Profile->ProbeRadius=-1.f; Errors.Reset();
	TestFalse(TEXT("负安全半径拒绝"), Profile->Validate(Errors));
	Profile=MakeProfile(); Profile->BlendFractions=FVector2D(.7,.5); Errors.Reset();
	TestFalse(TEXT("进退混合区间不能超过整个窗口"), Profile->Validate(Errors));
	Profile=MakeProfile(); Profile->Shots[0].FOV.X=std::numeric_limits<float>::quiet_NaN(); Errors.Reset();
	TestFalse(TEXT("非有限FOV拒绝"), Profile->Validate(Errors));
	Profile=MakeProfile(); Profile->Shots[0].Anchor=TEXT("MissingBone"); Errors.Reset();
	TestFalse(TEXT("必需锚点缺失不能静默成功"), Profile->ValidateMesh(nullptr,Errors));
	Profile=MakeProfile(); Profile->Shots[0].FOVCurve=NewObject<UCurveFloat>(Profile);
	Profile->Shots[0].FOVCurve->FloatCurve.AddKey(.5f, 180.f); Errors.Reset();
	TestFalse(TEXT("非法曲线采样拒绝"), Profile->Validate(Errors));
	Profile=MakeProfile(); Profile->PostProcessWeight=2.f; Errors.Reset();
	TestFalse(TEXT("本地PP权重必须有界"),Profile->Validate(Errors));
	Profile=MakeProfile(); FHodgeCinematicCameraEvent Event; Event.EventId=TEXT("Burst"); Event.Time=1.1f; Profile->LocalEvents.Add(Event); Errors.Reset();
	TestFalse(TEXT("本地表现事件不能越过电影窗口"),Profile->Validate(Errors));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeCinematicHeroLeaseTest, "Hodge.Cinematic.Ownership.HeroLease", EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeCinematicHeroLeaseTest::RunTest(const FString& Parameters)
{
	// 无世界组件验证实际Hero申请／释放入口，不绕过它的代次判断。
	auto* Hero=NewObject<UHodgeHeroComponent>();
	const auto Spec=FGameplayAbilitySpec(UHodgeGameplayAbility::StaticClass(),1).Handle;
	const auto First=Hero->SetAbilityCameraMode(UHodgeCameraMode_Cinematic::StaticClass(),Spec,FGuid::NewGuid());
	const auto Next=Hero->SetAbilityCameraMode(UHodgeCameraMode_Cinematic::StaticClass(),Spec,FGuid::NewGuid());
	TestTrue(TEXT("CIN-03 同Spec新申请有效"),Next.IsValid());
	TestFalse(TEXT("旧句柄不能清理新电影申请"),Hero->ClearAbilityCameraMode(First));
	TestTrue(TEXT("失败清理保留新电影模式"),Hero->GetDesiredCameraMode()==UHodgeCameraMode_Cinematic::StaticClass());
	TestFalse(TEXT("空Spec不能申请相机"),Hero->SetAbilityCameraMode(UHodgeCameraMode_ThirdPerson::StaticClass(),{}).IsValid());
	TestTrue(TEXT("当前身份可以释放"),Hero->ClearAbilityCameraMode(Next));
	TestFalse(TEXT("重复释放幂等"),Hero->ClearAbilityCameraMode(Next));
	TestTrue(TEXT("释放后没有遗留模式"),Hero->GetDesiredCameraMode()==nullptr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeCinematicSamplingTest, "Hodge.Cinematic.Profile.SourceClock", EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeCinematicSamplingTest::RunTest(const FString& Parameters)
{
	auto* Profile=MakeProfile(); FHodgeCinematicCameraSample Sample;
	TestTrue(TEXT("CIN-04 初始源位置采样"), Profile->Sample(0.f,Sample));
	TestEqual(TEXT("起始位置对应配置"), Sample.CameraOffset, Profile->Shots[0].StartOffset);
	TestTrue(TEXT("分镜边界有效"), Profile->Sample(1.f/3.f,Sample));
	TestEqual(TEXT("边界进入下一镜头"), Sample.ShotIndex, 1);
	TestTrue(TEXT("末帧有效"), Profile->Sample(1.f,Sample));
	TestEqual(TEXT("末帧对应最终位置"), Sample.CameraOffset, Profile->Shots.Last().EndOffset);
	TestFalse(TEXT("窗口之外拒绝采样"), Profile->Sample(-.1f,Sample));
	TestFalse(TEXT("非有限位置拒绝"), Profile->Sample(std::numeric_limits<float>::infinity(),Sample));
	TestEqual(TEXT("切入开始无电影贡献"), Profile->EvaluateWeight(0.f,0.f,2.f),0.f);
	TestEqual(TEXT("窗口结束完整返回"), Profile->EvaluateWeight(2.f,0.f,2.f),0.f);
	TestEqual(TEXT("窗口中段完整电影贡献"), Profile->EvaluateWeight(1.f,0.f,2.f),1.f);
	const float PausedWeight=Profile->EvaluateWeight(.1f,0.f,2.f);
	TestEqual(TEXT("暂停同一源位置不推进混合"), Profile->EvaluateWeight(.1f,0.f,2.f),PausedWeight);
	TestTrue(TEXT("退出随剩余源时间平滑衰减"), Profile->EvaluateWeight(1.9f,0.f,2.f)<Profile->EvaluateWeight(1.8f,0.f,2.f));
	TestEqual(TEXT("跳过整个窗口不重播"), Profile->EvaluateWeight(3.f,0.f,2.f),0.f);
	TestEqual(TEXT("退化窗口返回零"), Profile->EvaluateWeight(1.f,1.f,1.f),0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeCinematicHandleTest, "Hodge.Cinematic.Ownership.Identity", EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeCinematicHandleTest::RunTest(const FString& Parameters)
{
	FHodgeAbilityCameraHandle First; First.SpecHandle=FGameplayAbilitySpec(UHodgeGameplayAbility::StaticClass(),1).Handle; First.ExecutionId=FGuid::NewGuid(); First.Generation=1;
	TestTrue(TEXT("CIN-03 当前申请有效"), First.IsValid());
	TestTrue(TEXT("同一身份相等"), First==First);
	auto Next=First; ++Next.Generation;
	TestFalse(TEXT("旧代次不能释放同Spec新申请"), First==Next);
	Next=First; Next.ExecutionId=FGuid::NewGuid();
	TestFalse(TEXT("旧执行不能释放新执行"), First==Next);
	Next=First; Next.SpecHandle=FGameplayAbilitySpec(UHodgeGameplayAbility::StaticClass(),1).Handle;
	TestFalse(TEXT("其他技能不能释放相机"), First==Next);
	TestFalse(TEXT("空申请无效"), FHodgeAbilityCameraHandle().IsValid());
	return true;
}
#endif
