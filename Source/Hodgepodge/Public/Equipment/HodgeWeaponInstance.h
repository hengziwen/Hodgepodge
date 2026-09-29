// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// 暂未启用的角色外观动画类型支持。
//#include "Cosmetics/HodgeCosmeticAnimationTypes.h"

// 武器实例继承自通用装备运行时实例。
#include "Equipment/HodgeEquipmentInstance.h"

// 提供输入设备属性句柄，用于记录并移除武器激活的设备效果。
#include "GameFramework/InputDevicePropertyHandle.h"

#include "HodgeWeaponInstance.generated.h"

class UAnimInstance;
class UObject;
struct FFrame;
struct FGameplayTagContainer;
class UInputDeviceProperty;

/**
 * UHodgeWeaponInstance
 *
 * A piece of equipment representing a weapon spawned and applied to a pawn
 *
 * 表示一件已经生成并装备到 Pawn 身上的运行时武器实例。
 * 在通用装备实例基础上扩展武器交互时间、动画层选择和输入设备效果管理。
 */
UCLASS()
class HODGEPODGE_API UHodgeWeaponInstance : public UHodgeEquipmentInstance
{
	GENERATED_BODY()

public:
	// 构造武器运行时实例。
	UHodgeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UHodgeEquipmentInstance interface

	// 武器装备时执行武器专属初始化逻辑。
	virtual void OnEquipped() override;

	// 武器卸下时执行武器专属清理逻辑。
	virtual void OnUnequipped() override;

	//~End of UHodgeEquipmentInstance interface

	// 更新最近一次武器开火或使用的时间。
	UFUNCTION(BlueprintCallable)
	void UpdateFiringTime();

	// Returns how long it's been since the weapon was interacted with (fired or equipped)

	// 返回距离武器最近一次交互的时间，交互包括装备和开火。
	UFUNCTION(BlueprintPure)
	float GetTimeSinceLastInteractedWith() const;

protected:
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
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;

	/** Returns the owning Pawn's Platform User ID */

	// 获取当前武器所属 Pawn 对应本地玩家的 Platform User ID。
	UFUNCTION(BlueprintCallable)
	const FPlatformUserId GetOwningUserId() const;

	/** Callback for when the owning pawn of this weapon dies. Removes all spawned device properties. */

	// 所属 Pawn 开始死亡时调用，用于清除当前武器激活的所有输入设备效果。
	UFUNCTION()
	void OnDeathStarted(AActor* OwningActor);

	/**
	 * Apply the ApplicableDeviceProperties to the owning pawn of this weapon.
	 * Populate the DevicePropertyHandles so that they can be removed later. This will
	 * Play the device properties in Looping mode so that they will share the lifetime of the
	 * weapon being Equipped.
	 */

	// 应用武器配置的输入设备效果，并保存对应句柄以便后续统一移除。
	void ApplyDeviceProperties();

	/** Remove any device proeprties that were activated in ApplyDeviceProperties. */

	// 根据保存的句柄移除由当前武器激活的所有输入设备效果。
	void RemoveDeviceProperties();

private:
	/** Set of device properties activated by this weapon. Populated by ApplyDeviceProperties */

	// 保存当前武器实际激活的设备属性句柄，作为后续清理这些效果的运行时记录。
	UPROPERTY(Transient)
	TSet<FInputDevicePropertyHandle> DevicePropertyHandles;

	// 记录最近一次装备该武器的世界时间。
	double TimeLastEquipped = 0.0;

	// 记录最近一次使用或开火该武器的世界时间。
	double TimeLastFired = 0.0;
};
