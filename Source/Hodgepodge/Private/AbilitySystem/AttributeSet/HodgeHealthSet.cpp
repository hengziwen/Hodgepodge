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


// 定义普通伤害 GameplayTag。
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage, "Gameplay.Damage");

// 定义伤害免疫 GameplayTag。
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity, "Gameplay.DamageImmunity");

// 定义自毁伤害 GameplayTag，自毁伤害可以绕过部分伤害免疫和作弊保护逻辑。
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");

// 定义掉出世界相关伤害 GameplayTag。
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");

// 定义 Hodge 项目统一伤害消息 GameplayTag。
UE_DEFINE_GAMEPLAY_TAG(TAG_Hodge_Damage_Message, "Hodge.Damage.Message");

// 初始化生命属性集默认值。
UHodgeHealthSet::UHodgeHealthSet()
// 默认当前生命值为 100。
	: Health(100.0f)
	  // 默认最大生命值为 100。
	  , MaxHealth(100.0f)
{
	// 初始状态下角色并未处于生命耗尽状态。
	bOutOfHealth = false;

	// 初始化 GameplayEffect 执行前的最大生命值缓存。
	MaxHealthBeforeAttributeChange = 0.0f;

	// 初始化 GameplayEffect 执行前的生命值缓存。
	HealthBeforeAttributeChange = 0.0f;
}

// 注册 HealthSet 中需要进行网络复制的属性。
void UHodgeHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 先注册父类需要复制的属性。
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Health 复制给所有相关客户端，并始终触发 OnRep_Health。
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeHealthSet, Health, COND_None, REPNOTIFY_Always);

	// MaxHealth 复制给所有相关客户端，并始终触发 OnRep_MaxHealth。
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

// Health 从服务器复制到客户端后触发。
void UHodgeHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	// 通知 GAS 属性系统 Health 已完成网络同步，并维护内部聚合器等状态。
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeHealthSet, Health, OldValue);

	// Call the change callback, but without an instigator
	// 客户端通过属性复制只能知道数值发生了变化，这里没有完整的伤害发起者信息。

	// This could be changed to an explicit RPC in the future
	// 如果以后客户端需要完整 Instigator / Causer / EffectSpec，可以考虑通过显式 RPC 或消息系统传递。

	// These events on the client should not be changing attributes
	// 客户端这里广播的事件主要用于表现和监听，不应该在回调中再次修改权威属性。

	// 获取复制完成后的当前生命值。
	const float CurrentHealth = GetHealth();

	// 根据新旧 Health 差值估算本次变化量。
	const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();

	// 广播 Health 变化事件；复制路径无法提供完整 GE 上下文，因此前三个参数为空。
	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);

	// 如果之前还没有进入生命耗尽状态，并且这次 Health 已经下降到 0 或以下。
	if (!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		// 广播生命耗尽事件。
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(),
		                        CurrentHealth);
	}

	// 根据最新 Health 更新生命耗尽状态。
	bOutOfHealth = (CurrentHealth <= 0.0f);
}

// MaxHealth 从服务器复制到客户端后触发。
void UHodgeHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	// 通知 GAS 属性系统 MaxHealth 已完成网络同步。
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeHealthSet, MaxHealth, OldValue);

	// Call the change callback, but without an instigator
	// 客户端复制路径没有完整的 GameplayEffect 发起者上下文。

	// This could be changed to an explicit RPC in the future
	// 如果以后需要完整来源信息，可以改用 RPC 或其他事件消息机制。

	// 广播最大生命值变化事件，并通过新旧值差计算本次变化量。
	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxHealth() - OldValue.GetCurrentValue(),
	                             OldValue.GetCurrentValue(), GetMaxHealth());
}

