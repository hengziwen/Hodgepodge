/** @file HodgeCameraMode_Cinematic.h @brief 现有模式栈中的本地大招镜头，不拥有角色动作或服务器时序。 */
#pragma once
#include "Camera/HodgeCameraMode.h"
#include "Camera/HodgeCinematicCameraTypes.h"
#include "HodgeCameraMode_Cinematic.generated.h"
class UAudioComponent;
class UCameraShakeBase;
class APlayerCameraManager;

/** 使用精确蒙太奇位置控制运镜，操作朝向与视觉朝向分离。 */
UCLASS()
class HODGEPODGE_API UHodgeCameraMode_Cinematic : public UHodgeCameraMode
{
	GENERATED_BODY()
public:
	/** 内部进入混合从当前模式输出开始，禁用重复的外层进入时钟。 */
	UHodgeCameraMode_Cinematic();
	/** 电影窗口末段需要下层游戏镜头的实时输出。 */
	virtual bool KeepUnderlyingModes() const override;
	/** 模式退休只清理自己启动的音效和震屏实例。 */
	virtual void OnDeactivation() override;
	/** GA结束时即刻配对清理，即使UI覆盖使模式不再逐帧求值。 */
	void EndPresentation(const FHodgeAbilityCameraHandle& Handle);
protected:
	/** 角色水平空间构图，经安全检测后交给同一CameraComponent应用。 */
	virtual void UpdateView(float DeltaTime) override;
	/** 进入由视图混合处理，退出由栈权重处理，不叠加两个缓动。 */
	virtual void UpdateBlending(float DeltaTime) override;
private:
	/** 类实例缓存跨激活存在；每个新申请都重新捕获进入视图。 */
	FHodgeAbilityCameraHandle BoundHandle;
	FHodgeCameraModeView EntryView;
	/** 当前实际栈权重，源失效后保留末帧用于异常退出。 */
	float SourceBlendWeight = 1.f;
	/** 预测恢复重绑同一执行时保留已消费事件集合。 */
	FGuid EventExecution;
	TSet<FName> ConsumedEvents;
	float PreviousEventTime = 0.f;
	/** 弱引用只跟踪本地表现实例，不保留旧玩家或声源。 */
	UPROPERTY() TArray<TWeakObjectPtr<UAudioComponent>> OwnedAudio;
	UPROPERTY() TArray<TWeakObjectPtr<UCameraShakeBase>> OwnedShakes;
	UPROPERTY() TWeakObjectPtr<APlayerCameraManager> ShakeManager;
	/** 取消、死亡和模式替换配对结束本次表现资源。 */
	void ReleaseLocalEffects();
};
