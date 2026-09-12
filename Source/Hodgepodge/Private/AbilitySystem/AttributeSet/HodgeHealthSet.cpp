// 111屎山代码来袭


// UHodgeHealthSet 类定义。
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"

// GameplayEffect 执行阶段相关数据结构，例如 FGameplayEffectModCallbackData。
#include "GameplayEffectExtension.h"

// 项目自定义 AbilitySystemComponent。
#include "AbilitySystem/HodgeAbilitySystemComponent.h"

// 项目统一定义的 GameplayTag。
#include "AbilitySystem/HodgeGameplayTags.h"

// 属性网络复制所需宏与类型。
#include "Net/UnrealNetwork.h"

// 通用伤害类型标签。
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage, "Gameplay.Damage");

// 伤害免疫状态标签。
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity, "Gameplay.DamageImmunity");

// 自毁伤害标签，自毁伤害可以绕过部分伤害免疫或无敌逻辑。
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");

// 掉出世界边界造成的伤害标签。
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");

// Hodge 风格的伤害消息标签，用于统一广播伤害事件。
UE_DEFINE_GAMEPLAY_TAG(TAG_Hodge_Damage_Message, "Hodge.Damage.Message");

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHealthSet)

// 构造函数，设置生命与基础战斗属性的初始值。
UHodgeHealthSet::UHodgeHealthSet() : Health(100.0f)
                                     , MaxHealth(100.0f)
                                     , BaseDamage(0.0f)
                                     , BaseHeal(0.0f)
{
	// 初始状态下角色尚未进入生命耗尽状态。
	bOutOfHealth = false;

	// 初始化属性变化前的 MaxHealth 缓存值。
	MaxHealthBeforeAttributeChange = 0.0f;

	// 初始化属性变化前的 Health 缓存值。
	HealthBeforeAttributeChange = 0.0f;
}

// 注册需要通过网络复制的属性。
void UHodgeHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 先注册父类定义的复制属性。
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Health 向所有客户端复制，并且无论值是否看起来相同都执行 RepNotify。
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeHealthSet, Health, COND_None, REPNOTIFY_Always);

	// MaxHealth 向所有客户端复制，并始终触发 RepNotify。
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);

	// BaseDamage 只复制给拥有者客户端，并始终触发 RepNotify。
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeHealthSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);

	// BaseHeal 只复制给拥有者客户端，并始终触发 RepNotify。
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeHealthSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

// Health 从服务器复制到客户端后执行。
void UHodgeHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	// 通知 GAS 属性复制系统 Health 已经发生网络同步。
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeHealthSet, Health, OldValue);

	// Call the change callback, but without an instigator
	// 客户端通过属性复制感知变化时，没有可靠的 Instigator 信息。

	// This could be changed to an explicit RPC in the future
	// 如果未来需要完整上下文，可以考虑改成显式 RPC。

	// These events on the client should not be changing attributes
	// 客户端收到这些事件后只应该做表现，不应该再次修改权威属性。

	// 获取复制后的当前生命值。
	const float CurrentHealth = GetHealth();

	// 根据新旧值估算本次生命变化量。
	const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();

	// 广播 Health 变化事件；客户端这里拿不到完整的 EffectContext。
	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);

	// 如果之前还没有进入死亡状态，而当前生命首次降到 0 或以下，则广播生命耗尽事件。
	if (!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		// 客户端广播 OutOfHealth，用于驱动表现层死亡响应。
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(),
		                        CurrentHealth);
	}

	// 根据当前生命值重新记录是否处于生命耗尽状态。
	bOutOfHealth = (CurrentHealth <= 0.0f);
}

// MaxHealth 从服务器复制到客户端后执行。
void UHodgeHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	// 通知 GAS 属性复制系统 MaxHealth 已完成同步。
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeHealthSet, MaxHealth, OldValue);

	// Call the change callback, but without an instigator
	// 客户端复制回调中没有完整 Instigator / EffectContext。

	// This could be changed to an explicit RPC in the future
	// 如果需要完整上下文，可以改成服务端主动 RPC 通知。

	// 广播最大生命值变化事件。
	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxHealth() - OldValue.GetCurrentValue(),
	                             OldValue.GetCurrentValue(), GetMaxHealth());
}

