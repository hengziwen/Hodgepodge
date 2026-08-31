#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "HodgeAlsAnimNotify_FootstepEffects.generated.h"

enum EPhysicalSurface : int;
struct FHitResult;
class USoundBase;
class UMaterialInterface;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EHodgeAlsFootBone : uint8
{
	Left,
	Right,
};

UENUM(BlueprintType)
enum class EHodgeAlsFootstepSoundType : uint8
{
	Step,
	WalkRun,
	Land
};

UENUM(BlueprintType)
enum class EHodgeAlsFootstepSoundSpawnMode : uint8
{
	SpawnAtTraceHitLocation,
	SpawnAttachedToFootBone
};

UENUM(BlueprintType)
enum class EHodgeAlsFootstepDecalSpawnMode : uint8
{
	SpawnAtTraceHitLocation,
	SpawnAttachedToTraceHitComponent
};

UENUM(BlueprintType)
enum class EHodgeAlsFootstepParticleEffectSpawnMode : uint8
{
	SpawnAtTraceHitLocation,
	SpawnAttachedToFootBone
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsFootstepSoundSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TSoftObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	EHodgeAlsFootstepSoundSpawnMode SpawnMode{EHodgeAlsFootstepSoundSpawnMode::SpawnAtTraceHitLocation};
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsFootstepDecalSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TSoftObjectPtr<UMaterialInterface> DecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	EHodgeAlsFootstepDecalSpawnMode SpawnMode{EHodgeAlsFootstepDecalSpawnMode::SpawnAttachedToTraceHitComponent};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (AllowPreserveRatio))
	FVector3f Size{10.0f, 20.0f, 20.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float Duration{4.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float FadeOutDuration{2.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FVector3f LocationOffset{0.0f, -10.0f, -1.75f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FRotator3f FootLeftRotationOffset{90.0f, 0.0f, -90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FRotator3f FootRightRotationOffset{-90.0f, 0.0f, 90.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HODGEALS", AdvancedDisplay)
	FQuat4f FootLeftRotationOffsetQuaternion{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HODGEALS", AdvancedDisplay)
	FQuat4f FootRightRotationOffsetQuaternion{ForceInit};

public:
#if WITH_EDITOR
	void PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent);
#endif
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsFootstepParticleSystemSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	TSoftObjectPtr<UNiagaraSystem> ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	EHodgeAlsFootstepParticleEffectSpawnMode SpawnMode{EHodgeAlsFootstepParticleEffectSpawnMode::SpawnAtTraceHitLocation};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FVector3f LocationOffset{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FRotator3f FootLeftRotationOffset{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FRotator3f FootRightRotationOffset{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HODGEALS", AdvancedDisplay)
	FQuat4f FootLeftRotationOffsetQuaternion{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HODGEALS", AdvancedDisplay)
	FQuat4f FootRightRotationOffsetQuaternion{ForceInit};

public:
#if WITH_EDITOR
	void PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent);
#endif
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsFootstepEffectSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FHodgeAlsFootstepSoundSettings Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FHodgeAlsFootstepDecalSettings Decal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	FHodgeAlsFootstepParticleSystemSettings ParticleSystem;

public:
#if WITH_EDITOR
	void PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent);
#endif
};

UCLASS(Blueprintable, BlueprintType)
class HODGEPODGE_API UHodgeAlsFootstepEffectsSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TEnumAsByte<ECollisionChannel> SurfaceTraceChannel{ECC_Visibility};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float SurfaceTraceDistance{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", DisplayName = "Foot Left Y Axis")
	FVector3f FootLeftYAxis{0.0f, 0.0f, 1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", DisplayName = "Foot Left Z Axis")
	FVector3f FootLeftZAxis{1.0f, 0.0f, 0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", DisplayName = "Foot Right Y Axis")
	FVector3f FootRightYAxis{0.0f, 0.0f, 1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", DisplayName = "Foot Right Z Axis")
	FVector3f FootRightZAxis{-1.0f, 0.0f, 0.0f};

	// Prevents footstep decals from spawning if the angle between the foot's Z axis and the surface normal exceeds this value.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ClampMax = 90, ForceUnits = "deg"))
	float DecalSpawnAngleThreshold{35.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay, Meta = (ClampMin = 0, ClampMax = 1))
	float DecalSpawnAngleThresholdCos{FMath::Cos(FMath::DegreesToRadians(35.0f))};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ForceInlineRow))
	TMap<TEnumAsByte<EPhysicalSurface>, FHodgeAlsFootstepEffectSettings> Effects;

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent) override;
#endif
};

UCLASS(DisplayName = "HodgeAls Footstep Effects Animation Notify",
	AutoExpandCategories = ("Settings|Sound", "Settings|Decal", "Settings|Particle System"))
class HODGEPODGE_API UHodgeAlsAnimNotify_FootstepEffects : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UHodgeAlsFootstepEffectsSettings> FootstepEffectsSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	EHodgeAlsFootBone FootBone{EHodgeAlsFootBone::Left};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bSkipEffectsWhenInAir : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound")
	uint8 bSpawnSound : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound", Meta = (ClampMin = 0, ForceUnits = "x"))
	float SoundVolumeMultiplier{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound", Meta = (ClampMin = 0, ForceUnits = "x"))
	float SoundPitchMultiplier{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound")
	EHodgeAlsFootstepSoundType SoundType{EHodgeAlsFootstepSoundType::Step};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound")
	uint8 bIgnoreFootstepSoundBlockCurve : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Decal")
	uint8 bSpawnDecal : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Particle System")
	uint8 bSpawnParticleSystem : 1 {true};

public:
	virtual FString GetNotifyName_Implementation() const override;

#if WITH_EDITOR
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& NotifyEvent) override;
#endif

	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
	                    const FAnimNotifyEventReference& NotifyEventReference) override;

private:
	void SpawnSound(USkeletalMeshComponent* Mesh, const FHodgeAlsFootstepSoundSettings& SoundSettings,
	                const FVector& FootstepLocation, const FQuat& FootstepRotation) const;

	void SpawnDecal(USkeletalMeshComponent* Mesh, const FHodgeAlsFootstepDecalSettings& DecalSettings,
	                const FVector& FootstepLocation, const FQuat& FootstepRotation,
	                const FHitResult& FootstepHit, const FVector& FootZAxis) const;

	void SpawnParticleSystem(USkeletalMeshComponent* Mesh, const FHodgeAlsFootstepParticleSystemSettings& ParticleSystemSettings,
	                         const FVector& FootstepLocation, const FQuat& FootstepRotation) const;
};
