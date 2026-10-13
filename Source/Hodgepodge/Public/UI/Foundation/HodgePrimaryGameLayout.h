/** @file HodgePrimaryGameLayout.h
 * @brief 声明本地玩家UI层栈相关的类型、接口与现有处理流程。
 */
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
	/** 注册层。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param Container Container，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|UI")
	void RegisterLayer(FGameplayTag Tag, UCommonActivatableWidgetContainerBase* Container);
	/** 获取层。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @return 层的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|UI")
	UCommonActivatableWidgetContainerBase* GetLayer(FGameplayTag Tag) const;
	/** 将指定页面加入当前根布局的目标层。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param WidgetClass 控件类型，由调用方传入。
	 * @return 本次计算、查询或创建得到的CommonActivatable控件。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|UI")
	UCommonActivatableWidget* Push(FGameplayTag Tag, TSubclassOf<UCommonActivatableWidget> WidgetClass);
	/** 从所属根布局移除指定页面。
	 * @param Widget 控件，由调用方传入。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|UI")
	void Pop(UCommonActivatableWidget* Widget);
	/** 判断是否存在BlockingPage。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintPure, Category="Hodge|UI")
	bool HasBlockingPage() const;
	/** 异步加载页面并加入指定UI层，按请求身份处理取消。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param WidgetClass 控件类型，由调用方传入。
	 * @param Completed 已完成，由调用方传入。
	 * @param bSuspendInput 是否Suspend输入的控制标记。
	 * @return 本次计算、查询或创建得到的唯一标识。
	 */
	FGuid PushAsync(FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass,
	                TFunction<void(UCommonActivatableWidget*)> Completed, bool bSuspendInput = true);
	/** 取消对应的异步页面加载请求并释放挂起状态。
	 * @param Request 请求，由调用方传入。
	 */
	void CancelPush(FGuid Request);
	/** 关闭当前根布局并清理层栈与未完成的加载请求。 */
	void ReleaseLayout();
	/** 获取待处理请求数量。
	 * @return 待处理请求数量的当前值或引用。
	 */
	int32 GetPendingRequestCount() const { return Requests.Num(); }

protected:
    // 游戏层的数据或对象引用。
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> GameLayer;
    // 游戏菜单层的数据或对象引用。
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> GameMenuLayer;
    // 菜单层的数据或对象引用。
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> MenuLayer;
    // 模态页层的数据或对象引用。
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonActivatableWidgetContainerBase> ModalLayer;
	/** 在控件实例初始化时建立必要的输入或页面绑定。 */
	virtual void NativeOnInitialized() override;
	/** 在控件析构时解除界面监听与临时资源。 */
	virtual void NativeDestruct() override;

private:
	// Layers的记录集合。
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;

	/** 保存请求的数据和接口契约。 */
	struct FRequest
	{
		// 加载对应操作的配对句柄。
		TSharedPtr<FStreamableHandle> Load;
		// 当前角色使用的控制器。
		TWeakObjectPtr<APlayerController> Controller;
		// 已完成的数据或对象引用。
		TFunction<void(UCommonActivatableWidget*)> Completed;
		// 输入令牌的数据或对象引用。
		FName InputToken;
	};

	// Requests的记录集合。
	TMap<FGuid, FRequest> Requests;
	// 控制是否松开的状态标记。
	bool bReleased = false;
	/** 结束入栈。
	 * @param Request 请求，由调用方传入。
	 * @param Tag 标签，用于对应标签查询或匹配。
	 * @param WidgetClass 控件类型，由调用方传入。
	 */
	void FinishPush(FGuid Request, FGameplayTag Tag, TSoftClassPtr<UCommonActivatableWidget> WidgetClass);
};
