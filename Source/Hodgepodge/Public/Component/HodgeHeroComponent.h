// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// GameFramework 初始化状态接口，用于参与 Spawned → DataAvailable → DataInitialized → GameplayReady 初始化流程。
#include "Components/GameFrameworkInitStateInterface.h"

// PawnComponent：专门附加在 Pawn 上的组件基类。
#include "Components/PawnComponent.h"

// 输入映射配置结构 FInputMappingContextAndPriority 的定义。
#include "GameFeatures/GameFeatureAction_AddInputContextMapping.h"

// GAS AbilitySpecHandle，用于唯一标识某个 GameplayAbilitySpec。
#include "GameplayAbilitySpecHandle.h"

#include "HodgeHeroComponent.generated.h"

// 前置声明 EndPlay 原因枚举，避免额外包含头文件。
namespace EEndPlayReason
{
	enum Type : int;
}


// GameFrameworkComponentManager：负责组件扩展事件和 InitState 协调。
class UGameFrameworkComponentManager;

// Pawn 上实际用于绑定输入的 InputComponent。
class UInputComponent;

// 项目自定义相机模式。
class UHodgeCameraMode;

// 项目自定义输入配置，负责 InputAction 与 GameplayTag 的映射。
class UHodgeInputConfig;

class UObject;

// Actor 初始化状态发生变化时传递的参数。
struct FActorInitStateChangedParams;

struct FFrame;

// GameplayTag 类型。
struct FGameplayTag;

// Enhanced Input 输入值。
struct FInputActionValue;

/**
 * 移动意图发生变化（true = 出现了明确的移动意图，false = 意图消失）。
 *
 * 只在布尔结果翻转时广播：推杆过程中 Input_Move 是每帧触发的，不能每帧广播。
 * 需要方向或大小的一方自己读 GetMoveIntent()。
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeMoveIntentChanged, bool, bHasMoveIntent);

/**
 * Component that sets up input and camera handling for player controlled pawns (or bots that simulate players).
 * This depends on a PawnExtensionComponent to coordinate initialization.
 *
 * HeroComponent 负责玩家型 Pawn 的输入和相机初始化。
 * 它依赖 PawnExtensionComponent 协调 PawnData、Controller、PlayerState、ASC 等初始化顺序。
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	// 构造函数。
	UHodgeHeroComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns the hero component if one exists on the specified actor. */
	// 从指定 Actor 上查找 HeroComponent，不存在时返回 nullptr。
	UFUNCTION(BlueprintPure, Category = "Hodge|Hero")
	static UHodgeHeroComponent* FindHeroComponent(const AActor* Actor)
	{
		// Actor 有效时查找组件，否则直接返回 nullptr。
		return (Actor ? Actor->FindComponentByClass<UHodgeHeroComponent>() : nullptr);
	}

	/** Overrides the camera from an active gameplay ability */
	// 允许某个正在运行的 GameplayAbility 临时覆盖当前相机模式。
	void SetAbilityCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode,
	                          const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** Clears the camera override if it is set */
	// 如果指定 Ability 正是当前相机覆盖的拥有者，则清除 Ability 相机模式。
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** Adds mode-specific input config */
	// 动态添加一份额外输入配置，常用于 GameFeature、武器模式或特殊玩法。
	void AddAdditionalInputConfig(const UHodgeInputConfig* InputConfig);

	/** Removes a mode-specific input config if it has been added */
	// 移除之前动态添加的额外输入配置。
	void RemoveAdditionalInputConfig(const UHodgeInputConfig* InputConfig);

	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	// 判断当前 Pawn 是否已经完成足够的初始化，可以安全添加额外输入绑定。
	bool IsReadyToBindInputs() const;

	/**
	 * 玩家当前是否有明确的"移动意图"。
	 *
	 * 读的是**原始输入量**（Input_Move 的二维值），不依赖角色是否真的产生了位移：
	 * 攻击期间角色可能被蒙太奇"钉住"，但只要玩家在推摇杆 / 按方向键，这里就是 true。
	 * 这正是"用移动取消后摇"需要的那个信号 —— 它属于输入层，不属于 Timeline。
	 *
	 * ⚠️ 采集点在 Input_Move 里，位于任何"屏蔽移动输入"之前。若以后用 SetIgnoreMoveInput /
	 * DisableMovement 让攻击期间不能移动，Input_Move 将不再回调，这个意图会恒为 false，
	 * 依赖它的取消逻辑会**静默失效**。所以：禁止移动不要用屏蔽输入来实现。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Hero|Input")
	bool HasMoveIntent(float Threshold = 0.1f) const;

	// 最近的二维移动输入原始值（X = 左右，Y = 前后；未做阈值判断），供需要方向或大小的消费方使用。
	UFUNCTION(BlueprintPure, Category = "Hodge|Hero|Input")
	FVector2D GetMoveIntent() const { return CurrentMoveInput; }

	// 移动意图变化事件：只在 false ↔ true 翻转时广播。
	UPROPERTY(BlueprintAssignable, Category = "Hodge|Hero|Input")
	FHodgeMoveIntentChanged OnMoveIntentChanged;

	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	// 当 Ability 输入已经可以绑定时，通过 GameFrameworkComponentManager 广播的扩展事件名。
	static const FName NAME_BindInputsNow;

	/** The name of this component-implemented feature */
	// HeroComponent 在 InitState 系统中的 Feature 名称。
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface

	// 返回当前组件在初始化状态系统中的 Feature 名称。
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }

	// 判断当前初始化状态是否满足进入 DesiredState 的前置条件。
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                FGameplayTag DesiredState) const override;

	// 当组件真正切换到新的初始化状态时执行对应初始化逻辑。
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                   FGameplayTag DesiredState) override;

	// 监听同一个 Actor 上其他 Feature 的初始化状态变化。
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;

	// 检查当前组件是否可以继续沿默认 InitState 链推进。
	virtual void CheckDefaultInitialization() override;

	//~ End IGameFrameworkInitStateInterface interface

