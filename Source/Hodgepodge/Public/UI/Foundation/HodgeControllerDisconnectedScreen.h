/** @file HodgeControllerDisconnectedScreen.h
 * @brief 声明设备断连页面相关的类型、接口与现有处理流程。
 */
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"

#include "HodgeControllerDisconnectedScreen.generated.h"

class UHorizontalBox;
class UObject;
class UCommonButtonBase;
struct FPlatformUserSelectionCompleteParams;

/**
 * A screen to display when the user has had all of their controllers disconnected and needs to
 * re-connect them to continue playing the game.
 */
/** 封装设备断连页面的状态与操作接口。 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class UHodgeControllerDisconnectedScreen : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	/** 构造设备断连页面并初始化其默认状态。
	 * @param ObjectInitializer UE构造对象时传入的初始化器。
	 */
	UHodgeControllerDisconnectedScreen(const FObjectInitializer& ObjectInitializer);
	
protected:
	/** 处理页面进入活动状态的逻辑。 */
	virtual void NativeOnActivated() override;

	/** 处理变更用户Clicked。 */
	virtual void HandleChangeUserClicked();

	/**
	 * Called when the user has changed after selecting the prompt to change platform users. 
	 */
	/** 处理变更用户已完成。
	 * @param Params 参数，由调用方传入。
	 */
	virtual void HandleChangeUserCompleted(const FPlatformUserSelectionCompleteParams& Params);

	/**
	 * Returns true if the Change User button should be displayed.
	 * This will check the ICommonUIModule's platform trait tags at runtime.
	 */
	/** 判断是否应显示变更用户按钮。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	virtual bool ShouldDisplayChangeUserButton() const;

	/**
	 * Required platform traits that, when met, will display the "Change User" button
	 * allowing the player to change what signed in user is currently mapped to an input
	 * device.
	 */
	// 平台支持用户变更标签的数据或对象引用。
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer PlatformSupportsUserChangeTags;

	/**
	 * Platforms that have "strict" user pairing requirements may want to allow you to change your user right from
	 * the in-game UI here. These platforms are tagged with "Platform.Trait.Input.HasStrictControllerPairing" in
	 * Common UI.
	 *
	 * This HBox will be set to invisible if the platform you are on does NOT have that platform trait.
	 */
	// H盒体Switch用户的数据或对象引用。
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HBox_SwitchUser;

	/**
	* A button to handle changing the user on platforms with strict user pairing requirements.
	* 
	* @see HBox_SwitchUser
	*/
	// 按钮变更用户的数据或对象引用。
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> Button_ChangeUser;
};