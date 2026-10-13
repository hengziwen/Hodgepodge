/** @file HodgeGameplayCue_Movement.h
 * @brief 声明玩法GameplayCue移动相关的类型、接口与现有处理流程。
 */
#pragma once

#include "GameplayCueNotify_Actor.h"
#include "HodgeGameplayCue_Movement.generated.h"
class UNiagaraSystem;
class UNiagaraComponent;

/** 可配置的移动表现；不修改体力、防御、Actor 变换或速度。 */
UCLASS(Blueprintable)
class HODGEPODGE_API AHodgeGameplayCue_Movement : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
	/** 构造玩法GameplayCue移动并初始化其默认状态。 */
	AHodgeGameplayCue_Movement();
	// 效果的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UNiagaraSystem> Effect;
	/** 处理活动。
	 * @param Target 本次操作的目标。
	 * @param Parameters 调用方或测试框架传入的参数。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
	/** 在GameplayCue活动期间更新对应表现。
	 * @param Target 本次操作的目标。
	 * @param Parameters 调用方或测试框架传入的参数。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool WhileActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
	/** 处理移除。
	 * @param Target 本次操作的目标。
	 * @param Parameters 调用方或测试框架传入的参数。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
	/** 处理执行。
	 * @param Target 本次操作的目标。
	 * @param Parameters 调用方或测试框架传入的参数。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
private:
	// 活动效果的数据或对象引用。
	UPROPERTY(Transient) TObjectPtr<UNiagaraComponent> ActiveEffect;
};
