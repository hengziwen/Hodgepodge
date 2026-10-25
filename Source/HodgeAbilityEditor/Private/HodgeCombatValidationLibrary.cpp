/** @file HodgeCombatValidationLibrary.cpp
 * @brief 实现战斗PIE检查与调度函数库相关的类型、接口与现有处理流程。
 */
#include "HodgeCombatValidationLibrary.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Component/HodgeCharacterRotationComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimClassInterface.h"
#include "Animation/AnimNode_SequencePlayer.h"
#include "AnimNodes/AnimNode_SequenceEvaluator.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/UnrealType.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Data/HodgeAbilityDefinition.h"
#include "TimerManager.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Component/HodgeCombatComponentBase.h"
#include "Editor.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "Character/HodgeEnemyCharacter.h"
#include "Engine/StaticMeshActor.h"
#include "Animation/Skeleton.h"
#include "AbilitySystem/AttributeSet/HodgeResonanceSet.h"
#include "AbilitySystem/Effects/HodgeResonanceEnergyEffect.h"
#include "AbilitySystem/AttributeSet/HodgeHavocSet.h"
#include "AbilitySystem/Effects/HodgeHavocEffects.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "GameplayEffect.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCombatValidationLibrary)

/** 仅在权威 PIE 世界操作测试对象，回调检查双方 Avatar 身份，避免影响重生后的角色。 */
bool UHodgeCombatValidationLibrary::QueueNumberPopProbe(AActor* Source, AActor* Target, float Damage, bool bResetTargetHealth)
{
	if (!IsValid(Source) || !IsValid(Target) || !Source->HasAuthority() || !Target->HasAuthority() ||
		Source->GetWorld() != Target->GetWorld() || !Target->GetWorld() || Target->GetWorld()->WorldType != EWorldType::PIE ||
		!FMath::IsFinite(Damage) || Damage < 0.f) { return false; }
	const TWeakObjectPtr<AActor> WeakSource = Source, WeakTarget = Target;
	Target->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakSource, WeakTarget, Damage, bResetTargetHealth]()
	{
		auto* S = WeakSource.IsValid() ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(WeakSource.Get()) : nullptr;
		auto* T = WeakTarget.IsValid() ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(WeakTarget.Get()) : nullptr;
		if (!S || !T || S->GetAvatarActor() != WeakSource.Get() || T->GetAvatarActor() != WeakTarget.Get() || !T->GetSet<UHodgeHealthSet>()) { return; }
		if (bResetTargetHealth)
		{
			T->SetNumericAttributeBase(UHodgeHealthSet::GetMaxHealthAttribute(), 100000.f);
			T->SetNumericAttributeBase(UHodgeHealthSet::GetHealthAttribute(), 100000.f);
		}
		if (Damage <= 0.f) { return; }
		auto* Effect = NewObject<UGameplayEffect>();
		Effect->DurationPolicy = EGameplayEffectDurationType::Instant;
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UHodgeHealthSet::GetDamageAttribute(); Modifier.ModifierOp = EGameplayModOp::Additive;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Damage));
		Effect->Modifiers.Add(Modifier);
		FGameplayEffectSpec Spec(Effect, S->MakeEffectContext(), 1.f);
		T->ApplyGameplayEffectSpecToSelf(Spec);
	}));
	return true;
}

/** 读取实际运动源，避免把配置成根运动但没有真正提取位移误报为成功。 */
FVector UHodgeCombatValidationLibrary::InspectReactionMotion(AActor* Avatar)
{
	const auto* Character = Cast<ACharacter>(Avatar);
	const auto* Movement = Character ? Character->GetCharacterMovement() : nullptr;
	// HasRootMotionSources 也包含动画根运动；此处必须只读取外部 source group。
	return Movement ? FVector(Movement->HasAnimRootMotion() ? 1.f : 0.f, Movement->CurrentRootMotion.HasActiveRootMotionSources() ? 1.f : 0.f,
		Character->IsPlayingRootMotion() ? 1.f : 0.f) : FVector(-1.f);
}

bool UHodgeCombatValidationLibrary::QueueDarkFlowDelta(AActor* Avatar, float Delta)
{
	if (!Avatar || !Avatar->HasAuthority() || !Avatar->GetWorld() || Avatar->GetWorld()->WorldType!=EWorldType::PIE || !FMath::IsFinite(Delta)) { return false; }
	const TWeakObjectPtr<AActor> Weak=Avatar;
	Avatar->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([Weak,Delta]()
	{
		auto* ASC=Weak.IsValid() ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Weak.Get()) : nullptr;
		if (!ASC || ASC->GetAvatarActor()!=Weak.Get() || !ASC->GetSet<UHodgeHavocSet>()) { return; }
		auto Spec=ASC->MakeOutgoingSpec(UHodgeDarkFlowEffect::StaticClass(),1.f,ASC->MakeEffectContext());
		if (Spec.IsValid()) { Spec.Data->SetSetByCallerMagnitude(HodgeHavocTags::DarkFlowDelta,Delta); ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()); }
	}));
	return true;
}

