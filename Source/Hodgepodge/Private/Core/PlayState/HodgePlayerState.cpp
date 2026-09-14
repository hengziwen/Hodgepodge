// 111屎山代码来袭


// AHodgePlayerState 类定义。
#include "Core/PlayState/HodgePlayerState.h"

// 项目自定义 AbilitySystemComponent。
#include "AbilitySystem/HodgeAbilitySystemComponent.h"

// 项目生命属性集。
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"

// Experience 管理组件，用于监听当前 Experience 加载完成事件。
#include "Component/HodgeExperienceManagerComponent.h"

// GameFrameworkComponentManager，用于向 GameFeature / ModularGameplay 系统发送扩展事件。
#include "Components/GameFrameworkComponentManager.h"

// 项目自定义 GameMode。
#include "Core/GameMode/HodgeGameModeBase.h"

// 项目自定义 PlayerController。
#include "Core/PlayerController/HodgePlayerController.h"

// 网络属性复制相关宏与类型。
#include "Component/HodgePawnExtensionComponent.h"
#include "Data/HodgeAbilitySet.h"
#include "Data/HodgePawnData.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgePlayerState)

// PawnData 和 AbilitySet 完成配置后发送的组件扩展事件名称。
const FName AHodgePlayerState::NAME_HodgeAbilityReady("HodgeAbilitiesReady");

