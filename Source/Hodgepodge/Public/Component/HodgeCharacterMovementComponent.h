// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "NativeGameplayTags.h"

#include "HodgeCharacterMovementComponent.generated.h"

class UObject;
struct FFrame;

// 角色移动停止时使用的 GameplayTag。
HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);

/**
 * @brief 角色当前脚下地面的信息。
 * @note 地面信息不会每帧强制更新，而是在需要访问时检查并更新。
 */
USTRUCT(BlueprintType)
struct FHodgeCharacterGroundInfo
{
	GENERATED_BODY()

	// 初始化地面信息的缓存帧号和地面距离。
	FHodgeCharacterGroundInfo()
		: LastUpdateFrame(0)
		  , GroundDistance(0.0f)
	{
	}

	// 上一次更新地面信息时对应的帧号，用于判断缓存是否过期。
	uint64 LastUpdateFrame;

	// 角色脚下检测到的地面碰撞信息。
	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	// 角色当前位置到地面的距离。
	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};


/**
 * @brief Hodgepodge 项目使用的角色移动组件基类。
 *
 * 继承自 UCharacterMovementComponent，在 UE 默认角色移动系统的基础上扩展项目自己的移动逻辑。
 */
UCLASS(Config = Game)
class HODGEPODGE_API UHodgeCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// 构造角色移动组件并初始化移动相关配置。
	UHodgeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	// 模拟角色移动，用于处理角色移动状态和网络移动模拟。
	virtual void SimulateMovement(float DeltaTime) override;

	// 判断角色当前是否允许尝试跳跃。
	virtual bool CanAttemptJump() const override;

	// 获取角色当前的地面信息，如果缓存已经过期则先更新地面信息。
	UFUNCTION(BlueprintCallable, Category = "Hodge|CharacterMovement")
	const FHodgeCharacterGroundInfo& GetGroundInfo();

	// 设置从服务器复制过来的角色加速度。
	void SetReplicatedAcceleration(const FVector& InAcceleration);

	//~UMovementComponent interface

	// 获取指定时间间隔内角色应该产生的旋转变化量。
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;

	// 获取角色当前移动状态下允许使用的最大速度。
	virtual float GetMaxSpeed() const override;

	//~End of UMovementComponent interface

protected:
	// 初始化移动组件，并执行组件级初始化逻辑。
	virtual void InitializeComponent() override;

protected:
	// 缓存角色当前脚下的地面信息，不要直接访问，应通过 GetGroundInfo() 获取。
	FHodgeCharacterGroundInfo CachedGroundInfo;

	// 标记当前是否已经获得过网络复制的加速度。
	UPROPERTY(Transient)
	bool bHasReplicatedAcceleration = false;
};
