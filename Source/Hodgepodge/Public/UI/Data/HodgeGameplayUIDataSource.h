#pragma once
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "HodgeGameplayUIDataSource.generated.h"

class UHodgeLocalPlayerBase;
class UHodgeHealthComponent;
class UHodgePawnExtensionComponent;
class AHodgePlayerState;
class APlayerController;
class APlayerState;
class APawn;
class AActor;

USTRUCT(BlueprintType)
struct FHodgeUIVitalsSnapshot
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) float Health = 0;
	UPROPERTY(BlueprintReadOnly) float MaxHealth = 0;
	UPROPERTY(BlueprintReadOnly) float Percent = 0;
	UPROPERTY(BlueprintReadOnly) bool bReady = false;
	UPROPERTY(BlueprintReadOnly) bool bDead = false;
};

USTRUCT(BlueprintType)
struct FHodgeUIAbilityDisplayState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) bool bGranted = false;
	UPROPERTY(BlueprintReadOnly) bool bInputAllowed = false;
	UPROPERTY(BlueprintReadOnly) float CooldownRemaining = 0;
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


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeUIVitalsChanged, const FHodgeUIVitalsSnapshot&, Snapshot);

/** 当前本地玩家共享的数据接入；不创建控件，也不拥有玩法权威。 */
UCLASS(BlueprintType)
class HODGEPODGE_API UHodgeGameplayUIDataSource : public UObject
{
	GENERATED_BODY()
public:
	void Initialize(UHodgeLocalPlayerBase* Player, APlayerController* InController);
	void Shutdown();
	UPROPERTY(BlueprintAssignable, Category="Hodge|UI") FHodgeUIVitalsChanged OnVitalsChanged;
	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeUIVitalsSnapshot GetVitals() const { return Vitals; }
	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeUIAbilityDisplayState GetAbilityDisplayState(FGameplayTag InputTag) const;
	UFUNCTION(BlueprintCallable, Category="Hodge|UI") bool SubmitInput(FGameplayTag InputTag);
private:
	UPROPERTY(Transient) FHodgeUIVitalsSnapshot Vitals;
	TWeakObjectPtr<UHodgeLocalPlayerBase> LocalPlayer;
	TWeakObjectPtr<APlayerController> Controller;
	TWeakObjectPtr<APawn> Avatar;
	TWeakObjectPtr<UHodgeHealthComponent> Health;
	TWeakObjectPtr<UHodgePawnExtensionComponent> Extension;
	TWeakObjectPtr<AHodgePlayerState> PlayerState;
	FDelegateHandle PawnDelegate;
	FDelegateHandle PlayerStateDelegate;
	FDelegateHandle ReadyDelegate;
	bool bBound = false;
	void PawnChanged(UHodgeLocalPlayerBase* Player, APawn* Pawn);
	void StateChanged(UHodgeLocalPlayerBase* Player, APlayerState* State);
	void UnbindPawn();
	void UnbindState();
	void Refresh();
	bool CanUseGameplay() const;
	UFUNCTION() void AttributeChanged(UHodgeHealthComponent* Component, float Old, float New, AActor* Instigator);
	UFUNCTION() void DeathChanged(AActor* Actor);
};
