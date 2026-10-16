/** @file HodgeGameplayAbility_Melee.cpp
 * @brief 实现玩法能力Melee相关的类型、接口与现有处理流程。
 */
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "UObject/StrongObjectPtr.h"
#include "Component/HodgeDefenseComponent.h"
#include "AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "AbilitySystem/Executions/HodgeDamageExecution.h"
#include "AbilitySystem/HodgeGameplayEffectContext.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Combat/HodgeDamageRules.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Component/HodgeHitReactionComponent.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Components/SceneComponent.h"
#include "Equipment/HodgeWeaponInstance.h"
#include "Engine/World.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Melee)

/** 构造玩法能力Melee并初始化其默认状态。 */
UHodgeGameplayAbility_Melee::UHodgeGameplayAbility_Melee()
{
	FGameplayTagContainer Tags = GetAssetTags();
	Tags.AddTag(HodgeGameplayTags::Ability_Type_Action_Melee);
	SetAssetTags(Tags);
}

/** 根据目标资格与当前检测会话判断是否允许命中。 */
bool FHodgeMeleeHitHistory::CanHit(UAbilitySystemComponent* Target, double Time, float RepeatInterval) const
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsValid(Target) || !FMath::IsFinite(Time) || !FMath::IsFinite(RepeatInterval) || RepeatInterval < 0.f)
	{
		return false;
	}
	const double* LastHit = LastHitTimes.Find(TWeakObjectPtr<UAbilitySystemComponent>(Target));
	return !LastHit || (RepeatInterval > 0.f && Time - *LastHit >= RepeatInterval);
}

/** 记录本次检测会话已命中的目标以供去重。 */
void FHodgeMeleeHitHistory::RecordHit(UAbilitySystemComponent* Target, double Time)
{
	LastHitTimes.Add(TWeakObjectPtr<UAbilitySystemComponent>(Target), Time);
}

/** 校验执行Configuration。 */
void UHodgeGameplayAbility_Melee::ValidateExecutionConfiguration(
	const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const
{
	auto ValidateEffect = [&Errors](const FHodgeHitEffectConfig& Binding)
	{
		const UGameplayEffect* Effect = Binding.DamageEffect.GetDefaultObject();
		// 默认生命伤害的 GE 契约由近战能力校验，派生能力可以定义其他效果。
		if (!Effect || Effect->DurationPolicy != EGameplayEffectDurationType::Instant ||
			!Effect->Executions.ContainsByPredicate([](const FGameplayEffectExecutionDefinition& Entry)
			{
				return Entry.CalculationClass && Entry.CalculationClass->IsChildOf(
					UHodgeDamageExecution::StaticClass());
			}))
		{
			Errors.Add(FText::FromString(TEXT(
				"Configure DamageEffect with GameplayEffectParent_Damage_Basic or an Instant GE containing HodgeDamageExecution; override ability validation for other effects.")));
		}
	};
	if (Definition.DefaultHitConfig.DamageEffect) { ValidateEffect(Definition.DefaultHitConfig); }
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Definition.ExecutionConfig.Montage)
	{
		for (const FAnimNotifyEvent& Event : Definition.ExecutionConfig.Montage->Notifies)
		{
			const FHodgeAnimHitConfig* Config = nullptr;
			if (const auto* Notify = Cast<UHodgeAnimNotifyState_HitCheck>(Event.NotifyStateClass))
			{
				Config = &Notify->Hit;
			}
			if (const auto* Notify = Cast<UHodgeAnimNotify_Hit>(Event.Notify)) { Config = &Notify->Hit; }
			if (!Config) { continue; }
			Config->Validate(Errors);
			ValidateEffect(Config->bUseDefaultDamage ? Definition.DefaultHitConfig : Config->Damage);
			if (const auto* Notify = Cast<UHodgeAnimNotify_Hit>(Event.Notify))
			{
				for (const auto& Additional : Notify->AdditionalHits)
				{
					Additional.Validate(Errors);
					ValidateEffect(Additional.bUseDefaultDamage ? Definition.DefaultHitConfig : Additional.Damage);
				}
				for (const FAnimNotifyEvent& Other : Definition.ExecutionConfig.Montage->Notifies)
				{
					if (&Other == &Event || !Other.IsBranchingPoint()) { continue; }
					if (Other.GetTriggerTime() == Event.GetTriggerTime() || (Other.NotifyStateClass && Other.
						GetEndTriggerTime() == Event.GetTriggerTime()))
					{
						Errors.Add(FText::FromString(TEXT(
							"A point hit cannot overlap another Branching Point marker: use AdditionalHits in one point or place the marker at another time.")));
					}
				}
			}
		}
	}
}

