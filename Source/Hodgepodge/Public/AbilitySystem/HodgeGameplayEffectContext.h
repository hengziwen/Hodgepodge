/** @file HodgeGameplayEffectContext.h
 * @brief 声明GameplayEffect上下文相关的类型、接口与现有处理流程。
 */
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
	/** 构造GameplayEffect上下文并初始化其默认状态。 */
	FHodgeGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	// 使用 Instigator 和 EffectCauser 初始化 GameplayEffect Context。
	/** 构造GameplayEffect上下文并初始化其默认状态。
	 * @param InInstigator 发起本次行为的对象。
	 * @param InEffectCauser 效果Causer，由调用方传入。
	 */
	FHodgeGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	// 从 GameplayEffectContextHandle 中提取 Hodge 自定义 Context；类型不匹配或不存在时返回 nullptr。
	/** 从基础效果上下文中读取本项目的扩展上下文。
	 * @param Handle 对应操作或能力的句柄。
	 * @return 本次计算、查询或创建得到的GameplayEffect上下文。
	 */
	static HODGEPODGE_API FHodgeGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	// 设置当前 GameplayEffect 使用的 Ability Source 以及对应的 Source Level。
	/** 设置能力来源。
	 * @param InObject 对象，由调用方传入。
	 * @param InSourceLevel 来源等级，由调用方传入。
	 */
	void SetAbilitySource(const IHodgeAbilitySourceInterface* InObject, float InSourceLevel);

	// 获取当前 GameplayEffect 关联的 Ability Source；当前设计下仅保证 Authority 端有效。
	/** 获取能力来源。
	 * @return 能力来源的当前值或引用。
	 */
	const IHodgeAbilitySourceInterface* GetAbilitySource() const;

	// 复制 GameplayEffectContext，并对 HitResult 执行深拷贝。
	/** 为测试或编辑器制作复制指定对象。
	 * @return 本次计算、查询或创建得到的玩法效果上下文。
	 */
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
	/** 获取ScriptStruct。
	 * @return ScriptStruct的当前值或引用。
	 */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FHodgeGameplayEffectContext::StaticStruct();
	}

	// 重写网络序列化，用于同步自定义 Context 中新增的字段。
	/** 按既有协议读写网络载荷及对象引用。
	 * @param Ar 读取或写入数据的归档对象。
	 * @param Map 序列化使用的对象映射或当前映射表。
	 * @param bOutSuccess 输出序列化或处理是否成功。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	// 从 HitResult 中获取命中的物理材质。
	/** 获取Physical材质。
	 * @return Physical材质的当前值或引用。
	 */
	const UPhysicalMaterial* GetPhysicalMaterial() const;

public:
	// 用于标识同一发弹药中的多个子弹，默认 -1 表示未设置。
	UPROPERTY()
	int32 CartridgeID = -1;
	// 仅用于服务器同步结算匹配，不参与网络序列化。
	UPROPERTY() FGuid HitSettlementId;

protected:
	// Ability Source 对象的弱引用；该对象需要实现 IHodgeAbilitySourceInterface。
	// 当前该字段本身不会进行网络复制。
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};

// 为自定义 GameplayEffectContext 开启 GAS 所需的结构体操作能力。
template <>
/** 保存TStructOps类型TraitsFHodge玩法效果上下文的数据和接口契约。 */
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