FVector2D UHodgeCombatValidationLibrary::InspectDarkFlow(AActor* Avatar)
{
	const auto* ASC=Avatar ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Avatar) : nullptr;
	const auto* Set=ASC ? ASC->GetSet<UHodgeHavocSet>() : nullptr;
	return Set ? FVector2D(Set->GetDarkFlow(),Set->GetMaxDarkFlow()) : FVector2D(-1.,-1.);
}

bool UHodgeCombatValidationLibrary::QueueDarkSurgeDispel(AActor* Avatar)
{
	if (!Avatar || !Avatar->HasAuthority() || !Avatar->GetWorld() || Avatar->GetWorld()->WorldType!=EWorldType::PIE) { return false; }
	const TWeakObjectPtr<AActor> Weak=Avatar;
	Avatar->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([Weak]()
	{
		auto* ASC=Weak.IsValid() ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Weak.Get()) : nullptr;
		if (!ASC || ASC->GetAvatarActor()!=Weak.Get()) { return; }
		// 类型限定只撤销暗涌状态；生产移除委托负责配对清理与清空燃料。
		for (const auto Handle : ASC->GetActiveEffects(FGameplayEffectQuery()))
		{
			const auto* Effect=ASC->GetActiveGameplayEffect(Handle);
			if (Effect && Effect->Spec.Def && Effect->Spec.Def->IsA<UHodgeDarkSurgeEffect>()) { ASC->RemoveActiveGameplayEffect(Handle); }
		}
	}));
	return true;
}

bool UHodgeCombatValidationLibrary::QueueResonanceEnergyDelta(AActor* Avatar, float Delta)
{
	if (!Avatar || !Avatar->HasAuthority() || !Avatar->GetWorld() || Avatar->GetWorld()->WorldType!=EWorldType::PIE || !FMath::IsFinite(Delta)) { return false; }
	const TWeakObjectPtr<AActor> Weak=Avatar;
	Avatar->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([Weak,Delta]()
	{
		if (!Weak.IsValid()) { return; }
		auto* ASC=UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Weak.Get());
		if (!ASC || ASC->GetAvatarActor()!=Weak.Get() || !ASC->GetSet<UHodgeResonanceSet>()) { return; }
		auto Spec=ASC->MakeOutgoingSpec(UHodgeResonanceEnergyEffect::StaticClass(),1.f,ASC->MakeEffectContext());
		if (Spec.IsValid()) { Spec.Data->SetSetByCallerMagnitude(HodgeResonanceTags::EnergyDelta,Delta); ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()); }
	}));
	return true;
}

FVector2D UHodgeCombatValidationLibrary::InspectResonanceEnergy(AActor* Avatar)
{
	const auto* ASC=Avatar ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Avatar) : nullptr;
	const auto* Set=ASC ? ASC->GetSet<UHodgeResonanceSet>() : nullptr;
	return Set ? FVector2D(Set->GetResonanceEnergy(),Set->GetMaxResonanceEnergy()) : FVector2D(-1.,-1.);
}

bool UHodgeCombatValidationLibrary::QueueCinematicSourceProbe(AActor* Avatar, float Position, float PlayRate)
{
	if (!Avatar || !Avatar->GetWorld() || Avatar->GetWorld()->WorldType!=EWorldType::PIE || !FMath::IsFinite(Position) || !FMath::IsFinite(PlayRate) || Position<0.f || PlayRate<0.f) { return false; }
	auto* ASC=UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Avatar);
	auto* Ability=ASC ? Cast<UHodgeGameplayAbility_Definition>(ASC->GetAnimatingAbility()) : nullptr;
	if (!Ability) { return false; }
	const FGuid Execution=Ability->GetExecutionId(); const TWeakObjectPtr<AActor> Weak=Avatar;
	Avatar->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([Weak,Execution,Position,PlayRate]()
	{
		if (!Weak.IsValid()) { return; }
		auto* CurrentASC=UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Weak.Get());
		auto* Current=CurrentASC ? Cast<UHodgeGameplayAbility_Definition>(CurrentASC->GetAnimatingAbility()) : nullptr;
		UAnimMontage* Montage=nullptr; int32 Id=INDEX_NONE; float CurrentPosition=0.f;
		if (!Current || Current->GetExecutionId()!=Execution || !Current->GetCinematicSource(Montage,Id,CurrentPosition)) { return; }
		auto* Anim=CurrentASC->AbilityActorInfo->GetAnimInstance(); auto* Instance=Anim ? Anim->GetMontageInstanceForID(Id) : nullptr;
		if (Instance) { Instance->SetPosition(FMath::Min(Position,Montage->GetPlayLength()-.001f)); Instance->SetPlayRate(PlayRate); }
	}));
	return true;
}

