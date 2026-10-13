/** @file HodgeLockOnTestTypes.h @brief 为模式栈测试提供可实例化的第三人称镜头，不参与正式配置。 */
#pragma once
#include "Camera/HodgeCameraMode_ThirdPerson.h"
#include "HodgeLockOnTestTypes.generated.h"

/** ThirdPerson 原生基类是 Abstract，测试不能直接创建它。 */
UCLASS(NotBlueprintable)
class UHodgeCameraMode_LockOnTestFree : public UHodgeCameraMode_ThirdPerson
{
	GENERATED_BODY()
public:
	UHodgeCameraMode_LockOnTestFree();
};
