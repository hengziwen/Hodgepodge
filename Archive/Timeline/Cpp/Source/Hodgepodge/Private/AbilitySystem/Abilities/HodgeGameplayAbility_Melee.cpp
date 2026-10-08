#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
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
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeAbilityTimeline.h"
#include "Equipment/HodgeWeaponInstance.h"
#include "Engine/World.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_Melee)

bool FHodgeMeleeHitHistory::CanHit(UAbilitySystemComponent* Target, double Time, float RepeatInterval) const
{
	if (!IsValid(Target) || !FMath::IsFinite(Time) || !FMath::IsFinite(RepeatInterval) || RepeatInterval < 0.f) { return false; }
	const double* LastHit = LastHitTimes.Find(TWeakObjectPtr<UAbilitySystemComponent>(Target));
	return !LastHit || (RepeatInterval > 0.f && Time - *LastHit >= RepeatInterval);
}

void FHodgeMeleeHitHistory::RecordHit(UAbilitySystemComponent* Target, double Time)
{
	LastHitTimes.Add(TWeakObjectPtr<UAbilitySystemComponent>(Target), Time);
}

void UHodgeGameplayAbility_Melee::ValidateExecutionConfiguration(
	const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const
{
	auto ValidateEffect = [&Errors](const FHodgeHitEffectConfig& Binding)
	{
		const UGameplayEffect* Effect = Binding.DamageEffect.GetDefaultObject();
		// 默认生命伤害的 GE 契约由近战能力校验，派生能力可以定义其他效果。
		if (!Effect || Effect->DurationPolicy != EGameplayEffectDurationType::Instant ||
			!Effect->Executions.ContainsByPredicate([](const FGameplayEffectExecutionDefinition& Entry)
			{ return Entry.CalculationClass && Entry.CalculationClass->IsChildOf(UHodgeDamageExecution::StaticClass()); }))
		{
			Errors.Add(FText::FromString(TEXT("Configure DamageEffect with GameplayEffectParent_Damage_Basic or an Instant GE containing HodgeDamageExecution; override ability validation for other effects.")));
		}
	};
	for (const auto& Binding : Definition.HitWindows) { ValidateEffect(Binding); }
	for (const auto& Binding : Definition.HitPoints) { ValidateEffect(Binding); }
}

bool UHodgeGameplayAbility_Melee::PrepareHitExecutionContext_Implementation() { return true; }

bool UHodgeGameplayAbility_Melee::SetHitAnchor(FName Key, const FTransform& WorldTransform)
{
	if (!IsActive() || IsExecutionEnding() || !GetExecutionId().IsValid() || !CurrentActorInfo || !CurrentActorInfo->IsNetAuthority() ||
		Key.IsNone() || WorldTransform.ContainsNaN() || !WorldTransform.GetRotation().IsNormalized() ||
		!WorldTransform.GetScale3D().Equals(FVector::OneVector)) { return false; }
	HitAnchors.Add(Key, WorldTransform);
	if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo()))
	{
		Combat->UpdateDetectionAnchor(GetExecutionId(), Key, WorldTransform);
	}
	return true;
}

bool UHodgeGameplayAbility_Melee::SetHitTarget(FName Key, AActor* Target)
{
	if (!IsActive() || IsExecutionEnding() || !GetExecutionId().IsValid() || !CurrentActorInfo || !CurrentActorInfo->IsNetAuthority() ||
		Key.IsNone() || !FHodgeDamageRules::CanDamage(GetAvatarActorFromActorInfo(), Target, true) ||
		Target->GetWorld() != GetWorld()) { return false; }
	HitTargets.Add(Key, Target);
	return true;
}

void UHodgeGameplayAbility_Melee::ResetHitGeometryHistory()
{
	if (!IsActive() || IsExecutionEnding() || !CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) { return; }
	if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(GetAvatarActorFromActorInfo()))
	{
		for (const auto& Pair : WindowStates) { Combat->ResetDetectionHistory(Pair.Value.SessionHandle, GetExecutionId()); }
	}
}

