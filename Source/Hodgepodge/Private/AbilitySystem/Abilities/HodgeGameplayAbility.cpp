// 111屎山代码来袭


#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayEffectContext.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/Abilities/HodgeAbilityCost.h"
#include "Camera/HodgeCameraMode.h"
#include "Character/HodgeCombatCharacter.h"
#include "Component/HodgeHeroComponent.h"
#include "Interface/HodgeAbilitySourceInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility)

// 确保当前 Ability 已经实例化，否则输出错误并直接返回指定值。
#define ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(FunctionName, ReturnValue)																				\
{																																						\
	if (!ensure(IsInstantiated()))																														\
	{																																					\
		ABILITY_LOG(Error, TEXT("%s: " #FunctionName " cannot be called on a non-instanced ability. Check the instancing policy."), *GetPathName());	\
		return ReturnValue;																																\
	}																																					\
}

// Ability 激活失败时，用于发送简单用户提示消息的 GameplayTag。
UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, "Ability.UserFacingSimpleActivateFail.Message");

// Ability 激活失败时，用于发送失败动画消息的 GameplayTag。
UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, "Ability.PlayMontageOnActivateFail.Message");

UHodgeGameplayAbility::UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Ability 本身不复制到网络，由 ASC 和 AbilitySpec 负责必要的网络同步。
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;

	// 每个 Actor 拥有该 Ability 的独立实例，Ability 内部可以保存运行时状态。
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 默认使用本地预测执行，客户端可以先执行 Ability，再由服务器进行确认。
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// 默认允许客户端或服务器请求执行该 Ability。
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	// 默认通过输入触发 Ability。
	ActivationPolicy = EHodgeAbilityActivationPolicy::OnInputTriggered;

	// 默认不参与其他 Ability 的 Exclusive 互斥关系。
	ActivationGroup = EHodgeAbilityActivationGroup::Independent;

	// 默认关闭 Ability 取消时的额外日志。
	bLogCancelation = false;

	// 默认没有激活中的 CameraMode。
	ActiveCameraMode = nullptr;
}

// 从当前 ActorInfo 中获取项目自定义的 AbilitySystemComponent。
UHodgeAbilitySystemComponent* UHodgeGameplayAbility::GetHodgeAbilitySystemComponentFromActorInfo() const
{
	// ActorInfo 有效时，从其中获取 ASC，并转换成项目自己的 ASC 类型。
	return (CurrentActorInfo
		        ? Cast<UHodgeAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get())
		        : nullptr);
}

