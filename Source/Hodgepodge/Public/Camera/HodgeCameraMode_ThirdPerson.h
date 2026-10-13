/** @file HodgeCameraMode_ThirdPerson.h
 * @brief 声明第三人称相机模式相关的类型、接口与现有处理流程。
 */
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
UCLASS(Abstract, Blueprintable)
class UHodgeCameraMode_ThirdPerson : public UHodgeCameraMode
{
	GENERATED_BODY()

public:
	/** 构造第三人称相机模式并初始化其默认状态。 */
	UHodgeCameraMode_ThirdPerson();

	/** 锁定模式复用默认第三人称镜头的距离、FOV 和避障配置。 */
	void CopyViewSettingsFrom(const UHodgeCameraMode_ThirdPerson* Source);

protected:
	// 更新第三人称相机的最终视图数据。
	/** 更新视图。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void UpdateView(float DeltaTime) override;

	// 根据目标角色的位置、旋转和蹲伏状态更新相机。
	/** 更新关联目标。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	void UpdateForTarget(float DeltaTime);

	// 检测并处理相机与场景之间的穿透问题。
	/** 更新PreventPenetration。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	void UpdatePreventPenetration(float DeltaTime);

	// 根据碰撞检测结果将相机限制在不会穿透的位置。
	/** 准备vent相机Penetration。
	 * @param ViewTarget 视图目标，由调用方传入。
	 * @param SafeLoc 安全Loc，由调用方传入。
	 * @param CameraLoc 相机Loc，由调用方传入。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param DistBlockedPct Dist阻塞Pct，由调用方传入。
	 * @param bSingleRayOnly 是否SingleRayOnly的控制标记。
	 */
	void PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc, FVector& CameraLoc,
	                              float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly);

	// 绘制第三人称相机的调试信息。
	/** 按既有调试设置绘制当前状态或检测几何。
	 * @param Canvas 画布，由调用方传入。
	 */
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
	/** 设置目标下蹲偏移。
	 * @param NewTargetOffset 新目标偏移，由调用方传入。
	 */
	void SetTargetCrouchOffset(FVector NewTargetOffset);

	// 平滑更新当前蹲伏相机偏移。
	/** 更新下蹲偏移。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
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
