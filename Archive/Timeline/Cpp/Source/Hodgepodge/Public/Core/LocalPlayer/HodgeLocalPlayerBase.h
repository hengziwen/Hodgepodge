// **
//  * @file HodgeLocalPlayerBase.h
//  * @brief UHodgeLocalPlayerBase 类的头文件
//  *
//  * 本地玩家基类,提供 PlayerController/PlayerState/Pawn 的
//  * 多播事件委托,以及玩家视图的启用/禁用控制。
//  */

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "HodgeLocalPlayerBase.generated.h"

/**
 * @brief Hodgepodge 框架的本地玩家基类
 * 
 * 继承自 ULocalPlayer,扩展了玩家生命周期相关的事件委托和视图控制功能。
 * 主要用途:
 * 1. 提供 PlayerController / PlayerState / Pawn 被分配时的多播事件通知机制,
 *    使其他子系统可以在玩家对象就绪时做出响应(如初始化UI、绑定输入等)。
 * 2. 通过 bIsPlayerViewEnabled 开关控制玩家视口的投影数据生成,
 *    用于在特定场景下(如暂停、菜单界面)禁用玩家视图渲染。
 * 
 * 使用方式:
 * - 通过 CallAndRegister_OnPlayerControllerSet() 等方法注册回调,
 *   如果对应对象已经存在会立即触发一次回调,保证调用方不会错过已发生的事件。
 * - 子类可继承此类并扩展更多功能。
 */
UCLASS()
class HODGEPODGE_API UHodgeLocalPlayerBase : public ULocalPlayer
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数
	 *
	 * 当前为空实现,仅调用父类构造函数。
	 * bIsPlayerViewEnabled 默认初始化为 true。
	 */
	UHodgeLocalPlayerBase();


	/**
	 * @brief 当本地玩家被分配到 PlayerController 时触发的多播委托
	 * 
	 * 参数:
	 * - LocalPlayer: 触发事件的本地玩家对象(this)
	 * - PlayerController: 新分配的 PlayerController 指针
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UHodgeLocalPlayerBase* LocalPlayer,
	                                     APlayerController* PlayerController);
	FPlayerControllerSetDelegate OnPlayerControllerSet;

	/**
	 * @brief 当本地玩家被分配到 PlayerState 时触发的多播委托
	 * 
	 * 参数:
	 * - LocalPlayer: 触发事件的本地玩家对象(this)
	 * - PlayerState: 新分配的 PlayerState 指针
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UHodgeLocalPlayerBase* LocalPlayer,
	                                     APlayerState* PlayerState);
	FPlayerStateSetDelegate OnPlayerStateSet;

	/**
	 * @brief 当本地玩家被分配到 Pawn 时触发的多播委托
	 * 
	 * 参数:
	 * - LocalPlayer: 触发事件的本地玩家对象(this)
	 * - Pawn: 新分配的 Pawn 指针(可能是 ACharacter 或其他 Pawn 子类)
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerPawnSetDelegate, UHodgeLocalPlayerBase* LocalPlayer, APawn* Pawn);
	FPlayerPawnSetDelegate OnPlayerPawnSet;

	/**
	 * @brief 注册并立即触发 PlayerController 设置事件
	 * 
	 * 如果 PlayerController 已经存在,会先执行一次 Delegate 回调,
	 * 然后将 Delegate 添加到 OnPlayerControllerSet 多播委托中,
	 * 以便在未来 PlayerController 变化时再次触发。
	 * 
	 * @param Delegate 要注册的回调函数
	 * @return FDelegateHandle 用于后续通过 Remove() 取消注册
	 */
	FDelegateHandle CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate);

	/**
	 * @brief 注册并立即触发 PlayerState 设置事件
	 * 
	 * 如果 PlayerController 及其 PlayerState 已经存在,会先执行一次 Delegate 回调,
	 * 然后将 Delegate 添加到 OnPlayerStateSet 多播委托中。
	 * 
	 * @param Delegate 要注册的回调函数
	 * @return FDelegateHandle 用于后续通过 Remove() 取消注册
	 */
	FDelegateHandle CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate);

	/**
	 * @brief 注册并立即触发 Pawn 设置事件
	 * 
	 * 如果 PlayerController 及其 Pawn 已经存在,会先执行一次 Delegate 回调,
	 * 然后将 Delegate 添加到 OnPlayerPawnSet 多播委托中。
	 * 
	 * @param Delegate 要注册的回调函数
	 * @return FDelegateHandle 用于后续通过 Remove() 取消注册
	 */
	FDelegateHandle CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate);

public:
	/**
	 * @brief 获取视口的投影数据(重写基类方法)
	 * 
	 * 当 bIsPlayerViewEnabled 为 false 时返回 false,阻止生成投影数据,
	 * 从而实现禁用玩家视图的效果。这在暂停菜单、全屏UI界面等场景下非常有用。
	 * 当 bIsPlayerViewEnabled 为 true 时,调用基类的默认实现。
	 * 
	 * @param Viewport 目标视口
	 * @param ProjectionData 输出的投影数据
	 * @param StereoViewIndex 立体视图索引(用于VR/立体渲染)
	 * @return true 表示成功获取投影数据,false 表示视图已禁用
	 */
	virtual bool GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData,
	                               int32 StereoViewIndex) const override;

	/**
	 * @brief 查询玩家视图是否启用
	 * @return true 表示视图启用,false 表示视图已禁用
	 */
	bool IsPlayerViewEnabled() const { return bIsPlayerViewEnabled; }

	/**
	 * @brief 设置玩家视图启用状态
	 * 
	 * 设置为 false 后,GetProjectionData 将返回 false,
	 * 引擎不会为该玩家生成视口投影,画面将不会渲染该玩家的视点。
	 * 
	 * @param bInIsPlayerViewEnabled 是否启用玩家视图
	 */
	void SetIsPlayerViewEnabled(bool bInIsPlayerViewEnabled) { bIsPlayerViewEnabled = bInIsPlayerViewEnabled; }

	// 获取根UI布局(预留接口,待实现)
	//UPrimaryGameLayout* GetRootUILayout() const;

private:
	/**
	 * 简介: 玩家视图是否启用,默认为 true
	 * 
	 * 当设为 false 时,GetProjectionData() 将跳过投影数据生成,
	 * 常用于全屏UI、暂停菜单等需要覆盖整个屏幕的场景。
	 */
	bool bIsPlayerViewEnabled = true;
};