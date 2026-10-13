/** @file HodgePlayerCameraManager.h
 * @brief 声明玩家相机管理器相关的类型、接口与现有处理流程。
 */
// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "HodgePlayerCameraManager.generated.h"


class FDebugDisplayInfo;
class UCanvas;
class UObject;


#define HODGE_CAMERA_DEFAULT_FOV          (80.0f)   // Hodge 默认相机视野角。
#define HODGE_CAMERA_DEFAULT_PITCH_MIN  (-89.0f)   // Hodge 默认相机最小俯仰角。
#define HODGE_CAMERA_DEFAULT_PITCH_MAX  (89.0f)    // Hodge 默认相机最大俯仰角。

class UHodgeUICameraManagerComponent;

/**
 * AHodgePlayerCameraManager
 *
 * 项目使用的基础 PlayerCameraManager，负责管理玩家最终的相机视图。
 */
UCLASS(notplaceable, MinimalAPI)
class AHodgePlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	/** 构造玩家相机管理器并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	AHodgePlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	// 获取 UI 相机管理组件，用于处理需要暂时覆盖游戏相机的 UI 相机逻辑。
	/** 获取界面相机组件。
	 * @return 界面相机组件的当前值或引用。
	 */
	UHodgeUICameraManagerComponent* GetUICameraComponent() const;

protected:
	// 更新当前 ViewTarget，在这里可以让 UI 相机逻辑参与最终视图计算。
	/** 更新视图目标。
	 * @param OutVT 输出VT。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	// 绘制 PlayerCameraManager 相关的调试信息。
	/** 输出当前对象的调试状态。
	 * @param Canvas 画布，由调用方传入。
	 * @param DebugDisplay 调试显示，由调用方传入。
	 * @param YL YL，由调用方传入。
	 * @param YPos YPos，由调用方传入。
	 */
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	// UI 相机组件，在 UI 需要接管相机时可以覆盖正常的游戏相机。
	UPROPERTY(Transient)
	TObjectPtr<UHodgeUICameraManagerComponent> UICamera;
};
