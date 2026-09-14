#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "HodgeSurvivor.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UAnimationAsset;
class UMaterialInterface;
class UStaticMesh;
class UCanvasPanel;
class UVerticalBox;
class UTextBlock;
class UButton;
class UProgressBar;

UCLASS()
class HODGEPODGE_API AHodgeSurvivorHero : public ACharacter
{
    GENERATED_BODY()
public:
    AHodgeSurvivorHero();
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    UFUNCTION(BlueprintCallable) void Dash();
    UPROPERTY(BlueprintReadOnly) float DashCooldown = 0;
    UPROPERTY(BlueprintReadOnly) float Invulnerability = 0;
private:
    void Forward(const FInputActionValue& Value);
    void Right(const FInputActionValue& Value);
    UPROPERTY() TObjectPtr<UInputMappingContext> Mapping;
    UPROPERTY() TObjectPtr<UInputAction> ForwardAction;
    UPROPERTY() TObjectPtr<UInputAction> RightAction;
    UPROPERTY() TObjectPtr<UAnimationAsset> Idle;
    UPROPERTY() TObjectPtr<UAnimationAsset> Run;
    bool bRunning = false;
};

UENUM(BlueprintType)
enum class EHodgeSurvivorState : uint8 { Playing, Upgrade, Paused, Defeat, Victory };

struct FHodgeSurvivorEnemy
{
    TWeakObjectPtr<AActor> Actor;
    float Health = 30;
    float Speed = 130;
    bool bElite = false;
};

struct FHodgeSurvivorOrb
{
    TWeakObjectPtr<AActor> Actor;
    int32 Value = 1;
};

UCLASS()
class HODGEPODGE_API AHodgeSurvivorMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AHodgeSurvivorMode();
    virtual void StartPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    UFUNCTION(BlueprintCallable) void ChooseUpgrade(int32 Slot);
    UFUNCTION(BlueprintCallable) void TogglePause();
    UFUNCTION(BlueprintCallable) void RestartRun();
    UFUNCTION(BlueprintCallable) void ReturnToMenu();
    UFUNCTION(BlueprintCallable) void GrantExperience(int32 Amount);
    UFUNCTION(BlueprintCallable) void DamageHero(float Amount);
    UFUNCTION(BlueprintCallable) void SpawnEnemy(bool bElite = false);
    UFUNCTION(BlueprintPure) int32 GetEnemyCount() const { return Enemies.Num(); }
    UFUNCTION(BlueprintPure) int32 GetOrbCount() const { return Orbs.Num(); }
    UFUNCTION(BlueprintPure) FString GetUpgradeText(int32 Slot) const;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Survivor") float RoundDuration = 180;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Survivor") float SpawnInterval = 1.15f;
    UPROPERTY(EditDefaultsOnly, Category="Survivor") TSubclassOf<UUserWidget> InterfaceClass;
    UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EnemyMaterial;
    UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EliteMaterial;
    UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EnergyMaterial;
    UPROPERTY(BlueprintReadOnly) EHodgeSurvivorState State = EHodgeSurvivorState::Playing;
    UPROPERTY(BlueprintReadOnly) float Health = 100;
    UPROPERTY(BlueprintReadOnly) float MaxHealth = 100;
    UPROPERTY(BlueprintReadOnly) float Elapsed = 0;
    UPROPERTY(BlueprintReadOnly) int32 Kills = 0;
    UPROPERTY(BlueprintReadOnly) int32 Level = 1;
    UPROPERTY(BlueprintReadOnly) int32 Experience = 0;
    UPROPERTY(BlueprintReadOnly) int32 NextLevelXP = 5;
    UPROPERTY(BlueprintReadOnly) float Damage = 22;
    UPROPERTY(BlueprintReadOnly) float AttackInterval = .65f;
    UPROPERTY(BlueprintReadOnly) float PickupRadius = 320;
    UPROPERTY(BlueprintReadOnly) int32 PulseLevel = 0;
    UPROPERTY(BlueprintReadOnly) TObjectPtr<AHodgeSurvivorHero> Hero;
    UPROPERTY(BlueprintReadOnly) TObjectPtr<UUserWidget> Interface;
    TArray<int32> Choices;
private:
    void SetRunState(EHodgeSurvivorState NewState);
    void HitEnemy(int32 Index, float Amount);
    AActor* MakeShape(FVector Location, FVector Scale, UMaterialInterface* Material, float Life = 0);
    UPROPERTY() TObjectPtr<UStaticMesh> Sphere;
    UPROPERTY() TObjectPtr<USkeletalMesh> EnemyMesh;
    UPROPERTY() TObjectPtr<UAnimationAsset> EnemyRun;
    TArray<FHodgeSurvivorEnemy> Enemies;
    TArray<FHodgeSurvivorOrb> Orbs;
    float SpawnClock = 0;
    float AttackClock = 0;
    float HurtClock = 0;
    float PulseClock = 0;
    float InterfaceClock = 0;
    int32 NextElite = 35;
};

UCLASS()
class HODGEPODGE_API UHodgeSurvivorHUD : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    void Refresh(AHodgeSurvivorMode* Mode);
    UFUNCTION() void ChooseOne();
    UFUNCTION() void ChooseTwo();
    UFUNCTION() void ChooseThree();
    UFUNCTION() void Retry();
    UFUNCTION() void Menu();
    UFUNCTION() void Pause();
private:
    AHodgeSurvivorMode* Mode() const;
    void Text(FName Name, const FString& Value);
    UButton* AddButton(UVerticalBox* Box, const FString& Label, UTextBlock*& OutLabel);
    UPROPERTY() TObjectPtr<UCanvasPanel> Modal;
    UPROPERTY() TObjectPtr<UTextBlock> ModalTitle;
    UPROPERTY() TObjectPtr<UTextBlock> ModalDescription;
    UPROPERTY() TArray<TObjectPtr<UButton>> ChoiceButtons;
    UPROPERTY() TArray<TObjectPtr<UTextBlock>> ChoiceLabels;
    UPROPERTY() TObjectPtr<UButton> RetryButton;
    UPROPERTY() TObjectPtr<UButton> MenuButton;
    UPROPERTY() TObjectPtr<UButton> PauseButton;
    UPROPERTY() TObjectPtr<UProgressBar> XPBar;
    UPROPERTY() TObjectPtr<UTextBlock> XPText;
    UPROPERTY() TObjectPtr<UTextBlock> PauseLabel;
};
