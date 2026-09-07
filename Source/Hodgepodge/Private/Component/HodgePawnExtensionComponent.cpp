// 111屎山代码来袭


#include "Component/HodgePawnExtensionComponent.h"

#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Data/HodgePawnData.h"
#include "Net/UnrealNetwork.h"


class FLifetimeProperty;
class UActorComponent;

// PawnExtension 在 InitState 系统中的 Feature 名称，用于区分 Pawn 上的不同初始化功能模块
const FName UHodgePawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UHodgePawnExtensionComponent::UHodgePawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// PawnExtension 不需要 Tick
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	// PawnExtension 组件本身需要参与网络复制
	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
}

void UHodgePawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// PawnData 由服务器复制给客户端
	DOREPLIFETIME(UHodgePawnExtensionComponent, PawnData);
}

void UHodgePawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	// PawnExtension 必须挂在 Pawn 上
	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("HodgePawnExtensionComponent on [%s] can only be added to Pawn actors."),
	                 *GetNameSafe(GetOwner()));

	// 一个 Pawn 只允许存在一个 PawnExtensionComponent
	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UHodgePawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1),
	                 TEXT("Only one HodgePawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// 提前注册到 InitState 系统，使 PawnExtension 可以参与 Pawn 的初始化状态管理
	RegisterInitStateFeature();
}

void UHodgePawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 监听 Pawn 上其他 Feature 的 InitState 变化
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Pawn 进入 Spawned 状态，然后继续尝试推进后续初始化流程
	ensure(TryToChangeInitState(HodgeGameplayTags::InitState_Spawned));

	// 根据当前条件持续推进 Pawn 的默认初始化状态链
	CheckDefaultInitialization();
}

void UHodgePawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Pawn 销毁前先解除 ASC 绑定关系
	UninitializeAbilitySystem();

	// 从 InitState 系统中注销该 Feature
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UHodgePawnExtensionComponent::SetPawnData(const UHodgePawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();

	// PawnData 只能由服务器设置
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	// PawnData 一旦设置就不允许重复覆盖
	if (PawnData)
	{
		// UE_LOG(LogTemp, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."),
		//        *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	// 保存 Pawn 对应的数据定义
	PawnData = InPawnData;

	// PawnData 发生变化后立即请求一次网络更新
	Pawn->ForceNetUpdate();

	// PawnData 准备完成后继续推进初始化状态
	CheckDefaultInitialization();
}

void UHodgePawnExtensionComponent::OnRep_PawnData()
{
	// 客户端收到 PawnData 后继续推进初始化流程
	CheckDefaultInitialization();
}

void UHodgePawnExtensionComponent::InitializeAbilitySystem(UHodgeAbilitySystemComponent* InASC,
                                                           AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	// 如果已经绑定的是同一个 ASC，则无需重复初始化
	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		return;
	}

	// 如果之前绑定过其他 ASC，则先解除旧 ASC
	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();

	// 获取 ASC 当前绑定的 AvatarActor
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogTemp, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC),
	       *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	// 如果 ASC 已经绑定了其他 Pawn，需要先让旧 Pawn 解除 Avatar 身份
	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		UE_LOG(LogTemp, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// 一个 ASC 同一时间只能对应一个 AvatarActor，这里把旧 Pawn 踢出去
		// 客户端延迟情况下可能出现新 Pawn 已经生成并被 Possess，但旧 Pawn 还没有及时销毁
		ensure(!ExistingAvatar->HasAuthority());

		// 找到旧 Pawn 上的 PawnExtension，并让旧 Pawn 主动解除 ASC
		if (UHodgePawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	// 缓存当前 Pawn 使用的 ASC
	AbilitySystemComponent = InASC;

	// 初始化 ASC 的 ActorInfo：OwnerActor 通常负责拥有 ASC，Pawn 则作为 AvatarActor
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (ensure(PawnData))
	{
		// 根据 PawnData 设置 ASC 的 TagRelationshipMapping
		//InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	// 通知所有依赖 ASC 初始化完成的系统
	OnAbilitySystemInitialized.Broadcast();
}

void UHodgePawnExtensionComponent::UninitializeAbilitySystem()
{
	// 当前没有绑定 ASC，无需处理
	if (!AbilitySystemComponent)
	{
		return;
	}

	// 只有当前 Pawn 仍然是 ASC 的 AvatarActor 时，才由自己负责清理 ASC
	// 如果 ASC 已经被其他 Pawn 接管，则说明其他 Pawn 已经完成了重新初始化
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;

		// 死亡后仍然需要保留的 Ability 不参与取消
		AbilityTypesToIgnore.AddTag(HodgeGameplayTags::Ability_Behavior_SurvivesDeath);

		// 取消当前 Pawn 上除特殊保留 Ability 外的所有 Ability
		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);

		// 清理 Ability 输入状态
		//AbilitySystemComponent->ClearAbilityInput();

		// 移除当前 AvatarActor 上的所有 GameplayCue
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			// ASC 仍然有 OwnerActor，只需要解除 AvatarActor
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// ASC 连 OwnerActor 都没有时，直接清除完整 ActorInfo
			AbilitySystemComponent->ClearActorInfo();
		}

		// 通知依赖 ASC 的系统：ASC 已经解除初始化
		OnAbilitySystemUninitialized.Broadcast();
	}

	// 清除 PawnExtension 对 ASC 的缓存引用
	AbilitySystemComponent = nullptr;
}

