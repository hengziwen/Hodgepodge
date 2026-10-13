/** @file HodgeLockOnStateEffect.cpp @brief 授予锁定标签，不修改共享 ASC 的整类计数。 */
#include "AbilitySystem/HodgeLockOnStateEffect.h"
#include "Targeting/HodgeLockOnTypes.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeLockOnStateEffect)
UHodgeLockOnStateEffect::UHodgeLockOnStateEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	FInheritedTagContainer Tags; Tags.Added.AddTag(HodgeLockOnTags::Locked);
	auto* TagComponent = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("LockOnStateTags"));
	GEComponents.Add(TagComponent);
	TagComponent->SetAndApplyTargetTagChanges(Tags);
}
