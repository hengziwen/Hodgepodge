/** @file HodgeWingComponent.h
 * @brief 声明翅膀表现与采样组件相关的类型、接口与现有处理流程。
 */
#pragma once

#include "Components/ActorComponent.h"
#include "Animation/HodgeWingProfile.h"
#include "HodgeWingComponent.generated.h"

class ACharacter;
class USkeletalMeshComponent;
class UAnimInstance;
class UAnimSequenceBase;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UHodgeAnimNotifyState_WingAction;

/** Experience 注入的翅膀协调组件；角色骨骼 → 驱动参数 → 翼骨骼 → 命中采样。 */
UCLASS(Blueprintable, ClassGroup=(Hodge), meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeWingComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	/** 构造翅膀表现与采样组件并初始化其默认状态。 */
	UHodgeWingComponent();
	// 当前操作使用的配置对象。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Wing") TObjectPtr<UHodgeWingProfile> Profile;
	/** 预览时复用这个自有组件，空名称表示创建独立 WingMesh。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Wing") FName ExistingMeshComponentName;
	// 姿势的数据或对象引用。
	UPROPERTY(BlueprintReadOnly, Transient, Category="Hodge|Wing") FHodgeWingPose Pose;
	// 来源位置的数值或阈值。
	UPROPERTY(BlueprintReadOnly, Transient, Category="Hodge|Wing") float SourcePosition = 0.f;
	// 来源Instance标识的数值或阈值。
	UPROPERTY(BlueprintReadOnly, Transient, Category="Hodge|Wing") int32 SourceInstanceId = INDEX_NONE;
	/** 获取翅膀网格。
	 * @return 翅膀网格的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|Wing") USkeletalMeshComponent* GetWingMesh() const { return WingMesh; }
	/** 初始化翅膀。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Wing") bool InitializeWing();
	/** 根据本地角色动画当前时间刷新显示窗口和翼姿势。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Wing") void RefreshPresentation();
	/** 通知只唤醒协调 Tick，避免在角色骨骼刷新中重入翼求值。 */
	void RequestPresentationRefresh();
	/** 在服务器命中采样前准备对应执行的翼姿势，失败时禁止使用旧骨骼。
	 * @param ExecutionId 当前命中任务的攻击身份。
	 * @return 身份、主动画与骨骼均有效时返回 true。
	 */
	bool PrepareAuthorityHitPose(FGuid ExecutionId);
	/** 建立已校验通知窗口的会话。
	 * @param BindingIndex 源动画的通知索引。
	 * @param InstanceId 本地源动画实例身份。
	 * @param ExecutionId 关联的攻击执行身份。
	 * @return 合法身份对应的表现句柄，失败返回空句柄。
	 */
	FHodgeWingActionHandle BeginWingAction(int32 BindingIndex, int32 InstanceId, FGuid ExecutionId);
	/** 结束匹配会话。
	 * @param Handle 请求结束的表现句柄。
	 * @return 成功结束时返回 true，旧句柄或重复结束返回 false。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Wing") bool EndWingAction(FHodgeWingActionHandle Handle);
	/** 只清理该攻击执行拥有的翼表现。
	 * @param ExecutionId 正在结束的攻击执行身份。
	 */
	void EndWingForExecution(FGuid ExecutionId);
	/** 隐藏并使之前的表现句柄失效。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Wing") void ResetWingPresentation();
	/** 返回不允许外部修改的表现诊断快照。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Wing") FHodgeWingPresentationState GetPresentationState() const { return Presentation; }
	/** 返回实际材质可见度，淡出期间介于零与一之间。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Wing") float GetVisibilityAmount() const { return Fade.Amount; }
	/** 判断渲染尾迹是否正在淡出，不表示攻击判定仍有效。 */
	UFUNCTION(BlueprintPure, Category="Hodge|Wing") bool IsFadingOut() const { return Fade.bFading; }
	/** 更新姿势。
	 * @param Position 位置，由调用方传入。
	 * @param InstanceId 用于区分运行实例的标识。
	 * @param bActive 是否活动的控制标记。
	 * @param DeltaSeconds 本次更新的时间步长，单位秒。
	 */
	void UpdatePose(float Position, int32 InstanceId, bool bActive, float DeltaSeconds);
protected:
	/** 进入世界运行阶段并接入本对象的生命周期依赖。 */
	virtual void BeginPlay() override;
	/** 离开世界运行阶段并解除本对象持有的监听或资源。
	 * @param Reason Reason，由调用方传入。
	 */
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	/** 更新组件。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param TickType 帧更新类型，由调用方传入。
	 * @param ThisTickFunction This帧更新Function，由调用方传入。
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	/** 查询当前角色动画中的有效 Wing 通知窗口。
	 * @param BindingIndex 输出通知索引。
	 * @param InstanceId 输出本地实例身份。
	 * @param Position 输出源动画秒。
	 * @param SourceInstance 输出角色动画实例。
	 * @return 找到仍有效的有限时钟时返回 true。
	 */
	bool ResolveSource(int32& BindingIndex, int32& InstanceId, float& Position, UAnimInstance*& SourceInstance);
	/** 根据窗口通知求值真正的翼蒙太奇，角色时间是唯一播放时钟。 */
	bool EvaluateWindowPose(float Position);
	/** 当前窗口的配置来自源动画资产，不复制到 Profile。 */
	UPROPERTY(Transient) TObjectPtr<UHodgeAnimNotifyState_WingAction> ActiveWindow;
	/** 当前源资源用于区分同索引的不同攻击。 */
	UPROPERTY(Transient) TObjectPtr<UAnimSequenceBase> ActiveSourceAnimation;
	/** 当前窗口开始的角色源秒。 */
	float WindowStartTime = 0.f;
	/** 最近已求值的翼蒙太奇实例身份。 */
	int32 WingMontageInstanceId = INDEX_NONE;
	/** 当前窗口的淡出配置，结束后仍保留给材质尾迹使用。 */
	float WindowFadeOutSeconds = -1.f;
	/** 非法通知每个对象仅报告一次，防止逐帧刷日志。 */
	TSet<TWeakObjectPtr<UHodgeAnimNotifyState_WingAction>> InvalidWindows;
	/** 应用组件可见性，隐藏时关闭翼网格自动 Tick。 */
	void SetWingVisible(bool bVisible);
	/** 保留最后一帧姿势，开始淡出并同步渲染状态。 */
	void BeginFadeOut();
	/** 同步所有槽位的材质可见度和网格显示。 */
	void ApplyFadeVisibility();
	/** 每个网格独立持有动态材质，避免影响其他角色。 */
	UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;
	/** 复用用户网格时保留原材质，解绑后恢复，避免动态实例堆叠。 */
	UPROPERTY(Transient) TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;
	/** 淡出只推进材质，不重新求值已经结束的翼动画。 */
	FHodgeWingFadeState Fade;
	/** 判断死亡或 Avatar 更换是否禁止本组件继续显示。 */
	bool IsOwnerPresentationBlocked() const;
	/** 新动画模式的游戏线程会话及诊断数据。 */
	FHodgeWingPresentationState Presentation;
	/** 最近驱动当前会话的角色动画实例。 */
	TWeakObjectPtr<UAnimInstance> ActiveSourceInstance;
	/** 被明确取消的源不能在下一 Tick 自动复活。 */
	TWeakObjectPtr<UAnimInstance> SuppressedSourceInstance;
	/** 被取消源的本地实例身份。 */
	int32 SuppressedInstanceId = INDEX_NONE;
	/** 预览单节点实例的当前资源。 */
	TWeakObjectPtr<UAnimSequenceBase> PreviewSource;
	/** 预览循环或资源更换时递增的实例身份。 */
	int32 PreviewEpoch = 0;
	/** 用于检测预览循环的上一次源秒。 */
	float LastPreviewPosition = -1.f;
	/** 已绑定资源，防止成功初始化后创建重复网格。 */
	bool bInitialized = false;
	/** 永久配置错误只报告一次，显式初始化可以重试。 */
	bool bInitializationFailed = false;
	/** 防止通知和求值回调重入同一次更新。 */
	bool bRefreshing = false;
	/** 隐藏角色的挂点矩阵每帧最多补刷新一次，多个翼命中窗口共用。 */
	uint64 LastAuthorityBodyRefreshFrame = MAX_uint64;
	/** 只有本组件创建的网格才由本组件销毁。 */
	bool bOwnsWingMesh = false;
	// 翅膀网格的数据或对象引用。
	UPROPERTY(Transient) TObjectPtr<USkeletalMeshComponent> WingMesh;
	// 角色的弱引用，不延长目标对象生命周期。
	TWeakObjectPtr<ACharacter> Character;
	// 返回来源的数据或对象引用。
	FHodgeWingPose ReturnFrom;
	// 返回已过时间的数值或阈值。
	float ReturnElapsed = 0.f;
	// 控制是否Was活动的状态标记。
	bool bWasActive = false;
};
