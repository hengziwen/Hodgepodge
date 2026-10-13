/** @file HodgeCameraMode_LockOn.h @brief 复用第三人称距离、FOV 与避障的锁定取景。 */
#pragma once
#include "Camera/HodgeCameraMode_ThirdPerson.h"
#include "HodgeCameraMode_LockOn.generated.h"
/** 只生成实际镜头，不承担目标选择和控制器旋转写入。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeCameraMode_LockOn : public UHodgeCameraMode_ThirdPerson
{
	GENERATED_BODY()
public:
	virtual void OnBecomeTop() override;
protected:
	virtual void UpdateView(float DeltaTime) override;
	virtual FRotator GetPivotRotation() const override;
private:
	// 与角色控制朝向分离的平滑视图。
	FRotator LookRotation = FRotator::ZeroRotator;
	// 确认回包不会重复获取；技能模式返回只继承实际视角。
	TWeakObjectPtr<AActor> CameraTarget;
	FVector PreviousPivot = FVector::ZeroVector;
};
