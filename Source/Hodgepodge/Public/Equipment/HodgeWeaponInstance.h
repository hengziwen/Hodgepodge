/** @file HodgeWeaponInstance.h
 * @brief 声明武器运行实例相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// 暂未启用的角色外观动画类型支持。
//#include "Cosmetics/HodgeCosmeticAnimationTypes.h"

// 武器实例继承自通用装备运行时实例。
#include "Equipment/HodgeEquipmentInstance.h"
#include "Combat/HodgeHitDetection.h"
#include "Equipment/HodgeWeaponPresentationTypes.h"

// 提供输入设备属性句柄，用于记录并移除武器激活的设备效果。
#include "GameFramework/InputDevicePropertyHandle.h"

#include "HodgeWeaponInstance.generated.h"

class UAnimInstance;
class UObject;
struct FFrame;
struct FGameplayTagContainer;
class UInputDeviceProperty;
class UHodgeWeaponPresentationProfile;
class USkeletalMeshComponent;

/**
 * UHodgeWeaponInstance
 *
 * A piece of equipment representing a weapon spawned and applied to a pawn
 *
 * 表示一件已经生成并装备到 Pawn 身上的运行时武器实例。
 * 在通用装备实例基础上扩展武器交互时间、动画层选择和输入设备效果管理。
 */
class UHodgeCombatComponentBase;

/** 封装武器运行实例的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeWeaponInstance : public UHodgeEquipmentInstance
{
	GENERATED_BODY()

public:
	// 武器提供自身的 Socket 和尺寸，技能决定何时使用这些来源。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat", meta=(TitleProperty="SourceTag"))
	TArray<FHodgeHitSource> HitSources;

	// 表现配置的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Presentation")
	TObjectPtr<UHodgeWeaponPresentationProfile> PresentationProfile;
	/** 获取表现配置。
	 * @return 表现配置的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
	UHodgeWeaponPresentationProfile* GetPresentationProfile() const { return PresentationProfile; }
	/** 按执行身份申请武器手持窗口。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 * @param OccurrenceId 通知次数标识，由调用方传入。
	 * @param ActivationKey 本次激活的关联键。
	 * @param EndBehavior 此手持区间正常结束或取消时的释放方式。
	 * @return 本次计算、查询或创建得到的唯一标识。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Presentation")
	FGuid AcquireHandUse(FGuid ExecutionId, int32 OccurrenceId, int32 ActivationKey = 0, EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack);
	/** 释放手部Use。
	 * @param Handle 对应操作或能力的句柄。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Presentation")
	void ReleaseHandUse(FGuid Handle);
	/** 释放手部Uses关联执行。
	 * @param ExecutionId 本次动作执行的唯一身份。
	 */
	void ReleaseHandUsesForExecution(const FGuid& ExecutionId);
	/** 撤销被服务器拒绝的预测手持请求。
	 * @param ActivationKey 本次激活的关联键。
	 */
	void RejectPredictedHandUse(int32 ActivationKey);
	/** 获取手部Use数量。
	 * @return 手部Use数量的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
	int32 GetHandUseCount() const { return HandRequests.Num(); }
	/** 获取表现状态。
	 * @return 表现状态的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Presentation")
	FHodgeWeaponPresentationState GetPresentationState() const;
	/** 获取表现时间。
	 * @return 表现时间的当前值或引用。
	 */
	double GetPresentationTime() const;
	/** 按当前武器表现状态更新关联Actor。 */
	void RefreshPresentationActors();
	/** 解析表现武器。
	 * @param Pawn 当前操作关联的Pawn角色。
	 * @param SourceObject 能力或效果的来源对象。
	 * @return 本次计算、查询或创建得到的武器运行实例。
	 */
	static UHodgeWeaponInstance* ResolvePresentationWeapon(APawn* Pawn, UObject* SourceObject = nullptr);

	// 构造武器运行时实例。
	/** 构造武器运行实例并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/** 登记本类型需要网络复制的属性及复制条件。
	 * @param OutLifetimeProps 输出LifetimeProps。
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~UHodgeEquipmentInstance interface

	// 武器装备时执行武器专属初始化逻辑。
	/** 处理Equipped。 */
	virtual void OnEquipped() override;

	// 武器卸下时执行武器专属清理逻辑。
	/** 处理Unequipped。 */
	virtual void OnUnequipped() override;

	//~End of UHodgeEquipmentInstance interface

	// 更新最近一次武器开火或使用的时间。
	/** 更新攻击交互时间。 */
	UFUNCTION(BlueprintCallable)
	void UpdateFiringTime();

	// Returns how long it's been since the weapon was interacted with (fired or equipped)

	// 返回距离武器最近一次交互的时间，交互包括装备和开火。
	/** 获取时间Since上次Interacted关联。
	 * @return 时间Since上次Interacted关联的当前值或引用。
	 */
	UFUNCTION(BlueprintPure)
	float GetTimeSinceLastInteractedWith() const;

