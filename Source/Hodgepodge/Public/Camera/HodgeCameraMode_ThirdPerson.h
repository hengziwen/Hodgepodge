// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "HodgeCameraMode.h"
#include "HodgeCameraMode_ThirdPerson.generated.h"


struct FHodgePenetrationAvoidanceFeeler;
class UCurveVector;

/**
 * 第三人称相机模式，负责处理目标跟随、蹲伏偏移以及相机碰撞防穿透。
 */
UCLASS(Blueprintable)
class UHodgeCameraMode_ThirdPerson : public UHodgeCameraMode
{
	GENERATED_BODY()

public:
	UHodgeCameraMode_ThirdPerson();

protected:
	// 更新第三人称相机的最终视图数据。
	virtual void UpdateView(float DeltaTime) override;

	// 根据目标角色的位置、旋转和蹲伏状态更新相机。
	void UpdateForTarget(float DeltaTime);

	// 检测并处理相机与场景之间的穿透问题。
	void UpdatePreventPenetration(float DeltaTime);

	// 根据碰撞检测结果将相机限制在不会穿透的位置。
	void PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc, FVector& CameraLoc,
	                              float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly);

	// 绘制第三人称相机的调试信息。
	virtual void DrawDebug(UCanvas* Canvas) const override;

protected:
	// 根据目标的俯仰角计算相机相对于目标的局部空间偏移。
	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "!bUseRuntimeFloatCurves"))
	TObjectPtr<const UCurveVector> TargetOffsetCurve;

	// 是否使用运行时可编辑的 Float Curve 替代 CurveVector。
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	bool bUseRuntimeFloatCurves;

	// 目标相机偏移的 X 轴曲线。
	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
	FRuntimeFloatCurve TargetOffsetX;

	// 目标相机偏移的 Y 轴曲线。
	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
	FRuntimeFloatCurve TargetOffsetY;

	// 目标相机偏移的 Z 轴曲线。
	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUseRuntimeFloatCurves"))
	FRuntimeFloatCurve TargetOffsetZ;

	// 控制角色蹲伏时相机偏移的平滑速度。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Third Person")
	float CrouchOffsetBlendMultiplier = 5.0f;

	// 相机穿透修正相关参数。
public:
	// 相机发生穿透后，从正常位置过渡到修正位置所需的时间。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	float PenetrationBlendInTime = 0.1f;

	// 相机脱离穿透后，从修正位置恢复到正常位置所需的时间。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	float PenetrationBlendOutTime = 0.15f;

	// 是否启用相机碰撞检测，防止相机进入场景几何体。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	bool bPreventPenetration = true;

	// 是否启用预测性碰撞检测，在即将撞墙前提前拉近相机。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	bool bDoPredictiveAvoidance = true;

	// 相机发生碰撞后额外向外推出的距离。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float CollisionPushOutDistance = 2.f;

	// 当相机距离被碰撞压缩到原始距离的该比例时触发穿透报告。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float ReportPenetrationPercent = 0.f;

	// 用于检测相机周围障碍物的多条探测射线配置。
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TArray<FHodgePenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;

	// 当前从目标到期望相机位置的射线被阻挡的比例。
	UPROPERTY(Transient)
	float AimLineToDesiredPosBlockedPct;

	// 相机穿透检测过程中实际命中的 Actor，仅用于调试。
	UPROPERTY(Transient)
	TArray<TObjectPtr<const AActor>> DebugActorsHitDuringCameraPenetration;

#if ENABLE_DRAW_DEBUG
	// 上一次绘制相机调试信息的时间。
	mutable float LastDrawDebugTime = -MAX_FLT;
#endif

protected:
	// 设置角色蹲伏状态下相机最终需要达到的偏移量。
	void SetTargetCrouchOffset(FVector NewTargetOffset);

	// 平滑更新当前蹲伏相机偏移。
	void UpdateCrouchOffset(float DeltaTime);

	// 开始进行蹲伏偏移过渡时记录的初始偏移。
	FVector InitialCrouchOffset = FVector::ZeroVector;

	// 当前目标蹲伏偏移量。
	FVector TargetCrouchOffset = FVector::ZeroVector;

	// 当前蹲伏偏移的插值进度，0表示初始状态，1表示完成。
	float CrouchOffsetBlendPct = 1.0f;

	// 当前实际应用到相机上的蹲伏偏移。
	FVector CurrentCrouchOffset = FVector::ZeroVector;
};
