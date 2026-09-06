// 111 屎山代码来袭

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HodgeUICameraManagerComponent.generated.h"

class AHodgePlayerCameraManager;

class AActor;
class AHUD;
class APlayerController;
class FDebugDisplayInfo;
class UCanvas;
class UObject;

// UI Camera 管理组件，限制只能作为 HodgePlayerCameraManager 的子对象存在
UCLASS(Transient, Within=HodgePlayerCameraManager)
class UHodgeUICameraManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 获取指定 PlayerController 对应的 UI Camera 管理组件
	static UHodgeUICameraManagerComponent* GetComponent(APlayerController* PC);

public:
	// 构造函数
	UHodgeUICameraManagerComponent();

	// 初始化组件
	virtual void InitializeComponent() override;

	// 是否正在更新当前 ViewTarget
	bool IsSettingViewTarget() const { return bUpdatingViewTarget; }

	// 获取当前设置的 ViewTarget
	AActor* GetViewTarget() const { return ViewTarget; }

	// 设置 UI Camera 使用的 ViewTarget，并支持视角过渡参数
	void SetViewTarget(AActor* InViewTarget,
	                   FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());

	// 判断当前是否需要更新 ViewTarget
	bool NeedsToUpdateViewTarget() const;

	// 更新 ViewTarget，并将结果写入输出视角数据
	void UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime);

	// 绘制 UI Camera 相关的调试信息
	void OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);

private:
	// 当前 UI Camera 使用的 ViewTarget
	UPROPERTY(Transient)
	TObjectPtr<AActor> ViewTarget;

	// 是否正在执行 ViewTarget 更新
	UPROPERTY(Transient)
	bool bUpdatingViewTarget;
};
