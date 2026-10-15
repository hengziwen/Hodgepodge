/** @file HodgeCinematicCameraProfile.h @brief 以蒙太奇窗口归一化时间描述可编辑大招分镜。 */
#pragma once
#include "Engine/DataAsset.h"
#include "Camera/HodgeCinematicCameraTypes.h"
#include "Engine/Scene.h"
#include "HodgeCinematicCameraProfile.generated.h"
class UCurveVector;
class UCurveFloat;
class USkeletalMeshComponent;
class UCameraShakeBase;
class USoundBase;

/** 施法者专属表现节点，不作为世界特效或伤害授权事件。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeCinematicCameraEvent
{
	GENERATED_BODY()
	/** 同一执行只消费一次，恢复实例时不会重新播放过去的节点。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName EventId;
	/** 相对于电影窗口的0～1触发位置。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Time = .5f;
	/** 可选独立震屏类，不允许共享单例以免清理其他技能的震屏。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UCameraShakeBase> CameraShake;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ShakeScale = 1.f;
	/** 可选本地短音效，不支持持续循环音轨。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USoundBase> Sound;
};

/** 单段运镜的角色局部构图，不持有运行角色或动画实例。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeCinematicCameraShot
{
	GENERATED_BODY()
	/** 用于资产诊断和后续事件对照的唯一分镜名。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ShotId;
	/** 电影窗口内的归一化起点。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="1")) float Start = 0.f;
	/** 电影窗口内的归一化终点。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="1")) float End = 1.f;
	/** 位置参考骨骼；空值显式使用角色根部。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName Anchor;
	/** 观察点骨骼；空值显式使用角色根部。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName LookAt;
	/** 运镜起点的局部厘米偏移。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector StartOffset = FVector(180.f, 70.f, 15.f);
	/** 运镜终点的局部厘米偏移。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector EndOffset = FVector(140.f, 35.f, 10.f);
	/** 观察点的局部厘米偏移。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector LookAtOffset = FVector::ZeroVector;
	/** 起点与终点水平视野角，单位度。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector2D FOV = FVector2D(60.f, 45.f);
	/** 可选绝对局部位置曲线，曲线横轴为本段0～1时间。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UCurveVector> PositionCurve;
	/** 可选水平FOV曲线，曲线横轴为本段0～1时间。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UCurveFloat> FOVCurve;
	/** 本段开始与上一段末帧混合占本段时间的比例；零为硬切。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="1")) float BlendFraction = .2f;
};

/** 电影镜头配置与纯采样逻辑，播放身份保存在Hero而非资产。 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeCinematicCameraProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	/** 分镜必须连续覆盖0～1，同名或空洞视为配置错误。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shots") TArray<FHodgeCinematicCameraShot> Shots;
	/** 本地镜头事件与主蒙太奇共享源位置，空资源表示未配置正式表现。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation") TArray<FHodgeCinematicCameraEvent> LocalEvents;
	/** 仅在当前窗口按贡献度叠加，不改组件或场景永久后处理配置。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation") FPostProcessSettings PostProcess;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation", meta=(ClampMin="0", ClampMax="1")) float PostProcessWeight = 0.f;
	/** 切入和返回占电影窗口的比例，均以主蒙太奇源时间求权重。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend") FVector2D BlendFractions = FVector2D(.16f, .2f);
	/** 取消或源失效后返回普通镜头的本地实时时长。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend", meta=(Units="s", ClampMin="0")) float CancelBlendSeconds = .25f;
	/** 特写期间只抑制玩法Look入口，不屏蔽移动意图或UI输入。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input") bool bSuppressLookInput = true;
	/** 镜头安全检测半径和与观察点最小距离，单位厘米。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Safety") float ProbeRadius = 8.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Safety") float MinLookAtDistance = 35.f;
	/** 最大允许俯仰范围，避免近景LookAt翻转。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Safety") FVector2D PitchLimits = FVector2D(-70.f, 70.f);
	/** 非空时允许缺失锚点回退到这个真实骨骼，否则报告错误。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Safety") FName FallbackAnchor;
	/** 校验有限数值、连续区间与曲线采样；Errors追加诊断。 */
	bool Validate(TArray<FText>& Errors) const;
	/** 骨骼缺失且没有合法备用锚点时返回false。 */
	bool ValidateMesh(const USkeletalMeshComponent* Mesh, TArray<FText>& Errors) const;
	/** 仅采样本段局部构图，NormalizedTime必须处于0～1。 */
	bool Sample(float NormalizedTime, FHodgeCinematicCameraSample& Out) const;
	/** 以实际源时间求进入／退出权重；窗口外返回零。 */
	float EvaluateWeight(float Position, float WindowStart, float WindowEnd) const;
#if WITH_EDITOR
	/** 编辑器校验不保存或改写资产。 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
