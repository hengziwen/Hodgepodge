/** @file HodgeCombatAnimNotifies.cpp
 * @brief 实现战斗动画通知相关的类型、接口与现有处理流程。
 */
#include "Animation/HodgeCombatAnimNotifies.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/ActiveMontageInstanceScope.h"
#include "Animation/AnimNotifyQueue.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCombatAnimNotifies)

namespace
{
	/** 解析执行。
	 * @param Payload 动画通知或玩法事件携带的数据。
	 * @return 本次计算、查询或创建得到的玩法能力定义。
	 */
	UHodgeGameplayAbility_Definition* ResolveExecution(const FBranchingPointNotifyPayload& Payload)
	{
		// 先检查对象与上下文，避免在依赖失效后继续处理。
		if (!Payload.SkelMeshComponent || !Payload.NotifyEvent) { return nullptr; }
		// 后续GAS操作使用的能力系统组件。
		auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Payload.SkelMeshComponent->GetOwner());
		auto* GA = ASC ? Cast<UHodgeGameplayAbility_Definition>(ASC->GetAnimatingAbility()) : nullptr;
		UE_LOG(LogTemp, Verbose,
		       TEXT("[HodgeNotify] Resolve %s Authority=%d Instance=%d Source=%s Ability=%s Accept=%d"),
		       *GetNameSafe(Payload.NotifyEvent->Notify ? static_cast<UObject*>(Payload.NotifyEvent->Notify.Get()) :
			       static_cast<UObject*>(Payload.NotifyEvent->NotifyStateClass.Get())),
		       Payload.SkelMeshComponent->GetOwner()->HasAuthority(), Payload.MontageInstanceID,
		       *GetNameSafe(Payload.SequenceAsset), *GetNameSafe(GA), GA && GA->AcceptsNotify(Payload));
		return GA && GA->AcceptsNotify(Payload) ? GA : nullptr;
	}
}

/** 构造动画命中配置并初始化其默认状态。 */
FHodgeAnimHitConfig::FHodgeAnimHitConfig()
{
	WeaponSourceTag = HodgeGameplayTags::Combat_Source_Weapon_MainHand;
}

/** 检查当前配置并收集诊断。 */
bool FHodgeAnimHitConfig::Validate(TArray<FText>& Errors) const
{
	const int32 Before = Errors.Num();
	if (!bUseDefaultReaction) { ReactionOverride.Validate(Errors); }
	auto Error = [&Errors](const TCHAR* Message) { Errors.Add(FText::FromString(Message)); };
	if (Source == EHodgeAnimHitSource::CharacterMeshSocket && BoneOrSocket.IsNone())
	{
		Error(TEXT("CharacterMeshSocket requires BoneOrSocket."));
	}
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (Source == EHodgeAnimHitSource::EquippedWeapon && !WeaponSourceTag.IsValid())
	{
		Error(TEXT("EquippedWeapon requires WeaponSourceTag."));
	}
	if (Source == EHodgeAnimHitSource::NamedComponent && ComponentName.IsNone())
	{
		Error(TEXT("NamedComponent requires ComponentName."));
	}
	if (Source == EHodgeAnimHitSource::ExecutionAnchor && AnchorKey.IsNone())
	{
		Error(TEXT("ExecutionAnchor requires AnchorKey."));
	}
	if ((Source == EHodgeAnimHitSource::ExecutionTarget || TargetPolicy != EHodgeHitTargetPolicy::AnyInVolume) &&
		TargetKey.IsNone()) { Error(TEXT("Selected targets require TargetKey.")); }
	if (!AttackPhase.IsNone() && HitGroup.IsNone()) { Error(TEXT("AttackPhase requires an explicit HitGroup.")); }
	if (LocalTransform.ContainsNaN() || !LocalTransform.GetRotation().IsNormalized() || !LocalTransform.GetScale3D().
		Equals(FVector::OneVector)) { Error(TEXT("LocalTransform must be finite, normalized and unit scale.")); }
	if (!FMath::IsFinite(Radius) || Radius <= 0.f || BoxHalfExtent.ContainsNaN() || BoxHalfExtent.GetMin() <= 0.f ||
		!FMath::IsFinite(CapsuleHalfHeight) || (Shape == EHodgeHitShape::Capsule && CapsuleHalfHeight < Radius) ||
		!FMath::IsFinite(DamageScale) || DamageScale < 0.f || !FMath::IsFinite(RepeatHitInterval) || RepeatHitInterval <
		0.f ||
		!FMath::IsFinite(MaxTargetDistance) || MaxTargetDistance <= 0.f || !FMath::IsFinite(MaxAnchorDistance) ||
		MaxAnchorDistance <= 0.f)
	{
		Error(TEXT("Invalid hit dimensions, multiplier, interval or distance."));
	}
	if (Profile) { Profile->Validate(Errors); }
	return Errors.Num() == Before;
}

