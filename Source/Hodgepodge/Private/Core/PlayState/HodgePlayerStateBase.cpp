/**
 * @file HodgePlayerStateBase.cpp
 * @brief AHodgePlayerStateBase 类的实现
 *
 * 实现了玩家状态基类的 GAS 初始化逻辑:
 * - 构造函数:创建 ASC 子对象,设置网络更新频率
 * - GetAbilitySystemComponent:实现 IAbilitySystemInterface 接口
 * - InitializeAbilitySystemForCharacter:GAS 双初始化核心函数
 */

#include "Core/PlayState/HodgePlayerStateBase.h"
#include "AbilitySystem/HodgeAbilitySystemComponentBase.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgePlayerStateBase)

/**
 * @brief 构造函数
 *
 * 创建 AbilitySystemComponent 子对象,
 * 设置 NetUpdateFrequency 为 100Hz 以确保 GAS 属性的高频同步。
 */
AHodgePlayerStateBase::AHodgePlayerStateBase()
{
    AbilitySystemComponent = CreateDefaultSubobject<UHodgeAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
    SetNetUpdateFrequency(100.0f);
}

/**
 * @brief 获取能力系统组件
 * @return AbilitySystemComponent 指针,实现 IAbilitySystemInterface 接口
 */
UAbilitySystemComponent* AHodgePlayerStateBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

/**
 * @brief 为指定角色初始化 GAS 能力系统
 *
 * 执行逻辑:
 * 1. 参数校验:确保 AbilitySystemComponent 和 Avatar 均有效
 * 2. 如果 Avatar 发生变化(角色被替换/重生),调用 InitAbilityActorInfo 重新绑定
 * 3. 在服务器端(HasAuthority),首次调用时创建 AttributeSet:
 *    - 遍历 AttributeSetClasses 配置列表
 *    - 动态创建每个 AttributeSet 实例
 *    - 添加到 ASC 并记录到 AddedAttributeSets
 *    - 设置 bASCInitialized = true 防止重复创建
 *
 * @param Avatar 玩家当前控制的角色
 */
void AHodgePlayerStateBase::InitializeAbilitySystemForCharacter(ACharacter* Avatar)
{
    if (!AbilitySystemComponent || !Avatar)
    {
        return;
    }

    if (AbilitySystemComponent->GetAvatarActor() != Avatar)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, Avatar);
    }

    if (HasAuthority())
    {
        if (!bASCInitialized)
        {
            for (TSubclassOf<UAttributeSet> AttrClass : AttributeSetClasses)
            {
                if (!AttrClass) continue;
                UAttributeSet* NewAS = NewObject<UAttributeSet>(AbilitySystemComponent, AttrClass);
                if (NewAS)
                {
                    AbilitySystemComponent->AddAttributeSetSubobject(NewAS);
                    AddedAttributeSets.Add(NewAS);
                }
            }

            bASCInitialized = true;
        }
    }
}