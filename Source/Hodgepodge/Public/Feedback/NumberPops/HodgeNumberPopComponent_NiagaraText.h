// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Feedback/NumberPops/HodgeNumberPopComponent.h"
#include "HodgeNumberPopComponent_NiagaraText.generated.h"
class UHodgeDamagePopStyleNiagara;
class UNiagaraComponent;

/** 从 Lyra NiagaraText 迁入，一位本地 Controller 复用一个系统和事件数组。 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeNumberPopComponent_NiagaraText : public UHodgeNumberPopComponent
{
	GENERATED_BODY()
public:
	UHodgeNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void AddNumberPop(const FHodgeNumberPopRequest& Request) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// 诊断区分收到请求与真正送入 Niagara，不能把缺资产伪装成渲染成功。
	UFUNCTION(BlueprintPure, Category="Hodge|NumberPop") int32 GetReceivedCount() const { return ReceivedCount; }
	UFUNCTION(BlueprintPure, Category="Hodge|NumberPop") int32 GetRenderedCount() const { return RenderedCount; }
	UFUNCTION(BlueprintPure, Category="Hodge|NumberPop") FHodgeNumberPopRequest GetLastRequest() const { return LastRequest; }
	UFUNCTION(BlueprintPure, Category="Hodge|NumberPop") UNiagaraComponent* GetNumberPopNiagara() const { return NiagaraComp; }
	UPROPERTY(EditDefaultsOnly, Category="Number Pop|Style") TSoftObjectPtr<UHodgeDamagePopStyleNiagara> Style;
private:
	/** 只为本地玩家加载一次样式；配置失败明确诊断，不逐命中无限重试。 */
	bool LoadLocalStyle();
	// 软引用负责定位，强引用保证 GC 后同一场景仍能显示。
	UPROPERTY(Transient) TObjectPtr<UHodgeDamagePopStyleNiagara> LoadedStyle;
	UPROPERTY(Transient) TObjectPtr<UNiagaraComponent> NiagaraComp;
	UPROPERTY(Transient) FHodgeNumberPopRequest LastRequest;
	TArray<int32> ReceivedIds;
	int32 ReceivedCount = 0;
	int32 RenderedCount = 0;
	bool bStyleLoadAttempted = false;
};
