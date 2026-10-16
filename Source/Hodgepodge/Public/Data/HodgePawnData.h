/** @file HodgePawnData.h
 * @brief 声明Pawn能力/输入/相机配置相关的类型、接口与现有处理流程。
 */
// 111屎山代码来袭

#pragma once

// UE 核心类型与基础宏。
#include "CoreMinimal.h"

// UPrimaryDataAsset 所需头文件。
#include "Engine/DataAsset.h"
#include "Inventory/HodgeInventoryTypes.h"

#include "HodgePawnData.generated.h"

// AbilitySet：用于批量授予 Ability、GameplayEffect、AttributeSet 等 GAS 资源。
class UHodgeAbilitySet;
class UHodgeComboDefinition;
class UHodgeEquipmentDefinition;
class UHodgeCharacterStatProfile;
class UHodgeHitReactionProfile;
class UHodgeLockOnProfile;

// CameraMode：定义 Pawn 默认使用的相机模式。
class UHodgeCameraMode;
class UHodgeSprintAbilityProfile;

// InputConfig：定义 InputAction 与 GameplayTag 等输入配置。
class UHodgeInputConfig;

// AbilityTagRelationshipMapping：定义 AbilityTag 之间的阻塞、取消、激活要求等关系。
class UHodgeAbilityTagRelationshipMapping;

/**
 * Pawn data asset for storing pawn-specific information.
 *
 * Pawn 专属配置数据资产。
 * 用于集中描述一个 Pawn 应该使用什么 PawnClass、AbilitySet、输入配置、Tag 关系以及默认相机模式。
 */
UCLASS()
class HODGEPODGE_API UHodgePawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 构造函数。
	/** 构造Pawn能力/输入/相机配置并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgePawnData(const FObjectInitializer& ObjectInitializer);
#if WITH_EDITOR
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

public:
	// Class to instantiate for this pawn (should usually derive from AHodgePawn or AHodgeCharacter).
	// 实际创建 Pawn 时使用的 Pawn 类，通常应该继承自项目自己的 Pawn 或 Character 基类。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Pawn")
	TSubclassOf<APawn> PawnClass;

	// ASC 就绪后由服务器装备的默认武器，留空表示不自动装备。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Equipment")
	TSubclassOf<UHodgeEquipmentDefinition> DefaultWeaponDefinition;
	/** 玩家库存由 Experience 注入；关闭时保留旧默认装备链。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Inventory") bool bUseInventory = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Inventory", meta=(ClampMin="1", EditCondition="bUseInventory")) int32 InventoryMaxEntries = 40;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Inventory", meta=(EditCondition="bUseInventory")) TArray<FHodgeInventoryDefaultItem> DefaultInventoryItems;
	// 角色初始属性和等级成长配置，攻击需要其初始化完成后才能激活。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Attributes")
	TObjectPtr<UHodgeCharacterStatProfile> StatProfile;
	// 目标自身的受击动作、恢复规则和支持的 Impact。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat")
	TObjectPtr<UHodgeHitReactionProfile> HitReactionProfile;
	// Dash/Sprint参数配置的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Movement")
	TObjectPtr<UHodgeSprintAbilityProfile> SprintAbilityProfile;

	// 玩家显式锁定配置；空值表示此 Pawn 不启用锁定。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Targeting")
	TObjectPtr<UHodgeLockOnProfile> LockOnProfile;

	// Ability sets to grant to this pawn's ability system.
	// 需要授予给这个 Pawn 的 ASC 的 AbilitySet 集合。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Abilities")
	TArray<TObjectPtr<UHodgeAbilitySet>> AbilitySets;

	// 此角色使用的连段图配置，不替代单段技能 Definition。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Abilities")
	TObjectPtr<UHodgeComboDefinition> ComboDefinition;

	// What mapping of ability tags to use for actions taking by this pawn
	// 当前 Pawn 使用的 AbilityTag 关系配置，用于定义 Ability 之间的阻塞、取消和激活条件。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Abilities")
	TObjectPtr<UHodgeAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	// 玩家控制 Pawn 使用的输入配置，用于建立 InputAction 与项目输入逻辑之间的绑定关系。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Input")
	TObjectPtr<UHodgeInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	// 玩家控制 Pawn 默认使用的相机模式。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Camera")
	TSubclassOf<UHodgeCameraMode> DefaultCameraMode;
};
