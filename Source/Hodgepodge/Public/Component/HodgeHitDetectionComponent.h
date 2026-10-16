/** @file HodgeHitDetectionComponent.h @brief 集中管理命中会话、来源、几何历史与服务器姿势占用。 */
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/HodgeHitDetection.h"
#include "HodgeHitDetectionComponent.generated.h"
class UHodgeWeaponInstance;
class USceneComponent;
class USkeletalMeshComponent;
/** 保存命中检测会话的数据和接口契约。 */
USTRUCT()
struct FHodgeHitDetectionSession
{
	GENERATED_BODY()
	// 本次动作执行的唯一身份。
	UPROPERTY() FGuid ExecutionId;
	// 通知次数标识的数值或阈值。
	UPROPERTY() int32 OccurrenceId = INDEX_NONE;
	// 请求的数据或对象引用。
	UPROPERTY() FHodgeHitDetectionRequest Request;
	// 本次操作的来源。
	UPROPERTY() FHodgeHitSource Source;
	// 来源组件的弱引用，不延长目标对象生命周期。
	UPROPERTY() TWeakObjectPtr<USceneComponent> SourceComponent;
	// 武器的弱引用，不延长目标对象生命周期。
	UPROPERTY() TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
	// Strategy的数据或对象引用。
	TSubclassOf<UHodgeHitDetectionStrategy> Strategy;
	// 控制是否武器来源的状态标记。
	bool bWeaponSource = false;
	// 采样序号的数值或阈值。
	int32 SampleSequence = 0;
	// 更新前的状态。
	FHodgeHitGeometry Previous;
	// Fixed锚点的几何或变换数据。
	FTransform FixedAnchor = FTransform::Identity;
	// 控制是否存在判断Fixed锚点的状态标记。
	bool bHasFixedAnchor = false;
};

/** 无独立 Tick；采样时机由现有能力 Task 的 PostPhysics 节拍决定。 */
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeHitDetectionComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	/** 构造无 Tick、无复制的本地检测服务。 */
	UHodgeHitDetectionComponent();
	/** Combat 兼容资产字段只在建立会话前复制配置，运行会话仅由本组件保存。 */
	void ConfigureSources(const TArray<FHodgeHitSource>& InSources) { HitSources = InSources; }
	/** 绑定恢复后重新开放；关闭时必须先清理全部资源。 */
	void EnableDetection() { bEnabled = true; }
	/** 生命周期停止时清理会话及姿势占用，重复调用安全。 */
	void Shutdown();
	/** 仅注册且未关闭的组件接受新的采样请求。 */
	bool CanExecuteAbilities() const { return IsRegistered() && bEnabled; }
	/** 姿势占用按句柄配对，最后一个释放时恢复原策略。 */
	FGuid AcquirePoseLease();
	void ReleasePoseLease(FGuid Handle);
	/** 查询唯一会话与姿势账本，供兼容入口及验证使用。 */
	int32 GetPoseLeaseCount() const { return PoseLeases.Num(); }
	int32 GetDetectionSessionCount() const { return Sessions.Num(); }
	/** 下列入口固定执行身份，旧执行不能采样或结束新会话。 */
	uint64 CreateDetectionSession(const FGuid& ExecutionId, int32 OccurrenceId, const FHodgeHitDetectionRequest& Request);
	bool SampleDetection(uint64 Handle, const FGuid& ExecutionId, FHodgeHitDetectionBatch& OutBatch);
	bool ResetDetectionHistory(uint64 Handle, const FGuid& ExecutionId);
	bool IsDetectionSessionValid(uint64 Handle, const FGuid& ExecutionId) const;
	USceneComponent* GetDetectionSourceComponent(uint64 Handle, const FGuid& ExecutionId) const;
	void EndDetectionSession(uint64 Handle, const FGuid& ExecutionId);
	void EndDetectionSessionsForExecution(const FGuid& ExecutionId);
	void EndAllDetectionSessions();
	void UpdateDetectionAnchor(const FGuid& ExecutionId, FName Key, const FTransform& Transform);
	/** 来源准备与依赖由检测服务解析，Task 只保留原 PostPhysics 节拍。 */
	void AddSamplingPrerequisites(FTickFunction& Tick, uint64 Handle, const FGuid& ExecutionId) const;
	bool PrepareSourcePose(uint64 Handle, const FGuid& ExecutionId) const;
protected:
	/** 非 Actor 销毁的组件移除也必须释放服务器骨骼策略。 */
	virtual void OnUnregister() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
	bool ResolveSource(FGameplayTag Tag, FHodgeHitDetectionSession& Session) const;
	bool IsSourceValid(const FHodgeHitDetectionSession& Session) const;
	bool CaptureDetectionGeometry(const FHodgeHitDetectionSession& Session, FHodgeHitGeometry& Out) const;
	UPROPERTY(Transient) TArray<FHodgeHitSource> HitSources;
	UPROPERTY(Transient) TMap<uint64, FHodgeHitDetectionSession> Sessions;
	TSet<FGuid> PoseLeases;
	TWeakObjectPtr<USkeletalMeshComponent> PoseMesh;
	uint8 SavedPosePolicy = 0;
	uint64 NextHandle = 0;
	bool bEnabled = true;
};