/** 把验证用战斗输入排入后续游戏Tick。 */
bool UHodgeCombatValidationLibrary::QueueCombatInput(AActor* Avatar, FGameplayTag InputTag)
{
	// 按服务器权限区分本次处理分支。
	if (!Avatar || !Avatar->HasAuthority() || !Avatar->GetWorld() || Avatar->GetWorld()->WorldType != EWorldType::PIE || !InputTag.IsValid()) { return false; }
	const TWeakObjectPtr<AActor> Weak = Avatar;
	Avatar->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([Weak, InputTag]()
	{
		if (Weak.IsValid()) { if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(Weak.Get())) { Combat->InputPressed(InputTag); } }
	}));
	return true;
}

/** 把验证用GameplayEvent排入后续游戏Tick。 */
bool UHodgeCombatValidationLibrary::QueueGameplayEvent(UHodgeAbilitySystemComponent* ASC, FGameplayTag EventTag)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!ASC || !ASC->GetWorld() || ASC->GetWorld()->WorldType != EWorldType::PIE || !EventTag.IsValid()) { return false; }
	const TWeakObjectPtr<UHodgeAbilitySystemComponent> WeakASC = ASC;
	ASC->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakASC, EventTag]()
	{
		if (!WeakASC.IsValid()) { return; }
		FGameplayEventData Event; Event.EventTag = EventTag;
		Event.Instigator = Event.Target = WeakASC->GetAvatarActor();
		WeakASC->HandleGameplayEvent(EventTag, &Event);
	}));
	return true;
}

/** 检查朝向Sequences。 */
TArray<FName> UHodgeCombatValidationLibrary::InspectFacingSequences(AActor* Avatar)
{
	TArray<FName> Result;
	const auto* Character = Cast<ACharacter>(Avatar);
	if (!Character || !Character->GetMesh()) { return Result; }
	const USkeletalMeshComponent* Mesh = Character->GetMesh();
	TArray<UAnimInstance*> Instances = Mesh->GetLinkedAnimInstances();
	Instances.Add(Character->GetMesh()->GetAnimInstance());
	for (UAnimInstance* Instance : Instances)
	{
		const IAnimClassInterface* Class = Instance ? IAnimClassInterface::GetFromClass(Instance->GetClass()) : nullptr;
		if (!Class) { continue; }
		for (FStructProperty* Property : Class->GetAnimNodeProperties())
		{
			if (!Property || !Property->Struct->IsChildOf(FAnimNode_SequencePlayerBase::StaticStruct())) { continue; }
			const auto* Node = Property->ContainerPtrToValuePtr<FAnimNode_SequencePlayerBase>(Instance);
			if (Node && Node->GetCachedBlendWeight() > .05f && Node->GetSequence()) { Result.AddUnique(Node->GetSequence()->GetFName()); }
		}
	}
	return Result;
}

/** 检查掉头Times。 */
TArray<FHodgePivotValidationState> UHodgeCombatValidationLibrary::InspectPivotTimes(AActor* Avatar)
{
	TArray<FHodgePivotValidationState> Result;
	const auto* Character = Cast<ACharacter>(Avatar);
	if (!Character || !Character->GetMesh()) { return Result; }
	const USkeletalMeshComponent* Mesh = Character->GetMesh();
	TArray<UAnimInstance*> Instances = Mesh->GetLinkedAnimInstances();
	Instances.Add(Character->GetMesh()->GetAnimInstance());
	for (auto* Instance : Instances)
	{
		const auto* Class = Instance ? IAnimClassInterface::GetFromClass(Instance->GetClass()) : nullptr;
		if (!Class) { continue; }
		for (FStructProperty* Property : Class->GetAnimNodeProperties())
		{
			if (!Property || !Property->Struct->IsChildOf(FAnimNode_SequenceEvaluatorBase::StaticStruct())) { continue; }
			const auto* Node = Property->ContainerPtrToValuePtr<FAnimNode_SequenceEvaluatorBase>(Instance);
			if (!Node || !Node->GetSequence() || Node->GetCachedBlendWeight() <= .05f) { continue; }
			FHodgePivotValidationState& Row = Result.AddDefaulted_GetRef();
			Row.Sequence = Node->GetSequence()->GetFName(); Row.Time = Node->GetExplicitTime(); Row.Weight = Node->GetCachedBlendWeight();
		}
	}
	return Result;
}

