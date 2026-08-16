// **
//  * @file HodgePlayerControllerBase.h
//  * @brief AHodgePlayerControllerBase 类的头文件
//  *
//  * 玩家控制器基类,作为事件桥梁将引擎生命周期回调
//  * 桥接到 UHodgeLocalPlayerBase 的多播委托系统。
//  */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HodgePlayerControllerBase.generated.h"

/**
 * @brief Hodgepodge 框架的玩家控制器基类
 * 
 * 继承自 APlayerController,核心职责是作为**事件桥梁/适配器**,
 * 将 Unreal 引擎原生的 PlayerController 生命周期回调事件,
 * 桥接到 UHodgeLocalPlayerBase 的多播委托系统。
 * 
 * 设计模式:
 * 采用观察者模式,监听引擎层的关键事件(ReceivedPlayer、SetPawn、
 * OnPossess、OnUnPossess、OnRep_PlayerState),并将这些事件通过
 * UHodgeLocalPlayerBase 的多播委托(OnPlayerControllerSet、
 * OnPlayerStateSet、OnPlayerPawnSet)广播给所有订阅者。
 * 
 * 好处:
 * - 解耦: 各子系统(UI、输入、游戏逻辑等)不需要直接继承和重写
 *   PlayerController 的回调,只需注册委托即可响应事件
 * - 统一: 所有玩家相关的事件都通过 LocalPlayer 的多播委托分发,
 *   形成统一的事件分发中心
 * - 灵活: 支持任意数量的订阅者,便于扩展
 * 
 * 事件流向:
 * 引擎回调 → 重写拦截 → Cast<UHodgeLocalPlayerBase> 验证 → 广播多播委托
 */
UCLASS()
class HODGEPODGE_API AHodgePlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数
	 * 
	 * 当前实现为空,仅调用父类构造函数。
	 * 可在此处配置默认输入模式、设置输入组件等。
	 */
	AHodgePlayerControllerBase();

	/**
	 * @brief 当玩家(LocalPlayer)被分配到此PlayerController时调用(重写自APlayerController)
	 * 
	 * 这是 PlayerController 生命周期中最早被调用的事件之一,
	 * 表示本地玩家已经与此控制器绑定。
	 * 
	 * 事件广播:
	 * - OnPlayerControllerSet: 通知所有订阅者 PlayerController 已就绪
	 * - OnPlayerStateSet: 如果 PlayerState 已存在,同时广播玩家状态事件
	 *   (因为 ReceivedPlayer 时 PlayerState 可能已经被复制过来)
	 * 
	 * 触发时机: 本地玩家首次被分配到此控制器时(通常在游戏开始时)
	 */
	virtual void ReceivedPlayer() override;

	/**
	 * @brief 设置当前控制器管理的Pawn(重写自APlayerController)
	 * 
	 * 当 PlayerController 被分配了一个新的 Pawn 时触发。
	 * 与 OnPossess 不同:SetPawn 是更底层的操作,直接设置 Pawn 指针,
	 * 而 OnPossess 是更高层的"占有"概念(包含输入接管等逻辑)。
	 * 
	 * 事件广播: OnPlayerPawnSet - 通知订阅者 Pawn 已变更
	 * 
	 * @param InPawn 新的 Pawn 指针,可能为 nullptr(表示取消分配)
	 */
	virtual void SetPawn(APawn* InPawn) override;

	/**
	 * @brief 当控制器占有(Possess)一个Pawn时调用(重写自APlayerController)
	 * 
	 * Possess 是 Unreal 中玩家控制角色的核心机制:
	 * 当 PlayerController Possess 一个 Pawn(通常是 ACharacter)时,
	 * 该 Pawn 获得输入焦点,玩家的输入会被路由到它。
	 * 
	 * 事件广播: OnPlayerPawnSet - 通知订阅者 Pawn 已被 Possess
	 * 常见用途:
	 * - UI系统: 绑定角色血量、弹药等数据
	 * - 输入系统: 绑定角色输入
	 * - 摄像机系统: 切换到角色跟随视角
	 * 
	 * @param APawn 被 Possess 的 Pawn 指针
	 */
	virtual void OnPossess(class APawn* APawn) override;

	/**
	 * @brief 当控制器取消占有(Unpossess)当前Pawn时调用(重写自APlayerController)
	 * 
	 * Unpossess 是 Possess 的反向操作,当玩家角色死亡、切换角色、
	 * 或控制器被销毁时触发。此时 Pawn 失去输入焦点。
	 * 
	 * 事件广播: OnPlayerPawnSet(nullptr) - 通知订阅者当前没有活跃的 Pawn
	 * 常见用途:
	 * - UI系统: 清除角色相关的UI显示
	 * - 输入系统: 解绑角色输入
	 * - 摄像机系统: 切换到自由视角或其他相机
	 */
	virtual void OnUnPossess() override;

protected:
	/**
	 * 简介: 当 玩家State 通过网络复制到达客户端时调用(重写自A玩家Controller)
	 * 
	 * 这是一个客户端专用的回调(网络复制回调),
	 * 当服务器端的 玩家State 复制到本地客户端时,引擎自动调用此函数。
	 * 
	 * 事件广播: On玩家StateSet - 通知订阅者 玩家State 已就绪
	 * 这是客户端获取 玩家State 信息(如分数、队伍、玩家名等)的关键时机。
	 * 
	 * 注意: 此函数只在客户端调用,服务器端 玩家State 直接在 Received玩家 中获取。
	 */
	virtual void OnRep_PlayerState() override;
};
