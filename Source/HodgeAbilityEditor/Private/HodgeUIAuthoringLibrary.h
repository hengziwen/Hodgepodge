#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeUIAuthoringLibrary.generated.h"
class APlayerController;

/** 仅在编辑器制作 Main UI 资产并调度 PIE 回归，不进入 Game 模块。 */
UCLASS()
class UHodgeUIAuthoringLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString CreateFoundationAssets();
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString MigrateDesignerAssets();
    UFUNCTION(BlueprintPure, Category="Hodge|Editor|UI") static FString InspectDesignerAssets();
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString ConfigureDesignerPreviews();
    UFUNCTION(BlueprintPure, Category="Hodge|Editor|UI") static FString InspectPlayerUI(APlayerController* Player);
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool QueueUIAction(APlayerController* Player, FName Action, float Value = 0);
};
