/** @file HodgeTabListWidgetBase.h
 * @brief 声明标签页列表相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonTabListWidgetBase.h"

#include "HodgeTabListWidgetBase.generated.h"

class UCommonButtonBase;
class UCommonUserWidget;
class UObject;
class UWidget;
struct FFrame;

/** 保存Tab描述的数据和接口契约。 */
USTRUCT(BlueprintType)
struct FHodgeTabDescriptor
{
	GENERATED_BODY()

public:
	/** 构造Tab描述并初始化其默认状态。 */
	FHodgeTabDescriptor()
		: bHidden(false)
		  , CreatedTabContentWidget(nullptr)
	{
	}

	// Tab标识的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TabId;

	// Tab文字的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TabText;

	// Icon画刷的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush IconBrush;

	// 控制是否Hidden的状态标记。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bHidden;

	// Tab按钮类型的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonButtonBase> TabButtonType;

	//TODO NDarnell - This should become a TSoftClassPtr<>, the underlying common tab list needs to be able to handle lazy tab content construction.
	// Tab内容类型的数据或对象引用。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonUserWidget> TabContentType;

	// CreatedTab内容控件的数据或对象引用。
	UPROPERTY(Transient)
	TObjectPtr<UWidget> CreatedTabContentWidget;
};

/** 封装Tab按钮接口的状态与操作接口。 */
UINTERFACE(BlueprintType)
class UHodgeTabButtonInterface : public UInterface
{
	GENERATED_BODY()
};

/** 封装Tab按钮接口的状态与操作接口。 */
class IHodgeTabButtonInterface
{
	GENERATED_BODY()

public:
	/** 设置Tab标签信息。
	 * @param TabDescriptor Tab描述，由调用方传入。
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Tab Button")
	void SetTabLabelInfo(const FHodgeTabDescriptor& TabDescriptor);
};

/** 封装标签页列表的状态与操作接口。 */
UCLASS(Blueprintable, BlueprintType, Abstract, meta = (DisableNativeTick))
class HODGEPODGE_API UHodgeTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:
	/** 获取PreregisteredTab信息。
	 * @param TabNameId Tab名称标识，由调用方传入。
	 * @param OutTabInfo 输出Tab信息。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
	bool GetPreregisteredTabInfo(const FName TabNameId, FHodgeTabDescriptor& OutTabInfo);

	/** Helper method to get at all the preregistered tab infos */
	/** 获取AllPreregisteredTabInfos。
	 * @return AllPreregisteredTabInfos的当前值或引用。
	 */
	const TArray<FHodgeTabDescriptor>& GetAllPreregisteredTabInfos() { return PreregisteredTabInfoArray; }

	// Toggles whether or not a specified tab is hidden, can only be called before the switcher is associated
	/** 设置TabHidden状态。
	 * @param TabNameId Tab名称标识，由调用方传入。
	 * @param bHidden 是否Hidden的控制标记。
	 */
	UFUNCTION(BlueprintCallable, Category = "Tab List")
	void SetTabHiddenState(FName TabNameId, bool bHidden);

	/** 注册DynamicTab。
	 * @param TabDescriptor Tab描述，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool RegisterDynamicTab(const FHodgeTabDescriptor& TabDescriptor);

	/** 判断FirstTab活动。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool IsFirstTabActive() const;

	/** 判断上次Tab活动。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool IsLastTabActive() const;

	/** 判断Tab可见。
	 * @param TabId Tab标识，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
	bool IsTabVisible(FName TabId);

	/** 获取可见Tab数量。
	 * @return 可见Tab数量的当前值或引用。
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
	int32 GetVisibleTabCount();

	/** Delegate broadcast when a new tab is created. Allows hook ups after creation. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTabContentCreated, FName, TabId, UCommonUserWidget*, TabWidget);

	DECLARE_EVENT_TwoParams(UHodgeTabListWidgetBase, FOnTabContentCreatedNative, FName /* TabId */,
	                        UCommonUserWidget* /* TabWidget */);

	/** Broadcasts when a new tab is created. */
	// 事件Tab内容Created对应的事件通知。
	UPROPERTY(BlueprintAssignable, Category = "Tab List")
	FOnTabContentCreated OnTabContentCreated;
	// 事件Tab内容Created原生对应的事件通知。
	FOnTabContentCreatedNative OnTabContentCreatedNative;

protected:
	// UUserWidget interface
	/** 在控件实例初始化时建立必要的输入或页面绑定。 */
	virtual void NativeOnInitialized() override;
	/** 在控件构建时接入界面数据和生命周期。 */
	virtual void NativeConstruct() override;
	/** 在控件析构时解除界面监听与临时资源。 */
	virtual void NativeDestruct() override;
	// End UUserWidget

	/** 处理PreLinkedSwitcher变化。 */
	virtual void HandlePreLinkedSwitcherChanged() override;
	/** 处理PostLinkedSwitcher变化。 */
	virtual void HandlePostLinkedSwitcherChanged() override;

	/** 处理TabCreation。
	 * @param TabId Tab标识，由调用方传入。
	 * @param TabButton Tab按钮，由调用方传入。
	 */
	virtual void HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton) override;

private:
	/** 设置upTabs。 */
	void SetupTabs();

	// PreregisteredTab信息数组的记录集合。
	UPROPERTY(EditAnywhere, meta=(TitleProperty="TabId"))
	TArray<FHodgeTabDescriptor> PreregisteredTabInfoArray;

	/**
	 * Stores label info for tabs that have been registered at runtime but not yet created. 
	 * Elements are removed once they are created.
	 */
	// 待处理Tab标签信息映射的记录集合。
	UPROPERTY()
	TMap<FName, FHodgeTabDescriptor> PendingTabLabelInfoMap;
};