/** 构造动画通知状态命中检查并初始化其默认状态。 */
UHodgeAnimNotifyState_HitCheck::UHodgeAnimNotifyState_HitCheck() { bIsNativeBranchingPoint = true; }

/** 构造动画通知命中并初始化其默认状态。 */
UHodgeAnimNotify_Hit::UHodgeAnimNotify_Hit()
{
	bIsNativeBranchingPoint = true;
	Hit.Source = EHodgeAnimHitSource::AvatarRoot;
}

/** 构造动画通知状态玩法标签并初始化其默认状态。 */
UHodgeAnimNotifyState_GameplayTag::UHodgeAnimNotifyState_GameplayTag() { bIsNativeBranchingPoint = true; }
/** 构造动画通知状态武器手部并初始化其默认状态。 */
UHodgeAnimNotifyState_WeaponHand::UHodgeAnimNotifyState_WeaponHand() { bIsNativeBranchingPoint = true; }
/** 构造动画通知玩法事件并初始化其默认状态。 */
UHodgeAnimNotify_GameplayEvent::UHodgeAnimNotify_GameplayEvent() { bIsNativeBranchingPoint = true; }

/** 在动画通知区间开始时申请对应资源。 */
void UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = Cast<UHodgeGameplayAbility_Melee>(ResolveExecution(Payload)))
	{
		const int32 Occurrence = GA->BeginNotifyResource(Payload);
		if (Occurrence != INDEX_NONE) { GA->BeginNotifyHit(Occurrence, Hit, Payload.SkelMeshComponent, false); }
	}
}

/** 在动画通知区间结束时释放对应资源。 */
void UHodgeAnimNotifyState_HitCheck::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload)) { GA->EndNotifyResource(Payload); }
}

/** 消费匹配当前动画分支点的瞬时通知。 */
void UHodgeAnimNotify_Hit::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = Cast<UHodgeGameplayAbility_Melee>(ResolveExecution(Payload)))
	{
		const int32 Occurrence = GA->AllocateNotifyOccurrence();
		UE_LOG(LogTemp, Verbose, TEXT("[HodgeNotify] Point %s World=%s Authority=%d Occurrence=%d"), *GetName(),
		       *GA->GetWorld()->GetName(), GA->GetAvatarActorFromActorInfo()->HasAuthority(), Occurrence);
		GA->BeginNotifyHit(Occurrence, Hit, Payload.SkelMeshComponent, true);
		const FGuid Execution = GA->GetExecutionId();
		for (const auto& Config : AdditionalHits)
		{
			if (!GA->IsActive() || GA->GetExecutionId() != Execution) { break; }
			GA->BeginNotifyHit(GA->AllocateNotifyOccurrence(), Config, Payload.SkelMeshComponent, true);
		}
	}
}

/** 在动画通知区间开始时申请对应资源。 */
void UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload))
	{
		const int32 Occurrence = GA->BeginNotifyResource(Payload);
		if (Occurrence != INDEX_NONE) { GA->AcquireNotifyTag(Occurrence, StateTag); }
	}
}

/** 在动画通知区间结束时释放对应资源。 */
void UHodgeAnimNotifyState_GameplayTag::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload)) { GA->EndNotifyResource(Payload); }
}

/** 在动画通知区间开始时申请对应资源。 */
void UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload))
	{
		const int32 Occurrence = GA->BeginNotifyResource(Payload);
		if (Occurrence != INDEX_NONE) { GA->AcquireNotifyWeapon(Occurrence, EndBehavior); }
	}
}

