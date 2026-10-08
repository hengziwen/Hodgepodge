#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "HodgeComboDefinition.generated.h"

USTRUCT(BlueprintType)
struct FHodgeComboTransition
{
	GENERATED_BODY()
	// 触发此边的输入意图，与 TriggerEventTag 必须只填写一个。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TriggerInputIntentTag;
	// 触发此边的权威时间轴消息，与 TriggerInputIntentTag 必须只填写一个。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TriggerEventTag;
	// 跳转的目标节点标签，必须在 ComboTable 中存在对应行。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TargetComboTag;
	// 动作仍运行时要求本次 GA 同时开启的窗口，不能用其他技能的同名状态冒充。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer RequiredWindowTags;
	// 动作已结束且连段记忆有效时，可不依赖旧动作的窗口继续此跳转。
	// 允许在旧动作结束且连段记忆有效时由输入继续，仅用于非入口节点的输入跳转。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAllowAfterExecutionEnded = false;
	// 跳转要求来源 ASC 同时拥有的状态标签集合。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer RequiredSourceTags;
	// 来源 ASC 拥有其中任意标签时禁止此跳转。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer BlockedSourceTags;
	// 多条边同时满足时数值大的优先，同优先级保持配置中的先后顺序。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 TransitionPriority = 0;
};

USTRUCT(BlueprintType)
struct FHodgeComboRow : public FTableRowBase
{
	GENERATED_BODY()
	// 当前节点身份，DataTable 行名必须与此标签名称相同。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag ComboTag;
	// 当前节点要执行的 Definition 身份，入口节点必须留空，其他节点必须填写。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag AbilityTag;
	// 当前节点期间维护的状态标签，离开节点时撤销，入口节点不能配置。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer GrantedTags;
	// 从当前节点出发的跳转边列表，可分别配置输入边和消息边。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FHodgeComboTransition> Transitions;
};

USTRUCT(BlueprintType)
struct FHodgeComboInputBinding
{
	GENERATED_BODY()
	// 来自 InputConfig 的实际能力输入标签，一个标签只能映射一次。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag InputTag;
	// 把实际输入转换成连段图使用的语义意图，供 TriggerInputIntentTag 匹配。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag IntentTag;
};

UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeComboDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 使用 HodgeComboRow 行结构的连段图数据表，行名按 ComboTag 查找。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UDataTable> ComboTable;
	// 没有动作执行时的入口节点，必须存在且不授予技能或节点状态。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag EntryComboTag;
	// 实际输入标签到连段意图的映射；这些输入先由协调器消费。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FHodgeComboInputBinding> InputBindings;
	// 单槽输入缓存保持的世界秒数，过期后不再接段，新输入会替换旧输入。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float InputBufferSeconds = 0.3f;
	// 动作结束后保留可续接节点的世界秒数，0 表示不保留，只有可恢复的输入边才能续接。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float ComboRetentionSeconds = 1.f;
	// 允许移动取消当前连段动作的窗口标签，需由本次 GA 提供。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag MoveCancelWindowTag;
	// 触发移动取消要求的输入强度阈值，不是移动速度或厘米距离。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float MoveIntentThreshold = 0.1f;
	const FHodgeComboRow* FindNode(FGameplayTag Tag) const;
	bool ValidateDefinition(TArray<FText>& Errors) const;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
