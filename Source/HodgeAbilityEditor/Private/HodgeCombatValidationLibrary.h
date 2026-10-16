/** @file HodgeCombatValidationLibrary.h
 * @brief 声明战斗PIE检查与调度函数库相关的类型、接口与现有处理流程。
 */
#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayAbilitySpecHandle.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Combat/HodgeCharacterFacingTypes.h"
#include "HodgeCombatValidationLibrary.generated.h"

class UHodgeAbilitySystemComponent;
class UHodgeGameplayAbility_Definition;
class UAnimMontage;

/** 包含已经开始混出的实例，避免把 IsPlaying=false 当作姿势权重归零。 */
USTRUCT(BlueprintType)
struct FHodgeMontageValidationState
{
	GENERATED_BODY()
	// 用于区分运行实例的标识。
	UPROPERTY(BlueprintReadOnly) int32 InstanceId = INDEX_NONE;
	// 位置的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float Position = 0.f;
	// Weight的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float Weight = 0.f;
	// 期望Weight的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float DesiredWeight = 0.f;
	// 混合时间的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float BlendTime = 0.f;
	// 控制是否Playing的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bPlaying = false;
	// 控制是否Stopped的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bStopped = true;
	// 移动表现状态的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FName LocomotionState;
};

/** 保存掉头校验状态的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgePivotValidationState
{
	GENERATED_BODY()
	// 序号的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FName Sequence;
	// 当前处理使用的时间值。
	UPROPERTY(BlueprintReadOnly) float Time = 0.f;
	// Weight的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float Weight = 0.f;
};

/** 保存同步校验状态的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgeSyncValidationState
{
	GENERATED_BODY()
	// 分组的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FName Group;
	// 序号的数据或对象引用。
	UPROPERTY(BlueprintReadOnly) FName Sequence;
	// 当前处理使用的时间值。
	UPROPERTY(BlueprintReadOnly) float Time = 0.f;
	// Weight的数值或阈值。
	UPROPERTY(BlueprintReadOnly) float Weight = 0.f;
	// 控制是否Leader的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bLeader = false;
	// 控制是否标记同步的状态标记。
	UPROPERTY(BlueprintReadOnly) bool bMarkerSync = false;
};

/** PIE 验证入口：延迟到原生世界 Tick，避免编辑器脚本保护改变 RPC 调用空间。 */
UCLASS()
class HODGEABILITYEDITOR_API UHodgeCombatValidationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** 跳字 PIE 夹具：权威 Tick 经 Damage GE 结算；零伤重置仅供高血量测试目标。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueNumberPopProbe(AActor* Source, AActor* Target, float Damage, bool bResetTargetHealth = false);
	/** 暗流PIE夹具通过正式GE修改资源，只在权威游戏Tick执行。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation") static bool QueueDarkFlowDelta(AActor* Avatar, float Delta);
	/** 读取实际暗流与容量；缺少属性集明确返回负值。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation") static FVector2D InspectDarkFlow(AActor* Avatar);
	/** 驱散当前会话自己的状态GE，验证配对清理，不影响其他技能效果。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation") static bool QueueDarkSurgeDispel(AActor* Avatar);
	/** PIE夹具只在权威游戏Tick通过正式GE增减能量，不供生产技能调用。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation") static bool QueueResonanceEnergyDelta(AActor* Avatar, float Delta);
	/** 读取真实ASC资源；缺属性集返回(-1,-1)，不伪装成零能量。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation") static FVector2D InspectResonanceEnergy(AActor* Avatar);
	/** 按当前执行身份跳转或暂停蒙太奇，用于源时钟和迟到回调故障注入。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation") static bool QueueCinematicSourceProbe(AActor* Avatar, float Position, float PlayRate = 1.f);
	/** PIE 故障注入只在游戏 Tick 执行，验证预测暂停和权威拒绝后的恢复。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueAttackFacingProbe(AActor* Avatar, FName Action);

	/** 把测试能力激活或结束操作排入后续游戏Tick。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 * @param Handle 对应操作或能力的句柄。
	 * @param bCancel 是否取消的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueAbilityAction(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool bCancel = false);
	/** 把验证用GameplayEvent排入后续游戏Tick。
	 * @param ASC 后续GAS操作使用的能力系统组件。
	 * @param EventTag 事件标签，用于对应标签查询或匹配。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueGameplayEvent(UHodgeAbilitySystemComponent* ASC, FGameplayTag EventTag);
	/** 把验证用战斗输入排入后续游戏Tick。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param InputTag 输入动作对应的GameplayTag。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueCombatInput(AActor* Avatar, FGameplayTag InputTag);
	/** 把验证用基础朝向切换排入后续游戏Tick。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param Driver 当前朝向的驱动来源。
	 * @param bRelease 是否释放的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueFacingMode(AActor* Avatar, EHodgeCharacterFacingDriver Driver, bool bRelease = false);
	/** 把验证用动作朝向请求排入后续游戏Tick。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param Direction 方向，由调用方传入。
	 * @param bRelease 是否释放的控制标记。
	 * @param bInstant 是否Instant的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueActionFacing(AActor* Avatar, FVector Direction, bool bRelease = false, bool bInstant = false);
	/** 检查朝向Sequences。
	 * @param Avatar 表现角色，由调用方传入。
	 * @return 本次查询或构造得到的集合。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static TArray<FName> InspectFacingSequences(AActor* Avatar);
	/** 检查掉头Times。
	 * @param Avatar 表现角色，由调用方传入。
	 * @return 本次查询或构造得到的集合。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static TArray<FHodgePivotValidationState> InspectPivotTimes(AActor* Avatar);
	/** 检查移动表现同步。
	 * @param Avatar 表现角色，由调用方传入。
	 * @return 本次查询或构造得到的集合。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static TArray<FHodgeSyncValidationState> InspectLocomotionSync(AActor* Avatar);
	/** 设置朝向校验控制器Permission。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param bAllow 是否Allow的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool SetFacingValidationControllerPermission(AActor* Avatar, bool bAllow);
	/** 添加命中通知。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param Start 起始，由调用方传入。
	 * @param End 结束，由调用方传入。
	 * @param Config 配置，由调用方传入。
	 * @param bSingle 是否Single的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool AddHitNotify(UAnimMontage* Montage, float Start, float End, const FHodgeAnimHitConfig& Config, bool bSingle = false);
	/** 添加状态通知。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param Start 起始，由调用方传入。
	 * @param End 结束，由调用方传入。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool AddStateNotify(UAnimMontage* Montage, float Start, float End, FGameplayTag Tag);
	/** 添加武器手部通知。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param Start 起始，由调用方传入。
	 * @param End 结束，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool AddWeaponHandNotify(UAnimMontage* Montage, float Start, float End);
	/** 检查能力Windows。
	 * @param Ability 能力，由调用方传入。
	 * @return 本次计算、查询或创建得到的玩法标签Container。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static FGameplayTagContainer InspectAbilityWindows(UHodgeGameplayAbility_Definition* Ability);
	/** 检查命中Sessions。
	 * @param Avatar 表现角色，由调用方传入。
	 * @return 本次计算、查询或创建得到的整数32。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static int32 InspectHitSessions(AActor* Avatar);
	/** 检查姿势Leases。
	 * @param Avatar 表现角色，由调用方传入。
	 * @return 本次计算、查询或创建得到的整数32。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static int32 InspectPoseLeases(AActor* Avatar);
	/** 检查蒙太奇状态。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @return 本次计算、查询或创建得到的蒙太奇校验状态。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static FHodgeMontageValidationState InspectMontageState(AActor* Avatar, UAnimMontage* Montage);
	/** 配置校验PIE。
	 * @param Players 玩家，由调用方传入。
	 * @param bDedicated 是否Dedicated的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool ConfigureValidationPIE(int32 Players, bool bDedicated = false);
	/** 配置校验片段集合。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool ConfigureValidationSections(UAnimMontage* Montage);
	/** 按现有动画通知契约整理验证蒙太奇。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool NormalizeCombatNotifies(UAnimMontage* Montage);
	/** 配置命中受击校验蒙太奇。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param SlotName 槽位名称，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool ConfigureHitReactionValidationMontage(UAnimMontage* Montage, FName SlotName);
	/** 在指定测试世界生成用于受击验证的Actor。
	 * @param WorldContextObject 用于定位所属世界的上下文对象。
	 * @param ActorClass Actor对象类型，由调用方传入。
	 * @param Location 位置，由调用方传入。
	 * @return 本次计算、查询或创建得到的Actor对象。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation", meta=(WorldContext="WorldContextObject"))
	static AActor* SpawnHitReactionValidationActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FVector Location);
	/** 检查敌人决策。
	 * @param Avatar 表现角色，由调用方传入。
	 * @return 本次生成的文字或诊断信息。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
	static FString InspectEnemyBrain(AActor* Avatar);
	/** 把共享战斗请求或连段意图排入后续游戏Tick。
	 * @param Avatar 表现角色，由调用方传入。
	 * @param ActionTag 动作标签，用于对应标签查询或匹配。
	 * @param bComboIntent 是否连段意图的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
	static bool QueueCombatAction(AActor* Avatar, FGameplayTag ActionTag, bool bComboIntent = false);

};
