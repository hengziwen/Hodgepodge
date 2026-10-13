/** @file HodgeLockOnProfile.h @brief 玩家锁定的资格、输入和镜头调参。 */
#pragma once
#include "Engine/DataAsset.h"
#include "HodgeLockOnProfile.generated.h"
class UHodgeCameraMode;
class UUserWidget;
/** PawnData 显式引用的锁定配置；没有配置时不暗中启用。 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeLockOnProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	UHodgeLockOnProfile();
	// 获取与保持范围采用厘米，并通过不同半径形成滞回。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="1",Units="cm")) float AcquireDistance = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="1",Units="cm")) float KeepDistance = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="1",ClampMax="89",Units="deg")) float AcquireHalfAngle = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="0.01",Units="s")) float KeepCheckInterval = .1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="0",Units="s")) float DistanceGrace = .35f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="0",Units="s")) float OcclusionGrace = 1.f;
	// 评分权重顺序为屏幕中心、距离、目标优先级。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target") FVector ScoreWeights = FVector(.55,.30,.15);
	// 查询通道不使用模型 Overlap，目标仍须实现资格接口。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target") TEnumAsByte<ECollisionChannel> SightChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="0.01",Units="s")) float GestureWindow = .12f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="1",Units="deg")) float SwitchAngle = 18.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="1",Units="deg/s")) float SwitchSpeed = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="0",ClampMax="1")) float DirectionConsistency = .8f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="0",Units="s")) float SwitchDebounce = .25f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="0.01",Units="s")) float RearmWindow = .05f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="0",Units="deg")) float RearmAngle = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Network", meta=(ClampMin="0.1",Units="s")) float CommandTimeout = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Network", meta=(ClampMin="0.1",Units="s")) float ReferenceTimeout = 1.f;
	// 保留旧资产字段；软镜头不再用此值限制水平环绕。
	UPROPERTY(meta=(DeprecatedProperty,DeprecationMessage="软锁定使用自由环绕与 CameraScreenDeadZone")) float FramingYawLimit = 8.f;
	// 手动输入优先，停手后只在目标离开构图容忍区时柔和修正。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera") bool bEnableSoftCameraAssist = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input") bool bEnableMouseFlickSwitch = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="0",Units="s")) float CameraManualHold = .15f;
	UPROPERTY(meta=(DeprecatedProperty,DeprecationMessage="使用 CameraScreenDeadZone")) float CameraYawDeadZone = 20.f;
	UPROPERTY(meta=(DeprecatedProperty,DeprecationMessage="使用 CameraScreenDeadZone")) float CameraPitchDeadZone = 12.f;
	UPROPERTY(meta=(DeprecatedProperty,DeprecationMessage="使用 CameraYawStrength 和 CameraPitchStrength")) float CameraRecenterSharpness = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera") FVector2D PitchLimits = FVector2D(-55.f,55.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="1",Units="deg/s")) float CameraYawSpeed = 120.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="1",Units="deg/s")) float CameraPitchSpeed = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="0",Units="s")) float CameraBlendTime = .2f;
	// 获取阶段使用平滑起停曲线；角速度上限保护大角度和低帧率场景。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Acquire", meta=(ClampMin="0.05",Units="s")) float CameraAcquireDuration = .35f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Acquire", meta=(ClampMin="1",Units="deg/s")) float CameraAcquireSpeed = 720.f;
	// 屏幕坐标中心为零，边缘为 ±1；两项分别表示水平和垂直半宽。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Tracking") FVector2D CameraScreenDeadZone = FVector2D(.12,.16);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Tracking") FVector2D CameraScreenSoftZone = FVector2D(.6,.65);
	// 独立轴响应让俯仰保持较轻的跟随，不跟随每次身体高度起伏。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Tracking", meta=(ClampMin="0")) float CameraYawStrength = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Tracking", meta=(ClampMin="0")) float CameraPitchStrength = 2.f;
	// 抑制系数为一时，玩家输入与停手宽限内完全关闭辅助。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Input", meta=(ClampMin="0",ClampMax="1")) float CameraMouseSuppression = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Input", meta=(ClampMin="0.01",Units="s")) float CameraAssistRecoveryTime = .35f;
	// 目标进入角色水平近距时退出获取，并渐弱跟随，避免重叠方向翻转。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Safety", meta=(ClampMin="1",Units="cm")) float CameraMinTargetDistance = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera") TSubclassOf<UHodgeCameraMode> CameraMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI") TSoftClassPtr<UUserWidget> IndicatorClass;
	/** 校验有限值、正阈值及资源契约；失败时提供可定位诊断。 */
	bool Validate(FString& Error) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