// GameplayEffect 对 Attribute 执行最终修改之前调用。
bool UHodgeHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	// 先执行父类检查，如果父类拒绝本次修改，则直接终止。
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// Handle modifying incoming normal damage
	// 只对写入 Damage Meta Attribute 的效果执行伤害前置检查。
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 只有正数 Damage 才被视为实际收到的伤害。
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			// 检查当前 GameplayEffect 是否携带 SelfDestruct 动态 AssetTag。
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(
				TAG_Gameplay_DamageSelfDestruct);

			// 普通伤害遇到 DamageImmunity 时直接被阻止，自毁伤害例外。
			if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				// 将最终伤害量清零。
				Data.EvaluatedData.Magnitude = 0.0f;

				// 返回 false，阻止本次 GameplayEffect 对该 Attribute 的执行。
				return false;
			}

#if !UE_BUILD_SHIPPING
			// Check GodMode cheat, unlimited health is checked below
			// 非 Shipping 环境下检查 GodMode；UnlimitedHealth 会在后面的最低生命值逻辑中处理。

			// GodMode 可以完全阻止普通伤害，但自毁伤害仍然允许生效。
			if (Data.Target.HasMatchingGameplayTag(HodgeGameplayTags::Cheat_GodMode) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				// 将最终伤害量清零。
				Data.EvaluatedData.Magnitude = 0.0f;

				// 阻止本次伤害继续执行。
				return false;
			}
#endif // #if !UE_BUILD_SHIPPING
		}
	}

	// Save the current health
	// 缓存 GameplayEffect 真正执行之前的 Health，用于执行完成后计算变化和广播事件。
	HealthBeforeAttributeChange = GetHealth();

	// 同时缓存执行前的 MaxHealth。
	MaxHealthBeforeAttributeChange = GetMaxHealth();

	// 允许本次 GameplayEffect 修改继续执行。
	return true;
}

// GameplayEffect 完成对 Attribute 的修改后调用。
void UHodgeHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// 先执行父类后处理逻辑。
	Super::PostGameplayEffectExecute(Data);

	// 判断当前效果是否属于自毁伤害。
	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(
		TAG_Gameplay_DamageSelfDestruct);

	// 默认允许 Health 最低下降到 0。
	float MinimumHealth = 0.0f;

#if !UE_BUILD_SHIPPING
	// Godmode and unlimited health stop death unless it's a self destruct
	// 非 Shipping 环境下，GodMode / UnlimitedHealth 会阻止普通伤害将角色真正打死。

	// 非自毁伤害，并且目标具有 GodMode 或 UnlimitedHealth 时启用最低 1 点生命保护。
	if (!bIsDamageFromSelfDestruct &&
		(Data.Target.HasMatchingGameplayTag(HodgeGameplayTags::Cheat_GodMode) || Data.Target.HasMatchingGameplayTag(
			HodgeGameplayTags::Cheat_UnlimitedHealth)))
	{
		// 将最低生命值设置为 1，使后续 Clamp 不允许 Health 降到 0。
		MinimumHealth = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	// 获取当前 GameplayEffect 携带的 EffectContext。
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();

	// 获取最初触发该 GameplayEffect 的 Instigator。
	AActor* Instigator = EffectContext.GetOriginalInstigator();

	// 获取真正造成该 GameplayEffect 的 Causer，例如角色、武器、投射物等。
	AActor* Causer = EffectContext.GetEffectCauser();

	// 本次 GameplayEffect 修改的是 Damage Meta Attribute。
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Send a standardized verb message that other systems can observe
		// 当存在实际伤害时，可以向 GameplayMessage 系统广播标准化伤害消息。
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			// 原设计：构造统一的伤害消息，携带伤害来源、目标、Tag 和伤害量。
			// FHodgeVerbMessage Message;
			// Message.Verb = TAG_Hodge_Damage_Message;
			// Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			// Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			// Message.Target = GetOwningActor();
			// Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			// //@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			// //@TODO：后续补充 ContextTag，以及不属于 AbilitySystem 的来源 / Instigator Tag。
			// //@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
			// //@TODO：后续可以进一步判断敌方击杀、自杀、友军击杀等伤害关系。
			// Message.Magnitude = Data.EvaluatedData.Magnitude;
			//
			// 获取当前 World 的 GameplayMessageSubsystem，并广播伤害消息。
			// UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			// MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		// Convert into -Health and then clamp
		// 将 Damage Meta Attribute 真正转换为 Health 减少，并限制在 MinimumHealth ~ MaxHealth 范围。
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));

		// Damage 只是一次性 Meta Attribute，消费完成后立即清零，避免污染下一次伤害计算。
		SetDamage(0.0f);
	}

	// 本次 GameplayEffect 修改的是 Healing Meta Attribute。
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Convert into +Health and then clamo
		// 将 Healing Meta Attribute 转换为 Health 增加，并限制在 MinimumHealth ~ MaxHealth 范围。
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));

		// Healing 消费完成后清零，等待下一次治疗重新写入。
		SetHealing(0.0f);
	}

	// 本次 GameplayEffect 直接修改了 Health。
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		// 对直接修改后的 Health 再做一次合法范围限制。
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}

	// 本次 GameplayEffect 修改的是 MaxHealth。
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		// TODO clamp current health?
		// TODO：这里是否需要立即同步 Clamp 当前 Health，可以根据项目 MaxHealth 变化规则进一步决定。

		// Notify on any requested max health changes
		// 广播 MaxHealth 变化事件，并携带完整 GameplayEffect 上下文。
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                             MaxHealthBeforeAttributeChange, GetMaxHealth());
	}

	// If health has actually changed activate callbacks
	// 只有最终 Health 确实发生变化时才广播 HealthChanged。
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		// 广播生命值变化，并携带伤害来源、GE、修改前 Health 和修改后 Health。
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                          HealthBeforeAttributeChange, GetHealth());
	}

	// Health 首次下降到 0 或以下时触发生命耗尽事件。
	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		// 广播 OutOfHealth，供死亡系统等逻辑继续处理。
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		                        HealthBeforeAttributeChange, GetHealth());
	}

	// Check health again in case an event above changed it.
	// 上面的事件回调本身可能再次修改 Health，因此最后重新计算一次生命耗尽状态。
	bOutOfHealth = (GetHealth() <= 0.0f);
}

