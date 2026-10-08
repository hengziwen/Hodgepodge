#include "UI/HodgeGameplayWidgets.h"
#include "UI/Foundation/HodgePrimaryGameLayout.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include "UI/Extension/UIExtensionPointWidget.h"
#include "Core/LocalPlayer/HodgeLocalPlayerBase.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "Component/HodgeHealthComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Component/HodgeCombatComponentBase.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Data/HodgeComboDefinition.h"
#include "Abilities/GameplayAbility.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "NativeGameplayTags.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeGameplayWidgets)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_SLOT_VITALS, "UI.Slot.PlayerVitals");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_SLOT_ABILITIES, "UI.Slot.AbilityBar");

namespace
{
    UTextBlock* Text(UWidgetTree* Tree, const FString& Value, int32 Size = 18)
    {
        auto* Label = Tree->ConstructWidget<UTextBlock>();
        Label->SetText(FText::FromString(Value));
        auto Font = Label->GetFont(); Font.Size = Size; Label->SetFont(Font);
        return Label;
    }
    UVerticalBox* Panel(UWidgetTree* Tree, float Width)
    {
        auto* Overlay = Tree->ConstructWidget<UOverlay>();
        auto* Box = Tree->ConstructWidget<USizeBox>(); Box->SetWidthOverride(Width);
        auto* Slot = Overlay->AddChildToOverlay(Box);
        Slot->SetHorizontalAlignment(HAlign_Center); Slot->SetVerticalAlignment(VAlign_Center);
        auto* Border = Tree->ConstructWidget<UBorder>(); Border->SetPadding(FMargin(24));
        Border->SetBrushColor(FLinearColor(.025f,.035f,.05f,.96f)); Box->AddChild(Border);
        auto* Column = Tree->ConstructWidget<UVerticalBox>(); Border->AddChild(Column);
        Tree->RootWidget = Overlay;
        return Column;
    }
    UButton* Button(UWidgetTree* Tree, UVerticalBox* Column, const FString& Label)
    {
        auto* B = Tree->ConstructWidget<UButton>();
        B->AddChild(Text(Tree, Label)); Column->AddChild(B);
        return B;
    }
    UHodgePrimaryGameLayout* Root(const UUserWidget* Widget)
    {
        auto* GI = Widget->GetGameInstance();
        auto* Manager = GI ? GI->GetSubsystem<UHodgeUIManagerSubsystem>() : nullptr;
        return Manager ? Manager->GetRootLayout(Widget->GetOwningLocalPlayer()) : nullptr;
    }
}

UHodgeGameplayHUDWidget::UHodgeGameplayHUDWidget(const FObjectInitializer& Initializer) : Super(Initializer)
{
    InputConfig = EHodgeWidgetInputMode::Game;
    EscapeMenuClass = TSoftClassPtr<UCommonActivatableWidget>(FSoftObjectPath(
        TEXT("/Game/Main/UI/Menu/WBP_HodgePauseMenu.WBP_HodgePauseMenu_C")));
}

void UHodgeGameplayHUDWidget::NativeOnInitialized()
{
    if (!WidgetTree->RootWidget)
    {
        auto* Overlay = WidgetTree->ConstructWidget<UOverlay>(); WidgetTree->RootWidget = Overlay;
        auto* Vitals = WidgetTree->ConstructWidget<UUIExtensionPointWidget>(); Vitals->SetExtensionPointTag(TAG_UI_SLOT_VITALS);
        auto* VitalsSlot = Overlay->AddChildToOverlay(Vitals);
        VitalsSlot->SetHorizontalAlignment(HAlign_Left); VitalsSlot->SetVerticalAlignment(VAlign_Bottom);
        VitalsSlot->SetPadding(FMargin(32,0,0,32));
        auto* Abilities = WidgetTree->ConstructWidget<UUIExtensionPointWidget>(); Abilities->SetExtensionPointTag(TAG_UI_SLOT_ABILITIES);
        auto* AbilitySlot = Overlay->AddChildToOverlay(Abilities);
        AbilitySlot->SetHorizontalAlignment(HAlign_Right); AbilitySlot->SetVerticalAlignment(VAlign_Bottom);
        AbilitySlot->SetPadding(FMargin(0,0,32,32));
        auto* Hint = Text(WidgetTree, TEXT("Esc  Menu"), 16);
        auto* HintSlot = Overlay->AddChildToOverlay(Hint);
        HintSlot->SetHorizontalAlignment(HAlign_Right); HintSlot->SetVerticalAlignment(VAlign_Top);
        HintSlot->SetPadding(FMargin(20));
    }
    Super::NativeOnInitialized();
}