void UHodgeGameplayAbility_Melee::OnExecutionReady()
{
	Super::OnExecutionReady();
	WindowStates.Reset();
	HitGroups.Reset();
	HitAnchors.Reset();
	HitTargets.Reset();
	ConsumedPoints.Reset();
	const UHodgeAbilityDefinition* Definition = GetDefinition();
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority() || !Definition || (Definition->HitWindows.IsEmpty() && Definition->HitPoints.IsEmpty())) { return; }
	if (!PrepareHitExecutionContext()) { FinishExecution(true, true); return; }
	if (!IsActive() || IsExecutionEnding()) { return; }
	AActor* Avatar = GetAvatarActorFromActorInfo();
	auto* Combat = Avatar ? Avatar->FindComponentByClass<UHodgeCombatComponentBase>() : nullptr;
	if (!Combat)
	{
		UE_LOG(LogTemp, Error, TEXT("Melee ability [%s] requires an Experience AddComponents action providing CombatComponent on its Avatar."), *GetPathName());
		FinishExecution(true, true);
		return;
	}
	DetectionTask = UHodgeAbilityTask_WaitHitResults::WaitHitResults(this, Combat, GetExecutionTimeline(), GetExecutionId());
	DetectionTask->OnHitResults.AddUObject(this, &ThisClass::OnHitResults);
	DetectionTask->ReadyForActivation();
}

void UHodgeGameplayAbility_Melee::OnExecutionEnding(const FGuid& EndingExecutionId)
{
	// 基类已失效执行身份，Task 清理不会再消费旧批次。
	if (DetectionTask)
	{
		DetectionTask->OnHitResults.RemoveAll(this);
		DetectionTask->EndTask();
		DetectionTask = nullptr;
	}
	WindowStates.Reset();
	HitGroups.Reset();
	HitAnchors.Reset();
	HitTargets.Reset();
	ConsumedPoints.Reset();
	Super::OnExecutionEnding(EndingExecutionId);
}

void UHodgeGameplayAbility_Melee::OnExecutionWindowEntered(int32 EventIndex, FGameplayTag WindowTag)
{
	if (!DetectionTask || !DetectionTask->IsRunningForExecution(GetExecutionId()) || WindowStates.Contains(EventIndex)) { return; }
	const UHodgeAbilityDefinition* Definition = GetDefinition();
	const FHodgeHitWindowBinding* Binding = Definition ? Definition->HitWindows.FindByPredicate(
		[WindowTag](const FHodgeHitWindowBinding& Entry) { return Entry.WindowTag == WindowTag; }) : nullptr;
	if (!Binding) { return; }
	if (OpenHit(EventIndex, *Binding)) { DetectionTask->SampleWindow(EventIndex); }
}

bool UHodgeGameplayAbility_Melee::OpenHit(int32 EventIndex, const FHodgeHitWindowBinding& Binding)
{
	if (!DetectionTask || !DetectionTask->IsRunningForExecution(GetExecutionId()) || WindowStates.Contains(EventIndex)) { return false; }
	FHodgeHitDetectionRequest Request;
	Request.SourceTag = Binding.SourceTag;
	Request.Profile = Binding.Profile;
	Request.Volume = Binding.Volume;
	Request.TargetPolicy = Binding.TargetPolicy;
	Request.MaxTargetDistance = Binding.MaxTargetDistance;
	if (const FTransform* Anchor = HitAnchors.Find(Binding.Volume.AnchorKey)) { Request.RuntimeAnchor = *Anchor; Request.bHasRuntimeAnchor = true; }
	if (const auto* Target = HitTargets.Find(Binding.TargetKey)) { Request.RuntimeTarget = *Target; }
	if ((Binding.TargetPolicy != EHodgeHitTargetPolicy::AnyInVolume || Binding.Volume.AnchorKind == EHodgeHitAnchorKind::ExecutionTarget) &&
		!FHodgeDamageRules::CanDamage(GetAvatarActorFromActorInfo(), Request.RuntimeTarget.Get(), Binding.bAllowFriendlyFire))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit occurrence [%d] requires a valid server target key [%s]."), EventIndex, *Binding.TargetKey.ToString());
		return false;
	}
	const uint64 Handle = DetectionTask->CreateWindow(EventIndex, Request);
	if (Handle == 0) { return false; }
	FHodgeMeleeWindowState State;
	State.Binding = Binding;
	State.SessionHandle = Handle;
	if (Binding.HitGroup.IsNone()) { State.History = MakeShared<FHodgeMeleeHitHistory>(); }
	else
	{
		FHodgeHitGroupKey Key;
		Key.Group = Binding.HitGroup;
		Key.bByTime = Binding.HitGroupScope == EHodgeHitGroupScope::TriggerTime;
		if (Key.bByTime)
		{
			const auto* Timeline = GetDefinition()->ExecutionConfig.TimelineTaskConfig.Timeline.Get();
			const float Start = Timeline->Events[EventIndex].StartTime == 0.f ? 0.f : Timeline->Events[EventIndex].StartTime;
			FMemory::Memcpy(&Key.TimeKey, &Start, sizeof(Start));
		}
		TSharedPtr<FHodgeMeleeHitHistory>& History = HitGroups.FindOrAdd(Key);
		if (!History) { History = MakeShared<FHodgeMeleeHitHistory>(); }
		State.History = History;
	}
	WindowStates.Add(EventIndex, MoveTemp(State));
	return true;
}

