/** @file HodgeAttributeCoordinator.h
 * @brief 声明PlayerState属性协调器相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AbilitySystem/Stats/HodgeAttributeTypes.h"
#include "HodgeAttributeCoordinator.generated.h"

class AHodgePlayerState;
class UHodgeAbilitySystemComponent;
class UHodgeHealthSet;
class UHodgePawnData;
class UHodgeCharacterStatProfile;

/** PlayerState 所属服务器协调器，不 Tick，不拥有武器显示或存档 IO。 */
UCLASS()
class HODGEPODGE_API UHodgeAttributeCoordinator : public UObject
{
	GENERATED_BODY()
public:
	/** 准备表现角色。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param Data 当前操作使用的数据。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool PrepareAvatar(APawn* Avatar, const UHodgePawnData* Data);
	/** 完成本Avatar的属性和装备准备并发布结果。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool CompleteAvatarInitialization();
	/** 解除当前Avatar相关的属性与监听绑定。
	 * @param Avatar 表现角色，由调用方传入。
	 */
	void DetachAvatar(APawn* Avatar);
	/** 设置角色等级。
	 * @param Level 配置或运行实例的等级。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool SetCharacterLevel(int32 Level);
	/** 按当前属性范围恢复或设置角色生命。
	 * @param Health 当前生命值。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool RestoreHealth(float Health);
	/** 开始装备更新。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool BeginEquipmentUpdate();
	/** 结束装备更新。
	 * @param bSuccess 是否成功的控制标记。
	 */
	void FinishEquipmentUpdate(bool bSuccess);
	/** 记录下一次Avatar绑定应按重生策略初始化。 */
	void ExpectRespawn();
	/** 设置初始Saved生命值。
	 * @param Health 当前生命值。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool SetInitialSavedHealth(float Health);
	/** 判断Updating。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsUpdating() const { return bUpdating; }
	/** 判断Initializing。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsInitializing() const { return bUpdating && bBootstrap; }
	/** 判断已绑定目标。
	 * @param Avatar 表现角色，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsBoundTo(const APawn* Avatar) const;
	/** 判断就绪关联。
	 * @param Avatar 表现角色，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsReadyFor(const APawn* Avatar) const;
	/** 获取Or创建默认装备。
	 * @param Definition 当前操作使用的定义配置。
	 * @return Or创建默认装备的当前值或引用。
	 */
	FHodgeOwnedEquipmentState GetOrCreateDefaultEquipment(TSubclassOf<UHodgeEquipmentDefinition> Definition);
	/** 记录装备身份与等级以供后续属性协调。
	 * @param Id 当前记录的唯一标识。
	 * @param Definition 当前操作使用的定义配置。
	 * @param Level 配置或运行实例的等级。
	 */
	void RememberEquipment(FGuid Id, TSubclassOf<UHodgeEquipmentDefinition> Definition, int32 Level);
private:
	/** 获取玩家持久状态与ASC所有者。
	 * @return 玩家持久状态与ASC所有者的当前值或引用。
	 */
	AHodgePlayerState* GetPlayerState() const;
	/** 判断当前表现角色。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsCurrentAvatar() const;
	/** 校验CoreSets。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool ValidateCoreSets() const;
	/** 应用角色基类。
	 * @param Level 配置或运行实例的等级。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool ApplyCharacterBase(int32 Level);
	/** 开始更新。
	 * @param Reason Reason，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool BeginUpdate(EHodgeAttributeUpdateReason Reason);
	/** 提交更新。
	 * @param bSuccess 是否成功的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool CommitUpdate(bool bSuccess);
	/** 发布当前Avatar的属性就绪状态。
	 * @param bReady 依赖是否已准备完成。
	 */
	void PublishReady(bool bReady);
	// 已绑定表现角色的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<APawn> BoundAvatar;
	// 能力System的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeAbilitySystemComponent> AbilitySystem;
	// 角色使用的能力、输入与相机配置。
	UPROPERTY(Transient) TObjectPtr<const UHodgePawnData> PawnData;
	// 当前操作使用的配置对象。
	UPROPERTY(Transient) TObjectPtr<const UHodgeCharacterStatProfile> Profile;
	// 生命属性集的弱引用，不延长目标对象生命周期。
	UPROPERTY(Transient) TWeakObjectPtr<UHodgeHealthSet> HealthSet;
	// 控制是否Updating的状态标记。
	bool bUpdating = false;
	// 控制是否Bootstrap的状态标记。
	bool bBootstrap = false;
	// 控制是否Was就绪的状态标记。
	bool bWasReady = false;
	// 控制是否ExpectRespawn的状态标记。
	bool bExpectRespawn = false;
	// 控制是否存在判断Saved生命值的状态标记。
	bool bHasSavedHealth = false;
	// Saved生命值的数值或阈值。
	float SavedHealth = 0.f;
	// 旧生命值的数值或阈值。
	float OldHealth = 0.f;
	// 旧最大生命值的数值或阈值。
	float OldMaxHealth = 1.f;
	// 旧基础生命值的数值或阈值。
	float OldBaseHealth = 1.f;
	// 旧基础伤害的数值或阈值。
	float OldBaseDamage = 0.f;
	// Life初始化代的数值或阈值。
	int32 LifeGeneration = 0;
	// 已应用配置版本的数值或阈值。
	int32 AppliedProfileVersion = 0;
	// 更新Reason的枚举配置或当前取值。
	EHodgeAttributeUpdateReason UpdateReason = EHodgeAttributeUpdateReason::FirstSpawn;
};