void UHodgePlayerVitalsWidget::NativeOnInitialized()
{
    if (!WidgetTree->RootWidget)
    {
        auto* Box = WidgetTree->ConstructWidget<USizeBox>(); Box->SetWidthOverride(280);
        auto* Border = WidgetTree->ConstructWidget<UBorder>(); Border->SetPadding(FMargin(16));
        Border->SetBrushColor(FLinearColor(.025f,.035f,.05f,.85f)); Box->AddChild(Border);
        auto* Column = WidgetTree->ConstructWidget<UVerticalBox>(); Border->AddChild(Column);
        HealthValue = Text(WidgetTree, TEXT("HP  --"), 20); Column->AddChild(HealthValue);
        HealthBar = WidgetTree->ConstructWidget<UProgressBar>(); HealthBar->SetFillColorAndOpacity(FLinearColor(.15f,.7f,.45f));
        Column->AddChild(HealthBar); WidgetTree->RootWidget = Box;
    }
    Super::NativeOnInitialized();
}

void UHodgePlayerVitalsWidget::NativeConstruct()
{
    Super::NativeConstruct();
    LocalPlayer = GetOwningLocalPlayer<UHodgeLocalPlayerBase>();
    if (LocalPlayer.IsValid())
    {
        PawnDelegate = LocalPlayer->CallAndRegister_OnPlayerPawnSet(
            UHodgeLocalPlayerBase::FPlayerPawnSetDelegate::FDelegate::CreateUObject(this, &ThisClass::PawnChanged));
    }
}

void UHodgePlayerVitalsWidget::UnbindHealth()
{
    if (Health.IsValid())
    {
        Health->OnHealthChanged.RemoveAll(this); Health->OnMaxHealthChanged.RemoveAll(this);
        Health->OnDeathStarted.RemoveAll(this); Health->OnDeathFinished.RemoveAll(this);
    }
    if (Extension.IsValid()) { Extension->UnregisterAbilitySystemDelegates(this); }
    Extension.Reset(); Health.Reset();
}

void UHodgePlayerVitalsWidget::PawnChanged(UHodgeLocalPlayerBase* Player, APawn* Pawn)
{
    UnbindHealth();
    Health = Pawn ? UHodgeHealthComponent::FindHealthComponent(Pawn) : nullptr;
    if (Health.IsValid())
    {
        Health->OnHealthChanged.AddUniqueDynamic(this, &ThisClass::AttributeChanged);
        Health->OnMaxHealthChanged.AddUniqueDynamic(this, &ThisClass::AttributeChanged);
        Health->OnDeathStarted.AddUniqueDynamic(this, &ThisClass::DeathChanged);
        Health->OnDeathFinished.AddUniqueDynamic(this, &ThisClass::DeathChanged);
    }
    Extension = Pawn ? UHodgePawnExtensionComponent::FindPawnExtensionComponent(Pawn) : nullptr;
    if (Extension.IsValid())
    {
        Extension->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::BindHealth));
        Extension->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::Refresh));
    }
    Refresh();
}

void UHodgePlayerVitalsWidget::BindHealth() { Refresh(); }
void UHodgePlayerVitalsWidget::AttributeChanged(UHodgeHealthComponent* Component, float Old, float New, AActor* Instigator) { Refresh(); }
void UHodgePlayerVitalsWidget::DeathChanged(AActor* Actor) { Refresh(); }

void UHodgePlayerVitalsWidget::Refresh()
{
    bVitalsReady = Health.IsValid() && Health->GetMaxHealth() > 0;
    DisplayedHealth = bVitalsReady ? Health->GetHealth() : 0;
    DisplayedMaxHealth = bVitalsReady ? Health->GetMaxHealth() : 0;
    if (HealthValue) { HealthValue->SetText(FText::FromString(bVitalsReady
        ? FString::Printf(TEXT("HP  %.0f / %.0f"), DisplayedHealth, DisplayedMaxHealth) : TEXT("HP  --"))); }
    if (HealthBar) { HealthBar->SetPercent(bVitalsReady ? DisplayedHealth / DisplayedMaxHealth : 0); }
    OnVitalsUpdated();
}

void UHodgePlayerVitalsWidget::NativeDestruct()
{
    if (LocalPlayer.IsValid()) { LocalPlayer->OnPlayerPawnSet.Remove(PawnDelegate); }
    PawnDelegate.Reset(); LocalPlayer.Reset(); UnbindHealth();
    Super::NativeDestruct();
}

UHodgeAbilityBarWidget::UHodgeAbilityBarWidget(const FObjectInitializer& Initializer) : Super(Initializer)
{
    Slots.Add({FText::FromString(TEXT("Attack [LMB]")), HodgeGameplayTags::InputTag_Ability_Melee});
    Slots.Add({FText::FromString(TEXT("Jump [Space]")), HodgeGameplayTags::InputTag_Jump});
}

