#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "HodgeHitReactionTypes.generated.h"

class UAbilitySystemComponent;
class UAnimMontage;
class USkeletalMesh;

namespace HodgeHitReactionTags
{
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_Normal);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_Skill);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_SuperArmor);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_Vajra);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_Normal);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_Skill);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_SuperArmor);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_Vajra);
	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Controlled);
}

UENUM(BlueprintType)
enum class EHodgeImpactType : uint8 { HitStun, Knockback, Launch, Knockdown, AirHit, Slam };

UENUM(BlueprintType)
enum class EHodgeImpactDirection : uint8 { AwayFromSource, SourceForward, WorldDirection };

UENUM(BlueprintType)
enum class EHodgeHitAcceptancePolicy : uint8 { AcceptedDamage, ExplicitControl };

UENUM(BlueprintType)
enum class EHodgeHitReactionPhase : uint8 { None, Controlled, Airborne, Downed, GettingUp };

UENUM(BlueprintType)
enum class EHodgeHitReactionOutcome : uint8
{
	NoRequest, NoDamageOutput, Rejected, Dead, InvalidRequest, LowJudgement,
	NoImpact, Unsupported, NotReady, CannotInterrupt, Busy, Applied
};

/** 攻击方只提供效果参数，目标动画和恢复规则由目标 Profile 提供。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeImpactSpec
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHodgeImpactType Type = EHodgeImpactType::HitStun;
	// 0 使用目标默认时长。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="s")) float ControlDuration = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHodgeImpactDirection Direction = EHodgeImpactDirection::AwayFromSource;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector WorldDirection = FVector::ForwardVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="cm")) float KnockbackDistance = 120.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.01", Units="s")) float MoveDuration = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="cm/s")) float HorizontalSpeed = 180.f;
	// Launch/AirHit 向上，Slam 向下；配置保存非负幅值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="cm/s")) float VerticalSpeed = 650.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bKnockdownOnLanding = false;
	bool IsMovement() const;
	bool Validate(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Attack.Judgement,State.Combat.Body")) FGameplayTag AttackJudgementTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FHodgeImpactSpec> Impacts;
	// 仅预留，不消费韧性属性。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0")) float ReservedPoiseDamage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHodgeHitAcceptancePolicy HitAcceptancePolicy = EHodgeHitAcceptancePolicy::AcceptedDamage;
	bool IsConfigured() const;
	bool Validate(TArray<FText>& Errors) const;
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionResult
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FGuid HitId;
	UPROPERTY(BlueprintReadOnly) FGameplayTag AttackJudgementTag;
	UPROPERTY(BlueprintReadOnly) FGameplayTag TargetBodyTag;
	UPROPERTY(BlueprintReadOnly) int32 AttackRank = 0;
	UPROPERTY(BlueprintReadOnly) int32 TargetBodyRank = 0;
	UPROPERTY(BlueprintReadOnly) float ActualDamage = 0.f;
	UPROPERTY(BlueprintReadOnly) float ReservedPoiseDamage = 0.f;
	UPROPERTY(BlueprintReadOnly) EHodgeHitReactionOutcome Outcome = EHodgeHitReactionOutcome::NoRequest;
	UPROPERTY(BlueprintReadOnly) TArray<EHodgeImpactType> AppliedImpacts;
};

/** 当前目标的一份解析后计划；不依赖命中时的临时 Spec。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHitReactionState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) int32 Sequence = 0;
	UPROPERTY(BlueprintReadOnly) EHodgeHitReactionPhase Phase = EHodgeHitReactionPhase::None;
	UPROPERTY(BlueprintReadOnly) EHodgeImpactType Type = EHodgeImpactType::HitStun;
	UPROPERTY(BlueprintReadOnly) float StartedAt = 0.f;
	UPROPERTY(BlueprintReadOnly) float EndsAt = 0.f;
	UPROPERTY(BlueprintReadOnly) float ChainStartedAt = 0.f;
	UPROPERTY(BlueprintReadOnly) float AirStartedAt = 0.f;
	UPROPERTY(BlueprintReadOnly) float AirOriginZ = 0.f;
	UPROPERTY(BlueprintReadOnly) int32 AirHitCount = 0;
	UPROPERTY(BlueprintReadOnly) FVector MotionVelocity = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly) float MoveDuration = 0.f;
	UPROPERTY(BlueprintReadOnly) bool bHasMovement = false;
	UPROPERTY(BlueprintReadOnly) bool bKnockdownOnLanding = false;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> AirLoopMontage;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> LandingMontage;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> GetUpMontage;
	bool IsActive() const { return Phase != EHodgeHitReactionPhase::None; }
};

namespace HodgeHitReaction
{
	HODGEPODGE_API int32 BodyRank(FGameplayTag Tag);
	HODGEPODGE_API int32 JudgementRank(FGameplayTag Tag);
	HODGEPODGE_API int32 ResolveBody(const UAbilitySystemComponent* ASC, FGameplayTag& OutTag);
	HODGEPODGE_API bool CanImpact(int32 AttackRank, int32 BodyRank);
	HODGEPODGE_API bool CanUseMontage(const UAnimMontage* Montage, const USkeletalMesh* Mesh);
}