/** 在动画通知区间结束时释放对应资源。 */
void UHodgeAnimNotifyState_WeaponHand::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload)) { GA->EndNotifyResource(Payload); }
}

/** 消费匹配当前动画分支点的瞬时通知。 */
void UHodgeAnimNotify_GameplayEvent::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)
{
	if (auto* GA = ResolveExecution(Payload)) { GA->SendExecutionEvent(EventTag); }
}

namespace
{
	/** 整理当前蒙太奇分支点通知的载荷。
	 * @param Mesh 网格，由调用方传入。
	 * @param Reference 引用，由调用方传入。
	 * @return 本次计算、查询或创建得到的BranchingPoint通知载荷。
	 */
	FBranchingPointNotifyPayload MontagePayload(USkeletalMeshComponent* Mesh,
	                                            const FAnimNotifyEventReference& Reference)
	{
		const auto* Context = Reference.GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>();
		return FBranchingPointNotifyPayload(
			Mesh, const_cast<UAnimSequenceBase*>(Cast<UAnimSequenceBase>(Reference.GetSourceObject())),
			const_cast<FAnimNotifyEvent*>(Reference.GetNotify()), Context ? Context->MontageInstanceID : INDEX_NONE);
	}
}

/** 通知开始。 */
void UHodgeAnimNotifyState_HitCheck::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                 float TotalDuration, const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyBegin(Payload);
}

/** 通知结束。 */
void UHodgeAnimNotifyState_HitCheck::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                               const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyEnd(Payload);
}

/** 通知开始。 */
void UHodgeAnimNotifyState_GameplayTag::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                    float TotalDuration, const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyBegin(Payload);
}

/** 通知结束。 */
void UHodgeAnimNotifyState_GameplayTag::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                  const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyEnd(Payload);
}

/** 通知开始。 */
void UHodgeAnimNotifyState_WeaponHand::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                   float TotalDuration, const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyBegin(Payload);
}

/** 通知结束。 */
void UHodgeAnimNotifyState_WeaponHand::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                 const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotifyEnd(Payload);
}

/** 处理当前动画触发的通知入口。 */
void UHodgeAnimNotify_Hit::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotify(Payload);
}

/** 处理当前动画触发的通知入口。 */
void UHodgeAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                            const FAnimNotifyEventReference& Reference)
{
	FBranchingPointNotifyPayload Payload = MontagePayload(Mesh, Reference);
	BranchingPointNotify(Payload);
}

#if WITH_EDITOR
namespace
{
	/** 配置玩法通知。
	 * @param Event 事件，由调用方传入。
	 */
	void ConfigureGameplayNotify(FAnimNotifyEvent& Event)
	{
		// 玩法时机不随姿势混合、LOD、随机概率或动画图事件过滤消失。
		Event.MontageTickType = EMontageNotifyTickType::BranchingPoint;
		Event.TriggerWeightThreshold = 0.f;
		Event.NotifyTriggerChance = 1.f;
		Event.NotifyFilterType = ENotifyFilterType::NoFiltering;
		Event.bCanBeFilteredViaRequest = false;
		Event.bTriggerOnDedicatedServer = true;
		Event.bTriggerOnFollower = true;
	}
}

/** 处理动画通知Created输入编辑器。 */
void UHodgeAnimNotifyState_HitCheck::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)
{
	ConfigureGameplayNotify(Event);
}

/** 处理动画通知Created输入编辑器。 */
void UHodgeAnimNotify_Hit::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event) { ConfigureGameplayNotify(Event); }

/** 处理动画通知Created输入编辑器。 */
void UHodgeAnimNotifyState_GameplayTag::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)
{
	ConfigureGameplayNotify(Event);
}

/** 处理动画通知Created输入编辑器。 */
void UHodgeAnimNotifyState_WeaponHand::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)
{
	ConfigureGameplayNotify(Event);
}

/** 处理动画通知Created输入编辑器。 */
void UHodgeAnimNotify_GameplayEvent::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& Event)
{
	ConfigureGameplayNotify(Event);
}
#endif