void UHodgeAbilityBarWidget::NativeOnInitialized()
{
    if (!WidgetTree->RootWidget)
    {
        auto* Border = WidgetTree->ConstructWidget<UBorder>(); Border->SetPadding(FMargin(12));
        Border->SetBrushColor(FLinearColor(.025f,.035f,.05f,.85f));
        auto* Row = WidgetTree->ConstructWidget<UHorizontalBox>(); Border->AddChild(Row);
        for (int32 Index = 0; Index < Slots.Num(); ++Index)
        {
            auto* B = WidgetTree->ConstructWidget<UHodgeAbilitySlotButton>();
            auto* Label = Text(WidgetTree, Slots[Index].Label.ToString(), 18); B->AddChild(Label); Row->AddChild(B);
            Buttons.Add(B); Labels.Add(Label);
            B->BindSlot(this, Index);
        }
        WidgetTree->RootWidget = Border;
    }
    Super::NativeOnInitialized();
}

void UHodgeAbilityBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RefreshAbilities();
    GetWorld()->GetTimerManager().SetTimer(RefreshTimer, this, &ThisClass::RefreshAbilities, .1f, true);
}

void UHodgeAbilityBarWidget::RefreshAbilities()
{
    auto* PC = Cast<AHodgePlayerController>(GetOwningPlayer());
    auto* ASC = PC ? PC->GetHodgeAbilitySystemComponent() : nullptr;
    auto* Pawn = PC ? PC->GetPawn() : nullptr;
    auto* Health = Pawn ? UHodgeHealthComponent::FindHealthComponent(Pawn) : nullptr;
    const bool bReady = ASC && Pawn && ASC->GetAvatarActor() == Pawn && Health && Health->GetMaxHealth() > 0
        && !Health->IsDeadOrDying() && !ASC->HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked)
        && UHodgeUIManagerSubsystem::AllowsGameplayInput(PC);
    Available.SetNumZeroed(Slots.Num()); Cooldowns.SetNumZeroed(Slots.Num());
    for (int32 Index = 0; Index < Slots.Num(); ++Index)
    {
        bool bFound = false;
        if (bReady)
        {
            const auto* Combat = Pawn->FindComponentByClass<UHodgeCombatComponentBase>();
            const auto* Combo = Combat ? Combat->GetComboDefinition() : nullptr;
            if (Combo)
            { for (const auto& Binding : Combo->InputBindings) { if (Binding.InputTag == Slots[Index].InputTag) { bFound = true; } } }
            for (const auto& Spec : ASC->GetActivatableAbilities())
            {
                if (!Spec.GetDynamicSpecSourceTags().HasTagExact(Slots[Index].InputTag) || !Spec.Ability) { continue; }
                bFound = Spec.Ability->CanActivateAbility(Spec.Handle, ASC->AbilityActorInfo.Get());
                if (const auto* Tags = Spec.Ability->GetCooldownTags(); Tags && !Tags->IsEmpty())
                {
                    for (float Remaining : ASC->GetActiveEffectsTimeRemaining(FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*Tags)))
                    { Cooldowns[Index] = FMath::Max(Cooldowns[Index], Remaining); }
                }
                break;
            }
        }
        Available[Index] = bReady && bFound && Cooldowns[Index] <= 0;
        if (Buttons.IsValidIndex(Index)) { Buttons[Index]->SetIsEnabled(Available[Index]); }
        if (Labels.IsValidIndex(Index))
        { Labels[Index]->SetText(Cooldowns[Index] > 0 ? FText::FromString(FString::Printf(TEXT("%s  %.1fs"), *Slots[Index].Label.ToString(), Cooldowns[Index])) : Slots[Index].Label); }
    }
    OnAbilityDisplayUpdated();
}

void UHodgeAbilityBarWidget::TriggerSlot(int32 Index)
{
    RefreshAbilities();
    if (!Available.IsValidIndex(Index) || !Available[Index]) { return; }
    if (auto* PC = Cast<AHodgePlayerController>(GetOwningPlayer()))
    { if (auto* ASC = PC->GetHodgeAbilitySystemComponent())
      { ASC->AbilityInputTagPressed(Slots[Index].InputTag); ASC->AbilityInputTagReleased(Slots[Index].InputTag); } }
}
void UHodgeAbilityBarWidget::NativeDestruct()
{
    if (GetWorld()) { GetWorld()->GetTimerManager().ClearTimer(RefreshTimer); }
    Super::NativeDestruct();
}

void UHodgeAbilitySlotButton::BindSlot(UHodgeAbilityBarWidget* Owner, int32 InIndex)
{
    Bar = Owner; Index = InIndex;
    OnClicked.AddUniqueDynamic(this, &ThisClass::Trigger);
}
void UHodgeAbilitySlotButton::Trigger() { if (Bar.IsValid()) { Bar->TriggerSlot(Index); } }

