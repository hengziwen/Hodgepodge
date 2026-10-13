/** @file HodgePrimaryGameLayout.cpp
 * @brief 实现本地玩家UI层栈相关的类型、接口与现有处理流程。
 */
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

/** 在控件实例初始化时建立必要的输入或页面绑定。 */
void UHodgePrimaryGameLayout::NativeOnInitialized()
{
    RegisterLayer(HodgeGameplayTags::UI_Layer_Game, GameLayer);
    RegisterLayer(HodgeGameplayTags::UI_Layer_GameMenu, GameMenuLayer);
    RegisterLayer(HodgeGameplayTags::UI_Layer_Menu, MenuLayer);
    RegisterLayer(HodgeGameplayTags::UI_Layer_Modal, ModalLayer);
    Super::NativeOnInitialized();
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

/** 注册层。 */
void UHodgePrimaryGameLayout::RegisterLayer(FGameplayTag Tag, UCommonActivatableWidgetContainerBase* Container)
{
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (Tag.IsValid() && Container && (!Layers.Contains(Tag) || Layers[Tag] == Container)) { Layers.Add(Tag, Container); }
}

/** 获取层。 */
UCommonActivatableWidgetContainerBase* UHodgePrimaryGameLayout::GetLayer(FGameplayTag Tag) const
{
    const auto* Found = Layers.Find(Tag);
    return Found ? Found->Get() : nullptr;
}

/** 将指定页面加入当前根布局的目标层。 */
UCommonActivatableWidget* UHodgePrimaryGameLayout::Push(FGameplayTag Tag, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
    auto* Layer = GetLayer(Tag);
    auto* Widget = !bReleased && Layer && WidgetClass ? Layer->AddWidget(WidgetClass) : nullptr;
    if (auto* GI = GetGameInstance())
    { if (auto* UI = GI->GetSubsystem<UHodgeUIManagerSubsystem>()) { UI->RefreshInputState(GetOwningLocalPlayer()); } }
    return Widget;
}

/** 从所属根布局移除指定页面。 */
void UHodgePrimaryGameLayout::Pop(UCommonActivatableWidget* Widget)
{
    if (!Widget) { return; }
    for (const auto& Pair : Layers)
    {
        if (Pair.Value && Pair.Value->GetWidgetList().Contains(Widget)) { Pair.Value->RemoveWidget(*Widget); return; }
    }
}

/** 判断是否存在BlockingPage。 */
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

/** 异步加载页面并加入指定UI层，按请求身份处理取消。 */
FGuid UHodgePrimaryGameLayout::PushAsync(FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass,
    TFunction<void(UCommonActivatableWidget*)> Completed, bool bSuspendInput)
{
    // 先检查对象与上下文，避免在依赖失效后继续处理。
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

/** 结束入栈。 */
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

/** 取消对应的异步页面加载请求并释放挂起状态。 */
void UHodgePrimaryGameLayout::CancelPush(FGuid Id)
{
    FRequest Request;
    if (!Requests.RemoveAndCopyValue(Id, Request)) { return; }
    if (Request.Load) { Request.Load->CancelHandle(); }
    if (auto* GI = GetGameInstance())
    { if (auto* Manager = GI->GetSubsystem<UHodgeUIManagerSubsystem>()) { Manager->ResumeInput(GetOwningLocalPlayer(), Request.InputToken); } }
    Request.Completed(nullptr);
}

/** 关闭当前根布局并清理层栈与未完成的加载请求。 */
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

/** 在控件析构时解除界面监听与临时资源。 */
void UHodgePrimaryGameLayout::NativeDestruct()
{
    ReleaseLayout();
    Super::NativeDestruct();
}
