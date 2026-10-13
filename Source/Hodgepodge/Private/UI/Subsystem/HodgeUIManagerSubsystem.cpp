/** @file HodgeUIManagerSubsystem.cpp
 * @brief 实现本地玩家UI管理服务相关的类型、接口与现有处理流程。
 */
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include "UI/Foundation/HodgePrimaryGameLayout.h"
#include "Core/LocalPlayer/HodgeLocalPlayerBase.h"
#include "Core/GameInstance/HodgeGameInstanceBase.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "Component/HodgeHeroComponent.h"
#include "CommonInputSubsystem.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "UI/Data/HodgeGameplayUIDataSource.h"
#include "Kismet/GameplayStatics.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeUIManagerSubsystem)

/** 判断是否应创建Subsystem。 */
bool UHodgeUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    const auto* Instance = Cast<UHodgeGameInstanceBase>(Outer);
    return Instance && !Instance->IsDedicatedServerInstance() && Super::ShouldCreateSubsystem(Outer);
}

/** 初始化ialize。 */
void UHodgeUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    for (ULocalPlayer* Player : GetGameInstance()->GetLocalPlayers()) { PlayerAdded(Player); }
}

/** 为加入的LocalPlayer建立所需的本地管理状态。 */
void UHodgeUIManagerSubsystem::PlayerAdded(ULocalPlayer* Player)
{
    auto* Local = Cast<UHodgeLocalPlayerBase>(Player);
    if (!Local || Players.Contains(Player)) { return; }
    Players.Add(Player);
    if (auto* Router = Player->GetSubsystem<UCommonUIActionRouterBase>())
    {
        Players[Player].InputModeDelegate = Router->OnActiveInputModeChanged().AddWeakLambda(this,
            [this, Player](ECommonInputMode Mode) { if (Mode == ECommonInputMode::Menu) { ClearGameplayInput(Player); } });
    }
    // RegisterAndCall 可同步创建布局，回调期间不持有 Map 值引用。
    const auto Handle = Local->CallAndRegister_OnPlayerControllerSet(
        UHodgeLocalPlayerBase::FPlayerControllerSetDelegate::FDelegate::CreateWeakLambda(this,
            [this](UHodgeLocalPlayerBase* LP, APlayerController* PC) { ControllerChanged(LP, PC); }));
    if (auto* Data = Players.Find(Player)) { Data->ControllerDelegate = Handle; }
}

/** 在控制器切换时重新绑定当前依赖。 */
void UHodgeUIManagerSubsystem::ControllerChanged(ULocalPlayer* Player, APlayerController* Controller)
{
    auto* Data = Players.Find(Player);
    if (!Data || (Data->Controller == Controller && (Data->Root || Data->RootLoad))) { return; }
    DestroyRoot(Player);
    Data = Players.Find(Player);
    if (!Data) { return; }
    Data->Controller = Controller;
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (!Controller || Controller->GetLocalPlayer() != Player) { return; }
    Data->GameplayData = NewObject<UHodgeGameplayUIDataSource>(this);
    Data->GameplayData->Initialize(Cast<UHodgeLocalPlayerBase>(Player), Controller);
    if (RootLayoutClass.IsNull() || RootLayoutClass.Get()) { CreateRoot(Player, Controller); return; }
    TWeakObjectPtr<ULocalPlayer> WeakPlayer = Player;
    TWeakObjectPtr<APlayerController> WeakController = Controller;
    auto Load = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(RootLayoutClass.ToSoftObjectPath(),
        FStreamableDelegate::CreateWeakLambda(this, [this, WeakPlayer, WeakController]()
        { if (WeakPlayer.IsValid()) { CreateRoot(WeakPlayer.Get(), WeakController); } }));
    if (auto* Current = Players.Find(Player); Current && !Current->Root) { Current->RootLoad = Load; }
}