// 从当前 ActorInfo 中获取项目自定义的 PlayerController。
AHodgePlayerControllerBase* UHodgeGameplayAbility::GetHodgePlayerControllerFromActorInfo() const
{
	// ActorInfo 有效时，从 PlayerController 字段获取并转换成 Hodge 类型。
	return (CurrentActorInfo ? Cast<AHodgePlayerControllerBase>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

// 从当前 ActorInfo 中获取 Controller。
AController* UHodgeGameplayAbility::GetControllerFromActorInfo() const
{
	// 首先确认 ActorInfo 有效。
	if (CurrentActorInfo)
	{
		// 优先直接从 ActorInfo 中获取 PlayerController。
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}

		// 如果没有直接找到 Controller，则沿 Owner 链继续向上寻找。
		// Look for a player controller or pawn in the owner chain.
		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
		while (TestActor)
		{
			// 当前 Owner 本身就是 Controller 时直接返回。
			if (AController* C = Cast<AController>(TestActor))
			{
				return C;
			}

			// 当前 Owner 是 Pawn 时，从 Pawn 获取它对应的 Controller。
			if (APawn* Pawn = Cast<APawn>(TestActor))
			{
				return Pawn->GetController();
			}

			// 当前 Actor 不是 Controller 或 Pawn，则继续向 Owner 链上层查找。
			TestActor = TestActor->GetOwner();
		}
	}

	// 整条 Owner 链都没有找到 Controller。
	return nullptr;
}

// 从当前 ActorInfo 中获取 Ability 对应的战斗角色 Avatar。
AHodgeCombatCharacter* UHodgeGameplayAbility::GetHodgeCharacterFromActorInfo() const
{
	// ActorInfo 有效时，从 AvatarActor 获取实际执行 Ability 的角色。
	return (CurrentActorInfo ? Cast<AHodgeCombatCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UHodgeHeroComponent* UHodgeGameplayAbility::GetHeroComponentFromActorInfo() const
{
	// ActorInfo 有效时，从 AvatarActor 字段获取HodgeHeroComponent組件。
	return (CurrentActorInfo ? UHodgeHeroComponent::FindHeroComponent(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

// C++ 层处理 Ability 激活失败。
void UHodgeGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
	// 标记是否已经找到一个简单的用户提示原因。
	bool bSimpleFailureFound = false;

	// 遍历 Ability 激活失败产生的所有 GameplayTag。
	for (FGameplayTag Reason : FailedReason)
	{
		// 目前只允许处理一次简单失败提示。
		if (!bSimpleFailureFound)
		{
			// 根据失败 Tag 查找对应的用户提示文本。
			// if (const FText* pUserFacingMessage = FailureTagToUserFacingMessages.Find(Reason))
			// {
			// 	FHodgeAbilitySimpleFailureMessage Message;
			// 	Message.PlayerController = GetActorInfo().PlayerController.Get();
			// 	Message.FailureTags = FailedReason;
			// 	Message.UserFacingReason = *pUserFacingMessage;
			//
			// 	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			// 	MessageSystem.BroadcastMessage(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, Message);
			// 	bSimpleFailureFound = true;
			// }

			// 这里预留了根据失败 Tag 发送 GameplayMessage 的逻辑。
		}

		// 根据失败 Tag 查找需要播放的失败动画。
		if (UAnimMontage* pMontage = FailureTagToAnimMontage.FindRef(Reason))
		{
			// 创建失败动画消息数据。
			FHodgeAbilityMontageFailureMessage Message;

			// 记录发生失败的 PlayerController。
			Message.PlayerController = GetActorInfo().PlayerController.Get();

			// 记录发生失败的 AvatarActor。
			Message.AvatarActor = GetActorInfo().AvatarActor.Get();

			// 保存本次 Ability 激活失败的所有原因。
			Message.FailureTags = FailedReason;

			// 保存根据当前失败原因找到的动画 Montage。
			Message.FailureMontage = pMontage;

			// 这里预留了通过 GameplayMessageSubsystem 广播失败动画消息的逻辑。
			// UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			// MessageSystem.BroadcastMessage(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, Message);
		}
	}
}

// 检查当前 Ability 是否满足完整的激活条件。
bool UHodgeGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayTagContainer* SourceTags,
                                               const FGameplayTagContainer* TargetTags,
                                               FGameplayTagContainer* OptionalRelevantTags) const
{
	// 没有有效的 ActorInfo 或 ASC 时，Ability 无法激活。
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	// 先执行 UGameplayAbility 原本的通用激活条件检查。
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	//@TODO Possibly remove after setting up tag relationships
	// 获取当前 Ability 使用的项目自定义 ASC。
	UHodgeAbilitySystemComponent* HodgeASC = CastChecked<UHodgeAbilitySystemComponent>(
		ActorInfo->AbilitySystemComponent.Get());

	// 检查当前 Ability 所属的 ActivationGroup 是否被其他 Ability 阻塞。
	if (HodgeASC->IsActivationGroupBlocked(ActivationGroup))
	{
		// 如果调用方需要失败原因，则记录 ActivationGroup 被阻塞。
		if (OptionalRelevantTags)
		{
			OptionalRelevantTags->AddTag(HodgeGameplayTags::Ability_ActivateFail_ActivationGroup);
		}

		// ActivationGroup 被阻塞，因此不能激活。
		return false;
	}

	// 基础 GAS 条件和项目自定义 ActivationGroup 条件都满足。
	return true;
}

// 设置 Ability 是否允许被其他 Ability 取消。
void UHodgeGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
	// 可替换 Ability 必须允许被取消，否则 Exclusive_Replaceable 逻辑无法成立。
	// The ability can not block canceling if it's replaceable.
	if (!bCanBeCanceled && (ActivationGroup == EHodgeAbilityActivationGroup::Exclusive_Replaceable))
	{
		// 输出错误日志，提示当前 Ability 的 ActivationGroup 与取消设置冲突。
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "SetCanBeCanceled: Ability [%s] can not block canceling because its activation group is replaceable."
		       ), *GetName());
		return;
	}

	// 通过父类正式修改 Ability 的可取消状态。
	Super::SetCanBeCanceled(bCanBeCanceled);
}

// Ability 被授予 ASC 时调用。
void UHodgeGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	// 先执行 UGameplayAbility 的原始授予逻辑。
	Super::OnGiveAbility(ActorInfo, Spec);

	// 通知 Blueprint：Ability 已经被添加到 ASC。
	K2_OnAbilityAdded();

	// 如果配置为 OnSpawn，则在获得 Ability 后尝试自动激活。
	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

// Ability 从 ASC 中移除时调用。
void UHodgeGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilitySpec& Spec)
{
	// 通知 Blueprint：Ability 已经从 ASC 中移除。
	K2_OnAbilityRemoved();

	// 再执行 UGameplayAbility 的原始移除逻辑。
	Super::OnRemoveAbility(ActorInfo, Spec);
}

