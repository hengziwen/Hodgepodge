/** @file HodgeDefenseComponent.h
 * @brief 声明防御窗口组件相关的类型、接口与现有处理流程。
 */
#pragma once

#include "Components/PawnComponent.h"
#include "GameplayEffectTypes.h"
#include "HodgeDefenseComponent.generated.h"

class UHodgeAbilitySystemComponent;
class UHodgeSprintAbilityProfile;
class UGameplayAbility;
/** 定义Incoming命中Outcome的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeIncomingHitOutcome : uint8 { /* 允许提交本次命中。 */ Allowed, /* 本次命中被闪避窗口拒绝。 */ Dodged, /* 满足完美闪避窗口的命中。 */ PerfectDodge };

/** 在伤害及附带控制提交前解析闪避；动画和 Cue 不拥有防御权限。 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeDefenseComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	/** 初始化关联能力System。
	 * @param InASC 后续GAS操作使用的能力系统组件。
	 */
	void InitializeWithAbilitySystem(UHodgeAbilitySystemComponent* InASC);
	/** 解除初始化来源能力System。 */
	void UninitializeFromAbilitySystem();
	/** 注册闪避窗口。
	 * @param Source 本次操作的来源。
	 * @param Execution 执行，由调用方传入。
	 * @param Profile 当前操作使用的配置对象。
	 * @param StartTime 起始时间，由调用方传入。
	 * @return 本次计算、查询或创建得到的唯一标识。
	 */
	FGuid RegisterDodgeWindow(UGameplayAbility* Source, FGuid Execution, const UHodgeSprintAbilityProfile* Profile, double StartTime);
	/** 注销闪避窗口。
	 * @param Handle 对应操作或能力的句柄。
	 */
	void UnregisterDodgeWindow(FGuid Handle);
	/** 在伤害和Impact提交前判定当前防御窗口。
	 * @param Source 本次操作的来源。
	 * @param AttackExecution 攻击执行，由调用方传入。
	 * @param HitIndex 命中索引，由调用方传入。
	 * @param bCanBeDodged 是否允许BeDodged的控制标记。
	 * @param bCanTriggerPerfect 是否允许Trigger完美闪避的控制标记。
	 * @return 本次处理得到的状态枚举。
	 */
	EHodgeIncomingHitOutcome ResolveIncomingHit(AActor* Source, FGuid AttackExecution, int32 HitIndex, bool bCanBeDodged, bool bCanTriggerPerfect);
	/** 计算窗口。
	 * @param Profile 当前操作使用的配置对象。
	 * @param Age Age，由调用方传入。
	 * @param bRewarded 是否Rewarded的控制标记。
	 * @param bCanBeDodged 是否允许BeDodged的控制标记。
	 * @param bPerfectEligible 是否完美闪避Eligible的控制标记。
	 * @return 本次处理得到的状态枚举。
	 */
	static EHodgeIncomingHitOutcome EvaluateWindow(const UHodgeSprintAbilityProfile& Profile, double Age, bool bRewarded, bool bCanBeDodged, bool bPerfectEligible);
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param Reason Reason，由调用方传入。
	 */
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
private:
	/** 保存窗口的数据和接口契约。 */
	struct FWindow
	{
		// 本次操作的来源。
		TWeakObjectPtr<UGameplayAbility> Source;
		// 当前操作使用的配置对象。
		TWeakObjectPtr<const UHodgeSprintAbilityProfile> Profile;
		// 执行的数据或对象引用。
		FGuid Execution;
		// 起始时间的数值或阈值。
		double StartTime = 0.;
		// 控制是否Rewarded的状态标记。
		bool bRewarded = false;
	};
	// 后续GAS操作使用的能力系统组件。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> ASC;
	// Windows的记录集合。
	TMap<FGuid, FWindow> Windows;
};