/** 建立当前服务器命中执行所需的配置与身份。 */
bool UHodgeGameplayAbility_Melee::PrepareHitExecutionContext_Implementation() { return true; }

/** 设置命中锚点。 */
bool UHodgeGameplayAbility_Melee::SetHitAnchor(FName Key, const FTransform& WorldTransform)
{
	// 按服务器权限区分本次处理分支。
	if (!IsActive() || IsExecutionEnding() || !GetExecutionId().IsValid() || !CurrentActorInfo || !CurrentActorInfo->
		IsNetAuthority() ||
		Key.IsNone() || WorldTransform.ContainsNaN() || !WorldTransform.GetRotation().IsNormalized() ||
		!WorldTransform.GetScale3D().Equals(FVector::OneVector)) { return false; }
	HitAnchors.Add(Key, WorldTransform);
	if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo()))
	{
		Combat->UpdateDetectionAnchor(GetExecutionId(), Key, WorldTransform);
	}
	return true;
}

/** 设置命中目标。 */
bool UHodgeGameplayAbility_Melee::SetHitTarget(FName Key, AActor* Target)
{
	// 按服务器权限区分本次处理分支。
	if (!IsActive() || IsExecutionEnding() || !GetExecutionId().IsValid() || !CurrentActorInfo || !CurrentActorInfo->
		IsNetAuthority() ||
		Key.IsNone() || !FHodgeDamageRules::CanDamage(GetAvatarActorFromActorInfo(), Target, true) ||
		Target->GetWorld() != GetWorld()) { return false; }
	HitTargets.Add(Key, Target);
	return true;
}

/** 重置命中几何历史。 */
void UHodgeGameplayAbility_Melee::ResetHitGeometryHistory()
{
	// 按服务器权限区分本次处理分支。
	if (!IsActive() || IsExecutionEnding() || !CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) { return; }
	if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo()))
	{
		for (const auto& Pair : HitStates)
		{
			Combat->ResetDetectionHistory(Pair.Value.SessionHandle, GetExecutionId());
		}
	}
}

/** 处理执行就绪。 */
void UHodgeGameplayAbility_Melee::OnExecutionReady()
{
	Super::OnExecutionReady();
	HitStates.Reset();
	HitGroups.Reset();
	HitAnchors.Reset();
	HitTargets.Reset();
	// 当前操作使用的定义配置。
	const UHodgeAbilityDefinition* Definition = GetDefinition();
	// 按服务器权限区分本次处理分支。
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority() || !Definition) { return; }
	if (!PrepareHitExecutionContext())
	{
		FinishExecution(true, true);
		return;
	}
	// 根据当前生命周期状态决定是否继续执行。
	if (!IsActive() || IsExecutionEnding()) { return; }
	AActor* Avatar = GetAvatarActorFromActorInfo();
	// 统一战斗组件。
	auto* Combat = Avatar ? Avatar->FindComponentByClass<UHodgeCombatComponentBase>() : nullptr;
	if (!Combat)
	{
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "Melee ability [%s] requires an Experience AddComponents action providing CombatComponent on its Avatar."
		       ), *GetPathName());
		FinishExecution(true, true);
		return;
	}
	DetectionTask = UHodgeAbilityTask_WaitHitResults::WaitHitResults(this, Combat, GetExecutionId());
	DetectionTask->OnHitResults.AddUObject(this, &ThisClass::OnHitResults);
	DetectionTask->ReadyForActivation();
}

