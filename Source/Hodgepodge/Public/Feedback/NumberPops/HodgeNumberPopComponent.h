// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Components/ControllerComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/NetSerialization.h"
#include "HodgeNumberPopComponent.generated.h"

/** 从 Lyra NumberPop 请求适配；伤害保留浮点原值，不依赖目标仍然存活。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeNumberPopRequest
{
	GENERATED_BODY()
	// 服务器按接收 Controller 编号；零仅供本地样式预览。
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 EventId = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FVector_NetQuantize10 WorldLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float DamageAmount = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FGameplayTagContainer SourceTags;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FGameplayTagContainer TargetTags;
	// 当前结算没有暴击结果，正式路径始终为 false；保留 Lyra 的预览接口。
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool bIsCriticalDamage = false;
	/** 拒绝非法显示请求，不改变任何战斗属性。 */
	bool IsValid() const;
	/** 适配 Lyra 整数字形，正的小额伤害至少显示 1，原始数值仍保留。 */
	int32 GetNumberToDisplay() const;
};

/** 沿用 Lyra 的 Controller 侧表现门面，不承担伤害计算或奖励。 */
UCLASS(Abstract)
class HODGEPODGE_API UHodgeNumberPopComponent : public UControllerComponent
{
	GENERATED_BODY()
public:
	UHodgeNumberPopComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/** 由拥有者客户端或本地预览调用，具体后端处理资源复用。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|NumberPop")
	virtual void AddNumberPop(const FHodgeNumberPopRequest& Request) {}
};
