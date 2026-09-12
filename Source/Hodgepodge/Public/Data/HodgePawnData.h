// 111屎山代码来袭

#pragma once

// UE 核心类型与基础宏。
#include "CoreMinimal.h"

// UPrimaryDataAsset 所需头文件。
#include "Engine/DataAsset.h"

#include "HodgePawnData.generated.h"

// AbilitySet：用于批量授予 Ability、GameplayEffect、AttributeSet 等 GAS 资源。
class UHodgeAbilitySet;

// CameraMode：定义 Pawn 默认使用的相机模式。
class UHodgeCameraMode;

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
	UHodgePawnData(const FObjectInitializer& ObjectInitializer);

public:
	// Class to instantiate for this pawn (should usually derive from AHodgePawn or AHodgeCharacter).
	// 实际创建 Pawn 时使用的 Pawn 类，通常应该继承自项目自己的 Pawn 或 Character 基类。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	// 需要授予给这个 Pawn 的 ASC 的 AbilitySet 集合。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Abilities")
	TArray<TObjectPtr<UHodgeAbilitySet>> AbilitySets;

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
