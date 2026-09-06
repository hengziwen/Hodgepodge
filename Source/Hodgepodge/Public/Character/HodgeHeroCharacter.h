// **
//  * @file HodgeHeroCharacter.h
//  * @brief AHodgeHeroCharacter 类的头文件
//  *
//  * 英雄角色类,玩家实际操控的主角。
//  * 继承自 AHodgeCombatCharacter(其父类为 AHodgeCharacterBase,再往上是 ACharacter)。
//  *
//  * 本类目前只承担 GAS 的双初始化入口(PossessedBy + OnRep_PlayerState)。
//  * 相机组件与输入绑定已从本类移除,后续改由组件化 / GAS Ability 的方式接入,
//  * 接入点见 AHodgeHeroCharacter::BeginPlay 等生命周期扩展点。
//  */

#pragma once

#include "CoreMinimal.h"
#include "Character/HodgeCombatCharacter.h"
#include "HodgeHeroCharacter.generated.h"

/**
 * @brief Hodgepodge 框架的英雄角色类
 *
 * 继承自 AHodgeCombatCharacter,是玩家实际操控的主角类。
 *
 * 继承层次:
 * ACharacter → AHodgeCharacterBase → AHodgeCombatCharacter → AHodgeHeroCharacter
 *
 * 本类负责:
 * 1. 承载 GAS 的双初始化入口(PossessedBy + OnRep_PlayerState)
 * 2. 预留相机 / 输入的接入位置(当前已移除 ALS 的相机组件与输入绑定)
 *
 * 注意:原先基于 ALS 的 UAlsCameraComponent 与全部输入处理(移动、视角、冲刺、
 *      下蹲、跳跃、瞄准、布娃娃、翻滚、旋转模式、视角模式、切换肩部)已移除。
 *      框架自带的 UHodgeCameraComponent / UHodgeCameraSettings /
 *      UHodgeCameraAnimationInstance 不依赖 ALS,可在需要时重新挂载。
 */
UCLASS()
class HODGEPODGE_API AHodgeHeroCharacter : public AHodgeCombatCharacter
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数
	 *
	 * 当前仅把 FObjectInitializer 透传给父类,所有默认组件由基类负责。
	 */
	explicit AHodgeHeroCharacter(const FObjectInitializer& ObjectInitializer);

	// =============================================================================
	// GAS 双初始化入口
	// =============================================================================

public:
	/**
	 * 0@0b0mrief 服务器端角色被 Possess 时的回调(重写自 APawn)
	 *
	 * 这是 GAS 服务器端初始化的入口:
	 * 1. 调用 Super::PossessedBy() 执行父类逻辑
	 * 2. 获取 AHodgePlayerState 的 PlayerState
	 * 3. 调用 InitAbilityActorInfo 完成 GAS 初始化
	 *
	 * @param NewController 新的控制器指针(通常是 AHodgePlayerControllerBase)
	 */
	virtual void PossessedBy(AController* NewController) override;

	/**
	 * @brief 客户端 PlayerState 复制到位时的回调(重写自 AActor)
	 *
	 * 这是 GAS 客户端初始化的入口。
	 */
	virtual void OnRep_PlayerState() override;
};
