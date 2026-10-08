#include "AbilitySystem/Abilities/HodgeAbilityTask_WaitHitResults.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilityTask_WaitHitResults)

void FHodgeHitResultsTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType,
	ENamedThreads::Type CurrentThread, const FGraphEventRef& CompletionEvent)
{
	if (Target.IsValid() && TickType != LEVELTICK_ViewportsOnly) { Target->TickDetection(); }
}

FString FHodgeHitResultsTickFunction::DiagnosticMessage()
{
	return TEXT("HodgeAbilityTask_WaitHitResults.PostPhysics");
}

UHodgeAbilityTask_WaitHitResults* UHodgeAbilityTask_WaitHitResults::WaitHitResults(
	UHodgeGameplayAbility_Definition* OwningAbility, UHodgeCombatComponentBase* Combat,
	const FGuid& InExecutionId)
{
	auto* Task = NewAbilityTask<UHodgeAbilityTask_WaitHitResults>(OwningAbility);
	Task->ExecutionAbility = OwningAbility;
	Task->CombatComponent = Combat;
	Task->ExecutionId = InExecutionId;
	Task->Avatar = OwningAbility->GetAvatarActorFromActorInfo();
	return Task;
}

bool UHodgeAbilityTask_WaitHitResults::IsRunningForExecution(const FGuid& InExecutionId) const
{
	const UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	return !bStopped && !IsFinished() && ExecutionId.IsValid() && ExecutionId == InExecutionId &&
		ExecutionAbility.IsValid() && ExecutionAbility->IsActive() && !ExecutionAbility->IsExecutionEnding() &&
		ExecutionAbility->GetExecutionId() == ExecutionId && Avatar.IsValid() && Avatar->HasAuthority() &&
		CombatComponent.IsValid() && CombatComponent->CanExecuteAbilities() && CombatComponent->GetOwner() == Avatar.Get() && ASC &&
		ASC->IsOwnerActorAuthoritative() && ASC->GetAvatarActor() == Avatar.Get() &&
		!ASC->HasMatchingGameplayTag(HodgeGameplayTags::Status_Death);
}

void UHodgeAbilityTask_WaitHitResults::Activate()
{
	if (!IsRunningForExecution(ExecutionId)) { EndTask(); return; }
	DetectionTick.Target = this;
	DetectionTick.bCanEverTick = true;
	DetectionTick.bStartWithTickEnabled = false;
	DetectionTick.bRunOnAnyThread = false;
	DetectionTick.bTickEvenWhenPaused = false;
	DetectionTick.bAllowTickOnDedicatedServer = true;
	DetectionTick.TickGroup = TG_PostPhysics;
	DetectionTick.EndTickGroup = TG_PostPhysics;
	if (ACharacter* Character = Cast<ACharacter>(Avatar.Get()))
	{
		DetectionTick.AddPrerequisite(Character->GetMesh(), Character->GetMesh()->PrimaryComponentTick);
	}
	DetectionTick.RegisterTickFunction(Avatar->GetLevel());
	UpdateTickState();
}

uint64 UHodgeAbilityTask_WaitHitResults::CreateWindow(int32 OccurrenceId, const FHodgeHitDetectionRequest& Request)
{
	if (!IsRunningForExecution(ExecutionId) || Windows.Contains(OccurrenceId)) { return 0; }
	const uint64 Handle = CombatComponent->CreateDetectionSession(ExecutionId, OccurrenceId, Request);
	if (Handle == 0) { return 0; }
	if (USceneComponent* Source = CombatComponent->GetDetectionSourceComponent(Handle, ExecutionId))
	{
		// 武器骨骼也可能有独立更新任务，采样等待实际来源组件完成更新。
		DetectionTick.AddPrerequisite(Source, Source->PrimaryComponentTick);
	}
	FWindow Window;
	Window.Handle = Handle;
	Window.bOnce = Request.Profile && Request.Profile->SampleMode == EHodgeHitSampleMode::OnceOnEnter;
	Windows.Add(OccurrenceId, Window);
	UpdateTickState();
	return Handle;
}

bool UHodgeAbilityTask_WaitHitResults::IsBatchCurrent(const FHodgeHitDetectionBatch& Batch) const
{
	const FWindow* Window = Windows.Find(Batch.OccurrenceId);
	return !IsPaused() && IsRunningForExecution(Batch.ExecutionId) && Window && Window->Handle == Batch.SessionHandle &&
		CombatComponent->IsDetectionSessionValid(Batch.SessionHandle, Batch.ExecutionId);
}

