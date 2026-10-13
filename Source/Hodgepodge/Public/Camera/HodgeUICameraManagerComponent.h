/** @file HodgeUICameraManagerComponent.h
 * @brief 声明UI相机协调组件相关的类型、接口与现有处理流程。
 */
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
	/** 获取组件。
	 * @param PC 玩家控制器对象。
	 * @return 组件的当前值或引用。
	 */
	static UHodgeUICameraManagerComponent* GetComponent(APlayerController* PC);

public:
	// 构造函数
	/** 构造UI相机协调组件并初始化其默认状态。 */
	UHodgeUICameraManagerComponent();

	// 初始化组件
	/** 初始化组件。 */
	virtual void InitializeComponent() override;

	// 是否正在更新当前 ViewTarget
	/** 判断Setting视图目标。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsSettingViewTarget() const { return bUpdatingViewTarget; }

	// 获取当前设置的 ViewTarget
	/** 获取视图目标。
	 * @return 视图目标的当前值或引用。
	 */
	AActor* GetViewTarget() const { return ViewTarget; }

	// 设置 UI Camera 使用的 ViewTarget，并支持视角过渡参数
	/** 设置视图目标。
	 * @param InViewTarget 视图目标，由调用方传入。
	 * @param TransitionParams 转换参数，由调用方传入。
	 */
	void SetViewTarget(AActor* InViewTarget,
	                   FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());

	// 判断当前是否需要更新 ViewTarget
	/** 检查当前相机观察目标是否需要更新。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool NeedsToUpdateViewTarget() const;

	// 更新 ViewTarget，并将结果写入输出视角数据
	/** 更新视图目标。
	 * @param OutVT 输出VT。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	void UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime);

	// 绘制 UI Camera 相关的调试信息
	/** 处理Show调试信息。
	 * @param HUD HUD界面，由调用方传入。
	 * @param Canvas 画布，由调用方传入。
	 * @param DisplayInfo 显示信息，由调用方传入。
	 * @param YL YL，由调用方传入。
	 * @param YPos YPos，由调用方传入。
	 */
	void OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);

private:
	// 当前 UI Camera 使用的 ViewTarget
	UPROPERTY(Transient)
	TObjectPtr<AActor> ViewTarget;

	// 是否正在执行 ViewTarget 更新
	UPROPERTY(Transient)
	bool bUpdatingViewTarget;
};
