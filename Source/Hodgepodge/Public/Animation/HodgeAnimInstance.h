/** @file HodgeAnimInstance.h
 * @brief 声明角色动画数据实例相关的类型、接口与现有处理流程。
 */
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
	/** 构造角色动画数据实例并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer);

	// 将动画实例与角色的 AbilitySystemComponent 绑定，使动画可以读取 GAS 状态
	/** 初始化关联能力System。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 */
	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
	/** Sprint 的单一 Turn 按时间推进，不要求 Distance 曲线。 */
	/** 更新Sprint奔跑掉头。
	 * @param Context 本次操作的上下文。
	 * @param Node 节点，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Animation", meta=(BlueprintThreadSafe))
	bool UpdateSprintPivot(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	/** 更新Sprint奔跑循环。
	 * @param Context 本次操作的上下文。
	 * @param Node 节点，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Animation", meta=(BlueprintThreadSafe))
	bool UpdateSprintCycle(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	/** 判断是否应退出Sprint奔跑掉头。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Animation", meta=(BlueprintThreadSafe))
	bool ShouldExitSprintPivot();
	/** 推进转身动画的播放时间。
	 * @param Time 当前处理使用的时间值。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param Length Length，由调用方传入。
	 * @param Rate 速率，由调用方传入。
	 * @return 本次计算、查询或创建得到的浮点值。
	 */
	static float AdvanceTurnTime(float Time, float DeltaTime, float Length, float Rate);
	/** 判断转向Complete。
	 * @param Elapsed 本次等待或执行已经经过的时间。
	 * @param Length Length，由调用方传入。
	 * @param Rate 速率，由调用方传入。
	 * @param BlendOut 混合输出，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	static bool IsTurnComplete(float Elapsed, float Length, float Rate, float BlendOut);
	/** 根据当前速度与动画配置计算循环播放倍率。
	 * @param Speed 速度，由调用方传入。
	 * @param ReferenceSpeed 引用速度，由调用方传入。
	 * @param BaseRate 基础速率，由调用方传入。
	 * @param Minimum 最小，由调用方传入。
	 * @param Maximum 最大，由调用方传入。
	 * @return 本次计算、查询或创建得到的浮点值。
	 */
	static float MatchCycleRate(float Speed, float ReferenceSpeed, float BaseRate, float Minimum, float Maximum);
	/** 解析脚部IK混合权重。
	 * @param FullBodyWeight Full体状态Weight，由调用方传入。
	 * @param DashWeight Dash冲刺Weight，由调用方传入。
	 * @return 本次计算、查询或创建得到的浮点值。
	 */
	static float ResolveFootIKAlpha(float FullBodyWeight, float DashWeight);
	/** 解析循环同步分组。
	 * @param bSprint 是否Sprint奔跑的控制标记。
	 * @return 本次计算、查询或创建得到的名称。
	 */
	static FName ResolveCycleSyncGroup(bool bSprint);

protected:
#if WITH_EDITOR
	// 编辑器中验证动画实例配置是否正确
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

	// 动画实例初始化时调用，适合获取 Owner、Character 等基础引用
	/** 执行原生初始化动画。 */
	virtual void NativeInitializeAnimation() override;

	// 每帧更新动画实例数据，供动画蓝图读取最新角色状态
	/** 在游戏线程采集当前角色的动画表现数据。
	 * @param DeltaSeconds 本次更新的时间步长，单位秒。
	 */
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

	// 控制是否重置移动表现Yaw角的状态标记。
	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	bool bResetLocomotionYaw = false;

	// 移动表现根节点Yaw角缩放的数值或阈值。
	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float LocomotionRootYawScale = 1.f;
	/** Dash／根运动 Pivot 保留脚部贴地；其他全身动作按权重退出 IK。 */
	UPROPERTY(BlueprintReadOnly, Transient, Category="Character State Data")
	float LocomotionFootIKAlpha = 1.f;

	// 朝向表现的数据或对象引用。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
	FHodgeFacingPresentationSnapshot FacingPresentation;
	// 控制是否UseStrafe移动表现的状态标记。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
	bool bUseStrafeLocomotion = false;
	// Strafe移动表现Weight的数值或阈值。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
	float StrafeLocomotionWeight = 0.f;
	// 控制是否Allow移动表现掉头的状态标记。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data")
	bool bAllowLocomotionPivot = false;
	// 朝向Mode混合时间的数值或阈值。
	UPROPERTY(EditDefaultsOnly, Category="Character State Data", meta=(ClampMin="0", Units="s"))
	float FacingModeBlendTime = .12f;
	// 控制是否Sprinting的状态标记。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") bool bSprinting = false;
	// 控制是否Sprint奔跑掉头允许的状态标记。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") bool bSprintPivotAllowed = false;
	// Sprint奔跑循环的数据或对象引用。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") TObjectPtr<class UAnimSequence> SprintCycle;
	// Sprint奔跑转向的数据或对象引用。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") TObjectPtr<class UAnimSequence> SprintTurn;
	// Sprint奔跑转向Play速率的数值或阈值。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") float SprintTurnPlayRate = 1.f;
	// Sprint奔跑转向混合输出时间的数值或阈值。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") float SprintTurnBlendOutTime = .12f;
	// Sprint奔跑循环Play速率的数值或阈值。
	UPROPERTY(BlueprintReadOnly, Category="Character State Data") float SprintCyclePlayRate = 1.f;
};
