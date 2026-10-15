/** @file HodgeHeroComponent.h
 * @brief 声明玩家输入与相机接入组件相关的类型、接口与现有处理流程。
 */
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
#include "Combat/HodgeCharacterFacingTypes.h"
#include "Combat/HodgeMovementActionTypes.h"
#include "Camera/HodgeCinematicCameraTypes.h"
#include "Combat/HodgeHavocRules.h"

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
class UHodgeCinematicCameraProfile;
class UHodgeGameplayAbility_Definition;
class UHodgeAnimNotifyState_CinematicCamera;

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
	/** 构造玩家输入与相机接入组件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeHeroComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns the hero component if one exists on the specified actor. */
	// 从指定 Actor 上查找 HeroComponent，不存在时返回 nullptr。
	/** 查找玩家输入与相机接入组件。
	 * @param Actor 本次操作关联的Actor。
	 * @return 玩家输入与相机接入组件的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Hero")
	static UHodgeHeroComponent* FindHeroComponent(const AActor* Actor)
	{
		// Actor 有效时查找组件，否则直接返回 nullptr。
		return (Actor ? Actor->FindComponentByClass<UHodgeHeroComponent>() : nullptr);
	}

	/** Overrides the camera from an active gameplay ability */
	// 允许某个正在运行的 GameplayAbility 临时覆盖当前相机模式。
	/** 设置能力相机Mode。
	 * @param CameraMode 相机模式与混合栈，由调用方传入。
	 * @param OwningSpecHandle 已授予能力的Spec句柄。
	 */
	FHodgeAbilityCameraHandle SetAbilityCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode,
	                          const FGameplayAbilitySpecHandle& OwningSpecHandle, FGuid ExecutionId = FGuid());
	/** 相机释放必须匹配完整申请，避免同一Spec的迟到清理。 */
	bool ClearAbilityCameraMode(const FHodgeAbilityCameraHandle& Handle);
	/** 查询精确主蒙太奇窗口，只有本地拥有者建立电影申请。 */
	void RefreshCinematicCamera();
	/** 只释放指定执行拥有的电影申请，供取消和换Pawn清理。 */
	void EndCinematicCameraForExecution(FGuid ExecutionId);
	/** 运行时源或安全检查失败后，本次实例不无限重新申请。 */
	void AbortCinematicCamera(const FHodgeAbilityCameraHandle& Handle, const FString& Reason);
	/** 当前Profile受GC保护，电影模式只读消费。 */
	const UHodgeCinematicCameraProfile* GetCinematicProfile() const { return CinematicProfile; }
	/** 暴露只读源时间和会话身份，不授权服务器伤害。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Camera") FHodgeCinematicCameraState GetCinematicCameraState() const { return CinematicState; }
	/** 查询能力、锁定和默认配置仲裁后的期望镜头，实际混合栈另由相机组件查询。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Camera") TSubclassOf<UHodgeCameraMode> GetDesiredCameraMode() const { return DetermineCameraMode(); }

	/** Clears the camera override if it is set */
	// 如果指定 Ability 正是当前相机覆盖的拥有者，则清除 Ability 相机模式。
	/** 清空能力相机Mode。
	 * @param OwningSpecHandle 已授予能力的Spec句柄。
	 */
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** Adds mode-specific input config */
	// 动态添加一份额外输入配置，常用于 GameFeature、武器模式或特殊玩法。
	/** 添加Additional输入配置。
	 * @param InputConfig 输入动作与Tag映射，由调用方传入。
	 */
	void AddAdditionalInputConfig(const UHodgeInputConfig* InputConfig);

	/** Removes a mode-specific input config if it has been added */
	// 移除之前动态添加的额外输入配置。
	/** 移除Additional输入配置。
	 * @param InputConfig 输入动作与Tag映射，由调用方传入。
	 */
	void RemoveAdditionalInputConfig(const UHodgeInputConfig* InputConfig);

	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	// 判断当前 Pawn 是否已经完成足够的初始化，可以安全添加额外输入绑定。
	/** 判断就绪目标绑定Inputs。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsReadyToBindInputs() const;

	/**
	 * 玩家当前是否有明确的"移动意图"。
	 *
	 * 读的是**原始输入量**（Input_Move 的二维值），不依赖角色是否真的产生了位移：
	 * 攻击期间角色可能被蒙太奇"钉住"，但只要玩家在推摇杆 / 按方向键，这里就是 true。
	 * 这正是"用移动取消后摇"需要的那个信号 —— 它属于输入层，不属于 动画通知。
	 *
	 * ⚠️ 采集点在 Input_Move 里，位于任何"屏蔽移动输入"之前。若以后用 SetIgnoreMoveInput /
	 * DisableMovement 让攻击期间不能移动，Input_Move 将不再回调，这个意图会恒为 false，
	 * 依赖它的取消逻辑会**静默失效**。所以：禁止移动不要用屏蔽输入来实现。
	 */
	/** 判断是否存在移动意图。
	 * @param Threshold 阈值，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Hero|Input")
	bool HasMoveIntent(float Threshold = 0.1f) const;
	/** 重置玩法输入。 */
	void ResetGameplayInput();
	/** 开始Sprint奔跑输入。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Movement") void BeginSprintInput();
	/** 结束Sprint奔跑输入。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Movement") void EndSprintInput();
	/** 获取Sprint奔跑输入会话。
	 * @return Sprint奔跑输入会话的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Movement") FHodgeSprintInputSession GetSprintInputSession() const { return SprintInputSession; }
	/** 通知Dash冲刺Committed。
	 * @param SessionId 本次会话的唯一身份。
	 */
	void NotifyDashCommitted(int32 SessionId);
	/** 通知Sprint奔跑Committed。
	 * @param SessionId 本次会话的唯一身份。
	 */
	void NotifySprintCommitted(int32 SessionId);
	/** 通知移动动作结束。
	 * @param SessionId 本次会话的唯一身份。
	 * @param bHandedOff 是否HandedOff的控制标记。
	 */
	void NotifyMovementActionEnded(int32 SessionId, bool bHandedOff);
	/** 请求Sprint奔跑Handoff。
	 * @param SessionId 本次会话的唯一身份。
	 * @param HoldThreshold 按住阈值，由调用方传入。
	 */
	void RequestSprintHandoff(int32 SessionId, float HoldThreshold);

	// 最近的二维移动输入原始值（X = 左右，Y = 前后；未做阈值判断），供需要方向或大小的消费方使用。
	/** 获取移动意图。
	 * @return 移动意图的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Hero|Input")
	FVector2D GetMoveIntent() const { return CurrentMoveInput; }
	/** 获取移动意图快照。
	 * @return 移动意图快照的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Hero|Input")
	FHodgeMoveIntentSnapshot GetMoveIntentSnapshot() const { return MoveIntentSnapshot; }
	/** 获取世界移动意图。
	 * @return 世界移动意图的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Hero|Input")
	FVector GetWorldMoveIntent() const { return MoveIntentSnapshot.DesiredDirectionWorld; }

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
	/** 获取功能扩展名称。
	 * @return 功能扩展名称的当前值或引用。
	 */
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }

	// 判断当前初始化状态是否满足进入 DesiredState 的前置条件。
	/** 只判断初始化阶段能否推进，不承担阶段副作用。
	 * @param Manager 管理器，由调用方传入。
	 * @param CurrentState 当前状态，用于对应标签查询或匹配。
	 * @param DesiredState 期望状态，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                FGameplayTag DesiredState) const override;

	// 当组件真正切换到新的初始化状态时执行对应初始化逻辑。
	/** 执行初始化状态转换对应的阶段处理。
	 * @param Manager 管理器，由调用方传入。
	 * @param CurrentState 当前状态，用于对应标签查询或匹配。
	 * @param DesiredState 期望状态，用于对应标签查询或匹配。
	 */
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                   FGameplayTag DesiredState) override;

	// 监听同一个 Actor 上其他 Feature 的初始化状态变化。
	/** 处理Actor对象Init状态变化。
	 * @param Params 参数，由调用方传入。
	 */
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;

	// 检查当前组件是否可以继续沿默认 InitState 链推进。
	/** 尝试推进当前组件及相关Feature的初始化链。 */
	virtual void CheckDefaultInitialization() override;

	//~ End IGameFrameworkInitStateInterface interface