/** 处理执行Ending。 */
void UHodgeGameplayAbility_Melee::OnExecutionEnding(const FGuid& EndingExecutionId)
{
	// 基类已失效执行身份，Task 清理不会再消费旧批次。
	if (DetectionTask)
	{
		DetectionTask->OnHitResults.RemoveAll(this);
		DetectionTask->EndTask();
		DetectionTask = nullptr;
	}
	HitStates.Reset();
	HitGroups.Reset();
	HitAnchors.Reset();
	HitTargets.Reset();
	Super::OnExecutionEnding(EndingExecutionId);
}


/** 开始通知命中。 */
bool UHodgeGameplayAbility_Melee::BeginNotifyHit(int32 OccurrenceId, const FHodgeAnimHitConfig& Config,
                                                 USkeletalMeshComponent* Mesh, bool bSingle)
{
	// 按服务器权限区分本次处理分支。
	if (OccurrenceId == INDEX_NONE || !CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) { return false; }
	TArray<FText> Errors;
	if (!Config.Validate(Errors)) { return false; }
	// 当前操作使用的定义配置。
	const auto* Definition = GetDefinition();
	FHodgeHitEffectConfig Effect = Config.bUseDefaultDamage && Definition
		                               ? Definition->DefaultHitConfig
		                               : Config.Damage;
	Effect.Reaction = Config.bUseDefaultReaction && Definition
		                  ? Definition->DefaultHitConfig.Reaction
		                  : Config.ReactionOverride;
	Effect.DamageMultiplier *= Config.DamageScale;
	Effect.RepeatHitInterval = Config.RepeatHitInterval;
	Effect.HitGroup = Config.HitGroup;
	Effect.AttackPhase = Config.AttackPhase;
	Effect.bAllowFriendlyFire = Config.bAllowFriendlyFire;
	Effect.TargetPolicy = Config.TargetPolicy;
	if (!Effect.DamageEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("Notify hit has no DamageEffect: %s"), *GetNameSafe(Definition));
		return false;
	}
	FHodgeHitDetectionRequest Request;
	Request.Profile = Config.Profile
		                  ? Config.Profile.Get()
		                  : const_cast<UHodgeHitDetectionProfile*>(GetDefault<UHodgeHitDetectionProfile>());
	if (bSingle && !Config.Profile)
	{
		auto* Query = NewObject<UHodgeHitDetectionProfile>(this);
		Query->QueryMode = EHodgeHitQueryMode::Overlap;
		Query->SampleMode = EHodgeHitSampleMode::OnceOnEnter;
		Request.Profile = Query;
	}
	Request.Volume.GeometryMode = Config.Source == EHodgeAnimHitSource::EquippedWeapon
		                              ? EHodgeHitGeometryMode::ExistingSource
		                              : EHodgeHitGeometryMode::ConfiguredShape;
	Request.Volume.Shape = Config.Shape;
	Request.Volume.SphereRadius = Config.Radius;
	Request.Volume.CapsuleRadius = Config.Radius;
	Request.Volume.CapsuleHalfHeight = Config.CapsuleHalfHeight;
	Request.Volume.BoxHalfExtent = Config.BoxHalfExtent;
	Request.Volume.LocalTransform = Config.LocalTransform;
	Request.Volume.TransformPolicy = Config.TransformPolicy;
	Request.Volume.MaxAnchorDistance = Config.MaxAnchorDistance;
	Request.Volume.AnchorKey = Config.AnchorKey;
	Request.TargetPolicy = Config.TargetPolicy;
	Request.MaxTargetDistance = Config.MaxTargetDistance;
	if (const FTransform* Anchor = HitAnchors.Find(Config.AnchorKey))
	{
		Request.RuntimeAnchor = *Anchor;
		Request.bHasRuntimeAnchor = true;
	}
	if (const auto* Target = HitTargets.Find(Config.TargetKey)) { Request.RuntimeTarget = *Target; }
	UE_LOG(LogTemp, Verbose, TEXT("[HodgeNotify] Open %d Target=%s Policy=%d"), OccurrenceId,
	       *GetNameSafe(Request.RuntimeTarget.Get()), int32(Config.TargetPolicy));
	if (Config.Source == EHodgeAnimHitSource::EquippedWeapon)
	{
		Request.SourceTag = Config.WeaponSourceTag;
	}
	else if (Config.Source == EHodgeAnimHitSource::CharacterMeshSocket || Config.Source ==
		EHodgeAnimHitSource::NamedComponent)
	{
		Request.Volume.AnchorKind = EHodgeHitAnchorKind::RegisteredSource;
		Request.Volume.AnchorSocket = Config.BoneOrSocket;
		if (Config.Source == EHodgeAnimHitSource::CharacterMeshSocket) { Request.DirectComponent = Mesh; }
		else
		{
			TInlineComponentArray<USceneComponent*> Components(GetAvatarActorFromActorInfo());
			for (auto* Component : Components)
			{
				if (Component->GetFName() == Config.ComponentName)
				{
					Request.DirectComponent = Component;
					break;
				}
			}
		}
		if (!Request.DirectComponent.IsValid()) { return false; }
		Request.DirectSource.SourceTag = FGameplayTag::RequestGameplayTag(TEXT("Combat.Source.Body"), false);
	}
	else if (Config.Source == EHodgeAnimHitSource::ExecutionAnchor)
	{
		Request.Volume.AnchorKind = EHodgeHitAnchorKind::ExecutionTransform;
	}
	else if (Config.Source == EHodgeAnimHitSource::ExecutionTarget)
	{
		Request.Volume.AnchorKind = EHodgeHitAnchorKind::ExecutionTarget;
	}
	else { Request.Volume.AnchorKind = EHodgeHitAnchorKind::AvatarRoot; }
	if ((Config.TargetPolicy != EHodgeHitTargetPolicy::AnyInVolume || Config.Source ==
			EHodgeAnimHitSource::ExecutionTarget) &&
		!FHodgeDamageRules::CanDamage(GetAvatarActorFromActorInfo(), Request.RuntimeTarget.Get(),
		                              Config.bAllowFriendlyFire)) { return false; }
	if (!OpenHit(OccurrenceId, Effect, Request)) { return false; }
	if (bSingle) { DetectionTask->CloseWindow(OccurrenceId, true); }
	return true;
}