void UHodgeGameplayAbility_Melee::OnExecutionPoint(int32 EventIndex, FGameplayTag PointTag)
{
	if (EventIndex < 0 || ConsumedPoints.Contains(EventIndex) || !DetectionTask || !DetectionTask->IsRunningForExecution(GetExecutionId())) { return; }
	const auto* Definition = GetDefinition();
	const auto* Point = Definition ? Definition->HitPoints.FindByPredicate([PointTag](const auto& Entry) { return Entry.PointEventTag == PointTag; }) : nullptr;
	if (!Point) { return; }
	ConsumedPoints.Add(EventIndex);
	FHodgeHitWindowBinding Binding;
	static_cast<FHodgeHitEffectConfig&>(Binding) = *Point;
	if (!OpenHit(EventIndex, Binding)) { return; }
	const FGuid Execution = GetExecutionId();
	DetectionTask->SampleWindow(EventIndex);
	if (Execution == GetExecutionId() && DetectionTask) { DetectionTask->CloseWindow(EventIndex, false); WindowStates.Remove(EventIndex); }
}

void UHodgeGameplayAbility_Melee::OnExecutionWindowExited(int32 EventIndex, bool bSampleFinal)
{
	const FGuid ClosingExecution = GetExecutionId();
	if (DetectionTask) { DetectionTask->CloseWindow(EventIndex, bSampleFinal); }
	if (GetExecutionId() == ClosingExecution) { WindowStates.Remove(EventIndex); }
}

bool UHodgeGameplayAbility_Melee::IsMeleeBatchCurrent(const FHodgeHitDetectionBatch& Batch) const
{
	const FHodgeMeleeWindowState* State = WindowStates.Find(Batch.EventIndex);
	return IsActive() && !IsExecutionEnding() && CurrentActorInfo && CurrentActorInfo->IsNetAuthority() &&
		DetectionTask && DetectionTask->IsBatchCurrent(Batch) && State && State->SessionHandle == Batch.SessionHandle;
}

void UHodgeGameplayAbility_Melee::OnHitResults(const FHodgeHitDetectionBatch& Batch)
{
	if (!IsMeleeBatchCurrent(Batch)) { return; }
	FHodgeMeleeWindowState& State = WindowStates.FindChecked(Batch.EventIndex);
	if (Batch.SampleSequence <= State.LastSampleSequence) { return; }
	State.LastSampleSequence = Batch.SampleSequence;
	// 权限和身份检查先于蓝图事件，覆盖事件不能收到客户端或过期批次。
	ProcessMeleeHitResults(Batch);
}

bool UHodgeGameplayAbility_Melee::CanApplyMeleeHit_Implementation(AActor* Target, const FHitResult& Hit,
	const FHodgeHitWindowBinding& Binding) const
{
	return FHodgeDamageRules::CanDamage(GetAvatarActorFromActorInfo(), Target, Binding.bAllowFriendlyFire);
}

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