protected:
	// 组件注册到 Actor 时调用，可用于检查 Owner 和组件依赖。
	/** 组件注册后接入当前Owner的依赖。 */
	virtual void OnRegister() override;

	// 游戏开始时调用，通常在这里注册 InitState Feature 并开始初始化流程。
	/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
	virtual void BeginPlay() override;

	// 组件结束生命周期时调用，用于注销初始化 Feature 和清理资源。
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param EndPlayReason 结束PlayReason，由调用方传入。
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 初始化本地玩家输入，包括 MappingContext、Native Input 和 Ability Input 绑定。
	/** 初始化玩家输入。
	 * @param PlayerInputComponent 玩家输入组件，由调用方传入。
	 */
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	// Ability 类型输入按下时调用，将 InputTag 转发给 ASC。
	/** 把玩家输入按下转交ASC的Tag输入入口。
	 * @param InputTag 输入动作对应的GameplayTag。
	 */
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	// Ability 类型输入松开时调用，将 InputTag 转发给 ASC。
	/** 把玩家输入松开转交ASC的Tag输入入口。
	 * @param InputTag 输入动作对应的GameplayTag。
	 */
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	// 处理角色移动输入。
	/** 把当前二维输入转换并提交给角色移动意图。
	 * @param InputActionValue 输入动作值，由调用方传入。
	 */
	void Input_Move(const FInputActionValue& InputActionValue);

	// 移动输入结束（Completed）或被取消（Canceled）时清零移动意图。
	// 只接 Triggered 会让意图"粘住"：松手、切换 InputContext、失焦都不会让它回到 0。
	/** 清理已经停止的移动输入意图。
	 * @param InputActionValue 输入动作值，由调用方传入。
	 */
	void Input_MoveStopped(const FInputActionValue& InputActionValue);

	// 处理鼠标视角输入。
	/** 按输入配置处理鼠标视角增量。
	 * @param InputActionValue 输入动作值，由调用方传入。
	 */
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	// 处理手柄摇杆视角输入。
	/** 按输入配置处理手柄视角增量。
	 * @param InputActionValue 输入动作值，由调用方传入。
	 */
	void Input_LookStick(const FInputActionValue& InputActionValue);

	// 处理蹲伏输入。
	/** 处理当前玩家的下蹲输入。
	 * @param InputActionValue 输入动作值，由调用方传入。
	 */
	void Input_Crouch(const FInputActionValue& InputActionValue);

	// 处理自动奔跑输入。
	/** 切换当前玩家的自动移动意图。
	 * @param InputActionValue 输入动作值，由调用方传入。
	 */
	void Input_AutoRun(const FInputActionValue& InputActionValue);

	// 根据 Ability 覆盖和 PawnData 默认配置决定当前应该使用的相机模式。
	/** 按当前角色与能力状态选择相机模式类。
	 * @return 本次计算、查询或创建得到的TSubclassOfUHodge相机Mode。
	 */
	TSubclassOf<UHodgeCameraMode> DetermineCameraMode() const;

	// 记录移动意图并按需广播变化。Input_Move / Input_MoveStopped 都走这里，保证只有一条写入路径。
	/** 设置移动意图。
	 * @param NewValue 新值，由调用方传入。
	 */
	void SetMoveIntent(const FVector2D& NewValue);

	// 重新计算布尔意图，只在结果翻转时广播 OnMoveIntentChanged。
	/** 刷新移动意图。 */
	void RefreshMoveIntent();

