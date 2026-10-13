/** @file HodgeGroundedLocomotion.h
 * @brief 声明地面移动实验数据相关的类型、接口与现有处理流程。
 */
#pragma once
#include "CoreMinimal.h"
#include "CodexText/HodgeALSLocomotion.h"
#include "HodgeGroundedLocomotion.generated.h"

/** Flat-ground stop, turn and foot-lock layer for the CodexText experiment. */
/** 封装地面移动实验数据的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeGroundedLocomotion : public UHodgeALSLocomotion
{
	GENERATED_BODY()

public:
	/** 执行原生初始化动画。 */
	virtual void NativeInitializeAnimation() override;
	/** 在游戏线程采集当前角色的动画表现数据。
	 * @param Dt Dt，由调用方传入。
	 */
	virtual void NativeUpdateAnimation(float Dt) override;
    /** 执行原生Uninitialize动画。 */
    virtual void NativeUninitializeAnimation() override;
	/** 获取地面移动状态。
	 * @return 地面移动状态的当前值或引用。
	 */
	UFUNCTION(BlueprintPure)
	FName GetGroundedState();
	/** 获取地面移动旋转目标。
	 * @return 地面移动旋转目标的当前值或引用。
	 */
	UFUNCTION(BlueprintPure)
	float GetGroundedRotationTarget() const { return RotationTarget; }

	/** 判断Turning输入Place。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure)
	bool IsTurningInPlace() const { return bGroundTurnL || bGroundTurnR; }
	/** 设置Overlay启用。
	 * @param bEnabled 是否启用的控制标记。
	 */
	UFUNCTION(BlueprintCallable, Category="Grounded|Combat")
	void SetOverlayEnabled(bool bEnabled) { OverlayTarget = bEnabled ? 1.f : 0.f; }
	/** 设置Left手部Grip。
	 * @param Component 组件，由调用方传入。
	 * @param Socket 插槽，由调用方传入。
	 * @param Weight Weight，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, Category="Grounded|Combat")
	void SetLeftHandGrip(class USceneComponent* Component, FName Socket, float Weight = 1.f);
	// Overlay姿势的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grounded|Combat")
	TObjectPtr<class UAnimSequence> OverlayPose;
	// Overlay混合权重的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Combat")
	float OverlayAlpha = 0.f;
	// 手部IK混合权重的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Combat")
	float HandIKAlpha = 0.f;
	// 手部Grip位置的几何或变换数据。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Combat")
	FVector HandGripLocation = FVector::ZeroVector;
	// 手部Grip旋转的几何或变换数据。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Combat")
	FRotator HandGripRotation = FRotator::ZeroRotator;

	// 控制是否地面Idle的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundIdle = true;
	// 控制是否地面移动的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundMove = false;
	// 控制是否地面停止WalkL的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundStopWalkL = false;
	// 控制是否地面停止WalkR的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundStopWalkR = false;
	// 控制是否地面停止奔跑L的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundStopRunL = false;
	// 控制是否地面停止奔跑R的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundStopRunR = false;
	// 控制是否地面转向L的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundTurnL = false;
	// 控制是否地面转向R的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bGroundTurnR = false;
	// 地面动作时间的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	float GroundActionTime = 0.f;
	// 地面动作Play速率的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	float GroundActionPlayRate = 1.f;
	// 控制是否停止Planted的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bStopPlanted = false;
	// 控制是否停止Airborne的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bStopAirborne = true;
	// Stride缩放的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Stride")
	float StrideScale = 1.f;
	// Stride混合权重的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Stride")
	float StrideAlpha = 0.f;
	// Diagonal缩放的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Stride")
	float DiagonalScale = 1.f;
	// Stride方向的几何或变换数据。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Stride")
	FVector StrideDirection = FVector::ForwardVector;
	// 控制是否启用TerrainIK的状态标记。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grounded|Terrain")
	bool bEnableTerrainIK = true;
	// Terrain混合权重的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded|Terrain")
	float TerrainAlpha = 0.f;
	// 脚部混合权重L的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	float FootAlphaL = 0.f;
	// 脚部混合权重R的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	float FootAlphaR = 0.f;
	// 脚部目标L的几何或变换数据。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	FVector FootTargetL = FVector::ZeroVector;
	// 脚部目标R的几何或变换数据。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	FVector FootTargetR = FVector::ZeroVector;
	// 脚部旋转L的几何或变换数据。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	FRotator FootRotationL = FRotator::ZeroRotator;
	// 脚部旋转R的几何或变换数据。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	FRotator FootRotationR = FRotator::ZeroRotator;
	// 控制是否支持Left的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="Grounded")
	bool bSupportLeft = true;
	// 地面Actions的记录集合。
	UPROPERTY(EditDefaultsOnly, Category="Grounded")
	TArray<TObjectPtr<class UAnimSequence>> GroundActions;
	// 转向Yaw角Left的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, Category="Grounded")
	TObjectPtr<class UCurveFloat> TurnYawLeft;
	// 转向Yaw角Right的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, Category="Grounded")
	TObjectPtr<class UCurveFloat> TurnYawRight;
	// 转向阈值的数值或阈值。
	UPROPERTY(EditDefaultsOnly, Category="Grounded")
	float TurnThreshold = 70.f;

private:
    // 手部Grip组件的弱引用，不延长目标对象生命周期。
    TWeakObjectPtr<class USceneComponent> HandGripComponent;
    // 手部Grip插槽的数据或对象引用。
    FName HandGripSocket;
    // 手部GripWeight的数值或阈值。
    float HandGripWeight = 0.f;
    // Overlay目标的数值或阈值。
    float OverlayTarget = 0.f;
    /** 缓存当前脚部最终姿势以供后续表现计算。 */
    void CacheFinalizedFootPose();
    // 姿势网格的弱引用，不延长目标对象生命周期。
    TWeakObjectPtr<class USkeletalMeshComponent> PoseMesh;
    // Finalized姿势句柄对应的事件通知。
    FDelegateHandle FinalizedPoseHandle;
	// 前次原始双脚的几何或变换数据。
	FTransform PreviousRawFeet[2];
	// 锁定双脚的几何或变换数据。
	FTransform LockedFeet[2];
	// 控制是否Have姿势的状态标记。
	bool bHavePose = false;
	// 控制是否Had移动的状态标记。
	bool bHadMovement = false;
	// 控制是否上次Walk的状态标记。
	bool bLastWalk = false;
	// 控制是否锁定的状态标记。
	bool bLocked[2] = {false, false};
	// 前次锁定曲线的数值或阈值。
	float PreviousLockCurve[2] = {0.f, 0.f};
	// 旋转目标的数值或阈值。
	float RotationTarget = 0.f;
	// 转向起始Yaw角的数值或阈值。
	float TurnStartYaw = 0.f;
	// 转向Delay的数值或阈值。
	float TurnDelay = 0.f;
	// 停止条目Contact的数值或阈值。
	float StopEntryContact[2] = {0.f, 0.f};
	// 动作索引的数值或阈值。
	int32 ActionIndex = INDEX_NONE;
	// 前次Actor对象位置的几何或变换数据。
	FVector PreviousActorLocation = FVector::ZeroVector;
};

/** 封装地面动画图制作辅助的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeGroundedAuthoring : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 按当前地面动画配置接入腿部IK修正。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
	static bool RepairGroundedLegIK(UObject* Blueprint);
	/** 按现有地面状态规则整理动画图转换。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
	static bool RefineGroundedTransitions(UObject* Blueprint);
	/** 添加Stride层。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
	static bool AddStrideLayer(UObject* Blueprint);
	/** 添加Terrain层。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
	static bool AddTerrainLayer(UObject* Blueprint);
	/** 添加战斗层。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @param Overlay Overlay，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
	static bool AddCombatLayer(UObject* Blueprint, UObject* Overlay);

	/** 添加地面移动层。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @param Actions Actions，由调用方传入。
	 * @param Idle Idle，由调用方传入。
	 * @param Fall Fall，由调用方传入。
	 * @param PreviewMesh Preview网格，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
	static bool AddGroundedLayer(UObject* Blueprint, const TArray<UObject*>& Actions, UObject* Idle, UObject* Fall,
	                             UObject* PreviewMesh);
};
