// 111 屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "HodgePawnExtensionComponent.generated.h"

class UHodgePawnData;
class UHodgeAbilitySystemComponentBase;

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
	UHodgePawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	// 当前 PawnExtensionComponent 在 InitState 系统中的功能名称
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface

	// 返回当前组件对应的 InitState Feature 名称
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }

	// 判断组件是否允许从当前初始化状态切换到目标状态
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                FGameplayTag DesiredState) const override;

	// 执行从当前初始化状态切换到目标状态时的具体逻辑
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	                                   FGameplayTag DesiredState) override;

	// 当所属 Actor 的 InitState 发生变化时通知该组件
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;

	// 检查并推进当前组件的默认初始化流程
	virtual void CheckDefaultInitialization() override;

	//~ End IGameFrameworkInitStateInterface interface

	// 查找指定 Actor 上的 PawnExtensionComponent
	UFUNCTION(BlueprintPure, Category = "Hodge|Pawn")
	static UHodgePawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor)
	{
		// Actor 有效时查找对应组件，否则直接返回 nullptr
		return (Actor ? Actor->FindComponentByClass<UHodgePawnExtensionComponent>() : nullptr);
	}

	// 获取当前 Pawn 使用的 PawnData，并转换为指定类型
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	// 设置当前 Pawn 使用的 PawnData
	void SetPawnData(const UHodgePawnData* InPawnData);

	// 获取当前缓存的 AbilitySystemComponent
	UFUNCTION(BlueprintPure, Category = "Hodge|Pawn")
	UHodgeAbilitySystemComponentBase* GetHodgeAbilitySystemComponent() const { return AbilitySystemComponent; }

	// 初始化 AbilitySystem，并让当前 Pawn 成为 ASC 的 AvatarActor
	void InitializeAbilitySystem(UHodgeAbilitySystemComponentBase* InASC, AActor* InOwnerActor);

	// 解除当前 Pawn 与 AbilitySystem 的 Avatar 关系
	void UninitializeAbilitySystem();

	// Pawn 的 Controller 发生变化时调用
	void HandleControllerChanged();

	// Pawn 的 PlayerState 完成网络复制后调用
	void HandlePlayerStateReplicated();

	// Pawn 的输入组件完成创建后调用
	void SetupPlayerInputComponent();

	// 注册 AbilitySystem 初始化完成回调，如果当前已经初始化则立即执行回调
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	// 注册 AbilitySystem 反初始化回调
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:
	// 组件注册时调用
	virtual void OnRegister() override;

	// Actor 开始游戏时调用
	virtual void BeginPlay() override;

	// Actor 结束游戏时调用
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// PawnData 网络复制发生变化时调用
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
	TObjectPtr<UHodgeAbilitySystemComponentBase> AbilitySystemComponent;
};
