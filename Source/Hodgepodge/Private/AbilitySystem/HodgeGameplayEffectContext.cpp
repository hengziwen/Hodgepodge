// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/HodgeGameplayEffectContext.h"

#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayEffectContext)

class FArchive;

// 从 GameplayEffectContextHandle 中提取 Hodge 自定义的 GameplayEffectContext。
FHodgeGameplayEffectContext* FHodgeGameplayEffectContext::ExtractEffectContext(
	struct FGameplayEffectContextHandle Handle)
{
	// 从 Handle 中取得实际保存的 GameplayEffectContext。
	FGameplayEffectContext* BaseEffectContext = Handle.Get();

	// 确保 Context 存在，并且实际类型是 FHodgeGameplayEffectContext 或其子类型。
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(
		FHodgeGameplayEffectContext::StaticStruct()))
	{
		// 类型检查通过后，将基类 Context 转换为 Hodge 自定义 Context。
		return (FHodgeGameplayEffectContext*)BaseEffectContext;
	}

	// Context 不存在或类型不匹配时返回 nullptr。
	return nullptr;
}

// 自定义 GameplayEffectContext 的网络序列化。
bool FHodgeGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	// 复用 GAS 原生 GameplayEffectContext 的网络序列化逻辑。
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// 以下字段只用于 GameplayEffect 激活后的本地处理，因此不参与网络同步：
	// CartridgeID

	// 当前没有额外需要网络序列化的字段，因此直接返回成功。
	return true;
}

#if UE_WITH_IRIS
namespace UE::Net
{
	// 将 Hodge Context 的 Iris 网络序列化委托转发给 GAS 原生的 FGameplayEffectContextNetSerializer。
	// 如果以后修改 FHodgeGameplayEffectContext::NetSerialize()，当前转发实现将不足以覆盖新增字段，
	// 此时需要实现 Hodge 自己的自定义 NetSerializer。
	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(HodgeGameplayEffectContext,
	                                                                 FGameplayEffectContextNetSerializer);
}
#endif

// 设置当前 GameplayEffectContext 使用的 Ability Source。
void FHodgeGameplayEffectContext::SetAbilitySource(const IHodgeAbilitySourceInterface* InObject, float InSourceLevel)
{
	// 将 Ability Source 保存为弱 UObject 引用，避免 Context 持有对象的强引用。
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));

	// SourceLevel 暂时没有保存，保留原有设计。
	//SourceLevel = InSourceLevel;
}

// 获取当前 Context 关联的 Ability Source。
const IHodgeAbilitySourceInterface* FHodgeGameplayEffectContext::GetAbilitySource() const
{
	// 从弱 UObject 引用中取得对象，并转换回 Ability Source 接口。
	return Cast<IHodgeAbilitySourceInterface>(AbilitySourceObject.Get());
}

// 从当前命中结果中获取物理材质。
const UPhysicalMaterial* FHodgeGameplayEffectContext::GetPhysicalMaterial() const
{
	// 获取当前 Context 保存的 HitResult，并检查 HitResult 是否存在。
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		// 从 HitResult 中取得命中的物理材质。
		return HitResultPtr->PhysMaterial.Get();
	}

	// 没有 HitResult 时不存在物理材质。
	return nullptr;
}