/** 打开命中。 */
bool UHodgeGameplayAbility_Melee::OpenHit(int32 OccurrenceId, const FHodgeHitEffectConfig& Binding,
                                          const FHodgeHitDetectionRequest& Request)
{
	if (!DetectionTask || !DetectionTask->IsRunningForExecution(GetExecutionId()) || HitStates.Contains(OccurrenceId))
	{
		return false;
	}
	const uint64 Handle = DetectionTask->CreateWindow(OccurrenceId, Request);
	UE_LOG(LogTemp, Verbose, TEXT("[HodgeNotify] Session %d Handle=%llu"), OccurrenceId, Handle);
	if (Handle == 0) { return false; }
	FHodgeMeleeHitState State;
	State.Binding = Binding;
	State.SessionHandle = Handle;
	if (Binding.HitGroup.IsNone()) { State.History = MakeShared<FHodgeMeleeHitHistory>(); }
	else
	{
		FHodgeHitGroupKey Key;
		Key.Group = Binding.HitGroup;
		Key.Phase = Binding.AttackPhase;
		TSharedPtr<FHodgeMeleeHitHistory>& History = HitGroups.FindOrAdd(Key);
		if (!History) { History = MakeShared<FHodgeMeleeHitHistory>(); }
		State.History = History;
	}
	HitStates.Add(OccurrenceId, MoveTemp(State));
	return true;
}

/** 处理通知资源结束。 */
void UHodgeGameplayAbility_Melee::OnNotifyResourceEnded(int32 OccurrenceId)
{
	if (DetectionTask) { DetectionTask->CloseWindow(OccurrenceId, true); }
}

/** 判断MeleeBatch当前。 */
bool UHodgeGameplayAbility_Melee::IsMeleeBatchCurrent(const FHodgeHitDetectionBatch& Batch) const
{
	const FHodgeMeleeHitState* State = HitStates.Find(Batch.OccurrenceId);
	return IsActive() && !IsExecutionEnding() && CurrentActorInfo && CurrentActorInfo->IsNetAuthority() &&
		DetectionTask && DetectionTask->IsBatchCurrent(Batch) && State && State->SessionHandle == Batch.SessionHandle;
}

