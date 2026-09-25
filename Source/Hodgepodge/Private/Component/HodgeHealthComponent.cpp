// 111屎山代码来袭


// UHodgeHealthComponent 类定义。
#include "Component/HodgeHealthComponent.h"

// 项目自定义 AbilitySystemComponent。
#include "AbilitySystem/HodgeAbilitySystemComponent.h"

// 项目统一 GameplayTag 定义。
#include "AbilitySystem/HodgeGameplayTags.h"

// Health / MaxHealth / Damage / Healing 等生命属性定义。
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"

// 项目 AssetManager，用于加载 GameData 中配置的 GameplayEffect 类。
#include "Data/HodgeAssetManager.h"

// 项目全局 GameData，用于获取自毁伤害等公共资源配置。
#include "Data/HodgeGameData.h"

// 网络属性复制相关宏与类型。
#include "Net/UnrealNetwork.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHealthComponent)

// 定义角色被淘汰时使用的标准化 GameplayMessage Tag。
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Hodge_Elimination_Message, "Hodge.Elimination.Message");


// HealthComponent 构造函数。
UHodgeHealthComponent::UHodgeHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// HealthComponent 不需要在创建后自动开启 Tick。
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// HealthComponent 完全由 Attribute 事件和死亡流程驱动，不需要每帧 Tick。
	PrimaryComponentTick.bCanEverTick = false;

	// HealthComponent 默认参与网络复制，主要用于同步 DeathState。
	SetIsReplicatedByDefault(true);

	// 初始化时尚未绑定 ASC。
	AbilitySystemComponent = nullptr;

	// 初始化时尚未找到对应 HealthSet。
	HealthSet = nullptr;

	// 默认处于正常存活状态。
	DeathState = EHodgeDeathState::NotDead;
}

// 注册 HealthComponent 需要进行网络复制的属性。
void UHodgeHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 先注册父类需要复制的属性。
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 将死亡状态从服务器复制到客户端，并通过 OnRep_DeathState 同步死亡流程。
	DOREPLIFETIME(UHodgeHealthComponent, DeathState);
}

// Component 从所属 Actor 注销时调用。
void UHodgeHealthComponent::OnUnregister()
{
	// 注销前解除与 ASC / HealthSet 的事件绑定和引用关系。
	UninitializeFromAbilitySystem();

	// 执行父类注销逻辑。
	Super::OnUnregister();
}

// 使用指定 ASC 初始化 HealthComponent。
void UHodgeHealthComponent::InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC)
{
	// 获取当前 HealthComponent 所属 Actor。
	AActor* Owner = GetOwner();

	// HealthComponent 必须挂载在有效 Actor 上。
	check(Owner);

	// 如果已经绑定过 ASC，则不允许重复初始化。
	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "HodgeHealthComponent: Health component for owner [%s] has already been initialized with an ability system."
		       ), *GetNameSafe(Owner));
		return;
	}

	// 保存传入的 ASC。
	AbilitySystemComponent = InASC;

	// ASC 为空时无法继续初始化生命系统。
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("HodgeHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."
		       ), *GetNameSafe(Owner));
		return;
	}

	// 从 ASC 已注册的 AttributeSet 中查找 UHodgeHealthSet。
	HealthSet = AbilitySystemComponent->GetSet<UHodgeHealthSet>();

	// HealthSet 不存在说明当前 ASC 没有配置生命属性集。
	if (!HealthSet)
	{
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "HodgeHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."
		       ), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	// 监听 HealthSet 的 Health 变化事件。
	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);

	// 监听 HealthSet 的 MaxHealth 变化事件。
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);

	// 监听 Health 耗尽事件，用于触发死亡相关 GameplayEvent。
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	// 临时逻辑：初始化生命组件时直接把 Health BaseValue 重置为当前 MaxHealth。
	// 后续可以改为由数据表、PawnData、初始化 GameplayEffect 等数据驱动方式完成。
	AbilitySystemComponent->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());

	// 清理 ASC 上可能残留的死亡状态 GameplayTag。
	ClearGameplayTags();

	// 主动广播一次当前 Health，使 UI 等监听者初始化时能够立即获得生命值。
	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);

	// 主动广播一次当前生命属性状态，使相关监听者能够完成初始化。
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