// Ability 真正开始执行时调用。
void UHodgeGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	// 当前暂时只调用父类逻辑，具体 Ability 行为由子类实现。
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

// Ability 结束时调用。
void UHodgeGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                       bool bWasCancelled)
{
	// Ability 结束时清除它设置的 CameraMode。
	ClearCameraMode();

	// 执行 UGameplayAbility 的原始结束逻辑。
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// 检查 Ability 是否能够支付所有 Cost。
bool UHodgeGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	// 先检查 GAS 默认 Cost，再确认 ActorInfo 有效。
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
	{
		return false;
	}

	// 检查所有额外 Cost 是否都能够支付。
	// Verify we can afford any additional costs
	for (const TObjectPtr<UHodgeAbilityCost>& AdditionalCost : AdditionalCosts)
	{
		// 忽略空的 Cost 配置。
		if (AdditionalCost != nullptr)
		{
			// 只要有一个额外 Cost 无法支付，整个 Ability 就不能激活。
			if (!AdditionalCost->CheckCost(this, Handle, ActorInfo, /*inout*/ OptionalRelevantTags))
			{
				return false;
			}
		}
	}

	// 所有 Cost 都检查通过。
	return true;
}

// 实际应用 Ability 的所有额外 Cost。
void UHodgeGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo) const
{
	// 先执行 GAS 原生 Cost。
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	// ActorInfo 在这里必须有效。
	check(ActorInfo);

	// 判断当前 Ability 是否实际命中了目标。
	// Used to determine if the ability actually hit a target (as some costs are only spent on successful attempts)
	auto DetermineIfAbilityHitTarget = [&]()
	{
		// 只有服务器拥有权威的 TargetData，因此只在服务器判断命中结果。
		if (ActorInfo->IsNetAuthority())
		{
			// 获取项目自定义 ASC。
			if (UHodgeAbilitySystemComponent* ASC = Cast<UHodgeAbilitySystemComponent>(
				ActorInfo->AbilitySystemComponent.Get()))
			{
				// 用于保存当前 Ability 激活产生的 TargetData。
				FGameplayAbilityTargetDataHandle TargetData;

				// 从 ASC 中获取指定 Ability 实例本次激活的 TargetData。
				ASC->GetAbilityTargetData(Handle, ActivationInfo, TargetData);

				// 遍历所有 TargetData。
				for (int32 TargetDataIdx = 0; TargetDataIdx < TargetData.Data.Num(); ++TargetDataIdx)
				{
					// 只要某个 TargetData 中包含 HitResult，就认为 Ability 命中了目标。
					if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetData, TargetDataIdx))
					{
						return true;
					}
				}
			}
		}

		// 没有找到有效命中结果。
		return false;
	};

	// 应用所有额外 Cost。
	// Pay any additional costs
	bool bAbilityHitTarget = false;

	// 标记是否已经计算过本次 Ability 是否命中。
	bool bHasDeterminedIfAbilityHitTarget = false;

	// 遍历所有额外 Cost。
	for (const TObjectPtr<UHodgeAbilityCost>& AdditionalCost : AdditionalCosts)
	{
		// 忽略空的 Cost 配置。
		if (AdditionalCost != nullptr)
		{
			// 当前 Cost 只有在 Ability 命中目标后才需要支付。
			if (AdditionalCost->ShouldOnlyApplyCostOnHit())
			{
				// 多个 OnHit Cost 共用同一次命中判断，避免重复查询 TargetData。
				if (!bHasDeterminedIfAbilityHitTarget)
				{
					// 第一次遇到 OnHit Cost 时计算是否命中。
					bAbilityHitTarget = DetermineIfAbilityHitTarget();

					// 标记命中结果已经计算完成。
					bHasDeterminedIfAbilityHitTarget = true;
				}

				// 没有命中目标时跳过这个 Cost。
				if (!bAbilityHitTarget)
				{
					continue;
				}
			}

			// 真正应用当前额外 Cost。
			AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
		}
	}
}

