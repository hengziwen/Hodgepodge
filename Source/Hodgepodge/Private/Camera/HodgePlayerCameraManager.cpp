// 111屎山代码来袭


#include "Camera/HodgePlayerCameraManager.h"

#include "Camera/HodgeCameraComponent.h"
#include "Camera/HodgeUICameraManagerComponent.h"
#include "Engine/Canvas.h"

class FDebugDisplayInfo;

// UI 相机组件使用的默认对象名称。
static FName UICameraComponentName(TEXT("UICamera"));

AHodgePlayerCameraManager::AHodgePlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 设置默认相机 FOV。
	DefaultFOV = HODGE_CAMERA_DEFAULT_FOV;

	// 设置相机默认最小俯仰角。
	ViewPitchMin = HODGE_CAMERA_DEFAULT_PITCH_MIN;

	// 设置相机默认最大俯仰角。
	ViewPitchMax = HODGE_CAMERA_DEFAULT_PITCH_MAX;

	// 创建 UI 相机管理组件。
	UICamera = CreateDefaultSubobject<UHodgeUICameraManagerComponent>(UICameraComponentName);
}

UHodgeUICameraManagerComponent* AHodgePlayerCameraManager::GetUICameraComponent() const
{
	// 返回 UI 相机管理组件。
	return UICamera;
}

void AHodgePlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// 如果 UI 相机当前需要更新 ViewTarget，则让 UI 相机获得更高优先级。
	if (UICamera->NeedsToUpdateViewTarget())
	{
		// 先让父类正常计算 Gameplay Camera 的 ViewTarget。
		Super::UpdateViewTarget(OutVT, DeltaTime);

		// 再让 UI 相机修改最终的 ViewTarget。
		UICamera->UpdateViewTarget(OutVT, DeltaTime);

		// UI 相机已经处理完最终 ViewTarget，直接结束。
		return;
	}

	// 没有 UI 相机接管时，使用父类的正常相机逻辑。
	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void AHodgePlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL,
                                             float& YPos)
{
	// 调试绘制必须提供有效的 Canvas。
	check(Canvas);

	// 获取 Canvas 的调试绘制管理器。
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	// 使用较小字体绘制调试信息。
	DisplayDebugManager.SetFont(GEngine->GetSmallFont());

	// 设置当前 CameraManager 标题使用黄色显示。
	DisplayDebugManager.SetDrawColor(FColor::Yellow);

	// 显示当前 PlayerCameraManager 的名称。
	DisplayDebugManager.DrawString(FString::Printf(TEXT("HodgePlayerCameraManager: %s"), *GetNameSafe(this)));

	// 先绘制父类 PlayerCameraManager 的调试信息。
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	// 获取当前 PlayerController 控制的 Pawn。
	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	// 尝试从当前 Pawn 上获取 HodgeCameraComponent。
	if (const UHodgeCameraComponent* CameraComponent = UHodgeCameraComponent::FindCameraComponent(Pawn))
	{
		// 绘制 HodgeCameraComponent 以及 CameraModeStack 的调试信息。
		CameraComponent->DrawDebug(Canvas);
	}
}
