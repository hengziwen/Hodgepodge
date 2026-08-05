/**
 * @file HodgeCharacterBase.h
 * @brief AHodgeCharacterBase 类的头文件
 *
 * 角色基类,所有角色的公共基类,
 * 提供 PreInitializeComponents、BeginPlay、EndPlay 等生命周期扩展点。
 */

#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "HodgeCharacterBase.generated.h"

/**
 * @brief Hodgepodge 框架的角色基类
 *
 * 继承自 AAlsCharacter,是所有角色的公共基类。
 * 提供了 PreInitializeComponents、BeginPlay、EndPlay 等生命周期回调的扩展点,
 * 方便子类在角色的不同生命阶段插入自定义逻辑。
 *
 * 继承层次:
 * ACharacter → AAlsCharacter → AHodgeCharacterBase → AHodgeCombatCharacter → AHodgeHeroCharacter
 *
 * 继承 AAlsCharacter 是为了获得 ALS(Advanced Locomotion System)提供的
 * 移动、动画、状态管理等底层能力,同时保持本框架的生命周期扩展点。
 * 移动组件与相机组件沿用 ALS 原版(UAlsCharacterMovementComponent / UAlsCameraComponent)。
 *
 * 注意:GAS 初始化逻辑放在 AHodgeHeroCharacter 中,
 *      基类 AHodgeCharacterBase 保持通用,不直接依赖 GAS 系统。
 */
UCLASS()
class HODGEPODGE_API AHodgeCharacterBase : public AAlsCharacter
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数
	 *
	 * 接收 FObjectInitializer 并传递给父类 AAlsCharacter,
	 * 因为 AAlsCharacter 的构造函数需要它来设置默认移动组件。
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
	 * @brief Actor结束播放时的回调(重写自AActor)
	 *
	 * Actor被销毁、关卡切换或游戏结束时触发,用于清理资源。
	 * 当前实现仅调用 Super,预留扩展点供子类使用。
	 *
	 * @param EndPlayReason 结束原因(如关卡切换、Actor被销毁、游戏结束等)
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};