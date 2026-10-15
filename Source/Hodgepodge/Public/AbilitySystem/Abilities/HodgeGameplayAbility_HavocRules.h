/** @file HodgeGameplayAbility_HavocRules.h @brief 共鸣回路被动只协调资源与状态，不持有主动作。 */
#pragma once
#include "AbilitySystem/Abilities/HodgeGameplayAbility.h"
#include "Combat/HodgeHavocRules.h"
#include "HodgeGameplayAbility_HavocRules.generated.h"

class UHodgeGameplayAbility_Definition;
class UHodgePawnExtensionComponent;

/** 同一次暗涌的GE和Avatar归属，不能由旧执行清理新会话。 */
struct FHodgeDarkSurgeSession
{
	FGuid Id;
	FGuid EntryExecutionId;
	TWeakObjectPtr<AActor> Avatar;
	FActiveGameplayEffectHandle State;
	FActiveGameplayEffectHandle Drain;
	FHodgeHavocFuelClock Clock;
};

/** 服务器Independent规则能力；采用既有OnSpawn与PlayerState ASC生命周期。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayAbility_HavocRules : public UHodgeGameplayAbility
{
	GENERATED_BODY()
public:
	UHodgeGameplayAbility_HavocRules();
	/** 查找已经激活的唯一规则实例，不在命中时授予能力或资源集。 */
	static UHodgeGameplayAbility_HavocRules* Find(UAbilitySystemComponent* ASC);
	/** 只读校验满燃料、存活和当前Avatar，不支付100点成本。 */
	bool CanEnterDarkSurge(const AActor* Avatar) const;
	/** 只能从当前攻击执行提交，重复或迟到请求返回失败。 */
	bool TryEnterDarkSurge(UHodgeGameplayAbility_Definition* Source);
	/** 精确清理本次两个GE，重复结束不影响其他效果。 */
	void EndDarkSurge(FGuid SessionId);
	/** 合法命中或成功施放使用同一增量GE，返回实际增加值。 */
	float ApplyDarkFlowGain(const UHodgeGameplayAbility_Definition* Source, float Amount);
	/** 暗涌普攻受检命中额外回R能量，所有攻击共享角色级限流。 */
	void HandleEnhancedNormalHit(const UHodgeGameplayAbility_Definition* Source, float AcceptedDamage);
	/** 仅供所属周期GE执行，按真实时间返回本次扣除量。 */
	float SettleFuel(const FGameplayEffectSpec& Spec);
	/** 已配置的消耗速度，单位点／秒。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc", meta=(ClampMin="0.01")) float DrainPerSecond=10.f;
	/** 长按仍由输入层管理，此处只提供角色配置来源。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc", meta=(ClampMin="0.01")) float HoldThreshold=.35f;
	/** 达到长按阈值后只在此有界时间内等待合法攻击取消窗口。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc", meta=(ClampMin="0")) float HoldBufferSeconds=.3f;
	/** 非周期状态、周期消耗和资源增量必须各自配置有效GE。 */
	UPROPERTY(EditDefaultsOnly, Category="Hodge|Havoc") TSubclassOf<UGameplayEffect> StateEffect;
	UPROPERTY(EditDefaultsOnly, Category="Hodge|Havoc") TSubclassOf<UGameplayEffect> DrainEffect;
	UPROPERTY(EditDefaultsOnly, Category="Hodge|Havoc") TSubclassOf<UGameplayEffect> GainEffect;
	/** 查询会话只用于执行归属，不替代GE标签事实来源。 */
	FGuid GetModeSessionId() const { return Session.Id; }
protected:
	/** 注册属性和效果移除观察者，重复绑定先释放旧订阅。 */
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* Event) override;
	/** Pawn更换时原会话不能跨Avatar保留。 */
	virtual void OnPawnAvatarSet() override;
	/** 按所属句柄和委托解除，不清除其他GA状态。 */
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
		FGameplayAbilityActivationInfo ActivationInfo, bool bReplicate, bool bCancelled) override;
private:
	friend struct FHodgeHavocTestAccess;
	/** 已验证的入态事务，测试直接验证真实GE应用与回滚。 */
	bool BeginSession(FGuid EntryExecutionId);
	/** 当前会话与跨普攻的限流记录。 */
	FHodgeDarkSurgeSession Session;
	FHodgeHavocProcGate ProcGate;
	TWeakObjectPtr<AActor> BoundAvatar;
	TWeakObjectPtr<UAbilitySystemComponent> BoundASC;
	FDelegateHandle FuelChangedHandle;
	FDelegateHandle RemovedHandle;
	FDelegateHandle DeathHandle;
	TWeakObjectPtr<UHodgePawnExtensionComponent> BoundExtension;
	bool bEndingSession=false;
	/** 判断来源仍是当前角色的有效攻击执行。 */
	bool IsCurrentSource(const UHodgeGameplayAbility_Definition* Source) const;
	/** GE配置错误阻止入态，不能留下不耗燃料的永久增伤。 */
	bool ValidateEffects() const;
	/** 属性归零在Post变化委托处理，不在PreAttributeChange修改GE。 */
	void OnFuelChanged(const FOnAttributeChangeData& Data);
	/** 外部驱散任一所属GE时清理配对效果。 */
	void OnEffectRemoved(const FActiveGameplayEffect& Effect);
	/** 死亡无论是否处于暗涌都清空暗流，不取消其他能力。 */
	void OnDeathTagChanged(FGameplayTag Tag,int32 Count);
	/** 驱散、死亡或换Pawn清空服务器资源；失败入态回滚不调用此接口。 */
	void ClearDarkFlow();
	/** ASC从旧Pawn解绑后立即撤销该Pawn的会话，不等待下一个Pawn生成。 */
	void OnAvatarUninitialized(TWeakObjectPtr<AActor> ForAvatar);
	void BindAvatar();
	/** 注销所有本实例观察者。 */
	void Unbind();
};
