/** @file HodgeHavocTests.cpp @brief HDS-01／02／04／06的资源、周期及限流测试。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/AttributeSet/HodgeHavocSet.h"
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"
#include "AbilitySystem/Effects/HodgeHavocEffects.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h"
#include "Combat/HodgeHavocRules.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeAbilitySet.h"
#include "Data/HodgeComboDefinition.h"
#include "Animation/HodgeAnimNotify_DarkSurgeCommit.h"
#include "UI/Data/HodgeGameplayUIDataSource.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include <limits>

/** 测试仅跨过已验证的动画身份入口，事务本身使用正式GAS句柄与周期执行。 */
struct FHodgeHavocTestAccess
{
	static bool Enter(UHodgeGameplayAbility_HavocRules* Rules) { return Rules->BeginSession(FGuid::NewGuid()); }
	static FActiveGameplayEffectHandle State(UHodgeGameplayAbility_HavocRules* Rules) { return Rules->Session.State; }
	static FActiveGameplayEffectHandle Drain(UHodgeGameplayAbility_HavocRules* Rules) { return Rules->Session.Drain; }
};

namespace
{
	/** 隔离真实ASC与GE；不使用编辑器单例或修改共享CDO。 */
	struct FHavocWorld
	{
		UWorld* World;
		UHodgeAbilitySystemComponent* ASC;
		UHodgeHavocSet* Resource;
		UHodgeCombatSet* Combat;
		FHavocWorld()
		{
			const auto Values=UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
			World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
			// 周期集成测试会真实Tick世界，必须注册所属WorldContext以供引擎查询。
			GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
			auto* Owner=World->SpawnActor<AActor>(); ASC=NewObject<UHodgeAbilitySystemComponent>(Owner); ASC->RegisterComponent(); ASC->InitAbilityActorInfo(Owner,Owner);
			Resource=NewObject<UHodgeHavocSet>(Owner); ASC->AddAttributeSetSubobject(Resource);
			Combat=NewObject<UHodgeCombatSet>(Owner); ASC->AddAttributeSetSubobject(Combat);
			// 预热隔离世界TimerManager，后续周期GE进入活动队列而非首帧Pending。
			World->GetTimerManager().Tick(0.f);
		}
		~FHavocWorld() { World->DestroyWorld(false); GEngine->DestroyWorldContext(World); }
		/** 正式GE配置容量，不直接补满燃料。 */
		void Initialize() { ASC->ApplyGameplayEffectToSelf(GetDefault<UHodgeDarkFlowInitialization>(),1.f,ASC->MakeEffectContext()); }
		/** 所有增量走正式GAS聚合及属性约束。 */
		void Delta(float Amount)
		{
			auto Spec=ASC->MakeOutgoingSpec(UHodgeDarkFlowEffect::StaticClass(),1.f,ASC->MakeEffectContext());
			Spec.Data->SetSetByCallerMagnitude(HodgeHavocTags::DarkFlowDelta,Amount); ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHavocResourceTest,"Hodge.Havoc.Resource",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeHavocResourceTest::RunTest(const FString& Parameters)
{
	FHavocWorld F;
	TestFalse(TEXT("未初始化资源不能使用"),F.Resource->IsResourceReady());
	F.Initialize(); TestEqual(TEXT("容量100"),F.Resource->GetMaxDarkFlow(),100.f);
	F.Delta(99.f); TestEqual(TEXT("正常积攒"),F.Resource->GetDarkFlow(),99.f);
	F.Delta(20.f); TestEqual(TEXT("溢出截断不增加额外持续时间"),F.Resource->GetDarkFlow(),100.f);
	F.Delta(-30.f); F.Initialize(); TestEqual(TEXT("重复初始化不恢复燃料"),F.Resource->GetDarkFlow(),70.f);
	F.ASC->SetNumericAttributeBase(UHodgeHavocSet::GetMaxDarkFlowAttribute(),50.f);
	TestEqual(TEXT("降低容量同步压低燃料"),F.Resource->GetDarkFlow(),50.f);
	F.Delta(-100.f); TestEqual(TEXT("消耗下限零"),F.Resource->GetDarkFlow(),0.f);
	F.ASC->SetNumericAttributeBase(UHodgeHavocSet::GetDarkFlowAttribute(),std::numeric_limits<float>::quiet_NaN());
	TestEqual(TEXT("非法属性不能污染资源"),F.Resource->GetDarkFlow(),0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHavocFuelTest,"Hodge.Havoc.FuelClock",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeHavocFuelTest::RunTest(const FString& Parameters)
{
	FHodgeHavocFuelClock Clock;
	TestEqual(TEXT("旧周期没有会话时不扣除"),Clock.Settle(1.,10.f),0.f);
	Clock.Start(0.); TestEqual(TEXT("入态首帧不扣空100"),Clock.Settle(0.,10.f),0.f);
	TestEqual(TEXT("每0.1秒扣1"),Clock.Settle(.1,10.f),1.f);
	TestEqual(TEXT("卡顿按实际0.9秒补结算"),Clock.Settle(1.,10.f),9.f);
	TestEqual(TEXT("同帧补执行不能重复扣"),Clock.Settle(1.,10.f),0.f);
	TestEqual(TEXT("时间回退不产生退款"),Clock.Settle(.5,10.f),0.f);
	TestEqual(TEXT("非法速度不推进时钟"),Clock.Settle(2.,std::numeric_limits<float>::infinity()),0.f);
	TestEqual(TEXT("下一合法周期仍结算完整时间"),Clock.Settle(2.,10.f),10.f);
	Clock.Reset(); TestEqual(TEXT("结束后迟到回调无效"),Clock.Settle(10.,10.f),0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHavocProcTest,"Hodge.Havoc.GlobalProcGate",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeHavocProcTest::RunTest(const FString& Parameters)
{
	FHodgeHavocProcGate Gate;
	TestTrue(TEXT("第一刀可触发"),Gate.TryReserve(0.,1.));
	TestFalse(TEXT("同帧多目标不可重复触发"),Gate.TryReserve(0.,1.));
	TestFalse(TEXT("换段仍共享一秒限制"),Gate.TryReserve(.999,1.));
	TestTrue(TEXT("边界一秒允许"),Gate.TryReserve(1.,1.));
	TestFalse(TEXT("旧时间回调不能重置间隔"),Gate.TryReserve(.5,1.));
	TestFalse(TEXT("非法间隔拒绝"),Gate.TryReserve(2.,0.));
	Gate.Reset(); TestTrue(TEXT("换Pawn后重新建立角色限流"),Gate.TryReserve(0.,1.));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHavocEffectTest,"Hodge.Havoc.StateEffect",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeHavocEffectTest::RunTest(const FString& Parameters)
{
	FHavocWorld F; F.Initialize(); F.Delta(100.f);
	const auto Handle=F.ASC->ApplyGameplayEffectToSelf(GetDefault<UHodgeDarkSurgeEffect>(),1.f,F.ASC->MakeEffectContext());
	TestTrue(TEXT("GE授予真实状态标签"),F.ASC->HasMatchingGameplayTag(HodgeHavocTags::DarkSurge));
	TestEqual(TEXT("入态不会扣100"),F.Resource->GetDarkFlow(),100.f);
	TestEqual(TEXT("持续元素加成20%"),F.Combat->GetHavocDamageBonus(),.20f);
	TestEqual(TEXT("状态GE禁止周期增益"),GetDefault<UHodgeDarkSurgeEffect>()->Period.GetValueAtLevel(1.f),0.f);
	TestTrue(TEXT("消耗GE与状态GE分离"),GetDefault<UHodgeDarkFlowDrainEffect>()->Modifiers.IsEmpty());
	TestEqual(TEXT("非湮灭伤害不享受加成"),FHodgeHavocDamageRules::GetMultiplier(false,.20f),1.f);
	TestEqual(TEXT("湮灭倍率"),FHodgeHavocDamageRules::GetMultiplier(true,.20f),1.20f);
	F.ASC->RemoveActiveGameplayEffect(Handle);
	TestFalse(TEXT("移除后状态撤销"),F.ASC->HasMatchingGameplayTag(HodgeHavocTags::DarkSurge));
	TestEqual(TEXT("移除后加成撤销"),F.Combat->GetHavocDamageBonus(),0.f);
	F.ASC->RemoveActiveGameplayEffect(Handle); TestEqual(TEXT("重复清理不减成负数"),F.Combat->GetHavocDamageBonus(),0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHavocHoldTest,"Hodge.Havoc.HoldInput",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeHavocHoldTest::RunTest(const FString& Parameters)
{
	FHodgeHavocHoldSession Input;
	TestTrue(TEXT("按下建立一次会话"),Input.Begin(1.));
	TestFalse(TEXT("重复回调不推迟阈值"),Input.Begin(1.1));
	TestFalse(TEXT("短按不触发灭音"),Input.IsReady(1.34,.35f));
	TestTrue(TEXT("达到长按阈值"),Input.IsReady(1.36,.35f));
	TestFalse(TEXT("合法窗口可以在有界缓存中等待"),Input.HasExpired(1.5,.35f,.3f));
	TestTrue(TEXT("超时不无限等待后续动作"),Input.HasExpired(1.7,.35f,.3f));
	Input.Release(); TestFalse(TEXT("松开后旧定时器不能再触发"),Input.IsReady(2.,.35f));
	TestTrue(TEXT("下一次按下独立计时"),Input.Begin(2.));
	TestTrue(TEXT("时间回退取消会话"),Input.HasExpired(1.,.35f,.3f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHavocVariantTest,"Hodge.Havoc.VariantSelection",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeHavocVariantTest::RunTest(const FString& Parameters)
{
	FHavocWorld F;
	/** 使用正式蒙太奇副本建立可授予Definition，不修改项目资产。 */
	auto Make=[&](FGameplayTag AbilityTag,bool bEnhanced)
	{
		auto* Data=NewObject<UHodgeAbilityDefinition>(); Data->AbilityTag=AbilityTag;
		Data->AbilityClass=UHodgeGameplayAbility_Definition::StaticClass(); Data->InputTag=HodgeHavocTags::CircuitInput;
		Data->ExecutionRoute=EHodgeAbilityExecutionRoute::Standalone;
		auto* Source=LoadObject<UAnimMontage>(nullptr,TEXT("/Game/Main/Character/Hero/Anim/Montages/AM_Attack01_Montage.AM_Attack01_Montage"));
		Data->ExecutionConfig.Montage=DuplicateObject<UAnimMontage>(Source,Data); Data->ExecutionConfig.Montage->Notifies.Reset();
		(bEnhanced ? Data->RequiredSourceTags : Data->BlockedSourceTags).AddTag(HodgeHavocTags::DarkSurge);
		return Data;
	};
	const auto Normal=F.ASC->GiveAbilityDefinition(Make(HodgeHavocTags::CircuitAbility,false),1,F.ASC->GetOwner());
	const auto Enhanced=F.ASC->GiveAbilityDefinition(Make(HodgeHavocTags::HavocElement,true),1,F.ASC->GetOwner());
	TestTrue(TEXT("常态选择普通分支"),F.ASC->ResolveAbilityVariant(HodgeHavocTags::CircuitInput)==Normal);
	const auto State=F.ASC->ApplyGameplayEffectToSelf(GetDefault<UHodgeDarkSurgeEffect>(),1.f,F.ASC->MakeEffectContext());
	TestTrue(TEXT("状态改变选择增强分支"),F.ASC->ResolveAbilityVariant(HodgeHavocTags::CircuitInput)==Enhanced);
	TestFalse(TEXT("选路不依赖能否立即激活"),F.ASC->FindAbilitySpecFromHandle(Enhanced)->Ability->CanActivateAbility(Enhanced,F.ASC->AbilityActorInfo.Get()));
	TestTrue(TEXT("增强分支不可激活也不回退普通分支"),F.ASC->ResolveAbilityVariant(HodgeHavocTags::CircuitInput)==Enhanced);
	F.ASC->RemoveActiveGameplayEffect(State);
	TestTrue(TEXT("退态恢复普通分支"),F.ASC->ResolveAbilityVariant(HodgeHavocTags::CircuitInput)==Normal);
	const auto Duplicate=F.ASC->GiveAbilityDefinition(Make(HodgeHavocTags::DarkSurge,false),1,F.ASC->GetOwner());
	TestFalse(TEXT("歧义配置不按授予顺序选第一份"),F.ASC->ResolveAbilityVariant(HodgeHavocTags::CircuitInput).IsValid());
	F.ASC->ClearAbility(Duplicate); TestFalse(TEXT("无效输入无候选"),F.ASC->ResolveAbilityVariant({}).IsValid());
	auto* UI=NewObject<UHodgeGameplayUIDataSource>(); UI->Initialize(nullptr,nullptr);
	TestFalse(TEXT("未绑定玩家不伪造暗流Ready"),UI->GetHavocSnapshot().bReady);
	UI->Shutdown(); UI->Shutdown(); TestEqual(TEXT("重复解绑快照归零"),UI->GetHavocSnapshot().Percent,0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHavocSessionTest,"Hodge.Havoc.SessionIntegration",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeHavocSessionTest::RunTest(const FString& Parameters)
{
	FHavocWorld F; F.Initialize();
	const auto Handle=F.ASC->GiveAbility(FGameplayAbilitySpec(UHodgeGameplayAbility_HavocRules::StaticClass(),1));
	TestTrue(TEXT("OnSpawn已自动激活独立规则被动"),F.ASC->FindAbilitySpecFromHandle(Handle)->IsActive());
	auto* Rules=UHodgeGameplayAbility_HavocRules::Find(F.ASC);
	if (!TestNotNull(TEXT("查到唯一规则实例"),Rules)) { return false; }
	F.Delta(99.f); TestFalse(TEXT("99点拒绝入态"),Rules->CanEnterDarkSurge(F.ASC->GetAvatarActor()));
	F.Delta(1.f); TestTrue(TEXT("满100具有入态资格"),Rules->CanEnterDarkSurge(F.ASC->GetAvatarActor()));
	TestFalse(TEXT("伪造或丢失攻击来源不能提交"),Rules->TryEnterDarkSurge(nullptr));
	TestTrue(TEXT("正式状态和消耗事务成功"),FHodgeHavocTestAccess::Enter(Rules));
	const FGuid FirstId=Rules->GetModeSessionId();
	TestEqual(TEXT("入态不一次扣100"),F.Resource->GetDarkFlow(),100.f);
	TestFalse(TEXT("同会话不能重复入态"),FHodgeHavocTestAccess::Enter(Rules));
	// 越过0.1秒计时器边界，避免浮点相等时引擎把执行推迟到下一帧。
	for (int32 Frame=0; Frame<5; ++Frame) { ++GFrameCounter; F.World->Tick(LEVELTICK_All,.11f); }
	const float Elapsed=F.World->GetTimeSeconds();
	AddInfo(FString::Printf(TEXT("Fuel integration: world elapsed=%.6f, actual fuel=%.6f, rate=%.2f"),Elapsed,F.Resource->GetDarkFlow(),Rules->DrainPerSecond));
	TestTrue(TEXT("隔离世界确实推进了游戏时间"),Elapsed>.5f);
	TestTrue(TEXT("真实周期GE按实际世界时间扣燃料"),FMath::IsNearlyEqual(F.Resource->GetDarkFlow(),100.f-Elapsed*Rules->DrainPerSecond,.001f));
	TestEqual(TEXT("周期消耗不叠加增伤"),F.Combat->GetHavocDamageBonus(),.20f);
	Rules->EndDarkSurge(FGuid::NewGuid()); TestTrue(TEXT("旧会话不能清理当前状态"),Rules->GetModeSessionId()==FirstId);
	F.Delta(-200.f);
	TestFalse(TEXT("归零结束会话"),Rules->GetModeSessionId().IsValid());
	TestFalse(TEXT("归零撤销真实状态标签"),F.ASC->HasMatchingGameplayTag(HodgeHavocTags::DarkSurge));
	TestEqual(TEXT("归零撤销元素增伤"),F.Combat->GetHavocDamageBonus(),0.f);
	TestEqual(TEXT("状态和消耗没有残留GE"),F.ASC->GetNumActiveGameplayEffects(),0);
	F.Delta(100.f); TestTrue(TEXT("第二次入态重新建立会话"),FHodgeHavocTestAccess::Enter(Rules));
	Rules->EndDarkSurge(FirstId); TestTrue(TEXT("上次结束回调不能影响新会话"),Rules->GetModeSessionId().IsValid());
	F.ASC->RemoveActiveGameplayEffect(FHodgeHavocTestAccess::Drain(Rules));
	TestFalse(TEXT("外部移除消耗效果时配对退出"),Rules->GetModeSessionId().IsValid());
	TestEqual(TEXT("外部移除不留下永久增伤"),F.Combat->GetHavocDamageBonus(),0.f);
	TestEqual(TEXT("驱散清空暗流"),F.Resource->GetDarkFlow(),0.f);
	F.Delta(100.f);
	// 模拟应用消耗GE期间驱散状态，验证同步重入时不会遗留返回的消耗句柄。
	auto Reentry=F.ASC->OnGameplayEffectAppliedDelegateToSelf.AddLambda([Rules](UAbilitySystemComponent* ASC,const FGameplayEffectSpec& Spec,FActiveGameplayEffectHandle)
	{
		if (Spec.Def->IsA<UHodgeDarkFlowDrainEffect>()) { ASC->RemoveActiveGameplayEffect(FHodgeHavocTestAccess::State(Rules)); }
	});
	TestFalse(TEXT("部分授予遇驱散则事务失败"),FHodgeHavocTestAccess::Enter(Rules));
	F.ASC->OnGameplayEffectAppliedDelegateToSelf.Remove(Reentry);
	TestEqual(TEXT("失败回滚不留下孤立GE"),F.ASC->GetNumActiveGameplayEffects(),0);
	TestEqual(TEXT("失败回滚不留下增伤"),F.Combat->GetHavocDamageBonus(),0.f);
	TestEqual(TEXT("授予中真实驱散同样清空暗流"),F.Resource->GetDarkFlow(),0.f);
	F.Delta(100.f);
	TestTrue(TEXT("配置恢复后新事务正常"),FHodgeHavocTestAccess::Enter(Rules));
	F.ASC->AddLooseGameplayTag(HodgeHitReactionTags::Controlled);
	TestTrue(TEXT("普通受击不移除独立Buff"),Rules->GetModeSessionId().IsValid());
	F.ASC->RemoveLooseGameplayTag(HodgeHitReactionTags::Controlled);
	F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	TestFalse(TEXT("死亡标签立即结束状态"),Rules->GetModeSessionId().IsValid());
	TestEqual(TEXT("死亡不保留增伤"),F.Combat->GetHavocDamageBonus(),0.f);
	TestEqual(TEXT("死亡清空暗流"),F.Resource->GetDarkFlow(),0.f);
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Death);
	F.Delta(60.f); F.ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	TestEqual(TEXT("常态死亡也清空储能"),F.Resource->GetDarkFlow(),0.f);
	F.ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Death);
	F.Delta(100.f);
	TestTrue(TEXT("重新存活后事务可正常建立"),FHodgeHavocTestAccess::Enter(Rules));
	// 保留同一PlayerState所有者，更换Avatar不能继承旧Pawn燃料。
	auto* NewAvatar=F.World->SpawnActor<APawn>();
	F.ASC->InitAbilityActorInfo(F.ASC->GetOwnerActor(),NewAvatar);
	TestEqual(TEXT("更换Avatar清空暗流"),F.Resource->GetDarkFlow(),0.f);
	TestFalse(TEXT("更换Avatar撤销旧会话"),Rules->GetModeSessionId().IsValid());
	F.Delta(100.f); TestTrue(TEXT("新Avatar可建立独立会话"),FHodgeHavocTestAccess::Enter(Rules));
	F.ASC->ClearAbility(Handle);
	TestEqual(TEXT("移除规则能力释放所属GE"),F.ASC->GetNumActiveGameplayEffects(),0);
	TestEqual(TEXT("撤销规则依赖也清空资源"),F.Resource->GetDarkFlow(),0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHavocAssetsTest,"Hodge.Havoc.AssetConfiguration",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeHavocAssetsTest::RunTest(const FString& Parameters)
{
	// 正式资产是交付的一部分：验证授予与连段配置，不能只测试临时内存对象。
	const auto* Set=LoadObject<UHodgeAbilitySet>(nullptr,TEXT("/Game/Main/Character/Hero/Ability/Havoc/AS_Havoc"));
	const auto* Combo=LoadObject<UHodgeComboDefinition>(nullptr,TEXT("/Game/Main/Data/Combo/DA_LightCombo"));
	if (!TestNotNull(TEXT("正式能力集存在"),Set) || !TestNotNull(TEXT("正式连段图存在"),Combo)) { return false; }
	FHavocWorld F; TArray<FText> Errors;
	TestTrue(TEXT("能力集受检预校验通过"),Set->ValidateForGrant(F.ASC,Errors));
	TestTrue(TEXT("单图两套连段校验通过"),Combo->ValidateDefinition(Errors));
	for (const auto& Error : Errors) { AddError(Error.ToString()); }
	TestEqual(TEXT("共鸣回路包仅包含七个新攻击Definition"),Set->GetGrantedDefinitions().Num(),7);
	const auto* Circuit=LoadObject<UHodgeAbilityDefinition>(nullptr,TEXT("/Game/Main/Character/Hero/Ability/Havoc/DA_MieYin"));
	const auto* NormalE=LoadObject<UHodgeAbilityDefinition>(nullptr,TEXT("/Game/Main/Character/Hero/Ability/ResonanceSkill/DA_Resonance_Skill"));
	const auto* EnhancedE=LoadObject<UHodgeAbilityDefinition>(nullptr,TEXT("/Game/Main/Character/Hero/Ability/Havoc/DA_PoMing"));
	if (!Circuit || !NormalE || !EnhancedE) { AddError(TEXT("正式技能定义缺失")); return false; }
	const auto* NormalGA=NormalE->AbilityClass.GetDefaultObject(); const auto* EnhancedGA=EnhancedE->AbilityClass.GetDefaultObject();
	TestTrue(TEXT("两种E复用同一份冷却GE"),NormalGA->GetCooldownGameplayEffect()==EnhancedGA->GetCooldownGameplayEffect());
	float Duration=0.f;
	TestTrue(TEXT("共享冷却为固定8秒"),NormalGA->GetCooldownGameplayEffect() && NormalGA->GetCooldownGameplayEffect()->DurationMagnitude.GetStaticMagnitudeIfPossible(1.f,Duration) && FMath::IsNearlyEqual(Duration,8.f));
	TestEqual(TEXT("常态E成功施放回20点"),NormalE->DarkFlowGainOnCast,20.f);
	TestEqual(TEXT("破命成功施放回20点"),EnhancedE->DarkFlowGainOnCast,20.f);
	TestNull(TEXT("灭音不配置CD"),Circuit->AbilityClass.GetDefaultObject()->GetCooldownGameplayEffect());
	TestTrue(TEXT("灭音保留源动画根运动"),Circuit->ExecutionConfig.Montage->HasRootMotion());
	int32 Commits=0;
	for (const auto& Event : Circuit->ExecutionConfig.Montage->Notifies)
	{
		if (const auto* Commit=Cast<UHodgeAnimNotify_DarkSurgeCommit>(Event.Notify))
		{
			++Commits; TestTrue(TEXT("灭音Box采用固定快照"),Commit->Burst.Shape==EHodgeHitShape::Box && Commit->Burst.TransformPolicy==EHodgeHitTransformPolicy::SnapshotOnEventEnter);
		}
	}
	TestEqual(TEXT("灭音只有一个提交节点"),Commits,1);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeHavocDebugFillTest,"Hodge.Havoc.DebugFill",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeHavocDebugFillTest::RunTest(const FString& Parameters)
{
	// 真实PlayerState原生属性集，避免测试只验证独立ASC而漏掉命令所有权链路。
	FHavocWorld F; F.World->WorldType=EWorldType::EditorPreview;
	auto* State=F.World->SpawnActor<AHodgePlayerState>();
	State->PreInitializeComponents(); State->InitializeComponents(); State->PostInitializeComponents();
	auto* Controller=F.World->SpawnActor<AHodgePlayerController>(); Controller->PlayerState=State;
	auto* ASC=State->GetHodgeAbilitySystemComponent(); const auto* Resource=ASC->GetSet<UHodgeHavocSet>();
	Controller->HodgeFillDarkFlow(); TestEqual(TEXT("未初始化容量时命令不伪造Ready"),Resource->GetMaxDarkFlow(),0.f);
	ASC->ApplyGameplayEffectToSelf(GetDefault<UHodgeDarkFlowInitialization>(),1.f,ASC->MakeEffectContext());
	ASC->SetNumericAttributeBase(UHodgeHavocSet::GetDarkFlowAttribute(),25.f);
	Controller->HodgeFillDarkFlow(); TestEqual(TEXT("当前玩家暗流回满"),Resource->GetDarkFlow(),100.f);
	Controller->HodgeFillDarkFlow(); TestEqual(TEXT("重复回满不超上限"),Resource->GetDarkFlow(),100.f);
	ASC->SetNumericAttributeBase(UHodgeHavocSet::GetMaxDarkFlowAttribute(),150.f);
	Controller->HodgeFillDarkFlow(); TestEqual(TEXT("读取实际容量而非硬编码100"),Resource->GetDarkFlow(),150.f);
	TestFalse(TEXT("回满本身不自动进入暗涌"),ASC->HasMatchingGameplayTag(HodgeHavocTags::DarkSurge));
	ASC->SetNumericAttributeBase(UHodgeHavocSet::GetDarkFlowAttribute(),0.f);
	ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	Controller->HodgeFillDarkFlow(); TestEqual(TEXT("死亡拒绝回满"),Resource->GetDarkFlow(),0.f);
	ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Death);
	Controller->ServerCheat_Implementation(TEXT("HodgeFillDarkFlow; quit"));
	TestEqual(TEXT("非白名单文本拒绝执行"),Resource->GetDarkFlow(),0.f);
	Controller->ServerCheat_Implementation(TEXT("hodgefilldarkflow"));
	TestEqual(TEXT("白名单接受大小写变化"),Resource->GetDarkFlow(),150.f);
	return true;
}
#endif
