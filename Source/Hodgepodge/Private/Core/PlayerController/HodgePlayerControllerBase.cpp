// 111屎山代码来袭


#include "Core/PlayerController/HodgePlayerControllerBase.h"

#include "Core/LocalPlayer/HodgeLocalPlayerBase.h"


/**
 * @brief 构造函数
 * 
 * 当前为空实现,仅调用父类构造函数。
 * 可在此处初始化输入组件、设置默认输入模式等。
 */
AHodgePlayerControllerBase::AHodgePlayerControllerBase()
{
}

/**
 * @brief 当玩家被分配到此PlayerController时的回调(重写自APlayerController)
 * 
 * 这是 PlayerController 生命周期中最早的关键事件之一,
 * 表示本地玩家已经成功与此控制器绑定。
 * 
 * 执行逻辑:
 * 1. 先调用 Super::ReceivedPlayer() 执行父类的默认初始化逻辑
 * 2. 将 Player(ULocalPlayer*) 安全转换为 UHodgeLocalPlayerBase*,
 *    验证当前 LocalPlayer 确实是 Hodgepodge 框架的类型
 * 3. 广播 OnPlayerControllerSet 委托,通知所有订阅者 PlayerController 已就绪
 *    - 订阅者可以在此回调中获取 PlayerController 并进行相关初始化
 * 4. 如果 PlayerState 已存在(在服务器端 ReceivedPlayer 时 PlayerState 可能已经创建),
 *    则同时广播 OnPlayerStateSet,使订阅者能立即获取玩家状态信息
 * 
 * 为什么需要同时广播 PlayerState?
 * - 在服务器端,PlayerState 通常在 ReceivedPlayer 之前就已经创建并关联
 * - 在客户端,PlayerState 可能尚未通过网络复制到达,因此需要检查是否存在
 * 
 * 典型订阅者:
 * - 游戏模式管理器: 在 PlayerController 就绪后启动游戏逻辑
 * - UI 系统: 初始化玩家HUD
 */
void AHodgePlayerControllerBase::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (UHodgeLocalPlayerBase* LocalPlayer = Cast<UHodgeLocalPlayerBase>(Player))
	{
		LocalPlayer->OnPlayerControllerSet.Broadcast(LocalPlayer, this);

		if (PlayerState)
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}

/**
 * @brief 设置Pawn时的回调(重写自APlayerController)
 * 
 * 当 PlayerController 被分配了一个新的 Pawn 时触发。
 * 这是比 OnPossess 更底层的操作,直接设置 Pawn 指针,
 * 不涉及输入模式切换等 Possess 级别的逻辑。
 * 
 * 执行逻辑:
 * 1. 先调用 Super::SetPawn() 执行父类的默认赋值逻辑
 * 2. 验证 LocalPlayer 类型安全性
 * 3. 广播 OnPlayerPawnSet 委托,传递新的 Pawn 指针
 * 
 * 注意: SetPawn 可能在 OnPossess 之前或独立于 OnPossess 被调用,
 * 例如在无角色模式下设置一个空 Pawn 指针。
 * 
 * @param InPawn 新的 Pawn 指针,可能为 nullptr
 */
void AHodgePlayerControllerBase::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (UHodgeLocalPlayerBase* LocalPlayer = Cast<UHodgeLocalPlayerBase>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, InPawn);
	}
}

/**
 * @brief Possess一个Pawn时的回调(重写自APlayerController)
 * 
 * Possess 是 Unreal Engine 中玩家控制角色的核心机制:
 * 当 PlayerController Possess 一个 Pawn(通常是 ACharacter)后,
 * 该 Pawn 将获得输入焦点,玩家的输入事件(移动、跳跃、攻击等)
 * 会被引擎自动路由到该 Pawn 的输入组件。
 * 
 * 执行逻辑:
 * 1. 先调用 Super::OnPossess() 执行父类逻辑(如设置输入模式、启用输入等)
 * 2. 验证 LocalPlayer 类型安全性
 * 3. 广播 OnPlayerPawnSet 委托,传递被 Possess 的 Pawn 指针
 * 
 * 典型场景:
 * - 游戏开始时自动 Possess 默认角色
 * - 玩家切换角色时 Possess 新角色
 * - 重生后 Possess 新的角色实例
 * 
 * 常见订阅者用途:
 * - UI系统: 绑定角色的血量、耐力、弹药等数据到HUD显示
 * - 输入系统: 为角色设置特定的输入模式(如第三人称、射击模式)
 * - 摄像机系统: 切换到角色的跟随摄像机或第一人称视角
 * - 音效系统: 播放角色 Possess 的音效
 * 
 * @param APawn 被 Possess 的 Pawn 指针(通常是 ACharacter 子类)
 */
