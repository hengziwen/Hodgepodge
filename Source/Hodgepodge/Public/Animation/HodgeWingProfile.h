/** @file HodgeWingProfile.h
 * @brief 声明翅膀姿势配置相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HodgeWingProfile.generated.h"

class USkeletalMesh;
class UAnimInstance;
class UAnimMontage;
class UAnimSequence;
class UAnimSequenceBase;
class UMaterialInterface;

/** 独立于攻击时钟的淡出状态；重复关闭不会重新计时。 */
struct HODGEPODGE_API FHodgeWingFadeState
{
	/** 当前材质可见度，零为完全隐藏。 */
	float Amount = 0.f;
	/** 淡出起点，支持从当前可见度开始。 */
	float StartAmount = 0.f;
	/** 已经过的淡出秒数。 */
	float Elapsed = 0.f;
	/** 本轮淡出固定时长。 */
	float Duration = 0.f;
	/** 是否正在淡出。 */
	bool bFading = false;
	/** 恢复完全显示并取消旧淡出。 */
	void Show();
	/** 开始幂等淡出；零或非法时长立即隐藏。 */
	void BeginFade(float Seconds);
	/** 推进有限非负时间步长，非法步长不改变状态。 */
	void Advance(float DeltaSeconds);
	/** 立即清理材质可见度及淡出计时。 */
	void Reset();
};

/** 区分旧程序化姿势和新资源动画，保留旧资产默认行为。 */
UENUM(BlueprintType)
enum class EHodgeWingPlaybackMode : uint8
{
	Procedural,
	AnimationAsset
};

/** 角色动画源秒到翼动画源秒的映射，窗口由源通知定义。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeWingAnimationBinding
{
	GENERATED_BODY()
	/** 白名单中的角色动画，不接受调用方任意指定资源。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimSequenceBase> SourceAnimation;
	/** 使用独立翼骨架的动画。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimSequence> WingAnimation;
	/** 匹配源动画上的显示窗口。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName WindowId = TEXT("AttackWing");
	/** 角色时间映射的原点，单位秒。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float SourceStartTime = 0.f;
	/** 翼时间映射的原点，单位秒。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float WingStartTime = 0.f;
	/** 翼源秒相对角色源秒的倍率。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01")) float PlayRateRatio = 1.f;
	/** 校验资源及时钟配置。
	 * @param Mesh 本次表现使用的翼网格。
	 * @param Errors 收集具体失败诊断。
	 * @return 全部条件合法时返回 true。
	 */
	bool Validate(const USkeletalMesh* Mesh, TArray<FText>& Errors) const;
	/** 将角色源秒转换为有界翼源秒。
	 * @param SourceTime 当前角色动画位置，单位秒。
	 * @return 裁剪后的翼动画位置，非法输入返回零。
	 */
	float EvaluateWingTime(float SourceTime) const;
	/** 判断当前时间是否在同名通知窗口的并集内。
	 * @param SourceTime 当前角色动画位置，单位秒。
	 * @return 处于至少一个有效窗口时返回 true。
	 */
	bool IsWindowOpen(float SourceTime) const;
};

/** 初始化代和唯一身份共同阻止迟到回调结束新表现。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeWingActionHandle
{
	GENERATED_BODY()
	/** 当前表现会话的唯一标识。 */
	UPROPERTY(BlueprintReadOnly) FGuid Id;
	/** 组件重置时递增的初始化代。 */
	UPROPERTY(BlueprintReadOnly) int32 Generation = 0;
	/** 判断句柄是否拥有完整身份。 */
	bool IsValid() const { return Id.IsValid() && Generation > 0; }
};

/** 不持有世界对象的会话状态，供运行时和单元测试共用。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeWingPresentationState
{
	GENERATED_BODY()
	/** 当前会话句柄。 */
	UPROPERTY(BlueprintReadOnly) FHodgeWingActionHandle Handle;
	/** 当前攻击的执行身份。 */
	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
	/** Profile 中的当前映射索引。 */
	UPROPERTY(BlueprintReadOnly) int32 BindingIndex = INDEX_NONE;
	/** 本地角色动画实例的身份。 */
	UPROPERTY(BlueprintReadOnly) int32 SourceInstanceId = INDEX_NONE;
	/** 最近采样的角色源秒。 */
	UPROPERTY(BlueprintReadOnly) float SourceTime = 0.f;
	/** 最近求值的翼源秒。 */
	UPROPERTY(BlueprintReadOnly) float WingTime = 0.f;
	/** 网格是否已完成姿势准备并显示。 */
	UPROPERTY(BlueprintReadOnly) bool bVisible = false;
	/** 当前初始化代，不随每次攻击重置。 */
	UPROPERTY(BlueprintReadOnly) int32 Generation = 1;
	/** 成功的翼骨骼求值次数，便于验证空闲成本。 */
	UPROPERTY(BlueprintReadOnly) int32 PoseEvaluations = 0;
	/** 建立或复用同一来源会话。
	 * @param Binding 已校验的映射索引。
	 * @param Instance 本地源动画实例身份。
	 * @param Execution 本次执行身份。
	 * @return 同一来源复用句柄，非法身份返回空句柄。
	 */
	FHodgeWingActionHandle Begin(int32 Binding, int32 Instance, FGuid Execution);
	/** 仅结束匹配会话。
	 * @param EndingHandle 请求结束的会话身份。
	 * @return 匹配并成功结束时返回 true。
	 */
	bool End(FHodgeWingActionHandle EndingHandle);
	/** 清空会话并使之前的初始化代失效。 */
	void Reset();
};

