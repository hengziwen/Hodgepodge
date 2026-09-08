// **
//  * @file HodgeInputConfig.h
//  * @brief UHodgeInputConfig 类及 FHodgeInputAction 结构体的头文件
//  *
//  * 输入配置数据资产,基于 Enhanced Input 系统。
//  * 定义了输入动作与 GameplayTag 的映射关系,
//  * 支持 Native 原生输入和 Ability 能力输入两种绑定模式。
//  */

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "HodgeInputConfig.generated.h"

class UInputAction;

/**
 * @brief 输入动作与 GameplayTag 的映射结构体
 *
 * 将一个 UInputAction(Enhanced Input 输入动作)与一个 FGameplayTag 绑定,
 * 用于在输入系统中通过 Tag 查找对应的 InputAction。
 *
 * 用途:
 * - NativeInputActions: 手动绑定的原生输入(如移动、视角),通过 Tag 查找后手动 BindAction
 * - AbilityInputActions: 自动绑定的能力输入,Tag 与 GAS Ability 的 InputTag 匹配时自动激活能力
 */
USTRUCT(BlueprintType)
struct FHodgeInputAction
{
	GENERATED_BODY()

public:
	/**
	 * @brief Enhanced Input 输入动作对象
	 *
	 * 在编辑器中指定一个 UInputAction 资源,
	 * 定义输入触发的条件(如按下、长按、组合键等)。
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	/**
	 * @brief 对应的 GameplayTag
	 *
	 * 用于在输入绑定查找时进行匹配,
	 * 必须属于 "InputTag" 分类(Categories = "InputTag")。
	 * 例如:InputTag.Ability.Attack、InputTag.Move。
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * @brief Hodgepodge 框架的输入配置数据资产
 *
 * 继承自 UDataAsset,以数据驱动的方式配置输入动作与 GameplayTag 的映射关系。
 * 通过两个输入动作列表分别管理:
 * - NativeInputActions: 原生输入动作列表,需要通过 BindNativeAction 手动绑定
 * - AbilityInputActions: 能力输入动作列表,通过 BindAbilityActions 自动绑定到 GAS Ability
 *
 * 使用方式:
 * 1. 在编辑器中创建 UHodgeInputConfig 数据资产
 * 2. 在 NativeInputActions / AbilityInputActions 中添加 InputAction 与 InputTag 的映射
 * 3. 在角色的输入组件中使用此配置进行输入绑定
 *
 * 数据资产位置:/Game/Input/ 目录下
 */
UCLASS(BlueprintType, Blueprintable, Const)
class HODGEPODGE_API UHodgeInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数
	 *
	 * 调用父类 UDataAsset 构造函数,当前无自定义逻辑。
	 */
	UHodgeInputConfig(const FObjectInitializer& ObjectInitializer);

	/**
	 * @brief 根据 InputTag 查找原生输入动作
	 *
	 * 在 NativeInputActions 中查找与指定 InputTag 匹配的 InputAction。
	 * 用于原生输入的手动绑定(如移动、视角控制等)。
	 *
	 * @param InputTag 要查找的 GameplayTag,必须属于 InputTag 分类
	 * @param bLogNotFound 查找失败时是否输出错误日志,默认为 true
	 * @return 找到的 UInputAction 指针,未找到则返回 nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	/**
	 * @brief 根据 InputTag 查找能力输入动作
	 *
	 * 在 AbilityInputActions 中查找与指定 InputTag 匹配的 InputAction。
	 * 用于 GAS Ability 的自动绑定,Tag 必须与 Ability 上配置的 InputTag 一致。
	 *
	 * @param InputTag 要查找的 GameplayTag,必须属于 InputTag 分类
	 * @param bLogNotFound 查找失败时是否输出错误日志,默认为 true
	 * @return 找到的 UInputAction 指针,未找到则返回 nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	/**
	 * @brief 原生输入动作列表
	 *
	 * 存放需要手动绑定的输入动作(如移动、跳跃、视角等)。
	 * 这些动作不会自动绑定,需要通过 BindNativeAction 手动绑定到回调函数。
	 * 在编辑器默认值面板中配置。
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FHodgeInputAction> NativeInputActions;

	/**
	 * 简介: 能力输入动作列表
	 *
	 * 存放与 GAS 能力 关联的输入动作。
	 * 通过 绑定能力Actions 自动将 输入动作 绑定到 能力 的输入激活/释放回调。
	 * 输入Tag 必须与 能力 上配置的 输入Tag 一致才能正确匹配。
	 * 在编辑器默认值面板中配置。
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FHodgeInputAction> AbilityInputActions;
};
