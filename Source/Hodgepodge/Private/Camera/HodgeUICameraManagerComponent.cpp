// 111 屎山代码来袭

#include "Camera/HodgeUICameraManagerComponent.h"

#include "Camera/HodgePlayerCameraManager.h"
#include "GameFramework/HUD.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeUICameraManagerComponent)

class AActor;
class FDebugDisplayInfo;

// 获取 PlayerController 对应的 UI Camera 管理组件
UHodgeUICameraManagerComponent* UHodgeUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		// 获取 PlayerController 当前使用的 CameraManager，并转换为 HodgePlayerCameraManager
		if (AHodgePlayerCameraManager* PCCamera = Cast<AHodgePlayerCameraManager>(PC->PlayerCameraManager))
		{
			// 从 CameraManager 中获取 UI Camera 管理组件
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

// 构造 UI Camera 管理组件
UHodgeUICameraManagerComponent::UHodgeUICameraManagerComponent()
{
	// 允许组件参与 InitializeComponent 初始化流程
	bWantsInitializeComponent = true;

	// 类默认对象 CDO 不需要注册运行时调试回调
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// 注册 ShowDebug 调试信息回调
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

// 初始化 UI Camera 管理组件
void UHodgeUICameraManagerComponent::InitializeComponent()
{
	// 调用父类的组件初始化逻辑
	Super::InitializeComponent();
}

// 设置 UI Camera 的 ViewTarget
void UHodgeUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	// 临时标记当前正在修改 ViewTarget，离开作用域后自动恢复原状态
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	// 保存 UI Camera 当前使用的 ViewTarget
	ViewTarget = InViewTarget;

	// 将 ViewTarget 设置同步给所属的 HodgePlayerCameraManager
	CastChecked<AHodgePlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

// 判断 UI Camera 是否需要更新 ViewTarget
bool UHodgeUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	// 当前暂未实现额外的 ViewTarget 更新需求
	return false;
}

// 更新 UI Camera 的 ViewTarget 数据
void UHodgeUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
	// 当前暂未实现额外的 ViewTarget 更新逻辑
}

// 显示 UI Camera 相关的 ShowDebug 调试信息
void UHodgeUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo,
                                                     float& YL, float& YPos)
{
	// 当前暂未实现 UI Camera 调试信息显示
}
