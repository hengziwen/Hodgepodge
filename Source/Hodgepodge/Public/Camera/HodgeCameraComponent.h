// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "HodgeCameraComponent.generated.h"


class UCanvas;
class UHodgeCameraMode;
class UHodgeCameraModeStack;
class UObject;
struct FFrame;
struct FGameplayTag;
struct FMinimalViewInfo;
template <class TClass>
class TSubclassOf;

// 用于查询当前应该使用哪个 CameraMode 的委托。
DECLARE_DELEGATE_RetVal(TSubclassOf<UHodgeCameraMode>, FHodgeCameraModeDelegate);


/**
 * UHodgeCameraComponent
 *
 * 项目使用的基础相机组件，负责管理相机模式栈并输出最终相机视图。
 */
UCLASS()
class UHodgeCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UHodgeCameraComponent(const FObjectInitializer& ObjectInitializer);

	// 在指定 Actor 上查找 HodgeCameraComponent，不存在时返回 nullptr。
	UFUNCTION(BlueprintPure, Category = "Hodge|Camera")
	static UHodgeCameraComponent* FindCameraComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UHodgeCameraComponent>() : nullptr);
	}

	// 获取当前相机正在跟随或观察的目标 Actor，默认就是组件所属 Actor。
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// 用于查询当前应该使用的最佳 CameraMode。
	FHodgeCameraModeDelegate DetermineCameraModeDelegate;

	// 给当前帧的视野角额外增加偏移量，应用后下一帧会自动清除。
	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

	// 绘制相机相关调试信息。
	virtual void DrawDebug(UCanvas* Canvas) const;

	// 获取当前相机模式栈指定层的混合权重和 CameraTypeTag。
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:
	// 注册相机组件时执行初始化。
	virtual void OnRegister() override;

	// 计算当前帧最终需要输出的相机视图。
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	// 根据当前状态更新相机模式栈。
	virtual void UpdateCameraModes();

protected:
	// 相机模式栈，负责管理多个 CameraMode 以及它们之间的混合。
	UPROPERTY()
	TObjectPtr<UHodgeCameraModeStack> CameraModeStack;

	// 当前帧额外施加到视野角上的偏移量，应用后会清零。
	float FieldOfViewOffset;
};