/** 创建根节点。 */
void UHodgeUIManagerSubsystem::CreateRoot(ULocalPlayer* Player, TWeakObjectPtr<APlayerController> Controller)
{
    auto* Data = Players.Find(Player);
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (!Data || !Controller.IsValid() || Data->Controller != Controller || Data->Root) { return; }
    UClass* Class = RootLayoutClass.IsNull() ? UHodgePrimaryGameLayout::StaticClass() : RootLayoutClass.Get();
    if (!Class) { UE_LOG(LogTemp, Error, TEXT("[Hodge UI] Root load failed: %s"), *RootLayoutClass.ToString()); Data->RootLoad.Reset(); return; }
    auto* Root = CreateWidget<UHodgePrimaryGameLayout>(Controller.Get(), Class);
    Data->Root = Root;
    // 向当前监听者发布状态变化，调用方继续负责后续处理。
    if (Root && Root->AddToPlayerScreen(1000)) { OnRootLayoutChanged.Broadcast(Player, Root); }
    if (auto* Current = Players.Find(Player)) { Current->RootLoad.Reset(); }
}

/** 销毁根节点。 */
void UHodgeUIManagerSubsystem::DestroyRoot(ULocalPlayer* Player)
{
    auto* Data = Players.Find(Player);
    if (!Data) { return; }
    auto Root = Data->Root;
    auto GameplayData = Data->GameplayData;
    Data->GameplayData = nullptr;
    auto Load = MoveTemp(Data->RootLoad);
    Data->Root = nullptr;
    Data->Controller.Reset();
    if (Load) { Load->CancelHandle(); }
    // 向当前监听者发布状态变化，调用方继续负责后续处理。
    OnRootLayoutChanged.Broadcast(Player, nullptr);
    if (Root) { Root->ReleaseLayout(); Root->RemoveFromParent(); }
    if (GameplayData) { GameplayData->Shutdown(); }
    if (auto* Current = Players.Find(Player))
    {
        const auto Tokens = Current->InputTokens.Array();
        for (FName Token : Tokens) { ResumeInput(Player, Token); }
    }
}

/** 清理被移除LocalPlayer持有的本地管理状态。 */
void UHodgeUIManagerSubsystem::PlayerRemoved(ULocalPlayer* Player)
{
    const auto* Data = Players.Find(Player);
    if (!Data) { return; }
    if (auto* Local = Cast<UHodgeLocalPlayerBase>(Player)) { Local->OnPlayerControllerSet.Remove(Data->ControllerDelegate); }
    if (auto* Router = Player->GetSubsystem<UCommonUIActionRouterBase>()) { Router->OnActiveInputModeChanged().Remove(Data->InputModeDelegate); }
    DestroyRoot(Player);
    Players.Remove(Player);
}

/** 在子系统退出时清理其登记内容与监听。 */
void UHodgeUIManagerSubsystem::Deinitialize()
{
    TArray<TObjectPtr<ULocalPlayer>> Keys;
    Players.GetKeys(Keys);
    for (ULocalPlayer* Player : Keys) { PlayerRemoved(Player); }
    OnRootLayoutChanged.Clear();
    Super::Deinitialize();
}

/** 获取根节点布局。 */
UHodgePrimaryGameLayout* UHodgeUIManagerSubsystem::GetRootLayout(ULocalPlayer* Player) const
{
    const auto* Data = Players.Find(Player);
    return Data ? Data->Root.Get() : nullptr;
}

/** 判断游戏输入允许。 */
bool UHodgeUIManagerSubsystem::IsGameInputAllowed(ULocalPlayer* Player) const
{
    const auto* Data = Players.Find(Player);
    if (Data && (!Data->InputTokens.IsEmpty() || (Data->Root && Data->Root->HasBlockingPage()))) { return false; }
    const auto* Router = Player ? Player->GetSubsystem<UCommonUIActionRouterBase>() : nullptr;
    return !Router || Router->GetActiveInputMode() != ECommonInputMode::Menu;
}

/** 检查当前LocalPlayer是否允许接收玩法输入。 */
bool UHodgeUIManagerSubsystem::AllowsGameplayInput(const APlayerController* Controller)
{
    const auto* Player = Controller ? Controller->GetLocalPlayer() : nullptr;
    const auto* Instance = Player ? Player->GetGameInstance() : nullptr;
    const auto* Manager = Instance ? Instance->GetSubsystem<UHodgeUIManagerSubsystem>() : nullptr;
    return !Manager || Manager->IsGameInputAllowed(const_cast<ULocalPlayer*>(Player));
}

