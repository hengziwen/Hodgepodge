// **
//  * @file HodgeCombatCharacter.h
//  * @brief AHodgeCombatCharacter 类的头文件
//  *
//  * 战斗角色基类,CharacterBase 和 HeroCharacter 之间的中间层,
//  * 预留给战斗相关的通用逻辑。
//  */

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/HodgeAbilitySystemComponentBase.h"
#include "Character/HodgeCharacterBase.h"
#include "HodgeCombatCharacter.generated.h"

/**
 * @brief Hodgepodge 框架的战斗角色基类
 *
 * 继承自 AHodgeCharacterBase,是在 CharacterBase 和 HeroCharacter 之间的中间层。
 * 预留给战斗相关的通用逻辑使用,如战斗组件、攻击判定等。
 *
 * 继承层次:
 * AHodgeCharacterBase → AHodgeCombatCharacter → AHodgeHeroCharacter
 *
 * 使用场景:
 * - 如果需要多种战斗角色(如近战英雄、远程英雄),可以在此类中放置
 *   共用的战斗逻辑,避免在 HeroCharacter 中重复代码。
 * - 敌人 AI 角色也可以继承此类获得战斗能力,但不会包含 GAS 初始化逻辑。
 */
UCLASS()
class HODGEPODGE_API AHodgeCombatCharacter : public AHodgeCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/**
	 * 简介: 构造函数
	 *
	 * 将 FObject初始化r 传递给父类 AHodge角色Base,
	 * 保持整条继承链的 FObject初始化r 传递。
	 */

	explicit AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer);

	//~Begin IAbilitySystemInterface 接口实现
	// 获取能力系统组件（覆盖虚函数）
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End IAbilitySystemInterface 接口

protected:
	//~Begin APawn 接口重写
	// 当控制器拥有此角色时调用
	virtual void PossessedBy(AController* NewController) override;
	//~End APawn 接口

	/**
	 * 能力系统组件
	 * VisibleAnywhere - 在编辑器和运行时可见
	 * BlueprintReadOnly - 蓝图只读访问
	 * Category - 属性分类
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem")
	TObjectPtr<UHodgeAbilitySystemComponentBase> HodgeAbilitySystemComponent = nullptr;

	
	/**
	 * 运动扭曲组件
	 * 用于动态调整角色运动（如攻击时自动靠近目标）
	 */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MotionWarping")
	//UMotionWarpingComponent* MotionWarpingComponent;

public:
	// 内联获取能力系统组件
	FORCEINLINE UHodgeAbilitySystemComponentBase* GetHodgeAbilitySystemComponent() const
	{
		return HodgeAbilitySystemComponent;
	}
};
