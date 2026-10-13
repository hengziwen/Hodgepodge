/** @file HodgeBTTask_ExecuteAbility.cpp
 * @brief 实现行为树战斗请求任务相关的类型、接口与现有处理流程。
 */
#include "AI/HodgeBTTask_ExecuteAbility.h"
#include "AI/HodgeEnemyAIController.h"
#include "Component/HodgeEnemyInitComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Data/HodgeAbilityDefinition.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "GameFramework/Pawn.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeBTTask_ExecuteAbility)

/** 构造行为树战斗请求任务并初始化其默认状态。 */
UHodgeBTTask_ExecuteAbility::UHodgeBTTask_ExecuteAbility()
{
	NodeName = TEXT("Request Hodge Combat Action");
	bCreateNodeInstance = true; bNotifyTick = true; bNotifyTaskFinished = true;
	TargetActor.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetActor), AActor::StaticClass());
	TargetActor.SelectedKeyName = TEXT("TargetActor");
}

/** 从行为树资产解析节点依赖和黑板键配置。 */
void UHodgeBTTask_ExecuteAbility::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	bTargetKeyValid = false;
	if (const auto* Blackboard = GetBlackboardAsset())
	{
		TargetActor.ResolveSelectedKey(*Blackboard);
		const auto* Entry = Blackboard->GetKey(TargetActor.GetSelectedKeyID());
		const auto* ObjectKey = Entry ? Cast<UBlackboardKeyType_Object>(Entry->KeyType) : nullptr;
		bTargetKeyValid = ObjectKey && ObjectKey->BaseClass && ObjectKey->BaseClass->IsChildOf(AActor::StaticClass()) &&
			TargetActor.SelectedKeyName != FBlackboard::KeySelf;
	}
	if (!bTargetKeyValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy attack task [%s] in [%s] requires an Actor target key; [%s] is invalid (SelfActor cannot be attacked)."),
			*GetName(), *Asset.GetPathName(), *TargetActor.SelectedKeyName.ToString());
	}
}

/** 检查绑定对象与运行身份是否仍属于当前生命周期。 */
bool UHodgeBTTask_ExecuteAbility::IsCurrent() const
{
	return Controller.IsValid() && Pawn.IsValid() && Controller->IsPossessCurrent(Pawn.Get(), PossessGeneration) &&
		ASC.IsValid() && ASC->GetAvatarActor() == Pawn.Get() && Combat.IsValid() && Combat->GetOwner() == Pawn.Get();
}

/** 准备当前行为树任务所需的角色、目标和执行上下文。 */
EBTNodeResult::Type UHodgeBTTask_ExecuteAbility::ExecuteTask(UBehaviorTreeComponent& Owner, uint8*)
{
	Cleanup(false); ++AttemptId;
	if (!bTargetKeyValid) { return EBTNodeResult::Failed; }
	Controller = Cast<AHodgeEnemyAIController>(Owner.GetAIOwner()); Pawn = Controller.IsValid() ? Controller->GetPawn() : nullptr;
	Tree = &Owner; PossessGeneration = Controller.IsValid() ? Controller->GetPossessGeneration() : 0;
	auto* Init = Pawn.IsValid() ? Pawn->FindComponentByClass<UHodgeEnemyInitComponent>() : nullptr;
	// 本次操作的目标。
	auto* Target = Owner.GetBlackboardComponent() ? Cast<AActor>(Owner.GetBlackboardComponent()->GetValueAsObject(TargetActor.SelectedKeyName)) : nullptr;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Init || !Init->CanExecuteAIAction() || !Controller->IsValidEnemyTarget(Target) || !FMath::IsFinite(Timeout) || Timeout <= 0.f)
	{ return EBTNodeResult::Failed; }
	ASC = Cast<UHodgeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn.Get()));
	Combat = Pawn->FindComponentByClass<UHodgeCombatComponentBase>();
	if (!IsCurrent()) { return EBTNodeResult::Failed; }
	if (RequestType == EHodgeCombatActionRequestType::StandaloneAbility)
	{
		const auto* Definition = ASC->FindAbilityDefinition(ASC->FindDefinitionAbility(AbilityTag));
		if (!Definition || Definition->ExecutionRoute != EHodgeAbilityExecutionRoute::Standalone) { return EBTNodeResult::Failed; }
	}
	else if (!IntentTag.IsValid() || !Combat->GetComboDefinition()) { return EBTNodeResult::Failed; }
	Controller->StopMovement();
	// 朝向组件或旋转数据。
	auto* Rotation = Pawn->FindComponentByClass<UHodgeCharacterRotationComponent>();
	if (!Rotation) { return EBTNodeResult::Failed; }
	Facing = Rotation->RequestActionFacing(Target->GetActorLocation() - Pawn->GetActorLocation(), Pawn.Get(), FGuid::NewGuid(), false);
	if (!Facing.IsValid()) { return EBTNodeResult::Failed; }
	Elapsed = 0.f; bActivated = false; bEnded = false;
	return EBTNodeResult::InProgress;
}