/** 检查移动表现同步。 */
TArray<FHodgeSyncValidationState> UHodgeCombatValidationLibrary::InspectLocomotionSync(AActor* Avatar)
{
	TArray<FHodgeSyncValidationState> Result;
	const auto* Character = Cast<ACharacter>(Avatar);
	if (!Character || !Character->GetMesh()) { return Result; }
	const USkeletalMeshComponent* Mesh = Character->GetMesh();
	TArray<UAnimInstance*> Instances = Mesh->GetLinkedAnimInstances();
	Instances.Add(Character->GetMesh()->GetAnimInstance());
	for (const auto* Instance : Instances)
	{
		if (!Instance) { continue; }
		for (const auto& Pair : Instance->GetSyncGroupMapRead())
		{
			for (int32 Index = 0; Index < Pair.Value.ActivePlayers.Num(); ++Index)
			{
				const auto& Tick = Pair.Value.ActivePlayers[Index];
				if (!Tick.SourceAsset || Tick.EffectiveBlendWeight <= .01f) { continue; }
				auto& Row = Result.AddDefaulted_GetRef(); Row.Group = Pair.Key; Row.Sequence = Tick.SourceAsset->GetFName();
				Row.Time = Tick.TimeAccumulator ? *Tick.TimeAccumulator : 0.f; Row.Weight = Tick.EffectiveBlendWeight;
				Row.bLeader = Index == Pair.Value.GroupLeaderIndex; Row.bMarkerSync = Pair.Value.bCanUseMarkerSync;
			}
		}
	}
	return Result;
}

/** 设置朝向校验控制器Permission。 */
bool UHodgeCombatValidationLibrary::SetFacingValidationControllerPermission(AActor* Avatar, bool bAllow)
{
	// 按服务器权限区分本次处理分支。
	if (!Avatar || !Avatar->HasAuthority() || !Avatar->GetWorld() || Avatar->GetWorld()->WorldType != EWorldType::PIE) { return false; }
	// 朝向组件或旋转数据。
	auto* Rotation = Avatar->FindComponentByClass<UHodgeCharacterRotationComponent>();
	auto* Property = FindFProperty<FBoolProperty>(UHodgeCharacterRotationComponent::StaticClass(), TEXT("bAllowControllerFacingRequests"));
	if (!Rotation || !Property) { return false; }
	Property->SetPropertyValue_InContainer(Rotation, bAllow);
	return true;
}

/** 把验证用基础朝向切换排入后续游戏Tick。 */
bool UHodgeCombatValidationLibrary::QueueFacingMode(AActor* Avatar, EHodgeCharacterFacingDriver Driver, bool bRelease)
{
	if (!Avatar || !Avatar->GetWorld() || !Avatar->FindComponentByClass<UHodgeCharacterRotationComponent>()) { return false; }
	const TWeakObjectPtr<AActor> WeakAvatar = Avatar;
	Avatar->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakAvatar, Driver, bRelease]()
	{
		if (!WeakAvatar.IsValid()) { return; }
		auto* Rotation = WeakAvatar->FindComponentByClass<UHodgeCharacterRotationComponent>();
		if (!Rotation) { return; }
		if (bRelease) { Rotation->ReleaseRequestsForSource(WeakAvatar.Get()); }
		else { Rotation->AcquireBaseFacingMode(Driver, WeakAvatar.Get()); }
	}));
	return true;
}

/** 把验证用动作朝向请求排入后续游戏Tick。 */
bool UHodgeCombatValidationLibrary::QueueActionFacing(AActor* Avatar, FVector Direction, bool bRelease, bool bInstant)
{
	if (!Avatar || !Avatar->GetWorld()) { return false; }
	const TWeakObjectPtr<AActor> WeakAvatar = Avatar;
	Avatar->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakAvatar, Direction, bRelease, bInstant]()
	{
		if (!WeakAvatar.IsValid()) { return; }
		auto* Rotation = WeakAvatar->FindComponentByClass<UHodgeCharacterRotationComponent>();
		auto* ASC = Cast<UHodgeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(WeakAvatar.Get()));
		auto* Ability = ASC ? Cast<UHodgeGameplayAbility_Definition>(ASC->GetAnimatingAbility()) : nullptr;
		if (!Rotation || !Ability) { return; }
		if (bRelease) { Rotation->ReleaseRequestsForSource(Ability); }
		else { Rotation->RequestActionFacing(Direction, Ability, Ability->GetExecutionId(), bInstant); }
	}));
	return true;
}