void AHodgePlayerControllerBase::OnPossess(class APawn* APawn)
{
	Super::OnPossess(APawn);
	if (UHodgeLocalPlayerBase* LocalPlayer = Cast<UHodgeLocalPlayerBase>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, APawn);
	}
}

/**
 * @brief Unpossess当前Pawn时的回调(重写自APlayerController)
 * 
 * Unpossess 是 Possess 的反向操作,断开 PlayerController 与 Pawn 的连接。
 * 触发时机:
 * - 玩家角色死亡(需要等待重生)
 * - 玩家切换到另一个角色
 * - PlayerController 被销毁
 * - 游戏结束
 * 
 * 执行逻辑:
 * 1. 先调用 Super::OnUnPossess() 执行父类逻辑(如清除输入模式、禁用输入等)
 * 2. 验证 LocalPlayer 类型安全性
 * 3. 广播 OnPlayerPawnSet(nullptr),传递 nullptr 表示当前没有活跃的 Pawn
 *    订阅者应据此清理与 Pawn 相关的所有资源和引用
 * 
 * 常见订阅者用途:
 * - UI系统: 清除角色HUD,显示死亡界面
 * - 输入系统: 切换到非角色输入模式(如UI模式)
 * - 摄像机系统: 切换到自由视角或死亡摄像机
 * - 音效系统: 停止角色相关的循环音效
 */
void AHodgePlayerControllerBase::OnUnPossess()
{
	Super::OnUnPossess();
	if (UHodgeLocalPlayerBase* LocalPlayer = Cast<UHodgeLocalPlayerBase>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, nullptr);
	}
}

/**
 * @brief PlayerState网络复制到客户端时的回调(重写自APlayerController)
 * 
 * 这是一个**客户端专用**的回调函数,属于 UE 的属性复制机制。
 * 当服务器端创建或更新 PlayerState 后,引擎会通过网络将其复制到客户端,
 * 复制完成时自动调用此函数。
 * 
 * 执行逻辑:
 * 1. 先调用 Super::OnRep_PlayerState() 执行父类的默认逻辑
 * 2. 检查 PlayerState 是否有效
 * 3. 验证 LocalPlayer 类型安全性
 * 4. 广播 OnPlayerStateSet 委托,通知订阅者 PlayerState 已就绪
 * 
 * 为什么需要这个回调?
 * - 在客户端,PlayerState 不是直接创建的,而是通过网络复制过来的
 * - 客户端需要一个时机来确认 PlayerState 已经可用
 * - OnRep_PlayerState 就是这个"确认点",标志着 PlayerState 的数据
 *   (如玩家名、分数、队伍、生命值等)已经可以使用
 * 
 * 与 ReceivedPlayer 中广播 PlayerState 的区别:
 * - ReceivedPlayer: 服务器端直接获取 PlayerState,在游戏开始时触发一次
 * - OnRep_PlayerState: 客户端通过网络复制获取,可能在游戏进行中多次触发
 *   (例如 PlayerState 被重新创建或复制更新时)
 * 
 * 典型订阅者用途:
 * - 名称显示: 在HUD上显示玩家名
 * - 队伍系统: 根据 PlayerState 的队伍信息设置阵营
 * - 计分系统: 显示玩家分数
 */
void AHodgePlayerControllerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (PlayerState)
	{
		if (UHodgeLocalPlayerBase* LocalPlayer = Cast<UHodgeLocalPlayerBase>(Player))
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}