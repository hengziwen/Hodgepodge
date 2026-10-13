/** @file HodgeALSLocomotion.h
 * @brief 声明ALS移动对照实验相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Animation/HodgeAnimInstance.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeALSLocomotion.generated.h"

/** 定义ALS方向的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeALSDirection : uint8 { /* 前向选项。 */ Forward, /* 后向选项。 */ Backward, /* Left前向选项。 */ LeftForward, /* Left后向选项。 */ LeftBackward, /* Right前向选项。 */ RightForward, /* Right后向选项。 */ RightBackward };

/** Isolated ground locomotion experiment. Gameplay input and GAS remain owned by Hodge. */
/** 封装ALS移动对照实验的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeALSLocomotion : public UHodgeAnimInstance
{
	GENERATED_BODY()

public:
	/** 在游戏线程采集当前角色的动画表现数据。
	 * @param DeltaSeconds 本次更新的时间步长，单位秒。
	 */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	/** 获取实验场景活动状态。
	 * @return 实验场景活动状态的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="ALS Lab")
	FName GetLabActiveState();
	/** 设置HipBias。
	 * @param Bias Bias，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, Category="ALS Lab")
	void SetHipBias(float Bias) { HipOrientationBias = FMath::Clamp(Bias, -1.f, 1.f); }

	// 实验场景方向的枚举配置或当前取值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	EHodgeALSDirection LabDirection = EHodgeALSDirection::Forward;
	// 实验场景前向的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabForward = 1.f;
	// 实验场景后向的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabBackward = 0.f;
	// 实验场景Left的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabLeft = 0.f;
	// 实验场景Right的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabRight = 0.f;
	// 实验场景Gait的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabGait = 0.f;
	// 实验场景Play速率的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabPlayRate = 1.f;
	// 实验场景速度的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabSpeed = 0.f;
	// 实验场景视图角度的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabViewAngle = 0.f;
	// 实验场景Yaw角偏移的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabYawOffset = 0.f;
	// 实验场景双脚Crossing的数值或阈值。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	float LabFeetCrossing = 0.f;
	// HipOrientationBias的数值或阈值。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ALS Lab", meta=(ClampMin="-1", ClampMax="1"))
	float HipOrientationBias = 0.f;
	// 前向后向Yaw角曲线的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
	TObjectPtr<class UCurveFloat> ForwardBackwardYawCurve;
	// LeftYaw角曲线的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
	TObjectPtr<class UCurveFloat> LeftYawCurve;
	// RightYaw角曲线的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, Category="ALS Lab")
	TObjectPtr<class UCurveFloat> RightYawCurve;
	// 控制是否实验场景Idle的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabIdle = true;
	// 控制是否实验场景Fall的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabFall = false;
	// 控制是否实验场景F的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabF = false;
	// 控制是否实验场景B的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabB = false;
	// 控制是否实验场景LF的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabLF = false;
	// 控制是否实验场景LB的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabLB = false;
	// 控制是否实验场景RF的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabRF = false;
	// 控制是否实验场景RB的状态标记。
	UPROPERTY(Transient, BlueprintReadOnly, Category="ALS Lab")
	bool bLabRB = false;

private:
	// 方向按住Remaining的数值或阈值。
	float DirectionHoldRemaining = 0.f;
};

/** 封装ALS制作的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeALSAuthoring : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Only accepts a new, empty CodexText animation blueprint. Never rewrites another graph. */
	/** 构建移动表现图。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @param WalkRunAnimations Walk奔跑Animations，由调用方传入。
	 * @param Idle Idle，由调用方传入。
	 * @param Fall Fall，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
	static bool BuildLocomotionGraph(UObject* Blueprint, const TArray<UObject*>& WalkRunAnimations, UObject* Idle,
	                                 UObject* Fall);
};
