// Copyright Epic Games, Inc. All Rights Reserved.
#include "Inventory/HodgeInventoryItemInstance.h"
#include "Inventory/HodgeInventoryItemDefinition.h"
#include "Inventory/HodgeInventoryManagerComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeInventoryItemInstance)

void UHodgeInventoryItemInstance::Initialize(TSubclassOf<UHodgeInventoryItemDefinition> Definition, FGuid Id, int32 Level)
{
	ItemDefinition = Definition; ItemId = Id; ItemLevel = Level;
	TGuardValue<bool> Guard(bInitializing, true);
	for (const UHodgeInventoryItemFragment* Fragment : GetDefault<UHodgeInventoryItemDefinition>(Definition)->Fragments)
	{ if (Fragment) { Fragment->OnInstanceCreated(this); } }
}

void UHodgeInventoryItemInstance::InitializeStatTag(FGameplayTag Tag, int32 Count)
{
	if (bInitializing && Tag.IsValid() && Count > 0) { StatTags.AddStack(Tag, Count); }
}

void UHodgeInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, ItemId, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, ItemDefinition, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, ItemLevel, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, StatTags, COND_OwnerOnly);
}

void UHodgeInventoryItemInstance::PostNetReceive()
{
	Super::PostNetReceive();
	// 条目和子对象字段可以分批到达，实例就绪后补发可读快照。
	if (auto* Owner = GetTypedOuter<AActor>())
	{ if (auto* Manager = Owner->FindComponentByClass<UHodgeInventoryManagerComponent>()) { Manager->RefreshSnapshot(); } }
}

#if UE_WITH_IRIS
void UHodgeInventoryItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags Flags)
{
	UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, Flags);
}
#endif
