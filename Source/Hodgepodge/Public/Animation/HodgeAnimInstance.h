// 111 屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "Animation/AnimExecutionContext.h"
#include "Animation/AnimNodeReference.h"
#include "HodgeAnimInstance.generated.h"

/**
 * 角色动画实例基类，负责将角色的 Gameplay 状态同步给动画系统
 */
UCLASS(Config = Game)
class HODGEPODGE_API UHodgeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 构造动画实例并进行默认属性初始化
	UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer);

	// 将动画实例与角色的 AbilitySystemComponent 绑定，使动画可以读取 GAS 状态
	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
	/** Sprint 的单一 Turn 按时间推进，不要求 Distance 曲线。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Animation", meta=(BlueprintThreadSafe))
	bool UpdateSprintPivot(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, Category="Hodge|Animation", meta=(BlueprintThreadSafe))
	bool UpdateSprintCycle(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintPure, Category="Hodge|Animation", meta=(BlueprintThreadSafe))
	bool ShouldExitSprintPivot();
	static float AdvanceTurnTime(float Time, float DeltaTime, float Length, float Rate);
	static bool IsTurnComplete(float Elapsed, float Length, float Rate, float BlendOut);
	static float MatchCycleRate(float Speed, float ReferenceSpeed, float BaseRate, float Minimum, float Maximum);
	static float ResolveFootIKAlpha(float FullBodyWeight, float DashWeight);
	static FName ResolveCycleSyncGroup(bool bSprint);

protected:
#if WITH_EDITOR
	// 编辑器中验证动画实例配置是否正确
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

	// 动画实例初始化时调用，适合获取 Owner、Character 等基础引用
	virtual void NativeInitializeAnimation() override;

	// 每帧更新动画实例数据，供动画蓝图读取最新角色状态
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 将 GameplayTag 自动映射到动画蓝图变量，Tag 添加或移除时会自动更新对应变量
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	// 角色与地面的距离，-1 表示当前还没有有效的地面距离数据
	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;

	// 游戏线程缓存旋转策略，动画线程只读取这份表现快照。
	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	bool bSuppressLocomotionYaw = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	bool bResetLocomotionYaw = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float LocomotionRootYawScale = 1.f;
	/** Dash／根运动 Pivot 保留脚部贴地；其他全身动作按权重退出 IK。 */
	UPROPERTY(BlueprintReadOnly, Transient, Category="Character State Data")
	float LocomotionFootIKAlpha = 1.f;

	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
	FHodgeFacingPresentationSnapshot FacingPresentation;
	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
	bool bUseStrafeLocomotion = false;
	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
	float StrafeLocomotionWeight = 0.f;
	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
	bool bAllowLocomotionPivot = false;
	UPROPERTY(EditDefaultsOnly, Category="Character State Data", meta=(ClampMin="0", Units="s"))
	float FacingModeBlendTime = .12f;
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") bool bSprinting = false;
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") bool bSprintPivotAllowed = false;
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") TObjectPtr<class UAnimSequence> SprintCycle;
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") TObjectPtr<class UAnimSequence> SprintTurn;
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") float SprintTurnPlayRate = 1.f;
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") float SprintTurnBlendOutTime = .12f;
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") float SprintCyclePlayRate = 1.f;
};
