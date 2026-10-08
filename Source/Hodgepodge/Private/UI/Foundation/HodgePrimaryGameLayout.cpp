#include "UI/Foundation/HodgePrimaryGameLayout.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"
#include "Input/CommonUIInputTypes.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgePrimaryGameLayout)

void UHodgePrimaryGameLayout::NativeOnInitialized()
{
    RegisterLayer(HodgeGameplayTags::UI_Layer_Game, GameLayer);
    RegisterLayer(HodgeGameplayTags::UI_Layer_GameMenu, GameMenuLayer);
    RegisterLayer(HodgeGameplayTags::UI_Layer_Menu, MenuLayer);
    RegisterLayer(HodgeGameplayTags::UI_Layer_Modal, ModalLayer);
    Super::NativeOnInitialized();
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UHodgePrimaryGameLayout::RegisterLayer(FGameplayTag Tag, UCommonActivatableWidgetContainerBase* Container)
{
    if (Tag.IsValid() && Container && (!Layers.Contains(Tag) || Layers[Tag] == Container)) { Layers.Add(Tag, Container); }
}

UCommonActivatableWidgetContainerBase* UHodgePrimaryGameLayout::GetLayer(FGameplayTag Tag) const
{
    const auto* Found = Layers.Find(Tag);
    return Found ? Found->Get() : nullptr;
}

UCommonActivatableWidget* UHodgePrimaryGameLayout::Push(FGameplayTag Tag, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
    auto* Layer = GetLayer(Tag);
    auto* Widget = !bReleased && Layer && WidgetClass ? Layer->AddWidget(WidgetClass) : nullptr;
    if (auto* GI = GetGameInstance())
    { if (auto* UI = GI->GetSubsystem<UHodgeUIManagerSubsystem>()) { UI->RefreshInputState(GetOwningLocalPlayer()); } }
    return Widget;
}

void UHodgePrimaryGameLayout::Pop(UCommonActivatableWidget* Widget)
{
    if (!Widget) { return; }
    for (const auto& Pair : Layers)
    {
        if (Pair.Value && Pair.Value->GetWidgetList().Contains(Widget)) { Pair.Value->RemoveWidget(*Widget); return; }
    }
}

bool UHodgePrimaryGameLayout::HasBlockingPage() const
{
    for (const auto& Pair : Layers)
    {
        if (const auto* Active = Pair.Value ? Pair.Value->GetActiveWidget() : nullptr)
        {
            const auto Config = Active->GetDesiredInputConfig();
            if (Active->IsActivated() && (Config.IsSet() ? Config.GetValue().GetInputMode() == ECommonInputMode::Menu
                : Pair.Key != HodgeGameplayTags::UI_Layer_Game)) { return true; }
        }
    }
    return false;
}

FGuid UHodgePrimaryGameLayout::PushAsync(FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass,
    TFunction<void(UCommonActivatableWidget*)> Completed, bool bSuspendInput)
{
    if (bReleased || WidgetClass.IsNull() || !GetLayer(Tag)) { Completed(nullptr); return {}; }
    if (WidgetClass.Get()) { Completed(Push(Tag, WidgetClass.Get())); return {}; }
    const FGuid Id = FGuid::NewGuid();
    FRequest& Request = Requests.Add(Id);
    Request.Controller = GetOwningPlayer();
    Request.Completed = MoveTemp(Completed);
    Request.InputToken = bSuspendInput ? FName(*Id.ToString()) : NAME_None;
    if (auto* GI = GetGameInstance())
    { if (auto* Manager = GI->GetSubsystem<UHodgeUIManagerSubsystem>()) { Manager->SuspendInput(GetOwningLocalPlayer(), Request.InputToken); } }
    auto Load = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(WidgetClass.ToSoftObjectPath(),
        FStreamableDelegate::CreateWeakLambda(this, [this, Id, Tag, WidgetClass]() { FinishPush(Id, Tag, WidgetClass); }));
    if (auto* Pending = Requests.Find(Id)) { Pending->Load = Load; }
    if (!Load.IsValid()) { CancelPush(Id); }
    return Requests.Contains(Id) ? Id : FGuid();
}

void UHodgePrimaryGameLayout::FinishPush(FGuid Id, FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass)
{
    FRequest Request;
    if (!Requests.RemoveAndCopyValue(Id, Request)) { return; }
    auto* Widget = !bReleased && Request.Controller.IsValid() && Request.Controller.Get() == GetOwningPlayer()
        ? Push(Tag, WidgetClass.Get()) : nullptr;
    if (auto* GI = GetGameInstance())
    { if (auto* Manager = GI->GetSubsystem<UHodgeUIManagerSubsystem>()) { Manager->ResumeInput(GetOwningLocalPlayer(), Request.InputToken); } }
    Request.Completed(Widget);
}

void UHodgePrimaryGameLayout::CancelPush(FGuid Id)
{
    FRequest Request;
    if (!Requests.RemoveAndCopyValue(Id, Request)) { return; }
    if (Request.Load) { Request.Load->CancelHandle(); }
    if (auto* GI = GetGameInstance())
    { if (auto* Manager = GI->GetSubsystem<UHodgeUIManagerSubsystem>()) { Manager->ResumeInput(GetOwningLocalPlayer(), Request.InputToken); } }
    Request.Completed(nullptr);
}

void UHodgePrimaryGameLayout::ReleaseLayout()
{
    if (bReleased) { return; }
    bReleased = true;
    TArray<FGuid> Ids;
    Requests.GetKeys(Ids);
    for (FGuid Id : Ids) { CancelPush(Id); }
    for (const auto& Pair : Layers)
    {
        TArray<UCommonActivatableWidget*> Widgets = Pair.Value->GetWidgetList();
        for (auto* Widget : Widgets) { Pair.Value->RemoveWidget(*Widget); }
    }
}

void UHodgePrimaryGameLayout::NativeDestruct()
{
    ReleaseLayout();
    Super::NativeDestruct();
}
