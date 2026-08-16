// **
//  * @file HodgePlayerStateBase.h
//  * @brief AHodgePlayerStateBase 类的头文件
//  *
//  * 玩家状态基类,持有 GAS AbilitySystemComponent,
//  * 负责能力、属性、效果的管理,以及 GAS 的双初始化流程。
//  */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "HodgePlayerStateBase.generated.h"

class UHodgeAbilitySystemComponentBase;
class UAbilitySystemComponent;
class UAttributeSet;

/**
 * @brief Hodgepodge 框架的玩家状态基类
 *
 * 继承自 APlayerState 和 IAbilitySystemInterface,核心职责是:
 * 1. 持有并管理 AbilitySystemComponent(GAS 能力系统组件),
 *    ASC 作为玩家的能力、属性、效果的容器,跟随 PlayerState 的生命周期;
 * 2. 在服务器端为玩家动态创建 AttributeSet(属性集),
 *    将 AttributeSetClasses 配置的属性类型实例化并添加到 ASC 中;
 * 3. 通过 InitializeAbilitySystemForCharacter 实现 GAS 的双初始化流程:
 *    - 服务器端:PossessedBy() 时调用,创建 AttributeSet 并绑定 Avatar
 *    - 客户端:OnRep_PlayerState() 时调用,绑定 Avatar(AttributeSet 从服务器复制)
 *
 * 数据归属:
 * - 长期数据(等级、金币、装备、技能、属性等)存放在 PlayerState
 * - 临时战斗状态(位置、动画、移动等)存放在 Character(Avatar)
 *
 * 网络设置:
 * - NetUpdateFrequency = 100.f: 提高更新频率以确保 GAS 属性同步及时
 * - ASC 设置为 Mixed 复制模式,兼顾服务器权威和客户端预测
 */
UCLASS()
class HODGEPODGE_API AHodgePlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数
	 *
	 * 创建 AbilitySystemComponent 子对象,
	 * 设置 NetUpdateFrequency 为 100Hz 以确保 GAS 属性的高频同步。
	 */
	AHodgePlayerStateBase();

	/**
	 * @brief GAS 能力系统组件
	 *
	 * 作为 PlayerState 的子对象创建,跟随 PlayerState 生命周期,
	 * 管理玩家的所有能力(GameplayAbility)、属性(AttributeSet)和效果(GameplayEffect)。
	 * 通过 IAbilitySystemInterface 接口暴露给 GAS 系统使用。
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UHodgeAbilitySystemComponentBase* AbilitySystemComponent;

	/**
	 * @brief 获取能力系统组件(实现 IAbilitySystemInterface 接口)
	 * @return AbilitySystemComponent 指针
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/**
	 * @brief 为指定角色初始化 GAS 能力系统
	 *
	 * 这是 GAS 初始化的核心函数,负责:
	 * 1. 调用 InitAbilityActorInfo 绑定 ASC 与 Avatar(Character)
	 * 2. 在服务器端创建 AttributeSet(仅首次,通过 bASCInitialized 守卫)
	 *
	 * 调用时机:
	 * - 服务器端:AHodgeHeroCharacter::PossessedBy() 中调用
	 * - 客户端:AHodgeHeroCharacter::OnRep_PlayerState() 中调用
	 *
	 * @param Avatar 玩家当前控制的角色(ACharacter 子类)
	 */
	void InitializeAbilitySystemForCharacter(class ACharacter* Avatar);

	/**
	 * @brief 需要创建的属性集类型列表
	 *
	 * 在服务器端 InitializeAbilitySystemForCharacter 时,
	 * 会遍历此列表动态创建对应的 AttributeSet 并添加到 ASC 中。
	 * 在编辑器默认值面板中配置,可根据不同子类(如战士/法师)设置不同的属性集。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UAttributeSet>> AttributeSetClasses;

	/**
	 * @brief 运行时已创建的属性集实例列表
	 *
	 * 仅在服务器端填充,存储通过 AttributeSetClasses 动态创建的 AttributeSet 实例。
	 * 标记为 UPROPERTY() 以防止垃圾回收。
	 */
	UPROPERTY()
	TArray<UAttributeSet*> AddedAttributeSets;

private:
	/**
	 * 简介: ASC 是否已完成初始化
	 *
	 * 防止服务器端重复创建 属性集。
	 * 使用独立 bool 而非 Added属性集s.Num() == 0 判断,
	 * 因为死亡重生/重新 Possess 时数组可能为空但 ASC 已初始化过。
	 */
	bool bASCInitialized = false;
};