// BaseDamage 从服务器复制到拥有者客户端后执行。
void UHodgeHealthSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	// 通知 GAS BaseDamage 属性完成网络同步。
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeHealthSet, BaseDamage, OldValue);
}

// BaseHeal 从服务器复制到拥有者客户端后执行。
void UHodgeHealthSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	// 通知 GAS BaseHeal 属性完成网络同步。
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeHealthSet, BaseHeal, OldValue);
}

// GameplayEffect 真正修改属性之前执行。
bool UHodgeHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	// 先让父类执行基础检查；父类拒绝本次修改时直接终止。
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// Handle modifying incoming normal damage
	// 只有本次 GameplayEffect 正在修改 Damage Meta 属性时才执行伤害前置检查。
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 只有正数 Damage 才视为实际伤害。
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			// 检查本次伤害是否属于自毁伤害。
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(
				TAG_Gameplay_DamageSelfDestruct);

			// 目标拥有伤害免疫，并且本次不是自毁伤害时，阻止伤害。
			if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				// 将伤害量清零，避免后续扣除 Health。
				Data.EvaluatedData.Magnitude = 0.0f;

				// 返回 false，阻止本次属性修改继续执行。
				return false;
			}

#if !UE_BUILD_SHIPPING
			// Check GodMode cheat, unlimited health is checked below
			// 非 Shipping 构建下检查 GodMode；UnlimitedHealth 在后面的最终生命 Clamp 中处理。
			if (Data.Target.HasMatchingGameplayTag(HodgeGameplayTags::Cheat_GodMode) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				// GodMode 下直接将普通伤害清零。
				Data.EvaluatedData.Magnitude = 0.0f;

				// 阻止本次 Damage 修改继续执行。
				return false;
			}
#endif // #if !UE_BUILD_SHIPPING
		}
	}

	// Save the current health
	// 缓存 GameplayEffect 执行前的当前生命值。
	HealthBeforeAttributeChange = GetHealth();

	// 缓存 GameplayEffect 执行前的最大生命值。
	MaxHealthBeforeAttributeChange = GetMaxHealth();

	// 允许本次 GameplayEffect 正常执行。
	return true;
}

// GameplayEffect 完成属性修改后执行。
void UHodgeHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// 先执行父类后处理逻辑。
	Super::PostGameplayEffectExecute(Data);

	// 判断当前 GameplayEffect 是否带有自毁伤害标签。
	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(
		TAG_Gameplay_DamageSelfDestruct);

	// 默认情况下生命最低可以降到 0。
	float MinimumHealth = 0.0f;

