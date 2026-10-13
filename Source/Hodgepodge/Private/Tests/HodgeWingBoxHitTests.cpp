/** @file HodgeWingBoxHitTests.cpp
 * @brief 验证翅膀盒体的锚点、世界尺寸、查询与非法来源关闭。
 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Animation/HodgeWingProfile.h"
#include "Animation/AnimMontage.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeWingBoxHitTest, "Hodge.Wing.BoxHitGeometry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
/** 验证 WING-HIT-01/03，使用正式配置的副本，不修改资产。 */
bool FHodgeWingBoxHitTest::RunTest(const FString& Parameters)
{
	const auto* Profile = LoadObject<UHodgeWingProfile>(nullptr, TEXT("/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu.DA_PoverWing_Anzhu"));
	if (!TestNotNull(TEXT("WING-HIT-01 Formal profile"), Profile)) { return false; }
	FHodgeAnimHitConfig Config;
	bool bFound = false;
	for (const auto& Event : Profile->AttackMontage->Notifies)
	{
		const auto* Notify = Cast<UHodgeAnimNotifyState_HitCheck>(Event.NotifyStateClass);
		if (Notify && Notify->Hit.Source == EHodgeAnimHitSource::NamedComponent && Notify->Hit.ComponentName == TEXT("WingMesh"))
		{
			Config = Notify->Hit;
			bFound = true;
		}
	}
	if (!TestTrue(TEXT("WING-HIT-01 Wing notification exists"), bFound)) { return false; }
	TestTrue(TEXT("WING-HIT-01 Box follows a real bone"), Config.Shape == EHodgeHitShape::Box && Config.TransformPolicy == EHodgeHitTransformPolicy::Follow);
	TestTrue(TEXT("WING-HIT-02 Formal authority sampling enabled"), Profile->bEnableAuthorityHitSampling);
	TArray<FText> Errors;
	TestTrue(TEXT("WING-HIT-01 Valid measured configuration"), Config.Validate(Errors));
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
		.CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
	auto* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	if (!TestNotNull(TEXT("WING-HIT-03 Test world"), World)) { return false; }
	auto* Owner = World->SpawnActor<ACharacter>();
	auto* Target = World->SpawnActor<ACharacter>();
	Owner->GetMesh()->SetSkeletalMeshAsset(Profile->Mesh);
	Owner->GetMesh()->SetRelativeScale3D(FVector(.8f));
	Owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Target->GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	Target->GetCapsuleComponent()->SetCapsuleSize(5.f, 5.f);
	auto* Combat = NewObject<UHodgeCombatComponentBase>(Owner);
	Combat->RegisterComponent();
	auto* Query = NewObject<UHodgeHitDetectionProfile>(Owner);
	Query->QueryMode = EHodgeHitQueryMode::Overlap;
	Query->bRequireLineOfSight = false;
	FHodgeHitDetectionRequest Request;
	Request.Profile = Query;
	Request.DirectComponent = Owner->GetMesh();
	Request.Volume.GeometryMode = EHodgeHitGeometryMode::ConfiguredShape;
	Request.Volume.Shape = EHodgeHitShape::Box;
	Request.Volume.AnchorKind = EHodgeHitAnchorKind::RegisteredSource;
	Request.Volume.AnchorSocket = Config.BoneOrSocket;
	Request.Volume.LocalTransform = Config.LocalTransform;
	Request.Volume.BoxHalfExtent = Config.BoxHalfExtent;
	FTransform Anchor = Owner->GetMesh()->GetSocketTransform(Config.BoneOrSocket);
	Anchor.SetScale3D(FVector::OneVector);
	const FTransform Expected = Config.LocalTransform * Anchor;
	Target->SetActorLocation(Expected.GetLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	const FGuid Execution = FGuid::NewGuid();
	const uint64 Handle = Combat->CreateDetectionSession(Execution, 1, Request);
	TestTrue(TEXT("WING-HIT-03 Bone-backed box session opens"), Handle != 0);
	FHodgeHitDetectionBatch Batch;
	TestTrue(TEXT("WING-HIT-03 Box can query without model collision"), Combat->SampleDetection(Handle, Execution, Batch));
	TestTrue(TEXT("WING-HIT-01 Local offset uses world centimeters exactly once"), Batch.SourceOrigin.Equals(Expected.GetLocation(), .01f));
	TestTrue(TEXT("WING-HIT-03 Target inside box is detected"), Batch.Hits.ContainsByPredicate([Target](const FHitResult& Hit) { return Hit.GetActor() == Target; }));
	Target->SetActorLocation(Expected.GetLocation() + FVector(10000.f, 0.f, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
	Combat->SampleDetection(Handle, Execution, Batch);
	TestFalse(TEXT("WING-HIT-03 Outside target is excluded"), Batch.Hits.ContainsByPredicate([Target](const FHitResult& Hit) { return Hit.GetActor() == Target; }));
	Combat->EndDetectionSession(Handle, Execution);
	TestFalse(TEXT("WING-HIT-03 Closed session cannot sample"), Combat->SampleDetection(Handle, Execution, Batch));
	Request.Volume.AnchorSocket = TEXT("MissingWingHitBone");
	AddExpectedError(TEXT("Cannot resolve notification hit geometry"), EAutomationExpectedErrorFlags::Contains, 1);
	TestEqual(TEXT("WING-HIT-01 Missing bone rejects instead of using mesh origin"), Combat->CreateDetectionSession(Execution, 2, Request), uint64(0));
	World->DestroyWorld(false);
	return true;
}
#endif
