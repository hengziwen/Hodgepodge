// 111屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Core/PlayerController/HodgePlayerControllerBase.h"
#include "HodgeGameplayAbility.generated.h"


class UHodgeAbilityCost;
class UHodgeCameraMode;
class IHodgeAbilitySourceInterface;
class AHodgeCombatCharacter;
/**
 * EHodgeAbilityActivationPolicy
 *
 *	Defines how an ability is meant to activate.
 */
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
class HODGEPODGE_API UHodgeGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

	// 允许 AbilitySystemComponent 直接访问 Ability 的内部成员。
	friend class UHodgeAbilitySystemComponent;

public:
	// 初始化 HodgeGameplayAbility 默认属性。
	UHodgeGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 从 Ability 的 ActorInfo 中获取项目自定义的 AbilitySystemComponent。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	UHodgeAbilitySystemComponent* GetHodgeAbilitySystemComponentFromActorInfo() const;

	// 从 Ability 的 ActorInfo 中获取项目自定义的 PlayerController。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	AHodgePlayerControllerBase* GetHodgePlayerControllerFromActorInfo() const;

	// 从 Ability 的 ActorInfo 中获取当前 Controller。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	AController* GetControllerFromActorInfo() const;

	// 从 Ability 的 ActorInfo 中获取项目自定义的战斗角色。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	AHodgeCombatCharacter* GetHodgeCharacterFromActorInfo() const;

	// 获取 Ability 的激活策略。
	EHodgeAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	// 获取 Ability 的激活组。
	EHodgeAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	// 当 Ability 被赋予 ASC 并且已经拥有 Avatar 时，根据 OnSpawn 策略尝试激活 Ability。
	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	// Returns true if the requested activation group is a valid transition.
	// 判断当前 Ability 是否允许切换到指定的激活组。
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
		Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool CanChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup) const;

	// Tries to change the activation group.  Returns true if it successfully changed.
	// 尝试切换当前 Ability 的激活组，成功切换后返回 true。
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Hodge|Ability",
		Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool ChangeActivationGroup(EHodgeAbilityActivationGroup NewGroup);

	// 设置当前 Ability 激活期间使用的 CameraMode。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	void SetCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode);

	// 清除当前 Ability 设置的 CameraMode，Ability 结束时需要时会自动调用。
	UFUNCTION(BlueprintCallable, Category = "Hodge|Ability")
	void ClearCameraMode();

	// Ability 激活失败时同时触发 C++ 和 Blueprint 两层失败回调。
	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
	{
		// 先执行 C++ 层的失败处理逻辑。
		NativeOnAbilityFailedToActivate(FailedReason);

		// 再执行 Blueprint 层的失败事件。
		ScriptOnAbilityFailedToActivate(FailedReason);
	}

protected:
	// C++ 层处理 Ability 激活失败。
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	// Blueprint 层处理 Ability 激活失败。
	UFUNCTION(BlueprintImplementableEvent)
	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	//~UGameplayAbility interface
	// 检查 Ability 当前是否满足激活条件。
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	                                FGameplayTagContainer* OptionalRelevantTags) const override;

	// 设置当前 Ability 是否允许被取消。
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;

	// Ability 被授予 AbilitySystemComponent 时调用。
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// Ability 从 AbilitySystemComponent 中被移除时调用。
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// Ability 真正开始执行时调用。
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	// Ability 结束执行时调用，可以区分正常结束和被取消。
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	// 检查 Ability 的基础 Cost 是否能够支付。
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	// 应用 Ability 的基础 Cost 和项目自定义的 AdditionalCosts。
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo) const override;

	// 创建 Ability 使用的 GameplayEffectContext，并允许项目加入自定义上下文数据。
	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
	                                                       const FGameplayAbilityActorInfo* ActorInfo) const override;

	// 将当前 Ability 的标签写入 GameplayEffectSpec，使 Effect 可以知道是谁施加的。
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
	                                                  FGameplayAbilitySpec* AbilitySpec) const override;

	// 检查 Ability 的 Required / Blocked GameplayTag 是否满足激活要求。
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	                                               const FGameplayTagContainer* SourceTags = nullptr,
	                                               const FGameplayTagContainer* TargetTags = nullptr,
	                                               OUT FGameplayTagContainer* OptionalRelevantTags = nullptr)
	const override;
	//~End of UGameplayAbility interface

	// 当 AbilitySystem 初始化完成并设置 Pawn Avatar 后调用。
	virtual void OnPawnAvatarSet();

	// 获取 Ability 的来源对象、来源等级以及 Effect Causer。
	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              float& OutSourceLevel, const IHodgeAbilitySourceInterface*& OutAbilitySource,
	                              AActor*& OutEffectCauser) const;

	/** Called when this ability is granted to the ability system component. */
	// Ability 被授予 ASC 后触发的 Blueprint 事件。
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	/** Called when this ability is removed from the ability system component. */
	// Ability 从 ASC 移除后触发的 Blueprint 事件。
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	/** Called when the ability system is initialized with a pawn avatar. */
	// AbilitySystem 使用 Pawn 作为 Avatar 初始化完成后触发的 Blueprint 事件。
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

	// 当前 Ability 设置的 CameraMode。
	TSubclassOf<UHodgeCameraMode> ActiveCameraMode;
};
