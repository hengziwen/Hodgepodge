/** @file HodgeCombatValidationLibraryTests.cpp @brief 使用真实重复Montage实例验证释放采集不会漏掉旧动作。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "HodgeCombatValidationLibrary.h"
#include "Misc/AutomationTest.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMontageAllInstancesTest, "Hodge.Editor.MontageAllInstances",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 最新实例退出不能遮蔽同资源旧实例；原单实例查询保持原有采样语义。 */
bool FHodgeMontageAllInstancesTest::RunTest(const FString&)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::EditorPreview, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	// 栈上守卫在断言提前退出时仍销毁测试世界，所有实例继续由AnimInstance拥有。
	struct FWorldCleanup
	{
		UWorld* Value;
		~FWorldCleanup() { Value->DestroyWorld(false); }
	} Cleanup{World};
	auto* Character = World->SpawnActor<ACharacter>();
	auto* Mesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/Model/SKM_Pover_LyraLab.SKM_Pover_LyraLab"));
	auto* Source = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Main/Character/Hero/Anim/HitReactions/Behit/InPlace/AM_IP_Behit_S_R.AM_IP_Behit_S_R"));
	if (!TestNotNull(TEXT("Target mesh exists"), Mesh) || !TestNotNull(TEXT("Authored montage exists"), Source)) { return false; }
	Character->GetMesh()->SetSkeletalMesh(Mesh);
	Character->GetMesh()->SetAnimInstanceClass(UAnimInstance::StaticClass());
	auto* Anim = Character->GetMesh()->GetAnimInstance();
	if (!TestNotNull(TEXT("Real animation instance is ready"), Anim)) { return false; }
	auto* Montage = DuplicateObject<UAnimMontage>(Source, Character);
	Montage->BlendIn.SetBlendTime(0.f);
	if (!TestTrue(TEXT("First real montage starts"), Anim->Montage_Play(Montage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, false) > 0.f)) { return false; }
	auto* Older = Anim->GetActiveInstanceForMontage(Montage);
	if (!TestNotNull(TEXT("First active instance exists"), Older)) { return false; }
	Older->UpdateWeight(.01f);
	if (!TestTrue(TEXT("Second instance of the same resource starts"), Anim->Montage_Play(Montage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, false) > 0.f)) { return false; }
	// GetInstanceForMontage返回数组中的首个匹配项，最新播放实例应从活动映射取得。
	auto* Latest = Anim->GetActiveInstanceForMontage(Montage);
	if (!TestNotNull(TEXT("Latest active instance exists"), Latest)) { return false; }
	if (!TestTrue(TEXT("Replaying creates a different actual instance"), Older != Latest)) { return false; }
	Latest->Stop(FAlphaBlend(0.f));
	Latest->UpdateWeight(.01f);
	const auto Single = UHodgeCombatValidationLibrary::InspectMontageState(Character, Montage);
	TestEqual(TEXT("Existing sampler still selects the latest instance"), Single.InstanceId, Latest->GetInstanceID());
	TestFalse(TEXT("Latest instance has stopped"), Single.bPlaying);
	TestTrue(TEXT("Latest instance has zero remaining weight"), Single.Weight <= .001f);
	const auto All = UHodgeCombatValidationLibrary::InspectMontageStates(Character, Montage);
	TestEqual(TEXT("All-instance sampler preserves both identities"), All.Num(), 2);
	TestTrue(TEXT("Older playing or weighted instance remains visible to cleanup checks"), All.ContainsByPredicate([Older](const auto& State)
	{ return State.InstanceId == Older->GetInstanceID() && (State.bPlaying || State.Weight > .001f); }));
	Older->Stop(FAlphaBlend(0.f));
	Older->UpdateWeight(.01f);
	const auto Released = UHodgeCombatValidationLibrary::InspectMontageStates(Character, Montage);
	TestFalse(TEXT("All instances pass release only after both have exited"), Released.ContainsByPredicate([](const auto& State)
	{ return State.bPlaying || State.Weight > .001f; }));
	TestTrue(TEXT("Missing avatar has no montage instances"), UHodgeCombatValidationLibrary::InspectMontageStates(nullptr, Montage).IsEmpty());
	TestTrue(TEXT("Missing montage cannot match instances"), UHodgeCombatValidationLibrary::InspectMontageStates(Character, nullptr).IsEmpty());
	return true;
}
#endif
