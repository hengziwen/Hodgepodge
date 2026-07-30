// 111屎山代码来袭


#include "Core/PlayState/HodgePlayerStateBase.h"
#include "AbilitySystem/HodgeAbilitySystemComponentBase.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

AHodgePlayerStateBase::AHodgePlayerStateBase()
{
    AbilitySystemComponent = CreateDefaultSubobject<UHodgeAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
}

UAbilitySystemComponent* AHodgePlayerStateBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AHodgePlayerStateBase::InitializeAbilitySystemForCharacter(ACharacter* Avatar)
{
    if (!AbilitySystemComponent || !Avatar)
    {
        return;
    }

    if (!AbilitySystemComponent->GetOwner() || AbilitySystemComponent->GetAvatarActor() != Avatar)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, Avatar);
    }

    // Ensure attribute sets are added on server
    if (HasAuthority())
    {
        // Only add once
        if (AddedAttributeSets.Num() == 0)
        {
            for (TSubclassOf<UAttributeSet> AttrClass : AttributeSetClasses)
            {
                if (!AttrClass) continue;
                UAttributeSet* NewAS = NewObject<UAttributeSet>(AbilitySystemComponent, AttrClass);
                if (NewAS)
                {
                    AbilitySystemComponent->AddAttributeSetSubobject(NewAS);
                    AddedAttributeSets.Add(NewAS);
                }
            }
        }

        // TODO: Give default abilities/effects (ApplyGameplayEffectToSelf etc.)
    }
}

