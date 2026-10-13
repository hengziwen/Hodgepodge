/** @file HodgeWeaponPresentationActor.h
 * @brief 声明武器表现Actor相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Equipment/HodgeWeaponPresentationTypes.h"
#include "HodgeWeaponPresentationActor.generated.h"

class UHodgeWeaponInstance;
class UHodgeWeaponPresentationProfile;
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;
class APawn;

/** 根挂点和隐藏逻辑 Mesh 留在手部，仅可见 Mesh 执行回背与消隐。 */
UCLASS(Blueprintable)
class HODGEPODGE_API AHodgeWeaponPresentationActor : public AActor
{
	GENERATED_BODY()
public:
	/** 构造武器表现Actor并初始化其默认状态。 */
	AHodgeWeaponPresentationActor();
	/** 绑定武器。
	 * @param Instance Instance，由调用方传入。
	 */
	void BindWeapon(UHodgeWeaponInstance* Instance);
	/** 刷新表现。 */
	void RefreshPresentation();
	/** 获取检测网格。
	 * @return 检测网格的当前值或引用。
	 */
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetDetectionMesh() const { return SkeletalMesh; }
	/** 获取Visual网格。
	 * @return Visual网格的当前值或引用。
	 */
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetVisualMesh() const { return WeaponVisualMesh; }
	/** 获取可见性数量。
	 * @return 可见性数量的当前值或引用。
	 */
	UFUNCTION(BlueprintPure) float GetVisibilityAmount() const { return VisibilityAmount; }
	/** 获取VisualPhase。
	 * @return VisualPhase的当前值或引用。
	 */
	UFUNCTION(BlueprintPure) EHodgeWeaponPresentationPhase GetVisualPhase() const { return VisualPhase; }
	/** 获取Visual变换。
	 * @return Visual变换的当前值或引用。
	 */
	FTransform GetVisualTransform() const;
	/** 推进当前对象的随帧更新。
	 * @param DeltaSeconds 本次更新的时间步长，单位秒。
	 */
	virtual void Tick(float DeltaSeconds) override;
protected:
	/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
	virtual void BeginPlay() override;
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param Reason Reason，由调用方传入。
	 */
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	// 骨骼网格的数据或对象引用。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
	// 武器Visual网格的数据或对象引用。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMeshComponent> WeaponVisualMesh;
	/** 处理表现Phase变化。
	 * @param Phase Phase，由调用方传入。
	 */
	UFUNCTION(BlueprintImplementableEvent) void OnPresentationPhaseChanged(EHodgeWeaponPresentationPhase Phase);
private:
	/** 尝试绑定武器。 */
	void TryBindWeapon();
	/** 配置配置。
	 * @param Profile 当前操作使用的配置对象。
	 */
	void ConfigureProfile(const UHodgeWeaponPresentationProfile* Profile);
	/** 计算表现。 */
	void EvaluatePresentation();
	// 武器的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
	// 已应用配置的数据或对象引用。
	UPROPERTY(Transient) TObjectPtr<const UHodgeWeaponPresentationProfile> AppliedProfile;
	// DynamicMaterials的记录集合。
	UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;
	// 绑定重试计时器对应操作的配对句柄。
	FTimerHandle BindRetryTimer;
	// 绑定Attempts的数值或阈值。
	int32 BindAttempts = 0;
	// 可见性数量的数值或阈值。
	float VisibilityAmount = 0.f;
	// VisualPhase的枚举配置或当前取值。
	EHodgeWeaponPresentationPhase VisualPhase = EHodgeWeaponPresentationPhase::Hidden;
};
