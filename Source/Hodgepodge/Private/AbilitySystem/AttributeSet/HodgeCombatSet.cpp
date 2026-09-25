// 111屎山代码来袭


// UHodgeCombatSet 类定义。
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"

// UE 网络属性复制相关宏与类型。
#include "Net/UnrealNetwork.h"

// 初始化战斗属性集的默认属性值。
UHodgeCombatSet::UHodgeCombatSet()
// 默认基础伤害为 0。
	: BaseDamage(0.0f)
	  // 默认基础治疗量为 0。
	  , BaseHeal(0.0f)
{
}

// 注册 CombatSet 中需要进行网络复制的属性。
void UHodgeCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 先注册父类中需要复制的属性。
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// BaseDamage 只复制给该 Actor 的拥有者，并且每次收到复制数据时都触发 OnRep_BaseDamage。
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);

	// BaseHeal 只复制给该 Actor 的拥有者，并且每次收到复制数据时都触发 OnRep_BaseHeal。
	DOREPLIFETIME_CONDITION_NOTIFY(UHodgeCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

// BaseDamage 从服务器复制到拥有者客户端后触发。
void UHodgeCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	// 通知 GAS BaseDamage 已完成网络同步，并让 GAS 正确处理预测值、聚合器以及属性变化通知。
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeCombatSet, BaseDamage, OldValue);
}

// BaseHeal 从服务器复制到拥有者客户端后触发。
void UHodgeCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	// 通知 GAS BaseHeal 已完成网络同步，并让 GAS 正确处理预测值、聚合器以及属性变化通知。
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHodgeCombatSet, BaseHeal, OldValue);
}