protected:
	// 默认输入 MappingContext 及其优先级配置。
	/** 共鸣回路只观察长按，不推迟原普攻按下事件。 */
	FHodgeHavocHoldSession CircuitHold;
	FTimerHandle CircuitHoldTimer;
	void BeginCircuitHold();
	void UpdateCircuitHold();
	void EndCircuitHold();
	UPROPERTY(EditAnywhere)
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	/** Camera mode set by an ability. */
	// 当前由 GameplayAbility 临时指定的相机模式。
	UPROPERTY()
	TSubclassOf<UHodgeCameraMode> AbilityCameraMode;

	/** Spec handle for the last ability to set a camera mode. */
	// 当前 AbilityCameraMode 对应的 AbilitySpecHandle，用于确认谁拥有这次相机覆盖。
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;
	/** 完整能力相机身份是清理的唯一判据。 */
	FHodgeAbilityCameraHandle AbilityCameraHandle;
	/** 单调增长的申请代次，同类相机重复使用仍生成新会话。 */
	int32 NextCameraGeneration = 0;
	/** 当前电影配置引用，防止运行期间被GC回收。 */
	UPROPERTY() TObjectPtr<UHodgeCinematicCameraProfile> CinematicProfile;
	/** 本地电影事实来自实际Definition实例，而非通知对象中的变量。 */
	UPROPERTY() TWeakObjectPtr<UHodgeGameplayAbility_Definition> CinematicSource;
	UPROPERTY() TObjectPtr<UHodgeAnimNotifyState_CinematicCamera> CinematicWindow;
	FHodgeCinematicCameraState CinematicState;
	/** 失败执行仅报告一次，下一次激活可重新验证修复后的配置。 */
	FGuid FailedCinematicExecution;
	int32 FailedCinematicInstance = INDEX_NONE;
	/** 查询和结束回调发生重入时不重复申请。 */
	bool bRefreshingCinematic = false;

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
	// 移动意图快照的数据或对象引用。
	FHodgeMoveIntentSnapshot MoveIntentSnapshot;
	// Sprint奔跑输入会话的数据或对象引用。
	FHodgeSprintInputSession SprintInputSession;

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
