/** @file HodgeGameplayUIDataSource.h
 * @brief 声明本地玩家共享UI数据源相关的类型、接口与现有处理流程。
 */
#pragma once
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "HodgeGameplayUIDataSource.generated.h"

class UHodgeLocalPlayerBase;
class UHodgeHealthComponent;
class UHodgePawnExtensionComponent;
class AHodgePlayerState;
class APlayerController;
class APlayerState;
class APawn;
class AActor;
class UHodgeAbilitySystemComponent;
struct FOnAttributeChangeData;

/** 同一个资源条常态显示积攒、暗涌显示实际燃料，不创建独立Buff倒计时。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeHavocResourceSnapshot
{
	GENERATED_BODY()
	/** 当前拥有者的真实资源与容量。 */
	UPROPERTY(BlueprintReadOnly) float CurrentDarkFlow=0.f;
	UPROPERTY(BlueprintReadOnly) float MaxDarkFlow=0.f;
	UPROPERTY(BlueprintReadOnly) float Percent=0.f;
	/** 就绪依赖当前Avatar匹配；模式从GE授予标签推导。 */
	UPROPERTY(BlueprintReadOnly) bool bReady=false;
	UPROPERTY(BlueprintReadOnly) bool bDarkSurge=false;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeUIHavocChanged,const FHodgeHavocResourceSnapshot&,Snapshot);

/** 独立充能快照不把满充当成冷却完成或允许释放。 */
USTRUCT(BlueprintType)
struct FHodgeUIResonanceSnapshot
{
	GENERATED_BODY()
	/** 服务器属性／拥有者预测提供的当前能量。 */
	UPROPERTY(BlueprintReadOnly) float Energy = 0.f;
	/** 当前配置容量，未初始化时为零。 */
	UPROPERTY(BlueprintReadOnly) float Capacity = 0.f;
	/** 安全归一化显示值，不用于直接扣除能量。 */
	UPROPERTY(BlueprintReadOnly) float Percent = 0.f;
	/** 本地玩家与ASC／Avatar链路准备完成。 */
	UPROPERTY(BlueprintReadOnly) bool bReady = false;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeUIResonanceChanged, const FHodgeUIResonanceSnapshot&, Snapshot);

/** 当前LocalPlayer的只读生命与等级快照，不拥有服务器属性权威。 */
USTRUCT(BlueprintType)
struct FHodgeUIVitalsSnapshot
{
	GENERATED_BODY()
	// 当前生命值。
	UPROPERTY(BlueprintReadOnly) float Health = 0;
	// 最大生命值。
	UPROPERTY(BlueprintReadOnly) float MaxHealth = 0;
	// 生命或进度的归一化比例。
	UPROPERTY(BlueprintReadOnly) float Percent = 0;
	// 当前角色的真实等级。
	UPROPERTY(BlueprintReadOnly) int32 CharacterLevel = 0;
	// 依赖是否已准备完成。
	UPROPERTY(BlueprintReadOnly) bool bReady = false;
	// 角色是否已进入死亡状态。
	UPROPERTY(BlueprintReadOnly) bool bDead = false;
};

/** 保存界面能力显示状态的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgeUIAbilityDisplayState
{
	GENERATED_BODY()
	// 控制是否Granted的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bGranted = false;
	// 控制是否输入允许的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bInputAllowed = false;
	// 冷却Remaining的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float CooldownRemaining = 0;
	/** 与最长剩余冷却来自同一GE实例的总时长，单位秒。 */
	UPROPERTY(BlueprintReadOnly) float CooldownDuration = 0;
};

