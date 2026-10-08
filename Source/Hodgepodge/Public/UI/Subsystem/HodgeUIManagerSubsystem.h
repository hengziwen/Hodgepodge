#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/SoftObjectPtr.h"
#include "HodgeUIManagerSubsystem.generated.h"
class UHodgePrimaryGameLayout;
class UHodgeGameplayUIDataSource;
class ULocalPlayer;
class APlayerController;
struct FStreamableHandle;
DECLARE_MULTICAST_DELEGATE_TwoParams(FHodgeRootLayoutChanged, ULocalPlayer*, UHodgePrimaryGameLayout*);

USTRUCT()
struct FHodgePlayerUILayout
{
    GENERATED_BODY()
    UPROPERTY(Transient) TObjectPtr<UHodgePrimaryGameLayout> Root;
    UPROPERTY(Transient) TObjectPtr<UHodgeGameplayUIDataSource> GameplayData;
    UPROPERTY(Transient) TWeakObjectPtr<APlayerController> Controller;
    FDelegateHandle ControllerDelegate;
    FDelegateHandle InputModeDelegate;
    TSet<FName> InputTokens;
    TSharedPtr<FStreamableHandle> RootLoad;
};

/** 每个本地玩家独立拥有根布局，不管理角色属性或在线会话。 */
UCLASS(Config=Game)
class HODGEPODGE_API UHodgeUIManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    void PlayerAdded(ULocalPlayer* Player);
    void PlayerRemoved(ULocalPlayer* Player);
    UFUNCTION(BlueprintPure, Category="Hodge|UI") UHodgePrimaryGameLayout* GetRootLayout(ULocalPlayer* Player) const;
    UFUNCTION(BlueprintPure, Category="Hodge|UI") bool IsGameInputAllowed(ULocalPlayer* Player) const;
    UFUNCTION(BlueprintPure, Category="Hodge|UI", meta=(WorldContext="WorldContextObject"))
    static UHodgeUIManagerSubsystem* GetUIManager(const UObject* WorldContextObject);
    UFUNCTION(BlueprintPure, Category="Hodge|UI") static UHodgePrimaryGameLayout* GetRootLayoutForController(APlayerController* Controller);
    UFUNCTION(BlueprintPure, Category="Hodge|UI") static UHodgeGameplayUIDataSource* GetGameplayDataForController(APlayerController* Controller);
    static bool AllowsGameplayInput(const APlayerController* Controller);
    void SuspendInput(ULocalPlayer* Player, FName Token);
    void ResumeInput(ULocalPlayer* Player, FName Token);
    void RefreshInputState(ULocalPlayer* Player);
    FHodgeRootLayoutChanged OnRootLayoutChanged;
    int32 GetPlayerLayoutCount() const { return Players.Num(); }
protected:
    // 未配置时使用原生根布局；正式工程指向 Main 内的 Widget Blueprint。
    UPROPERTY(Config, EditDefaultsOnly, Category="UI") TSoftClassPtr<UHodgePrimaryGameLayout> RootLayoutClass;
private:
    UPROPERTY(Transient) TMap<TObjectPtr<ULocalPlayer>, FHodgePlayerUILayout> Players;
    void ControllerChanged(ULocalPlayer* Player, APlayerController* Controller);
    void CreateRoot(ULocalPlayer* Player, TWeakObjectPtr<APlayerController> Controller);
    void DestroyRoot(ULocalPlayer* Player);
    void ClearGameplayInput(ULocalPlayer* Player);
};
