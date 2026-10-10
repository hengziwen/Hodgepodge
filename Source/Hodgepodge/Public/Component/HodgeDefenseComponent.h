#pragma once

#include "Components/PawnComponent.h"
#include "GameplayEffectTypes.h"
#include "HodgeDefenseComponent.generated.h"

class UHodgeAbilitySystemComponent;
class UHodgeSprintAbilityProfile;
class UGameplayAbility;
UENUM(BlueprintType)
enum class EHodgeIncomingHitOutcome : uint8 { Allowed, Dodged, PerfectDodge };

/** 在伤害及附带控制提交前解析闪避；动画和 Cue 不拥有防御权限。 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeDefenseComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
	void UninitializeFromAbilitySystem();
	FGuid RegisterDodgeWindow(UGameplayAbility* Source, FGuid Execution, const UHodgeSprintAbilityProfile* Profile, double StartTime);
	void UnregisterDodgeWindow(FGuid Handle);
	EHodgeIncomingHitOutcome ResolveIncomingHit(AActor* Source, FGuid AttackExecution, int32 HitIndex, bool bCanBeDodged, bool bCanTriggerPerfect);
	static EHodgeIncomingHitOutcome EvaluateWindow(const UHodgeSprintAbilityProfile& Profile, double Age, bool bRewarded, bool bCanBeDodged, bool bPerfectEligible);
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
private:
	struct FWindow
	{
		TWeakObjectPtr<UGameplayAbility> Source;
		TWeakObjectPtr<const UHodgeSprintAbilityProfile> Profile;
		FGuid Execution;
		double StartTime = 0.;
		bool bRewarded = false;
	};
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
	TMap<FGuid, FWindow> Windows;
};
