#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Equipment/HodgeWeaponPresentationTypes.h"
#include "HodgeWeaponPresentationActor.generated.h"

class UHodgeWeaponInstance;
class UHodgeWeaponPresentationProfile;
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;
class APawn;

/** 根挂点和隐藏逻辑 Mesh 留在手部，仅可见 Mesh 执行回背与消隐。 */
UCLASS(Blueprintable)
class HODGEPODGE_API AHodgeWeaponPresentationActor : public AActor
{
	GENERATED_BODY()
public:
	AHodgeWeaponPresentationActor();
	void BindWeapon(UHodgeWeaponInstance* Instance);
	void RefreshPresentation();
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetDetectionMesh() const { return SkeletalMesh; }
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetVisualMesh() const { return WeaponVisualMesh; }
	UFUNCTION(BlueprintPure) float GetVisibilityAmount() const { return VisibilityAmount; }
	UFUNCTION(BlueprintPure) EHodgeWeaponPresentationPhase GetVisualPhase() const { return VisualPhase; }
	FTransform GetVisualTransform() const;
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> WeaponVisualMesh;
	UFUNCTION(BlueprintImplementableEvent) void OnPresentationPhaseChanged(EHodgeWeaponPresentationPhase Phase);
private:
	void TryBindWeapon();
	void ConfigureProfile(const UHodgeWeaponPresentationProfile* Profile);
	void EvaluatePresentation();
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
	UPROPERTY(Transient) TObjectPtr<const UHodgeWeaponPresentationProfile> AppliedProfile;
	UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;
	FTimerHandle BindRetryTimer;
	int32 BindAttempts = 0;
	float VisibilityAmount = 0.f;
	EHodgeWeaponPresentationPhase VisualPhase = EHodgeWeaponPresentationPhase::Hidden;
};
