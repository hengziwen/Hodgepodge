#pragma once

#include "GameFramework/RootMotionSource.h"
#include "HodgeAlsRootMotionSource_Mantling.generated.h"

class UHodgeAlsMantlingSettings;

USTRUCT()
struct HODGEPODGE_API FHodgeAlsRootMotionSource_Mantling : public FRootMotionSource
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<const UHodgeAlsMantlingSettings> MantlingSettings;

	UPROPERTY()
	TWeakObjectPtr<const UPrimitiveComponent> TargetPrimitive;

	UPROPERTY()
	FVector TargetRelativeLocation{ForceInit};

	UPROPERTY()
	FRotator TargetRelativeRotation{ForceInit};

	UPROPERTY()
	FVector ActorFeetLocationOffset{ForceInit};

	UPROPERTY()
	FRotator ActorRotationOffset{ForceInit};

	UPROPERTY()
	FVector TargetAnimationLocation{ForceInit};

	UPROPERTY(Meta = (ClampMin = 0, ForceUnits = "s"))
	float MontageStartTime{0.0f};

public:
	FHodgeAlsRootMotionSource_Mantling();

	virtual FRootMotionSource* Clone() const override;

	virtual bool Matches(const FRootMotionSource* Other) const override;

	virtual void PrepareRootMotion(float SimulationDeltaTime, float DeltaTime, const ACharacter& Character,
	                               const UCharacterMovementComponent& Movement) override;

	virtual bool NetSerialize(FArchive& Archive, UPackageMap* Map, bool& bSuccess) override;

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual FString ToSimpleString() const override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

template <>
struct TStructOpsTypeTraits<FHodgeAlsRootMotionSource_Mantling> : public TStructOpsTypeTraitsBase2<FHodgeAlsRootMotionSource_Mantling>
{
	enum
	{
		WithNetSerializer = true,
	};
};