/** 处理命中Results。 */
void UHodgeGameplayAbility_Melee::OnHitResults(const FHodgeHitDetectionBatch& Batch)
{
	if (!IsMeleeBatchCurrent(Batch)) { return; }
	FHodgeMeleeHitState& State = HitStates.FindChecked(Batch.OccurrenceId);
	if (Batch.SampleSequence <= State.LastSampleSequence) { return; }
	State.LastSampleSequence = Batch.SampleSequence;
	// 权限和身份检查先于蓝图事件，覆盖事件不能收到客户端或过期批次。
	ProcessMeleeHitResults(Batch);
}

/** 检查该候选命中是否满足目标与执行约束。 */
bool UHodgeGameplayAbility_Melee::CanApplyMeleeHit_Implementation(AActor* Target, const FHitResult& Hit,
                                                                  const FHodgeHitEffectConfig& Binding) const
{
	return FHodgeDamageRules::CanDamage(GetAvatarActorFromActorInfo(), Target, Binding.bAllowFriendlyFire);
}

/** 构造Melee命中上下文。 */
FGameplayEffectContextHandle UHodgeGameplayAbility_Melee::MakeMeleeHitContext(
	const FHodgeHitDetectionBatch& Batch, const FHitResult& Hit) const
{
	FGameplayEffectContextHandle Context = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
	Context.AddHitResult(Hit, true);
	Context.AddOrigin(Batch.SourceOrigin);
	// 武器只有实现来源接口时才替换衰减来源，不覆盖 AbilitySpec 的授予来源。
	if (Batch.Weapon.IsValid())
	{
		if (const auto* Source = Cast<IHodgeAbilitySourceInterface>(Batch.Weapon.Get()))
		{
			if (auto* Typed = FHodgeGameplayEffectContext::ExtractEffectContext(Context))
			{
				Typed->SetAbilitySource(Source, GetAbilityLevel());
				Context.AddSourceObject(Batch.Weapon.Get());
			}
		}
	}
	return Context;
}

/** 根据本次命中上下文构造实际效果规格。 */
FGameplayEffectSpecHandle UHodgeGameplayAbility_Melee::BuildMeleeHitSpec(const FHodgeHitDetectionBatch& Batch,
                                                                         const FHitResult& Hit,
                                                                         const FHodgeHitEffectConfig& Binding) const
{
	// 后续GAS操作使用的能力系统组件。
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	// 按服务器权限区分本次处理分支。
	if (!ASC || !CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) { return {}; }
	// 使用窗口显式配置的项目伤害 GE，缺少配置时不创建替代效果。
	if (!Binding.DamageEffect) { return {}; }
	FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Binding.DamageEffect, GetAbilityLevel(),
	                                                       MakeMeleeHitContext(Batch, Hit));
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Spec.IsValid()) { return {}; }
	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(CurrentSpecHandle);
	// 完整 HitResult 先进入 Context，再执行 GA 的标签扩展与授予参数初始化。
	ApplyAbilityTagsToGameplayEffectSpec(*Spec.Data.Get(), AbilitySpec);
	if (AbilitySpec) { Spec.Data->SetByCallerTagMagnitudes = AbilitySpec->SetByCallerTagMagnitudes; }
	Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_DamageMultiplier, Binding.DamageMultiplier);
	if (Binding.DamageType.IsValid()) { Spec.Data->AddDynamicAssetTag(Binding.DamageType); }
	if (Binding.DamageElement.IsValid()) { Spec.Data->AddDynamicAssetTag(Binding.DamageElement); }
	if (Binding.bAllowFriendlyFire)
	{
		Spec.Data->AddDynamicAssetTag(HodgeGameplayTags::GameplayEffect_Damage_AllowFriendlyFire);
	}
	return Spec;
}

