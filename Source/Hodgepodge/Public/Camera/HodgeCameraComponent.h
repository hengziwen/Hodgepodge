/** @file HodgeCameraComponent.h
 * @brief 声明相机模式与混合组件相关的类型、接口与现有处理流程。
 */
// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Camera/HodgeCameraMode.h"
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
	/** 构造相机模式与混合组件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeCameraComponent(const FObjectInitializer& ObjectInitializer);

	// 在指定 Actor 上查找 HodgeCameraComponent，不存在时返回 nullptr。
	/** 查找相机模式与混合组件。
	 * @param Actor 本次操作关联的Actor。
	 * @return 相机模式与混合组件的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Camera")
	static UHodgeCameraComponent* FindCameraComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UHodgeCameraComponent>() : nullptr);
	}

	// 获取当前相机正在跟随或观察的目标 Actor，默认就是组件所属 Actor。
	/** 获取目标Actor对象。
	 * @return 目标Actor对象的当前值或引用。
	 */
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// 用于查询当前应该使用的最佳 CameraMode。
	FHodgeCameraModeDelegate DetermineCameraModeDelegate;

	// 给当前帧的视野角额外增加偏移量，应用后下一帧会自动清除。
	/** 添加字段Of视图偏移。
	 * @param FovOffset Fov偏移，由调用方传入。
	 */
	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

	// 绘制相机相关调试信息。
	/** 按既有调试设置绘制当前状态或检测几何。
	 * @param Canvas 画布，由调用方传入。
	 */
	virtual void DrawDebug(UCanvas* Canvas) const;

	// 获取当前相机模式栈指定层的混合权重和 CameraTypeTag。
	/** 获取混合信息。
	 * @param OutWeightOfTopLayer 输出WeightOfTop层。
	 * @param OutTagOfTopLayer 标签OfTop层，用于对应标签查询或匹配。
	 */
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;
	/** 查询实际模式栈顶，避免把目标选择存在误当成锁定镜头正在生效。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Camera") TSubclassOf<UHodgeCameraMode> GetActiveCameraModeClass() const;
	/** 返回上一帧已混合输出；切换镜头以此作为连续起点。 */
	bool GetLastCameraModeView(FHodgeCameraModeView& OutView) const;
	/** 异常退出从当前视觉视图混合，但恢复方向使用游戏操作朝向。 */
	void QueueCinematicReturn(float Seconds, FRotator GameplayRotation);
	/** 仅在电影返回切换这一帧提供操作旋转种子。 */
	bool GetGameplayReturnRotation(FRotator& Out) const;
	/** 只清理对应句柄启动的电影音效和震屏。 */
	void EndCinematicPresentation(const struct FHodgeAbilityCameraHandle& Handle);

protected:
	// 注册相机组件时执行初始化。
	/** 组件注册后接入当前Owner的依赖。 */
	virtual void OnRegister() override;

	// 计算当前帧最终需要输出的相机视图。
	/** 获取相机视图。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param DesiredView 期望视图，由调用方传入。
	 */
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	// 根据当前状态更新相机模式栈。
	/** 更新相机Modes。 */
	virtual void UpdateCameraModes();

protected:
	// 相机模式栈，负责管理多个 CameraMode 以及它们之间的混合。
	UPROPERTY()
	TObjectPtr<UHodgeCameraModeStack> CameraModeStack;

	// 当前帧额外施加到视野角上的偏移量，应用后会清零。
	float FieldOfViewOffset;
	FHodgeCameraModeView LastCameraModeView;
	bool bHasCameraModeView = false;
	/** 下一次真实模式切换消费这份退出配置，不保留到其他技能。 */
	float PendingReturnSeconds = -1.f;
	FRotator PendingGameplayRotation = FRotator::ZeroRotator;
};
