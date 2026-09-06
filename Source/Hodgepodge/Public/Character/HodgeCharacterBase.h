// **
//  * @file HodgeCharacterBase.h
//  * @brief AHodgeCharacterBase 类的头文件
//  *
//  * 角色基类,所有角色的公共基类,
//  * 提供 PreInitializeComponents、BeginPlay、EndPlay 等生命周期扩展点。
//  */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HodgeCharacterBase.generated.h"

/**
 * @brief Hodgepodge 框架的角色基类
 *
 * 继承自 ACharacter,是所有角色的公共基类。
 * 提供了 PreInitializeComponents、BeginPlay、EndPlay 等生命周期回调的扩展点,
 * 方便子类在角色的不同生命阶段插入自定义逻辑。
 *
 * 继承层次:
 * ACharacter → AHodgeCharacterBase → AHodgeCombatCharacter → AHodgeHeroCharacter
 *                                                         → AHodgeEnemyCharacter
 *
 * 本类不再依赖 ALS(Advanced Locomotion System),移动组件在构造函数中通过
 * FObjectInitializer::SetDefaultSubobjectClass 替换为 UHodgeCharacterMovementComponent。
 * 移动参数(速度、加速度、旋转方式等)由蓝图 / 数据资产配置,C++ 侧不再硬编码。
 *
 * 注意:GAS 初始化逻辑放在 AHodgeHeroCharacter 中,
 *      基类 AHodgeCharacterBase 保持通用,不直接依赖 GAS 系统。
 */
UCLASS()
class HODGEPODGE_API AHodgeCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数
	 *
	 * 接收 FObjectInitializer 并传递给父类 ACharacter,
	 * 同时用它把默认的 CharacterMovement 子组件替换为 UHodgeCharacterMovementComponent。
	 */
	explicit AHodgeCharacterBase(const FObjectInitializer& ObjectInitializer);

	/**
	 * @brief Actor组件初始化前的回调(重写自AActor)
	 *
	 * 在组件被初始化之前调用,用于设置组件间的引用关系等。
	 * 当前实现仅调用 Super,预留扩展点供子类使用。
	 */
	virtual void PreInitializeComponents() override;

	/**
	 * @brief Actor组件初始化完成后的回调(重写自AActor)
	 *
	 * 所有组件初始化完成后调用,此时可以安全地与其他组件交互。
	 * 当前实现仅调用 Super,预留扩展点供子类使用。
	 */
	virtual void BeginPlay() override;

	/**
	 * 简介: Actor(演员)结束播放时的回调(重写自AActor(演员))
	 *
	 * Actor(演员)被销毁、关卡切换或游戏结束时触发,用于清理资源。
	 * 当前实现仅调用 Super,预留扩展点供子类使用。
	 *
	 * 参数: EndPlayReason 结束原因(如关卡切换、Actor(演员)被销毁、游戏结束等)
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
