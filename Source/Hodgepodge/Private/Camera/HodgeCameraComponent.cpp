/** @file HodgeCameraComponent.cpp
 * @brief 实现相机模式与混合组件相关的类型、接口与现有处理流程。
 */
// 111屎山代码来袭


#include "Camera/HodgeCameraComponent.h"

#include "Camera/HodgeCameraMode.h"
#include "Engine/Canvas.h"
#include "Component/HodgeHeroComponent.h"
#include "Data/HodgeCinematicCameraProfile.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/HodgeCameraMode_Cinematic.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCameraComponent)


/** 构造相机模式与混合组件并初始化其默认状态。 */
UHodgeCameraComponent::UHodgeCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 相机模式栈初始为空，在 OnRegister 中创建。
	CameraModeStack = nullptr;

	// 当前帧额外的 FOV 偏移量默认为0。
	FieldOfViewOffset = 0.0f;
}

/** 组件注册后接入当前Owner的依赖。 */
void UHodgeCameraComponent::OnRegister()
{
	Super::OnRegister();

	// 第一次注册组件时创建相机模式栈。
	if (!CameraModeStack)
	{
		CameraModeStack = NewObject<UHodgeCameraModeStack>(this);

		// 确保相机模式栈创建成功。
		check(CameraModeStack);
	}
}

/** 获取相机视图。 */
void UHodgeCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	// 相机模式栈必须存在。
	check(CameraModeStack);
	// 求值前查询实际蒙太奇窗口，跳段或丢失通知也能配对恢复。
	if (auto* Hero = UHodgeHeroComponent::FindHeroComponent(GetTargetActor())) { Hero->RefreshCinematicCamera(); }

	// 根据当前游戏状态决定并更新需要使用的 CameraMode。
	UpdateCameraModes();

	// 保存相机模式栈计算出来的最终相机视图。
	FHodgeCameraModeView CameraModeView;

	// 更新并混合相机模式栈，得到最终相机位置、旋转、控制旋转和 FOV。
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	// 让 PlayerController 与最新的相机视角保持同步。
	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		// 获取目标 Pawn 对应的玩家控制器。
		if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
		{
			// 将相机模式计算出的控制旋转同步给 PlayerController。
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	// 应用当前帧额外增加的 FOV 偏移量。
	CameraModeView.FieldOfView += FieldOfViewOffset;
	LastCameraModeView = CameraModeView; bHasCameraModeView = true;

	// FOV 偏移只生效一帧，应用后立即清零。
	FieldOfViewOffset = 0.0f;

	// 将相机模式计算出的最终视图同步到 CameraComponent。
	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);

	// 更新 CameraComponent 自身的 FOV。
	FieldOfView = CameraModeView.FieldOfView;

	// 将相机模式计算出的结果填充到 UE 最终需要的 FMinimalViewInfo。
	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;

	// 保留正交相机相关参数。
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;

	// 保留相机宽高比设置。
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;

	// 保留是否使用 FOV 计算 LOD 的设置。
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;

	// 保留当前相机投影模式。
	DesiredView.ProjectionMode = ProjectionMode;

	// 检查 CameraActor 是否需要覆盖后处理设置。
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;

	// 后处理混合权重大于0时才使用自定义后处理设置。
	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}
	// PCM每帧清理缓存；电影退出后不再提交，保留场景和其他相机的后处理。
	if (const auto* Hero=UHodgeHeroComponent::FindHeroComponent(GetTargetActor()))
	{
		const auto State=Hero->GetCinematicCameraState(); const auto* Profile=Hero->GetCinematicProfile();
		const auto* Pawn=Cast<APawn>(GetTargetActor()); const auto* PC=Pawn ? Pawn->GetController<APlayerController>() : nullptr;
		if (State.bActive && Profile && Profile->PostProcessWeight>0.f && PC && PC->IsLocalController() && PC->PlayerCameraManager && UHodgeUIManagerSubsystem::AllowsGameplayInput(PC))
		{ FPostProcessSettings Settings=Profile->PostProcess; PC->PlayerCameraManager->AddCachedPPBlend(Settings,Profile->PostProcessWeight*State.Weight,VTBlendOrder_Override); }
	}


	// XR 头戴设备相机需要使用父类额外处理。
	if (IsXRHeadTrackedCamera())
	{
		// XR 模式下前面的部分相机逻辑可能不适用，但后处理设置仍然需要保留。
		Super::GetCameraView(DeltaTime, DesiredView);
	}
}

