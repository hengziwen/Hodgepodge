/** @file HodgeInteractionTestTypes.cpp @brief 用原生目标模拟选项变更与提交拒绝，不复刻组件校验。 */
#include "HodgeInteractionTestTypes.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include <limits>
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeInteractionTestTypes)

AHodgeInteractionTestHero::AHodgeInteractionTestHero(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 保持正式 Hero 默认子对象与所有权链，仅额外提供视点异常注入。
}

FVector AHodgeInteractionTestHero::GetPawnViewLocation() const
{
	// 只使业务视线端点非法，避免测试本身触发 Transform 的非有限数断言。
	return bInvalidViewLocation ? FVector(std::numeric_limits<double>::quiet_NaN(), 0., 0.) : Super::GetPawnViewLocation();
}

AHodgeInteractionTestTarget::AHodgeInteractionTestTarget()
{
	// 仅隔离测试 Actor 复制选项与结果，用于真实双客户端 RPC 验证。
	bReplicates = true;
	// 使用真实查询碰撞，确保候选获取与视线测试经过引擎物理场景。
	InteractionShape = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionShape"));
	SetRootComponent(InteractionShape);
	InteractionShape->InitSphereRadius(20.f);
	InteractionShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionShape->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionShape->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionShape->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Option.RunId = FGuid::NewGuid();
	Option.Revision = 1;
	Option.Text = FText::FromString(TEXT("测试交互"));
}

bool AHodgeInteractionTestTarget::GetInteractionOption(const APawn*, FHodgeInteractionOption& OutOption) const
{
	// 可用性属于目标业务契约，不依赖测试用的距离或玩家身份判断。
	if (!bOptionAvailable) { return false; }
	OutOption = Option;
	return true;
}

FHodgeInteractionResult AHodgeInteractionTestTarget::ExecuteInteraction(APawn* RequestingPawn, const FGuid& ExpectedRunId, int32 ExpectedRevision)
{
	++ExecuteAttempts;
	FHodgeInteractionResult Result;
	// 目标在最终提交点再次维护自己的原子选项身份，避免真实副作用重复发生。
	if (!bOptionAvailable || ExpectedRunId != Option.RunId || ExpectedRevision != Option.Revision || bRejectExecution)
	{
		Result.Reason = TEXT("测试目标拒绝本次提交");
		return Result;
	}
	LastExecutingPawn = RequestingPawn;
	++SuccessfulExecutions;
	++Option.Revision;
	Result.bSucceeded = true;
	return Result;
}

void AHodgeInteractionTestTarget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 多端只观察服务器提交后的选项和计数，测试不依赖本地假结果。
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHodgeInteractionTestTarget, Option);
	DOREPLIFETIME(AHodgeInteractionTestTarget, bOptionAvailable);
	DOREPLIFETIME(AHodgeInteractionTestTarget, SuccessfulExecutions);
}

FHodgeInteractionOption AHodgeInteractionTestTarget::GetOptionSnapshot() const
{
	return Option;
}

int32 AHodgeInteractionTestTarget::GetSuccessfulExecutions() const
{
	return SuccessfulExecutions;
}

int32 AHodgeInteractionTestTarget::GetExecuteAttempts() const
{
	return ExecuteAttempts;
}
