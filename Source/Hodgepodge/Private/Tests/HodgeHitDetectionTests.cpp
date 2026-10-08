#if WITH_DEV_AUTOMATION_TESTS

#include "Combat/HodgeHitDetection.h"
#include "Components/BoxComponent.h"
#include "Misc/AutomationTest.h"
#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitGeometryTest, "Hodge.Combat.HitGeometry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeHitGeometryTest::RunTest(const FString& Parameters)
{
	const auto* SocketStrategy = GetDefault<UHodgeSocketSweepStrategy>();
	const auto* BoxStrategy = GetDefault<UHodgeBoxSweepStrategy>();
	auto* Component = NewObject<USceneComponent>();
	FHodgeHitSource Source;
	Source.LocalOffset = FVector(100.f, 0.f, 50.f);
	FHodgeHitGeometry Geometry;
	TestTrue(TEXT("Body origin supports local offset without sockets"), SocketStrategy->Capture(Component, Source, Geometry));
	TestEqual(TEXT("Body origin produces one sample"), Geometry.Points.Num(), 1);
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHitProfileValidationTest, "Hodge.Combat.HitProfileValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FHodgeHitProfileValidationTest::RunTest(const FString& Parameters)
{
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

#endif
