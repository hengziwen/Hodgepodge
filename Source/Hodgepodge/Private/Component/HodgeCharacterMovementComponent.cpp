// 111 屎山代码来袭

#include "Component/HodgeCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterMovementComponent)

// 定义“移动停止”GameplayTag，用于通过 GAS 禁止角色移动和旋转
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_MovementStopped, "Gameplay.MovementStopped");

namespace HodgeCharacter
{
	// 地面检测向下追踪的最大距离
	static float GroundTraceDistance = 100000.0f;

	// 控制地面检测射线长度的控制台变量，可在运行时通过控制台修改
	FAutoConsoleVariableRef CVar_GroundTraceDistance(
		TEXT("HodgeCharacter.GroundTraceDistance"), GroundTraceDistance,
		TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
};


UHodgeCharacterMovementComponent::UHodgeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// 模拟角色移动，存在服务端同步加速度时需要保护复制过来的加速度不被父类逻辑覆盖
void UHodgeCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	if (bHasReplicatedAcceleration)
	{
		// 保存网络同步过来的加速度，防止父类模拟过程中修改它
		const FVector OriginalAcceleration = Acceleration;

		// 执行 CharacterMovementComponent 原本的移动模拟逻辑
		Super::SimulateMovement(DeltaTime);

		// 恢复复制过来的加速度，保证本次移动使用的是服务端同步值
		Acceleration = OriginalAcceleration;
	}
	else
	{
		// 没有复制加速度时，直接使用父类默认移动模拟逻辑
		Super::SimulateMovement(DeltaTime);
	}
}

// 判断当前角色是否允许尝试跳跃
bool UHodgeCharacterMovementComponent::CanAttemptJump() const
{
	// 与 UCharacterMovementComponent 默认实现类似，但这里不检查蹲伏状态
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling());

	// Falling 状态用于支持二段跳以及跳跃持续按压等情况，具体是否合法由 Character 再进行校验
}

void UHodgeCharacterMovementComponent::InitializeComponent()
{
	// 执行父类移动组件的初始化逻辑
	Super::InitializeComponent();
}

// 获取角色当前脚下的地面信息，必要时会重新进行地面检测
const FHodgeCharacterGroundInfo& UHodgeCharacterMovementComponent::GetGroundInfo()
{
	// 没有角色或者本帧已经更新过地面信息时，直接返回缓存
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}

	// Walking 状态下 CharacterMovementComponent 已经计算出了地面信息，直接复用
	if (MovementMode == MOVE_Walking)
	{
		// 使用当前移动组件计算好的地面碰撞结果
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;

		// Walking 状态下认为角色已经站在地面上，因此距离为 0
		CachedGroundInfo.GroundDistance = 0.0f;
	}
	else
	{
		// 获取角色胶囊体，用于计算角色与地面的实际距离
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);

		// 获取未缩放的胶囊体半高
		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();

		// 使用 UpdatedComponent 的碰撞类型作为地面检测通道，没有时默认使用 Pawn 通道
		const ECollisionChannel CollisionChannel = (UpdatedComponent
			                                            ? UpdatedComponent->GetCollisionObjectType()
			                                            : ECC_Pawn);

		// 从角色当前位置开始进行地面检测
		const FVector TraceStart(GetActorLocation());

		// 向角色下方进行超长距离射线检测
		const FVector TraceEnd(TraceStart.X, TraceStart.Y,
		                       (TraceStart.Z - HodgeCharacter::GroundTraceDistance - CapsuleHalfHeight));

		// 配置射线检测参数，并忽略角色自身
		FCollisionQueryParams QueryParams(
			SCENE_QUERY_STAT(HodgeCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);

		// 配置碰撞响应参数
		FCollisionResponseParams ResponseParam;

		// 根据当前移动组件初始化碰撞查询参数
		InitCollisionParams(QueryParams, ResponseParam);

		// 保存射线检测结果
		FHitResult HitResult;

		// 从角色位置向下发射射线，检测角色下方的地面
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams,
		                                     ResponseParam);

		// 保存本次地面检测的碰撞结果
		CachedGroundInfo.GroundHitResult = HitResult;

		// 默认认为地面距离非常远，后续如果检测到地面会重新计算
		CachedGroundInfo.GroundDistance = HodgeCharacter::GroundTraceDistance;

		// NavWalking 状态下直接认为角色贴着导航地面
		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.0f;
		}
		// 检测到阻挡地面后，根据碰撞距离和胶囊体半高计算角色到地面的实际距离
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}

	// 记录本次地面信息更新发生在哪一帧，用于避免同一帧重复检测
	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	// 返回当前缓存的地面信息
	return CachedGroundInfo;
}

// 接收服务端同步过来的角色加速度
void UHodgeCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)
{
	// 标记已经收到服务端复制的加速度
	bHasReplicatedAcceleration = true;

	// 使用服务端同步过来的加速度
	Acceleration = InAcceleration;
}

// 获取角色在当前帧允许产生的旋转变化
FRotator UHodgeCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	// 从角色 Owner 上获取对应的 AbilitySystemComponent
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		// 如果角色拥有 MovementStopped 标签，则禁止角色旋转
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
		{
			// 返回零旋转，表示本帧不允许发生任何旋转
			return FRotator(0, 0, 0);
		}
	}

	// 没有禁止移动时使用 CharacterMovementComponent 默认的旋转计算
	return Super::GetDeltaRotation(DeltaTime);
}

// 获取角色当前最大移动速度
float UHodgeCharacterMovementComponent::GetMaxSpeed() const
{
	// 从角色 Owner 上获取对应的 AbilitySystemComponent
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		// 如果角色拥有 MovementStopped 标签，则禁止角色移动
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
		{
			// 返回 0 速度，使角色完全停止移动
			return 0;
		}
	}

	// 没有禁止移动时使用 CharacterMovementComponent 默认最大速度
	return Super::GetMaxSpeed();
}
