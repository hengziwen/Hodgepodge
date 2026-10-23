/** @file HodgeHitDetectionTests.cpp
 * @brief 实现命中检测配置自动化测试相关的类型、接口与现有处理流程。
 */
#if WITH_DEV_AUTOMATION_TESTS

#include "Combat/HodgeHitDetection.h"
#include "Component/HodgeHitDetectionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/LineBatchComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "HAL/IConsoleManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitGeometryTest, "Hodge.Combat.HitGeometry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeHitGeometryTest::RunTest(const FString& Parameters)
{
	const auto* SocketStrategy = GetDefault<UHodgeSocketSweepStrategy>();
	const auto* BoxStrategy = GetDefault<UHodgeBoxSweepStrategy>();
	auto* Component = NewObject<USceneComponent>();
	// 本次操作的来源。
	FHodgeHitSource Source;
	Source.LocalOffset = FVector(100.f, 0.f, 50.f);
	FHodgeHitGeometry Geometry;
	TestTrue(TEXT("Body origin supports local offset without sockets"), SocketStrategy->Capture(Component, Source, Geometry));
	TestEqual(TEXT("Body origin produces one sample"), Geometry.Points.Num(), 1);
	TestEqual(TEXT("Captured sphere preserves the source radius"), Geometry.Radius, Source.Radius);
	if (!Geometry.Points.IsEmpty()) { TestTrue(TEXT("Local offset becomes sample position"), Geometry.Points[0].Equals(Source.LocalOffset)); }
	Source.Sockets.Add(TEXT("MissingSocket"));
	TestFalse(TEXT("Missing socket must not silently fall back to body origin"), SocketStrategy->Capture(Component, Source, Geometry));
	TestFalse(TEXT("Box strategy requires an actual box component"), BoxStrategy->Capture(Component, Source, Geometry));
	auto* Box = NewObject<UBoxComponent>();
	Box->SetBoxExtent(FVector(20.f, 30.f, 40.f), false);
	Box->SetRelativeScale3D(FVector(2.f));
	TestTrue(TEXT("Box shape can be read while collision is disabled"), BoxStrategy->Capture(Box, Source, Geometry));
	TestTrue(TEXT("Box extent includes component scale"), Geometry.BoxExtent.Equals(FVector(40.f, 60.f, 80.f)));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeExistingSourceRadiusTest, "Hodge.Combat.ExistingSourceQueryRadius",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 从真实会话捕获来源再查询，覆盖不经过采样点但位于球半径内的接触。 */
bool FHodgeExistingSourceRadiusTest::RunTest(const FString& Parameters)
{
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
		.CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
	auto* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	if (!TestNotNull(TEXT("Query world exists"), World)) { return false; }
	ON_SCOPE_EXIT { World->DestroyWorld(false); };
	auto* Attacker = World->SpawnActor<ACharacter>();
	auto* Target = World->SpawnActor<ACharacter>();
	if (!TestNotNull(TEXT("Attacker exists"), Attacker) || !TestNotNull(TEXT("Target exists"), Target)) { return false; }
	Target->GetCapsuleComponent()->SetCapsuleSize(20.f, 50.f);
	Target->GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	auto* Detection = NewObject<UHodgeHitDetectionComponent>(Attacker);
	Detection->RegisterComponent();
	const FVector Origin = Attacker->GetMesh()->GetComponentLocation();
	FHodgeHitDetectionRequest Request;
	Request.DirectComponent = Attacker->GetMesh();
	Request.DirectSource.Radius = 60.f;
	Request.Profile = NewObject<UHodgeHitDetectionProfile>(Detection);
	Request.Profile->bRequireLineOfSight = false;
	Request.Profile->SampleMode = EHodgeHitSampleMode::OnceOnEnter;
	const FGuid ExecutionId = FGuid::NewGuid();
	for (const auto Mode : {EHodgeHitQueryMode::Overlap, EHodgeHitQueryMode::Sweep})
	{
		Request.Profile->QueryMode = Mode;
		const uint64 Handle = Detection->CreateDetectionSession(ExecutionId, int32(Mode), Request);
		if (!TestTrue(TEXT("Existing source opens a real session"), Handle != 0)) { return false; }
		for (const bool bInside : {true, false})
		{
			// 胶囊半径为20，中心距70只与60半径的球相交；距100时应在检测体外。
			Target->SetActorLocation(Origin + FVector(0.f, bInside ? 70.f : 100.f, 0.f));
			FHodgeHitDetectionBatch Batch;
			TestTrue(TEXT("Query session samples successfully"), Detection->SampleDetection(Handle, ExecutionId, Batch));
			const bool bHit = Batch.Hits.ContainsByPredicate([Target](const FHitResult& Hit) { return Hit.GetActor() == Target; });
			TestEqual(TEXT("Both query modes honor the declared source radius"), bHit, bInside);
		}
		Detection->EndDetectionSession(Handle, ExecutionId);
	}
	TestEqual(TEXT("All radius test sessions are released"), Detection->GetDetectionSessionCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitProfileValidationTest, "Hodge.Combat.HitProfileValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 执行本自动化用例的准备、操作与断言。 */
bool FHodgeHitProfileValidationTest::RunTest(const FString& Parameters)
{
	// 当前操作使用的配置对象。
	auto* Profile = NewObject<UHodgeHitDetectionProfile>();
	TArray<FText> Errors;
	TestTrue(TEXT("Default profile is usable"), Profile->Validate(Errors));
	Profile->HalfAngleDegrees = std::numeric_limits<float>::quiet_NaN();
	TestFalse(TEXT("NaN geometry is rejected"), Profile->Validate(Errors));
	Profile->HalfAngleDegrees = 180.f;
	Profile->ObjectTypes.Reset();
	TestFalse(TEXT("An empty target query is rejected"), Profile->Validate(Errors));
	return true;
}

#if ENABLE_DRAW_DEBUG
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeConfiguredShapeDebugTest, "Hodge.Combat.ConfiguredShapeDebug",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 开关必须改变实际绘制，但不能改变同一判定框的命中结果。 */
bool FHodgeConfiguredShapeDebugTest::RunTest(const FString& Parameters)
{
	auto& Console = IConsoleManager::Get();
	IConsoleVariable* Master = Console.FindConsoleVariable(TEXT("Hodge.Combat.DebugDraw"));
	IConsoleVariable* Shapes = Console.FindConsoleVariable(TEXT("Hodge.Combat.DebugDraw.ConfiguredShape"));
	IConsoleVariable* Weapon = Console.FindConsoleVariable(TEXT("Hodge.Combat.DebugDraw.Weapon"));
	IConsoleVariable* Duration = Console.FindConsoleVariable(TEXT("Hodge.Combat.DebugDraw.Duration"));
	if (!TestNotNull(TEXT("Master switch exists"), Master) || !TestNotNull(TEXT("Shape switch exists"), Shapes) ||
		!TestNotNull(TEXT("Weapon switch exists"), Weapon) || !TestNotNull(TEXT("Lifetime setting exists"), Duration)) { return false; }
	const int32 OldMaster = Master->GetInt(), OldShapes = Shapes->GetInt(), OldWeapon = Weapon->GetInt();
	const float OldDuration = Duration->GetFloat();
	// 测试只改变当前进程的显示设置，结束恢复，不写配置文件。
	ON_SCOPE_EXIT
	{
		Master->Set(OldMaster, ECVF_SetByCode);
		Shapes->Set(OldShapes, ECVF_SetByCode);
		Weapon->Set(OldWeapon, ECVF_SetByCode);
		Duration->Set(OldDuration, ECVF_SetByCode);
	};
	const auto Values = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
		.CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
	auto* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Values);
	if (!TestNotNull(TEXT("Query world exists"), World)) { return false; }
	ON_SCOPE_EXIT { World->DestroyWorld(false); };
	if (!TestNotNull(TEXT("Debug line batcher exists"), World->PersistentLineBatcher.Get())) { return false; }
	auto* Target = World->SpawnActor<ACharacter>();
	if (!TestNotNull(TEXT("Target exists"), Target)) { return false; }
	Target->GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	auto* Profile = NewObject<UHodgeHitDetectionProfile>();
	Profile->QueryMode = EHodgeHitQueryMode::Overlap;
	FHodgeHitSource Source;
	Source.SourceTag = FGameplayTag::RequestGameplayTag(TEXT("Combat.Source.Weapon.MainHand"));
	FHodgeHitGeometry Geometry;
	Geometry.Shape = EHodgeHitShape::Box;
	Geometry.BoxExtent = FVector(200.f);
	Geometry.Transform = Target->GetActorTransform();
	const auto* Strategy = GetDefault<UHodgeShapeQueryStrategy>();
	Duration->Set(1.f, ECVF_SetByCode);
	Weapon->Set(0, ECVF_SetByCode);
	Master->Set(1, ECVF_SetByCode);
	for (int32 Case = 0; Case < 3; ++Case)
	{
		// 关闭分类、开启分类、关闭总开关三条路径使用完全相同的几何和对象查询。
		Shapes->Set(Case == 0 ? 0 : 1, ECVF_SetByCode);
		Master->Set(Case == 2 ? 0 : 1, ECVF_SetByCode);
		World->PersistentLineBatcher->Flush();
		TArray<FHitResult> Hits;
		Strategy->Detect(World, Source, Profile, Geometry, Geometry, FCollisionQueryParams(), Hits);
		TestTrue(TEXT("Debug switches preserve real target hit"), Hits.ContainsByPredicate(
			[Target](const FHitResult& Hit) { return Hit.GetActor() == Target; }));
		const bool bHasLines = !World->PersistentLineBatcher->BatchedLines.IsEmpty();
		TestEqual(TEXT("Configured shape switch works independently of weapon category"), bHasLines, Case == 1);
	}
	return true;
}
#endif

#endif