FGameplayEffectSpecHandle UHodgeGameplayAbility_Melee::BuildMeleeHitSpec(const FHodgeHitDetectionBatch& Batch,
	const FHitResult& Hit, const FHodgeHitWindowBinding& Binding) const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) { return {}; }
	// 使用窗口显式配置的项目伤害 GE，缺少配置时不创建替代效果。
	if (!Binding.DamageEffect) { return {}; }
	FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Binding.DamageEffect, GetAbilityLevel(), MakeMeleeHitContext(Batch, Hit));
	if (!Spec.IsValid()) { return {}; }
	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(CurrentSpecHandle);
	// 完整 HitResult 先进入 Context，再执行 GA 的标签扩展与授予参数初始化。
	ApplyAbilityTagsToGameplayEffectSpec(*Spec.Data.Get(), AbilitySpec);
	if (AbilitySpec) { Spec.Data->SetByCallerTagMagnitudes = AbilitySpec->SetByCallerTagMagnitudes; }
	Spec.Data->SetSetByCallerMagnitude(HodgeGameplayTags::SetByCaller_DamageMultiplier, Binding.DamageMultiplier);
	if (Binding.DamageType.IsValid()) { Spec.Data->AddDynamicAssetTag(Binding.DamageType); }
	if (Binding.bAllowFriendlyFire) { Spec.Data->AddDynamicAssetTag(HodgeGameplayTags::GameplayEffect_Damage_AllowFriendlyFire); }
	return Spec;
}

void UHodgeGameplayAbility_Melee::ProcessMeleeHitResults_Implementation(const FHodgeHitDetectionBatch& Batch)
{
	if (!IsMeleeBatchCurrent(Batch)) { return; }
	const FHodgeMeleeWindowState Snapshot = WindowStates.FindChecked(Batch.EventIndex);
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
			!Hit.GetComponent() || BatchTargets.Contains(TWeakObjectPtr<UAbilitySystemComponent>(TargetASC))) { continue; }
		if (!CanApplyMeleeHit(Target, Hit, Snapshot.Binding)) { continue; }
		if (!IsMeleeBatchCurrent(Batch)) { break; }
		if (!IsValid(TargetASC) || !IsValid(Target) || TargetASC->GetAvatarActor() != Target) { continue; }
		if (!Snapshot.History->CanHit(TargetASC, Batch.SampleTime, Snapshot.Binding.RepeatHitInterval)) { continue; }
		FGameplayEffectSpecHandle Spec = BuildMeleeHitSpec(Batch, Hit, Snapshot.Binding);
		if (!IsMeleeBatchCurrent(Batch)) { break; }
		if (!Spec.IsValid() || !IsValid(TargetASC) || !IsValid(Target) || TargetASC->GetAvatarActor() != Target) { continue; }
		// 先占用机会再进入 GAS，免疫或回调重入也不会重复处理本次接触。
		Snapshot.History->RecordHit(TargetASC, Batch.SampleTime);
		BatchTargets.Add(TWeakObjectPtr<UAbilitySystemComponent>(TargetASC));
		ApplyMeleeHitEffects(Batch, Hit, Snapshot.Binding, TargetASC, Spec);
	}
}

void UHodgeGameplayAbility_Melee::ApplyMeleeHitEffects_Implementation(const FHodgeHitDetectionBatch& Batch,
	const FHitResult& Hit, const FHodgeHitWindowBinding& Binding, UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpecHandle& Spec)
{
	if (!IsMeleeBatchCurrent(Batch) || !IsValid(TargetASC) || !Spec.IsValid()) { return; }
	// 使用目标 ASC 的 Avatar，Context 中仍保留原碰撞命中信息。
	auto* Target = new FGameplayAbilityTargetData_ActorArray();
	// ActorArray 写回 Context 的 Origin，必须使用本次采样来源而非默认零坐标。
	Target->SourceLocation.LiteralTransform = FTransform(Batch.SourceOrigin);
	Target->TargetActorArray.Add(TargetASC->GetAvatarActor());
	FGameplayAbilityTargetDataHandle TargetData(Target);
	ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, Spec, TargetData);
}
