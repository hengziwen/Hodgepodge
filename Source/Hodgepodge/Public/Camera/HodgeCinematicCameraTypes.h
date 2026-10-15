/** @file HodgeCinematicCameraTypes.h @brief 定义技能相机归属和无状态分镜采样结果。 */
#pragma once
#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "HodgeCinematicCameraTypes.generated.h"

/** 代次区分同一技能的重复激活，旧回调不能释放新相机。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAbilityCameraHandle
{
	GENERATED_BODY()
	/** 能力授予身份，不代替单次执行身份。 */
	UPROPERTY(BlueprintReadOnly) FGameplayAbilitySpecHandle SpecHandle;
	/** 本地执行GUID，不要求与服务器数值相同。 */
	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
	/** Hero生成的相机申请代次，零表示未申请。 */
	UPROPERTY(BlueprintReadOnly) int32 Generation = 0;
	/** 返回是否持有有效授予身份及申请代次。 */
	bool IsValid() const { return SpecHandle.IsValid() && Generation > 0; }
	/** 所有字段匹配才属于同一份相机申请。 */
	bool operator==(const FHodgeAbilityCameraHandle& Other) const
	{ return SpecHandle == Other.SpecHandle && ExecutionId == Other.ExecutionId && Generation == Other.Generation; }
};

/** 配置求值输出局部偏移，最终世界变换由CameraMode执行。 */
struct HODGEPODGE_API FHodgeCinematicCameraSample
{
	/** 本次选中的分镜序号，无有效采样时为INDEX_NONE。 */
	int32 ShotIndex = INDEX_NONE;
	/** 基于角色水平坐标架的相机位置。 */
	FVector CameraOffset = FVector::ZeroVector;
	/** 基于角色水平坐标架的观察点偏移。 */
	FVector LookAtOffset = FVector::ZeroVector;
	/** 镜头水平视野角，单位度。 */
	float FieldOfView = 60.f;
};

/** 只读表现诊断，供调参和PIE验证追踪真实执行。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeCinematicCameraState
{
	GENERATED_BODY()
	/** 当前电影申请身份，失效时不允许操作相机。 */
	UPROPERTY(BlueprintReadOnly) FHodgeAbilityCameraHandle Handle;
	/** 主角色实际蒙太奇实例，停止或换实例后重新绑定。 */
	UPROPERTY(BlueprintReadOnly) int32 MontageInstanceId = INDEX_NONE;
	/** 主角色蒙太奇源秒，不是独立累计的世界秒。 */
	UPROPERTY(BlueprintReadOnly) float SourcePosition = 0.f;
	/** 原生通知条的开始和结束源秒。 */
	UPROPERTY(BlueprintReadOnly) float WindowStart = 0.f;
	UPROPERTY(BlueprintReadOnly) float WindowEnd = 0.f;
	/** 本帧电影镜头贡献，正常退出末帧为零。 */
	UPROPERTY(BlueprintReadOnly) float Weight = 0.f;
	/** 当前分镜名，没有有效电影申请时为空。 */
	UPROPERTY(BlueprintReadOnly) FName ShotId;
	/** 只表示本地电影申请存在，不作为服务器技能就绪条件。 */
	UPROPERTY(BlueprintReadOnly) bool bActive = false;
};
