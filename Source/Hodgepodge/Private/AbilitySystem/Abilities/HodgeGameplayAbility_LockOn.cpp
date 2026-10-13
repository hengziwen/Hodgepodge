/** @file HodgeGameplayAbility_LockOn.cpp @brief 基于既有 MovementAction 约定的 TargetData 命令链。 */
#include "AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h"
#include "Component/HodgeLockOnComponent.h"
#include "Targeting/HodgeLockOnTypes.h"
#include "Data/HodgeLockOnProfile.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayAbility_LockOn)
UHodgeGameplayAbility_LockOn::UHodgeGameplayAbility_LockOn()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationGroup = EHodgeAbilityActivationGroup::Independent;
	ActivationPolicy = EHodgeAbilityActivationPolicy::OnInputTriggered;
	bServerRespectsRemoteAbilityCancellation = false;
	FGameplayTagContainer Tags = GetAssetTags(); Tags.AddTag(HodgeLockOnTags::Ability); SetAssetTags(Tags);
}
bool UHodgeGameplayAbility_LockOn::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	const FGameplayTagContainer* Source, const FGameplayTagContainer* Target, FGameplayTagContainer* Relevant) const
{
	const auto* Lock = Info && Info->AvatarActor.IsValid() ? Info->AvatarActor->FindComponentByClass<UHodgeLockOnComponent>() : nullptr;
	return Lock && Lock->IsReady() && Super::CanActivateAbility(Handle,Info,Source,Target,Relevant);
}
void UHodgeGameplayAbility_LockOn::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event)
{
	Super::ActivateAbility(Handle,Info,ActivationInfo,Event); Sequence = 0;
	Component = Info->AvatarActor->FindComponentByClass<UHodgeLockOnComponent>();
	if (!Component.IsValid() || !Component->IsReady()) { EndAbility(Handle,Info,ActivationInfo,Info->IsNetAuthority(),true); return; }
	ComponentGeneration = Component->GetGeneration();
	CurrentActivationInfo.bCanBeEndedByOtherInstance = !Info->IsNetAuthority();
	auto* ASC = Info->AbilitySystemComponent.Get(); FPredictionKey Key = ActivationInfo.GetActivationPredictionKey();
	DataHandle = ASC->AbilityTargetDataSetDelegate(Handle,Key).AddUObject(this,&ThisClass::OnTargetData);
	GetWorld()->GetTimerManager().SetTimer(TimeoutHandle,this,&ThisClass::OnTimeout,Component->GetProfile()->CommandTimeout,false);
	if (Info->IsLocallyControlled())
	{
		FHodgeLockOnTargetData Command;
		if (!Component->TakeCommand(Command)) { EndAbility(Handle,Info,ActivationInfo,Info->IsNetAuthority(),true); return; }
		Sequence = Command.RequestSequence;
		if (!Info->IsNetAuthority())
		{
			Key.NewRejectedDelegate().BindUObject(Component.Get(),&UHodgeLockOnComponent::RejectPrediction,Sequence,Component->GetGeneration());
			FScopedPredictionWindow Prediction(ASC,true);
			ASC->ServerSetReplicatedTargetData(Handle,Key,FGameplayAbilityTargetDataHandle(new FHodgeLockOnTargetData(Command)),{},ASC->ScopedPredictionKey);
		}
		else { OnTargetData(FGameplayAbilityTargetDataHandle(new FHodgeLockOnTargetData(Command)),{}); }
	}
	else { ASC->CallReplicatedTargetDataDelegatesIfSet(Handle,Key); }
}
void UHodgeGameplayAbility_LockOn::OnTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag)
{
	if (!IsActive()) { return; }
	bool bValid = Data.Num() == 1 && Data.Get(0) && Data.Get(0)->GetScriptStruct() == FHodgeLockOnTargetData::StaticStruct();
	FHodgeLockOnTargetData Copy;
	if (bValid) { Copy = *static_cast<const FHodgeLockOnTargetData*>(Data.Get(0)); Sequence = Copy.RequestSequence; }
	GetAbilitySystemComponentFromActorInfo()->ConsumeClientReplicatedTargetData(CurrentSpecHandle,CurrentActivationInfo.GetActivationPredictionKey());
	if (bValid && Component.IsValid() && Component->GetGeneration() == ComponentGeneration && CurrentActorInfo->IsNetAuthority()) { Component->ProcessAuthorityCommand(Copy); }
	// 拒绝业务目标也结束命令，但不把服务器拒绝误认为整个 GAS 激活失败。
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,CurrentActorInfo->IsNetAuthority(),!bValid);
}
void UHodgeGameplayAbility_LockOn::OnTimeout()
{ if (IsActive()) { EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,CurrentActorInfo->IsNetAuthority(),true); } }
void UHodgeGameplayAbility_LockOn::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled)
{
	if (GetWorld()) { GetWorld()->GetTimerManager().ClearTimer(TimeoutHandle); }
	if (Info && Info->AbilitySystemComponent.IsValid())
	{
		Info->AbilitySystemComponent->AbilityTargetDataSetDelegate(Handle,ActivationInfo.GetActivationPredictionKey()).Remove(DataHandle);
		Info->AbilitySystemComponent->ConsumeClientReplicatedTargetData(Handle,ActivationInfo.GetActivationPredictionKey());
		if (auto* ASC = Cast<UHodgeAbilitySystemComponent>(Info->AbilitySystemComponent.Get())) { ASC->RetireLockOnCommand(Handle,ActivationInfo.GetActivationPredictionKey()); }
	}
	auto* Lock = Component.Get(); const int32 EndedSequence = Sequence;
	Super::EndAbility(Handle,Info,ActivationInfo,bReplicate,bCancelled);
	if (Lock && Lock->GetGeneration() == ComponentGeneration && Info && Info->AvatarActor.Get() == Lock->GetOwner()) { Lock->NotifyCommandEnded(EndedSequence,bCancelled); }
	Component.Reset();
}