/** 消费检测得到的命中结果并提交允许的命中效果。 */
void UHodgeGameplayAbility_Melee::ProcessMeleeHitResults_Implementation(const FHodgeHitDetectionBatch& Batch)
{
	if (!IsMeleeBatchCurrent(Batch)) { return; }
	const FHodgeMeleeHitState Snapshot = HitStates.FindChecked(Batch.OccurrenceId);
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC || !SourceASC->GetSet<UHodgeCombatSet>())
	{
		UE_LOG(LogTemp, Error, TEXT("Default melee damage [%s] requires CombatSet on the source ASC."), *GetPathName());
		return;
	}
	TSet<TWeakObjectPtr<UAbilitySystemComponent>> BatchTargets;
	for (const FHitResult& Hit : Batch.Hits)
	{
		if (!IsMeleeBatchCurrent(Batch)) { break; }
		AActor* HitActor = Hit.GetActor();
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
		AActor* Target = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
		if (Snapshot.Binding.TargetPolicy != EHodgeHitTargetPolicy::AnyInVolume && Target != HitActor) { continue; }
		if (!IsValid(Target) || TargetASC == SourceASC || Target == GetAvatarActorFromActorInfo() ||
			!Hit.GetComponent() || BatchTargets.Contains(TWeakObjectPtr<UAbilitySystemComponent>(TargetASC)))
		{
			continue;
		}
		if (!CanApplyMeleeHit(Target, Hit, Snapshot.Binding)) { continue; }
		if (!IsMeleeBatchCurrent(Batch)) { break; }
		if (!IsValid(TargetASC) || !IsValid(Target) || TargetASC->GetAvatarActor() != Target) { continue; }
		if (!Snapshot.History->CanHit(TargetASC, Batch.SampleTime, Snapshot.Binding.RepeatHitInterval)) { continue; }
		FGameplayEffectSpecHandle Spec = BuildMeleeHitSpec(Batch, Hit, Snapshot.Binding);
		if (!IsMeleeBatchCurrent(Batch)) { break; }
		if (!Spec.IsValid() || !IsValid(TargetASC) || !IsValid(Target) || TargetASC->GetAvatarActor() != Target)
		{
			continue;
		}
		// 先占用机会再进入 GAS，免疫或回调重入也不会重复处理本次接触。
		Snapshot.History->RecordHit(TargetASC, Batch.SampleTime);
		BatchTargets.Add(TWeakObjectPtr<UAbilitySystemComponent>(TargetASC));
		SubmitMeleeHitEffects(Batch, Hit, Snapshot.Binding, TargetASC, Spec);
	}
}