TSubclassOf<UHodgeCameraMode> UHodgeCameraComponent::GetActiveCameraModeClass() const
{
	return CameraModeStack ? CameraModeStack->GetTopCameraModeClass() : nullptr;
}
bool UHodgeCameraComponent::GetLastCameraModeView(FHodgeCameraModeView& OutView) const
{
	if (!bHasCameraModeView) { return false; }
	OutView = LastCameraModeView; return true;
}

void UHodgeCameraComponent::QueueCinematicReturn(float Seconds, FRotator GameplayRotation)
{
	PendingReturnSeconds = FMath::Max(0.f, Seconds);
	PendingGameplayRotation = GameplayRotation;
}

bool UHodgeCameraComponent::GetGameplayReturnRotation(FRotator& Out) const
{
	if (PendingReturnSeconds < 0.f) { return false; }
	Out = PendingGameplayRotation;
	return true;
}

void UHodgeCameraComponent::EndCinematicPresentation(const FHodgeAbilityCameraHandle& Handle)
{
	if (CameraModeStack)
	{
		if (auto* Mode=Cast<UHodgeCameraMode_Cinematic>(CameraModeStack->FindExistingMode(UHodgeCameraMode_Cinematic::StaticClass()))) { Mode->EndPresentation(Handle); }
	}
}

/** 更新相机Modes。 */
void UHodgeCameraComponent::UpdateCameraModes()
{
	// 相机模式栈必须存在。
	check(CameraModeStack);

	// 只有相机模式栈处于激活状态时才更新 CameraMode。
	if (CameraModeStack->IsStackActivate())
	{
		// 检查是否绑定了 CameraMode 查询委托。
		if (DetermineCameraModeDelegate.IsBound())
		{
			// 通过委托查询当前应该使用的 CameraMode 类。
			if (const TSubclassOf<UHodgeCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
			{
				// 将查询到的 CameraMode 推入相机模式栈。
				CameraModeStack->PushCameraMode(CameraMode, PendingReturnSeconds);
				PendingReturnSeconds = -1.f;
			}
		}
	}
}

/** 按既有调试设置绘制当前状态或检测几何。 */
void UHodgeCameraComponent::DrawDebug(UCanvas* Canvas) const
{
	// 调试绘制必须提供有效的 Canvas。
	check(Canvas);

	// 获取 Canvas 的调试绘制管理器。
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	// 设置调试文本使用较小字体。
	DisplayDebugManager.SetFont(GEngine->GetSmallFont());

	// 设置相机标题使用黄色显示。
	DisplayDebugManager.SetDrawColor(FColor::Yellow);

	// 显示当前相机跟随的目标 Actor。
	DisplayDebugManager.DrawString(FString::Printf(TEXT("HodgeCameraComponent: %s"), *GetNameSafe(GetTargetActor())));

	// 后续相机数据使用白色显示。
	DisplayDebugManager.SetDrawColor(FColor::White);

	// 显示当前相机世界位置。
	DisplayDebugManager.DrawString(FString::Printf(TEXT("   Location: %s"), *GetComponentLocation().ToCompactString()));

	// 显示当前相机世界旋转。
	DisplayDebugManager.DrawString(FString::Printf(TEXT("   Rotation: %s"), *GetComponentRotation().ToCompactString()));

	// 显示当前相机 FOV。
	DisplayDebugManager.DrawString(FString::Printf(TEXT("   FOV: %f"), FieldOfView));

	// 相机模式栈必须存在。
	check(CameraModeStack);

	// 绘制相机模式栈内部的调试信息。
	CameraModeStack->DrawDebug(Canvas);
}

/** 获取混合信息。 */
void UHodgeCameraComponent::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const
{
	// 相机模式栈必须存在。
	check(CameraModeStack);

	// 从相机模式栈获取当前层的混合权重和 CameraTypeTag。
	CameraModeStack->GetBlendInfo(/*out*/ OutWeightOfTopLayer, /*out*/ OutTagOfTopLayer);
}
