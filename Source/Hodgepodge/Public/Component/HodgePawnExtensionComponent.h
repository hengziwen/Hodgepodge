/** @file HodgePawnExtensionComponent.h
 * @brief 声明Pawn初始化扩展组件相关的类型、接口与现有处理流程。
 */
// 111 屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "HodgePawnExtensionComponent.generated.h"

class UHodgePawnData;
class UHodgeAbilitySystemComponent;

// 前置声明 Actor 生命周期结束原因枚举
namespace EEndPlayReason
{
	enum Type : int;
}

/**
 * Pawn 扩展组件，为所有 Pawn 提供通用的扩展能力。
 * 该组件负责协调 Pawn 上其他组件的初始化顺序，并管理 Pawn 与 AbilitySystem 的关联。
 */
UCLASS()
class HODGEPODGE_API UHodgePawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	// 构造 Pawn 扩展组件
	/** 构造Pawn初始化扩展组件并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgePawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	// 当前 PawnExtensionComponent 在 InitState 系统中的功能名称
	static const FName NAME_ActorFeatureName;
	// 事件Pawn角色数据变化对应的事件通知。
	FSimpleMulticastDelegate OnPawnDataChanged;

	//~ Begin IGameFrameworkInitStateInterface interface

	// 返回当前组件对应的 InitState Feature 名称
	/** 获取功能扩展名称。
	 * @return 功能扩展名称的当前值或引用。
	 */
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }

	// 判断组件是否允许从当前初始化状态切换到目标状态
	/** 只判断初始化阶段能否推进，不承担阶段副作用。
	 * @param Manager 管理器，由调用方传入。
	 * @param CurrentState 当前状态，用于对应标签查询或匹配。
	 * @param DesiredState 期望状态，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                FGameplayTag DesiredState) const override;

	// 执行从当前初始化状态切换到目标状态时的具体逻辑
	/** 执行初始化状态转换对应的阶段处理。
	 * @param Manager 管理器，由调用方传入。
	 * @param CurrentState 当前状态，用于对应标签查询或匹配。
	 * @param DesiredState 期望状态，用于对应标签查询或匹配。
	 */
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                   FGameplayTag DesiredState) override;

	// 当所属 Actor 的 InitState 发生变化时通知该组件
	/** 处理Actor对象Init状态变化。
	 * @param Params 参数，由调用方传入。
	 */
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;

	// 检查并推进当前组件的默认初始化流程
	/** 尝试推进当前组件及相关Feature的初始化链。 */
	virtual void CheckDefaultInitialization() override;

	//~ End IGameFrameworkInitStateInterface interface

	// 查找指定 Actor 上的 PawnExtensionComponent
	/** 查找Pawn初始化扩展组件。
	 * @param Actor 本次操作关联的Actor。
	 * @return Pawn初始化扩展组件的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Pawn")
	static UHodgePawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor)
	{
		// Actor 有效时查找对应组件，否则直接返回 nullptr
		return (Actor ? Actor->FindComponentByClass<UHodgePawnExtensionComponent>() : nullptr);
	}

	// 获取当前 Pawn 使用的 PawnData，并转换为指定类型
	template <class T>
	/** 获取Pawn能力/输入/相机配置。
	 * @return Pawn能力/输入/相机配置的当前值或引用。
	 */
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	// 设置当前 Pawn 使用的 PawnData
	/** 设置Pawn能力/输入/相机配置。
	 * @param InPawnData 当前角色使用的PawnData配置。
	 */
	void SetPawnData(const UHodgePawnData* InPawnData);

	// 获取当前缓存的 AbilitySystemComponent
	/** 获取能力系统组件。
	 * @return 能力系统组件的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Pawn")
	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }

	// 初始化 AbilitySystem，并让当前 Pawn 成为 ASC 的 AvatarActor
	/** 建立ASC与当前Pawn的ActorInfo绑定并通知依赖组件。
	 * @param InASC 后续GAS操作使用的能力系统组件。
	 * @param InOwnerActor ASC或组件的持有Actor。
	 */
	void InitializeAbilitySystem(UHodgeAbilitySystemComponent* InASC, AActor* InOwnerActor);

	// 解除当前 Pawn 与 AbilitySystem 的 Avatar 关系
	/** 解除当前Pawn与ASC的绑定并通知依赖组件。 */
	void UninitializeAbilitySystem();

	// Pawn 的 Controller 发生变化时调用
	/** 处理控制器变化。 */
	void HandleControllerChanged();

	// Pawn 的 PlayerState 完成网络复制后调用
	/** 处理玩家状态复制。 */
	void HandlePlayerStateReplicated();

	// Pawn 的输入组件完成创建后调用
	/** 将角色输入组件绑定到既有动作与标签配置。 */
	void SetupPlayerInputComponent();

	// 注册 AbilitySystem 初始化完成回调，如果当前已经初始化则立即执行回调
	/** 处理能力SystemInitialized注册And调用。
	 * @param Delegate 委托，由调用方传入。
	 */
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	// 注册 AbilitySystem 反初始化回调
	/** 处理能力SystemUninitialized注册。
	 * @param Delegate 委托，由调用方传入。
	 */
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

	// 动态组件移除时解除自身回调，避免重新添加后遗留旧监听。
	/** 注销能力SystemDelegates。
	 * @param Subscriber Subscriber，由调用方传入。
	 */
	void UnregisterAbilitySystemDelegates(const UObject* Subscriber);

protected:
	// 组件注册时调用
	/** 组件注册后接入当前Owner的依赖。 */
	virtual void OnRegister() override;

	// Actor 开始游戏时调用
	/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
	virtual void BeginPlay() override;

	// Actor 结束游戏时调用
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param EndPlayReason 结束PlayReason，由调用方传入。
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// PawnData 网络复制发生变化时调用
	/** 收到PawnData复制后推进本地初始化。 */
	UFUNCTION()
	void OnRep_PawnData();

	// Pawn 成为 AbilitySystem 的 AvatarActor 时触发
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	// Pawn 从 AbilitySystem 的 AvatarActor 中移除时触发
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	// 当前 Pawn 使用的 PawnData，用于配置 Pawn 的基础属性和能力
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "Hodge|Pawn")
	TObjectPtr<const UHodgePawnData> PawnData;

	// 缓存当前使用的 AbilitySystemComponent，ASC 实际所有者可以是其他 Actor
	UPROPERTY(Transient)
	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;
};
