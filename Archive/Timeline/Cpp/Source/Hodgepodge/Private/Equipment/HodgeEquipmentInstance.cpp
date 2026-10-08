// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/HodgeEquipmentInstance.h"
#include "Data/HodgeEquipmentStatProfile.h"

// 提供 USkeletalMeshComponent，用于 Character 装备 Actor 的骨骼 Socket 挂接。
#include "Components/SkeletalMeshComponent.h"

// 提供 ACharacter，用于优先将装备 Actor 挂接到角色 Mesh。
#include "GameFramework/Character.h"

// 提供 FHodgeEquipmentActorToSpawn 装备 Actor 生成配置。
#include "Equipment/HodgeEquipmentDefinition.h"

// 提供 DOREPLIFETIME 等属性复制相关功能。
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS

// 提供 Iris UObject 属性复制 Fragment 的自动创建与注册功能。
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"

#endif // UE_WITH_IRIS

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeEquipmentInstance)

class FLifetimeProperty;
class UClass;
class USceneComponent;

// 构造装备运行时实例。
UHodgeEquipmentInstance::UHodgeEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// 通过所属 Pawn 获取当前装备实例所在的 World。
UWorld* UHodgeEquipmentInstance::GetWorld() const
{
	// EquipmentInstance 的 Outer 被设计为所属 Pawn。
	if (APawn* OwningPawn = GetPawn())
	{
		// 装备实例与所属 Pawn 使用同一个 World。
		return OwningPawn->GetWorld();
	}
	else
	{
		// 当前没有有效所属 Pawn 时无法取得 World。
		return nullptr;
	}
}

// 注册需要通过传统 UE Replication 系统复制的属性。
void UHodgeEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 保留父类需要注册的复制属性。
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, EquipmentId);
	DOREPLIFETIME(ThisClass, EquipmentLevel);
	DOREPLIFETIME(ThisClass, StatProfile);

	// 将装备来源对象 Instigator 注册为复制属性。
	DOREPLIFETIME(ThisClass, Instigator);

	// 将当前装备实例生成的 Actor 列表注册为复制属性。
	DOREPLIFETIME(ThisClass, SpawnedActors);
}

#if UE_WITH_IRIS

// Iris 启用时为当前装备 UObject 注册网络复制 Fragment。
void UHodgeEquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context,
                                                           UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object

	// 根据当前 UObject 的复制属性自动创建并注册 PropertyReplicationFragment。
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}

#endif // UE_WITH_IRIS

// 获取当前装备实例所属的 Pawn，约定 EquipmentInstance 的 Outer 就是 Pawn。
APawn* UHodgeEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

// 获取指定类型的所属 Pawn，仅当实际 Pawn 类型满足 PawnType 时返回。
APawn* UHodgeEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	// 默认没有找到符合类型要求的 Pawn。
	APawn* Result = nullptr;

	// 获取传入的实际 Pawn Class。
	if (UClass* ActualPawnType = PawnType)
	{
		// 检查当前 Outer 是否属于指定 Pawn 类型或其派生类型。
		if (GetOuter()->IsA(ActualPawnType))
		{
			// 类型满足要求时将 Outer 转换为 Pawn 返回。
			Result = Cast<APawn>(GetOuter());
		}
	}

	// 返回满足类型要求的 Pawn，不满足时返回 nullptr。
	return Result;
}

// 根据 Definition 提供的生成配置创建并挂接装备 Actor。
void UHodgeEquipmentInstance::SpawnEquipmentActors(const TArray<FHodgeEquipmentActorToSpawn>& ActorsToSpawn)
{
	// 只有存在有效所属 Pawn 时才能生成并挂接装备 Actor。
	if (APawn* OwningPawn = GetPawn())
	{
		// 普通 Pawn 默认将装备 Actor 挂接到 RootComponent。
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();

		// Character 优先将装备 Actor 挂接到 SkeletalMeshComponent，以支持骨骼 Socket。
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}

		// 遍历 Definition 中配置的所有装备 Actor 生成信息。
		for (const FHodgeEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			// 延迟生成装备 Actor，并将所属 Pawn 设置为新 Actor 的 Owner。
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity,
			                                                          OwningPawn);

			// 完成延迟生成流程，初始 Transform 使用单位变换。
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);

			// 设置装备 Actor 相对于挂接目标的配置偏移。
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);

			// 将装备 Actor 挂接到目标组件指定 Socket，并保留当前相对 Transform。
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform,
			                            SpawnInfo.AttachSocket);

			// 保存实际生成的 Actor，供查询、网络复制以及卸装销毁使用。
			SpawnedActors.Add(NewActor);
		}
		OnSpawnedActorsChanged();
	}
}

// 销毁当前装备实例生成的所有装备 Actor。
void UHodgeEquipmentInstance::DestroyEquipmentActors()
{
	// 遍历当前装备实例记录的所有已生成 Actor。
	for (AActor* Actor : SpawnedActors)
	{
		// 只销毁仍然有效的 Actor。
		if (Actor)
		{
			Actor->Destroy();
		}
	}
	SpawnedActors.Reset();
	OnSpawnedActorsChanged();
}

void UHodgeEquipmentInstance::OnRep_SpawnedActors()
{
	OnSpawnedActorsChanged();
}

// 装备正式生效时触发装备生命周期事件。
void UHodgeEquipmentInstance::OnEquipped()
{
	// 将装备事件转发给蓝图实现。
	K2_OnEquipped();
}

// 装备被卸下时触发卸装生命周期事件。
void UHodgeEquipmentInstance::OnUnequipped()
{
	// 将卸装事件转发给蓝图实现。
	K2_OnUnequipped();
}

// Instigator 在客户端通过网络复制发生变化时触发，目前没有额外处理逻辑。
void UHodgeEquipmentInstance::OnRep_Instigator()
{
}

void UHodgeEquipmentInstance::SetStatIdentity(FGuid Id, int32 Level, const UHodgeEquipmentStatProfile* Profile)
{
    if (!GetPawn() || !GetPawn()->HasAuthority()) { return; }
    EquipmentId = Id;
    EquipmentLevel = Level;
    StatProfile = Profile;
    GetPawn()->ForceNetUpdate();
}