/** 保存HUD界面能力槽位的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgeHUDAbilitySlot
{
    GENERATED_BODY()
    // 显示名称和键位提示属于 UI，不改变能力逻辑。
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Label;
    // 表达输入意图；普攻由 CombatComponent 按图选段。
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="InputTag")) FGameplayTag InputTag;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeUIVitalsChanged, const FHodgeUIVitalsSnapshot&, Snapshot);

/** 当前本地玩家共享的数据接入；不创建控件，也不拥有玩法权威。 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeGameplayUIDataSource : public UObject
{
	GENERATED_BODY()
public:
	/** 初始化ialize。
	 * @param Player 玩家，由调用方传入。
	 * @param InController 当前角色使用的控制器。
	 */
	void Initialize(UHodgeLocalPlayerBase* Player, APlayerController* InController);
	/** 停止当前对象的运行绑定并清理其资源。 */
	void Shutdown();
	// 真实生命或等级快照变化的通知。
	UPROPERTY(BlueprintAssignable, Category="Hodge|UI") FHodgeUIVitalsChanged OnVitalsChanged;
	/** 获取生命显示。
	 * @return 生命显示的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeUIVitalsSnapshot GetVitals() const { return Vitals; }
	/** 当前拥有者充能数据，冷却另由GetAbilityDisplayState查询。 */
	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeUIResonanceSnapshot GetResonance() const { return Resonance; }
	/** 不提供UI修改燃料入口，只返回当前本地玩家快照。 */
	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeHavocResourceSnapshot GetHavocSnapshot() const { return Havoc; }
	UPROPERTY(BlueprintAssignable, Category="Hodge|UI") FHodgeUIHavocChanged OnHavocChanged;
	/** 只在能量、容量或可用性实际变化时广播。 */
	UPROPERTY(BlueprintAssignable, Category="Hodge|UI") FHodgeUIResonanceChanged OnResonanceChanged;
	/** 获取能力显示状态。
	 * @param InputTag 输入动作对应的GameplayTag。
	 * @return 能力显示状态的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeUIAbilityDisplayState GetAbilityDisplayState(FGameplayTag InputTag) const;
	/** 从当前本地玩家的数据源提交合法技能输入意图。
	 * @param InputTag 输入动作对应的GameplayTag。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|UI") bool SubmitInput(FGameplayTag InputTag);
private:
	/** 暗流与现有数据源共用PlayerState及Pawn绑定链路。 */
	UPROPERTY(Transient) FHodgeHavocResourceSnapshot Havoc;
	FDelegateHandle DarkFlowDelegate;
	FDelegateHandle MaxDarkFlowDelegate;
	FDelegateHandle DarkSurgeDelegate;
	/** 标签变化只刷新表现，不控制状态生效或结束。 */
	void HavocModeChanged(FGameplayTag Tag,int32 Count);
	// 生命显示的数据或对象引用。
	UPROPERTY(Transient) FHodgeUIVitalsSnapshot Vitals;
	/** 充能快照只读玩法属性，不拥有回能计时或成本。 */
	UPROPERTY(Transient) FHodgeUIResonanceSnapshot Resonance;
	TWeakObjectPtr<UHodgeAbilitySystemComponent> ResonanceASC;
	FDelegateHandle EnergyDelegate;
	FDelegateHandle CapacityDelegate;
	/** ASC属性变化复用现有数据源刷新，换PlayerState时配对解绑。 */
	void ResonanceAttributeChanged(const FOnAttributeChangeData& Data);
	// 本地玩家的弱引用，不延长目标对象生命周期。
	TWeakObjectPtr<UHodgeLocalPlayerBase> LocalPlayer;
	// 当前角色使用的控制器。
	TWeakObjectPtr<APlayerController> Controller;
	// 表现角色的弱引用，不延长目标对象生命周期。
	TWeakObjectPtr<APawn> Avatar;
	// 当前生命值。
	TWeakObjectPtr<UHodgeHealthComponent> Health;
	// 扩展的弱引用，不延长目标对象生命周期。
	TWeakObjectPtr<UHodgePawnExtensionComponent> Extension;
	// 玩家持有的持久状态与ASC入口。
	TWeakObjectPtr<AHodgePlayerState> PlayerState;
	// Pawn角色委托对应的事件通知。
	FDelegateHandle PawnDelegate;
	// 玩家状态委托对应的事件通知。
	FDelegateHandle PlayerStateDelegate;
	// 就绪委托对应的事件通知。
	FDelegateHandle ReadyDelegate;
	// 控制是否已绑定的状态标记。
	bool bBound = false;
	/** 在本地玩家切换Pawn时解绑旧数据并接入新角色。
	 * @param Player 玩家，由调用方传入。
	 * @param Pawn 当前操作关联的Pawn角色。
	 */
	void PawnChanged(UHodgeLocalPlayerBase* Player, APawn* Pawn);
	/** 在本地玩家切换PlayerState时更新数据监听。
	 * @param Player 玩家，由调用方传入。
	 * @param State 当前状态数据。
	 */
	void StateChanged(UHodgeLocalPlayerBase* Player, APlayerState* State);
	/** 解绑Pawn角色。 */
	void UnbindPawn();
	/** 解绑状态。 */
	void UnbindState();
	/** 重新采集当前依赖的数据并更新缓存状态。 */
	void Refresh();
	/** 检查是否允许Use玩法。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool CanUseGameplay() const;
	/** 收到属性变化后刷新当前数据快照。
	 * @param Component 组件，由调用方传入。
	 * @param Old 旧，由调用方传入。
	 * @param New 新，由调用方传入。
	 * @param Instigator 发起本次行为的对象。
	 */
	UFUNCTION() void AttributeChanged(UHodgeHealthComponent* Component, float Old, float New, AActor* Instigator);
	/** 收到死亡阶段变化后刷新当前数据快照。
	 * @param Actor 本次操作关联的Actor。
	 */
	UFUNCTION() void DeathChanged(AActor* Actor);
};
