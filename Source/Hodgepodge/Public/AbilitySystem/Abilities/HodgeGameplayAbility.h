/** @file HodgeGameplayAbility.h
 * @brief 声明GameplayAbility相关的类型、接口与现有处理流程。
 */
// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "Camera/HodgeCinematicCameraTypes.h"
#include "HodgeGameplayAbility.generated.h"


class UHodgeHeroComponent;
class UHodgeAbilityCost;
class UHodgeCameraMode;
class IHodgeAbilitySourceInterface;
class AHodgeCombatCharacter;
/**
 * EHodgeAbilityActivationPolicy
 *
 *	Defines how an ability is meant to activate.
 */
/** 定义能力Activation策略的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeAbilityActivationPolicy : uint8
{
	// 输入触发时尝试激活一次 Ability。
	OnInputTriggered,

	// 只要输入保持激活状态，就持续尝试激活 Ability。
	WhileInputActive,

	// Pawn Avatar 被设置后尝试自动激活 Ability。
	OnSpawn
};


/**
 * EHodgeAbilityActivationGroup
 *
 *	Defines how an ability activates in relation to other abilities.
 */
/** 定义能力Activation分组的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeAbilityActivationGroup : uint8
{
	// 独立运行，不会受到其他 Ability 的 Exclusive 组限制。
	Independent,

	// 当前 Ability 可以被其他 Exclusive Ability 取消并替换。
	Exclusive_Replaceable,

	// 当前 Ability 激活后，会阻止其他 Exclusive Ability 激活。
	Exclusive_Blocking,

	// 枚举数量标记，不作为实际的 Ability 激活组使用。
	MAX UMETA(Hidden)
};

/** Failure reason that can be used to play an animation montage when a failure occurs */
/** 保存能力蒙太奇失败消息的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgeAbilityMontageFailureMessage
{
	GENERATED_BODY()

public:
	// Ability 激活失败时对应的 PlayerController，仅当 AbilitySystemComponent 属于玩家时有效。
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	// 导致 Ability 激活失败的 Avatar Actor。
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> AvatarActor = nullptr;

	// Ability 激活失败的所有原因 GameplayTag。
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer FailureTags;

	// 根据失败原因需要播放的失败提示动画。
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAnimMontage> FailureMontage = nullptr;
};

/**
 * UHodgeGameplayAbility
 *
 *	项目使用的 GameplayAbility 基类。
 *
 * 这个类在 UGameplayAbility 的基础上增加项目自己的 Ability 规则，
 * 包括激活策略、激活组、额外 Cost、失败反馈、CameraMode 和 AbilitySource 等。
 *
 * UHodgeGameplayAbility
 *
 *	The base gameplay ability class used by this project.
 */
