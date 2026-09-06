/**
 * @file HodgeCombatCharacter.cpp
 * @brief AHodgeCombatCharacter 类的实现
 *
 * 战斗角色基类，当前包含角色基础移动、GAS、GameplayTag、Team 以及网络复制相关逻辑。
 * 作为 AHodgeCharacterBase 和 AHodgeHeroCharacter 之间的中间层，
 * 负责提供所有战斗角色共用的基础能力。
 */

#include "Character/HodgeCombatCharacter.h"

#include "SignificanceManager.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Camera/HodgeCameraComponent.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/PlayerController/HodgePlayerControllerBase.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Net/UnrealNetwork.h"

class AActor;
class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UInputComponent;

// 角色胶囊体使用的碰撞配置名称
static FName NAME_HodgeCharacterCollisionProfile_Capsule(TEXT("HodgePawnCapsule"));

// 角色骨骼网格使用的碰撞配置名称
static FName NAME_HodgeCharacterCollisionProfile_Mesh(TEXT("HodgePawnMesh"));

// 构造战斗角色并替换默认的 CharacterMovementComponent
AHodgeCombatCharacter::AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHodgeCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	// 尽可能关闭角色 Tick，角色移动由 CharacterMovementComponent 等系统驱动
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// 设置角色的网络裁剪距离平方
	SetNetCullDistanceSquared(900000000.0f);

	// 获取角色胶囊体组件
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);

	// 设置胶囊体半径和半高
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);

	// 设置角色胶囊体的碰撞配置
	CapsuleComp->SetCollisionProfileName(NAME_HodgeCharacterCollisionProfile_Capsule);

	// 获取角色骨骼网格组件
	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);

	// 将骨骼网格绕 Z 轴旋转 -90 度，使模型朝向符合角色坐标系
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// 模型导出时以 Y 轴为前方，这里旋转为 UE 默认的 X 轴前方
	MeshComp->SetCollisionProfileName(NAME_HodgeCharacterCollisionProfile_Mesh);

	// 获取自定义角色移动组件
	UHodgeCharacterMovementComponent* HodgeMoveComp = CastChecked<UHodgeCharacterMovementComponent>(
		GetCharacterMovement());

	// 设置重力缩放
	HodgeMoveComp->GravityScale = 1.0f;

	// 设置角色最大加速度
	HodgeMoveComp->MaxAcceleration = 2400.0f;

	// 设置制动摩擦系数
	HodgeMoveComp->BrakingFrictionFactor = 1.0f;

	// 设置角色制动摩擦力
	HodgeMoveComp->BrakingFriction = 6.0f;

	// 设置角色在地面移动时的摩擦力
	HodgeMoveComp->GroundFriction = 8.0f;

	// 设置行走状态下的制动减速度
	HodgeMoveComp->BrakingDecelerationWalking = 1400.0f;

	// 不使用 Controller 的期望旋转控制角色朝向
	HodgeMoveComp->bUseControllerDesiredRotation = false;

	// 不根据移动方向自动旋转角色
	HodgeMoveComp->bOrientRotationToMovement = false;

	// 设置角色旋转速度
	HodgeMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	// 禁止动画根运动期间由物理系统额外影响角色旋转
	HodgeMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;

	// 告诉导航系统角色支持蹲伏
	HodgeMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

	// 允许角色处于蹲伏状态时走下悬崖
	HodgeMoveComp->bCanWalkOffLedgesWhenCrouching = true;

	// 设置角色蹲伏后的胶囊体半高
	HodgeMoveComp->SetCrouchedHalfHeight(65.0f);

	//Pawn 扩展组件，负责连接 Pawn 与 AbilitySystem 等系统
	PawnExtComponent = CreateDefaultSubobject<UHodgePawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	//Pawn 的 AbilitySystem 初始化完成后注册回调
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));

	//Pawn 的 AbilitySystem 反初始化时注册回调
	PawnExtComponent->OnAbilitySystemUninitialized_Register(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	// 创建角色生命值组件
	//
	// HealthComponent = CreateDefaultSubobject<UHodgeHealthComponent>(TEXT("HealthComponent"));

	// 角色开始死亡时触发死亡流程
	// HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);

	// 角色死亡流程完成后触发销毁流程
	// HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	// 创建角色 Camera Component
	CameraComponent = CreateDefaultSubobject<UHodgeCameraComponent>(TEXT("CameraComponent"));

	// 设置 Camera Component 的相对位置
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

	// Pitch 不跟随 Controller 旋转
	bUseControllerRotationPitch = false;

	// Yaw 跟随 Controller 旋转
	bUseControllerRotationYaw = true;

	// Roll 不跟随 Controller 旋转
	bUseControllerRotationRoll = false;

	// 设置角色默认视线高度
	BaseEyeHeight = 80.0f;

	// 设置角色蹲伏时的视线高度
	CrouchedEyeHeight = 50.0f;
}

