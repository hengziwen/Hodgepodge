/** @file HodgeCombatAnimNotifies.h
 * @brief 声明战斗动画通知相关的类型、接口与现有处理流程。
 */
#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Combat/HodgeHitDetection.h"
#include "Equipment/HodgeWeaponPresentationTypes.h"
#include "HodgeCombatAnimNotifies.generated.h"

/** 定义动画命中来源的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeAnimHitSource : uint8 { /* 角色网格插槽选项。 */ CharacterMeshSocket, /* Equipped武器选项。 */ EquippedWeapon, /* 表现角色根节点选项。 */ AvatarRoot, /* Named组件选项。 */ NamedComponent, /* 执行锚点选项。 */ ExecutionAnchor, /* 执行目标选项。 */ ExecutionTarget };

/** 命中时机直接在蒙太奇编辑，查询与效果参数随该通知保存。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAnimHitConfig
{
	GENERATED_BODY()
	/** 构造动画命中配置并初始化其默认状态。 */
	FHodgeAnimHitConfig();
	// 身体部位直接使用本次通知的 Mesh，不需要注册来源标签。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeAnimHitSource Source = EHodgeAnimHitSource::CharacterMeshSocket;
	// 角色骨骼或插槽名，如 Bip001LHand；NamedComponent 可留空取组件原点。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source==EHodgeAnimHitSource::CharacterMeshSocket || Source==EHodgeAnimHitSource::NamedComponent", EditConditionHides)) FName BoneOrSocket;
	// EquippedWeapon 使用武器实例已有的刀根刀尖来源。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source.Weapon", EditCondition="Source==EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) FGameplayTag WeaponSourceTag;
	// NamedComponent 使用当前 Avatar 上的实际组件对象名。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source==EHodgeAnimHitSource::NamedComponent", EditConditionHides)) FName ComponentName;
	// 检测体形状；武器来源沿用武器的采样点及半径。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) EHodgeHitShape Shape = EHodgeHitShape::Sphere;
	// 球体或胶囊半径，单位厘米。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm", EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon && Shape!=EHodgeHitShape::Box", EditConditionHides)) float Radius = 15.f;
	// 盒体三个轴的半尺寸，单位厘米。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon && Shape==EHodgeHitShape::Box", EditConditionHides)) FVector BoxHalfExtent = FVector(100.f);
	// 胶囊半高包含端帽，不能小于 Radius。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm", EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon && Shape==EHodgeHitShape::Capsule", EditConditionHides)) float CapsuleHalfHeight = 100.f;
	// 相对骨骼、角色根或运行锚点的偏移，Scale 必须为 1。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) FTransform LocalTransform = FTransform::Identity;
	// Follow 跟随锚点，SnapshotOnEventEnter 在通知进入时固定锚点。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source!=EHodgeAnimHitSource::EquippedWeapon", EditConditionHides)) EHodgeHitTransformPolicy TransformPolicy = EHodgeHitTransformPolicy::Follow;
	// 可选共享查询配置，留空采用 Pawn／Sweep／视线检查默认值。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) TObjectPtr<UHodgeHitDetectionProfile> Profile;
	// ExecutionAnchor 对应 SetHitAnchor 提供的服务器运行键。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="Source==EHodgeAnimHitSource::ExecutionAnchor", EditConditionHides)) FName AnchorKey;
	// 范围内所有目标、范围内指定目标或服务器确认目标。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
	// 指定目标或目标锚点对应 SetHitTarget 的键。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="TargetPolicy!=EHodgeHitTargetPolicy::AnyInVolume || Source==EHodgeAnimHitSource::ExecutionTarget", EditConditionHides)) FName TargetKey;
	// 限制指定目标离施法者的距离，单位厘米。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="1", EditCondition="TargetPolicy!=EHodgeHitTargetPolicy::AnyInVolume || Source==EHodgeAnimHitSource::ExecutionTarget", EditConditionHides)) float MaxTargetDistance = 2000.f;
	// 限制运行锚点离施法者的距离，单位厘米。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(ClampMin="1")) float MaxAnchorDistance = 5000.f;
	// 开启时继承 Definition 默认 GE、倍率与伤害类型，仍应用本通知的倍率。
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bUseDefaultDamage = true;
	// 反应覆盖独立于伤害覆盖，终结刀无需复制伤害配置。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction") bool bUseDefaultReaction = true;
	// 本分段是击飞资格的唯一作者入口，不从默认削韧或默认 Impact 隐式继承。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction", meta=(DisplayName="Can Launch", ToolTip="允许本命中分段触发或续接 Fly；关闭时空中目标正常落地起身。")) bool bCanLaunch = false;
	// 受击Override的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction", meta=(EditCondition="!bUseDefaultReaction", EditConditionHides)) FHodgeHitReactionConfig ReactionOverride;
	// 关闭默认伤害时使用的显式效果参数。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bUseDefaultDamage", EditConditionHides)) FHodgeHitEffectConfig Damage;
	// 乘在默认或显式伤害倍率上，允许同一技能的终结段单独调整。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float DamageScale = 1.f;
	// 0 表示本记录内每目标一次，正数允许按世界时间重复命中。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float RepeatHitInterval = 0.f;
	// 留空每次进入独立去重；非空可共享整次执行或显式攻击阶段。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) FName HitGroup;
	// 非空时与 HitGroup 一起区分同一技能内不同刀，不能使用开始时间作为键。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) FName AttackPhase;
	// 是否允许命中同队目标，仍不允许自身或已经死亡的目标。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay) bool bAllowFriendlyFire = false;
	/** 检查当前配置并收集诊断。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Validate(TArray<FText>& Errors) const;
	/** 解析默认／覆盖反应后，始终使用本分段的击飞开关。 */
	FHodgeHitReactionConfig ResolveReaction(const FHodgeHitReactionConfig& DefaultReaction) const;
};