UCLASS(Abstract, HideCategories = Input,
	Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
/** 封装GameplayAbility的状态与操作接口。 */
class HODGEPODGE_API UHodgeGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

	// 允许 AbilitySystemComponent 直接访问 Ability 的内部成员。
	friend class UHodgeAbilitySystemComponent;

public:
	// 初始化 HodgeGameplayAbility 默认属性。
	/** 构造GameplayAbility并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	// 控制是否RequiresInitializedAttributes的状态标记。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Attributes") bool bRequiresInitializedAttributes = false;
	// 控制是否Allow持续期间命中Reacting的状态标记。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat") bool bAllowWhileHitReacting = false;

	// 从 Ability 的 ActorInfo 中获取项目自定义的 AbilitySystemComponent。
	/** 获取能力System组件来源Actor对象信息。
	 * @return 能力System组件来源Actor对象信息的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponentFromActorInfo() const;

	// 从 Ability 的 ActorInfo 中获取项目自定义的 PlayerController。
	/** 获取玩家控制器来源Actor对象信息。
	 * @return 玩家控制器来源Actor对象信息的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	AHodgePlayerController* GetHodgePlayerControllerFromActorInfo() const;

	// 从 Ability 的 ActorInfo 中获取当前 Controller。
	/** 获取控制器来源Actor对象信息。
	 * @return 控制器来源Actor对象信息的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	AController* GetControllerFromActorInfo() const;

	// 从 Ability 的 ActorInfo 中获取项目自定义的战斗角色。
	/** 获取角色来源Actor对象信息。
	 * @return 角色来源Actor对象信息的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	AHodgeCombatCharacter* GetHodgeCharacterFromActorInfo() const;

	// 从 Ability 的 ActorInfo 中获取项目自定义的HeroComponent
	/** 获取玩家角色组件来源Actor对象信息。
	 * @return 玩家角色组件来源Actor对象信息的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	UHodgeHeroComponent* GetHeroComponentFromActorInfo() const;

	// 获取 Ability 的激活策略。
	/** 获取Activation策略。
	 * @return Activation策略的当前值或引用。
	 */
	EHodgeAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	// 获取 Ability 的激活组。
	/** 获取Activation分组。
	 * @return Activation分组的当前值或引用。
	 */
	EHodgeAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	// 当 Ability 被赋予 ASC 并且已经拥有 Avatar 时，根据 OnSpawn 策略尝试激活 Ability。
	/** 尝试Activate能力事件生成。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param Spec 规格，由调用方传入。
	 */
	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	// Returns true if the requested activation group is a valid transition.
	// 判断当前 Ability 是否允许切换到指定的激活组。
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
		Meta = (ExpandBoolAsExecs = "ReturnValue"))
	/** 检查是否允许变更Activation分组。
	 * @param NewGroup 新分组，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const;

	// Tries to change the activation group.  Returns true if it successfully changed.
	// 尝试切换当前 Ability 的激活组，成功切换后返回 true。
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
		Meta = (ExpandBoolAsExecs = "ReturnValue"))
	/** 按现有互斥约束切换该能力的激活分组。
	 * @param NewGroup 新分组，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup);

	// 设置当前 Ability 激活期间使用的 CameraMode。
	/** 设置相机模式与混合栈。
	 * @param CameraMode 相机模式与混合栈，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	void SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode);

	// 清除当前 Ability 设置的 CameraMode，Ability 结束时需要时会自动调用。
	/** 清空相机模式与混合栈。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	void ClearCameraMode();

	// Ability 激活失败时同时触发 C++ 和 Blueprint 两层失败回调。
	/** 处理能力Failed目标Activate。
	 * @param FailedReason FailedReason，用于对应标签查询或匹配。
	 */
	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
	{
		// 先执行 C++ 层的失败处理逻辑。
		NativeOnAbilityFailedToActivate(FailedReason);

		// 再执行 Blueprint 层的失败事件。
		ScriptOnAbilityFailedToActivate(FailedReason);
	}

