#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayAbilitySpecHandle.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "HodgeCombatValidationLibrary.generated.h"

class UHodgeAbilitySystemComponent;
class UHodgeGameplayAbility_Definition;
class UAnimMontage;

/** PIE 验证入口：延迟到原生世界 Tick，避免编辑器脚本保护改变 RPC 调用空间。 */
UCLASS()
class HODGEABILITYEDITOR_API UHodgeCombatValidationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueAbilityAction(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool bCancel = false);
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
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool ConfigureValidationPIE(int32 Players, bool bDedicated = false);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool ConfigureValidationSections(UAnimMontage* Montage);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool NormalizeCombatNotifies(UAnimMontage* Montage);
};