protected:
	/** 处理SpawnedActors变化。 */
	virtual void OnSpawnedActorsChanged() override;
	/** 在UObject销毁前清理仍由本对象持有的资源。 */
	virtual void BeginDestroy() override;
	// 装备状态下根据角色外观标签选择动画层的配置，目前暂未启用。
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	//FHodgeAnimLayerSelectionSet EquippedAnimSet;

	// 未装备状态下根据角色外观标签选择动画层的配置，目前暂未启用。
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	//FHodgeAnimLayerSelectionSet UneuippedAnimSet;

	/**
	 * Device properties that should be applied while this weapon is equipped.
	 * These properties will be played in with the "Looping" flag enabled, so they will
	 * play continuously until this weapon is unequipped! 
	 */

	// 武器装备期间需要持续应用的输入设备属性，例如手柄震动或其他设备反馈效果。
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Input Devices")
	TArray<TObjectPtr<UInputDeviceProperty>> ApplicableDeviceProperties;

	// Choose the best layer from EquippedAnimSet or UneuippedAnimSet based on the specified gameplay tags

	// 根据装备状态和角色外观 GameplayTag，从对应动画配置中选择最合适的动画层。
	/** 按当前装备与角色条件选择动画层类。
	 * @param bEquipped 是否Equipped的控制标记。
	 * @param CosmeticTags Cosmetic标签，用于对应标签查询或匹配。
	 * @return 本次计算、查询或创建得到的TSubclassOfU动画Instance。
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;

	/** Returns the owning Pawn's Platform User ID */

	// 获取当前武器所属 Pawn 对应本地玩家的 Platform User ID。
	/** 获取Owning用户标识。
	 * @return Owning用户标识的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable)
	const FPlatformUserId GetOwningUserId() const;

	/** Callback for when the owning pawn of this weapon dies. Removes all spawned device properties. */

	// 所属 Pawn 开始死亡时调用，用于清除当前武器激活的所有输入设备效果。
	/** 处理死亡开始。
	 * @param OwningActor OwningActor对象，由调用方传入。
	 */
	UFUNCTION()
	void OnDeathStarted(AActor* OwningActor);

	/**
	 * Apply the ApplicableDeviceProperties to the owning pawn of this weapon.
	 * Populate the DevicePropertyHandles so that they can be removed later. This will
	 * Play the device properties in Looping mode so that they will share the lifetime of the
	 * weapon being Equipped.
	 */

	// 应用武器配置的输入设备效果，并保存对应句柄以便后续统一移除。
	/** 应用设备Properties。 */
	void ApplyDeviceProperties();

	/** Remove any device proeprties that were activated in ApplyDeviceProperties. */

	// 根据保存的句柄移除由当前武器激活的所有输入设备效果。
	/** 移除设备Properties。 */
	void RemoveDeviceProperties();

private:
	friend struct FHodgeWeaponPresentationTestAccess;
	/** 保存手部请求的数据和接口契约。 */
	struct FHandRequest
	{
		/** 本次动作执行的唯一身份。 */
		FGuid ExecutionId;
		/** 通知进入时生成的区间身份。 */
		int32 OccurrenceId = INDEX_NONE;
		/** 服务器与预测拥有者关联的激活键。 */
		int32 ActivationKey = 0;
		/** 通知开始时保存释放方式，共享 Notify 不保存运行状态。 */
		EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack;
		/** 批量结束同一执行时按申请顺序释放。 */
		uint64 Order = 0;
	};
	/** 当前武器生命周期中的手持申请序号。 */
	uint64 NextHandRequestOrder = 0;
	// 手部Requests的记录集合。
	TMap<FGuid, FHandRequest> HandRequests;
	// 表现计时器对应操作的配对句柄。
	FTimerHandle PresentationTimer;
	// 控制是否表现Equipped的状态标记。
	bool bPresentationEquipped = false;
	// 控制是否表现禁用的状态标记。
	bool bPresentationDisabled = false;
	// 控制是否Predicted表现的状态标记。
	bool bPredictedPresentation = false;
	// 本地表现的数据或对象引用。
	FHodgeWeaponPresentationState LocalPresentation;
	// 复制表现的数据或对象引用。
	UPROPERTY(ReplicatedUsing=OnRep_PresentationState)
	FHodgeWeaponPresentationState ReplicatedPresentation;
	/** 收到表现状态复制后更新本地状态。 */
	UFUNCTION() void OnRep_PresentationState();
	/** 初始化表现。 */
	void InitializePresentation();
	/** 清理当前武器表现的计时器与来源记录。 */
	void ShutdownPresentation();
	/** 设置表现Phase。
	 * @param Phase Phase，由调用方传入。
	 * @param ActivationKey 本次激活的关联键。
	 */
	void SetPresentationPhase(EHodgeWeaponPresentationPhase Phase, int32 ActivationKey, EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack);
	/** 按配置安排下一次武器表现阶段切换。
	 * @param Phase Phase，由调用方传入。
	 * @param Seconds 秒，由调用方传入。
	 */
	void SchedulePresentationPhase(EHodgeWeaponPresentationPhase Phase, float Seconds, EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack);
	/** 开始Idle表现。 */
	void BeginIdlePresentation(EHodgeWeaponHandEndBehavior EndBehavior);
	/** 检查是否允许Drive表现。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool CanDrivePresentation() const;
	/** 清空表现计时器。 */
	void ClearPresentationTimer();
	// 战斗姿势占用许可的数据或对象引用。
	FGuid CombatPoseLease;
	// 姿势战斗的弱引用，不延长目标对象生命周期。
	TWeakObjectPtr<UHodgeCombatComponentBase> PoseCombat;
	/** 更新所属对象姿势策略。 */
	void UpdateOwnerPosePolicy();
	/** Set of device properties activated by this weapon. Populated by ApplyDeviceProperties */

	// 保存当前武器实际激活的设备属性句柄，作为后续清理这些效果的运行时记录。
	UPROPERTY(Transient)
	TSet<FInputDevicePropertyHandle> DevicePropertyHandles;

	// 记录最近一次装备该武器的世界时间。
	double TimeLastEquipped = 0.0;

	// 记录最近一次使用或开火该武器的世界时间。
	double TimeLastFired = 0.0;
};
