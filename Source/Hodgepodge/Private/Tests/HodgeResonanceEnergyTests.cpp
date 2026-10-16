/** @file HodgeResonanceEnergyTests.cpp @brief 验证充能成本、真实阶段去重和UI无效上下文。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "AbilitySystem/AttributeSet/HodgeResonanceSet.h"
#include "AbilitySystem/Effects/HodgeResonanceEnergyEffect.h"
#include "AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_Melee.h"
#include "UI/Data/HodgeGameplayUIDataSource.h"
#include "Component/HodgeHealthComponent.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Core/PlayState/HodgePlayerState.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Data/HodgeAbilitySet.h"
#include "UObject/UnrealType.h"
#include <limits>

namespace
{
	/** 真实ASC与GE运行在隔离世界中，不使用直接改成员的假成本。 */
	struct FResourceWorld
	{
		UWorld* World;
		UHodgeAbilitySystemComponent* ASC;
		UHodgeResonanceSet* Set;
		FResourceWorld()
		{
			const auto Values=UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false);
			World=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&Values);
			auto* Owner=World->SpawnActor<AActor>(); ASC=NewObject<UHodgeAbilitySystemComponent>(Owner); ASC->RegisterComponent(); ASC->InitAbilityActorInfo(Owner,Owner);
			Set=NewObject<UHodgeResonanceSet>(Owner); ASC->AddAttributeSetSubobject(Set);
		}
		~FResourceWorld() { World->DestroyWorld(false); }
		/** 初始化只配置容量，重复执行不改当前能量。 */
		void Initialize() { ASC->ApplyGameplayEffectToSelf(GetDefault<UHodgeResonanceEnergyInitialization>(),1.f,ASC->MakeEffectContext()); }
		/** 通过正式增量GE修改能量，测试范围约束与成本使用同一属性。 */
		void Delta(float Value)
		{
			auto Spec=ASC->MakeOutgoingSpec(UHodgeResonanceEnergyEffect::StaticClass(),1.f,ASC->MakeEffectContext());
			Spec.Data->SetSetByCallerMagnitude(HodgeResonanceTags::EnergyDelta,Value); ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeResonanceCostTest,"Hodge.Resonance.ResourceAndCost",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeResonanceCostTest::RunTest(const FString& Parameters)
{
	FResourceWorld F; auto* Cost=NewObject<UHodgeAbilityCost_ResonanceEnergy>();
	TestFalse(TEXT("未初始化容量不允许释放"),Cost->CheckCost(nullptr,{},F.ASC->AbilityActorInfo.Get(),nullptr));
	F.Initialize(); TestEqual(TEXT("默认初始化容量100"),F.Set->GetMaxResonanceEnergy(),100.f);
	F.Delta(99.f); TestFalse(TEXT("99不能释放"),Cost->CheckCost(nullptr,{},F.ASC->AbilityActorInfo.Get(),nullptr));
	F.Delta(1.f); TestTrue(TEXT("100允许释放"),Cost->CheckCost(nullptr,{},F.ASC->AbilityActorInfo.Get(),nullptr));
	Cost->ApplyCost(nullptr,{},F.ASC->AbilityActorInfo.Get(),{}); TestEqual(TEXT("正式GE消耗满充"),F.Set->GetResonanceEnergy(),0.f);
	F.Delta(500.f); TestEqual(TEXT("回能截断到上限"),F.Set->GetResonanceEnergy(),100.f);
	F.Delta(-80.f); F.Initialize(); TestEqual(TEXT("重复初始化不恢复已消耗能量"),F.Set->GetResonanceEnergy(),20.f);
	F.ASC->SetNumericAttributeBase(UHodgeResonanceSet::GetMaxResonanceEnergyAttribute(),10.f);
	TestEqual(TEXT("容量降低同时压低能量"),F.Set->GetResonanceEnergy(),10.f);
	Cost->CostGameplayEffectClass=nullptr;
	TestFalse(TEXT("缺成本GE不伪装成功"),Cost->CheckCost(nullptr,{},F.ASC->AbilityActorInfo.Get(),nullptr));
	Cost->CostGameplayEffectClass=UHodgeResonanceEnergyInitialization::StaticClass(); TestFalse(TEXT("容量GE不能误用为成本"),Cost->ValidateEffect());
	TestFalse(TEXT("没有ASC的请求拒绝"),Cost->CheckCost(nullptr,{},nullptr,nullptr));
	F.ASC->SetNumericAttributeBase(UHodgeResonanceSet::GetMaxResonanceEnergyAttribute(),0.f);
	TestFalse(TEXT("零容量不是满充可释放"),F.Set->IsResourceReady());
	F.Initialize(); F.Delta(50.f);
	auto* HealthSet=NewObject<UHodgeHealthSet>(F.ASC->GetOwner()); F.ASC->AddAttributeSetSubobject(HealthSet);
	auto* Health=NewObject<UHodgeHealthComponent>(F.ASC->GetOwner()); Health->RegisterComponent(); Health->InitializeWithAbilitySystem(F.ASC);
	Health->StartDeath(); TestEqual(TEXT("真实服务器死亡入口清空充能"),F.Set->GetResonanceEnergy(),0.f);
	Health->StartDeath(); TestEqual(TEXT("重复死亡入口不产生资源副作用"),F.Set->GetResonanceEnergy(),0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeResonancePhaseTest,"Hodge.Resonance.PhaseLedger",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeResonancePhaseTest::RunTest(const FString& Parameters)
{
	FHodgeAttackPhaseLedger Ledger;
	TestFalse(TEXT("打空不占用阶段"),Ledger.TryReserve(TEXT("Attack"),TEXT("Strike"),0.f));
	TestFalse(TEXT("非法生命伤害不占用阶段"),Ledger.TryReserve(TEXT("Attack"),TEXT("Strike"),std::numeric_limits<float>::quiet_NaN()));
	TestTrue(TEXT("首次真实生命伤害回能"),Ledger.TryReserve(TEXT("Attack"),TEXT("Strike"),10.f));
	TestFalse(TEXT("同阶段多个敌人只回一次"),Ledger.TryReserve(TEXT("Attack"),TEXT("Strike"),20.f));
	TestTrue(TEXT("不同阶段分别回能"),Ledger.TryReserve(TEXT("Attack"),TEXT("Burst"),20.f));
	TestFalse(TEXT("缺阶段身份拒绝"),Ledger.TryReserve(TEXT("Attack"),NAME_None,10.f));
	Ledger.Reset(); TestTrue(TEXT("下一执行重新获得回能机会"),Ledger.TryReserve(TEXT("Attack"),TEXT("Strike"),10.f));
	auto* UI=NewObject<UHodgeGameplayUIDataSource>(); UI->Initialize(nullptr,nullptr);
	TestFalse(TEXT("无本地上下文不发布就绪能量"),UI->GetResonance().bReady);
	UI->Shutdown(); UI->Shutdown(); TestEqual(TEXT("重复解绑后能量显示归零"),UI->GetResonance().Percent,0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeResonancePlayerStateBootstrapTest,"Hodge.Resonance.PlayerStateBeforePawn",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeResonancePlayerStateBootstrapTest::RunTest(const FString& Parameters)
{
	FResourceWorld Fixture;
	// 隔离Experience依赖，显式执行真实组件初始化顺序，不由测试手工绑定ActorInfo。
	Fixture.World->WorldType=EWorldType::EditorPreview;
	auto* State=Fixture.World->SpawnActor<AHodgePlayerState>();
	State->PreInitializeComponents(); State->InitializeComponents(); State->PostInitializeComponents();
	auto* ASC=State->GetHodgeAbilitySystemComponent();
	TestEqual(TEXT("持久Owner在组件初始化时绑定"),ASC->GetOwnerActor(),static_cast<AActor*>(State));
	TestNull(TEXT("此阶段尚未拥有Pawn"),ASC->GetAvatarActor());
	TestTrue(TEXT("没有Avatar仍可授予持久资源"),ASC->IsOwnerActorAuthoritative());
	auto* Set=NewObject<UHodgeAbilitySet>();
	FHodgeAbilitySet_GameplayEffect Init;
	Init.GameplayEffect=UHodgeResonanceEnergyInitialization::StaticClass(); Init.EffectLevel=1.f;
	// 测试通过既有反射配置入口填入受保护资产数组，不扩大运行时接口。
	auto* Effects=FindFProperty<FArrayProperty>(Set->GetClass(),TEXT("GrantedGameplayEffects"));
	FScriptArrayHelper Entries(Effects,Effects->ContainerPtrToValuePtr<void>(Set));
	const int32 Index=Entries.AddValue();
	*reinterpret_cast<FHodgeAbilitySet_GameplayEffect*>(Entries.GetRawPtr(Index))=Init;
	Set->GiveToAbilitySystem(ASC,nullptr);
	const auto* Resource=ASC->GetSet<UHodgeResonanceSet>();
	TestNotNull(TEXT("唯一原生资源集已注册"),Resource);
	if (Resource) { TestEqual(TEXT("实际AbilitySet在Pawn前授予容量100"),Resource->GetMaxResonanceEnergy(),100.f); }
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeResonanceDebugFillTest,"Hodge.Resonance.DebugFill",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FHodgeResonanceDebugFillTest::RunTest(const FString& Parameters)
{
	FResourceWorld Fixture; Fixture.World->WorldType=EWorldType::EditorPreview;
	auto* State=Fixture.World->SpawnActor<AHodgePlayerState>();
	State->PreInitializeComponents(); State->InitializeComponents(); State->PostInitializeComponents();
	auto* Controller=Fixture.World->SpawnActor<AHodgePlayerController>(); Controller->PlayerState=State;
	auto* ASC=State->GetHodgeAbilitySystemComponent(); const auto* Resource=ASC->GetSet<UHodgeResonanceSet>();
	// 未配置容量时调试命令也不能伪造初始化成功。
	Controller->HodgeFillResonance(); TestEqual(TEXT("未初始化资源保持零"),Resource->GetMaxResonanceEnergy(),0.f);
	ASC->ApplyGameplayEffectToSelf(GetDefault<UHodgeResonanceEnergyInitialization>(),1.f,ASC->MakeEffectContext());
	ASC->SetNumericAttributeBase(UHodgeResonanceSet::GetResonanceEnergyAttribute(),25.f);
	Controller->HodgeFillResonance(); TestEqual(TEXT("正式命令将已有能量回满"),Resource->GetResonanceEnergy(),100.f);
	Controller->HodgeFillResonance(); TestEqual(TEXT("重复命令不超过上限"),Resource->GetResonanceEnergy(),100.f);
	ASC->SetNumericAttributeBase(UHodgeResonanceSet::GetMaxResonanceEnergyAttribute(),150.f);
	Controller->HodgeFillResonance(); TestEqual(TEXT("按当前容量回满而非写死100"),Resource->GetResonanceEnergy(),150.f);
	ASC->SetNumericAttributeBase(UHodgeResonanceSet::GetResonanceEnergyAttribute(),0.f);
	ASC->AddLooseGameplayTag(HodgeGameplayTags::Status_Death);
	Controller->HodgeFillResonance(); TestEqual(TEXT("死亡状态不重新充能"),Resource->GetResonanceEnergy(),0.f);
	ASC->RemoveLooseGameplayTag(HodgeGameplayTags::Status_Death);
	Controller->ServerCheat_Implementation(TEXT("HodgeFillResonance; quit"));
	TestEqual(TEXT("非白名单文本不能执行"),Resource->GetResonanceEnergy(),0.f);
	return true;
}
#endif