// 解除 HealthComponent 与 AbilitySystem 的绑定。
void UHodgeHealthComponent::UninitializeFromAbilitySystem()
{
	// 先清除当前组件维护的死亡状态 GameplayTag。
	ClearGameplayTags();

	// 如果当前已经绑定 HealthSet。
	if (HealthSet)
	{
		// 移除当前组件注册的 Health 变化监听。
		HealthSet->OnHealthChanged.RemoveAll(this);

		// 移除当前组件注册的 MaxHealth 变化监听。
		HealthSet->OnMaxHealthChanged.RemoveAll(this);

		// 移除当前组件注册的 Health 耗尽监听。
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	// 清空 HealthSet 引用。
	HealthSet = nullptr;

	// 清空 ASC 引用。
	AbilitySystemComponent = nullptr;
}

// 清除 HealthComponent 维护的死亡相关 GameplayTag。
void UHodgeHealthComponent::ClearGameplayTags()
{
	// 只有已经绑定 ASC 时才能操作 LooseGameplayTag。
	if (AbilitySystemComponent)
	{
		// 清除“正在死亡”状态。
		AbilitySystemComponent->SetLooseGameplayTagCount(HodgeGameplayTags::Status_Death_Dying, 0);

		// 清除“已经死亡”状态。
		AbilitySystemComponent->SetLooseGameplayTagCount(HodgeGameplayTags::Status_Death_Dead, 0);
	}
}

// 获取当前生命值。
float UHodgeHealthComponent::GetHealth() const
{
	// HealthSet 有效时读取 Health，否则返回 0。
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

// 获取当前最大生命值。
float UHodgeHealthComponent::GetMaxHealth() const
{
	// HealthSet 有效时读取 MaxHealth，否则返回 0。
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

// 获取归一化生命值。
float UHodgeHealthComponent::GetHealthNormalized() const
{
	// HealthSet 有效时才进行计算。
	if (HealthSet)
	{
		// 获取当前生命值。
		const float Health = HealthSet->GetHealth();

		// 获取当前最大生命值。
		const float MaxHealth = HealthSet->GetMaxHealth();

		// MaxHealth 大于 0 时返回 Health / MaxHealth，否则返回 0，避免除零。
		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	// 没有 HealthSet 时视为没有有效生命值。
	return 0.0f;
}

// HealthSet 广播 Health 变化后进入这里。
void UHodgeHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
                                                float OldValue, float NewValue)
{
	// 将 AttributeSet 层的事件转换为 HealthComponent 对外暴露的蓝图事件。
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

// HealthSet 广播 MaxHealth 变化后进入这里。
void UHodgeHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                                   const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
                                                   float OldValue, float NewValue)
{
	// 将 AttributeSet 层的 MaxHealth 变化继续广播给组件外部系统。
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

// HealthSet 检测到生命值耗尽时进入这里。
void UHodgeHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
                                              const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
                                              float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
	// 死亡 GameplayEvent 需要有效 ASC 和造成此次死亡的 GameplayEffectSpec。
	if (AbilitySystemComponent && DamageEffectSpec)
	{
		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
		// 通过 ASC 发送 GameplayEvent.Death，可由监听该 EventTag 的死亡 GameplayAbility 响应。
		{
			// 构造 GAS GameplayEvent 载荷。
			FGameplayEventData Payload;

			// 设置事件类型为 GameplayEvent.Death。
			Payload.EventTag = HodgeGameplayTags::GameplayEvent_Death;

			// 设置导致此次死亡的 Instigator。
			Payload.Instigator = DamageInstigator;

			// 死亡事件目标为当前 ASC 的 AvatarActor。
			Payload.Target = AbilitySystemComponent->GetAvatarActor();

			// 保存造成此次死亡的 GameplayEffect Definition。
			Payload.OptionalObject = DamageEffectSpec->Def;

			// 将原始 EffectContext 一并传递给死亡 Ability。
			Payload.ContextHandle = DamageEffectSpec->GetEffectContext();

			// 携带 GameplayEffect 捕获到的 Source GameplayTag。
			Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();

			// 携带 GameplayEffect 捕获到的 Target GameplayTag。
			Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();

			// 将造成此次死亡的效果数值作为事件 Magnitude。
			Payload.EventMagnitude = DamageMagnitude;

			// 创建预测窗口，为当前 GameplayEvent 处理建立 PredictionKey 上下文。
			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);

			// 将 GameplayEvent.Death 发送给 ASC，使对应 GameplayAbility 能够响应死亡事件。
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		// Send a standardized verb message that other systems can observe
		// 原设计：额外发送统一的 Elimination Message，让 GAS 之外的系统也能够监听玩家淘汰事件。
		{
			// 构造标准化淘汰消息。
			// FHodgeVerbMessage Message;
			// Message.Verb = TAG_Hodge_Elimination_Message;
			// Message.Instigator = DamageInstigator;
			// Message.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			// Message.Target = UHodgeVerbMessageHelpers::GetPlayerStateFromObject(
			//     AbilitySystemComponent->GetAvatarActor());
			// Message.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			// //@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			// //@TODO：补充 ContextTag，以及 GAS 之外来源对象携带的 Tag。
			// //@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
			// //@TODO：进一步区分敌方击杀、自杀、友军击杀等淘汰关系。
			//
			// 获取 GameplayMessageSubsystem 并广播淘汰消息。
			// UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			// MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		//@TODO: assist messages (could compute from damage dealt elsewhere)?
		//@TODO：后续可以根据其他地方记录的伤害贡献计算并发送助攻消息。
	}

#endif // #if WITH_SERVER_CODE
}

// DeathState 从服务器复制到客户端后调用。
void UHodgeHealthComponent::OnRep_DeathState(EHodgeDeathState OldDeathState)
{
	// 保存服务器刚刚复制过来的最新死亡状态。
	const EHodgeDeathState NewDeathState = DeathState;

	// Revert the death state for now since we rely on StartDeath and FinishDeath to change it.
	// 临时恢复到旧状态，因为死亡状态不能简单直接覆盖，
	// 而是需要通过 StartDeath / FinishDeath 补执行每个死亡阶段对应的逻辑。
	DeathState = OldDeathState;

	// 客户端本地死亡状态已经比服务器同步过来的状态更靠后。
	if (OldDeathState > NewDeathState)
	{
		// The server is trying to set us back but we've already predicted past the server state.
		// 说明客户端已经预测到了更靠后的死亡阶段，此时不让较旧服务器状态把客户端流程倒退。
		UE_LOG(LogTemp, Warning,
		       TEXT("HodgeHealthComponent: Predicted past server death state [%d] -> [%d] for owner [%s]."),
		       (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		return;
	}

	// 客户端此前仍然处于 NotDead。
	if (OldDeathState == EHodgeDeathState::NotDead)
	{
		// 服务器状态已经推进到 DeathStarted。
		if (NewDeathState == EHodgeDeathState::DeathStarted)
		{
			// 在客户端补执行死亡开始阶段。
			StartDeath();
		}

		// 服务器状态已经直接推进到了 DeathFinished。
		else if (NewDeathState == EHodgeDeathState::DeathFinished)
		{
			// 客户端必须先补执行 DeathStarted。
			StartDeath();

			// 再补执行 DeathFinished，保证死亡流程顺序完整。
			FinishDeath();
		}
		else
		{
			// 出现未定义的死亡状态转换时输出错误日志。
			UE_LOG(LogTemp, Error, TEXT("HodgeHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."),
			       (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}

	// 客户端此前已经执行到了 DeathStarted。
	else if (OldDeathState == EHodgeDeathState::DeathStarted)
	{
		// 合法的下一阶段只能是 DeathFinished。
		if (NewDeathState == EHodgeDeathState::DeathFinished)
		{
			// 补执行死亡完成逻辑。
			FinishDeath();
		}
		else
		{
			// 出现非法死亡状态转换时输出错误日志。
			UE_LOG(LogTemp, Error, TEXT("HodgeHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."),
			       (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}

	// 检查 StartDeath / FinishDeath 执行后，本地 DeathState 是否最终追上服务器同步状态。
	ensureMsgf((DeathState == NewDeathState),
	           TEXT("HodgeHealthComponent: Death transition failed [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState,
	           (uint8)NewDeathState, *GetNameSafe(GetOwner()));
}

// 开始死亡流程。
void UHodgeHealthComponent::StartDeath()
{
	// 只有 NotDead 状态才能进入 DeathStarted，防止重复执行死亡开始逻辑。
	if (DeathState != EHodgeDeathState::NotDead)
	{
		return;
	}

	// 将死亡状态推进到 DeathStarted。
	DeathState = EHodgeDeathState::DeathStarted;

	// ASC 有效时同步 GAS 层的“正在死亡”状态。
	if (AbilitySystemComponent)
	{
		// 添加 Status.Death.Dying LooseGameplayTag。
		AbilitySystemComponent->SetLooseGameplayTagCount(HodgeGameplayTags::Status_Death_Dying, 1);
	}

	// 获取当前 HealthComponent 所属 Actor。
	AActor* Owner = GetOwner();

	// HealthComponent 必须存在有效 Owner。
	check(Owner);

	// 广播死亡开始事件，可用于动画、输入、UI、角色逻辑等响应。
	OnDeathStarted.Broadcast(Owner);

	// 强制 Owner 尽快进行一次网络更新，加速 DeathState 等状态同步。
	Owner->ForceNetUpdate();
}

// 完成死亡流程。
void UHodgeHealthComponent::FinishDeath()
{
	// 只有已经进入 DeathStarted 的角色才能进入 DeathFinished。
	if (DeathState != EHodgeDeathState::DeathStarted)
	{
		return;
	}

	// 将死亡状态推进到 DeathFinished。
	DeathState = EHodgeDeathState::DeathFinished;

	// ASC 有效时同步 GAS 层的“已经死亡”状态。
	if (AbilitySystemComponent)
	{
		// 添加 Status.Death.Dead LooseGameplayTag。
		AbilitySystemComponent->SetLooseGameplayTagCount(HodgeGameplayTags::Status_Death_Dead, 1);
	}

	// 获取当前 HealthComponent 所属 Actor。
	AActor* Owner = GetOwner();

	// HealthComponent 必须拥有有效 Owner。
	check(Owner);

	// 广播死亡流程完成事件。
	OnDeathFinished.Broadcast(Owner);

	// 强制 Owner 尽快同步最新 DeathState。
	Owner->ForceNetUpdate();
}

// 对自身施加足以致死的 GameplayEffect。
void UHodgeHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	// 只有仍然存活并且存在有效 ASC 时才能执行自毁伤害。
	if ((DeathState == EHodgeDeathState::NotDead) && AbilitySystemComponent)
	{
		// 从全局 GameData 获取统一的 SetByCaller Damage GameplayEffect 类。
		const TSubclassOf<UGameplayEffect> DamageGE = UHodgeAssetManager::GetSubclass(
			UHodgeGameData::Get().DamageGameplayEffect_SetByCaller);

		// Damage GameplayEffect 配置无效时无法继续。
		if (!DamageGE)
		{
			UE_LOG(LogTemp, Error,
			       TEXT(
				       "HodgeHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to find gameplay effect [%s]."
			       ), *GetNameSafe(GetOwner()), *UHodgeGameData::Get().DamageGameplayEffect_SetByCaller.GetAssetName());
			return;
		}

		// 使用当前 ASC 创建一个 Damage GameplayEffectSpec。
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			DamageGE, 1.0f, AbilitySystemComponent->MakeEffectContext());

		// 从 SpecHandle 中取得真正的 GameplayEffectSpec。
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		// Spec 创建失败时终止自毁流程。
		if (!Spec)
		{
			UE_LOG(LogTemp, Error,
			       TEXT(
				       "HodgeHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to make outgoing spec for [%s]."
			       ), *GetNameSafe(GetOwner()), *GetNameSafe(DamageGE));
			return;
		}

		// 给本次 GameplayEffect 添加 SelfDestruct Tag，
		// 让 HealthSet 能够识别这是特殊自毁伤害并绕过普通伤害免疫逻辑。
		Spec->AddDynamicAssetTag(TAG_Gameplay_DamageSelfDestruct);

		// 如果此次自毁是由于掉出世界导致。
		if (bFellOutOfWorld)
		{
			// 额外添加 FellOutOfWorld Tag，记录本次死亡原因。
			Spec->AddDynamicAssetTag(TAG_Gameplay_FellOutOfWorld);
		}

		// 使用最大生命值作为本次自毁伤害量。
		const float DamageAmount = GetMaxHealth();

		// 通过 SetByCaller 将实际伤害值写入 GameplayEffectSpec。
		Spec->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_Damage, DamageAmount);

		// 将构造完成的伤害 GameplayEffect 应用到自身 ASC。
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}