/** 把测试能力激活或结束操作排入后续游戏Tick。 */
bool UHodgeCombatValidationLibrary::QueueAbilityAction(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool bCancel)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!IsValid(ASC) || !Handle.IsValid() || !ASC->GetWorld() || ASC->GetWorld()->WorldType != EWorldType::PIE) { return false; }
	TWeakObjectPtr<UHodgeAbilitySystemComponent> WeakASC = ASC;
	TWeakObjectPtr<AActor> Avatar = ASC->GetAvatarActor();
	// 通过ASC发起激活，保留GAS自身的条件与消耗校验。
	ASC->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakASC, Avatar, Handle, bCancel]()
	{
		if (!WeakASC.IsValid() || !Avatar.IsValid() || WeakASC->GetAvatarActor() != Avatar.Get()) { return; }
		if (bCancel)
		{
			FGameplayAbilitySpec* Spec = WeakASC->FindAbilitySpecFromHandle(Handle);
			UGameplayAbility* Instance = Spec ? Spec->GetPrimaryInstance() : nullptr;
			if (Instance && Instance->IsActive())
			{
				Instance->CancelAbility(Handle, WeakASC->AbilityActorInfo.Get(), Instance->GetCurrentActivationInfo(), true);
			}
		}
		else
		{
			const bool bActivated = WeakASC->TryActivateAbility(Handle, true);
			UE_LOG(LogTemp, Display, TEXT("HodgeValidation native activation: World=%s Authority=%d Result=%d"),
				*WeakASC->GetWorld()->GetName(), WeakASC->IsOwnerActorAuthoritative(), bActivated);
		}
	}));
	return true;
}

namespace
{
	/** 添加制作通知。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param Notify 通知，由调用方传入。
	 * @param Start 起始，由调用方传入。
	 * @param End 结束，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool AddAuthoringNotify(UAnimMontage* Montage, UObject* Notify, float Start, float End)
	{
		if (!Montage || !Notify || Start < 0.f || Start >= Montage->GetPlayLength() || End > Montage->GetPlayLength() || End < Start) { return false; }
		Montage->Modify();
		FAnimNotifyEvent Event;
		Event.Notify = Cast<UAnimNotify>(Notify);
		Event.NotifyStateClass = Cast<UAnimNotifyState>(Notify);
		if (Event.NotifyStateClass && End <= Start) { return false; }
		Event.Guid = FGuid::NewGuid();
		Event.NotifyName = Notify->GetFName();
		Event.TrackIndex = Montage->AnimNotifyTracks.Num();
		Event.TriggerWeightThreshold = 0.f;
		Event.bTriggerOnDedicatedServer = true;
		Event.Link(Montage, Start);
		Event.TriggerTimeOffset = GetTriggerTimeOffsetForType(Montage->CalculateOffsetForNotify(Start));
		if (Event.NotifyStateClass)
		{
			Event.SetDuration(End - Start);
			Event.EndLink.Link(Montage, End);
			Event.EndTriggerTimeOffset = GetTriggerTimeOffsetForType(Montage->CalculateOffsetForNotify(End));
		}
		FAnimNotifyTrack Track;
		Track.TrackName = Notify->GetFName();
		Montage->AnimNotifyTracks.Add(Track);
		if (Event.Notify) { Event.Notify->OnAnimNotifyCreatedInEditor(Event); }
		if (Event.NotifyStateClass) { Event.NotifyStateClass->OnAnimNotifyCreatedInEditor(Event); }
		Montage->Notifies.Add(Event);
		Montage->RefreshCacheData();
		Montage->MarkPackageDirty();
		return true;
	}
}

/** 添加命中通知。 */
bool UHodgeCombatValidationLibrary::AddHitNotify(UAnimMontage* Montage, float Start, float End, const FHodgeAnimHitConfig& Config, bool bSingle)
{
	if (!Montage) { return false; }
	if (bSingle)
	{
		auto* Notify = NewObject<UHodgeAnimNotify_Hit>(Montage, NAME_None, RF_Transactional);
		Notify->Hit = Config;
		return AddAuthoringNotify(Montage, Notify, Start, Start);
	}
	auto* Notify = NewObject<UHodgeAnimNotifyState_HitCheck>(Montage, NAME_None, RF_Transactional);
	Notify->Hit = Config;
	return AddAuthoringNotify(Montage, Notify, Start, End);
}

/** 添加状态通知。 */
bool UHodgeCombatValidationLibrary::AddStateNotify(UAnimMontage* Montage, float Start, float End, FGameplayTag Tag)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Montage || !Tag.IsValid()) { return false; }
	auto* Notify = NewObject<UHodgeAnimNotifyState_GameplayTag>(Montage, NAME_None, RF_Transactional);
	Notify->StateTag = Tag;
	return AddAuthoringNotify(Montage, Notify, Start, End);
}