/** 相对参考姿势的局部旋转；不修改翼骨骼原有平移和缩放。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeWingPose
{
	GENERATED_BODY()
	// 翼根相对参考姿势的局部旋转。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FRotator Root = FRotator::ZeroRotator;
	// 翼中段相对参考姿势的局部旋转。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FRotator Mid = FRotator::ZeroRotator;
	// 翼末端相对参考姿势的局部旋转。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FRotator Tip = FRotator::ZeroRotator;
	/** 根据给定权重混合两份状态或表现数据。
	 * @param A 参与当前计算的第一份输入值。
	 * @param B 参与当前计算的第二份输入值。
	 * @param Alpha 混合权重，由调用方传入。
	 * @return 本次计算、查询或创建得到的翅膀姿势。
	 */
	static FHodgeWingPose Blend(const FHodgeWingPose& A, const FHodgeWingPose& B, float Alpha);
};

/** 独立翅膀的资源、挂点及以角色蒙太奇源秒为时钟的挥翼配置。 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeWingProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	/** 旧模式保留原姿势校验，新模式使用动画映射。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing") EHodgeWingPlaybackMode PlaybackMode = EHodgeWingPlaybackMode::Procedural;
	/** 新动画模式的资源白名单与时间映射。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing") TArray<FHodgeWingAnimationBinding> AnimationBindings;
	/** 开启后服务器创建隐藏翼网格，命中任务按攻击主时钟准备骨骼。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing|Hit") bool bEnableAuthorityHitSampling = false;
	/** 动画窗口关闭或攻击取消后的淡出时长；零表示立即隐藏。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing|Fade", meta=(ClampMin="0", Units="s")) float FadeOutSeconds = .35f;
	/** 翅膀各材质共同使用的可见度标量参数。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing|Fade") FName VisibilityParameter = TEXT("WingVisibility");
	/** 按网格槽位覆盖材质，不修改导入模型的原始材质引用。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hodge|Wing|Fade") TArray<TObjectPtr<UMaterialInterface>> Materials;
	// 该表现对象使用的网格资源或组件。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USkeletalMesh> Mesh;
	// 独立表现网格使用的动画实例类。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UAnimInstance> AnimClass;
	// 表现Actor或网格挂接到角色的插槽名。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AttachSocket = TEXT("WingAttach");
	// 挂接后的相对变换偏移。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FTransform MeshOffset = FTransform::Identity;
	// 驱动当前攻击表现时钟的蒙太奇。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> AttackMontage;
	// 准备阶段的蒙太奇源时间，单位秒。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Units="s", ClampMin="0")) float WindupStart = .05f;
	// 挥击阶段的蒙太奇源时间，单位秒。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Units="s", ClampMin="0")) float StrikeStart = .25f;
	// 挥击达到峰值的蒙太奇源时间，单位秒。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Units="s", ClampMin="0")) float StrikePeak = .4f;
	// 恢复阶段结束的蒙太奇源时间，单位秒。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Units="s", ClampMin="0")) float RecoveryEnd = .75f;
	// 取消后回到休止姿势的混合时间，单位秒。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Units="s", ClampMin="0.01")) float CancelReturnSeconds = .18f;
	// 准备阶段使用的局部骨骼旋转姿势。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeWingPose WindupPose;
	// 挥击阶段使用的局部骨骼旋转姿势。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeWingPose StrikePose;
	/** 判断是否存在有效Timing。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool HasValidTiming() const;
	/** 计算当前配置结果。
	 * @param MontagePosition 蒙太奇位置，由调用方传入。
	 * @return 本次计算、查询或创建得到的翅膀姿势。
	 */
	FHodgeWingPose Evaluate(float MontagePosition) const;
	/** 检查当前配置并收集诊断。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
