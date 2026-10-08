#pragma once
#include "UI/HodgeHUDLayout.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Components/Button.h"
#include "HodgeGameplayWidgets.generated.h"
class UTextBlock;
class UProgressBar;
class UButton;
class UHodgeHealthComponent;
class UHodgeLocalPlayerBase;
class UHodgePawnExtensionComponent;
class UHodgeAbilityBarWidget;
class UHodgeConfirmationWidget;

UCLASS()
class HODGEPODGE_API UHodgeAbilitySlotButton : public UButton
{
    GENERATED_BODY()
public:
    void BindSlot(UHodgeAbilityBarWidget* Owner, int32 InIndex);
private:
    TWeakObjectPtr<UHodgeAbilityBarWidget> Bar;
    int32 Index = INDEX_NONE;
    UFUNCTION() void Trigger();
};

/** 正式 HUD 的最小默认树；美术 Blueprint 可替换布局。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeGameplayHUDWidget : public UHodgeHUDLayout
{
    GENERATED_BODY()
public:
    UHodgeGameplayHUDWidget(const FObjectInitializer& Initializer);
protected:
    virtual void NativeOnInitialized() override;
};

/** 生命显示订阅当前 Pawn，换 Pawn 时解除旧对象绑定。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgePlayerVitalsWidget : public UCommonUserWidget
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, Category="Vitals") float DisplayedHealth = 0;
    UPROPERTY(BlueprintReadOnly, Category="Vitals") float DisplayedMaxHealth = 0;
    UPROPERTY(BlueprintReadOnly, Category="Vitals") bool bVitalsReady = false;
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    UFUNCTION(BlueprintImplementableEvent, Category="Vitals") void OnVitalsUpdated();
private:
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> HealthValue;
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UProgressBar> HealthBar;
    TWeakObjectPtr<UHodgeLocalPlayerBase> LocalPlayer;
    TWeakObjectPtr<UHodgeHealthComponent> Health;
    TWeakObjectPtr<UHodgePawnExtensionComponent> Extension;
    FDelegateHandle PawnDelegate;
    void PawnChanged(UHodgeLocalPlayerBase* Player, APawn* Pawn);
    void BindHealth();
    void UnbindHealth();
    void Refresh();
    UFUNCTION() void AttributeChanged(UHodgeHealthComponent* Component, float Old, float New, AActor* Instigator);
    UFUNCTION() void DeathChanged(AActor* Actor);
};

USTRUCT(BlueprintType)
struct FHodgeHUDAbilitySlot
{
    GENERATED_BODY()
    // 显示名称和键位提示属于 UI，不改变能力逻辑。
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Label;
    // 表达输入意图；普攻由 CombatComponent 按图选段。
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="InputTag")) FGameplayTag InputTag;
};

UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeAbilityBarWidget : public UCommonUserWidget
{
    GENERATED_BODY()
public:
    UHodgeAbilityBarWidget(const FObjectInitializer& Initializer);
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities") TArray<FHodgeHUDAbilitySlot> Slots;
    UPROPERTY(BlueprintReadOnly, Category="Abilities") TArray<bool> Available;
    UPROPERTY(BlueprintReadOnly, Category="Abilities") TArray<float> Cooldowns;
    UFUNCTION(BlueprintCallable, Category="Abilities") void TriggerSlot(int32 Index);
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    UFUNCTION(BlueprintImplementableEvent, Category="Abilities") void OnAbilityDisplayUpdated();
private:
    UPROPERTY(Transient) TArray<TObjectPtr<UTextBlock>> Labels;
    UPROPERTY(Transient) TArray<TObjectPtr<UButton>> Buttons;
    FTimerHandle RefreshTimer;
    void RefreshAbilities();
};

UCLASS(Blueprintable)
class HODGEPODGE_API UHodgePauseMenuWidget : public UHodgeActivatableWidget
{
    GENERATED_BODY()
public:
    UHodgePauseMenuWidget(const FObjectInitializer& Initializer);
    // 确认页面由菜单配置，正式资产可以替换原生默认外观。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Menu") TSubclassOf<UHodgeConfirmationWidget> ConfirmationClass;
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeOnDeactivated() override;
    virtual void NativeDestruct() override;
    virtual UWidget* NativeGetDesiredFocusTarget() const override;
private:
    UPROPERTY(Transient) TObjectPtr<UButton> ResumeButton;
    TWeakObjectPtr<UHodgeConfirmationWidget> Confirmation;
    void DismissConfirmation();
    UFUNCTION() void Resume();
    UFUNCTION() void ShowConfirmation();
};

UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeConfirmationWidget : public UHodgeActivatableWidget
{
    GENERATED_BODY()
public:
    UHodgeConfirmationWidget(const FObjectInitializer& Initializer);
    void SetResultCallback(TFunction<void(bool)> Callback) { Result = MoveTemp(Callback); }
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeDestruct() override;
    virtual UWidget* NativeGetDesiredFocusTarget() const override;
    virtual void NativeOnDeactivated() override;
private:
    UPROPERTY(Transient) TObjectPtr<UButton> AcceptButton;
    TFunction<void(bool)> Result;
    void Complete(bool Accepted);
    UFUNCTION() void Accept();
    UFUNCTION() void Cancel();
};