/** 持续命中区间；通知对象只存配置，运行状态属于当前 GA。 */
UCLASS(meta=(DisplayName="Hodge 持续命中"))
class HODGEPODGE_API UHodgeAnimNotifyState_HitCheck : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	/** 构造动画通知状态命中检查并初始化其默认状态。 */
	UHodgeAnimNotifyState_HitCheck();
#if WITH_EDITOR
	/** 处理动画通知Created输入编辑器。
	 * @param Event 事件，由调用方传入。
	 */
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
#endif
	// 本段检测来源、形状、效果和去重参数。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit") FHodgeAnimHitConfig Hit;
	/** 通知开始。
	 * @param Mesh 网格，由调用方传入。
	 * @param Animation 动画，由调用方传入。
	 * @param TotalDuration Total持续时间，由调用方传入。
	 * @param Reference 引用，由调用方传入。
	 */
	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& Reference) override;
	/** 通知结束。
	 * @param Mesh 网格，由调用方传入。
	 * @param Animation 动画，由调用方传入。
	 * @param Reference 引用，由调用方传入。
	 */
	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
	/** 在动画通知区间开始时申请对应资源。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
	/** 在动画通知区间结束时释放对应资源。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
};

/** 封装动画通知命中的状态与操作接口。 */
UCLASS(meta=(DisplayName="Hodge 单次命中"))
class HODGEPODGE_API UHodgeAnimNotify_Hit : public UAnimNotify
{
	GENERATED_BODY()
public:
	/** 构造动画通知命中并初始化其默认状态。 */
	UHodgeAnimNotify_Hit();
#if WITH_EDITOR
	/** 处理动画通知Created输入编辑器。
	 * @param Event 事件，由调用方传入。
	 */
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
#endif
	// 一次触发创建一次独立命中，会在骨骼更新后采样并释放。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit") FHodgeAnimHitConfig Hit;
	// 同一时刻的其他来源放在本通知内，避免引擎同帧 Branching Point 冲突。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category="Hit") TArray<FHodgeAnimHitConfig> AdditionalHits;
	/** 处理当前动画触发的通知入口。
	 * @param Mesh 网格，由调用方传入。
	 * @param Animation 动画，由调用方传入。
	 * @param Reference 引用，由调用方传入。
	 */
	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
	/** 消费匹配当前动画分支点的瞬时通知。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;
};

/** 封装动画通知状态玩法标签的状态与操作接口。 */
UCLASS(meta=(DisplayName="Hodge 状态区间"))
class HODGEPODGE_API UHodgeAnimNotifyState_GameplayTag : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	/** 构造动画通知状态玩法标签并初始化其默认状态。 */
	UHodgeAnimNotifyState_GameplayTag();
#if WITH_EDITOR
	/** 处理动画通知Created输入编辑器。
	 * @param Event 事件，由调用方传入。
	 */
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
#endif
	// 作用域状态，如 Status.Rotation.Locked 或 Status.Attack.Cancel.NextAttack。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="State") FGameplayTag StateTag;
	/** 通知开始。
	 * @param Mesh 网格，由调用方传入。
	 * @param Animation 动画，由调用方传入。
	 * @param TotalDuration Total持续时间，由调用方传入。
	 * @param Reference 引用，由调用方传入。
	 */
	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& Reference) override;
	/** 通知结束。
	 * @param Mesh 网格，由调用方传入。
	 * @param Animation 动画，由调用方传入。
	 * @param Reference 引用，由调用方传入。
	 */
	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
	/** 在动画通知区间开始时申请对应资源。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
	/** 在动画通知区间结束时释放对应资源。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
};

/** 封装动画通知状态武器手部的状态与操作接口。 */
UCLASS(meta=(DisplayName="Hodge 武器手持区间"))
class HODGEPODGE_API UHodgeAnimNotifyState_WeaponHand : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	/** 最后一个手持请求结束时执行的方式，攻击取消也沿用本次配置。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon", meta=(DisplayName="结束方式"))
	EHodgeWeaponHandEndBehavior EndBehavior = EHodgeWeaponHandEndBehavior::ReturnToBack;
	/** 构造动画通知状态武器手部并初始化其默认状态。 */
	UHodgeAnimNotifyState_WeaponHand();
