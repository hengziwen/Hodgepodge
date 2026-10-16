// Copyright Epic Games, Inc. All Rights Reserved.
#include "Feedback/NumberPops/HodgeNumberPopComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeNumberPopComponent)

/** 网络载荷必须是有限的正伤害与有效世界位置。 */
bool FHodgeNumberPopRequest::IsValid() const
{
	return FMath::IsFinite(DamageAmount) && DamageAmount > 0.f && !WorldLocation.ContainsNaN();
}

/** 仅在渲染边界取整，避免小额伤害显示 0 或浮点转换溢出。 */
int32 FHodgeNumberPopRequest::GetNumberToDisplay() const
{
	return IsValid() ? int32(FMath::Clamp<int64>(FMath::RoundToInt64(FMath::Min(double(DamageAmount), double(MAX_int32))), 1, MAX_int32)) : 0;
}

/** 与 Lyra 一样不为每个数字创建 Tick 组件。 */
UHodgeNumberPopComponent::UHodgeNumberPopComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}
