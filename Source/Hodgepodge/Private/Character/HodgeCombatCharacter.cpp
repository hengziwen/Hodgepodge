/**
 * @file HodgeCombatCharacter.cpp
 * @brief AHodgeCombatCharacter 类的实现
 *
 * 战斗角色基类,当前仅包含头文件引用。
 * 作为 AHodgeCharacterBase 和 AHodgeHeroCharacter 之间的中间层,
 * 预留给战斗相关的通用逻辑(如战斗组件、攻击判定等)。
 */

#include "Character/HodgeCombatCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCombatCharacter)

/**
 * @brief 构造函数
 *
 * 将 FObjectInitializer 传递给父类 AHodgeCharacterBase。
 * 当前为空实现,预留扩展点,供子类配置战斗相关的默认组件或属性。
 */
AHodgeCombatCharacter::AHodgeCombatCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 禁用Tick以提高性能
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// 禁用网格接收贴花（提高渲染性能）
	GetMesh()->bReceivesDecals = false;
}

UAbilitySystemComponent* AHodgeCombatCharacter::GetAbilitySystemComponent() const
{
	return GetHodgeAbilitySystemComponent();
}

void AHodgeCombatCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 确保能力系统组件有效
	if (HodgeAbilitySystemComponent)
	{
		/**
		 * 初始化能力系统角色信息
		 * this指针传递两次：分别作为OwnerActor和AvatarActor
		 * 在服务器端初始化
		 */
		//HodgeAbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}
