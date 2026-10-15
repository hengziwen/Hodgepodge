/** @file HodgeAbilityIconWidget.cpp @brief 底部向上冷却恢复、中心倒计时及独立顺时针能量环。 */
#include "UI/HodgeAbilityIconWidget.h"
#include "UI/Data/HodgeGameplayUIDataSource.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilityIconWidget)

FHodgeAbilityIconVisualState UHodgeAbilityIconWidget::EvaluateState(const FHodgeUIAbilityDisplayState& Ability,const FHodgeUIResonanceSnapshot& Resource,bool bEnergy)
{
	FHodgeAbilityIconVisualState Out;
	Out.bReady=Ability.bGranted && (!bEnergy || Resource.bReady);
	Out.CooldownRemaining=FMath::IsFinite(Ability.CooldownRemaining) ? FMath::Max(0.f,Ability.CooldownRemaining) : 0.f;
	Out.bCoolingDown=Out.CooldownRemaining>0.f;
	Out.CooldownReadyPercent=Out.bCoolingDown ? (FMath::IsFinite(Ability.CooldownDuration) && Ability.CooldownDuration>0.f ? FMath::Clamp(1.f-Out.CooldownRemaining/Ability.CooldownDuration,0.f,1.f) : 0.f) : 1.f;
	Out.EnergyPercent=bEnergy && Resource.bReady && FMath::IsFinite(Resource.Percent) ? FMath::Clamp(Resource.Percent,0.f,1.f) : 0.f;
	Out.bCanActivate=Out.bReady && Ability.bInputAllowed && !Out.bCoolingDown;
	return Out;
}

void UHodgeAbilityIconWidget::NativeConstruct()
{
	Super::NativeConstruct(); ApplyStyle();
	if (AbilityButton) { AbilityButton->OnClicked.AddUniqueDynamic(this,&ThisClass::RequestAbility); }
	RefreshDisplay();
}

void UHodgeAbilityIconWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties(); ApplyStyle(); RefreshDisplay();
}

void UHodgeAbilityIconWidget::ApplyStyle()
{
	// 与Lyra MaterialProgressBar一致，每个Image持有自己的MID，不改共享材质。
	if (SkillIcon && IconMaterial)
	{
		if (!IconMID || IconMID->Parent!=IconMaterial) { SkillIcon->SetBrushFromMaterial(IconMaterial); IconMID=SkillIcon->GetDynamicMaterial(); }
		if (IconMID) { IconMID->SetTextureParameterValue(TEXT("IconTexture"),IconTexture); IconMID->SetScalarParameterValue(TEXT("Darkness"),FMath::Clamp(CooldownDarkness,0.f,1.f)); }
	}
	if (EnergyRing && EnergyRingMaterial)
	{
		if (!RingMID || RingMID->Parent!=EnergyRingMaterial) { EnergyRing->SetBrushFromMaterial(EnergyRingMaterial); RingMID=EnergyRing->GetDynamicMaterial(); }
		if (RingMID) { RingMID->SetVectorParameterValue(TEXT("ColorA"),FillColor); }
	}
	// 布局和字体以Designer为准，运行时仅同步显示内容与材质参数。
	if (KeyText) { KeyText->SetText(KeyLabel); }
	if (AbilityButton) { AbilityButton->SetToolTipText(Caption); }
}

void UHodgeAbilityIconWidget::NativeTick(const FGeometry& Geometry,float DeltaTime)
{
	Super::NativeTick(Geometry,DeltaTime); RefreshDisplay();
}

void UHodgeAbilityIconWidget::RefreshDisplay()
{
	if (IsDesignTime())
	{
		VisualState.bReady=true; VisualState.bCanActivate=true; VisualState.EnergyPercent=FMath::Clamp(PreviewEnergyPercent,0.f,1.f);
		VisualState.CooldownReadyPercent=FMath::Clamp(PreviewCooldownReadyPercent,0.f,1.f); VisualState.bCoolingDown=VisualState.CooldownReadyPercent<1.f; VisualState.CooldownRemaining=(1.f-VisualState.CooldownReadyPercent)*8.f;
	}
	else
	{
		Data=UHodgeUIManagerSubsystem::GetGameplayDataForController(GetOwningPlayer());
		VisualState=EvaluateState(Data ? Data->GetAbilityDisplayState(InputTag) : FHodgeUIAbilityDisplayState(),Data ? Data->GetResonance() : FHodgeUIResonanceSnapshot(),bShowEnergyRing);
	}
	if (IconMID)
	{
		IconMID->SetScalarParameterValue(TEXT("Progress"),VisualState.CooldownReadyPercent);
		IconMID->SetScalarParameterValue(TEXT("CoolingDown"),VisualState.bCoolingDown ? 1.f : 0.f);
		IconMID->SetScalarParameterValue(TEXT("Unavailable"),!VisualState.bCanActivate && !VisualState.bCoolingDown ? 1.f : 0.f);
	}
	if (RingMID) { RingMID->SetScalarParameterValue(TEXT("Progress"),VisualState.EnergyPercent); }
	if (EnergyRing) { EnergyRing->SetVisibility(bShowEnergyRing ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed); }
	if (CooldownText)
	{
		CooldownText->SetVisibility(VisualState.bCoolingDown ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		CooldownText->SetText(FText::AsNumber(FMath::CeilToInt(VisualState.CooldownRemaining)));
	}
	if (AbilityButton) { AbilityButton->SetIsEnabled(VisualState.bCanActivate); }
	// 透明等待晚到授予，不使用Collapsed停止UI Tick。
	SetVisibility(ESlateVisibility::SelfHitTestInvisible); SetRenderOpacity(VisualState.bReady ? 1.f : 0.f);
}

void UHodgeAbilityIconWidget::NativeDestruct()
{
	if (AbilityButton) { AbilityButton->OnClicked.RemoveAll(this); }
	Data=nullptr; IconMID=nullptr; RingMID=nullptr; Super::NativeDestruct();
}

void UHodgeAbilityIconWidget::RequestAbility()
{
	if (Data) { Data->SubmitInput(InputTag); }
}