// Actor 的组件初始化阶段
void AHodgeCombatCharacter::PreInitializeComponents()
{
	// 调用父类初始化逻辑
	Super::PreInitializeComponents();
}

// Actor 开始游戏
void AHodgeCombatCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 获取当前 World
	UWorld* World = GetWorld();

	// 专用服务器不需要注册 SignificanceManager
	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);

	if (bRegisterWithSignificanceManager)
	{
		// 获取 Hodge 自定义 SignificanceManager
		// if (UHodgeSignificanceManager* SignificanceManager = USignificanceManager::Get<
		//     UHodgeSignificanceManager>(World))
		// {
		//     // 将角色注册到 SignificanceManager
		//     //@TODO: SignificanceManager->RegisterObject(this, (EFortSignificanceType)SignificanceType);
		// }
	}
}

// Actor 结束游戏
void AHodgeCombatCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 获取当前 World
	UWorld* World = GetWorld();

	// 专用服务器不需要处理 SignificanceManager
	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);

	if (bRegisterWithSignificanceManager)
	{
		// 获取 Hodge 自定义 SignificanceManager
		// if (UHodgeSignificanceManager* SignificanceManager = USignificanceManager::Get<
		//     UHodgeSignificanceManager>(World))
		// {
		//     // 从 SignificanceManager 中移除当前角色
		//     SignificanceManager->UnregisterObject(this);
		// }
	}
}

// 重置角色状态
void AHodgeCombatCharacter::Reset()
{
	// 禁用角色移动和碰撞
	DisableMovementAndCollision();

	// 调用蓝图中的 Reset 逻辑
	K2_OnReset();

	// 解除角色系统并销毁角色
	UninitAndDestroy();
}

// 注册需要进行网络复制的属性
void AHodgeCombatCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 只向 SimulatedProxy 复制角色加速度
	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);

	// 复制角色所属 Team ID
	DOREPLIFETIME(ThisClass, MyTeamID);
}

// Actor 网络复制之前调用，用于更新需要复制的数据
void AHodgeCombatCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	// 获取角色移动组件
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// 将加速度压缩为 XY 方向 + XY 大小 + Z 分量
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();

		// 用极坐标表示 XY 平面加速度
		double AccelXYRadians, AccelXYMagnitude;
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		// 将 [0, 2π] 的方向角压缩到 uint8 的 [0, 255]
		ReplicatedAcceleration.AccelXYRadians = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);

		// 将 [0, MaxAccel] 的 XY 加速度大小压缩到 [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);

		// 将 [-MaxAccel, MaxAccel] 的 Z 轴加速度压缩到 int8 的 [-127, 127]
		ReplicatedAcceleration.AccelZ = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);
	}
}

