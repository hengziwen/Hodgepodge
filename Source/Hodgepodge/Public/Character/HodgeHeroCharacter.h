// **
//  * @file HodgeHeroCharacter.h
//  * @brief AHodgeHeroCharacter 类的头文件
//  *
//  * 英雄角色类,玩家实际操控的主角。
//  * 继承自 AHodgeCombatCharacter(其父类为 AHodgeCharacterBase,再往上是 AAlsCharacter),
//  * 因此天然获得 ALS 的移动、动画与状态管理能力。
//  *
//  * 本类承担两类职责:
//  * 1. GAS 的双初始化入口(PossessedBy + OnRep_PlayerState)
//  * 2. ALS 的相机组件与输入接入(输入使用 Hodgepodge 输入框架 UHodgeInputComponentBase)
//  */

#pragma once

#include "CoreMinimal.h"
#include "Character/HodgeCombatCharacter.h"
#include "HodgeHeroCharacter.generated.h"

struct FInputActionValue;
class UAlsCameraComponent;
class UHodgeInputComponentBase;
class UHodgeInputConfig;
class UInputMappingContext;

/**
 * @brief Hodgepodge 框架的英雄角色类
 *
 * 继承自 AHodgeCombatCharacter,是玩家实际操控的主角类。
 *
 * 继承层次:
 * ACharacter → AAlsCharacter → AHodgeCharacterBase → AHodgeCombatCharacter → AHodgeHeroCharacter
 *
 * 通过继承 AAlsCharacter 获得:
 * - ALS 移动组件 UAlsCharacterMovementComponent(在 AAlsCharacter 构造函数中自动替换)
 * - ALS 动画实例 UAlsAnimationInstance(动画蓝图可直接使用)
 * - ALS 完整的状态机(视角、姿态、步态、旋转模式、布娃娃、攀爬等)
 *
 * 本类负责:
 * 1. 承载 GAS 的双初始化入口(PossessedBy + OnRep_PlayerState)
 * 2. 创建并挂载 ALS 相机组件 UAlsCameraComponent
 * 3. 处理 ALS 移动所需的输入(移动、视角、冲刺、下蹲、跳跃、瞄准、
 *    布娃娃、翻滚、旋转模式、视角模式、切换肩部等)
 *
 * 注意:移动组件与相机组件沿用 ALS 原版。
 *      输入动作(UInputAction)统一由 UHodgeInputConfig 数据资产管理,
 *      通过 InputTag 映射;输入组件重写为 UHodgeInputComponentBase。
 */
UCLASS()
class HODGEPODGE_API AHodgeHeroCharacter : public AHodgeCombatCharacter
{
	GENERATED_BODY()

protected:
	/**
	 * @brief ALS 相机组件
	 *
	 * 基于 UAlsCameraComponent,提供第一/第三人称相机、肩部切换、
	 * 场景深度等 ALS 的相机能力。
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Camera")
	TObjectPtr<UAlsCameraComponent> Camera;

	// =============================================================================
	// ALS 输入
	//
	// 输入动作(U输入动作)统一由 UHodge输入Config 数据资产管理,
	// 通过 输入Tag 与对应的 U输入动作 映射(见 Native输入动作s)。
	// 输入映射上下文(输入MappingContext)仍由角色蓝图配置,
	// 并在 NotifyControllerChanged 中负责添加/移除。
	// =============================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hodge|Input", Meta = (DisplayThumbnail = false))
	TObjectPtr<UHodgeInputConfig> InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hodge|Input", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputMappingContext> InputMappingContext;

	// =============================================================================
	// 视角灵敏度
	// =============================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hodge|Input", Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookUpMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hodge|Input", Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookRightMouseSensitivity{1.0f};

public:
	/**
	 * @brief 构造函数
	 *
	 * 创建 ALS 相机组件并挂载到 Mesh 上。
	 */
	explicit AHodgeHeroCharacter(const FObjectInitializer& ObjectInitializer);

	/**
	 * @brief 创建玩家输入组件(重写自 APawn)
	 *
	 * 返回 UHodgeInputComponentBase,使角色的输入组件具备 Hodgepodge
	 * 输入框架的绑定能力(BindNativeAction 等)。
	 *
	 * @return 新创建的玩家输入组件
	 */
	virtual UInputComponent* CreatePlayerInputComponent() override;

	/**
	 * @brief 控制器变化时同步输入映射上下文
	 *
	 * 当角色被 Possess 或 Unpossess 时,自动添加/移除 InputMappingContext。
	 */
	virtual void NotifyControllerChanged() override;

	// =============================================================================
	// Camera
	// =============================================================================

protected:
	/**
	 * @brief 计算相机视图(重写自 AAlsCharacter)
	 *
	 * 当 ALS 相机组件激活时使用其视图,否则回退到父类逻辑。
	 */
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

	// =============================================================================
	// 输入
	// =============================================================================

protected:
	/**
	 * @brief 设置玩家输入组件(重写自 ACharacter)
	 *
	 * 将 ALS 移动所需的各个输入动作绑定到对应的处理函数。
	 * 输入动作通过 UHodgeInputConfig 按 InputTag 查找并绑定。
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

	// =============================================================================
	// ALS 移动输入处理函数
	// =============================================================================

private:
	void Input_OnLookMouse(const FInputActionValue& ActionValue);

	void Input_OnMove(const FInputActionValue& ActionValue);

	void Input_OnSprint(const FInputActionValue& ActionValue);

	void Input_OnWalk();

	void Input_OnCrouch();

	void Input_OnJump(const FInputActionValue& ActionValue);

	void Input_OnAim(const FInputActionValue& ActionValue);

	void Input_OnRagdoll();

	void Input_OnRoll();

	void Input_OnRotationMode();

	void Input_OnViewMode();

	void Input_OnSwitchShoulder();

	// =============================================================================
	// GAS 双初始化入口
	// =============================================================================

public:
	/**
	 * @brief 服务器端角色被 Possess 时的回调(重写自 ACharacter)
	 *
	 * 这是 GAS 服务器端初始化的入口。执行流程:
	 * 1. 调用 Super::PossessedBy() 执行父类逻辑(含 AAlsCharacter 的 RefreshMeshProperties)
	 * 2. 获取 AHodgePlayerStateBase 的 PlayerState
	 * 3. 调用 InitializeAbilitySystemForCharacter(this) 完成 GAS 初始化
	 *
	 * @param NewController 新的控制器指针(通常是 AHodgePlayerControllerBase)
	 */
	virtual void PossessedBy(AController* NewController) override;

	/**
	 * 简介: 客户端 玩家State 复制到位时的回调(重写自 A角色)
	 *
	 * 这是 GAS 客户端初始化的入口。
	 */
	virtual void OnRep_PlayerState() override;

	// =============================================================================
	// Debug
	// =============================================================================

public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;
};
