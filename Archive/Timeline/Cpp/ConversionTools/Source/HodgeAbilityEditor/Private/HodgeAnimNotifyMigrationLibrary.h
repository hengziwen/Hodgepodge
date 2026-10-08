#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeAnimNotifyMigrationLibrary.generated.h"
class UHodgeAbilityDefinition;
class UGameplayEffect;

/** 一次性作者转换工具，旧字段清理后连同实现一起归档。 */
UCLASS()
class UHodgeAnimNotifyMigrationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Migration")
	static bool ConvertLegacyDefinition(UHodgeAbilityDefinition* Definition, TSubclassOf<UGameplayEffect> MissingDamageEffect);
};
