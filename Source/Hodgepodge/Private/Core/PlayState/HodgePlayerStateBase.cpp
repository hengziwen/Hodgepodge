#include "Core/PlayState/HodgePlayerStateBase.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Components/PlayerStateComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgePlayerStateBase)

// PlayerState 组件初始化前注册为 GameFrameworkComponentManager 的接收者。
void AHodgePlayerStateBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// 将当前 PlayerState 注册到组件管理器，使其可以接收模块化组件扩展事件。
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

// PlayerState 开始运行时通知模块化组件当前 Actor 已经准备完成。
void AHodgePlayerStateBase::BeginPlay()
{
	// 发送 GameActorReady 事件，通知动态添加到 PlayerState 上的组件可以开始初始化。
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

// PlayerState 销毁前从模块化组件管理器中移除注册。
void AHodgePlayerStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 移除 GameFrameworkComponentManager 对当前 PlayerState 的管理。
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

// 重置 PlayerState 以及挂载在其上的所有 PlayerStateComponent。
void AHodgePlayerStateBase::Reset()
{
	// 先执行 APlayerState 原本的重置逻辑。
	Super::Reset();

	// 获取当前 PlayerState 上挂载的所有 PlayerStateComponent。
	TArray<UPlayerStateComponent*> ModularComponents;
	GetComponents(ModularComponents);

	// 依次通知每个 PlayerStateComponent 重置自身状态。
	for (UPlayerStateComponent* Component : ModularComponents)
	{
		Component->Reset();
	}
}

// 将旧 PlayerState 的数据复制到新的 PlayerState，同时迁移各个 PlayerStateComponent 的数据。
void AHodgePlayerStateBase::CopyProperties(APlayerState* PlayerState)
{
	// 先复制 APlayerState 自身定义的数据。
	Super::CopyProperties(PlayerState);

	// 获取当前旧 PlayerState 上挂载的所有 PlayerStateComponent。
	TInlineComponentArray<UPlayerStateComponent*> PlayerStateComponents;
	GetComponents(PlayerStateComponents);

	// 遍历旧 PlayerState 上的每一个组件。
	for (UPlayerStateComponent* SourcePSComp : PlayerStateComponents)
	{
		// 在目标 PlayerState 上查找同类型、同名称的对应组件。
		if (UPlayerStateComponent* TargetComp = Cast<UPlayerStateComponent>(
			static_cast<UObject*>(FindObjectWithOuter(PlayerState, SourcePSComp->GetClass(),
			                                          SourcePSComp->GetFName()))))
		{
			// 将旧组件中的自定义数据复制到新 PlayerState 对应组件。
			SourcePSComp->CopyProperties(TargetComp);
		}
	}
}
