/**
 * @file HodgeHeroCharacter.cpp
 * @brief AHodgeHeroCharacter 类的实现
 *
 * 实现了英雄角色的 GAS 双初始化入口(PossessedBy + OnRep_PlayerState)。
 *
 * 原先由 ALS 提供的相机组件(UAlsCameraComponent)与全部输入处理已移除,
 * 相关逻辑后续改由组件化 / GAS Ability 的方式接入。
 */

#include "Character/HodgeHeroCharacter.h"

#include "Core/PlayState/HodgePlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHeroCharacter)

/**
 * @brief 构造函数
 *
 * 当前仅把 FObjectInitializer 透传给父类,默认组件由基类创建。
 */
AHodgeHeroCharacter::AHodgeHeroCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

/**
 * @brief 服务器端角色被 Possess 时的回调(重写自 APawn)
 *
 * 这是 GAS 服务器端初始化的入口:
 * 1. 调用 Super::PossessedBy() 执行父类逻辑
 * 2. 获取 AHodgePlayerState 的 PlayerState
 * 3. 调用 InitAbilityActorInfo 完成 GAS 初始化
 *
 * @param NewController 新的控制器指针
 */
void AHodgeHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AHodgePlayerState* PS = GetPlayerState<AHodgePlayerState>())
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		HodgeAbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}
}

/**
 * @brief 客户端 PlayerState 复制到位时的回调(重写自 AActor)
 *
 * 这是 GAS 客户端初始化的入口:
 * 1. 调用 Super::OnRep_PlayerState() 执行父类逻辑
 * 2. 获取 AHodgePlayerState 的 PlayerState
 * 3. 调用 InitAbilityActorInfo 完成 GAS 初始化
 *
 * 注意:此函数仅在客户端调用,因为 OnRep_ 是属性复制回调。
 */
void AHodgeHeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AHodgePlayerState* PS = GetPlayerState<AHodgePlayerState>())
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		HodgeAbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}
}