#if WITH_EDITOR
	/** 处理动画通知Created输入编辑器。
	 * @param Event 事件，由调用方传入。
	 */
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
#endif
	/** 通知开始。
	 * @param Mesh 网格，由调用方传入。
	 * @param Animation 动画，由调用方传入。
	 * @param TotalDuration Total持续时间，由调用方传入。
	 * @param Reference 引用，由调用方传入。
	 */
	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& Reference) override;
	/** 通知结束。
	 * @param Mesh 网格，由调用方传入。
	 * @param Animation 动画，由调用方传入。
	 * @param Reference 引用，由调用方传入。
	 */
	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
	/** 在动画通知区间开始时申请对应资源。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
	/** 在动画通知区间结束时释放对应资源。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
};

/** 封装动画通知玩法事件的状态与操作接口。 */
UCLASS(meta=(DisplayName="Hodge 玩法消息"))
class HODGEPODGE_API UHodgeAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
public:
	/** 构造动画通知玩法事件并初始化其默认状态。 */
	UHodgeAnimNotify_GameplayEvent();
#if WITH_EDITOR
	/** 处理动画通知Created输入编辑器。
	 * @param Event 事件，由调用方传入。
	 */
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) override;
#endif
	// 发送给 ASC 与当前连段协调器的业务消息，普通命中不需要填写。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Event") FGameplayTag EventTag;
	/** 处理当前动画触发的通知入口。
	 * @param Mesh 网格，由调用方传入。
	 * @param Animation 动画，由调用方传入。
	 * @param Reference 引用，由调用方传入。
	 */
	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& Reference) override;
	/** 消费匹配当前动画分支点的瞬时通知。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 */
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;
};
