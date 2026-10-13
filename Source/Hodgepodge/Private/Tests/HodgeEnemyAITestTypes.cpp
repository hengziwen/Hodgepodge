/** @file HodgeEnemyAITestTypes.cpp
 * @brief 实现怪物测试专用配置与Actor相关的类型、接口与现有处理流程。
 */
#include "HodgeEnemyAITestTypes.h"
#include "Equipment/HodgeWeaponInstance.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Components/SceneComponent.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEnemyAITestTypes)

/** 添加能力。 */
void UHodgeEnemyTestAbilitySet::AddAbility(TSubclassOf<UHodgeGameplayAbility> Class)
{
	FHodgeAbilitySet_GameplayAbility Entry; Entry.Ability = Class; GrantedGameplayAbilities.Add(Entry);
}
/** 添加效果。 */
void UHodgeEnemyTestAbilitySet::AddEffect(TSubclassOf<UGameplayEffect> Class)
{
	FHodgeAbilitySet_GameplayEffect Entry; Entry.GameplayEffect = Class; GrantedGameplayEffects.Add(Entry);
}
/** 构造敌人测试装备并初始化其默认状态。 */
UHodgeEnemyTestEquipment::UHodgeEnemyTestEquipment() { InstanceType = UHodgeWeaponInstance::StaticClass(); }
/** 处理授予能力。 */
void UHodgeEnemyRebindOnGrantAbility::OnGiveAbility(const FGameplayAbilityActorInfo* Info, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(Info, Spec);
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Info && Info->AvatarActor.IsValid())
	{ if (auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(Info->AvatarActor.Get())) { Extension->UninitializeAbilitySystem(); } }
}
/** 构造敌人事件生成测试能力并初始化其默认状态。 */
UHodgeEnemyOnSpawnTestAbility::UHodgeEnemyOnSpawnTestAbility() { ActivationPolicy = EHodgeAbilityActivationPolicy::OnSpawn; }
/** 构造敌人装备测试Actor对象并初始化其默认状态。 */
AHodgeEnemyEquipmentTestActor::AHodgeEnemyEquipmentTestActor()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
}
/** 处理Construction。 */
void AHodgeEnemyEquipmentFailActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform); Destroy();
}

/** 构造敌人Instant测试效果并初始化其默认状态。 */
UHodgeEnemyInstantTestEffect::UHodgeEnemyInstantTestEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo Mod; Mod.Attribute = UHodgeCombatSet::GetBaseDamageAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive; Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1.f));
	Modifiers.Add(Mod);
}