/** 清空玩法输入。 */
void UHodgeUIManagerSubsystem::ClearGameplayInput(ULocalPlayer* Player)
{
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (auto* PC = Player ? Cast<AHodgePlayerController>(Player->GetPlayerController(GetWorld())) : nullptr)
    {
        if (auto* ASC = PC->GetHodgeAbilitySystemComponent()) { ASC->ReleaseAbilityInput(); }
        if (auto* Pawn = PC->GetPawn())
        { if (auto* Hero = Pawn->FindComponentByClass<UHodgeHeroComponent>()) { Hero->ResetGameplayInput(); } }
    }
}

/** 刷新输入状态。 */
void UHodgeUIManagerSubsystem::RefreshInputState(ULocalPlayer* Player)
{
    if (!IsGameInputAllowed(Player)) { ClearGameplayInput(Player); }
}

/** 按来源令牌挂起本地玩家的输入。 */
void UHodgeUIManagerSubsystem::SuspendInput(ULocalPlayer* Player, FName Token)
{
    auto* Data = Players.Find(Player);
    if (!Data || Token.IsNone() || Data->InputTokens.Contains(Token)) { return; }
    Data->InputTokens.Add(Token);
    ClearGameplayInput(Player);
    if (auto* Input = Player->GetSubsystem<UCommonInputSubsystem>())
    {
        for (ECommonInputType Type : {ECommonInputType::MouseAndKeyboard, ECommonInputType::Gamepad, ECommonInputType::Touch})
        { Input->SetInputTypeFilter(Type, Token, true); }
    }
}

/** 释放对应来源的输入挂起令牌。 */
void UHodgeUIManagerSubsystem::ResumeInput(ULocalPlayer* Player, FName Token)
{
    auto* Data = Players.Find(Player);
    if (!Data || Token.IsNone() || !Data->InputTokens.Remove(Token)) { return; }
    if (auto* Input = Player->GetSubsystem<UCommonInputSubsystem>())
    {
        for (ECommonInputType Type : {ECommonInputType::MouseAndKeyboard, ECommonInputType::Gamepad, ECommonInputType::Touch})
        { Input->SetInputTypeFilter(Type, Token, false); }
    }
}

/** 获取界面管理器。 */
UHodgeUIManagerSubsystem* UHodgeUIManagerSubsystem::GetUIManager(const UObject* WorldContextObject)
{
    auto* Instance = WorldContextObject ? UGameplayStatics::GetGameInstance(WorldContextObject) : nullptr;
    return Instance ? Instance->GetSubsystem<UHodgeUIManagerSubsystem>() : nullptr;
}
/** 获取根节点布局关联控制器。 */
UHodgePrimaryGameLayout* UHodgeUIManagerSubsystem::GetRootLayoutForController(APlayerController* Controller)
{
    auto* Player = Controller ? Controller->GetLocalPlayer() : nullptr;
    auto* Instance = Player ? Player->GetGameInstance() : nullptr;
    auto* UI = Instance ? Instance->GetSubsystem<UHodgeUIManagerSubsystem>() : nullptr;
    return UI ? UI->GetRootLayout(Player) : nullptr;
}
/** 获取玩法数据关联控制器。 */
UHodgeGameplayUIDataSource* UHodgeUIManagerSubsystem::GetGameplayDataForController(APlayerController* Controller)
{
    auto* Player = Controller ? Controller->GetLocalPlayer() : nullptr;
    auto* Instance = Player ? Player->GetGameInstance() : nullptr;
    auto* UI = Instance ? Instance->GetSubsystem<UHodgeUIManagerSubsystem>() : nullptr;
    const auto* Data = UI ? UI->Players.Find(Player) : nullptr;
    return Data && Data->Controller == Controller ? Data->GameplayData.Get() : nullptr;
}