/** 提交Melee命中Effects。 */
void UHodgeGameplayAbility_Melee::SubmitMeleeHitEffects(const FHodgeHitDetectionBatch& Batch,
                                                        const FHitResult& Hit, const FHodgeHitEffectConfig& Binding,
                                                        UAbilitySystemComponent* TargetASC,
                                                        const FGameplayEffectSpecHandle& Spec)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsMeleeBatchCurrent(Batch) || !IsValid(TargetASC) || !Spec.IsValid()) { return; }
	// GE 回调可撤销本能力并触发 GC，伤害确认完成前保持当前实例存活。
	TStrongObjectPtr<UHodgeGameplayAbility_Melee> KeepSource(this);
	TWeakObjectPtr<AActor> Avatar = TargetASC->GetAvatarActor();
	if (auto* Defense = Avatar.IsValid() ? Avatar->FindComponentByClass<UHodgeDefenseComponent>() : nullptr)
	{
		if (Defense->ResolveIncomingHit(GetAvatarActorFromActorInfo(), Batch.ExecutionId, Batch.OccurrenceId,
		                                Binding.bCanBeDodged,
		                                Binding.bCanTriggerPerfectDodge) != EHodgeIncomingHitOutcome::Allowed)
		{
			return;
		}
	}
	TWeakObjectPtr<UHodgeHitReactionComponent> Reaction = Avatar.IsValid()
		                                                      ? Avatar->FindComponentByClass<
			                                                      UHodgeHitReactionComponent>()
		                                                      : nullptr;
	const FGuid Id = Reaction.IsValid()
		                 ? Reaction->BeginHit(Binding.Reaction, GetAvatarActorFromActorInfo(), Batch.SourceOrigin,
		                                      Batch.ResultKind == EHodgeHitResultKind::Sweep ? &Hit : nullptr,
		                                      Batch.ExecutionId, Binding.AttackPhase)
		                 : FGuid();
	const FGameplayEffectContextHandle SettlementContext = Spec.Data->GetContext();
	auto* Context = FHodgeGameplayEffectContext::ExtractEffectContext(SettlementContext);
	const FGuid PreviousId = Context ? Context->HitSettlementId : FGuid();
	const FGuid SettlementId = Id.IsValid() ? Id : FGuid::NewGuid();
	if (Context) { Context->HitSettlementId = SettlementId; }
	float AcceptedDamage = 0.f;
	// 只有显式配置规则的动作观察实际伤害，普通 NPC 不创建临时观察者。
	TStrongObjectPtr<UHodgeHealthSet> HealthSet(HasAttackSettlementRules()
		                                            ? const_cast<UHodgeHealthSet*>(TargetASC->GetSet<UHodgeHealthSet>())
		                                            : nullptr);
	FDelegateHandle Observer;
	if (HealthSet.Get())
	{
		// 临时观察同一结算GUID的真实生命扣除，目标死亡仍可确认有效命中。
		Observer = HealthSet->OnDamageAccepted.AddLambda(
			[&AcceptedDamage,SettlementId](AActor*, AActor*, const FGameplayEffectSpec* EventSpec, float Amount, float,
			                               float)
			{
				const auto* EventContext = EventSpec
					                           ? FHodgeGameplayEffectContext::ExtractEffectContext(
						                           EventSpec->GetContext())
					                           : nullptr;
				if (EventContext && EventContext->HitSettlementId == SettlementId && FMath::IsFinite(Amount) && Amount >
					0.f) { AcceptedDamage += Amount; }
			});
	}
	ApplyMeleeHitEffects(Batch, Hit, Binding, TargetASC, Spec);
	if (HealthSet.Get()) { HealthSet->OnDamageAccepted.Remove(Observer); }
	if (Context) { Context->HitSettlementId = PreviousId; }
	if (IsMeleeBatchCurrent(Batch) && HasAttackSettlementRules())
	{
		FHodgeAttackSettlement Settlement;
		Settlement.ExecutionId = Batch.ExecutionId;
		Settlement.SettlementId = SettlementId;
		Settlement.AbilityHandle = GetCurrentAbilitySpecHandle();
		Settlement.HitGroup = Binding.HitGroup;
		Settlement.AttackPhase = Binding.AttackPhase;
		Settlement.TargetASC = TargetASC;
		Settlement.AcceptedDamage = AcceptedDamage;
		PublishAttackSettlement(Settlement);
	}
	if (Reaction.IsValid() && Id.IsValid())
	{
		const FHodgeHitReactionResult Result = Reaction->FinishHit(Id);
		if (Avatar.IsValid() && IsMeleeBatchCurrent(Batch)) { OnMeleeReactionResolved(Avatar.Get(), Result); }
	}
}

/** 按命中配置提交伤害效果与附带反应。 */
void UHodgeGameplayAbility_Melee::ApplyMeleeHitEffects_Implementation(const FHodgeHitDetectionBatch& Batch,
                                                                      const FHitResult& Hit,
                                                                      const FHodgeHitEffectConfig& Binding,
                                                                      UAbilitySystemComponent* TargetASC,
                                                                      const FGameplayEffectSpecHandle& Spec)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsMeleeBatchCurrent(Batch) || !IsValid(TargetASC) || !Spec.IsValid()) { return; }
	UE_LOG(LogTemp, Verbose, TEXT("[HodgeNotify] Apply %d Target=%s"), Batch.OccurrenceId,
	       *GetNameSafe(TargetASC->GetAvatarActor()));
	// 使用目标 ASC 的 Avatar，Context 中仍保留原碰撞命中信息。
	auto* Target = new FGameplayAbilityTargetData_ActorArray();
	// ActorArray 写回 Context 的 Origin，必须使用本次采样来源而非默认零坐标。
	Target->SourceLocation.LiteralTransform = FTransform(Batch.SourceOrigin);
	Target->TargetActorArray.Add(TargetASC->GetAvatarActor());
	FGameplayAbilityTargetDataHandle TargetData(Target);
	// 将构造的效果规格提交给GAS执行属性更新。
	ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, Spec, TargetData);
}