// PlayerState 构造函数。
AHodgePlayerState::AHodgePlayerState(const FObjectInitializer& ObjectInitializer)
{
	// 创建挂载在 PlayerState 上的项目自定义 ASC。
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UHodgeAbilitySystemComponent>(
		this, TEXT("AbilitySystemComponent"));

	// ASC 需要参与网络复制。
	AbilitySystemComponent->SetIsReplicated(true);

	// 使用 Mixed 复制模式：GameplayEffect 主要复制给拥有者，GameplayTag / GameplayCue 等仍可向其他客户端同步。
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	// 创建 HealthSet，并保留强引用，确保 ASC 初始化发现 AttributeSet 之前不会被 GC 回收。
	HealthSet = CreateDefaultSubobject<UHodgeHealthSet>(TEXT("HealthSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	// PlayerState 承载 ASC 和视角等高频同步数据，因此提高网络更新频率。
	SetNetUpdateFrequency(100.0f);

	// 初始状态下不属于任何队伍。
	MyTeamID = FGenericTeamId::NoTeam;

	// 初始状态下不属于任何小队。
	MySquadID = INDEX_NONE;
}

// 获取拥有当前 PlayerState 的 HodgePlayerController。
AHodgePlayerController* AHodgePlayerState::GetHodgePlayerController() const
{
	// PlayerState 的 Owner 通常就是对应 PlayerController。
	return Cast<AHodgePlayerController>(GetOwner());
}

// IAbilitySystemInterface 接口实现，返回当前 PlayerState 持有的 ASC。
UAbilitySystemComponent* AHodgePlayerState::GetAbilitySystemComponent() const
{
	// 转发到项目自定义 ASC 获取函数。
	return GetHodgeAbilitySystemComponent();
}

// 为当前 PlayerState 设置 PawnData。
void AHodgePlayerState::SetPawnData(const UHodgePawnData* InPawnData)
{
	// PawnData 必须有效。
	check(InPawnData);

	// PawnData 只允许服务器权威端设置。
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	// 当前已经拥有 PawnData 时不允许重复设置。
	if (PawnData)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."),
		       *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	// Push Model 下手动标记 PawnData 属性已经发生变化。
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);

	// 保存服务器确定下来的 PawnData。
	PawnData = InPawnData;

	// 遍历 PawnData 中配置的所有 AbilitySet。
	for (const UHodgeAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		// 跳过空 AbilitySet。
		if (AbilitySet)
		{
			// 将 AbilitySet 中配置的 Ability / GameplayEffect / AttributeSet 授予当前 ASC。
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	// 通知 GameFrameworkComponentManager：当前 PlayerState 的 Ability 已经准备完成。
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_HodgeAbilityReady);

	// 强制尽快进行一次网络更新，让 PawnData 等变化及时同步。
	ForceNetUpdate();
}

// Actor 组件正式初始化之前调用。
void AHodgePlayerState::PreInitializeComponents()
{
	// 执行父类初始化逻辑。
	Super::PreInitializeComponents();

	// ASC 必须已经由构造函数创建。
	check(AbilitySystemComponent);

	// 第一次初始化 ASC 的 ActorInfo。
	// OwnerActor 为 PlayerState，AvatarActor 当前使用 GetPawn()，后续 PawnExtension 可以重新绑定真正 Avatar。
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	// 获取当前 World。
	UWorld* World = GetWorld();

	// 只有真正游戏 World 且当前不是纯客户端时，才监听 Experience 加载。
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		// 获取当前 GameState。
		AGameStateBase* GameState = GetWorld()->GetGameState();

		// 游戏流程中这里应该已经存在 GameState。
		check(GameState);

		// 从 GameState 获取 ExperienceManagerComponent。
		UHodgeExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<
			UHodgeExperienceManagerComponent>();

		// ExperienceManagerComponent 是 Experience 系统的必要组件。
		check(ExperienceComponent);

		// 如果 Experience 已经加载就立即回调，否则注册回调等待加载完成。
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(
			FOnHodgeExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

// Actor 组件初始化完成后调用。
void AHodgePlayerState::PostInitializeComponents()
{
	// 当前仅执行父类默认逻辑。
	Super::PostInitializeComponents();
}

// 重置 PlayerState。
void AHodgePlayerState::Reset()
{
	// 当前仅执行父类重置逻辑。
	Super::Reset();
}

// PlayerState 在客户端完成初始化时调用。
void AHodgePlayerState::ClientInitialize(AController* C)
{
	// 执行父类客户端初始化逻辑。
	Super::ClientInitialize(C);

	// 客户端初始化 PlayerState 后主动推动 PawnExtension 的 InitState。
	if (UHodgePawnExtensionComponent* PawnExtComp = UHodgePawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

// 在 PlayerState 被替换或复制到新 PlayerState 时复制需要保留的数据。
void AHodgePlayerState::CopyProperties(APlayerState* PlayerState)
{
	// 先复制父类维护的数据。
	Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
	// TODO：后续在这里复制断线重连 / PlayerState 切换时需要保留的统计数据。
}

// 注册 PlayerState 网络复制属性。
void AHodgePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 先注册父类复制属性。
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 创建一组公共复制参数。
	FDoRepLifetimeParams SharedParams;

	// 启用 Push Model，需要在属性修改时显式 MARK_PROPERTY_DIRTY。
	SharedParams.bIsPushBased = true;

	// PawnData 使用 Push Model 向客户端复制。
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);

	// 玩家连接状态使用 Push Model 复制。
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams)

	// 队伍 ID 使用 Push Model 复制。
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamID, SharedParams);

	// 小队 ID 使用 Push Model 复制。
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MySquadID, SharedParams);

	// 视角旋转不需要发回给拥有者，因为拥有者本地本来就知道自己的视角。
	SharedParams.Condition = ELifetimeCondition::COND_SkipOwner;

	// 将 ReplicatedViewRotation 复制给除拥有者以外的客户端，用于观战 / Replay。
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedViewRotation, SharedParams);

	// StatTags 使用自身复制逻辑进行同步。
	DOREPLIFETIME(ThisClass, StatTags);
}

// PlayerState 被标记为 Deactivated 时调用，常见于玩家断线。
void AHodgePlayerState::OnDeactivated()
{
	// 控制是否立即销毁这个已经失活的 PlayerState。
	bool bDestroyDeactivatedPlayerState = false;

	// 根据当前连接状态决定失活 PlayerState 的处理方式。
	switch (GetPlayerConnectionType())
	{
	// 正常玩家或已经进入 Inactive 状态的玩家。
	case EHodgePlayerConnectionType::Player:
	case EHodgePlayerConnectionType::InactivePlayer:
		//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
		// TODO：后续可以由 Experience 决定断线 PlayerState 是立即销毁还是保留一段时间。
		// (e.g., for long running servers where they might build up if lots of players cycle through)
		// 例如长期运行服务器中，如果保留太多断线玩家可能造成 PlayerState 累积。
		bDestroyDeactivatedPlayerState = true;
		break;

	// 其他连接类型当前同样直接销毁。
	default:
		bDestroyDeactivatedPlayerState = true;
		break;
	}

	// 将玩家连接状态更新为 InactivePlayer。
	SetPlayerConnectionType(EHodgePlayerConnectionType::InactivePlayer);

	// 当前策略要求销毁失活 PlayerState。
	if (bDestroyDeactivatedPlayerState)
	{
		// 销毁当前 PlayerState。
		Destroy();
	}
}

