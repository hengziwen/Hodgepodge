#pragma once
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "HodgePrimaryGameLayout.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;
struct FStreamableHandle;

/** 当前本地玩家的层栈和异步请求；业务内容由 Experience 注入。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgePrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Hodge|UI")
	void RegisterLayer(FGameplayTag Tag, UCommonActivatableWidgetContainerBase* Container);
	UFUNCTION(BlueprintPure, Category="Hodge|UI")
	UCommonActivatableWidgetContainerBase* GetLayer(FGameplayTag Tag) const;
	UFUNCTION(BlueprintCallable, Category="Hodge|UI")
	UCommonActivatableWidget* Push(FGameplayTag Tag, TSubclassOf<UCommonActivatableWidget> WidgetClass);
	UFUNCTION(BlueprintCallable, Category="Hodge|UI")
	void Pop(UCommonActivatableWidget* Widget);
	UFUNCTION(BlueprintPure, Category="Hodge|UI")
	bool HasBlockingPage() const;
	FGuid PushAsync(FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass,
	                TFunction<void(UCommonActivatableWidget*)> Completed, bool bSuspendInput = true);
	void CancelPush(FGuid Request);
	void ReleaseLayout();
	int32 GetPendingRequestCount() const { return Requests.Num(); }

protected:
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> GameLayer;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> GameMenuLayer;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> MenuLayer;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> ModalLayer;
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;

	struct FRequest
	{
		TSharedPtr<FStreamableHandle> Load;
		TWeakObjectPtr<APlayerController> Controller;
		TFunction<void(UCommonActivatableWidget*)> Completed;
		FName InputToken;
	};

	TMap<FGuid, FRequest> Requests;
	bool bReleased = false;
	void FinishPush(FGuid Request, FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass);
};