/** 添加武器手部通知。 */
bool UHodgeCombatValidationLibrary::AddWeaponHandNotify(UAnimMontage* Montage, float Start, float End)
{
	if (!Montage) { return false; }
	return AddAuthoringNotify(Montage, NewObject<UHodgeAnimNotifyState_WeaponHand>(Montage, NAME_None, RF_Transactional), Start, End);
}

/** 检查能力Windows。 */
FGameplayTagContainer UHodgeCombatValidationLibrary::InspectAbilityWindows(UHodgeGameplayAbility_Definition* Ability)
{
	return Ability ? Ability->GetExecutionWindows() : FGameplayTagContainer();
}
/** 检查命中Sessions。 */
int32 UHodgeCombatValidationLibrary::InspectHitSessions(AActor* Avatar)
{
	// 统一战斗组件。
	const auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(Avatar);
	return Combat ? Combat->GetDetectionSessionCount() : 0;
}
/** 检查姿势Leases。 */
int32 UHodgeCombatValidationLibrary::InspectPoseLeases(AActor* Avatar)
{
	// 统一战斗组件。
	const auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(Avatar);
	return Combat ? Combat->GetPoseLeaseCount() : 0;
}

/** 检查蒙太奇状态。 */
FHodgeMontageValidationState UHodgeCombatValidationLibrary::InspectMontageState(AActor* Avatar, UAnimMontage* Montage)
{
	FHodgeMontageValidationState Result;
	const auto* Character = Cast<ACharacter>(Avatar);
	// 当前网格使用的动画实例。
	UAnimInstance* Anim = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (!Anim) { return Result; }
	const int32 Machine = Anim->GetStateMachineIndex(TEXT("LocomotionSM"));
	if (Machine != INDEX_NONE) { Result.LocomotionState = Anim->GetCurrentStateName(Machine); }
	for (int32 Index = Anim->MontageInstances.Num() - 1; Index >= 0; --Index)
	{
		const FAnimMontageInstance* Instance = Anim->MontageInstances[Index];
		if (!Instance || Instance->Montage != Montage) { continue; }
		Result.InstanceId = Instance->GetInstanceID();
		Result.Position = Instance->GetPosition();
		Result.Weight = Instance->GetWeight();
		Result.DesiredWeight = Instance->GetDesiredWeight();
		Result.BlendTime = Instance->GetBlendTime();
		Result.bPlaying = Instance->IsPlaying();
		Result.bStopped = Instance->IsStopped();
		break;
	}
	return Result;
}

/** 全量返回同资源的每个实例，不用最新实例的退出状态替代旧实例权重。 */
TArray<FHodgeMontageValidationState> UHodgeCombatValidationLibrary::InspectMontageStates(AActor* Avatar, UAnimMontage* Montage)
{
	TArray<FHodgeMontageValidationState> Results;
	const auto* Character = Cast<ACharacter>(Avatar);
	UAnimInstance* Anim = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (!Anim || !Montage) { return Results; }
	const int32 Machine = Anim->GetStateMachineIndex(TEXT("LocomotionSM"));
	const FName Locomotion = Machine != INDEX_NONE ? Anim->GetCurrentStateName(Machine) : NAME_None;
	for (const FAnimMontageInstance* Instance : Anim->MontageInstances)
	{
		if (!Instance || Instance->Montage != Montage) { continue; }
		auto& Result = Results.AddDefaulted_GetRef();
		Result.InstanceId = Instance->GetInstanceID();
		Result.Position = Instance->GetPosition();
		Result.Weight = Instance->GetWeight();
		Result.DesiredWeight = Instance->GetDesiredWeight();
		Result.BlendTime = Instance->GetBlendTime();
		Result.bPlaying = Instance->IsPlaying();
		Result.bStopped = Instance->IsStopped();
		Result.LocomotionState = Locomotion;
	}
	return Results;
}

