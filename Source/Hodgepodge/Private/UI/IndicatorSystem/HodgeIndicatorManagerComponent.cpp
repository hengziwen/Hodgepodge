// Copyright Epic Games, Inc. All Rights Reserved.

// UHodgeIndicatorManagerComponent 类定义。
// 该组件负责维护当前 Controller 对应玩家的所有 IndicatorDescriptor。
#include "UI/IndicatorSystem/HodgeIndicatorManagerComponent.h"

// Indicator 描述对象定义。
// 每个 UIndicatorDescriptor 描述一个需要交给 Indicator UI 系统展示的目标。
#include "UI/IndicatorSystem/IndicatorDescriptor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeIndicatorManagerComponent)

// 构造当前 Controller 对应的 Indicator 管理组件。
UHodgeIndicatorManagerComponent::UHodgeIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 允许组件随宿主 Controller 自动完成注册，
	// 不需要外部代码手动调用 RegisterComponent。
	bAutoRegister = true;

	// 组件注册后自动进入激活状态，
	// 不需要外部代码额外调用 Activate。
	bAutoActivate = true;
}

/*static*/
// 根据指定 Controller 查找其拥有的 UHodgeIndicatorManagerComponent。
UHodgeIndicatorManagerComponent* UHodgeIndicatorManagerComponent::GetComponent(AController* Controller)
{
	// 只有传入有效 Controller 时才进行组件查找。
	if (Controller)
	{
		// 从 Controller 已拥有的组件中查找 UHodgeIndicatorManagerComponent。
		// 如果存在则返回对应组件，否则返回 nullptr。
		return Controller->FindComponentByClass<UHodgeIndicatorManagerComponent>();
	}

	// Controller 无效时无法获取对应的 IndicatorManager。
	return nullptr;
}

// 向当前 Manager 注册一个新的 IndicatorDescriptor。
void UHodgeIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	// 将当前 Manager 记录到 Descriptor 中，
	// 建立 IndicatorDescriptor -> IndicatorManagerComponent 的归属关系。
	IndicatorDescriptor->SetIndicatorManagerComponent(this);

	// 广播 Indicator 新增事件。
	// 监听该事件的 UI 系统可以根据 Descriptor 创建对应的 Indicator 表现。
	OnIndicatorAdded.Broadcast(IndicatorDescriptor);

	// 将 Descriptor 保存到当前 Manager 的 Indicator 列表中。
	Indicators.Add(IndicatorDescriptor);
}

// 从当前 Manager 中移除指定的 IndicatorDescriptor。
void UHodgeIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	// 只有 Descriptor 有效时才执行移除逻辑。
	if (IndicatorDescriptor)
	{
		// 检查当前 Descriptor 所记录的 Manager 是否就是当前组件。
		// 用于发现 Descriptor 被错误地交给其他 IndicatorManager 移除等逻辑问题。
		ensure(IndicatorDescriptor->GetIndicatorManagerComponent() == this);

		// 广播 Indicator 移除事件。
		// 监听该事件的 UI 系统可以根据 Descriptor 移除对应的 Indicator 表现。
		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);

		// 从当前 Manager 维护的 Indicator 列表中删除该 Descriptor。
		Indicators.Remove(IndicatorDescriptor);
	}
}
