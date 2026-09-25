// 111屎山代码来袭

#pragma once

// UE 核心类型与基础宏。
#include "CoreMinimal.h"

// Modular Gameplay 中的 GameFrameworkComponent 基类。
#include "Components/GameFrameworkComponent.h"

#include "HodgeHealthComponent.generated.h"


// 项目自定义 AbilitySystemComponent 前置声明。
class UHodgeAbilitySystemComponent;

// 项目生命属性集前置声明。
class UHodgeHealthSet;

// UObject 前置声明。
class UObject;

// 蓝图 VM 调用栈结构前置声明。
struct FFrame;

// GameplayEffectSpec 前置声明，用于传递造成属性变化的 GameplayEffect 信息。
struct FGameplayEffectSpec;

// 死亡事件动态多播委托。
// OwningActor 表示当前 HealthComponent 所属 Actor。
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeHealth_DeathEvent, AActor*, OwningActor);

// 生命属性变化动态多播委托。
// HealthComponent 表示发生变化的生命组件。
// OldValue / NewValue 表示属性修改前后的数值。
// Instigator 表示引发本次属性变化的 Actor。
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHodgeHealth_AttributeChanged, UHodgeHealthComponent*, HealthComponent,
                                              float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * EHodgeDeathState
 *
 *	Defines current state of death.
 *
 * 表示角色当前死亡流程所处的阶段。
 * 将死亡拆分为“未死亡 → 开始死亡 → 完成死亡”，
 * 方便死亡动画、输入禁用、尸体处理、销毁等逻辑分阶段执行。
 */
UENUM(BlueprintType)
enum class EHodgeDeathState : uint8
{
	// 当前角色正常存活，没有进入死亡流程。
	NotDead = 0,

	// 已经开始死亡流程，但死亡流程尚未完全结束。
	DeathStarted,

	// 死亡流程已经全部完成。
	DeathFinished
};


/**
 * UHodgeHealthComponent
 *
 *	An actor component used to handle anything related to health.
 *
 * 角色生命系统的逻辑组件。
 *
 * HealthSet 负责保存 Health / MaxHealth 等 GAS Attribute，
 * HealthComponent 则负责监听这些 Attribute 的变化，
 * 并在此基础上组织生命值事件、死亡状态和死亡流程。
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	// 构造 HealthComponent。
	UHodgeHealthComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	// 从指定 Actor 上查找 UHodgeHealthComponent，不存在时返回 nullptr。
	UFUNCTION(BlueprintPure, Category = "Hodge|Health")
	static UHodgeHealthComponent* FindHealthComponent(const AActor* Actor)
	{
		// Actor 有效时通过 FindComponentByClass 查找 HealthComponent。
		return (Actor ? Actor->FindComponentByClass<UHodgeHealthComponent>() : nullptr);
	}

	// Initialize the component using an ability system component.
	// 使用指定 ASC 初始化 HealthComponent，并建立与 HealthSet / 属性事件之间的绑定关系。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	// 解除 HealthComponent 与 ASC / HealthSet 的绑定并清理相关引用。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
	void UninitializeFromAbilitySystem();

	// Returns the current health value.
	// 获取 HealthSet 当前生命值。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
	float GetHealth() const;

	// Returns the current maximum health value.
	// 获取 HealthSet 当前最大生命值。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
	float GetMaxHealth() const;

	// Returns the current health in the range [0.0, 1.0].
	// 获取归一化生命值，通常表示为 Health / MaxHealth，并限制在 [0.0, 1.0] 范围。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
	float GetHealthNormalized() const;

	// 获取当前死亡流程状态。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Health")
	EHodgeDeathState GetDeathState() const { return DeathState; }

	// 判断当前角色是否已经死亡或正在执行死亡流程。
	// ExpandBoolAsExecs 允许蓝图根据 bool 返回值直接展开为不同执行引脚。
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Health",
		Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EHodgeDeathState::NotDead); }

	// Begins the death sequence for the owner.
	// 开始所属 Actor 的死亡流程，将死亡状态推进到 DeathStarted。
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	// 完成所属 Actor 的死亡流程，将死亡状态推进到 DeathFinished。
	virtual void FinishDeath();

	// Applies enough damage to kill the owner.
	// 对自身施加足以致死的伤害，用于自毁、掉出世界等需要主动结束角色生命的场景。
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

public:
	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	// Health 发生变化时广播；客户端通过属性复制收到变化时 Instigator 可能为空或无效。
	UPROPERTY(BlueprintAssignable)
	FHodgeHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	// MaxHealth 发生变化时广播；客户端通过属性复制收到变化时 Instigator 可能为空或无效。
	UPROPERTY(BlueprintAssignable)
	FHodgeHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	// 死亡流程开始时广播。
	UPROPERTY(BlueprintAssignable)
	FHodgeHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	// 死亡流程全部完成时广播。
	UPROPERTY(BlueprintAssignable)
	FHodgeHealth_DeathEvent OnDeathFinished;

protected:
	// Component 从所属 Actor 注销时调用，用于执行组件生命周期结束前的清理。
	virtual void OnUnregister() override;

	// 清理 HealthComponent 负责维护的死亡等 GameplayTag 状态。
	void ClearGameplayTags();

	// HealthSet 的 Health 发生变化时的内部处理函数。
	// 将 AttributeSet 层的属性事件转换为 HealthComponent 对外暴露的生命值变化事件。
	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                 const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                                 float NewValue);

	// HealthSet 的 MaxHealth 发生变化时的内部处理函数。
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
	                                    const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude,
	                                    float OldValue, float NewValue);

	// HealthSet 检测到 Health 耗尽时调用，用于触发死亡流程相关逻辑。
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
	                               const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue,
	                               float NewValue);

	// DeathState 从服务器复制到客户端后触发。
	// OldDeathState 用于和最新 DeathState 比较，从而补执行客户端缺失的死亡阶段事件。
	UFUNCTION()
	virtual void OnRep_DeathState(EHodgeDeathState OldDeathState);

protected:
	// Ability system used by this component.
	// 当前 HealthComponent 使用的 ASC，用于访问 AttributeSet、GameplayTag 和 GameplayEffect 等 GAS 功能。
	UPROPERTY()
	TObjectPtr<UHodgeAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	// 当前 HealthComponent 监听的 HealthSet。
	// HealthSet 的 Attribute 负责保存数值，HealthComponent 负责将数值变化转换为上层生命 / 死亡逻辑。
	UPROPERTY()
	TObjectPtr<const UHodgeHealthSet> HealthSet;

	// Replicated state used to handle dying.
	// 当前死亡流程状态，由服务器进行权威修改并复制到客户端。
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EHodgeDeathState DeathState;
};