void UHodgePawnExtensionComponent::HandleControllerChanged()
{
	// 如果 ASC 存在并且当前 Pawn 仍然是 ASC 的 AvatarActor，则刷新 ASC 的 ActorInfo
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		// 确保 ASC 的 OwnerActor 与其自身保存的 OwnerActor 一致
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());

		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			// OwnerActor 已经失效，解除 ASC
			UninitializeAbilitySystem();
		}
		else
		{
			// Controller 发生变化后刷新 AbilityActorInfo
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}

	// Controller 变化后继续尝试推进初始化状态
	CheckDefaultInitialization();
}

void UHodgePawnExtensionComponent::HandlePlayerStateReplicated()
{
	// PlayerState 到达后重新检查 Pawn 初始化条件
	CheckDefaultInitialization();
}

void UHodgePawnExtensionComponent::SetupPlayerInputComponent()
{
	// 输入组件准备完成后重新检查 Pawn 是否可以进入 GameplayReady
	CheckDefaultInitialization();
}

void UHodgePawnExtensionComponent::CheckDefaultInitialization()
{
	// 先尝试推进 Pawn 上其他依赖 Feature 的初始化状态
	CheckDefaultInitializationForImplementers();

	// PawnExtension 默认的初始化状态链：Spawned -> DataAvailable -> DataInitialized -> GameplayReady
	static const TArray<FGameplayTag> StateChain = {
		HodgeGameplayTags::InitState_Spawned, HodgeGameplayTags::InitState_DataAvailable,
		HodgeGameplayTags::InitState_DataInitialized, HodgeGameplayTags::InitState_GameplayReady
	};

	// 从当前状态开始沿着状态链持续推进，直到某个条件不满足
	ContinueInitStateChain(StateChain);
}

bool UHodgePawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
                                                      FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	// 初始状态 -> Spawned：只要 Pawn 有效，就认为 Pawn 已经生成
	if (!CurrentState.IsValid() && DesiredState == HodgeGameplayTags::InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable：需要 PawnData 和必要的 Controller
	if (CurrentState == HodgeGameplayTags::InitState_Spawned && DesiredState ==
		HodgeGameplayTags::InitState_DataAvailable)
	{
		// PawnData 是进入 DataAvailable 的必要条件
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		// 服务器 Pawn 或本地控制 Pawn 都必须已经拥有 Controller
		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	// DataAvailable -> DataInitialized：必须等 Pawn 上所有 Feature 都完成 DataAvailable
	else if (CurrentState == HodgeGameplayTags::InitState_DataAvailable && DesiredState ==
		HodgeGameplayTags::InitState_DataInitialized)
	{
		// 只有所有 Feature 都到达 DataAvailable 后，才能进入 DataInitialized
		return Manager->HaveAllFeaturesReachedInitState(Pawn, HodgeGameplayTags::InitState_DataAvailable);
	}
	// DataInitialized -> GameplayReady：当前没有额外条件
	else if (CurrentState == HodgeGameplayTags::InitState_DataInitialized && DesiredState ==
		HodgeGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	// 当前状态无法进入目标状态
	return false;
}

void UHodgePawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
                                                         FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// DataInitialized 阶段目前没有实际初始化逻辑，由其他 Feature 监听状态变化并完成对应工作
	if (DesiredState == HodgeGameplayTags::InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void UHodgePawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// 其他 Feature 到达 DataAvailable 后，检查 PawnExtension 是否也可以继续初始化
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == HodgeGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UHodgePawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(
	FSimpleMulticastDelegate::FDelegate Delegate)
{
	// 防止同一个对象重复注册 ASC 初始化回调
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	// 如果 ASC 已经初始化，则注册完成后立即执行一次回调
	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UHodgePawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	// 防止同一个对象重复注册 ASC 解除初始化回调
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}
