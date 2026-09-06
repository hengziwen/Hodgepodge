/**
 * @file HodgeCharacterBase.cpp
 * @brief AHodgeCharacterBase 类的实现
 *
 * 实现了角色基类的核心生命周期回调:
 * - PreInitializeComponents:组件初始化前的扩展点
 * - BeginPlay:角色开始播放的扩展点
 * - EndPlay:角色结束播放的扩展点
 *
 * 构造函数负责把默认的 CharacterMovement 子组件替换为 UHodgeCharacterMovementComponent。
 */

#include "Character/HodgeCharacterBase.h"

#include "Component/HodgeCharacterMovementComponent.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCharacterBase)

/**
 * @brief 构造函数
 *
 * 通过 FObjectInitializer 把 ACharacter 默认的 CharacterMovement 子组件
 * 替换为 UHodgeCharacterMovementComponent。
 *
 * 移动组件的具体参数(速度、加速度、摩擦、旋转方式等)不再在 C++ 里硬编码,
 * 统一交给蓝图 / 数据资产配置,避免把 ALS 的调参残留到框架里。
 */
AHodgeCharacterBase::AHodgeCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHodgeCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
}

/**
 * @brief Actor组件初始化前的回调(重写自AActor)
 *
 * 预留扩展点,供子类在组件初始化前执行自定义逻辑
 * (如设置组件间的依赖关系、获取组件引用等)。
 */
void AHodgeCharacterBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

/**
 * @brief Actor结束播放时的回调(重写自AActor)
 *
 * 预留扩展点,供子类在角色销毁时执行资源清理
 * (如释放组件、保存状态等)。
 *
 * @param EndPlayReason 结束原因
 */
void AHodgeCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::EndPlay(EndPlayReason);
}

/**
 * @brief Actor开始播放时的回调(重写自AActor)
 *
 * 预留扩展点,供子类在角色生成后执行初始化逻辑
 * (如绑定输入、获取组件引用、播放出生特效等)。
 */
void AHodgeCharacterBase::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}