protected:
	// C++ 层处理 Ability 激活失败。
	/** 执行原生事件能力Failed目标Activate。
	 * @param FailedReason FailedReason，用于对应标签查询或匹配。
	 */
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	// Blueprint 层处理 Ability 激活失败。
	/** 把技能激活失败交给现有蓝图反馈入口。
	 * @param FailedReason FailedReason，用于对应标签查询或匹配。
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	//~UGameplayAbility interface
	// 检查 Ability 当前是否满足激活条件。
	/** 检查技能激活所需的上下文、标签与现有GAS限制。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param SourceTags 来源标签，用于对应标签查询或匹配。
	 * @param TargetTags 目标标签，用于对应标签查询或匹配。
	 * @param OptionalRelevantTags OptionalRelevant标签，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	                                FGameplayTagContainer* OptionalRelevantTags) const override;

	// 设置当前 Ability 是否允许被取消。
	/** 设置允许BeCanceled。
	 * @param bCanBeCanceled 是否允许BeCanceled的控制标记。
	 */
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;

	// Ability 被授予 AbilitySystemComponent 时调用。
	/** 处理授予能力。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param Spec 规格，由调用方传入。
	 */
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// Ability 从 AbilitySystemComponent 中被移除时调用。
	/** 处理移除能力。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param Spec 规格，由调用方传入。
	 */
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// Ability 真正开始执行时调用。
	/** 启动本次技能，按现有配置提交消耗并建立执行资源。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param TriggerEventData Trigger事件数据，由调用方传入。
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	// Ability 结束执行时调用，可以区分正常结束和被取消。
	/** 结束本次技能并释放其拥有的资源，再交还GAS生命周期。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 * @param bReplicateEndAbility 是否Replicate结束能力的控制标记。
	 * @param bWasCancelled 是否Was已取消的控制标记。
	 */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	// 检查 Ability 的基础 Cost 是否能够支付。
	/** 检查消耗。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * OUT参数；用于保持该接口的调用签名。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	// 应用 Ability 的基础 Cost 和项目自定义的 AdditionalCosts。
	/** 应用消耗。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param ActivationInfo 本次技能激活与预测信息。
	 */
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo) const override;

	// 创建 Ability 使用的 GameplayEffectContext，并允许项目加入自定义上下文数据。
	/** 构造效果上下文。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @return 本次操作的句柄，调用方应按现有接口检查有效性。
	 */
	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
	                                                       const FGameplayAbilityActorInfo* ActorInfo) const override;

	// 将当前 Ability 的标签写入 GameplayEffectSpec，使 Effect 可以知道是谁施加的。
	/** 应用能力标签目标玩法效果规格。
	 * @param Spec 规格，由调用方传入。
	 * @param AbilitySpec 能力规格，由调用方传入。
	 */
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
	                                                  FGameplayAbilitySpec* AbilitySpec) const override;

	// 检查 Ability 的 Required / Blocked GameplayTag 是否满足激活要求。
	/** 按GAS标签条件检查技能是否允许执行。
	 * @param AbilitySystemComponent 能力系统组件，由调用方传入。
	 * @param SourceTags 来源标签，用于对应标签查询或匹配。
	 * @param TargetTags 目标标签，用于对应标签查询或匹配。
	 * OUT参数；用于保持该接口的调用签名。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	                                               const FGameplayTagContainer* SourceTags = nullptr,
	                                               const FGameplayTagContainer* TargetTags = nullptr,
	                                               OUT FGameplayTagContainer* OptionalRelevantTags = nullptr)
	const override;
	//~End of UGameplayAbility interface

	// 当 AbilitySystem 初始化完成并设置 Pawn Avatar 后调用。
	/** 处理Pawn角色表现角色设置。 */
	virtual void OnPawnAvatarSet();

	// 获取 Ability 的来源对象、来源等级以及 Effect Causer。
	/** 获取能力来源。
	 * @param Handle 对应操作或能力的句柄。
	 * @param ActorInfo 本次GAS调用关联的ASC、Owner与Avatar信息。
	 * @param OutSourceLevel 输出来源等级。
	 * @param OutAbilitySource 输出能力来源。
	 * @param OutEffectCauser 输出效果Causer。
	 */
	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              float& OutSourceLevel, const IHodgeAbilitySourceInterface*& OutAbilitySource,
	                              AActor*& OutEffectCauser) const;

	/** Called when this ability is granted to the ability system component. */
	// Ability 被授予 ASC 后触发的 Blueprint 事件。
	/** 触发蓝图能力Added。 */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	/** Called when this ability is removed from the ability system component. */
	// Ability 从 ASC 移除后触发的 Blueprint 事件。
	/** 触发蓝图能力Removed。 */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	/** Called when the ability system is initialized with a pawn avatar. */
	// AbilitySystem 使用 Pawn 作为 Avatar 初始化完成后触发的 Blueprint 事件。
	/** 触发蓝图Pawn角色表现角色设置。 */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();

protected:
	// 定义 Ability 应该通过什么方式激活。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Ability Activation")
	EHodgeAbilityActivationPolicy ActivationPolicy;

	// 定义当前 Ability 与其他 Ability 激活之间的互斥关系。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Ability Activation")
	EHodgeAbilityActivationGroup ActivationGroup;

	// 激活该 Ability 时除了基础 Cost 外还需要支付的额外消耗。
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
	TArray<TObjectPtr<UHodgeAbilityCost>> AdditionalCosts;

	// 将失败 GameplayTag 映射为给玩家显示的简单错误文本。
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	TMap<FGameplayTag, FText> FailureTagToUserFacingMessages;

	// 将失败 GameplayTag 映射为发生该失败时需要播放的动画。
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;

	// 是否在 Ability 被取消时额外输出日志，目前主要用于追踪和定位 Bug。
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	bool bLogCancelation;

	/** 相机申请的完整身份，结束时不能仅凭Spec清理新会话。 */
	FHodgeAbilityCameraHandle ActiveCameraHandle;
	// 当前 Ability 设置的 CameraMode。
	TSubclassOf<UHodgeCameraMode> ActiveCameraMode;
};