void UHodgeAbilityTask_WaitHitResults::SampleWindow(int32 OccurrenceId)
{
	if (bSampling || IsPaused() || !IsRunningForExecution(ExecutionId)) { return; }
	FWindow* Window = Windows.Find(OccurrenceId);
	if (!Window || (Window->bOnce && Window->bSampled)) { return; }
	Window->bSampled = true;
	const uint64 Handle = Window->Handle;
	TGuardValue<bool> Guard(bSampling, true);
	FHodgeHitDetectionBatch Batch;
	if (!CombatComponent->SampleDetection(Handle, ExecutionId, Batch))
	{
		UE_LOG(LogTemp, Verbose, TEXT("[HodgeNotify] Sample failed %d"), OccurrenceId);
		CombatComponent->EndDetectionSession(Handle, ExecutionId);
		Windows.Remove(OccurrenceId);
		UpdateTickState();
		return;
	}
	UE_LOG(LogTemp, Verbose, TEXT("[HodgeNotify] Sample %d Hits=%d Current=%d Broadcast=%d"), OccurrenceId, Batch.Hits.Num(), IsBatchCurrent(Batch), ShouldBroadcastAbilityTaskDelegates());
	if (IsBatchCurrent(Batch) && ShouldBroadcastAbilityTaskDelegates()) { OnHitResults.Broadcast(Batch); }
	UpdateTickState();
}

void UHodgeAbilityTask_WaitHitResults::CloseWindow(int32 OccurrenceId, bool bSampleFinal)
{
	FWindow* Window = Windows.Find(OccurrenceId);
	if (!Window || Window->bClosing) { return; }
	if (bSampleFinal) { Window->bClosing = true; UpdateTickState(); return; }
	const uint64 Handle = Window->Handle;
	if (CombatComponent.IsValid()) { CombatComponent->EndDetectionSession(Handle, ExecutionId); }
	Windows.Remove(OccurrenceId);
	UpdateTickState();
}

void UHodgeAbilityTask_WaitHitResults::TickDetection()
{
	if (bTickingDetection || IsPaused()) { return; }
	TGuardValue<bool> Guard(bTickingDetection, true);
	const FGuid TickExecution = ExecutionId;
	if (!IsRunningForExecution(TickExecution)) { EndTask(); return; }
	if (!IsRunningForExecution(TickExecution)) { if (!IsFinished()) { EndTask(); } return; }
	TArray<int32> Indices;
	Windows.GetKeys(Indices);
	for (int32 OccurrenceId : Indices)
	{
		if (!IsRunningForExecution(TickExecution)) { break; }
		const FWindow* Window = Windows.Find(OccurrenceId);
		if (!Window) { continue; }
		const bool bClosing = Window->bClosing;
		// 首次采样发生在骨骼更新后，短区间同帧 Begin/End 也保留一次有效查询。
		if (!Window->bSampled) { CombatComponent->ResetDetectionHistory(Window->Handle, TickExecution); }
		SampleWindow(OccurrenceId);
		if (bClosing && IsRunningForExecution(TickExecution))
		{
			Window = Windows.Find(OccurrenceId);
			if (Window) { CombatComponent->EndDetectionSession(Window->Handle, TickExecution); Windows.Remove(OccurrenceId); }
		}

	}
	UpdateTickState();
}

void UHodgeAbilityTask_WaitHitResults::UpdateTickState()
{
	bool bNeedsTick = false;
	for (const auto& Pair : Windows) { bNeedsTick |= Pair.Value.bClosing || !Pair.Value.bOnce || !Pair.Value.bSampled; }
	DetectionTick.SetTickFunctionEnable(!bStopped && !IsPaused() && bNeedsTick);
}

void UHodgeAbilityTask_WaitHitResults::Pause()
{
	Super::Pause();
	UpdateTickState();
}

void UHodgeAbilityTask_WaitHitResults::Resume()
{
	Super::Resume();
	if (!IsRunningForExecution(ExecutionId)) { EndTask(); return; }
	// Task 暂停期间可能移动来源，恢复前丢弃旧轨迹，避免补扫暂停路径。
	TArray<int32> Indices;
	Windows.GetKeys(Indices);
	for (int32 OccurrenceId : Indices)
	{
		const FWindow* Window = Windows.Find(OccurrenceId);
		if (Window && !CombatComponent->ResetDetectionHistory(Window->Handle, ExecutionId)) { CloseWindow(OccurrenceId, false); }
	}
	UpdateTickState();
}

void UHodgeAbilityTask_WaitHitResults::ReleaseSessions()
{
	bStopped = true;
	DetectionTick.UnRegisterTickFunction();
	DetectionTick.Target.Reset();
	const FGuid EndingExecution = ExecutionId;
	ExecutionId.Invalidate();
	OnHitResults.Clear();
	Windows.Reset();
	if (CombatComponent.IsValid()) { CombatComponent->EndDetectionSessionsForExecution(EndingExecution); }
}

void UHodgeAbilityTask_WaitHitResults::OnDestroy(bool bInOwnerFinished)
{
	ReleaseSessions();
	Super::OnDestroy(bInOwnerFinished);
}

void UHodgeAbilityTask_WaitHitResults::BeginDestroy()
{
	ReleaseSessions();
	Super::BeginDestroy();
}