/** 配置校验PIE。 */
bool UHodgeCombatValidationLibrary::ConfigureValidationPIE(int32 Players, bool bDedicated)
{
	if (!GEditor || GEditor->PlayWorld || Players < 1 || Players > 2) { return false; }
	auto* Settings = GetMutableDefault<ULevelEditorPlaySettings>();
	Settings->SetPlayNumberOfClients(Players);
	Settings->SetRunUnderOneProcess(true);
	Settings->SetPlayNetMode(bDedicated ? PIE_Client : (Players > 1 ? PIE_ListenServer : PIE_Standalone));
	return true;
}

/** 配置校验片段集合。 */
bool UHodgeCombatValidationLibrary::ConfigureValidationSections(UAnimMontage* Montage)
{
	if (!Montage || !Montage->GetPathName().StartsWith(TEXT("/Game/CodexText/AnimNotifyCombat/")) || Montage->GetPlayLength() <= .5f) { return false; }
	Montage->Modify();
	Montage->CompositeSections.Reset();
	FCompositeSection First;
	First.SectionName = TEXT("Default");
	First.NextSectionName = TEXT("SecondHalf");
	First.Link(Montage, 0.f);
	FCompositeSection Second;
	Second.SectionName = TEXT("SecondHalf");
	Second.Link(Montage, .5f);
	Montage->CompositeSections.Add(First);
	Montage->CompositeSections.Add(Second);
	Montage->MarkPackageDirty();
	return true;
}

/** 按现有动画通知契约整理验证蒙太奇。 */
bool UHodgeCombatValidationLibrary::NormalizeCombatNotifies(UAnimMontage* Montage)
{
	if (!Montage) { return false; }
	Montage->Modify();
	for (FAnimNotifyEvent& Event : Montage->Notifies)
	{
		UAnimNotify* Point = Event.Notify.Get();
		UAnimNotifyState* State = Event.NotifyStateClass.Get();
		if (Point && (Point->IsA<UHodgeAnimNotify_Hit>() || Point->IsA<UHodgeAnimNotify_GameplayEvent>())) { Point->OnAnimNotifyCreatedInEditor(Event); }
		if (State && (State->IsA<UHodgeAnimNotifyState_HitCheck>() || State->IsA<UHodgeAnimNotifyState_GameplayTag>() || State->IsA<UHodgeAnimNotifyState_WeaponHand>())) { State->OnAnimNotifyCreatedInEditor(Event); }
	}
	Montage->RefreshCacheData();
	Montage->MarkPackageDirty();
	return true;
}

/** 配置命中受击校验蒙太奇。 */
bool UHodgeCombatValidationLibrary::ConfigureHitReactionValidationMontage(UAnimMontage* Montage, FName SlotName)
{
	if (!Montage || SlotName.IsNone() || !Montage->GetPathName().StartsWith(TEXT("/Game/CodexText/HitReactionValidation/"))) { return false; }
	Montage->Modify();
	Montage->Notifies.Reset();
	for (auto& Track : Montage->SlotAnimTracks) { Track.SlotName = SlotName; }
	Montage->RefreshCacheData();
	Montage->MarkPackageDirty();
	return true;
}

/** 在指定测试世界生成用于受击验证的Actor。 */
AActor* UHodgeCombatValidationLibrary::SpawnHitReactionValidationActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FVector Location)
{
	// 本次操作所在的世界。
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!World || World->WorldType != EWorldType::PIE || World->GetNetMode() == NM_Client || !ActorClass || Location.ContainsNaN()) { return nullptr; }
	const bool bFixtureEnemy = ActorClass->IsChildOf(AHodgeEnemyCharacter::StaticClass()) &&
		(ActorClass->GetPathName().StartsWith(TEXT("/Game/CodexText/HitReactionValidation/")) ||
		 ActorClass->GetPathName().StartsWith(TEXT("/Game/Main/Character/Enemies/Guardian/Validation/")));
	// 交互回归仅允许这个原生隔离目标，不扩展为任意运行时生成入口。
	const bool bInteractionFixture = ActorClass->GetPathName() == TEXT("/Script/Hodgepodge.HodgeInteractionTestTarget");
	if (!bFixtureEnemy && !bInteractionFixture && ActorClass != AStaticMeshActor::StaticClass()) { return nullptr; }
	FActorSpawnParameters Parameters;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Actor = World->SpawnActor<AActor>(ActorClass, Location, FRotator::ZeroRotator, Parameters);
	if (Actor) { Actor->SetReplicates(true); }
	return Actor;
}