UHodgePauseMenuWidget::UHodgePauseMenuWidget(const FObjectInitializer& Initializer) : Super(Initializer)
{ InputConfig = EHodgeWidgetInputMode::Menu; bIsBackHandler = true; ConfirmationClass = UHodgeConfirmationWidget::StaticClass(); }

void UHodgePauseMenuWidget::NativeOnInitialized()
{
    if (!WidgetTree->RootWidget)
    {
        auto* Column = Panel(WidgetTree, 380); Column->AddChild(Text(WidgetTree, TEXT("Game menu"), 24));
        ResumeButton = Button(WidgetTree, Column, TEXT("Continue")); ResumeButton->OnClicked.AddUniqueDynamic(this, &ThisClass::Resume);
        auto* Confirm = Button(WidgetTree, Column, TEXT("Continue with confirmation")); Confirm->OnClicked.AddUniqueDynamic(this, &ThisClass::ShowConfirmation);
        Column->AddChild(Text(WidgetTree, TEXT("Esc / B  Back"), 14));
    }
    Super::NativeOnInitialized();
}
UWidget* UHodgePauseMenuWidget::NativeGetDesiredFocusTarget() const { return ResumeButton ? ResumeButton.Get() : Super::NativeGetDesiredFocusTarget(); }
void UHodgePauseMenuWidget::Resume() { if (auto* Layout = Root(this)) { Layout->Pop(this); } }
void UHodgePauseMenuWidget::ShowConfirmation()
{
    if (Confirmation.IsValid() && Confirmation->IsActivated()) { return; }
    if (auto* Layout = Root(this))
    {
        auto* Dialog = Cast<UHodgeConfirmationWidget>(Layout->Push(HodgeGameplayTags::UI_Layer_Modal, ConfirmationClass));
        Confirmation = Dialog;
        if (Dialog) { TWeakObjectPtr<UHodgePauseMenuWidget> Weak = this;
            Dialog->SetResultCallback([Weak](bool Accepted) { if (Accepted && Weak.IsValid()) { Weak->Resume(); } }); }
    }
}

void UHodgePauseMenuWidget::DismissConfirmation()
{
    auto Dialog = Confirmation; Confirmation.Reset();
    if (auto* Layout = Root(this)) { Layout->Pop(Dialog.Get()); }
}
void UHodgePauseMenuWidget::NativeOnDeactivated()
{
    // 菜单退出时只撤掉自己的确认框，其他 Modal 页面保持各自所有权。
    DismissConfirmation();
    Super::NativeOnDeactivated();
}
void UHodgePauseMenuWidget::NativeDestruct()
{
    DismissConfirmation();
    Super::NativeDestruct();
}

UHodgeConfirmationWidget::UHodgeConfirmationWidget(const FObjectInitializer& Initializer) : Super(Initializer)
{ InputConfig = EHodgeWidgetInputMode::Menu; bIsBackHandler = true; }
void UHodgeConfirmationWidget::NativeOnInitialized()
{
    if (!WidgetTree->RootWidget)
    {
        auto* Column = Panel(WidgetTree, 400); Column->AddChild(Text(WidgetTree, TEXT("Return to the game?"), 22));
        AcceptButton = Button(WidgetTree, Column, TEXT("Confirm")); AcceptButton->OnClicked.AddUniqueDynamic(this, &ThisClass::Accept);
        auto* B = Button(WidgetTree, Column, TEXT("Cancel")); B->OnClicked.AddUniqueDynamic(this, &ThisClass::Cancel);
    }
    Super::NativeOnInitialized();
}
UWidget* UHodgeConfirmationWidget::NativeGetDesiredFocusTarget() const { return AcceptButton ? AcceptButton.Get() : Super::NativeGetDesiredFocusTarget(); }
void UHodgeConfirmationWidget::Complete(bool Accepted)
{
    auto Callback = MoveTemp(Result);
    if (auto* Layout = Root(this)) { Layout->Pop(this); }
    if (Callback) { Callback(Accepted); }
}
void UHodgeConfirmationWidget::Accept() { Complete(true); }
void UHodgeConfirmationWidget::Cancel() { Complete(false); }
void UHodgeConfirmationWidget::NativeDestruct()
{
    auto Callback = MoveTemp(Result);
    if (Callback) { Callback(false); }
    Super::NativeDestruct();
}

void UHodgeConfirmationWidget::NativeOnDeactivated()
{
    auto Callback = MoveTemp(Result);
    if (Callback) { Callback(false); }
    Super::NativeOnDeactivated();
}
