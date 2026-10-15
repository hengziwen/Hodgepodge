/** @file HodgeCameraMode_LockOn.cpp @brief 与控制朝向分离的锁定构图与既有避障。 */
#include "Camera/HodgeCameraMode_LockOn.h"
#include "Camera/HodgeCameraComponent.h"
#include "Component/HodgeLockOnComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Data/HodgePawnData.h"
#include "Data/HodgeLockOnProfile.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "SceneView.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCameraMode_LockOn)
void UHodgeCameraMode_LockOn::OnBecomeTop()
{
	Super::OnBecomeTop(); LookRotation = Super::GetPivotRotation();
	FHodgeCameraModeView PreviousView;
	if (GetHodgeCameraComponent()->GetLastCameraModeView(PreviousView)) { LookRotation = PreviousView.Rotation; }
	// 电影正面特写不能作为锁定恢复方向，只有对应退出切换使用游戏旋转种子。
	GetHodgeCameraComponent()->GetGameplayReturnRotation(LookRotation);
	const auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(GetTargetActor());
	const auto* Data = Extension ? Extension->GetPawnData<UHodgePawnData>() : nullptr;
	if (Data && Data->DefaultCameraMode && Data->DefaultCameraMode->IsChildOf(UHodgeCameraMode_ThirdPerson::StaticClass()))
	{ CopyViewSettingsFrom(Cast<UHodgeCameraMode_ThirdPerson>(Data->DefaultCameraMode->GetDefaultObject())); }
	if (auto* Lock = GetTargetActor()->FindComponentByClass<UHodgeLockOnComponent>(); Lock && Lock->GetProfile())
	{
		BlendTime = Lock->GetProfile()->CameraBlendTime;
		Lock->SeedCameraView(LookRotation);
		if (CameraTarget.Get() != Lock->GetPresentationTarget())
		{ CameraTarget = Lock->GetPresentationTarget(); Lock->BeginCameraAcquire(); }
	}
	PreviousPivot = GetPivotLocation();
}
FRotator UHodgeCameraMode_LockOn::GetPivotRotation() const { return LookRotation; }
void UHodgeCameraMode_LockOn::UpdateView(float Delta)
{
	auto* Lock = GetTargetActor() ? GetTargetActor()->FindComponentByClass<UHodgeLockOnComponent>() : nullptr;
	if (Lock && Lock->GetPresentationTarget() && Lock->GetProfile())
	{
		const auto* Profile = Lock->GetProfile();
		const bool bTop = GetHodgeCameraComponent()->GetActiveCameraModeClass() == GetClass();
		if (bTop && CameraTarget.Get() != Lock->GetPresentationTarget())
		{ CameraTarget = Lock->GetPresentationTarget(); Lock->BeginCameraAcquire(); }
		FHodgeCameraModeView ActualView;
		if (GetHodgeCameraComponent()->GetLastCameraModeView(ActualView))
		{
			FHodgeLockCameraFrame Frame; Frame.PivotLocation = GetPivotLocation();
			// 平移到本帧角色位置后再投影，避免奔跑时把上一帧位移误作镜头轨道偏移。
			Frame.CameraLocation = ActualView.Location + Frame.PivotLocation - PreviousPivot;
			Frame.ViewRotation = ActualView.Rotation; Frame.HorizontalFOV = ActualView.FieldOfView;
			Frame.AspectRatio = GetHodgeCameraComponent()->AspectRatio;
			const auto* Pawn = Cast<APawn>(GetTargetActor());
			const auto* PC = Pawn ? Pawn->GetController<APlayerController>() : nullptr;
			int32 Width=0, Height=0; if (PC) { PC->GetViewportSize(Width,Height); }
			if (!GetHodgeCameraComponent()->bConstrainAspectRatio && Width>0 && Height>0) { Frame.AspectRatio = static_cast<float>(Width)/Height; }
			const auto* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
			FSceneViewProjectionData Projection;
			// 读取实际投影，兼容 MaintainYFOV、窗口比例和分屏视口。
			if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport,Projection))
			{
				const double X=Projection.ProjectionMatrix.M[0][0], Y=Projection.ProjectionMatrix.M[1][1];
				if (X>0. && Y>0.) { Frame.HorizontalFOV=FMath::RadiansToDegrees(2.*FMath::Atan(1./X)); Frame.AspectRatio=Y/X; }
			}
			if (Lock->GetPresentationLockPoint(Frame.TargetLocation)) { LookRotation = Lock->UpdateCameraView(Frame, Delta, bTop); }
			PreviousPivot = Frame.PivotLocation;
		}
		ViewPitchMin = Profile->PitchLimits.X; ViewPitchMax = Profile->PitchLimits.Y;
	}
	// 控制旋转已经表达实际视角，退出层继续跟随本帧输入，不削弱鼠标响应。
	else { LookRotation = Super::GetPivotRotation(); CameraTarget.Reset(); PreviousPivot = GetPivotLocation(); }
	Super::UpdateView(Delta);
}