/** 更新行为树潜在任务，并推进准备或等待阶段。 */
void UHodgeBTTask_ExecuteAbility::TickTask(UBehaviorTreeComponent& Owner, uint8*, float DeltaSeconds)
{
	Elapsed += DeltaSeconds;
	auto* Init = Pawn.IsValid() ? Pawn->FindComponentByClass<UHodgeEnemyInitComponent>() : nullptr;
	if (!IsCurrent() || !Init || !Init->CanExecuteAIAction() || Elapsed > Timeout)
	{ Cleanup(true); FinishLatentTask(Owner, EBTNodeResult::Failed); return; }
	if (bActivated) { return; }
	// 朝向组件或旋转数据。
	auto* Rotation = Pawn->FindComponentByClass<UHodgeCharacterRotationComponent>();
	if (!Rotation || !Rotation->IsActionFacingApplied(Facing)) { return; }
	const uint32 Attempt = AttemptId;
	RequestChangedHandle = Combat->OnActionRequestChanged.AddWeakLambda(this,
		[this, Attempt](const FHodgeCombatActionRequestState& State) { HandleRequestChanged(Attempt, State); });
	bSubmitting = true;
	const auto State = RequestType == EHodgeCombatActionRequestType::StandaloneAbility
		? Combat->RequestStandaloneAbility(AbilityTag) : Combat->SubmitCombatIntent(IntentTag);
	bSubmitting = false;
	RequestId = State.RequestId; bActivated = true;
	// 请求可同步拒绝或完成，返回快照补偿提交期间尚未绑定的请求身份。
	HandleRequestChanged(Attempt, State);
}

/** 只消费匹配本次尝试与请求身份的终结状态，结束对应行为树任务。 */
void UHodgeBTTask_ExecuteAbility::HandleRequestChanged(uint32 Attempt, const FHodgeCombatActionRequestState& State)
{
	if (bSubmitting || Attempt != AttemptId || !IsCurrent() || State.RequestId != RequestId || !State.IsTerminal()) { return; }
	bEnded = true;
	// 当前操作关联的所属对象。
	auto* Owner = Tree.Get();
	const auto Result = State.Status == EHodgeCombatActionRequestStatus::Completed ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	Cleanup(false);
	if (Owner) { FinishLatentTask(*Owner, Result); }
}

/** 按当前对象的来源归属解除监听并清理资源。 */
void UHodgeBTTask_ExecuteAbility::Cleanup(bool bCancel)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Combat.IsValid()) { Combat->OnActionRequestChanged.Remove(RequestChangedHandle); }
	RequestChangedHandle.Reset();
	const FGuid Previous = RequestId; RequestId.Invalidate();
	if (bCancel && Combat.IsValid()) { Combat->CancelCombatActionRequest(Previous); }
	if (Pawn.IsValid())
	{ if (auto* Rotation = Pawn->FindComponentByClass<UHodgeCharacterRotationComponent>()) { Rotation->ReleaseFacingRequest(Facing); } }
	Facing = {}; bActivated = false;
}

/** 取消当前任务拥有的操作并解除监听。 */
EBTNodeResult::Type UHodgeBTTask_ExecuteAbility::AbortTask(UBehaviorTreeComponent&, uint8*)
{ ++AttemptId; Cleanup(true); return EBTNodeResult::Aborted; }

/** 在行为树任务结束时完成本节点的收尾。 */
void UHodgeBTTask_ExecuteAbility::OnTaskFinished(UBehaviorTreeComponent& Owner, uint8* Memory, EBTNodeResult::Type Result)
{ Cleanup(false); Super::OnTaskFinished(Owner, Memory, Result); }