// PlayerState 从失活状态重新激活时调用。
void AHodgePlayerState::OnReactivated()
{
	// 执行父类重新激活逻辑。
	Super::OnReactivated();

	// 如果当前仍被标记为 InactivePlayer。
	if (GetPlayerConnectionType() == EHodgePlayerConnectionType::InactivePlayer)
	{
		// 恢复为正常 Player 状态。
		SetPlayerConnectionType(EHodgePlayerConnectionType::Player);
	}
}

// 修改当前玩家连接状态。
void AHodgePlayerState::SetPlayerConnectionType(EHodgePlayerConnectionType NewType)
{
	// Push Model 下标记连接状态已变化。
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);

	// 保存新的连接状态。
	MyPlayerConnectionType = NewType;
}

// 设置玩家所属小队 ID。
void AHodgePlayerState::SetSquadID(int32 NewSquadID)
{
	// SquadID 只允许服务器权威端修改。
	if (HasAuthority())
	{
		// Push Model 下标记属性发生变化。
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MySquadID, this);

		// 设置新的小队 ID。
		MySquadID = NewSquadID;
	}
}

// 为某个统计 GameplayTag 增加指定层数。
void AHodgePlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	// 将层数写入 StatTags 容器。
	StatTags.AddStack(Tag, StackCount);
}

// 为某个统计 GameplayTag 移除指定层数。
void AHodgePlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	// 从 StatTags 容器中减少对应层数。
	StatTags.RemoveStack(Tag, StackCount);
}

// 获取指定统计 GameplayTag 当前拥有的层数。
int32 AHodgePlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
	// 从 StatTags 中查询对应 StackCount。
	return StatTags.GetStackCount(Tag);
}

// 判断当前 PlayerState 是否拥有指定统计 GameplayTag。
bool AHodgePlayerState::HasStatTag(FGameplayTag Tag) const
{
	// 只要容器中存在该 Tag 即返回 true。
	return StatTags.ContainsTag(Tag);
}

// 获取复制后的玩家视角旋转。
FRotator AHodgePlayerState::GetReplicatedViewRotation() const
{
	// 返回 PlayerState 中缓存的 ReplicatedViewRotation。
	return ReplicatedViewRotation;
}

// 更新用于网络同步的玩家视角旋转。
void AHodgePlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)
{
	// 只有旋转实际变化时才触发 Push Model 更新。
	if (NewRotation != ReplicatedViewRotation)
	{
		// 标记 ReplicatedViewRotation 属性发生变化。
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedViewRotation, this);

		// 保存新的视角旋转。
		ReplicatedViewRotation = NewRotation;
	}
}

// Experience 完成加载后调用。
void AHodgePlayerState::OnExperienceLoaded(const UHodgeExperienceDefinition* CurrentExperience)
{
	// 只有服务器存在 AuthGameMode。
	if (AHodgeGameModeBase* HodgeGameMode = GetWorld()->GetAuthGameMode<AHodgeGameModeBase>())
	{
		// 根据当前拥有者 Controller，从 GameMode 获取应该使用的 PawnData。
		if (const UHodgePawnData* NewPawnData = HodgeGameMode->GetPawnDataForController(GetOwningController()))
		{
			// 将 PawnData 写入 PlayerState，并进一步授予 AbilitySet。
			SetPawnData(NewPawnData);
		}
		else
		{
			// 找不到 PawnData 表示玩家初始化链路无法继续，输出错误日志。
			UE_LOG(LogTemp, Error,
			       TEXT(
				       "AHodgePlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"
			       ), *GetNameSafe(this));
		}
	}
}

// PawnData 从服务器复制到客户端后触发。
void AHodgePlayerState::OnRep_PawnData()
{
	// 当前暂无额外处理逻辑。
}

// TeamID 从服务器复制到客户端后触发。
void AHodgePlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	// 原设计：当队伍发生变化时广播 TeamChanged 事件。
	//ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

// SquadID 从服务器复制到客户端后触发。
void AHodgePlayerState::OnRep_MySquadID()
{
	//@TODO: Let the squad subsystem know (once that exists)
	// TODO：后续 SquadSubsystem 完成后，在这里通知小队系统当前玩家的小队发生变化。
}