// Controller 发生变化时调用
void AHodgeCombatCharacter::NotifyControllerChanged()
{
	// 保存 Controller 变化之前的 Team ID
	// const FGenericTeamId OldTeamId = GetGenericTeamId();

	Super::NotifyControllerChanged();

	// Controller 变化后根据 Controller 更新角色所属 Team
	// if (HasAuthority() && (Controller != nullptr))
	// {
	//     // 如果 Controller 实现了 Team Agent 接口，则读取其 Team ID
	//     if (IHodgeTeamAgentInterface* ControllerWithTeam = Cast<IHodgeTeamAgentInterface>(Controller))
	//     {
	//        MyTeamID = ControllerWithTeam->GetGenericTeamId();
	//        ConditionalBroadcastTeamChanged(this, OldTeamId, MyTeamID);
	//     }
	// }
}

// 获取当前角色对应的 Hodge PlayerController
AHodgePlayerControllerBase* AHodgeCombatCharacter::GetHodgePlayerController() const
{
	// Controller 为空时允许返回 nullptr，否则要求类型必须正确
	return CastChecked<AHodgePlayerControllerBase>(Controller, ECastCheckedType::NullAllowed);
}

// 获取当前角色对应的 Hodge PlayerState
AHodgePlayerState* AHodgeCombatCharacter::GetHodgePlayerState() const
{
	// PlayerState 为空时允许返回 nullptr，否则要求类型必须正确
	return CastChecked<AHodgePlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

// 获取角色对应的 Hodge AbilitySystemComponent
UHodgeAbilitySystemComponentBase* AHodgeCombatCharacter::GetHodgeAbilitySystemComponent() const
{
	// 将通用 AbilitySystemComponent 转换为 Hodge 自定义 ASC
	return Cast<UHodgeAbilitySystemComponentBase>(GetAbilitySystemComponent());
}

// 实现 GAS 的 AbilitySystemInterface
UAbilitySystemComponent* AHodgeCombatCharacter::GetAbilitySystemComponent() const
{
	// 从 PawnExtensionComponent 获取 ASC
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetHodgeAbilitySystemComponent();
}

// AbilitySystem 初始化完成后调用
void AHodgeCombatCharacter::OnAbilitySystemInitialized()
{
	// 获取角色对应的 Hodge ASC
	UHodgeAbilitySystemComponentBase* HodgeASC = GetHodgeAbilitySystemComponent();
	check(HodgeASC);

	// 使用 ASC 初始化生命值组件
	// HealthComponent->InitializeWithAbilitySystem(HodgeASC);

	// 初始化角色 GameplayTag
	InitializeGameplayTags();
}

// AbilitySystem 反初始化时调用
void AHodgeCombatCharacter::OnAbilitySystemUninitialized()
{
	// 解除生命值组件与 ASC 的绑定
	// HealthComponent->UninitializeFromAbilitySystem();
}

// Pawn 被 Controller 占有时调用
void AHodgeCombatCharacter::PossessedBy(AController* NewController)
{
	// 保存被占有之前的 Team ID
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	// 通知 PawnExtensionComponent Controller 发生变化
	PawnExtComponent->HandleControllerChanged();

	// 如果 Controller 提供 Team 信息，则同步角色 Team
	// if (IHodgeTeamAgentInterface* ControllerAsTeamProvider = Cast<IHodgeTeamAgentInterface>(NewController))
	// {
	//     MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();

	//     // 监听 Controller 后续的 Team 变化
	//     ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	// }

	// 广播角色 Team 发生变化
	// ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

// Pawn 失去 Controller 占有时调用
void AHodgeCombatCharacter::UnPossessed()
{
	// 保存旧 Controller
	AController* const OldController = Controller;

	// 停止监听旧 Controller 的 Team 变化
	// const FGenericTeamId OldTeamID = MyTeamID;
	// if (IHodgeTeamAgentInterface* ControllerAsTeamProvider = Cast<IHodgeTeamAgentInterface>(OldController))
	// {
	//     ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	// }

	// 调用父类解除占有逻辑
	Super::UnPossessed();

	// 通知 PawnExtensionComponent Controller 发生变化
	PawnExtComponent->HandleControllerChanged();

	// 根据解除占有后的状态重新决定 Team
	// MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);

	// 广播 Team 变化
	// ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

// Controller 网络复制发生变化时调用
void AHodgeCombatCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	// 通知 PawnExtensionComponent Controller 已经复制完成
	PawnExtComponent->HandleControllerChanged();
}

// PlayerState 网络复制完成时调用
void AHodgeCombatCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 通知 PawnExtensionComponent PlayerState 已经复制完成
	PawnExtComponent->HandlePlayerStateReplicated();
}

// 设置玩家输入
void AHodgeCombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 由 PawnExtensionComponent 设置额外输入
	PawnExtComponent->SetupPlayerInputComponent();
}