#if !UE_BUILD_SHIPPING
	// Godmode and unlimited health stop death unless it's a self destruct
	// 非 Shipping 构建下，GodMode 和 UnlimitedHealth 会阻止普通伤害让角色死亡。
	if (!bIsDamageFromSelfDestruct &&
		(Data.Target.HasMatchingGameplayTag(HodgeGameplayTags::Cheat_GodMode) || Data.Target.HasMatchingGameplayTag(
			HodgeGameplayTags::Cheat_UnlimitedHealth)))
	{
		// 将最低生命限制为 1，保证角色不会进入死亡状态。
		MinimumHealth = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	// 获取本次 GameplayEffect 的上下文。
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();

	// 获取最初发起本次效果的 Actor。
	AActor* Instigator = EffectContext.GetOriginalInstigator();

	// 获取实际造成本次效果的 Actor，例如 Projectile、Weapon 等。
	AActor* Causer = EffectContext.GetEffectCauser();

	// 本次修改的是 Damage Meta 属性。
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Send a standardized verb message that other systems can observe
		// 如果本次确实产生了正伤害，可以向 GameplayMessage 系统广播统一伤害消息。
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			// 构造统一伤害消息。
			// FHodgeVerbMessage Message;

			// 设置消息类型为伤害消息。
			// Message.Verb = TAG_Hodge_Damage_Message;

			// 记录实际造成伤害的 Actor。
			// Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();

			// 记录伤害来源捕获到的 GameplayTag。
			// Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

			// 记录伤害目标。
			// Message.Target = GetOwningActor();

			// 记录伤害目标捕获到的 GameplayTag。
			// Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();

			// //@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			// //@TODO: 后续补充 ContextTag，以及非 GAS 来源的 InstigatorTag。

			// //@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
			// //@TODO: 后续区分敌对击杀、自杀、队友击杀等伤害关系。

			// 记录本次伤害数值。
			// Message.Magnitude = Data.EvaluatedData.Magnitude;
			//

			// 获取当前 World 对应的 GameplayMessageSubsystem。
			// UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

			// 广播标准化伤害消息。
			// MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		// Convert into -Health and then clamp
		// 将 Damage Meta 属性真正转换成 Health 扣减，并限制在 MinimumHealth ~ MaxHealth。
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));

		// Damage 属于一次性 Meta 属性，结算完成后立即清零。
		SetDamage(0.0f);
	}

	// 本次修改的是 Healing Meta 属性。
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Convert into +Health and then clamo
		// 将 Healing Meta 属性转换为 Health 增量，并限制在合法生命范围内。
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));

		// Healing 属于一次性 Meta 属性，结算完成后立即清零。
		SetHealing(0.0f);
	}

	// 本次 GameplayEffect 直接修改 Health。
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		// 对直接修改后的 Health 进行限制，之后统一进入下方生命变化和死亡判断。
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}

	// 本次 GameplayEffect 修改 MaxHealth。
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		// TODO clamp current health?
		// TODO：这里可以考虑是否立即同步限制当前 Health。

		// Notify on any requested max health changes
		// 广播最大生命值变化事件，并携带完整 GameplayEffect 上下文。
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                             MaxHealthBeforeAttributeChange, GetMaxHealth());
	}

	// If health has actually changed activate callbacks
	// 只有生命值确实发生变化时才广播 HealthChanged。
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		// 广播生命值变化事件。
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                          HealthBeforeAttributeChange, GetHealth());
	}

	// 当前生命首次降到 0 或以下时广播 OutOfHealth。
	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		// 广播生命耗尽事件，供死亡系统监听。
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                        HealthBeforeAttributeChange, GetHealth());
	}

	// Check health again in case an event above changed it.
	// 上面的事件回调可能再次修改 Health，因此最后重新计算生命耗尽状态。
	bOutOfHealth = (GetHealth() <= 0.0f);
}

// Attribute 的 BaseValue 修改之前执行。
void UHodgeHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// 先执行父类逻辑。
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// 对新的 BaseValue 进行统一范围限制。
	ClampAttribute(Attribute, NewValue);
}

// Attribute 的 CurrentValue 修改之前执行。
void UHodgeHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// 先执行父类逻辑。
	Super::PreAttributeChange(Attribute, NewValue);

	// 对即将生效的新属性值进行统一范围限制。
	ClampAttribute(Attribute, NewValue);
}

// Attribute 真正修改完成之后执行。
void UHodgeHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	// 先执行父类后处理逻辑。
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// 如果发生变化的是 MaxHealth。
	if (Attribute == GetMaxHealthAttribute())
	{
		// Make sure current health is not greater than the new max health.
		// MaxHealth 下降后，确保当前 Health 不会超过新的最大生命值。
		if (GetHealth() > NewValue)
		{
			// 获取当前 AttributeSet 所属的 Hodge ASC。
			UHodgeAbilitySystemComponent* HodgeASC = GetHodgeAbilitySystemComponent();

			// 正常情况下 AttributeSet 必须存在所属 ASC。
			check(HodgeASC);

			// 通过 ASC 正式覆盖 Health，而不是直接修改底层 AttributeData。
			HodgeASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	// 如果之前已经处于 OutOfHealth，但当前生命重新大于 0，则退出生命耗尽状态。
	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		// 允许以后再次从正生命降到 0 时重新触发 OnOutOfHealth。
		bOutOfHealth = false;
	}
}

// 对 HealthSet 中需要限制范围的属性进行统一 Clamp。
void UHodgeHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// 当前修改的是 Health。
	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		// Health 最低为 0，最高不能超过 MaxHealth。
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	// 当前修改的是 MaxHealth。
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		// MaxHealth 最低保持为 1，避免出现 0 或负数最大生命。
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