protected:
	// 组件注册到 Actor 时调用，可用于检查 Owner 和组件依赖。
	virtual void OnRegister() override;

	// 游戏开始时调用，通常在这里注册 InitState Feature 并开始初始化流程。
	virtual void BeginPlay() override;

	// 组件结束生命周期时调用，用于注销初始化 Feature 和清理资源。
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 初始化本地玩家输入，包括 MappingContext、Native Input 和 Ability Input 绑定。
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	// Ability 类型输入按下时调用，将 InputTag 转发给 ASC。
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	// Ability 类型输入松开时调用，将 InputTag 转发给 ASC。
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	// 处理角色移动输入。
	void Input_Move(const FInputActionValue& InputActionValue);

	// 移动输入结束（Completed）或被取消（Canceled）时清零移动意图。
	// 只接 Triggered 会让意图"粘住"：松手、切换 InputContext、失焦都不会让它回到 0。
	void Input_MoveStopped(const FInputActionValue& InputActionValue);

	// 处理鼠标视角输入。
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	// 处理手柄摇杆视角输入。
	void Input_LookStick(const FInputActionValue& InputActionValue);

	// 处理蹲伏输入。
	void Input_Crouch(const FInputActionValue& InputActionValue);

	// 处理自动奔跑输入。
	void Input_AutoRun(const FInputActionValue& InputActionValue);

	// 根据 Ability 覆盖和 PawnData 默认配置决定当前应该使用的相机模式。
	TSubclassOf<UHodgeCameraMode> DetermineCameraMode() const;

	// 记录移动意图并按需广播变化。Input_Move / Input_MoveStopped 都走这里，保证只有一条写入路径。
	void SetMoveIntent(const FVector2D& NewValue);

	// 重新计算布尔意图，只在结果翻转时广播 OnMoveIntentChanged。
	void RefreshMoveIntent();

protected:
	// 默认输入 MappingContext 及其优先级配置。
	UPROPERTY(EditAnywhere)
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	/** Camera mode set by an ability. */
	// 当前由 GameplayAbility 临时指定的相机模式。
	UPROPERTY()
	TSubclassOf<UHodgeCameraMode> AbilityCameraMode;

	/** Spec handle for the last ability to set a camera mode. */
	// 当前 AbilityCameraMode 对应的 AbilitySpecHandle，用于确认谁拥有这次相机覆盖。
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;

	/** True when player input bindings have been applied, will never be true for non - players */
	// 标记本地玩家输入绑定是否已经完成；非玩家控制 Pawn 永远不会为 true。
	bool bReadyToBindInputs;

	/**
	 * 最近一次二维移动输入原始值（X = 左右，Y = 前后）。
	 *
	 * 由 Input_Move（Triggered）写入、Input_MoveStopped（Completed / Canceled）清零。
	 * 它同时供 HasMoveIntent / GetMoveIntent 与 OnMoveIntentChanged 使用。
	 */
	FVector2D CurrentMoveInput = FVector2D::ZeroVector;

	// 上一次广播出去的移动意图，用于"只在翻转时广播"。
	bool bLastMoveIntent = false;

	/**
	 * 保存 AddAdditionalInputConfig 产生的 Ability 输入绑定句柄，键为对应的 InputConfig。
	 * RemoveAdditionalInputConfig 通过它精确解绑，避免句柄丢失导致额外输入无法移除。
	 *
	 * 说明：这里刻意不用 UPROPERTY。InputConfig 指针为 const，UHT 不支持
	 * TObjectPtr<const T> 作为反射属性；InputConfig 的生命周期由 GameFeature/Experience
	 * 的软引用资产链持有，本 map 只作为查找键，不延长其生命周期。
	 */
	TMap<const UHodgeInputConfig*, TArray<uint32>> AdditionalInputConfigHandles;
};
