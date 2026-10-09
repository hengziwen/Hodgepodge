#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayAbilitySpecHandle.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "HodgeCombatValidationLibrary.generated.h"

class UHodgeAbilitySystemComponent;
class UHodgeGameplayAbility_Definition;
class UAnimMontage;

/** 包含已经开始混出的实例，避免把 IsPlaying=false 当作姿势权重归零。 */
USTRUCT(BlueprintType)
struct FHodgeMontageValidationState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) int32 InstanceId = INDEX_NONE;
	UPROPERTY(BlueprintReadOnly) float Position = 0.f;
	UPROPERTY(BlueprintReadOnly) float Weight = 0.f;
	UPROPERTY(BlueprintReadOnly) float DesiredWeight = 0.f;
	UPROPERTY(BlueprintReadOnly) float BlendTime = 0.f;
	UPROPERTY(BlueprintReadOnly) bool bPlaying = false;
	UPROPERTY(BlueprintReadOnly) bool bStopped = true;
	UPROPERTY(BlueprintReadOnly) FName LocomotionState;
};

/** PIE 验证入口：延迟到原生世界 Tick，避免编辑器脚本保护改变 RPC 调用空间。 */
UCLASS()
class HODGEABILITYEDITOR_API UHodgeCombatValidationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueAbilityAction(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool bCancel = false);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueFacingMode(AActor* Avatar, EHodgeCharacterFacingDriver Driver, bool bRelease = false);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueActionFacing(AActor* Avatar, FVector Direction, bool bRelease = false, bool bInstant = false);
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static TArray<FName> InspectFacingSequences(AActor* Avatar);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool SetFacingValidationControllerPermission(AActor* Avatar, bool bAllow);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool AddHitNotify(UAnimMontage* Montage, float Start, float End, const FHodgeAnimHitConfig& Config, bool bSingle = false);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool AddStateNotify(UAnimMontage* Montage, float Start, float End, FGameplayTag Tag);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool AddWeaponHandNotify(UAnimMontage* Montage, float Start, float End);
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static FGameplayTagContainer InspectAbilityWindows(UHodgeGameplayAbility_Definition* Ability);
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static int32 InspectHitSessions(AActor* Avatar);
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static int32 InspectPoseLeases(AActor* Avatar);
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static FHodgeMontageValidationState InspectMontageState(AActor* Avatar, UAnimMontage* Montage);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool ConfigureValidationPIE(int32 Players, bool bDedicated = false);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool ConfigureValidationSections(UAnimMontage* Montage);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool NormalizeCombatNotifies(UAnimMontage* Montage);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool ConfigureHitReactionValidationMontage(UAnimMontage* Montage, FName SlotName);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation", meta=(WorldContext="WorldContextObject"))
	static AActor* SpawnHitReactionValidationActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FVector Location);
};
