/** @file HodgeEnemyInitComponent.h
 * @brief 声明怪物初始化协调组件相关的类型、接口与现有处理流程。
 */
#pragma once

#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Data/HodgeAbilitySet.h"
#include "HodgeEnemyInitComponent.generated.h"

class UHodgePawnExtensionComponent;
class UHodgePawnData;
class UHodgeExperienceDefinition;
class AGameStateBase;

/** 定义敌人初始化结果的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeEnemyInitializationResult : uint8 { /* 等待必要依赖。 */ Waiting, /* 初始化依赖已经满足。 */ Ready, /* 配置或执行失败。 */ Failed, /* 当前生命周期或绑定已失效。 */ Invalidated };

DECLARE_MULTICAST_DELEGATE(FHodgeEnemyReadinessChanged);

/** 协调怪物自身的初始化；属性、技能和装备仍由已有系统执行。 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeEnemyInitComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	/** 构造怪物初始化协调组件并初始化其默认状态。
	 * @param Initializer UE构造对象时传入的初始化器。
	 */
	UHodgeEnemyInitComponent(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	// NAMEActor对象功能扩展名称的数据或对象引用。
	static const FName NAME_ActorFeatureName;
	/** 获取功能扩展名称。
	 * @return 功能扩展名称的当前值或引用。
	 */
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	/** 只判断初始化阶段能否推进，不承担阶段副作用。
	 * @param Manager 管理器，由调用方传入。
	 * @param Current 当前，用于对应标签查询或匹配。
	 * @param Desired 期望，用于对应标签查询或匹配。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag Current, FGameplayTag Desired) const override;
	/** 处理Actor对象Init状态变化。
	 * @param Params 参数，由调用方传入。
	 */
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	/** 尝试推进当前组件及相关Feature的初始化链。 */
	virtual void CheckDefaultInitialization() override;
	/** 按已有依赖与状态尝试推进初始化。
	 * @return 本次处理得到的状态枚举。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Enemy") EHodgeEnemyInitializationResult TryInitialize();
	/** 查询本Feature是否已完成当前初始化链。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") bool IsGameplayReady() const;
	/** 检查当前怪物的ASC、基础授予和必要装备依赖。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") bool HasValidCombatDependencies() const;
	/** 检查怪物是否已就绪且当前未被死亡或控制限制。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") bool CanExecuteAIAction() const;
	/** 获取初始化Diagnostic。
	 * @return 初始化Diagnostic的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") FString GetInitializationDiagnostic() const { return Diagnostic; }
	/** 获取初始化结果。
	 * @return 初始化结果的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Enemy") EHodgeEnemyInitializationResult GetInitializationResult() const { return Result; }
	/** 获取初始化初始化代。
	 * @return 初始化初始化代的当前值或引用。
	 */
	int32 GetInitializationGeneration() const { return Generation; }
	// 初始化依赖或就绪状态变化的通知。
	FHodgeEnemyReadinessChanged OnReadinessChanged;
protected:
	/** 组件注册后接入当前Owner的依赖。 */
	virtual void OnRegister() override;
	/** 组件注销时解除依赖并清理本组件资源。 */
	virtual void OnUnregister() override;
	/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
	virtual void BeginPlay() override;
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param Reason Reason，由调用方传入。
	 */
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
private:
	friend struct FHodgeEnemyInitTestAccess;
	/** 初始化步骤。
	 * @return 本次处理得到的状态枚举。
	 */
	EHodgeEnemyInitializationResult InitializeStep();
	/** 记录当前初始化或执行失败并完成必要清理。
	 * @param Reason Reason，由调用方传入。
	 * @return 本次处理得到的状态枚举。
	 */
	EHodgeEnemyInitializationResult Fail(const FString& Reason);
	/** 等待当前阶段满足完成条件。
	 * @param Reason Reason，由调用方传入。
	 * @return 本次处理得到的状态枚举。
	 */
	EHodgeEnemyInitializationResult Wait(const TCHAR* Reason);
	/** 绑定Dependencies。 */
	void BindDependencies();
	/** 处理ASCUninitialized。 */
	void HandleASCUninitialized();
	/** 处理Experience配置已加载。
	 * @param Experience Experience配置，由调用方传入。
	 */
	void HandleExperienceLoaded(const UHodgeExperienceDefinition* Experience);
	/** 处理游戏状态设置。
	 * @param State 当前状态数据。
	 */
	void HandleGameStateSet(AGameStateBase* State);
	/** 按当前对象的来源归属解除监听并清理资源。 */
	void Cleanup();
	/** 判断绑定当前。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsBindingCurrent() const;
	// 控制是否RequireExperience配置的状态标记。
	UPROPERTY(EditDefaultsOnly, Category="Hodge|Enemy") bool bRequireExperience = true;
	// 本生命周期使用的角色配置。
	UPROPERTY(Transient) TObjectPtr<const UHodgePawnData> BoundData;
	// 本生命周期绑定的ASC弱引用。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> BoundASC;
	// 扩展的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgePawnExtensionComponent> Extension;
	// 基础Handles对应操作的配对句柄。
	UPROPERTY(Transient) FHodgeAbilitySet_GrantedHandles BaseHandles;
	// 本次操作的结果。
	UPROPERTY(Transient) EHodgeEnemyInitializationResult Result = EHodgeEnemyInitializationResult::Waiting;
	// Diagnostic的数据或对象引用。
	UPROPERTY(Transient) FString Diagnostic;
	// 游戏状态句柄对应的事件通知。
	FDelegateHandle GameStateHandle;
	// 控制是否Experience配置已绑定的状态标记。
	bool bExperienceBound = false;
	// 控制是否Experience配置就绪的状态标记。
	bool bExperienceReady = false;
	// 控制是否Trying的状态标记。
	bool bTrying = false;
	// 控制是否Recheck的状态标记。
	bool bRecheck = false;
	// 控制是否Closed的状态标记。
	bool bClosed = false;
	// 正在清理，防止回调重复进入清理。
	bool bCleaning = false;
	// 控制是否Stats已应用的状态标记。
	bool bStatsApplied = false;
	// 控制是否基础Granted的状态标记。
	bool bBaseGranted = false;
	// 控制是否装备Prepared的状态标记。
	bool bEquipmentPrepared = false;
	// 初始化代的数值或阈值。
	int32 Generation = 1;
};