// 创建当前 Ability 使用的 GameplayEffectContext。
FGameplayEffectContextHandle UHodgeGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
                                                                      const FGameplayAbilityActorInfo* ActorInfo) const
{
	// 先使用 GAS 原生逻辑创建基础 EffectContext。
	FGameplayEffectContextHandle ContextHandle = Super::MakeEffectContext(Handle, ActorInfo);

	// 将基础 Context 转换成项目自定义的 GameplayEffectContext。
	FHodgeGameplayEffectContext* EffectContext = FHodgeGameplayEffectContext::ExtractEffectContext(ContextHandle);

	// 确保自定义 EffectContext 成功创建。
	check(EffectContext);

	// ActorInfo 在这里必须有效。
	check(ActorInfo);

	// Effect 的实际施加者 Actor。
	AActor* EffectCauser = nullptr;

	// Ability 的来源对象，例如武器、技能来源物体等。
	const IHodgeAbilitySourceInterface* AbilitySource = nullptr;

	// Ability 来源对象提供的等级。
	float SourceLevel = 0.0f;

	// 获取 Ability 的 Source、SourceLevel 和 EffectCauser。
	GetAbilitySource(Handle, ActorInfo, /*out*/ SourceLevel, /*out*/ AbilitySource, /*out*/ EffectCauser);

	// 获取 AbilitySpec 配置的 SourceObject。
	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

	// 获取 AbilitySystem 的 OwnerActor 作为 Instigator。
	AActor* Instigator = ActorInfo ? ActorInfo->OwnerActor.Get() : nullptr;

	// 将 AbilitySource 和来源等级写入自定义 EffectContext。
	EffectContext->SetAbilitySource(AbilitySource, SourceLevel);

	// 设置 Effect 的 Instigator 和实际造成效果的 Actor。
	EffectContext->AddInstigator(Instigator, EffectCauser);

	// 将 Ability 的 SourceObject 添加到 EffectContext。
	EffectContext->AddSourceObject(SourceObject);

	// 返回最终构建完成的 EffectContext。
	return ContextHandle;
}