// 初始化角色 GameplayTag
void AHodgeCombatCharacter::InitializeGameplayTags()
{
	// 获取角色对应的 ASC
	if (UHodgeAbilitySystemComponentBase* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		// 清除之前 Pawn 可能残留在 ASC 上的移动模式 GameplayTag
		for (const TPair<uint8, FGameplayTag>& TagMapping : HodgeGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				// 将普通移动模式 Tag 的 Loose Tag 数量清零
				HodgeASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		// 清除之前 Pawn 可能残留的自定义移动模式 GameplayTag
		for (const TPair<uint8, FGameplayTag>& TagMapping : HodgeGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				// 将自定义移动模式 Tag 的 Loose Tag 数量清零
				HodgeASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		// 获取当前角色移动组件
		UHodgeCharacterMovementComponent* HodgeMoveComp = CastChecked<UHodgeCharacterMovementComponent>(
			GetCharacterMovement());

		// 根据当前移动模式重新添加对应的 GameplayTag
		SetMovementModeTag(HodgeMoveComp->MovementMode, HodgeMoveComp->CustomMovementMode, true);
	}
}

// 获取角色当前拥有的 GameplayTag
void AHodgeCombatCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	// 从 ASC 获取当前角色所有 GameplayTag
	if (const UHodgeAbilitySystemComponentBase* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		HodgeASC->GetOwnedGameplayTags(TagContainer);
	}
}

// 判断角色是否拥有指定 GameplayTag
bool AHodgeCombatCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	// 委托 ASC 判断 GameplayTag
	if (const UHodgeAbilitySystemComponentBase* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		return HodgeASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

// 判断角色是否拥有指定 GameplayTag 容器中的全部 Tag
bool AHodgeCombatCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	// 委托 ASC 判断是否匹配全部 GameplayTag
	if (const UHodgeAbilitySystemComponentBase* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		return HodgeASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

// 判断角色是否拥有指定 GameplayTag 容器中的任意 Tag
bool AHodgeCombatCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	// 委托 ASC 判断是否至少匹配一个 GameplayTag
	if (const UHodgeAbilitySystemComponentBase* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		return HodgeASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

// 角色掉出世界时调用
void AHodgeCombatCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	// 通过生命值组件执行自毁处理
	// HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

// 角色开始死亡流程
void AHodgeCombatCharacter::OnDeathStarted(AActor*)
{
	// 禁用角色移动和碰撞
	DisableMovementAndCollision();
}

// 角色死亡流程完成
void AHodgeCombatCharacter::OnDeathFinished(AActor*)
{
	// 下一帧执行角色销毁，避免在当前死亡事件回调中直接销毁对象
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

// 禁用角色移动和碰撞
void AHodgeCombatCharacter::DisableMovementAndCollision()
{
	// 如果角色存在 Controller，则禁止 Controller 接收移动输入
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	// 获取角色胶囊体
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);

	// 禁用胶囊体碰撞
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 忽略所有碰撞通道
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 获取角色移动组件
	UHodgeCharacterMovementComponent* HodgeMoveComp = CastChecked<UHodgeCharacterMovementComponent>(
		GetCharacterMovement());

	// 立即清除当前速度
	HodgeMoveComp->StopMovementImmediately();

	// 禁用角色移动
	HodgeMoveComp->DisableMovement();
}

// 因死亡流程销毁角色
void AHodgeCombatCharacter::DestroyDueToDeath()
{
	// 触发蓝图中的死亡完成事件
	K2_OnDeathFinished();

	// 解除角色系统并执行销毁
	UninitAndDestroy();
}

// 解除角色相关系统并销毁角色
void AHodgeCombatCharacter::UninitAndDestroy()
{
	// 只有服务器负责解除 Controller 和设置角色生命周期
	if (GetLocalRole() == ROLE_Authority)
	{
		// 标记角色即将被销毁并解除 Controller
		DetachFromControllerPendingDestroy();

		// 延迟 0.1 秒自动销毁 Actor
		SetLifeSpan(0.1f);
	}

	// 获取角色对应的 ASC
	if (UHodgeAbilitySystemComponentBase* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		// 只有当前角色仍然是 ASC 的 AvatarActor 时才需要解除 ASC
		if (HodgeASC->GetAvatarActor() == this)
		{
			// 当前 PawnExtensionComponent 已被移除，ASC 解除逻辑暂时关闭
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	// 隐藏角色，但不立即销毁 Actor
	SetActorHiddenInGame(true);
}

// 角色移动模式发生变化时调用
void AHodgeCombatCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	// 获取自定义角色移动组件
	UHodgeCharacterMovementComponent* HodgeMoveComp = CastChecked<UHodgeCharacterMovementComponent>(
		GetCharacterMovement());

	// 移除旧移动模式对应的 GameplayTag
	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);

	// 添加新移动模式对应的 GameplayTag
	SetMovementModeTag(HodgeMoveComp->MovementMode, HodgeMoveComp->CustomMovementMode, true);
}

// 根据移动模式设置对应 GameplayTag
void AHodgeCombatCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	// 获取角色 ASC
	if (UHodgeAbilitySystemComponentBase* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		// 用于保存当前移动模式对应的 GameplayTag
		const FGameplayTag* MovementModeTag = nullptr;

		// 自定义移动模式从 CustomMovementModeTagMap 中查找
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = HodgeGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			// 普通移动模式从 MovementModeTagMap 中查找
			MovementModeTag = HodgeGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		// 找到有效 GameplayTag 后更新 Loose GameplayTag 数量
		if (MovementModeTag && MovementModeTag->IsValid())
		{
			// bTagEnabled 为 true 时添加 Tag，为 false 时移除 Tag
			HodgeASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

// 切换角色蹲伏状态
void AHodgeCombatCharacter::ToggleCrouch()
{
	// 获取自定义角色移动组件
	const UHodgeCharacterMovementComponent* HodgeMoveComp = CastChecked<UHodgeCharacterMovementComponent>(
		GetCharacterMovement());

	// 已经蹲伏或已经请求蹲伏时执行站起
	if (bIsCrouched || HodgeMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	// 只有在地面移动状态下才允许进入蹲伏
	else if (HodgeMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

// 角色开始蹲伏时调用
void AHodgeCombatCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	// 获取角色 ASC
	if (UHodgeAbilitySystemComponentBase* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		// 添加角色正在蹲伏的 GameplayTag
		HodgeASC->SetLooseGameplayTagCount(HodgeGameplayTags::Status_Crouching, 1);
	}

	// 调用父类的蹲伏逻辑
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

// 角色结束蹲伏时调用
void AHodgeCombatCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	// 获取角色 ASC
	if (UHodgeAbilitySystemComponentBase* HodgeASC = GetHodgeAbilitySystemComponent())
	{
		// 移除角色正在蹲伏的 GameplayTag
		HodgeASC->SetLooseGameplayTagCount(HodgeGameplayTags::Status_Crouching, 0);
	}

	// 调用父类的起身逻辑
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

// 判断角色是否允许跳跃
bool AHodgeCombatCharacter::CanJumpInternal_Implementation() const
{
	// 与 ACharacter 默认实现基本一致，但这里不额外检查角色是否处于蹲伏状态
	return JumpIsAllowedInternal();
}

// 接收到网络复制的加速度数据
void AHodgeCombatCharacter::OnRep_ReplicatedAcceleration()
{
	// 获取角色移动组件
	if (UHodgeCharacterMovementComponent* HodgeMovementComponent = Cast<UHodgeCharacterMovementComponent>(
		GetCharacterMovement()))
	{
		// 获取最大加速度，用于还原压缩数据
		const double MaxAccel = HodgeMovementComponent->MaxAcceleration;

		// 将 [0, 255] 的 XY 加速度大小还原到 [0, MaxAccel]
		const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * MaxAccel / 255.0;

		// 将 [0, 255] 的方向数据还原到 [0, 2π]
		const double AccelXYRadians = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0;

		// 创建解压后的加速度向量
		FVector UnpackedAcceleration(FVector::ZeroVector);

		// 将极坐标形式的 XY 加速度转换回笛卡尔坐标
		FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);

		// 将 [-127, 127] 的 Z 加速度还原到 [-MaxAccel, MaxAccel]
		UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0;

		// 将解压后的加速度交给移动组件使用
		HodgeMovementComponent->SetReplicatedAcceleration(UnpackedAcceleration);
	}
}

// Controller 所属 Team 发生变化时调用
void AHodgeCombatCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	// 保存当前角色原来的 Team ID
	const FGenericTeamId MyOldTeamID = MyTeamID;

	// 根据 Controller 新 Team 更新角色 Team
	// MyTeamID = IntegerToGenericTeamId(NewTeam);

	// 广播角色 Team 发生变化
	// ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}

// Team ID 网络复制发生变化时调用
void AHodgeCombatCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	// 广播角色 Team ID 发生变化
	// ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

// 尝试使用 FastSharedReplication 同步角色移动数据
bool AHodgeCombatCharacter::UpdateSharedReplication()
{
	// FastSharedReplication 只能由服务器执行
	if (GetLocalRole() == ROLE_Authority)
	{
		// 创建本次需要发送的共享移动数据
		FSharedRepMovement SharedMovement;

		// 从当前角色状态填充共享移动数据
		if (SharedMovement.FillForCharacter(this))
		{
			// 只有移动数据发生变化时才发送 RPC，避免每帧重复发送相同数据
			if (!SharedMovement.Equals(LastSharedReplication, this))
			{
				// 保存本次发送的数据，供下一帧进行比较
				LastSharedReplication = SharedMovement;

				// 保存当前网络移动模式
				ReplicatedMovementMode = SharedMovement.RepMovementMode;

				// 将移动数据通过 Multicast RPC 发送给客户端
				FastSharedReplication(SharedMovement);
			}

			return true;
		}
	}

	// 当前无法进行 FastSharedReplication，不发送数据
	return false;
}

// FastSharedReplication 在客户端执行的实现
void AHodgeCombatCharacter::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)
{
	// 回放过程中不执行快速移动复制
	if (GetWorld()->IsPlayingReplay())
	{
		return;
	}

	// 只有 SimulatedProxy 需要通过该 RPC 更新移动状态
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		// 更新服务器最后一次 Transform 更新时间戳
		ReplicatedServerLastTransformUpdateTimeStamp = SharedRepMovement.RepTimeStamp;

		// 如果网络移动模式发生变化，则通知移动组件
		if (ReplicatedMovementMode != SharedRepMovement.RepMovementMode)
		{
			ReplicatedMovementMode = SharedRepMovement.RepMovementMode;
			GetCharacterMovement()->bNetworkMovementModeChanged = true;
			GetCharacterMovement()->bNetworkUpdateReceived = true;
		}

		// 更新位置、旋转、速度等网络移动数据
		FRepMovement& MutableRepMovement = GetReplicatedMovement_Mutable();
		MutableRepMovement = SharedRepMovement.RepMovement;

		// 处理复制后的移动状态
		OnRep_ReplicatedMovement();

		// 更新代理角色的跳跃力状态
		bProxyIsJumpForceApplied = SharedRepMovement.bProxyIsJumpForceApplied;

		// 如果蹲伏状态发生变化，则执行对应的网络回调
		if (bIsCrouched != SharedRepMovement.bIsCrouched)
		{
			bIsCrouched = SharedRepMovement.bIsCrouched;
			OnRep_IsCrouched();
		}
	}
}

// 初始化共享移动数据
FSharedRepMovement::FSharedRepMovement()
{
	// 设置位置数据使用两位小数精度进行量化
	RepMovement.LocationQuantizationLevel = EVectorQuantization::RoundTwoDecimals;
}

// 从角色当前状态填充共享移动数据
bool FSharedRepMovement::FillForCharacter(ACharacter* Character)
{
	// 获取角色根组件
	if (USceneComponent* PawnRootComponent = Character->GetRootComponent())
	{
		// 获取角色移动组件
		UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();

		// 获取相对于世界原点重新基准化后的角色位置
		RepMovement.Location = FRepMovement::RebaseOntoZeroOrigin(PawnRootComponent->GetComponentLocation(), Character);

		// 保存角色当前旋转
		RepMovement.Rotation = PawnRootComponent->GetComponentRotation();

		// 保存角色当前线速度
		RepMovement.LinearVelocity = CharacterMovement->Velocity;

		// 将移动模式压缩为网络传输格式
		RepMovementMode = CharacterMovement->PackNetworkMovementMode();

		// 保存代理角色当前是否应该应用跳跃力
		bProxyIsJumpForceApplied = Character->bProxyIsJumpForceApplied || (Character->JumpForceTimeRemaining > 0.0f);

		// 保存角色当前是否处于蹲伏状态
		bIsCrouched = Character->bIsCrouched;

		// 默认情况下时间戳不参与复制
		if ((CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Linear) || CharacterMovement->
			bNetworkAlwaysReplicateTransformUpdateTimestamp)
		{
			// 获取服务器最后一次 Transform 更新的时间戳
			RepTimeStamp = CharacterMovement->GetServerLastTransformUpdateTimeStamp();
		}
		else
		{
			// 不需要时间戳时设置为 0
			RepTimeStamp = 0.f;
		}

		return true;
	}

	// 没有根组件时无法生成有效移动数据
	return false;
}

// 判断两份共享移动数据是否完全一致
bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const
{
	// 比较位置
	if (RepMovement.Location != Other.RepMovement.Location)
	{
		return false;
	}

	// 比较旋转
	if (RepMovement.Rotation != Other.RepMovement.Rotation)
	{
		return false;
	}

	// 比较线速度
	if (RepMovement.LinearVelocity != Other.RepMovement.LinearVelocity)
	{
		return false;
	}

	// 比较移动模式
	if (RepMovementMode != Other.RepMovementMode)
	{
		return false;
	}

	// 比较跳跃力状态
	if (bProxyIsJumpForceApplied != Other.bProxyIsJumpForceApplied)
	{
		return false;
	}

	// 比较蹲伏状态
	if (bIsCrouched != Other.bIsCrouched)
	{
		return false;
	}

	// 所有需要比较的数据都一致
	return true;
}

// 自定义网络序列化共享移动数据
bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	// 默认认为序列化成功
	bOutSuccess = true;

	// 序列化角色位置、旋转、速度等标准移动数据
	RepMovement.NetSerialize(Ar, Map, bOutSuccess);

	// 序列化移动模式
	Ar << RepMovementMode;

	// 序列化跳跃力状态
	Ar << bProxyIsJumpForceApplied;

	// 序列化蹲伏状态
	Ar << bIsCrouched;

	// 只有时间戳不为 0 时才额外传输时间戳
	uint8 bHasTimeStamp = (RepTimeStamp != 0.f);

	// 使用 1 bit 标记时间戳是否存在
	Ar.SerializeBits(&bHasTimeStamp, 1);

	if (bHasTimeStamp)
	{
		// 序列化时间戳
		Ar << RepTimeStamp;
	}
	else
	{
		// 接收端没有时间戳时保持为 0
		RepTimeStamp = 0.f;
	}

	return true;
}
