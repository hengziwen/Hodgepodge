// **
//  * @file HodgeInputComponentBase.h
//  * @brief UHodgeInputComponent 类的头文件
//  *
//  * 输入组件基类,基于 Enhanced Input 系统,
//  * 提供了输入映射管理、原生输入绑定、GAS 能力输入绑定等核心功能。
//  * 是角色输入系统的统一入口,供角色基类调用。
//  */

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "HodgeInputConfig.h"
#include "HodgeInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;

/**
 * @brief Hodgepodge 框架的输入组件基类
 *
 * 继承自 UEnhancedInputComponent,基于 Enhanced Input 系统。
 * 作为角色输入处理的核心组件,提供了以下功能:
 *
 * 1. 输入映射管理:
 *    - AddInputMappings/RemoveInputMappings: 添加/移除输入映射上下文
 * 2. 原生输入绑定(BindNativeAction):
 *    - 根据 InputTag 查找 InputAction,绑定到指定的回调函数
 *    - 用于处理移动、视角等非 GAS 的原生输入
 * 3. 能力输入绑定(BindAbilityActions):
 *    - 遍历 AbilityInputActions 列表,自动绑定 InputAction 到 GAS Ability
 *    - 为每个 Ability 绑定 Pressed(激活)和 Completed(释放)两个触发事件
 * 4. 绑定清理(RemoveBinds):
 *    - 根据绑定句柄列表移除所有已注册的输入绑定
 *
 * 使用流程:
 * 1. 角色 BeginPlay 时,获取 UHodgeInputConfig 数据资产
 * 2. 调用 AddInputMappings 添加输入映射上下文
 * 3. 调用 BindNativeAction 绑定原生输入(如 Move、Look)
 * 4. 调用 BindAbilityActions 绑定 GAS 能力输入
 * 5. 角色销毁时调用 RemoveBinds 清理绑定
 */
UCLASS(Config = Input)
class HODGEPODGE_API UHodgeInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数
	 *
	 * 调用父类 UEnhancedInputComponent 构造函数,当前无自定义初始化逻辑。
	 */
	UHodgeInputComponent();

	/**
	 * @brief 将输入配置中的映射添加到 Enhanced Input 子系统
	 *
	 * 从 InputConfig 中读取输入映射上下文(Mapping Context),
	 * 添加到 EnhancedInputLocalPlayerSubsystem 中使其生效。
	 * 当前为空实现,子类可重写以添加自定义的映射添加逻辑。
	 *
	 * @param InputConfig 输入配置数据资产,提供映射上下文信息
	 * @param InputSubsystem Enhanced Input 本地玩家子系统,用于管理输入映射
	 */
	void AddInputMappings(const UHodgeInputConfig* InputConfig,
	                      UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	/**
	 * @brief 将输入配置中的映射从 Enhanced Input 子系统中移除
	 *
	 * 从 EnhancedInputLocalPlayerSubsystem 中移除 InputConfig 对应的映射上下文。
	 * 当前为空实现,子类可重写以添加自定义的映射移除逻辑。
	 *
	 * @param InputConfig 输入配置数据资产,提供映射上下文信息
	 * @param InputSubsystem Enhanced Input 本地玩家子系统,用于管理输入映射
	 */
	void RemoveInputMappings(const UHodgeInputConfig* InputConfig,
	                         UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	/**
	 * @brief 绑定单个原生输入动作到回调函数
	 *
	 * 根据指定的 InputTag 在 InputConfig 中查找对应的 InputAction,
	 * 并将其绑定到指定的回调函数。用于处理非 GAS 的原生输入(如移动、视角)。
	 *
	 * 模板参数:
	 * - UserClass: 回调函数所属的类类型
	 * - FuncType: 回调函数的签名类型
	 *
	 * @param InputConfig 输入配置数据资产,用于查找 InputAction
	 * @param InputTag 要绑定的输入 Tag(如 InputTag.Move、InputTag.Look)
	 * @param TriggerEvent 触发事件(如 Triggered、Started、Completed)
	 * @param Object 回调函数所属的对象实例指针
	 * @param Func 回调函数指针,使用 BindLambda 或 BindUObject 等方式绑定
	 * @param bLogIfNotFound 未找到 InputAction 时是否输出日志警告
	 */
	template <class UserClass, typename FuncType>
	void BindNativeAction(const UHodgeInputConfig* InputConfig, const FGameplayTag& InputTag,
	                      ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	/**
	 * @brief 批量绑定所有能力输入动作到 GAS Ability
	 *
	 * 遍历 InputConfig 中的 AbilityInputActions 列表,
	 * 将每个 InputAction 绑定到 Ability 的激活(Pressed)和释放(Released)回调。
	 * 每个绑定的句柄会被记录到 BindHandles 中,便于后续统一清理。
	 *
	 * 模板参数:
	 * - UserClass: 回调函数所属的类类型
	 * - PressedFuncType: 按下/激活回调函数的签名类型
	 * - ReleasedFuncType: 释放回调函数的签名类型
	 *
	 * @param InputConfig 输入配置数据资产,提供 AbilityInputActions 列表
	 * @param Object 回调函数所属的对象实例指针
	 * @param PressedFunc 按下/激活时的回调函数(绑定到 ETriggerEvent::Triggered)
	 * @param ReleasedFunc 释放时的回调函数(绑定到 ETriggerEvent::Completed)
	 * @param BindHandles 输出参数,存储所有绑定的句柄 ID,用于后续 RemoveBinds 清理
	 */
	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UHodgeInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
	                        ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	/**
	 * 简介: 根据句柄列表移除所有已注册的输入绑定
	 *
	 * 遍历 绑定Handles 中的每个句柄 ID,调用 Remove绑定ingByHandle 逐一移除。
	 * 最后清空 绑定Handles 数组。通常在角色销毁或输入组件失活时调用。
	 *
	 * 参数: 绑定Handles 之前通过 绑定能力Actions 记录的绑定句柄列表
	 */
	void RemoveBinds(TArray<uint32>& BindHandles);
};


template <class UserClass, typename FuncType>
void UHodgeInputComponent::BindNativeAction(const UHodgeInputConfig* InputConfig, const FGameplayTag& InputTag,
                                            ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func,
                                            bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UHodgeInputComponent::BindAbilityActions(const UHodgeInputConfig* InputConfig, UserClass* Object,
                                              PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc,
                                              TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FHodgeInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc,
				                           Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc,
				                           Action.InputTag).GetHandle());
			}
		}
	}
}
