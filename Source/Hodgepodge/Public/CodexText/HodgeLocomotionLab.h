#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/GameMode/HodgeGameModeBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeLocomotionLab.generated.h"

/** Opt-in experiment. Existing HeroComponent still owns movement and look input. */
UCLASS(Blueprintable, ClassGroup=(Hodge), meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeLocomotionLabComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UHodgeLocomotionLabComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion") float WalkSpeed = 180.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion") float RunSpeed = 420.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Locomotion") bool bCombatFacing = true;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Locomotion") bool bWalking = false;
    UFUNCTION(BlueprintCallable) void SetCombatFacing(bool bEnabled);
    UFUNCTION(BlueprintCallable) void SetWalking(bool bEnabled);
    UFUNCTION(BlueprintCallable) void SetKeyboardControlsEnabled(bool bEnabled) { bKeyboardControlsEnabled = bEnabled; }
private:
    bool bKeyboardControlsEnabled = true;
};

/** Uses the normal Hodge Experience/PlayerState/ASC initialization in an isolated map. */
UCLASS()
class HODGEPODGE_API AHodgeLocomotionLabMode : public AHodgeGameModeBase
{
    GENERATED_BODY()
public:
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};

/** Editor-only authoring implementation; no editor modules are linked in Game builds. */
UCLASS()
class HODGEPODGE_API UHodgeLocomotionLabAuthoring : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
    static bool RemapCopy(UObject* Asset, const TArray<UObject*>& Sources, const TArray<UObject*>& Copies);
    UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
    static bool ConfigureGroundBlend(UObject* Blueprint, UObject* BlendSpace);
};
