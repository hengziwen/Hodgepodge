/** @file HodgeInteractionTestTypes.h @brief 交互回归使用的真实物理目标与可观察提交计数。 */
#pragma once

#include "GameFramework/Actor.h"
#include "Character/HodgeHeroCharacter.h"
#include "Interaction/HodgeInteractableTarget.h"
#include "HodgeInteractionTestTypes.generated.h"

class USphereComponent;

/** 通过虚函数注入非法视点，不向引擎 Transform 写入 NaN 或修改生产校验。 */
UCLASS(Transient, NotBlueprintable)
class AHodgeInteractionTestHero : public AHodgeHeroCharacter
{
	GENERATED_BODY()
public:
	AHodgeInteractionTestHero(const FObjectInitializer& ObjectInitializer);
	virtual FVector GetPawnViewLocation() const override;
	bool bInvalidViewLocation = false;
};

/** 仅提供目标契约，距离、身份与遮挡仍由生产交互组件判断。 */
UCLASS(Transient, NotBlueprintable)
class AHodgeInteractionTestTarget : public AActor, public IHodgeInteractableTarget
{
	GENERATED_BODY()
public:
	AHodgeInteractionTestTarget();
	virtual bool GetInteractionOption(const APawn* RequestingPawn, FHodgeInteractionOption& OutOption) const override;
	virtual FHodgeInteractionResult ExecuteInteraction(APawn* RequestingPawn, const FGuid& ExpectedRunId, int32 ExpectedRevision) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** PIE 工具只读当前复制快照，不另开目标执行捷径。 */
	UFUNCTION(BlueprintPure) FHodgeInteractionOption GetOptionSnapshot() const;
	UFUNCTION(BlueprintPure) int32 GetSuccessfulExecutions() const;
	UFUNCTION(BlueprintPure) int32 GetExecuteAttempts() const;

	/** 只统计实际到达目标提交入口的次数，便于区分组件拒绝与目标拒绝。 */
	int32 ExecuteAttempts = 0;
	UPROPERTY(Replicated) int32 SuccessfulExecutions = 0;
	UPROPERTY(Replicated) bool bOptionAvailable = true;
	bool bRejectExecution = false;
	UPROPERTY(Replicated) FHodgeInteractionOption Option;
	TWeakObjectPtr<APawn> LastExecutingPawn;
	UPROPERTY(VisibleAnywhere) TObjectPtr<USphereComponent> InteractionShape;
};
