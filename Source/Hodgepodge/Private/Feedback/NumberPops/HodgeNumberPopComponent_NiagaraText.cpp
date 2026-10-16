// Copyright Epic Games, Inc. All Rights Reserved.
#include "Feedback/NumberPops/HodgeNumberPopComponent_NiagaraText.h"
#include "Feedback/NumberPops/HodgeDamagePopStyleNiagara.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Engine/World.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeNumberPopComponent_NiagaraText)

DEFINE_LOG_CATEGORY_STATIC(LogHodgeNumberPop, Log, All);

/** 软引用只有本地玩家才解析，CDO 与远端 Controller 不加载 Niagara。 */
UHodgeNumberPopComponent_NiagaraText::UHodgeNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Style = TSoftObjectPtr<UHodgeDamagePopStyleNiagara>(FSoftObjectPath(TEXT("/Game/Main/UI/Feedback/DA_DamageNumberStyle.DA_DamageNumberStyle")));
}

/** 在进入游戏时预载本地样式，避免第一刀再创建整条资源依赖。 */
void UHodgeNumberPopComponent_NiagaraText::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld()->IsGameWorld()) { LoadLocalStyle(); }
}

/** 样式缺失只影响表现，不改变伤害事实。 */
bool UHodgeNumberPopComponent_NiagaraText::LoadLocalStyle()
{
	const auto* PC = GetController<APlayerController>();
	if (!PC || !PC->IsLocalController() || GetNetMode() == NM_DedicatedServer) { return false; }
	if (!bStyleLoadAttempted)
	{
		bStyleLoadAttempted = true;
		LoadedStyle = Style.LoadSynchronous();
		const auto* Loaded = LoadedStyle.Get();
		if (!Loaded || !Loaded->TextNiagara || Loaded->NiagaraArrayName.IsNone() || Loaded->MaxBufferedRequests < 1)
		{ UE_LOG(LogHodgeNumberPop, Error, TEXT("Invalid local number-pop style: %s"), *Style.ToString()); }
	}
	return LoadedStyle && LoadedStyle->TextNiagara && !LoadedStyle->NiagaraArrayName.IsNone() && LoadedStyle->MaxBufferedRequests > 0;
}

/** 沿用 Lyra 的 XYZ 位置加 W 数值协议；负号仅表示本地暴击样式预览。 */
void UHodgeNumberPopComponent_NiagaraText::AddNumberPop(const FHodgeNumberPopRequest& Request)
{
	const auto* PC = GetController<APlayerController>();
	if (!PC || !PC->IsLocalController() || GetNetMode() == NM_DedicatedServer || !Request.IsValid() ||
		(Request.EventId != 0 && ReceivedIds.Contains(Request.EventId))) { return; }
	if (Request.EventId != 0)
	{
		ReceivedIds.Add(Request.EventId);
		if (ReceivedIds.Num() > 128) { ReceivedIds.RemoveAt(0, ReceivedIds.Num() - 128, EAllowShrinking::No); }
	}
	++ReceivedCount;
	LastRequest = Request;
	if (!GetWorld()->IsGameWorld() || !LoadLocalStyle()) { return; }
	if (!NiagaraComp)
	{
		NiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
		NiagaraComp->SetAutoActivate(false);
		NiagaraComp->SetAsset(LoadedStyle->TextNiagara);
		NiagaraComp->SetUsingAbsoluteLocation(true);
		NiagaraComp->RegisterComponent();
	}
	NiagaraComp->Activate(false);
	NiagaraComp->SetWorldLocation(Request.WorldLocation);
	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(NiagaraComp, LoadedStyle->NiagaraArrayName);
	const int32 Limit = FMath::Clamp(LoadedStyle->MaxBufferedRequests, 1, 512);
	if (DamageList.Num() >= Limit) { DamageList.RemoveAt(0, DamageList.Num() - Limit + 1, EAllowShrinking::No); }
	const float Number = float(Request.GetNumberToDisplay()) * (Request.bIsCriticalDamage ? -1.f : 1.f);
	DamageList.Add(FVector4(Request.WorldLocation.X, Request.WorldLocation.Y, Request.WorldLocation.Z, Number));
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, LoadedStyle->NiagaraArrayName, DamageList);
	++RenderedCount;
	UE_LOG(LogHodgeNumberPop, Verbose, TEXT("Local=%s Id=%d Accepted=%.3f Display=%d Location=%s"),
		*GetNameSafe(GetOwner()), Request.EventId, Request.DamageAmount, Request.GetNumberToDisplay(), *Request.WorldLocation.ToString());
}

/** Controller 离开世界才销毁共享系统，Pawn 死亡或更换不会中断尾部数字。 */
void UHodgeNumberPopComponent_NiagaraText::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (NiagaraComp) { NiagaraComp->DestroyComponent(); NiagaraComp = nullptr; }
	ReceivedIds.Reset();
	LoadedStyle = nullptr;
	LastRequest = FHodgeNumberPopRequest();
	Super::EndPlay(EndPlayReason);
}
