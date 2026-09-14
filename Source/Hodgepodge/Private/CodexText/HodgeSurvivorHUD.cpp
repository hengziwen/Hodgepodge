#include "CodexText/HodgeSurvivor.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Styling/CoreStyle.h"
#include "Engine/World.h"

namespace
{
void Place(UCanvasPanel* Canvas,UWidget* Widget,FVector2D Position,FVector2D Size)
{
    auto* Slot=Canvas->AddChildToCanvas(Widget);
    Slot->SetPosition(Position); Slot->SetSize(Size);
}
void Stretch(UCanvasPanel* Canvas,UWidget* Widget)
{
    auto* Slot=Canvas->AddChildToCanvas(Widget);
    Slot->SetAnchors(FAnchors(0,0,1,1)); Slot->SetOffsets(FMargin(0));
}
}
AHodgeSurvivorMode* UHodgeSurvivorHUD::Mode() const { return GetWorld()?GetWorld()->GetAuthGameMode<AHodgeSurvivorMode>():nullptr; }
void UHodgeSurvivorHUD::Text(FName Name,const FString& Value)
{
    if (auto* Label=Cast<UTextBlock>(GetWidgetFromName(Name))) Label->SetText(FText::FromString(Value));
}
UButton* UHodgeSurvivorHUD::AddButton(UVerticalBox* Box,const FString& Label,UTextBlock*& OutLabel)
{
    auto* Button=WidgetTree->ConstructWidget<UButton>();
    Button->SetBackgroundColor(FLinearColor(.08f,.19f,.26f,1));
    Button->IsFocusable=false;
    OutLabel=WidgetTree->ConstructWidget<UTextBlock>();
    OutLabel->SetText(FText::FromString(Label));
    OutLabel->SetFont(FCoreStyle::GetDefaultFontStyle("Bold",23));
    OutLabel->SetColorAndOpacity(FSlateColor(FLinearColor(.88f,.98f,1)));
    OutLabel->SetMargin(FMargin(20,16));
    Button->AddChild(OutLabel);
    Box->AddChildToVerticalBox(Button)->SetPadding(FMargin(0,5));
    return Button;
}
void UHodgeSurvivorHUD::NativeConstruct()
{
    Super::NativeConstruct();
    if (Modal || !WidgetTree) return;
    auto* Canvas=Cast<UCanvasPanel>(GetWidgetFromName(TEXT("HUDCanvas")));
    if (!Canvas) return;
    TArray<UWidget*> Widgets; WidgetTree->GetAllWidgets(Widgets);
    for (UWidget* Widget:Widgets)
    {
        const FString Name=Widget->GetName();
        if (Name==TEXT("SnowWhiteBackground") || Name.StartsWith(TEXT("Compass")) || Name.StartsWith(TEXT("Heading")) || Name.StartsWith(TEXT("Radar")) || Name==TEXT("PreviewDisclaimer"))
            Widget->SetVisibility(ESlateVisibility::Collapsed);
    }
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    Text(TEXT("TaskTitle"),TEXT("白夜幸存者"));
    Text(TEXT("TaskName"),TEXT("守住阵地 · 生存 3 分钟"));
    Text(TEXT("TaskDescription"),TEXT("自动索敌攻击，拾取蓝色晶体升级"));
    Text(TEXT("ChatHint"),TEXT("WASD 移动 / 空格 冲刺"));
    Text(TEXT("PartyStatus"),TEXT("自动攻击 · 靠近晶体可吸收"));
    Text(TEXT("ColdWarning"),TEXT("远离包围！精英每 35 秒出现"));
    Text(TEXT("ControlLabel0"),TEXT("自动攻击")); Text(TEXT("ControlLabel1"),TEXT("无需瞄准"));
    Text(TEXT("ControlKey2"),TEXT("W")); Text(TEXT("ControlLabel2"),TEXT("WASD 移动"));
    Text(TEXT("ControlKey3"),TEXT("空")); Text(TEXT("ControlLabel3"),TEXT("空格 冲刺"));
    Text(TEXT("ControlKey4"),TEXT("P")); Text(TEXT("ControlLabel4"),TEXT("暂停 / 继续"));
    Text(TEXT("ControlKey5"),TEXT("1")); Text(TEXT("ControlLabel5"),TEXT("1 / 2 / 3 升级"));
    Text(TEXT("WeaponName"),TEXT("自动脉冲枪")); Text(TEXT("AmmoType"),TEXT("无限能量"));
    Text(TEXT("CarryWeight"),TEXT("移动收集经验"));
    Text(TEXT("HotbarLabel0"),TEXT("伤害")); Text(TEXT("HotbarLabel1"),TEXT("攻速"));
    Text(TEXT("HotbarLabel2"),TEXT("脉冲")); Text(TEXT("HotbarLabel3"),TEXT("拾取"));
    Text(TEXT("OutdoorTemperature"),TEXT("生存挑战")); Text(TEXT("BodyTemperature"),TEXT("3:00 完成撤离"));
    if (auto* Return=Cast<UButton>(GetWidgetFromName(TEXT("ReturnButton"))))
    {
        Return->OnClicked.Clear(); Return->OnClicked.AddDynamic(this,&ThisClass::Menu);
    }
    XPBar=WidgetTree->ConstructWidget<UProgressBar>();
    XPBar->SetFillColorAndOpacity(FLinearColor(.1f,.8f,1));
    Place(Canvas,XPBar,FVector2D(500,42),FVector2D(630,15));
    XPText=WidgetTree->ConstructWidget<UTextBlock>();
    XPText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold",24));
    XPText->SetJustification(ETextJustify::Center);
    Place(Canvas,XPText,FVector2D(500,64),FVector2D(630,38));
    Modal=WidgetTree->ConstructWidget<UCanvasPanel>();
    Stretch(Canvas,Modal);
    auto* Shade=WidgetTree->ConstructWidget<UBorder>();
    Shade->SetBrushColor(FLinearColor(.005f,.012f,.025f,.94f)); Stretch(Modal,Shade);
    auto* Box=WidgetTree->ConstructWidget<UVerticalBox>();
    Place(Modal,Box,FVector2D(466,130),FVector2D(740,720));
    ModalTitle=WidgetTree->ConstructWidget<UTextBlock>();
    ModalTitle->SetFont(FCoreStyle::GetDefaultFontStyle("Bold",46));
    ModalTitle->SetColorAndOpacity(FSlateColor(FLinearColor(.3f,.9f,1)));
    Box->AddChildToVerticalBox(ModalTitle)->SetPadding(FMargin(0,0,0,15));
    ModalDescription=WidgetTree->ConstructWidget<UTextBlock>();
    ModalDescription->SetFont(FCoreStyle::GetDefaultFontStyle("Regular",24));
    ModalDescription->SetAutoWrapText(true);
    Box->AddChildToVerticalBox(ModalDescription)->SetPadding(FMargin(0,0,0,20));
    for (int32 I=0;I<3;++I)
    {
        UTextBlock* Label=nullptr;
        ChoiceButtons.Add(AddButton(Box,TEXT(""),Label)); ChoiceLabels.Add(Label);
    }
    ChoiceButtons[0]->OnClicked.AddDynamic(this,&ThisClass::ChooseOne);
    ChoiceButtons[1]->OnClicked.AddDynamic(this,&ThisClass::ChooseTwo);
    ChoiceButtons[2]->OnClicked.AddDynamic(this,&ThisClass::ChooseThree);
    UTextBlock* Label=nullptr;
    PauseButton=AddButton(Box,TEXT("继续战斗  [P]"),Label); PauseLabel=Label;
    PauseButton->OnClicked.AddDynamic(this,&ThisClass::Pause);
    RetryButton=AddButton(Box,TEXT("重新开始"),Label); RetryButton->OnClicked.AddDynamic(this,&ThisClass::Retry);
    MenuButton=AddButton(Box,TEXT("返回主菜单"),Label); MenuButton->OnClicked.AddDynamic(this,&ThisClass::Menu);
    Modal->SetVisibility(ESlateVisibility::Collapsed);
}
void UHodgeSurvivorHUD::Refresh(AHodgeSurvivorMode* M)
{
    if (!M || !Modal) return;
    const int32 Seconds=FMath::Max(0,FMath::CeilToInt(M->RoundDuration-M->Elapsed));
    Text(TEXT("DayNumber"),FString::Printf(TEXT("第 %d 波"),1+int32(M->Elapsed/30)));
    Text(TEXT("DayTime"),FString::Printf(TEXT("%02d:%02d"),Seconds/60,Seconds%60));
    Text(TEXT("TaskDistance"),FString::Printf(TEXT("%d 击杀"),M->Kills));
    Text(TEXT("HealthValue"),FString::Printf(TEXT("%.0f"),M->Health));
    Text(TEXT("StaminaValue"),FString::Printf(TEXT("Lv.%d"),M->Level));
    Text(TEXT("HungerValue"),FString::Printf(TEXT("%d"),M->Kills));
    Text(TEXT("ThirstValue"),M->Hero && M->Hero->DashCooldown>0?FString::Printf(TEXT("%.1fs"),M->Hero->DashCooldown):TEXT("就绪"));
    Text(TEXT("MagazineAmmo"),FString::Printf(TEXT("%.0f"),M->Damage)); Text(TEXT("ReserveAmmo"),TEXT(" 伤害"));
    Text(TEXT("ItemCount0"),FString::Printf(TEXT("%.0f"),M->Damage));
    Text(TEXT("ItemCount1"),FString::Printf(TEXT("%.1f/s"),1/M->AttackInterval));
    Text(TEXT("ItemCount2"),FString::Printf(TEXT("Lv.%d"),M->PulseLevel));
    Text(TEXT("ItemCount3"),FString::Printf(TEXT("%.0f"),M->PickupRadius));
    Text(TEXT("ItemCount4"),TEXT("空格")); Text(TEXT("ItemCount5"),TEXT("P 暂停")); Text(TEXT("ItemCount6"),TEXT("自动"));
    const float XP=FMath::Clamp(float(M->Experience)/M->NextLevelXP,0.f,1.f);
    XPBar->SetPercent(XP);
    XPText->SetText(FText::FromString(FString::Printf(TEXT("LEVEL %02d   ·   经验 %d / %d"),M->Level,M->Experience,M->NextLevelXP)));
    const FName Bars[]={TEXT("HealthBar"),TEXT("StaminaBar"),TEXT("HungerBar"),TEXT("ThirstBar")};
    const float Values[]={M->Health/M->MaxHealth,XP,FMath::Clamp(M->Elapsed/M->RoundDuration,0.f,1.f),M->Hero?1-M->Hero->DashCooldown/3:1};
    for (int32 I=0;I<4;++I)
        if (UWidget* Bar=GetWidgetFromName(Bars[I])) { Bar->SetRenderTransformPivot(FVector2D(0,.5f)); Bar->SetRenderScale(FVector2D(FMath::Clamp(Values[I],0.f,1.f),1)); }
    const bool Upgrade=M->State==EHodgeSurvivorState::Upgrade;
    const bool Paused=M->State==EHodgeSurvivorState::Paused;
    Modal->SetVisibility(M->State==EHodgeSurvivorState::Playing?ESlateVisibility::Collapsed:ESlateVisibility::Visible);
    for (int32 I=0;I<3;++I)
    {
        ChoiceButtons[I]->SetVisibility(Upgrade?ESlateVisibility::Visible:ESlateVisibility::Collapsed);
        ChoiceLabels[I]->SetText(FText::FromString(FString::Printf(TEXT("[%d]  %s"),I+1,*M->GetUpgradeText(I))));
    }
    PauseButton->SetVisibility(Paused?ESlateVisibility::Visible:ESlateVisibility::Collapsed);
    RetryButton->SetVisibility(Upgrade?ESlateVisibility::Collapsed:ESlateVisibility::Visible);
    const FString Title=Upgrade?TEXT("升级 · 选择强化"):Paused?TEXT("已暂停"):M->State==EHodgeSurvivorState::Victory?TEXT("撤离成功"):TEXT("挑战结束");
    ModalTitle->SetText(FText::FromString(Title));
    ModalDescription->SetText(FText::FromString(Upgrade?TEXT("敌人已暂停。选择一项强化，继续你的生存之旅。"):
        FString::Printf(TEXT("存活 %02d:%02d  ·  击杀 %d  ·  等级 %d\nWASD 移动，空格冲刺，拾取晶体获得强化。"),int32(M->Elapsed)/60,int32(M->Elapsed)%60,M->Kills,M->Level)));
}
void UHodgeSurvivorHUD::NativeDestruct()
{
    for (UButton* Button:ChoiceButtons) if (Button) Button->OnClicked.RemoveAll(this);
    if (RetryButton) RetryButton->OnClicked.RemoveAll(this);
    if (MenuButton) MenuButton->OnClicked.RemoveAll(this);
    if (PauseButton) PauseButton->OnClicked.RemoveAll(this);
    Super::NativeDestruct();
}
void UHodgeSurvivorHUD::ChooseOne() { if (auto* M=Mode()) M->ChooseUpgrade(0); }
void UHodgeSurvivorHUD::ChooseTwo() { if (auto* M=Mode()) M->ChooseUpgrade(1); }
void UHodgeSurvivorHUD::ChooseThree() { if (auto* M=Mode()) M->ChooseUpgrade(2); }
void UHodgeSurvivorHUD::Retry() { if (auto* M=Mode()) M->RestartRun(); }
void UHodgeSurvivorHUD::Menu() { if (auto* M=Mode()) M->ReturnToMenu(); }
void UHodgeSurvivorHUD::Pause() { if (auto* M=Mode()) M->TogglePause(); }
