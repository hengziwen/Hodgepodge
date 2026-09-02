// 111 屎山代码来袭

#include "Animation/HodgeAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "Character/HodgeCharacterBase.h"
#include "Component/HodgeCharacterMovementComponent.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAnimInstance)

// 构造动画实例
UHodgeAnimInstance::UHodgeAnimInstance(const FObjectInitializer& ObjectInitializer)
{
}

// 将动画实例与角色的 AbilitySystemComponent 绑定
void UHodgeAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	// ASC 必须有效，否则动画实例无法获取 GameplayTag 状态
	check(ASC);

	// 初始化 GameplayTag 到动画实例属性的自动映射
	GameplayTagPropertyMap.Initialize(this, ASC);
}

#if WITH_EDITOR
// 编辑器中检查 GameplayTagPropertyMap 等动画实例配置是否有效
EDataValidationResult UHodgeAnimInstance::IsDataValid(class FDataValidationContext& Context) const
{
	// 先执行父类的数据验证
	Super::IsDataValid(Context);

	// 验证 GameplayTagPropertyMap 的配置是否正确
	GameplayTagPropertyMap.IsDataValid(this, Context);

	// 只要验证过程中产生错误，就认为当前动画实例配置无效
	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

// 动画实例初始化时调用
void UHodgeAnimInstance::NativeInitializeAnimation()
{
	// 执行 UAnimInstance 默认初始化逻辑
	Super::NativeInitializeAnimation();

	// 获取当前动画实例所属的 Actor
	if (AActor* OwningActor = GetOwningActor())
	{
		// 从 Owner 上获取对应的 AbilitySystemComponent
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			// 将当前动画实例与 ASC 绑定，使 GameplayTag 状态能够自动同步到动画变量
			InitializeWithAbilitySystem(ASC);
		}
	}
}

// 每帧更新动画实例中的角色状态数据
void UHodgeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	// 执行 UAnimInstance 默认的动画更新逻辑
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 获取当前动画实例所属的 HodgeCharacter
	const AHodgeCharacterBase* Character = Cast<AHodgeCharacterBase>(GetOwningActor());
	if (!Character)
	{
		// Owner 不是 HodgeCharacter 时无法获取角色移动数据，直接结束更新
		return;
	}

	// 获取角色使用的 HodgeCharacterMovementComponent
	UHodgeCharacterMovementComponent* CharMoveComp = CastChecked<UHodgeCharacterMovementComponent>(
		Character->GetCharacterMovement());

	// 获取角色当前脚下的地面信息
	const FHodgeCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();

	// 将移动组件计算出的离地距离同步到动画实例，供动画蓝图使用
	GroundDistance = GroundInfo.GroundDistance;
}
