#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayAbilitySpecHandle.h"
#include "HodgeCombatValidationLibrary.generated.h"

class UHodgeAbilitySystemComponent;

/** PIE 验证入口：延迟到原生世界 Tick，避免编辑器脚本保护改变 RPC 调用空间。 */
UCLASS()
class HODGEABILITYEDITOR_API UHodgeCombatValidationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueAbilityAction(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool bCancel = false);
};