// 将当前 Ability 的标签附加到 GameplayEffectSpec。
void UHodgeGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
                                                                 FGameplayAbilitySpec* AbilitySpec) const
{
	// 先执行 GAS 原生 AbilityTag → GameplayEffectSpec 的处理。
	Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);

	// 如果 EffectContext 中存在 HitResult，则可以根据命中的物理材质进一步添加标签。
	if (const FHitResult* HitResult = Spec.GetContext().GetHitResult())
	{
		// 当前物理材质标签扩展逻辑暂时被注释掉。
		// if (const UPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UPhysicalMaterialWithTags>(
		// 	HitResult->PhysMaterial.Get()))
		// {
		// 	Spec.CapturedTargetTags.GetSpecTags().AppendTags(PhysMatWithTags->Tags);
		// }
	}
}

// 检查 Ability 的 GameplayTag 激活条件。
bool UHodgeGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
                                                              const FGameplayTagContainer* SourceTags,
                                                              const FGameplayTagContainer* TargetTags,
                                                              OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	// 专门扩展 GAS 默认逻辑，用于处理死亡状态和项目自定义 AbilityTag 关系。
	// Specialized version to handle death exclusion and AbilityTags expansion via ASC

	// 标记 Ability 是否因为 Blocked Tag 而无法激活。
	bool bBlocked = false;

	// 标记 Ability 是否因为缺少 Required Tag 而无法激活。
	bool bMissing = false;

	// 获取全局 AbilitySystemGlobals。
	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();

	// 获取 GAS 默认的“被 Block”失败 Tag。
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;

	// 获取 GAS 默认的“缺少 Required Tag”失败 Tag。
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	// 检查当前 Ability 自身的 AssetTags 是否被 ASC 主动 Block。
	if (AbilitySystemComponent.AreAbilityTagsBlocked(GetAssetTags()))
	{
		// 当前 Ability 被 AbilityTag 阻塞。
		bBlocked = true;
	}

	// 尝试获取项目自定义 ASC，以便扩展 Ability 的激活 Tag 规则。
	const UHodgeAbilitySystemComponent* HodgeASC = Cast<UHodgeAbilitySystemComponent>(&AbilitySystemComponent);

	// 临时保存最终计算出来的 Required Tags。
	static FGameplayTagContainer AllRequiredTags;

	// 临时保存最终计算出来的 Blocked Tags。
	static FGameplayTagContainer AllBlockedTags;

	// 先复制 Ability 自身配置的 Required Tags。
	AllRequiredTags = ActivationRequiredTags;

	// 先复制 Ability 自身配置的 Blocked Tags。
	AllBlockedTags = ActivationBlockedTags;

	// 根据项目自定义的 AbilityTagRelationshipMapping 扩展 Required / Blocked Tags。
	if (HodgeASC)
	{
		HodgeASC->GetAdditionalActivationTagRequirements(GetAssetTags(), AllRequiredTags, AllBlockedTags);
	}

	// 检查最终计算得到的 Required / Blocked Tags。
	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		// 临时保存 ASC 当前拥有的全部 GameplayTag。
		static FGameplayTagContainer AbilitySystemComponentTags;

		// 清空上一次计算留下的标签。
		AbilitySystemComponentTags.Reset();

		// 获取 ASC 当前拥有的 GameplayTag。
		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		// 当前 ASC 只要拥有任意一个 BlockedTag，Ability 就不能激活。
		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			// 如果需要返回详细失败原因，并且当前角色处于死亡状态，则返回 IsDead。
			if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(HodgeGameplayTags::Status_Death))
			{
				// If player is dead and was rejected due to blocking tags, give that feedback
				OptionalRelevantTags->AddTag(HodgeGameplayTags::Ability_ActivateFail_IsDead);
			}

			// 标记 Ability 因 BlockedTag 激活失败。
			bBlocked = true;
		}

		// 当前 ASC 必须拥有所有 RequiredTag，否则 Ability 无法激活。
		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			// 标记 Ability 缺少 RequiredTag。
			bMissing = true;
		}
	}

	// 如果存在 SourceTags，则检查 Ability 来源对象的标签条件。
	if (SourceTags != nullptr)
	{
		// Source 只在存在 Required 或 Blocked 条件时进行检查。
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			// Source 拥有任意 BlockedTag 时，Ability 无法激活。
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			// Source 缺少任意 RequiredTag 时，Ability 无法激活。
			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	// 如果存在 TargetTags，则检查 Ability 目标对象的标签条件。
	if (TargetTags != nullptr)
	{
		// Target 只在存在 Required 或 Blocked 条件时进行检查。
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			// Target 拥有任意 BlockedTag 时，Ability 无法激活。
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			// Target 缺少任意 RequiredTag 时，Ability 无法激活。
			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	// Blocked 条件优先于 Missing 条件处理。
	if (bBlocked)
	{
		// 如果调用方需要失败原因，则添加 GAS 标准 Blocked 失败 Tag。
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}

		// Ability 被 Block，因此不能激活。
		return false;
	}

	// 没有 Block，但缺少 RequiredTag 时也不能激活。
	if (bMissing)
	{
		// 如果调用方需要失败原因，则添加 GAS 标准 Missing 失败 Tag。
		if (OptionalRelevantTags && MissingTag.IsValid())
		{
			OptionalRelevantTags->AddTag(MissingTag);
		}

		// Ability 缺少必要条件，因此不能激活。
		return false;
	}

	// 所有 AbilityTag 条件均满足。
	return true;
}

