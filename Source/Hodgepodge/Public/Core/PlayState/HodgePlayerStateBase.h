#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HodgePlayerStateBase.generated.h"

/**
 * @brief Hodgepodge 框架的玩家状态基类。
 * @note PlayerState 用于保存与玩家身份和游戏状态相关的数据，这些数据通常需要在服务器和客户端之间同步。
 */
UCLASS()
class HODGEPODGE_API AHodgePlayerStateBase : public APlayerState
{
	GENERATED_BODY()

public:
	//~ Begin AActor interface

	// Actor 组件初始化前调用，可用于初始化 PlayerState 的组件和基础状态。
	virtual void PreInitializeComponents() override;

	// PlayerState 开始游戏时调用，可用于执行初始化逻辑。
	virtual void BeginPlay() override;

	// PlayerState 即将销毁时调用，用于清理运行时资源和解除绑定。
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 重置 PlayerState 的运行时状态，例如玩家重新开始游戏时恢复默认状态。
	virtual void Reset() override;

	//~ End AActor interface

protected:
	//~ Begin APlayerState interface

	// 地图切换或玩家状态迁移时，将当前 PlayerState 的数据复制到新的 PlayerState。
	virtual void CopyProperties(APlayerState* PlayerState);

	//~ End APlayerState interface
};
