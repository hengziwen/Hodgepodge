/** @file HodgeAbilityTask_WaitMoveCancel.cpp
 * @brief 实现移动取消等待任务相关的类型、接口与现有处理流程。
 */
/**
 * @file HodgeAbilityTask_WaitMoveCancel.cpp
 * @brief UHodgeAbilityTask_WaitMoveCancel 的实现（见头文件里的职责边界）
 */

#include "AbilitySystem/Abilities/HodgeAbilityTask_WaitMoveCancel.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Component/HodgeHeroComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAbilityTask_WaitMoveCancel)

/** 构造能力任务等待移动取消并初始化其默认状态。 */
UHodgeAbilityTask_WaitMoveCancel::UHodgeAbilityTask_WaitMoveCancel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

/** 更新行为树潜在任务，并推进准备或等待阶段。 */
void UHodgeAbilityTask_WaitMoveCancel::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	Evaluate(TEXT("ThresholdCheck"));
}

/** 创建等待玩家移动取消意图的AbilityTask。 */
UHodgeAbilityTask_WaitMoveCancel* UHodgeAbilityTask_WaitMoveCancel::WaitMoveCancel(
	UGameplayAbility* OwningAbility, FGameplayTag CancelWindowTag, float MoveIntentThreshold)
{
	UHodgeAbilityTask_WaitMoveCancel* MyObj = NewAbilityTask<UHodgeAbilityTask_WaitMoveCancel>(OwningAbility);
	MyObj->WindowTag = CancelWindowTag;
	MyObj->IntentThreshold = MoveIntentThreshold;
	return MyObj;
}

/** 开始本对象的执行流程。 */
void UHodgeAbilityTask_WaitMoveCancel::Activate()
{
	// 后续GAS操作使用的能力系统组件。
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();

	// 没有 ASC 就永远判定不了：直接收尾，别留一个活着的空任务。
	if (!Ability || !ASC || !Ability->GetCurrentActorInfo()->IsLocallyControlled()
		|| !WindowTag.IsValid() || !FMath::IsFinite(IntentThreshold) || IntentThreshold < 0.f)
	{
		EndTask();
		return;
	}

	SubscribedASC = ASC;

	// 信号一：取消窗口标签（动画通知 在 Window 进入 / 退出时加减）。
	// 必须保存句柄并在 OnDestroy 解绑：任务比 ASC 短命，不解绑会留下指向已销毁对象的委托。
	if (WindowTag.IsValid())
	{
		WindowTagDelegateHandle = ASC->RegisterGameplayTagEvent(WindowTag, EGameplayTagEventType::NewOrRemoved)
		                             .AddUObject(this, &ThisClass::HandleWindowTagChanged);
	}

	// 信号二：移动意图（输入层）。找不到 HeroComponent（AI、模拟代理）就只订阅标签，
	// 条件于是永不成立 —— 这正是头文件里说的"本地控制端语义"。
	if (UHodgeHeroComponent* HeroComponent =
		    UHodgeHeroComponent::FindHeroComponent(Ability->GetAvatarActorFromActorInfo()))
	{
		SubscribedHeroComponent = HeroComponent;
		HeroComponent->OnMoveIntentChanged.AddDynamic(this, &ThisClass::HandleMoveIntentChanged);
	}

	// 起点也要判一次：任务创建时窗口可能已经开着、玩家可能已经在推摇杆。
	Evaluate(TEXT("Activate"));
}

/** 处理销毁。 */
void UHodgeAbilityTask_WaitMoveCancel::OnDestroy(bool bInOwnerFinished)
{
	// 头等规则：从这里开始，任何仍在调用栈里的 Evaluate 都不许再广播。
	// 监听者通常会在 OnMoveCancel 回调里同步 EndAbility，本任务会在**广播途中**被销毁。
	bInOnDestroy = true;

	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (UAbilitySystemComponent* ASC = SubscribedASC.Get())
	{
		if (WindowTagDelegateHandle.IsValid())
		{
			ASC->UnregisterGameplayTagEvent(WindowTagDelegateHandle, WindowTag, EGameplayTagEventType::NewOrRemoved);
		}
	}
	WindowTagDelegateHandle.Reset();

	// 解除本对象的监听，避免旧生命周期回调继续进入。
	if (UHodgeHeroComponent* HeroComponent = SubscribedHeroComponent.Get())
	{
		HeroComponent->OnMoveIntentChanged.RemoveDynamic(this, &ThisClass::HandleMoveIntentChanged);
	}

	Super::OnDestroy(bInOwnerFinished);
}

/** 计算当前配置结果。 */
void UHodgeAbilityTask_WaitMoveCancel::Evaluate(const TCHAR* Source)
{
	if (bSucceeded || bInOnDestroy)
	{
		return;
	}

	// 后续GAS操作使用的能力系统组件。
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();

	// 条件一：取消窗口开着。没开就什么都不做 —— 攻击前半段推杆不能取消，靠的就是这里。
	if (!ASC || !WindowTag.IsValid() || !ASC->HasMatchingGameplayTag(WindowTag))
	{
		return;
	}

	// 条件二：玩家确实有移动意图。
	const UHodgeHeroComponent* HeroComponent = SubscribedHeroComponent.Get();
	if (!HeroComponent || !HeroComponent->HasMoveIntent(IntentThreshold))
	{
		return;
	}

	// 先立"已完成"标记、再广播：监听者会在回调里结束 Ability，那会同步销毁本任务；
	// 标记必须先立住，否则从回调返回后可能再判一次、再广播一次。
	bSucceeded = true;

	// 与引擎自带的 AbilityTask 一致（见 AbilityTask_WaitGameplayTag）：
	// 只在任务仍允许广播时才广播 —— Ability 正在结束、或任务已不活跃时不广播。
	// 这时"取消"已经没有意义了，而且广播出去只会让监听者再走一遍结束流程。
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[Hodge] WaitMoveCancel 条件成立（%s）"), Source);
		OnMoveCancel.Broadcast();
	}

	// 兜底：监听者没有结束 Ability（例如只是记了一条日志）时，本任务必须自己收尾，
	// 否则它会一直挂在 Ability 上。bInOnDestroy 用来识别"广播期间已经被销毁"的情况。
	if (!bInOnDestroy)
	{
		EndTask();
	}
}

/** 处理移动意图变化。 */
void UHodgeAbilityTask_WaitMoveCancel::HandleMoveIntentChanged(bool /*bHasMoveIntent*/)
{
	// 刻意不看参数：只有变成 true 才可能让条件成立，但无脑重判一次最简单、也最不容易漏。
	Evaluate(TEXT("MoveIntentChanged"));
}

/** 处理窗口标签变化。 */
void UHodgeAbilityTask_WaitMoveCancel::HandleWindowTagChanged(FGameplayTag /*Tag*/, int32 NewCount)
{
	// 只在标签被加上时判定：窗口关闭不会让条件成立。
	if (NewCount > 0)
	{
		Evaluate(TEXT("WindowOpened"));
	}
}
