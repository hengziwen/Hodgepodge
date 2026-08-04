/**
 * @file HodgeCharacterBase.cpp
 * @brief AHodgeCharacterBase 类的实现
 *
 * 实现了角色基类的核心生命周期回调:
 * - PreInitializeComponents:组件初始化前的扩展点
 * - BeginPlay:角色开始播放的扩展点
 * - EndPlay:角色结束播放的扩展点
 */

#include "Character/HodgeCharacterBase.h"

/**
 * @brief 构造函数
 *
 * 将 FObjectInitializer 传递给父类 AAlsCharacter,
 * 由父类负责把默认移动组件替换为 UAlsCharacterMovementComponent。
 */
AHodgeCharacterBase::AHodgeCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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
	Super::BeginPlay();
}