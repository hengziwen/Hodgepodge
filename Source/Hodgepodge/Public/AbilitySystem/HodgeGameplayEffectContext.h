// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectTypes.h"
#include "Interface/HodgeAbilitySourceInterface.h"

#include "HodgeGameplayEffectContext.generated.h"

class AActor;
class FArchive;

class UObject;
class UPhysicalMaterial;

/**
 * GameplayEffect 的自定义上下文。
 *
 * 用于在 GameplayEffect 触发和传递过程中保存额外信息，例如：
 * - Ability Source：当前效果来自哪个能力来源对象
 * - CartridgeID：用于区分同一发弹药中的多个子弹
 * - HitResult：命中信息以及对应的物理材质
 *
 * 继承自 GAS 原生的 FGameplayEffectContext，可以通过 GameplayEffectContextHandle 在 GAS 中传递。
 */
USTRUCT()
struct FHodgeGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	// 使用父类默认构造函数初始化 GameplayEffect Context。
	FHodgeGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	// 使用 Instigator 和 EffectCauser 初始化 GameplayEffect Context。
	FHodgeGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	// 从 GameplayEffectContextHandle 中提取 Hodge 自定义 Context；类型不匹配或不存在时返回 nullptr。
	static HODGEPODGE_API FHodgeGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	// 设置当前 GameplayEffect 使用的 Ability Source 以及对应的 Source Level。
	void SetAbilitySource(const IHodgeAbilitySourceInterface* InObject, float InSourceLevel);

	// 获取当前 GameplayEffect 关联的 Ability Source；当前设计下仅保证 Authority 端有效。
	const IHodgeAbilitySourceInterface* GetAbilitySource() const;

	// 复制 GameplayEffectContext，并对 HitResult 执行深拷贝。
	virtual FGameplayEffectContext* Duplicate() const override
	{
		FHodgeGameplayEffectContext* NewContext = new FHodgeGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// 深拷贝 HitResult，避免新 Context 与原 Context 共享同一份命中数据。
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	// 返回当前自定义 Context 对应的 UScriptStruct 类型信息。
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FHodgeGameplayEffectContext::StaticStruct();
	}

	// 重写网络序列化，用于同步自定义 Context 中新增的字段。
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	// 从 HitResult 中获取命中的物理材质。
	const UPhysicalMaterial* GetPhysicalMaterial() const;

public:
	// 用于标识同一发弹药中的多个子弹，默认 -1 表示未设置。
	UPROPERTY()
	int32 CartridgeID = -1;

protected:
	// Ability Source 对象的弱引用；该对象需要实现 IHodgeAbilitySourceInterface。
	// 当前该字段本身不会进行网络复制。
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};

// 为自定义 GameplayEffectContext 开启 GAS 所需的结构体操作能力。
template <>
struct TStructOpsTypeTraits<FHodgeGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FHodgeGameplayEffectContext>
{
	enum
	{
		// 告诉 UE 该结构体拥有自定义 NetSerialize，实现网络序列化。
		WithNetSerializer = true,

		// 告诉 UE 该结构体支持自定义复制操作。
		WithCopy = true
	};
};