// Pawn Avatar 设置完成后调用。
void UHodgeGameplayAbility::OnPawnAvatarSet()
{
	// 通知 Blueprint：当前 Ability 已经拥有有效的 Pawn Avatar。
	K2_OnPawnAvatarSet();
}

// 获取当前 Ability 的来源对象以及 EffectCauser。
void UHodgeGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel,
                                             const IHodgeAbilitySourceInterface*& OutAbilitySource,
                                             AActor*& OutEffectCauser) const
{
	// 默认来源等级为 0。
	OutSourceLevel = 0.0f;

	// 默认没有 AbilitySource。
	OutAbilitySource = nullptr;

	// 默认没有 EffectCauser。
	OutEffectCauser = nullptr;

	// 默认使用当前 Ability 的 Avatar 作为实际造成效果的 Actor。
	OutEffectCauser = ActorInfo->AvatarActor.Get();

	// If we were added by something that's an ability info source, use it
	// 获取授予该 Ability 的 SourceObject。
	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

	// 如果 SourceObject 实现了 AbilitySourceInterface，则将其作为 AbilitySource。
	OutAbilitySource = Cast<IHodgeAbilitySourceInterface>(SourceObject);
}

// 如果 Ability 配置为 OnSpawn，则在 Avatar 设置后尝试自动激活。
void UHodgeGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,
                                                      const FGameplayAbilitySpec& Spec) const
{
	// Try to activate if activation policy is on spawn.
	// 只有 AvatarInfo 有效、Ability 当前未激活并且激活策略为 OnSpawn 时才尝试激活。
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EHodgeAbilityActivationPolicy::OnSpawn))
	{
		// 获取当前 Ability 对应的 ASC。
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

		// 获取当前 Ability 对应的 Avatar Actor。
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// 如果 Avatar 已经 TearOff 或即将死亡，则暂时不要自动激活。
		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			// LocalPredicted / LocalOnly 表示该 Ability 允许本地执行。
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) ||
				(NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);

			// ServerOnly / ServerInitiated 表示该 Ability 需要服务器执行。
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (
				NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			// 当前 Actor 被本地控制，并且 Ability 允许本地执行。
			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;

			// 当前 Actor 位于服务器，并且 Ability 允许服务器执行。
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			// 满足客户端执行条件或服务器执行条件时尝试激活 Ability。
			if (bClientShouldActivate || bServerShouldActivate)
			{
				// 通过 ASC 使用当前 AbilitySpecHandle 发起 Ability 激活。
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

// 判断 Ability 是否可以切换到指定的 ActivationGroup。
bool UHodgeGameplayAbility::CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const
{
	// 只有已经实例化并且当前正在运行的 Ability 才允许切换 ActivationGroup。
	if (!IsInstantiated() || !IsActive())
	{
		return false;
	}

	// 已经处于目标 Group 时无需切换，直接认为成功。
	if (ActivationGroup == NewGroup)
	{
		return true;
	}

	// 获取当前 Ability 对应的项目自定义 ASC。
	UHodgeAbilitySystemComponent* HodgeASC = GetHodgeAbilitySystemComponentFromActorInfo();
	check(HodgeASC);

	// 非 Blocking Ability 如果想切换到一个被阻塞的 Group，则不允许切换。
	if ((ActivationGroup != EHodgeAbilityActivationGroup::Exclusive_Blocking) && HodgeASC->
		IsActivationGroupBlocked(NewGroup))
	{
		// This ability can't change groups if it's blocked (unless it is the one doing the blocking).
		return false;
	}

	// 想成为 Replaceable 时，当前 Ability 必须允许被取消。
	if ((NewGroup == EHodgeAbilityActivationGroup::Exclusive_Replaceable) && !CanBeCanceled())
	{
		// This ability can't become replaceable if it can't be canceled.
		return false;
	}

	// 所有切换条件均满足。
	return true;
}

// 尝试修改当前 Ability 的 ActivationGroup。
bool UHodgeGameplayAbility::ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup)
{
	// 当前 Ability 必须已经实例化，否则不能修改运行时状态。
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ChangeActivationGroup, false);

	// 目标 ActivationGroup 不满足切换条件时直接失败。
	if (!CanChangeActivationGroup(NewGroup))
	{
		return false;
	}

	// 只有实际发生 Group 变化时才更新 ASC 中的注册关系。
	if (ActivationGroup != NewGroup)
	{
		// 获取当前 Ability 对应的项目自定义 ASC。
		UHodgeAbilitySystemComponent* HodgeASC = GetHodgeAbilitySystemComponentFromActorInfo();
		check(HodgeASC);

		// 从旧 ActivationGroup 中移除当前 Ability。
		HodgeASC->RemoveAbilityFromActivationGroup(ActivationGroup, this);

		// 将当前 Ability 加入新的 ActivationGroup。
		HodgeASC->AddAbilityToActivationGroup(NewGroup, this);

		// 最后更新 Ability 自身记录的 ActivationGroup。
		ActivationGroup = NewGroup;
	}

	// 成功完成 Group 切换。
	return true;
}

// 设置当前 Ability 使用的 CameraMode。
void UHodgeGameplayAbility::SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode)
{
	// 当前 Ability 必须已经实例化，因为 CameraMode 属于 Ability 的运行时状态。
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(SetCameraMode,);

	//当前 CameraMode 系统暂时被注释掉，预留给 HeroComponent 统一管理。
	if (UHodgeHeroComponent* HeroComponent = GetHeroComponentFromActorInfo())
	{
		HeroComponent->SetAbilityCameraMode(CameraMode, CurrentSpecHandle);
		ActiveCameraMode = CameraMode;
	}
}

// 清除当前 Ability 设置的 CameraMode。
void UHodgeGameplayAbility::ClearCameraMode()
{
	// 当前 Ability 必须已经实例化，因为需要访问运行时 CameraMode 状态。
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ClearCameraMode,);

	// 如果存在激活的 CameraMode，则通知 HeroComponent 清理。
	if (ActiveCameraMode)
	{
		if (UHodgeHeroComponent* HeroComponent = GetHeroComponentFromActorInfo())
		{
			HeroComponent->ClearAbilityCameraMode(CurrentSpecHandle);
		}

		ActiveCameraMode = nullptr;
	}
}
