#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

float UHodgeAbilitySystemComponent::PlayMontage(UGameplayAbility* Ability, FGameplayAbilityActivationInfo Info,
	UAnimMontage* Montage, float Rate, FName Section, float StartTime)
{
	const UHodgeAbilityDefinition* Definition = Ability ? FindAbilityDefinition(Ability->GetCurrentAbilitySpecHandle()) : nullptr;
	const float Result = Super::PlayMontage(Ability, Info, Montage, Rate, Section, StartTime);
	if (Result > 0.f)
	{
		DefinitionMontage.Definition = Definition;
		DefinitionMontage.PlayInstanceID = GetRepAnimMontageInfo().PlayInstanceId;
		DefinitionMontage.bNaturalStop = false;
		ApplyDefinitionMontageSettings();
	}
	return Result;
}

float UHodgeAbilitySystemComponent::PlayMontageSimulated(UAnimMontage* Montage, float Rate, FName Section)
{
	const float Result = Super::PlayMontageSimulated(Montage, Rate, Section);
	ApplyDefinitionMontageSettings();
	return Result;
}

void UHodgeAbilitySystemComponent::ApplyDefinitionMontageSettings()
{
	if (!DefinitionMontage.Definition || !AbilityActorInfo.IsValid()) { return; }
	if (!AbilityActorInfo->IsLocallyControlled() && !IsOwnerActorAuthoritative()
		&& DefinitionMontage.PlayInstanceID != GetRepAnimMontageInfo().PlayInstanceId) { return; }
	const auto& Config = DefinitionMontage.Definition->ExecutionConfig;
	UAnimInstance* Anim = AbilityActorInfo->GetAnimInstance();
	FAnimMontageInstance* Instance = Anim ? Anim->GetActiveInstanceForMontage(Config.Montage) : nullptr;
	if (!Instance || Instance->GetInstanceID() == ConfiguredMontageInstance) { return; }
	ConfiguredMontageInstance = Instance->GetInstanceID();
	Instance->Play(Instance->GetPlayRate(), FMontageBlendSettings(FAlphaBlendArgs(Config.BlendIn.MakeBlend())));
	Instance->bEnableAutoBlendOut = false;
}

void UHodgeAbilitySystemComponent::OnRep_DefinitionMontage()
{
	ApplyDefinitionMontageSettings();
	if (DefinitionMontage.PlayInstanceID == GetRepAnimMontageInfo().PlayInstanceId && GetRepAnimMontageInfo().IsStopped)
	{
		CurrentMontageStop();
	}
}

void UHodgeAbilitySystemComponent::OnRep_ReplicatedAnimMontage()
{
	Super::OnRep_ReplicatedAnimMontage();
	ApplyDefinitionMontageSettings();
}

void UHodgeAbilitySystemComponent::StopDefinitionMontage(UGameplayAbility* Ability, bool bNatural)
{
	if (GetAnimatingAbility() != Ability) { return; }
	DefinitionMontage.bNaturalStop = bNatural;
	CurrentMontageStop();
}

void UHodgeAbilitySystemComponent::CurrentMontageStop(float OverrideBlendOutTime)
{
	const auto* Definition = DefinitionMontage.Definition.Get();
	if (Definition && AbilityActorInfo.IsValid() && GetCurrentMontage() == Definition->ExecutionConfig.Montage
		&& (IsOwnerActorAuthoritative() || AbilityActorInfo->IsLocallyControlled()
			|| DefinitionMontage.PlayInstanceID == GetRepAnimMontageInfo().PlayInstanceId))
	{
		UAnimInstance* Anim = AbilityActorInfo->GetAnimInstance();
		FAnimMontageInstance* Instance = Anim ? Anim->GetActiveInstanceForMontage(GetCurrentMontage()) : nullptr;
		if (Instance)
		{
			const auto& Config = Definition->ExecutionConfig;
			Instance->Stop((DefinitionMontage.bNaturalStop ? Config.NaturalBlendOut : Config.StopBlendOut).MakeBlend(),
				!DefinitionMontage.bNaturalStop);
			if (ShouldRecordMontageReplication()) { AnimMontage_UpdateReplicatedData(); }
		}
		return;
	}
	Super::CurrentMontageStop(OverrideBlendOutTime);
}