/** 检查敌人决策。 */
FString UHodgeCombatValidationLibrary::InspectEnemyBrain(AActor* Avatar)
{
    // 当前操作关联的Pawn角色。
    const auto* Pawn = Cast<APawn>(Avatar);
    const auto* AI = Pawn ? Cast<AAIController>(Pawn->GetController()) : nullptr;
    const auto* BT = AI ? AI->FindComponentByClass<UBehaviorTreeComponent>() : nullptr;
    if (!BT) { return TEXT("No BehaviorTreeComponent"); }
    const auto* Tree = BT->GetCurrentTree();
    const auto* Node = BT->GetActiveNode();
    return FString::Printf(TEXT("Running=%d RootChildren=%d Active=%s %s"), BT->IsRunning(),
        Tree && Tree->RootNode ? Tree->RootNode->Children.Num() : -1,
        Node ? *Node->GetNodeName() : TEXT("None"), *BT->GetDebugInfoString());
}

/** 把共享战斗请求或连段意图排入后续游戏Tick。 */
bool UHodgeCombatValidationLibrary::QueueCombatAction(AActor* Avatar, FGameplayTag ActionTag, bool bComboIntent)
{
	// 按服务器权限区分本次处理分支。
	if (!IsValid(Avatar) || !Avatar->GetWorld() || Avatar->GetWorld()->WorldType != EWorldType::PIE ||
		(!Avatar->HasAuthority() && Avatar->GetLocalRole() != ROLE_AutonomousProxy)) { return false; }
	const TWeakObjectPtr<AActor> Weak = Avatar;
	Avatar->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(Avatar, [Weak, ActionTag, bComboIntent]()
	{
		if (auto* Combat = UHodgeCombatComponentBase::FindCombatComponent(Weak.Get()))
		{ if (bComboIntent) { Combat->SubmitCombatIntent(ActionTag); } else { Combat->RequestStandaloneAbility(ActionTag); } }
	}));
	return true;
}

/** 使用实际暂停／恢复方法，操作身份在排队和执行时都必须匹配。 */
bool UHodgeCombatValidationLibrary::QueueAttackFacingProbe(AActor* Avatar, FName Action)
{
 if (!Avatar || !Avatar->GetWorld() || Avatar->GetWorld()->WorldType!=EWorldType::PIE) { return false; }
 auto* ASC=Cast<UHodgeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Avatar));
 auto* Ability=ASC ? Cast<UHodgeGameplayAbility_Definition>(ASC->GetAnimatingAbility()) : nullptr;
 if (!Ability || !Ability->IsActive() || !Ability->GetDefinition()->FacingWarp.bEnabled) { return false; }
 const TWeakObjectPtr<UHodgeGameplayAbility_Definition> Weak=Ability; const FGuid Execution=Ability->GetExecutionId();
 Avatar->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([Weak,Execution,Action]()
 {
  auto* Current=Weak.Get(); if (!Current || !Current->IsActive() || Current->GetExecutionId()!=Execution) { return; }
  if (Action==TEXT("Suspend")) { Current->BeginPredictedMoveCancel(); return; }
  if (Action!=TEXT("RestoreFromAuthority")) { return; }
  const auto* Pawn=Cast<APawn>(Current->GetAvatarActorFromActorInfo());
  const auto* PlayerState=Pawn ? Pawn->GetPlayerState() : nullptr; if (!PlayerState || !GEngine) { return; }
  for (const auto& Context : GEngine->GetWorldContexts())
  {
   UWorld* World=Context.World(); if (!World || World->WorldType!=EWorldType::PIE || World->GetNetMode()==NM_Client) { continue; }
   for (TActorIterator<APawn> It(World); It; ++It)
   {
    const auto* State=It->GetPlayerState(); if (!State || State->GetPlayerId()!=PlayerState->GetPlayerId()) { continue; }
    auto* PeerASC=Cast<UHodgeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(*It));
    const auto* PeerAbility=PeerASC ? Cast<UHodgeGameplayAbility_Definition>(PeerASC->GetAnimatingAbility()) : nullptr;
    auto* Character=Cast<ACharacter>(*It); auto* Anim=Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
    const auto* Instance=Anim ? Anim->GetActiveInstanceForMontage(Current->GetDefinition()->ExecutionConfig.Montage) : nullptr;
    if (!PeerAbility || !PeerAbility->IsActive() || !Instance) { continue; }
    Current->ResolvePredictedMoveCancel(false,Instance->GetPosition(),World->GetGameState()->GetServerWorldTimeSeconds(),Instance->GetPlayRate()); return;
   }
  }
 })); return true;
}