// Attribute 的 BaseValue 即将发生变化之前调用。
void UHodgeHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// 先执行父类 BaseValue 修改前逻辑。
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// 对即将写入的基础值进行合法范围限制。
	ClampAttribute(Attribute, NewValue);
}

// Attribute 的 CurrentValue 即将发生变化之前调用。
void UHodgeHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// 先执行父类 CurrentValue 修改前逻辑。
	Super::PreAttributeChange(Attribute, NewValue);

	// 对即将生效的当前值进行合法范围限制。
	ClampAttribute(Attribute, NewValue);
}

// Attribute 完成变化之后调用。
void UHodgeHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	// 先执行父类属性变化后逻辑。
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// 如果发生变化的是 MaxHealth。
	if (Attribute == GetMaxHealthAttribute())
	{
		// Make sure current health is not greater than the new max health.
		// MaxHealth 降低后，需要保证当前 Health 不会继续高于新的最大生命值。

		// 当前 Health 已经超过新的 MaxHealth。
		if (GetHealth() > NewValue)
		{
			// 获取拥有当前 AttributeSet 的项目 ASC。
			UHodgeAbilitySystemComponent* HodgeASC = GetHodgeAbilitySystemComponent();

			// HealthSet 正常工作时必须存在有效 ASC。
			check(HodgeASC);

			// 通过 ASC 将 Health Override 为新的 MaxHealth。
			HodgeASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	// 如果之前已经处于 OutOfHealth 状态，但现在 Health 又恢复到了 0 以上。
	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		// 清除生命耗尽状态，使角色以后再次归零时能够重新触发 OnOutOfHealth。
		bOutOfHealth = false;
	}
}

// 对不同生命属性执行统一的数值范围限制。
void UHodgeHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// 当前正在修改 Health。
	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		// Health 最低为 0，最高不能超过当前 MaxHealth。
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	// 当前正在修改 MaxHealth。
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		// MaxHealth 最低保持为 1，避免出现 0 或负数最大生命值。
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
