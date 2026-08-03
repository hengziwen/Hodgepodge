/**
 * @file HodgeGameModeBase.cpp
 * @brief AHodgeGameModeBase 类的实现
 *
 * 实现了游戏模式基类的默认配置:
 * - 构造函数:设置默认的游戏框架类(控制器、角色、状态等)
 * - InitGameState/InitGame:预留初始化扩展点
 * - PlayerCanRestart_Implementation:控制玩家重启逻辑
 */

#include "Core/GameMode/HodgeGameModeBase.h"

#include "Character/HodgeCharacterBase.h"
#include "Core/GameState/HodgeGameStateBase.h"
#include "Core/HUD/HodgeHUDBase.h"
#include "Core/PlayerController/HodgePlayerControllerBase.h"
#include "Core/PlayState/HodgePlayerStateBase.h"

/**
 * @brief 构造函数
 *
 * 设置默认的游戏框架类,这些类会在游戏开始时被自动实例化:
 * - PlayerControllerClass → AHodgePlayerControllerBase:玩家控制器
 * - DefaultPawnClass → AHodgeCharacterBase:默认角色(后续可改为 AHodgeHeroCharacter)
 * - GameStateClass → AHodgeGameStateBase:游戏状态
 * - PlayerStateClass → AHodgePlayerStateBase:玩家状态(持有 ASC)
 * - HUDClass → AHodgeHUDBase:HUD 界面
 */
AHodgeGameModeBase::AHodgeGameModeBase()
{
	PlayerControllerClass = AHodgePlayerControllerBase::StaticClass();
	DefaultPawnClass = AHodgeCharacterBase::StaticClass();
	GameStateClass = AHodgeGameStateBase::StaticClass();
	PlayerStateClass = AHodgePlayerStateBase::StaticClass();
	HUDClass = AHodgeHUDBase::StaticClass();
}

/**
 * @brief 初始化游戏状态(重写自 AGameModeBase)
 *
 * 预留扩展点,供子类在 GameState 创建后设置初始值。
 */
void AHodgeGameModeBase::InitGameState()
{
	Super::InitGameState();
}

/**
 * @brief 初始化游戏(重写自 AGameModeBase)
 *
 * 预留扩展点,供子类在游戏开始时执行自定义初始化
 * (如读取游戏参数、加载配置等)。
 */
void AHodgeGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

/**
 * @brief 判断玩家是否可以重启(重写自 AGameModeBase)
 *
 * 实现 PlayerCanRestart 的逻辑,委托给 ControllerCanRestart。
 *
 * @param Player 玩家控制器指针
 * @return true 表示允许重启
 */
bool AHodgeGameModeBase::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

/**
 * @brief 判断控制器是否可以重启
 *
 * 默认返回 true,允许玩家重启。
 * 子类可重写实现特定的重启条件(如玩家是否在安全区域等)。
 *
 * @param Controller 控制器指针
 * @return true 表示允许重启
 */
bool AHodgeGameModeBase::ControllerCanRestart(AController* Controller)
{
	return true;
}