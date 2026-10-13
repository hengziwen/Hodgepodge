/** @file HodgeUIAuthoringLibrary.cpp
 * @brief 实现UMG编辑器制作与验证函数库相关的类型、接口与现有处理流程。
 */
#include "HodgeUIAuthoringLibrary.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "UI/IndicatorSystem/IndicatorLayer.h"
#include "UI/Targeting/HodgeLockOnIndicatorWidget.h"
#include "UI/IndicatorSystem/HodgeIndicatorManagerComponent.h"
#include "UI/IndicatorSystem/IndicatorDescriptor.h"
#include "Editor.h"
#include "UI/Foundation/HodgePrimaryGameLayout.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"

#include "UI/Extension/UIExtensionPointWidget.h"
#include "Data/HodgeExperienceActionSet.h"
#include "Data/HodgeExperienceDefinition.h"
#include "GameFeatures/GameFeatureAction_AddWidget.h"
#include "Component/HodgeExperienceManagerComponent.h"
#include "Component/HodgeHealthComponent.h"
#include "Component/HodgeHeroComponent.h"
#include "Component/HodgePawnExtensionComponent.h"
#include "Data/HodgePawnData.h"
#include "Camera/HodgeCameraMode.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "AbilitySystem/HodgeAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h"
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"
#include "Core/PlayerController/HodgePlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Factories/BlueprintFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/CompilerResultsLog.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "CommonInputBaseTypes.h"
#include "CommonUITypes.h"
#include "CommonGameViewportClient.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameModeBase.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/FileHelper.h"
#include "ImageUtils.h"
#include "GameFramework/PlayerState.h"
#include "GameFeaturesSubsystem.h"
#include "GameplayEffect.h"
#include "TimerManager.h"
#include "UObject/SavePackage.h"
#include "UObject/UnrealType.h"
#include "Misc/PackageName.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "UI/Data/HodgeGameplayUIDataSource.h"
#include "UI/HodgeHUDLayout.h"
#include "UI/HodgeActivatableWidget.h"
#include "CommonUserWidget.h"
#include "CommonButtonBase.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "K2Node_Self.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_BreakStruct.h"
#include "K2Node_FormatText.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_RemoveDelegate.h"
#include "K2Node_CallDelegate.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_DynamicCast.h"
#include "K2Node_ExecutionSequence.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/DynamicEntryBox.h"
#include "UObject/MetaData.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeUIAuthoringLibrary)

namespace
{
    /** 将当前报告对象序列化为JSON字符串。
     * @param Data 当前操作使用的数据。
     * @return 本次生成的文字或诊断信息。
     */
    FString Json(const TSharedRef<FJsonObject>& Data)
    { FString Result; FJsonSerializer::Serialize(Data, TJsonWriterFactory<>::Create(&Result)); return Result; }
    /** 保存当前资产。
     * @param Asset 资产，由调用方传入。
     * @return true表示本次检查或操作成功，false表示失败。
     */
    bool Save(UObject* Asset)
    {
        FSavePackageArgs Args; Args.TopLevelFlags = RF_Public | RF_Standalone; Args.SaveFlags = SAVE_NoError;
        const FString Filename = FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(), FPackageName::GetAssetPackageExtension());
        return UPackage::SavePackage(Asset->GetOutermost(), Asset, *Filename, Args);
    }
    /** 在指定资产路径创建或取得蓝图。
     * @param Path 路径，由调用方传入。
     * @param Parent 父项，由调用方传入。
     * @param Widget 控件，由调用方传入。
     * @return 本次计算、查询或创建得到的Blueprint。
     */
    UBlueprint* Blueprint(const FString& Path, UClass* Parent, bool Widget)
    {
        // 先检查对象与上下文，避免在依赖失效后继续处理。
        if (auto* Existing = LoadObject<UBlueprint>(nullptr, *Path))
        { return Existing->ParentClass == Parent ? Existing : nullptr; }
        const FString Name = FPackageName::GetShortName(Path);
        UPackage* Package = CreatePackage(*Path);
        UFactory* Factory = Widget ? static_cast<UFactory*>(NewObject<UWidgetBlueprintFactory>()) : static_cast<UFactory*>(NewObject<UBlueprintFactory>());
        if (auto* WF = Cast<UWidgetBlueprintFactory>(Factory)) { WF->ParentClass = Parent; }
        if (auto* BF = Cast<UBlueprintFactory>(Factory)) { BF->ParentClass = Parent; }
        auto* BP = Cast<UBlueprint>(Factory->FactoryCreateNew(Widget ? UWidgetBlueprint::StaticClass() : UBlueprint::StaticClass(), Package,
            *Name, RF_Public | RF_Standalone | RF_Transactional, nullptr, GWarn));
        if (!BP) { return nullptr; }
        FAssetRegistryModule::AssetCreated(BP);
        FCompilerResultsLog Log; FKismetEditorUtilities::CompileBlueprint(BP, EBlueprintCompileOptions::None, &Log);
        if (Log.NumErrors || !BP->GeneratedClass || !Save(BP)) { return nullptr; }
        return BP;
    }
    /** 在测试调度入口执行指定的界面动作。
     * @param World 本次操作所在的世界。
     * @return 本次计算、查询或创建得到的游戏功能扩展动作。
     */
    UGameFeatureAction* UIAction(UWorld* World)
    {
        auto* Manager = World && World->GetGameState() ? World->GetGameState()->FindComponentByClass<UHodgeExperienceManagerComponent>() : nullptr;
        // 先检查对象与上下文，避免在依赖失效后继续处理。
        if (!Manager || !Manager->IsExperienceLoaded()) { return nullptr; }
        for (const UHodgeExperienceActionSet* Set : Manager->GetCurrentExperienceChecked()->ActionSets)
        { for (UGameFeatureAction* Action : Set->Actions) { if (Action && Action->IsA<UGameFeatureAction_AddWidgets>()) { return Action; } } }
        return nullptr;
    }
}

/** 创建FoundationAssets。 */
FString UHodgeUIAuthoringLibrary::CreateFoundationAssets()
{
    auto Report = MakeShared<FJsonObject>(); Report->SetBoolField(TEXT("success"), false);
    if (!GEditor || GEditor->PlayWorld)
    { Report->SetStringField(TEXT("error"), TEXT("Stop PIE before authoring UI assets")); return Json(Report); }
    const FString Base = TEXT("/Game/Main/UI/");
    auto* RootBP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("Foundation/WBP_HodgePrimaryLayout")));
    auto* HUDBP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("HUD/WBP_HodgeHUD")));
    auto* VitalsBP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("HUD/WBP_PlayerVitals")));
    auto* AbilitiesBP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("HUD/WBP_AbilityBar")));
    auto* MenuBP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("Menu/WBP_HodgePauseMenu")));
    auto* DialogBP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("Menu/WBP_HodgeConfirmation")));
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (!RootBP || !HUDBP || !VitalsBP || !AbilitiesBP || !MenuBP || !DialogBP)
    { Report->SetStringField(TEXT("error"), TEXT("MigrateDesignerAssets must create the formal WBP first")); return Json(Report); }

    const FString TablePath = Base + TEXT("Foundation/DT_UIInputActions");
    auto* Table = LoadObject<UDataTable>(nullptr, *TablePath);
    if (!Table)
    {
        Table = NewObject<UDataTable>(CreatePackage(*TablePath), *FPackageName::GetShortName(TablePath), RF_Public | RF_Standalone);
        Table->RowStruct = FCommonInputActionDataBase::StaticStruct();
        for (const auto& Name : {FName(TEXT("Back")), FName(TEXT("Confirm"))})
        {
            FCommonInputActionDataBase Row; Row.DisplayName = FText::FromName(Name);
            auto* KeyInfo = FindFProperty<FStructProperty>(Row.StaticStruct(), TEXT("KeyboardInputTypeInfo"))->ContainerPtrToValuePtr<FCommonInputTypeInfo>(&Row);
            KeyInfo->SetKey(Name == TEXT("Back") ? EKeys::Escape : EKeys::Enter);
            auto* PadInfo = FindFProperty<FStructProperty>(Row.StaticStruct(), TEXT("DefaultGamepadInputTypeInfo"))->ContainerPtrToValuePtr<FCommonInputTypeInfo>(&Row);
            PadInfo->SetKey(Name == TEXT("Back") ? EKeys::Gamepad_FaceButton_Right : EKeys::Gamepad_FaceButton_Bottom);
            Table->AddRow(Name, Row);
        }
        FAssetRegistryModule::AssetCreated(Table);
        if (!Save(Table)) { return Json(Report); }
    }
    auto* InputBP = Blueprint(Base + TEXT("Foundation/B_HodgeUIInputData"), UCommonUIInputData::StaticClass(), false);
    if (!InputBP) { return Json(Report); }
    auto* Input = InputBP->GeneratedClass->GetDefaultObject<UCommonUIInputData>();
    Input->DefaultBackAction.DataTable = Table; Input->DefaultBackAction.RowName = TEXT("Back");
    Input->DefaultClickAction.DataTable = Table; Input->DefaultClickAction.RowName = TEXT("Confirm");
    InputBP->MarkPackageDirty(); if (!Save(InputBP)) { return Json(Report); }

    const FString SetPath = Base + TEXT("EAS_HodgeGameplayUI");
    auto* Set = LoadObject<UHodgeExperienceActionSet>(nullptr, *SetPath);
    if (!Set)
    {
        Set = NewObject<UHodgeExperienceActionSet>(CreatePackage(*SetPath), *FPackageName::GetShortName(SetPath), RF_Public | RF_Standalone);
        auto* Action = NewObject<UGameFeatureAction_AddWidgets>(Set, NAME_None, RF_Transactional);
        FHodgeHUDLayoutRequest Layout; Layout.LayerID = HodgeGameplayTags::UI_Layer_Game; Layout.LayoutClass = HUDBP->GeneratedClass; Action->Layout.Add(Layout);
        FHodgeHUDElementEntry Vitals; Vitals.SlotID = FGameplayTag::RequestGameplayTag(TEXT("UI.Slot.PlayerVitals")); Vitals.WidgetClass = VitalsBP->GeneratedClass; Action->Widgets.Add(Vitals);
        FHodgeHUDElementEntry Abilities; Abilities.SlotID = FGameplayTag::RequestGameplayTag(TEXT("UI.Slot.AbilityBar")); Abilities.WidgetClass = AbilitiesBP->GeneratedClass; Action->Widgets.Add(Abilities);
        Set->Actions.Add(Action); Set->UpdateAssetBundleData();
        FAssetRegistryModule::AssetCreated(Set); if (!Save(Set)) { return Json(Report); }
    }
    auto* ExpBP = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Main/Experiences/Exp_HodgeDefaultExperience.Exp_HodgeDefaultExperience"));
    auto* Exp = ExpBP && ExpBP->GeneratedClass ? ExpBP->GeneratedClass->GetDefaultObject<UHodgeExperienceDefinition>() : nullptr;
    if (!Exp) { return Json(Report); }
    ExpBP->Modify(); Exp->Modify(); Exp->ActionSets.AddUnique(Set);
    FKismetEditorUtilities::CompileBlueprint(ExpBP);
    Exp = ExpBP->GeneratedClass->GetDefaultObject<UHodgeExperienceDefinition>();
    Exp->ActionSets.AddUnique(Set);
    Exp->UpdateAssetBundleData(); ExpBP->MarkPackageDirty();
    if (!Save(ExpBP)) { return Json(Report); }
    Report->SetBoolField(TEXT("success"), true); Report->SetNumberField(TEXT("assets"), 11);
    return Json(Report);
}

/** 检查玩家界面。 */
FString UHodgeUIAuthoringLibrary::InspectPlayerUI(APlayerController* Player)
{
    auto Data = MakeShared<FJsonObject>();
    auto* LP = Player ? Player->GetLocalPlayer() : nullptr;
    auto* Instance = LP ? LP->GetGameInstance() : nullptr;
    auto* Manager = Instance ? Instance->GetSubsystem<UHodgeUIManagerSubsystem>() : nullptr;
    auto* Root = Manager ? Manager->GetRootLayout(LP) : nullptr;
    Data->SetBoolField(TEXT("root"), Root != nullptr);
    Data->SetStringField(TEXT("pawn"), Player && Player->GetPawn() ? Player->GetPawn()->GetPathName() : TEXT(""));
    Data->SetNumberField(TEXT("managed_players"), Manager ? Manager->GetPlayerLayoutCount() : 0);
    Data->SetNumberField(TEXT("platform_user"), LP ? LP->GetPlatformUserId().GetInternalId() : -1);
    Data->SetNumberField(TEXT("input_device"), LP ? IPlatformInputDeviceMapper::Get().GetPrimaryInputDeviceForUser(LP->GetPlatformUserId()).GetId() : -1);
    Data->SetBoolField(TEXT("own_viewport"), LP && Player->GetWorld()->GetGameViewport() && Player->GetWorld()->GetGameViewport()->GetGameInstance() == LP->GetGameInstance());
    Data->SetBoolField(TEXT("game_input"), Manager ? Manager->IsGameInputAllowed(LP) : true);
    Data->SetNumberField(TEXT("pending"), Root ? Root->GetPendingRequestCount() : 0);
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (!Root) { return Json(Data); }
    for (const auto& Pair : {TPair<FString,FGameplayTag>(TEXT("game"), HodgeGameplayTags::UI_Layer_Game),
        TPair<FString,FGameplayTag>(TEXT("menu"), HodgeGameplayTags::UI_Layer_Menu),
        TPair<FString,FGameplayTag>(TEXT("modal"), HodgeGameplayTags::UI_Layer_Modal)})
    {
        auto* Container = Root->GetLayer(Pair.Value);
        Data->SetNumberField(Pair.Key + TEXT("_count"), Container ? Container->GetWidgetList().Num() : 0);
        Data->SetBoolField(Pair.Key + TEXT("_active"), Container && Container->GetActiveWidget() && Container->GetActiveWidget()->IsActivated());
    }
    auto* Game = Root->GetLayer(HodgeGameplayTags::UI_Layer_Game);
    auto* HUD = Game ? Game->GetActiveWidget() : nullptr;
    if (HUD && HUD->WidgetTree)
    {
        const FGeometry& RootGeometry = Root->GetCachedGeometry();
        const FVector2D RootSize = RootGeometry.GetLocalSize();
        if (RootSize.X > 0 && RootSize.Y > 0)
        {
            auto RecordPosition = [&](UWidget* Widget, const FString& Prefix)
            {
                if (!Widget) { return; }
                const FGeometry& Geometry = Widget->GetCachedGeometry();
                const FVector2D Center = RootGeometry.AbsoluteToLocal(Geometry.LocalToAbsolute(Geometry.GetLocalSize() * .5f));
                Data->SetNumberField(Prefix + TEXT("_x"), Center.X / RootSize.X);
                Data->SetNumberField(Prefix + TEXT("_y"), Center.Y / RootSize.Y);
            };
            RecordPosition(HUD->WidgetTree->FindWidget(TEXT("PlayerVitalsPoint")), TEXT("vitals_center"));
            RecordPosition(HUD->WidgetTree->FindWidget(TEXT("EndButton")), TEXT("end_center"));
        }
    }
    int32 Entries = 0;
    if (HUD && HUD->WidgetTree)
    {
        HUD->WidgetTree->ForEachWidget([&](UWidget* Widget)
        {
            if (auto* Point = Cast<UUIExtensionPointWidget>(Widget))
            {
                Entries += Point->GetNumEntries();
                for (auto* Entry : Point->GetAllEntries())
                {
                    if (Entry->GetClass()->GetFName() == TEXT("WBP_PlayerVitals_C") && Entry->WidgetTree)
                    {
                        if (auto* Bar = Cast<UProgressBar>(Entry->WidgetTree->FindWidget(TEXT("HealthBar")))) { Data->SetNumberField(TEXT("display_percent"),Bar->GetPercent()); }
                        if (auto* Text = Cast<UTextBlock>(Entry->WidgetTree->FindWidget(TEXT("HealthText")))) { Data->SetStringField(TEXT("display_text"),Text->GetText().ToString()); }
                        if (auto* Text = Cast<UTextBlock>(Entry->WidgetTree->FindWidget(TEXT("LevelText")))) { Data->SetStringField(TEXT("display_level"),Text->GetText().ToString()); }
                    }
                    if (Entry->GetClass()->GetFName() == TEXT("WBP_AbilityBar_C") && Entry->WidgetTree)
                    {
                        TArray<TSharedPtr<FJsonValue>> Available;
                        if (auto* Box = Cast<UDynamicEntryBox>(Entry->WidgetTree->FindWidget(TEXT("AbilityEntries"))))
                        {
                            for (auto* Slot : Box->GetAllEntries())
                            {
                                auto* Button = Slot && Slot->WidgetTree ? Slot->WidgetTree->FindWidget(TEXT("AbilityButton")) : nullptr;
                                Available.Add(MakeShared<FJsonValueBoolean>(Button && Button->GetIsEnabled()));
                            }
                        }
                        Data->SetArrayField(TEXT("abilities"),Available);
                    }
                }
            }
        });
    }
    if (auto* Source = UHodgeUIManagerSubsystem::GetGameplayDataForController(Player))
    {
        const auto Vitals = Source->GetVitals();
        Data->SetNumberField(TEXT("health"),Vitals.Health); Data->SetNumberField(TEXT("max_health"),Vitals.MaxHealth);
        Data->SetBoolField(TEXT("vitals_ready"),Vitals.bReady);
    }
    Data->SetNumberField(TEXT("entries"), Entries);
    if (auto* IndicatorManager = Player->FindComponentByClass<UHodgeIndicatorManagerComponent>())
    {
        Data->SetNumberField(TEXT("indicators"),IndicatorManager->GetIndicators().Num());
        TArray<TSharedPtr<FJsonValue>> Widgets;
        for (auto* Descriptor : IndicatorManager->GetIndicators())
        {
            auto Entry = MakeShared<FJsonObject>(); Entry->SetStringField(TEXT("class"),Descriptor->GetIndicatorClass().ToString());
            auto* Widget = Descriptor->IndicatorWidget.Get(); Entry->SetBoolField(TEXT("ready"),Widget!=nullptr);
            if (Widget) { Entry->SetStringField(TEXT("world"),GetPathNameSafe(Widget->GetWorld())); }
            Widgets.Add(MakeShared<FJsonValueObject>(Entry));
        }
        Data->SetArrayField(TEXT("indicator_widgets"),Widgets);
    }
    if (Player->GetPawn())
    {
        if (auto* Hero = Player->GetPawn()->FindComponentByClass<UHodgeHeroComponent>())
        { Data->SetBoolField(TEXT("move_intent"), Hero->HasMoveIntent()); }
    }
    return Json(Data);
}

/** 把验证用界面或属性操作排入后续游戏Tick。 */
bool UHodgeUIAuthoringLibrary::QueueUIAction(APlayerController* Player, FName Action, float Value)
{
    if (!Player || !Player->GetWorld() || Player->GetWorld()->WorldType != EWorldType::PIE) { return false; }
    TWeakObjectPtr<APlayerController> Weak = Player;
    // 向当前监听者发布状态变化，调用方继续负责后续处理。
    Player->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([Weak, Action, Value]()
    {
        auto* PC = Weak.Get(); if (!PC) { return; }
        auto* LP = PC->GetLocalPlayer();
        auto* Instance = LP ? LP->GetGameInstance() : nullptr;
    auto* Manager = Instance ? Instance->GetSubsystem<UHodgeUIManagerSubsystem>() : nullptr;
        auto* Root = Manager ? Manager->GetRootLayout(LP) : nullptr;
        // 从按键入口验证 IMC，而非绕过映射直接注入 InputAction。
        if (Action == TEXT("LockOnKey"))
        {
            if (PC->IsLocalController()) { PC->InputKey(FInputKeyParams(EKeys::MiddleMouseButton,Value > .5f ? IE_Pressed : IE_Released,Value > .5f ? 1. : 0.)); }
            return;
        }
        if (Action == TEXT("CameraOverride"))
        {
            // 此入口只操作 PIE 本地 Hero，复用实际 GA 使用的模式所有权接口。
            if (!PC->IsLocalController() || !PC->GetPawn() || !PC->GetWorld()->IsPlayInEditor()) { return; }
            auto* Hero = PC->GetPawn()->FindComponentByClass<UHodgeHeroComponent>();
            const auto* Extension = UHodgePawnExtensionComponent::FindPawnExtensionComponent(PC->GetPawn());
            const auto* Data = Extension ? Extension->GetPawnData<UHodgePawnData>() : nullptr;
            auto* ASC = Extension ? Extension->GetHodgeAbilitySystemComponent() : nullptr;
            const auto* Spec = ASC ? ASC->FindAbilitySpecFromClass(UHodgeGameplayAbility_LockOn::StaticClass()) : nullptr;
            if (Hero && Data && Data->DefaultCameraMode && Spec)
            {
                // 用已授予的命令 Spec 标识验证请求，不在 Editor 模块生成不可导出的 GAS 句柄。
                if (Value > .5f) { Hero->SetAbilityCameraMode(Data->DefaultCameraMode,Spec->Handle); }
                else { Hero->ClearAbilityCameraMode(Spec->Handle); }
            }
            return;
        }
        if (Action == TEXT("Damage"))
        {
            auto* HC = Cast<AHodgePlayerController>(PC);
            auto* ASC = HC ? HC->GetHodgeAbilitySystemComponent() : nullptr;
            if (ASC && PC->HasAuthority())
            {
                auto* GE = NewObject<UGameplayEffect>(); GE->DurationPolicy = EGameplayEffectDurationType::Instant;
                auto& Mod = GE->Modifiers.AddDefaulted_GetRef(); Mod.Attribute = UHodgeHealthSet::GetDamageAttribute();
                Mod.ModifierOp = EGameplayModOp::Additive; Mod.ModifierMagnitude = FScalableFloat(Value);
                ASC->ApplyGameplayEffectToSelf(GE, 1, ASC->MakeEffectContext());
            }
            return;
        }
        if (Action == TEXT("Respawn"))
        {
            if (auto* Mode = PC->GetWorld()->GetAuthGameMode())
            {
                if (auto* Pawn = PC->GetPawn()) { PC->UnPossess(); Pawn->Destroy(); }
                Mode->RestartPlayer(PC);
            }
            return;
        }
        if (Action == TEXT("Capture") && Root && FSlateApplication::IsInitialized())
        {
            TArray<FColor> Pixels; FIntVector Size;
            if (FSlateApplication::Get().TakeScreenshot(Root->TakeWidget(), Pixels, Size))
            {
                TArray64<uint8> PNG; FImageUtils::PNGCompressImageArray(Size.X, Size.Y, Pixels, PNG);
                FFileHelper::SaveArrayToFile(PNG, *(FPaths::ProjectSavedDir() / TEXT("UIFoundation/UI-Slate.png")));
            }
            return;
        }
        if (Action == TEXT("Level"))
        {
            if (APlayerState* PS = PC->PlayerState; PS && PC->HasAuthority())
            { if (auto* Function = PS->FindFunction(TEXT("SetCharacterLevel")))
              { struct { int32 Level; bool ReturnValue; } Params{int32(Value), false}; PS->ProcessEvent(Function, &Params); } }
            return;
        }
        if (Action == TEXT("Deactivate") || Action == TEXT("Activate"))
        {
            if (auto* Feature = UIAction(PC->GetWorld()))
            {
                auto* Context = GEngine->GetWorldContextFromWorld(PC->GetWorld());
                if (Action == TEXT("Activate"))
                { FGameFeatureActivatingContext C; C.SetRequiredWorldContextHandle(Context->ContextHandle); Feature->OnGameFeatureActivating(C); }
                else
                { FGameFeatureDeactivatingContext C(TEXTVIEW("UIValidation"), [](FStringView){}); C.SetRequiredWorldContextHandle(Context->ContextHandle); Feature->OnGameFeatureDeactivating(C); }
            }
            return;
        }
        if (!Root) { return; }
        if (Action == TEXT("RebuildRoot")) { Manager->PlayerRemoved(LP); Manager->PlayerAdded(LP); return; }
        if (Action == TEXT("Escape") || Action == TEXT("RoutedEscape"))
        {
            // 回归期间允许 Esc 进入游戏视口，不触发编辑器的停止 Play 快捷键。
            TGuardValue<bool> InputWorld(GIsPlayInEditorWorld, false);
            auto* Viewport = PC->GetWorld()->GetGameViewport();
            if (Viewport)
            {
                const auto Device = IPlatformInputDeviceMapper::Get().GetPrimaryInputDeviceForUser(LP->GetPlatformUserId());
                if (Action == TEXT("RoutedEscape"))
                {
                    // 仅本次原生回归绕过编辑器的 StopPlaySession 快捷键，不改用户键位或运行时路由。
                    if (auto* Router = LP->GetSubsystem<UCommonUIActionRouterBase>())
                    {
                        const auto Result = Router->ProcessInput(EKeys::Escape, IE_Pressed);
                        Router->ProcessInput(EKeys::Escape, IE_Released);
                        UE_LOG(LogTemp, Display, TEXT("[Hodge UI Validation] Routed Escape LP=%s Result=%d Mode=%d"),
                            *LP->GetName(), int32(Result), int32(Router->GetActiveInputMode()));
                    }
                }
                else
                {
                    Viewport->InputKey(FInputKeyEventArgs(Viewport->Viewport, Device, EKeys::Escape, IE_Pressed));
                    Viewport->InputKey(FInputKeyEventArgs(Viewport->Viewport, Device, EKeys::Escape, IE_Released));
                }
            }
        }
        else if (Action == TEXT("EndButton"))
        {
            auto* HUD = Root->GetLayer(HodgeGameplayTags::UI_Layer_Game)->GetActiveWidget();
            auto* Button = HUD && HUD->WidgetTree ? Cast<UButton>(HUD->WidgetTree->FindWidget(TEXT("EndButton"))) : nullptr;
            if (Button) { Button->OnClicked.Broadcast(); }
        }
        else if (Action == TEXT("MenuReturn"))
        {
            auto* Menu = Root->GetLayer(HodgeGameplayTags::UI_Layer_Menu)->GetActiveWidget();
            auto* Button = Menu && Menu->WidgetTree ? Menu->WidgetTree->FindWidget(TEXT("ResumeButton")) : nullptr;
            if (Button) { if (auto* Function = Button->FindFunction(TEXT("HandleButtonClicked"))) { Button->ProcessEvent(Function, nullptr); } }
        }
        else if (Action == TEXT("Menu"))
        { Root->PushAsync(HodgeGameplayTags::UI_Layer_Menu, TSoftClassPtr<UCommonActivatableWidget>(FSoftObjectPath(TEXT("/Game/Main/UI/Menu/WBP_HodgePauseMenu.WBP_HodgePauseMenu_C"))), [](UCommonActivatableWidget*){}); }
        else if (Action == TEXT("Modal"))
        { Root->Push(HodgeGameplayTags::UI_Layer_Modal, LoadClass<UCommonActivatableWidget>(nullptr, TEXT("/Game/Main/UI/Menu/WBP_HodgeConfirmation.WBP_HodgeConfirmation_C"))); }
        else if (Action == TEXT("ConfirmMenu") || Action == TEXT("AcceptDialog") || Action == TEXT("CancelDialog"))
        {
            const bool bMenu = Action == TEXT("ConfirmMenu");
            auto* Page = Root->GetLayer(bMenu ? HodgeGameplayTags::UI_Layer_Menu : HodgeGameplayTags::UI_Layer_Modal)->GetActiveWidget();
            auto* Button = Page && Page->WidgetTree ? Page->WidgetTree->FindWidget(bMenu ? TEXT("ConfirmButton") : (Action == TEXT("AcceptDialog") ? TEXT("AcceptButton") : TEXT("CancelButton"))) : nullptr;
            if (Button) { if (auto* Function = Button->FindFunction(TEXT("HandleButtonClicked"))) { Button->ProcessEvent(Function,nullptr); } }
        }
        else if (Action == TEXT("Close"))
        {
            const auto Modal = Root->GetLayer(HodgeGameplayTags::UI_Layer_Modal);
            const auto Menu = Root->GetLayer(HodgeGameplayTags::UI_Layer_Menu);
            Root->Pop(Modal->GetActiveWidget() ? Modal->GetActiveWidget() : Menu->GetActiveWidget());
        }
        else if (Action == TEXT("CancelAsync") || Action == TEXT("FailAsync"))
        {
            FGuid Id = Root->PushAsync(HodgeGameplayTags::UI_Layer_Menu,
                TSoftClassPtr<UCommonActivatableWidget>(FSoftObjectPath(Action == TEXT("FailAsync") ? TEXT("/Game/CodexText/UIValidation/Missing.Missing_C")
                    : TEXT("/Game/Main/UI/Menu/WBP_HodgePauseMenu.WBP_HodgePauseMenu_C"))), [](UCommonActivatableWidget*){});
            if (Action == TEXT("CancelAsync")) { Root->CancelPush(Id); }
        }
    }));
    return true;
}

namespace
{
    /** 保存Designer图的数据和接口契约。 */
    struct FDesignerGraph
    {
        // BP的数据或对象引用。
        UWidgetBlueprint* BP;
        // 图的数据或对象引用。
        UEdGraph* Graph;
        // Schema的数据或对象引用。
        const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
        // 收集配置或执行失败的诊断。
        TArray<FString>& Errors;
        // X的数值或阈值。
        int32 X = 0;
        // Y的数值或阈值。
        int32 Y = 0;
        /** 构造Designer图并初始化其默认状态。
         * @param InBP BP，由调用方传入。
         * @param InErrors 收集配置或执行失败的诊断。
         */
        FDesignerGraph(UWidgetBlueprint* InBP, TArray<FString>& InErrors) : BP(InBP), Errors(InErrors)
        {
            Graph = BP->UbergraphPages.Num() ? BP->UbergraphPages[0].Get() : FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("EventGraph"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
            if (!BP->UbergraphPages.Contains(Graph)) { FBlueprintEditorUtils::AddUbergraphPage(BP, Graph); }
        }
        /** 创建当前图中的指定节点。
         * @param Allocate Allocate，由调用方传入。
         * @return 本次计算、查询或创建得到的T。
         */
        template<class T> T* Node(bool Allocate = true)
        {
            auto* N = NewObject<T>(Graph); Graph->AddNode(N, false, false); N->CreateNewGuid();
            N->NodePosX = X; N->NodePosY = Y; X += 240;
            if (Allocate) { N->AllocateDefaultPins(); }
            return N;
        }
        /** 构造当前图节点需要的引脚类型描述。
         * @param Category Category，由调用方传入。
         * @param Object 对象，由调用方传入。
         * @return 本次计算、查询或创建得到的Ed图Pin类型。
         */
        static FEdGraphPinType Type(FName Category, UObject* Object = nullptr)
        { FEdGraphPinType T; T.PinCategory = Category; T.PinSubCategoryObject = Object; return T; }
        /** 查找目标节点的指定引脚。
         * @param N N，由调用方传入。
         * @param Name 名称，由调用方传入。
         * @return 本次计算、查询或创建得到的Ed图Pin。
         */
        UEdGraphPin* Pin(UEdGraphNode* N, FName Name)
        {
            auto* P = N ? N->FindPin(Name) : nullptr;
            if (!P) { Errors.Add(FString::Printf(TEXT("%s: missing %s.%s"), *BP->GetName(), *GetNameSafe(N), *Name.ToString())); }
            return P;
        }
        /** 按图模式校验并连接两个引脚。
         * @param A 参与当前计算的第一份输入值。
         * @param B 参与当前计算的第二份输入值。
         */
        void Link(UEdGraphPin* A, UEdGraphPin* B)
        {
            if (!A || !B || !Schema->TryCreateConnection(A, B))
            { Errors.Add(FString::Printf(TEXT("%s: failed link %s -> %s"), *BP->GetName(), A ? *A->PinName.ToString() : TEXT("null"), B ? *B->PinName.ToString() : TEXT("null"))); }
        }
        /** 设置指定节点引脚的默认值。
         * @param N N，由调用方传入。
         * @param Name 名称，由调用方传入。
         * @param Value 值，由调用方传入。
         */
        void Default(UEdGraphNode* N, FName Name, const FString& Value)
        { if (auto* P = Pin(N, Name)) { Schema->TrySetDefaultValue(*P, Value); } }
        /** 设置指定引脚使用的对象引用。
         * @param N N，由调用方传入。
         * @param Name 名称，由调用方传入。
         * @param Value 值，由调用方传入。
         */
        void Object(UEdGraphNode* N, FName Name, UObject* Value)
        { if (auto* P = Pin(N, Name)) { Schema->TrySetDefaultObject(*P, Value); } }
        /** 连接编辑器节点的执行引脚。
         * @param A 参与当前计算的第一份输入值。
         * @param B 参与当前计算的第二份输入值。
         */
        void Exec(UEdGraphNode* A, UEdGraphNode* B)
        { Link(Pin(A, UEdGraphSchema_K2::PN_Then), Pin(B, UEdGraphSchema_K2::PN_Execute)); }
        /** 生成当前蓝图自身对象的引用引脚。
         * @return 本次计算、查询或创建得到的Ed图Pin。
         */
        UEdGraphPin* Self() { return Pin(Node<UK2Node_Self>(), UEdGraphSchema_K2::PN_Self); }
        /** 创建指定类函数的蓝图调用节点。
         * @param Class 类型，由调用方传入。
         * @param Name 名称，由调用方传入。
         * @param Target 本次操作的目标。
         * @return 本次计算、查询或创建得到的2节点调用Function。
         */
        UK2Node_CallFunction* Call(UClass* Class, FName Name, UEdGraphPin* Target = nullptr)
        {
            auto* N = Node<UK2Node_CallFunction>(false); N->FunctionReference.SetExternalMember(Name, Class); N->AllocateDefaultPins();
            if (Target) { Link(Target, Pin(N, UEdGraphSchema_K2::PN_Self)); }
            return N;
        }
        /** 创建当前蓝图自身函数的调用节点。
         * @param Name 名称，由调用方传入。
         * @return 本次计算、查询或创建得到的2节点调用Function。
         */
        UK2Node_CallFunction* CallSelf(FName Name)
        {
            auto* N = Node<UK2Node_CallFunction>(false); N->FunctionReference.SetSelfMember(Name); N->AllocateDefaultPins(); return N;
        }
        /** 取得当前节点的结果引脚。
         * @param N N，由调用方传入。
         * @return 本次计算、查询或创建得到的Ed图Pin。
         */
        UEdGraphPin* Return(UEdGraphNode* N) { return Pin(N, N && N->IsA<UK2Node_FormatText>() ? FName(TEXT("Result")) : UEdGraphSchema_K2::PN_ReturnValue); }
        /** 获取当前记录。
         * @param Name 名称，由调用方传入。
         * @return 的当前值或引用。
         */
        UEdGraphPin* Get(FName Name)
        {
            auto* N = Node<UK2Node_VariableGet>(false); N->VariableReference.SetSelfMember(Name); N->AllocateDefaultPins(); return Pin(N, Name);
        }
        /** 设置当前记录。
         * @param Name 名称，由调用方传入。
         * @param Value 值，由调用方传入。
         * @return 本次计算、查询或创建得到的2节点Variable设置。
         */
        UK2Node_VariableSet* Set(FName Name, UEdGraphPin* Value = nullptr)
        {
            auto* N = Node<UK2Node_VariableSet>(false); N->VariableReference.SetSelfMember(Name); N->AllocateDefaultPins();
            if (Value) { Link(Value, Pin(N, Name)); }
            return N;
        }
        /** 创建指定的蓝图事件节点。
         * @param Class 类型，由调用方传入。
         * @param Name 名称，由调用方传入。
         * @return 本次计算、查询或创建得到的2节点事件。
         */
        UK2Node_Event* Event(UClass* Class, FName Name)
        {
            X = 0; Y += 480;
            auto* N = Node<UK2Node_Event>(false); N->EventReference.SetExternalMember(Name, Class); N->bOverrideFunction = true; N->AllocateDefaultPins(); return N;
        }
        /** 创建带参数的自定义事件节点。
         * @param Name 名称，由调用方传入。
         * @param Params 参数，由调用方传入。
         * @return 本次计算、查询或创建得到的2节点自定义事件。
         */
        UK2Node_CustomEvent* Custom(FName Name, const TArray<TPair<FName, FEdGraphPinType>>& Params = {})
        {
            X = 0; Y += 480;
            auto* N = Node<UK2Node_CustomEvent>(false); N->CustomFunctionName = Name; N->AllocateDefaultPins();
            for (const auto& P : Params) { N->CreateUserDefinedPin(P.Key, P.Value, EGPD_Output); }
            return N;
        }
        /** 向当前蓝图添加指定成员变量。
         * @param Name 名称，由调用方传入。
         * @param T T，由调用方传入。
         * @param Value 值，由调用方传入。
         */
        void Variable(FName Name, const FEdGraphPinType& T, const FString& Value = {})
        {
            if (!FBlueprintEditorUtils::AddMemberVariable(BP, Name, T, Value)) { Errors.Add(TEXT("Cannot add variable ") + Name.ToString()); }
        }
        /** 刷新蓝图的反射骨架以供后续节点绑定。 */
        void Skeleton() { FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP); }
        /** 取得当前本地玩家的根布局或当前根节点。
         * @return 本次计算、查询或创建得到的Ed图Pin。
         */
        UEdGraphPin* Root()
        {
            // 玩家控制器对象。
            auto* PC = Call(UUserWidget::StaticClass(), TEXT("GetOwningPlayer"));
            auto* R = Call(UHodgeUIManagerSubsystem::StaticClass(), TEXT("GetRootLayoutForController"));
            Link(Return(PC), Pin(R, TEXT("Controller"))); return Return(R);
        }
        /** 获取当前LocalPlayer对应的共享界面数据源。
         * @return 本次计算、查询或创建得到的Ed图Pin。
         */
        UEdGraphPin* Data()
        {
            // 玩家控制器对象。
            auto* PC = Call(UUserWidget::StaticClass(), TEXT("GetOwningPlayer"));
            auto* D = Call(UHodgeUIManagerSubsystem::StaticClass(), TEXT("GetGameplayDataForController"));
            Link(Return(PC), Pin(D, TEXT("Controller"))); return Return(D);
        }
        /** 为当前图创建条件分支节点。
         * @param Previous 更新前的状态。
         * @param Condition 条件，由调用方传入。
         * @return 本次计算、查询或创建得到的2节点IfThenElse。
         */
        UK2Node_IfThenElse* Branch(UEdGraphNode* Previous, UEdGraphPin* Condition)
        {
            auto* B = Node<UK2Node_IfThenElse>(); Exec(Previous, B); Link(Condition, Pin(B, UEdGraphSchema_K2::PN_Condition)); return B;
        }
        /** 检查当前对象或记录是否可用于后续处理。
         * @param Previous 更新前的状态。
         * @param Value 值，由调用方传入。
         * @return 本次计算、查询或创建得到的2节点IfThenElse。
         */
        UK2Node_IfThenElse* Valid(UEdGraphNode* Previous, UEdGraphPin* Value)
        {
            auto* C = Call(UKismetSystemLibrary::StaticClass(), TEXT("IsValid")); Link(Value, Pin(C, TEXT("Object"))); return Branch(Previous, Return(C));
        }
        /** 创建用于拆分指定结构数据的图节点。
         * @param Struct Struct，由调用方传入。
         * @param Value 值，由调用方传入。
         * @return 本次计算、查询或创建得到的2节点BreakStruct。
         */
        UK2Node_BreakStruct* Break(UScriptStruct* Struct, UEdGraphPin* Value)
        {
            auto* B = Node<UK2Node_BreakStruct>(false); B->StructType = Struct; B->bMadeAfterOverridePinRemoval = true; B->AllocateDefaultPins();
            for (auto* P : B->Pins) { if (P->Direction == EGPD_Input && P->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct) { Link(Value, P); break; } }
            return B;
        }
        /** 创建用于绑定事件的委托引脚。
         * @param Function Function，由调用方传入。
         * @return 本次计算、查询或创建得到的Ed图Pin。
         */
        UEdGraphPin* Delegate(FName Function)
        {
            auto* D = Node<UK2Node_CreateDelegate>(); D->SetFunction(Function); Link(Self(), D->GetObjectInPin()); return D->GetDelegateOutPin();
        }
        /** 建立当前对象的委托或节点连接。
         * @param Class 类型，由调用方传入。
         * @param Name 名称，由调用方传入。
         * @param Target 本次操作的目标。
         * @param Function Function，由调用方传入。
         * @return 本次计算、查询或创建得到的T。
         */
        template<class T> T* Bind(UClass* Class, FName Name, UEdGraphPin* Target, FName Function)
        {
            auto* N = Node<T>(false); N->DelegateReference.SetExternalMember(Name, Class); N->AllocateDefaultPins();
            Link(Target, Pin(N, UEdGraphSchema_K2::PN_Self));
            auto* D = Node<UK2Node_CreateDelegate>(); Link(Self(), D->GetObjectInPin()); Link(D->GetDelegateOutPin(), N->GetDelegatePin()); D->SetFunction(Function); return N;
        }
        /** 建立指定按钮的蓝图点击事件节点。
         * @param Widget 控件，由调用方传入。
         * @param Class 类型，由调用方传入。
         * @param DelegateName 委托名称，由调用方传入。
         * @return 本次计算、查询或创建得到的2节点组件已绑定事件。
         */
        UK2Node_ComponentBoundEvent* Click(FName Widget, UClass* Class, FName DelegateName)
        {
            X = 0; Y += 480;
            auto* N = Node<UK2Node_ComponentBoundEvent>(false);
            auto* Property = FindFProperty<FObjectProperty>(BP->SkeletonGeneratedClass, Widget);
            auto* DelegateProperty = FindFProperty<FMulticastDelegateProperty>(Class, DelegateName);
            if (!Property || !DelegateProperty) { Errors.Add(TEXT("Missing button property/delegate: ") + Widget.ToString()); return N; }
            N->InitializeComponentBoundEventParams(Property, DelegateProperty); N->AllocateDefaultPins(); return N;
        }
        /** 创建指定类型的对象转换节点。
         * @param Class 类型，由调用方传入。
         * @param Value 值，由调用方传入。
         * @return 本次计算、查询或创建得到的2节点DynamicCast。
         */
        UK2Node_DynamicCast* Cast(UClass* Class, UEdGraphPin* Value)
        {
            auto* N = Node<UK2Node_DynamicCast>(false); N->TargetType = Class; N->SetPurity(true); N->AllocateDefaultPins(); Link(Value, N->GetCastSourcePin()); return N;
        }
        /** 创建当前图中遍历集合的执行节点。
         * @param Previous 更新前的状态。
         * @param Array 数组，由调用方传入。
         * @return 本次计算、查询或创建得到的2节点MacroInstance。
         */
        UK2Node_MacroInstance* ForEach(UEdGraphNode* Previous, UEdGraphPin* Array)
        {
            auto* Standard = LoadObject<UBlueprint>(nullptr, TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros"));
            UEdGraph* Macro = nullptr;
            if (Standard) { for (UEdGraph* G : Standard->MacroGraphs) { if (G->GetFName() == TEXT("ForEachLoop")) { Macro = G; } } }
            auto* N = Node<UK2Node_MacroInstance>(false); N->SetMacroGraph(Macro); N->AllocateDefaultPins();
            Link(Pin(Previous, UEdGraphSchema_K2::PN_Then), Pin(N, TEXT("Exec"))); Link(Array, Pin(N, TEXT("Array"))); return N;
        }
        /** 生成页面的默认焦点目标。
         * @param Widget 控件，由调用方传入。
         */
        void Focus(FName Widget)
        {
            auto* G = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("BP_GetDesiredFocusTarget"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
            FBlueprintEditorUtils::AddFunctionGraph(BP, G, false, UCommonActivatableWidget::StaticClass());
            UK2Node_FunctionResult* Result = nullptr; UK2Node_FunctionEntry* Entry = nullptr;
            for (UEdGraphNode* N : G->Nodes) { if (auto* R = ::Cast<UK2Node_FunctionResult>(N)) { Result = R; } if (auto* E = ::Cast<UK2Node_FunctionEntry>(N)) { Entry = E; } }
            auto* Getter = NewObject<UK2Node_VariableGet>(G); G->AddNode(Getter, false, false); Getter->CreateNewGuid(); Getter->VariableReference.SetSelfMember(Widget); Getter->AllocateDefaultPins();
            Link(Getter->FindPin(Widget), Result ? Result->FindPin(UEdGraphSchema_K2::PN_ReturnValue) : nullptr);
            if (Entry && Result && Entry->FindPin(UEdGraphSchema_K2::PN_Then)) { Link(Entry->FindPin(UEdGraphSchema_K2::PN_Then), Result->FindPin(UEdGraphSchema_K2::PN_Execute)); }
        }
        /** 从所属根布局移除指定页面。
         * @param Previous 更新前的状态。
         * @param Widget 控件，由调用方传入。
         */
        void Pop(UEdGraphNode* Previous, UEdGraphPin* Widget)
        {
            auto* V = Valid(Previous, Root()); auto* P = Call(UHodgePrimaryGameLayout::StaticClass(), TEXT("Pop"), Root());
            Link(Widget, Pin(P, TEXT("Widget"))); Link(Pin(V, UEdGraphSchema_K2::PN_Then), Pin(P, UEdGraphSchema_K2::PN_Execute));
        }
    };

    /** 在当前控件树中建立文字显示元素。
     * @param Tree 编辑器制作或查询使用的UMG控件树。
     * @param Name 名称，由调用方传入。
     * @param Value 值，由调用方传入。
     * @param Size 大小，由调用方传入。
     * @return 本次计算、查询或创建得到的文字Block。
     */
    UTextBlock* DesignerText(UWidgetTree* Tree, FName Name, const TCHAR* Value, int32 Size = 18)
    {
        auto* T = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name); T->bIsVariable = true; T->SetText(FText::FromString(Value));
        auto Font = T->GetFont(); Font.Size = Size; T->SetFont(Font); return T;
    }
    /** 建立当前页面使用的布局面板。
     * @param Tree 编辑器制作或查询使用的UMG控件树。
     * @param Width 宽度，由调用方传入。
     * @return 本次计算、查询或创建得到的垂直盒体。
     */
    UVerticalBox* DesignerPanel(UWidgetTree* Tree, float Width)
    {
        auto* Root = Tree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("PageRoot")); Tree->RootWidget = Root;
        auto* Box = Tree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("PanelSize")); Box->SetWidthOverride(Width);
        auto* Slot = Root->AddChildToOverlay(Box); Slot->SetHorizontalAlignment(HAlign_Center); Slot->SetVerticalAlignment(VAlign_Center);
        auto* Border = Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PanelBackground")); Border->SetPadding(FMargin(24));
        Border->SetBrushColor(FLinearColor(.025f,.035f,.05f,.96f)); Box->AddChild(Border);
        auto* Column = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Content")); Border->AddChild(Column); return Column;
    }
    /** 在指定控件树中建立按钮实例。
     * @param Tree 编辑器制作或查询使用的UMG控件树。
     * @param Parent 父项，由调用方传入。
     * @param Class 类型，由调用方传入。
     * @param Name 名称，由调用方传入。
     * @return 本次计算、查询或创建得到的控件。
     */
    UWidget* DesignerButton(UWidgetTree* Tree, UPanelWidget* Parent, UClass* Class, FName Name)
    {
        auto* B = Tree->ConstructWidget<UCommonButtonBase>(Class, Name); B->bIsVariable = true; Parent->AddChild(B); return B;
    }
    /** 编译当前UMG资产并收集图与控件树诊断。
     * @param BP BP，由调用方传入。
     * @param Errors 收集配置或执行失败的诊断。
     * @param SaveAsset 保存资产，由调用方传入。
     * @return true表示本次检查或操作成功，false表示失败。
     */
    bool CompileDesigner(UWidgetBlueprint* BP, TArray<FString>& Errors, bool SaveAsset = false)
    {
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        FCompilerResultsLog Log; FKismetEditorUtilities::CompileBlueprint(BP, EBlueprintCompileOptions::None, &Log);
        if (Log.NumErrors) { Errors.Add(TEXT("Blueprint compilation failed: ") + BP->GetName()); return false; }
        return !SaveAsset || Save(BP);
    }
    /** 按当前制作契约取得UMG蓝图资产。
     * @param Path 路径，由调用方传入。
     * @param Parent 父项，由调用方传入。
     * @param Errors 收集配置或执行失败的诊断。
     * @return 本次计算、查询或创建得到的控件Blueprint。
     */
    UWidgetBlueprint* DesignerBlueprint(const FString& Path, UClass* Parent, TArray<FString>& Errors)
    {
        auto* BP = LoadObject<UWidgetBlueprint>(nullptr, *Path);
        if (!BP) { BP = Cast<UWidgetBlueprint>(Blueprint(Path, Parent, true)); }
        // 先检查对象与上下文，避免在依赖失效后继续处理。
        if (!BP) { Errors.Add(TEXT("Missing WBP: ") + Path); return nullptr; }
        if (BP->GetOutermost()->GetMetaData()->GetValue(BP, TEXT("HodgeUMGVersion")) == FString(TEXT("1"))) { return BP; }
        if (BP->WidgetTree && BP->WidgetTree->RootWidget)
        { Errors.Add(TEXT("Designer tree is already edited; refusing overwrite: ") + Path); return nullptr; }
        BP->Modify(); BP->ParentClass = Parent;
        for (UEdGraph* Graph : BP->UbergraphPages) { Graph->Nodes.Empty(); }
        CompileDesigner(BP, Errors);
        return BP;
    }
}

namespace
{
    /** 构建按钮图。
     * @param BP BP，由调用方传入。
     * @param Errors 收集配置或执行失败的诊断。
     */
    void BuildButtonGraph(UWidgetBlueprint* BP, TArray<FString>& Errors)
    {
        FDesignerGraph G(BP, Errors);
        G.Variable(TEXT("LabelText"), G.Type(UEdGraphSchema_K2::PC_Text), TEXT("INVTEXT(\"Button\")"));
        auto* E = G.Custom(TEXT("SetLabel"), {{TEXT("Label"), G.Type(UEdGraphSchema_K2::PC_Text)}});
        G.Skeleton();
        auto* Set = G.Call(UTextBlock::StaticClass(), TEXT("SetText"), G.Get(TEXT("ButtonLabel")));
        auto* Value = G.Set(TEXT("LabelText"), G.Pin(E, TEXT("Label"))); G.Exec(E, Value);
        G.Link(G.Pin(E, TEXT("Label")), G.Pin(Set, TEXT("InText"))); G.Exec(Value, Set);
        auto* Preview = G.Event(UUserWidget::StaticClass(), TEXT("PreConstruct"));
        auto* Apply = G.CallSelf(TEXT("SetLabel")); G.Link(G.Get(TEXT("LabelText")), G.Pin(Apply, TEXT("Label"))); G.Exec(Preview, Apply);
    }
    /** 构建生命显示图。
     * @param BP BP，由调用方传入。
     * @param Errors 收集配置或执行失败的诊断。
     * @param bReferenceHUD 是否引用HUD界面的控制标记。
     */
    void BuildVitalsGraph(UWidgetBlueprint* BP, TArray<FString>& Errors, bool bReferenceHUD = false)
    {
        FDesignerGraph G(BP, Errors);
        if (FBlueprintEditorUtils::FindNewVariableIndex(BP, TEXT("GameplayData")) == INDEX_NONE)
        { G.Variable(TEXT("GameplayData"), G.Type(UEdGraphSchema_K2::PC_Object, UHodgeGameplayUIDataSource::StaticClass())); }
        auto SnapshotType = G.Type(UEdGraphSchema_K2::PC_Struct, FHodgeUIVitalsSnapshot::StaticStruct());
        SnapshotType.bIsReference = true; SnapshotType.bIsConst = true;
        auto* Apply = G.Custom(TEXT("ApplyVitals"), {{TEXT("Snapshot"), SnapshotType}});
        auto* Refresh = G.Custom(TEXT("RefreshVitals")); G.Skeleton();
        auto* Break = G.Break(FHodgeUIVitalsSnapshot::StaticStruct(), G.Pin(Apply, TEXT("Snapshot")));
        auto* Condition = G.Branch(Apply, G.Pin(Break, TEXT("bReady")));
        for (bool Ready : {true, false})
        {
            auto* Format = G.Node<UK2Node_FormatText>();
            G.Schema->TrySetDefaultText(*Format->GetFormatPin(), FText::FromString(bReferenceHUD
                ? (Ready ? TEXT("{Health}/{MaxHealth}") : TEXT("--/--"))
                : (Ready ? TEXT("HP {Health} / {MaxHealth}") : TEXT("HP --"))));
            Format->PinDefaultValueChanged(Format->GetFormatPin());
            if (Ready)
            {
                for (FName Name : {FName(TEXT("Health")), FName(TEXT("MaxHealth"))})
                {
                    auto* Text = G.Call(UKismetTextLibrary::StaticClass(), TEXT("Conv_FloatToText"));
                    G.Link(G.Pin(Break, Name), G.Pin(Text, TEXT("Value"))); G.Default(Text, TEXT("MaximumFractionalDigits"), TEXT("0"));
                    G.Link(G.Return(Text), Format->FindArgumentPin(Name));
                }
            }
            auto* Label = G.Call(UTextBlock::StaticClass(), TEXT("SetText"), G.Get(TEXT("HealthText")));
            G.Link(G.Return(Format), G.Pin(Label, TEXT("InText")));
            G.Link(G.Pin(Condition, Ready ? UEdGraphSchema_K2::PN_Then : UEdGraphSchema_K2::PN_Else), G.Pin(Label, UEdGraphSchema_K2::PN_Execute));
            auto* Progress = G.Call(UProgressBar::StaticClass(), TEXT("SetPercent"), G.Get(TEXT("HealthBar")));
            G.Link(G.Pin(Break, TEXT("Percent")), G.Pin(Progress, TEXT("InPercent"))); G.Exec(Label, Progress);
            if (bReferenceHUD)
            {
                auto* Level = G.Node<UK2Node_FormatText>();
                G.Schema->TrySetDefaultText(*Level->GetFormatPin(), FText::FromString(Ready ? TEXT("Lv. {Level}") : TEXT("Lv. --")));
                Level->PinDefaultValueChanged(Level->GetFormatPin());
                if (Ready) { G.Link(G.Pin(Break, TEXT("CharacterLevel")), Level->FindArgumentPin(TEXT("Level"))); }
                auto* SetLevel = G.Call(UTextBlock::StaticClass(), TEXT("SetText"), G.Get(TEXT("LevelText")));
                G.Link(G.Return(Level), G.Pin(SetLevel, TEXT("InText"))); G.Exec(Progress, SetLevel);
            }
        }
        auto* Valid = G.Valid(Refresh, G.Get(TEXT("GameplayData")));
        auto* Vitals = G.Call(UHodgeGameplayUIDataSource::StaticClass(), TEXT("GetVitals"), G.Get(TEXT("GameplayData")));
        auto* Update = G.CallSelf(TEXT("ApplyVitals")); G.Link(G.Return(Vitals), G.Pin(Update, TEXT("Snapshot")));
        G.Link(G.Pin(Valid, UEdGraphSchema_K2::PN_Then), G.Pin(Update, UEdGraphSchema_K2::PN_Execute));
        auto* Construct = G.Event(UUserWidget::StaticClass(), TEXT("Construct"));
        auto* Data = G.Set(TEXT("GameplayData"), G.Data()); G.Exec(Construct, Data);
        auto* HasData = G.Valid(Data, G.Get(TEXT("GameplayData")));
        auto* Bind = G.Bind<UK2Node_AddDelegate>(UHodgeGameplayUIDataSource::StaticClass(), TEXT("OnVitalsChanged"), G.Get(TEXT("GameplayData")), TEXT("ApplyVitals"));
        G.Link(G.Pin(HasData, UEdGraphSchema_K2::PN_Then), G.Pin(Bind, UEdGraphSchema_K2::PN_Execute));
        auto* RefreshCall = G.CallSelf(TEXT("RefreshVitals")); G.Exec(Bind, RefreshCall);
        auto* Destruct = G.Event(UUserWidget::StaticClass(), TEXT("Destruct"));
        auto* HasOld = G.Valid(Destruct, G.Get(TEXT("GameplayData")));
        auto* Unbind = G.Bind<UK2Node_RemoveDelegate>(UHodgeGameplayUIDataSource::StaticClass(), TEXT("OnVitalsChanged"), G.Get(TEXT("GameplayData")), TEXT("ApplyVitals"));
        G.Link(G.Pin(HasOld, UEdGraphSchema_K2::PN_Then), G.Pin(Unbind, UEdGraphSchema_K2::PN_Execute));
        auto* Clear = G.Set(TEXT("GameplayData")); G.Exec(Unbind, Clear);
        auto* Preview = G.Event(UUserWidget::StaticClass(), TEXT("PreConstruct"));
        auto* Design = G.Branch(Preview, G.Pin(Preview, TEXT("IsDesignTime")));
        auto* PreviewText = G.Call(UTextBlock::StaticClass(), TEXT("SetText"), G.Get(TEXT("HealthText")));
        G.Schema->TrySetDefaultText(*G.Pin(PreviewText, TEXT("InText")), FText::FromString(bReferenceHUD ? TEXT("75/100") : TEXT("HP 75 / 100")));
        G.Link(G.Pin(Design, UEdGraphSchema_K2::PN_Then), G.Pin(PreviewText, UEdGraphSchema_K2::PN_Execute));
        auto* PreviewBar = G.Call(UProgressBar::StaticClass(), TEXT("SetPercent"), G.Get(TEXT("HealthBar"))); G.Default(PreviewBar, TEXT("InPercent"), TEXT("0.75")); G.Exec(PreviewText, PreviewBar);
    }
    /** 构建槽位图。
     * @param BP BP，由调用方传入。
     * @param ButtonClass 按钮类型，由调用方传入。
     * @param Errors 收集配置或执行失败的诊断。
     */
    void BuildSlotGraph(UWidgetBlueprint* BP, UClass* ButtonClass, TArray<FString>& Errors)
    {
        FDesignerGraph G(BP, Errors);
        G.Variable(TEXT("GameplayData"), G.Type(UEdGraphSchema_K2::PC_Object, UHodgeGameplayUIDataSource::StaticClass()));
        G.Variable(TEXT("InputTag"), G.Type(UEdGraphSchema_K2::PC_Struct, FGameplayTag::StaticStruct()));
        auto* Init = G.Custom(TEXT("InitializeSlot"), {{TEXT("Label"), G.Type(UEdGraphSchema_K2::PC_Text)},
            {TEXT("Tag"), G.Type(UEdGraphSchema_K2::PC_Struct, FGameplayTag::StaticStruct())},
            {TEXT("Source"), G.Type(UEdGraphSchema_K2::PC_Object, UHodgeGameplayUIDataSource::StaticClass())}});
        auto* Refresh = G.Custom(TEXT("RefreshState")); G.Skeleton();
        auto* SetData = G.Set(TEXT("GameplayData"), G.Pin(Init, TEXT("Source"))); G.Exec(Init, SetData);
        auto* SetTag = G.Set(TEXT("InputTag"), G.Pin(Init, TEXT("Tag"))); G.Exec(SetData, SetTag);
        auto* Label = G.Call(ButtonClass, TEXT("SetLabel"), G.Get(TEXT("AbilityButton"))); G.Link(G.Pin(Init, TEXT("Label")), G.Pin(Label, TEXT("Label"))); G.Exec(SetTag, Label);
        auto* R = G.CallSelf(TEXT("RefreshState")); G.Exec(Label, R);
        auto* V = G.Valid(Refresh, G.Get(TEXT("GameplayData")));
        auto* Query = G.Call(UHodgeGameplayUIDataSource::StaticClass(), TEXT("GetAbilityDisplayState"), G.Get(TEXT("GameplayData")));
        G.Link(G.Get(TEXT("InputTag")), G.Pin(Query, TEXT("InputTag")));
        auto* State = G.Break(FHodgeUIAbilityDisplayState::StaticStruct(), G.Return(Query));
        auto* Enabled = G.Call(UWidget::StaticClass(), TEXT("SetIsEnabled"), G.Get(TEXT("AbilityButton")));
        G.Link(G.Pin(State, TEXT("bInputAllowed")), G.Pin(Enabled, TEXT("bInIsEnabled")));
        G.Link(G.Pin(V, UEdGraphSchema_K2::PN_Then), G.Pin(Enabled, UEdGraphSchema_K2::PN_Execute));
        auto* Time = G.Call(UKismetTextLibrary::StaticClass(), TEXT("Conv_FloatToText"));
        G.Link(G.Pin(State, TEXT("CooldownRemaining")), G.Pin(Time, TEXT("Value"))); G.Default(Time, TEXT("MaximumFractionalDigits"), TEXT("1"));
        auto* Cooldown = G.Call(UTextBlock::StaticClass(), TEXT("SetText"), G.Get(TEXT("CooldownText"))); G.Link(G.Return(Time), G.Pin(Cooldown, TEXT("InText"))); G.Exec(Enabled, Cooldown);
        auto* Greater = G.Call(UKismetMathLibrary::StaticClass(), TEXT("Greater_DoubleDouble"));
        G.Link(G.Pin(State, TEXT("CooldownRemaining")), G.Pin(Greater, TEXT("A"))); G.Default(Greater, TEXT("B"), TEXT("0"));
        auto* Show = G.Branch(Cooldown, G.Return(Greater));
        for (bool Visible : {true,false})
        {
            auto* Visibility = G.Call(UWidget::StaticClass(), TEXT("SetVisibility"), G.Get(TEXT("CooldownText")));
            G.Default(Visibility, TEXT("InVisibility"), Visible ? TEXT("Visible") : TEXT("Hidden"));
            G.Link(G.Pin(Show, Visible ? UEdGraphSchema_K2::PN_Then : UEdGraphSchema_K2::PN_Else), G.Pin(Visibility, UEdGraphSchema_K2::PN_Execute));
        }
        auto* Click = G.Click(TEXT("AbilityButton"), UCommonButtonBase::StaticClass(), TEXT("OnButtonBaseClicked"));
        auto* HasData = G.Valid(Click, G.Get(TEXT("GameplayData")));
        auto* Submit = G.Call(UHodgeGameplayUIDataSource::StaticClass(), TEXT("SubmitInput"), G.Get(TEXT("GameplayData")));
        G.Link(G.Get(TEXT("InputTag")), G.Pin(Submit, TEXT("InputTag"))); G.Link(G.Pin(HasData, UEdGraphSchema_K2::PN_Then), G.Pin(Submit, UEdGraphSchema_K2::PN_Execute));
        auto* Preview = G.Event(UUserWidget::StaticClass(), TEXT("PreConstruct")); auto* Design = G.Branch(Preview, G.Pin(Preview, TEXT("IsDesignTime")));
        auto* PreviewLabel = G.Call(ButtonClass, TEXT("SetLabel"), G.Get(TEXT("AbilityButton")));
        G.Schema->TrySetDefaultText(*G.Pin(PreviewLabel, TEXT("Label")), FText::FromString(TEXT("Skill [Key]")));
        G.Link(G.Pin(Design, UEdGraphSchema_K2::PN_Then), G.Pin(PreviewLabel, UEdGraphSchema_K2::PN_Execute));
    }
    /** 构建条形显示图。
     * @param BP BP，由调用方传入。
     * @param SlotClass 槽位类型，由调用方传入。
     * @param Errors 收集配置或执行失败的诊断。
     */
    void BuildBarGraph(UWidgetBlueprint* BP, UClass* SlotClass, TArray<FString>& Errors)
    {
        FDesignerGraph G(BP, Errors);
        auto Slots = G.Type(UEdGraphSchema_K2::PC_Struct, FHodgeHUDAbilitySlot::StaticStruct()); Slots.ContainerType = EPinContainerType::Array;
        G.Variable(TEXT("Slots"), Slots);
        G.Variable(TEXT("GameplayData"), G.Type(UEdGraphSchema_K2::PC_Object, UHodgeGameplayUIDataSource::StaticClass()));
        G.Variable(TEXT("RefreshTimer"), G.Type(UEdGraphSchema_K2::PC_Struct, FTimerHandle::StaticStruct()));
        auto* Refresh = G.Custom(TEXT("RefreshAbilities")); G.Skeleton();
        auto* Entries = G.Call(UDynamicEntryBoxBase::StaticClass(), TEXT("GetAllEntries"), G.Get(TEXT("AbilityEntries")));
        auto* LoopRefresh = G.ForEach(Refresh, G.Return(Entries));
        auto* Entry = G.Cast(SlotClass, G.Pin(LoopRefresh, TEXT("Array Element")));
        auto* Update = G.Call(SlotClass, TEXT("RefreshState"), Entry->GetCastResultPin());
        G.Link(G.Pin(LoopRefresh, TEXT("LoopBody")), G.Pin(Update, UEdGraphSchema_K2::PN_Execute));
        auto* Construct = G.Event(UUserWidget::StaticClass(), TEXT("Construct")); auto* Source = G.Set(TEXT("GameplayData"), G.Data()); G.Exec(Construct, Source);
        auto* Reset = G.Call(UDynamicEntryBox::StaticClass(), TEXT("Reset"), G.Get(TEXT("AbilityEntries"))); G.Exec(Source, Reset);
        auto* Loop = G.ForEach(Reset, G.Get(TEXT("Slots")));
        auto* Config = G.Break(FHodgeHUDAbilitySlot::StaticStruct(), G.Pin(Loop, TEXT("Array Element")));
        auto* Create = G.Call(UDynamicEntryBox::StaticClass(), TEXT("BP_CreateEntryOfClass"), G.Get(TEXT("AbilityEntries"))); G.Object(Create, TEXT("EntryClass"), SlotClass);
        G.Link(G.Pin(Loop, TEXT("LoopBody")), G.Pin(Create, UEdGraphSchema_K2::PN_Execute));
        auto* CastEntry = G.Cast(SlotClass, G.Return(Create)); auto* Init = G.Call(SlotClass, TEXT("InitializeSlot"), CastEntry->GetCastResultPin());
        G.Link(G.Pin(Config, TEXT("Label")), G.Pin(Init, TEXT("Label"))); G.Link(G.Pin(Config, TEXT("InputTag")), G.Pin(Init, TEXT("Tag")));
        G.Link(G.Get(TEXT("GameplayData")), G.Pin(Init, TEXT("Source"))); G.Exec(Create, Init);
        auto* Timer = G.Call(UKismetSystemLibrary::StaticClass(), TEXT("K2_SetTimerDelegate"));
        auto* TimerEvent = G.Node<UK2Node_CreateDelegate>(); G.Link(G.Self(),TimerEvent->GetObjectInPin()); G.Link(TimerEvent->GetDelegateOutPin(),G.Pin(Timer,TEXT("Delegate"))); TimerEvent->SetFunction(TEXT("RefreshAbilities")); G.Default(Timer, TEXT("Time"), TEXT("0.1")); G.Default(Timer, TEXT("bLooping"), TEXT("true"));
        G.Link(G.Pin(Loop, TEXT("Completed")), G.Pin(Timer, UEdGraphSchema_K2::PN_Execute));
        auto* Handle = G.Set(TEXT("RefreshTimer"), G.Return(Timer)); G.Exec(Timer, Handle);
        auto* Destruct = G.Event(UUserWidget::StaticClass(), TEXT("Destruct"));
        auto* Clear = G.Call(UKismetSystemLibrary::StaticClass(), TEXT("K2_ClearAndInvalidateTimerHandle")); G.Link(G.Self(), G.Pin(Clear, TEXT("WorldContextObject"))); G.Link(G.Get(TEXT("RefreshTimer")), G.Pin(Clear, TEXT("Handle"))); G.Exec(Destruct, Clear);
        auto* Remove = G.Call(UDynamicEntryBox::StaticClass(), TEXT("Reset"), G.Get(TEXT("AbilityEntries"))); G.Exec(Clear, Remove);
        auto* ClearData = G.Set(TEXT("GameplayData")); G.Exec(Remove, ClearData);
    }
}

namespace
{
    /** 添加结果Dispatcher。
     * @param BP BP，由调用方传入。
     */
    void AddResultDispatcher(UWidgetBlueprint* BP)
    {
        FEdGraphPinType T; T.PinCategory = UEdGraphSchema_K2::PC_MCDelegate;
        FBlueprintEditorUtils::AddMemberVariable(BP, TEXT("OnCompleted"), T);
        auto* Graph = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("OnCompleted"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
        const auto* Schema = GetDefault<UEdGraphSchema_K2>(); Graph->bEditable = false;
        Schema->CreateDefaultNodesForGraph(*Graph); Schema->CreateFunctionGraphTerminators(*Graph, static_cast<UClass*>(nullptr));
        Schema->AddExtraFunctionFlags(Graph, FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public); Schema->MarkFunctionEntryAsEditable(Graph, true);
        for (UEdGraphNode* N : Graph->Nodes)
        {
            if (auto* Entry = ::Cast<UK2Node_FunctionEntry>(N))
            {
                Entry->CreateUserDefinedPin(TEXT("bAccepted"), FDesignerGraph::Type(UEdGraphSchema_K2::PC_Boolean), EGPD_Output);
                Entry->CreateUserDefinedPin(TEXT("Dialog"), FDesignerGraph::Type(UEdGraphSchema_K2::PC_Object, UCommonActivatableWidget::StaticClass()), EGPD_Output);
                Entry->CreateUserDefinedPin(TEXT("Request"), FDesignerGraph::Type(UEdGraphSchema_K2::PC_Int), EGPD_Output);
            }
        }
        BP->DelegateSignatureGraphs.Add(Graph); FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    }
    /** 构建确认图。
     * @param BP BP，由调用方传入。
     * @param Errors 收集配置或执行失败的诊断。
     */
    void BuildConfirmationGraph(UWidgetBlueprint* BP, TArray<FString>& Errors)
    {
        AddResultDispatcher(BP);
        FDesignerGraph G(BP, Errors);
        G.Variable(TEXT("Completed"), G.Type(UEdGraphSchema_K2::PC_Boolean));
        G.Variable(TEXT("RequestSerial"), G.Type(UEdGraphSchema_K2::PC_Int));
        auto* FinishGraph = FBlueprintEditorUtils::CreateNewGraph(BP, TEXT("Finish"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
        FBlueprintEditorUtils::AddFunctionGraph(BP, FinishGraph, true, static_cast<UClass*>(nullptr));
        UK2Node_FunctionEntry* Finish = nullptr;
        UK2Node_FunctionResult* FinishReturn = nullptr;
        for (UEdGraphNode* N : FinishGraph->Nodes)
        { if (auto* E = ::Cast<UK2Node_FunctionEntry>(N)) { Finish = E; } if (auto* R = ::Cast<UK2Node_FunctionResult>(N)) { FinishReturn = R; } }
        Finish->CreateUserDefinedPin(TEXT("bAccepted"), G.Type(UEdGraphSchema_K2::PC_Boolean), EGPD_Output);
        auto* Init = G.Custom(TEXT("InitializeRequest"), {{TEXT("Request"), G.Type(UEdGraphSchema_K2::PC_Int)}});
        G.Skeleton();
        auto* Serial = G.Set(TEXT("RequestSerial"), G.Pin(Init, TEXT("Request"))); G.Exec(Init, Serial);
        G.Graph = FinishGraph;
        auto* Already = G.Branch(Finish, G.Get(TEXT("Completed")));
        auto* Complete = G.Set(TEXT("Completed")); G.Default(Complete, TEXT("Completed"), TEXT("true"));
        G.Link(G.Pin(Already, UEdGraphSchema_K2::PN_Else), G.Pin(Complete, UEdGraphSchema_K2::PN_Execute));
        auto* Sequence = G.Node<UK2Node_ExecutionSequence>(); G.Exec(Complete, Sequence);
        auto* HasRoot = G.Node<UK2Node_IfThenElse>();
        auto* CheckRoot = G.Call(UKismetSystemLibrary::StaticClass(), TEXT("IsValid")); G.Link(G.Root(), G.Pin(CheckRoot, TEXT("Object")));
        G.Link(G.Return(CheckRoot), G.Pin(HasRoot, UEdGraphSchema_K2::PN_Condition)); G.Link(G.Pin(Sequence, TEXT("Then_0")), G.Pin(HasRoot, UEdGraphSchema_K2::PN_Execute));
        auto* Pop = G.Call(UHodgePrimaryGameLayout::StaticClass(), TEXT("Pop"), G.Root()); G.Link(G.Self(), G.Pin(Pop, TEXT("Widget"))); G.Link(G.Pin(HasRoot, UEdGraphSchema_K2::PN_Then), G.Pin(Pop, UEdGraphSchema_K2::PN_Execute));
        auto* Result = G.Node<UK2Node_CallDelegate>(false); Result->DelegateReference.SetSelfMember(TEXT("OnCompleted")); Result->AllocateDefaultPins();
        G.Link(G.Pin(Finish, TEXT("bAccepted")), G.Pin(Result, TEXT("bAccepted"))); G.Link(G.Self(), G.Pin(Result, TEXT("Dialog"))); G.Link(G.Get(TEXT("RequestSerial")), G.Pin(Result, TEXT("Request"))); G.Link(G.Pin(Sequence, TEXT("Then_1")), G.Pin(Result, UEdGraphSchema_K2::PN_Execute));
        if (FinishReturn) { G.Exec(Result, FinishReturn); G.Link(G.Pin(Already, UEdGraphSchema_K2::PN_Then), G.Pin(FinishReturn, UEdGraphSchema_K2::PN_Execute)); }
        G.Graph = BP->UbergraphPages[0];
        auto* Activated = G.Event(UCommonActivatableWidget::StaticClass(), TEXT("BP_OnActivated")); auto* Reset = G.Set(TEXT("Completed")); G.Default(Reset, TEXT("Completed"), TEXT("false")); G.Exec(Activated, Reset);
        for (FName Event : {FName(TEXT("BP_OnDeactivated")), FName(TEXT("Destruct"))})
        {
            auto* E = G.Event(Event == TEXT("Destruct") ? UUserWidget::StaticClass() : UCommonActivatableWidget::StaticClass(), Event);
            auto* F = G.CallSelf(TEXT("Finish")); G.Default(F, TEXT("bAccepted"), TEXT("false")); G.Exec(E, F);
        }
        for (const auto& Pair : {TPair<FName,bool>(TEXT("AcceptButton"), true), TPair<FName,bool>(TEXT("CancelButton"), false)})
        {
            auto* E = G.Click(Pair.Key, UCommonButtonBase::StaticClass(), TEXT("OnButtonBaseClicked")); auto* F = G.CallSelf(TEXT("Finish")); G.Default(F, TEXT("bAccepted"), Pair.Value ? TEXT("true") : TEXT("false")); G.Exec(E, F);
        }
        G.Focus(TEXT("AcceptButton"));
    }
    /** 构建菜单图。
     * @param BP BP，由调用方传入。
     * @param ConfirmationClass 确认类型，由调用方传入。
     * @param Errors 收集配置或执行失败的诊断。
     */
    void BuildMenuGraph(UWidgetBlueprint* BP, UClass* ConfirmationClass, TArray<FString>& Errors)
    {
        FDesignerGraph G(BP, Errors);
        G.Variable(TEXT("ConfirmationClass"), G.Type(UEdGraphSchema_K2::PC_Class, UCommonActivatableWidget::StaticClass()), ConfirmationClass->GetPathName());
        G.Variable(TEXT("ActiveDialog"), G.Type(UEdGraphSchema_K2::PC_Object, ConfirmationClass));
        G.Variable(TEXT("RequestSerial"), G.Type(UEdGraphSchema_K2::PC_Int));
        auto* Show = G.Custom(TEXT("ShowConfirmation"));
        auto* Resume = G.Custom(TEXT("Resume"));
        auto* Dismiss = G.Custom(TEXT("DismissConfirmation"), {{TEXT("Dialog"), G.Type(UEdGraphSchema_K2::PC_Object, ConfirmationClass)}});
        auto* Completed = G.Custom(TEXT("HandleConfirmationResult"), {{TEXT("bAccepted"), G.Type(UEdGraphSchema_K2::PC_Boolean)},
            {TEXT("Dialog"), G.Type(UEdGraphSchema_K2::PC_Object, UCommonActivatableWidget::StaticClass())}, {TEXT("Request"), G.Type(UEdGraphSchema_K2::PC_Int)}});
        G.Skeleton(); G.Pop(Resume, G.Self());
        auto* Existing = G.Valid(Show, G.Get(TEXT("ActiveDialog")));
        auto* HasRoot = G.Node<UK2Node_IfThenElse>();
        auto* IsRoot = G.Call(UKismetSystemLibrary::StaticClass(), TEXT("IsValid")); G.Link(G.Root(), G.Pin(IsRoot, TEXT("Object")));
        G.Link(G.Pin(Existing, UEdGraphSchema_K2::PN_Else), G.Pin(HasRoot, UEdGraphSchema_K2::PN_Execute)); G.Link(G.Return(IsRoot), G.Pin(HasRoot, UEdGraphSchema_K2::PN_Condition));
        auto* Push = G.Call(UHodgePrimaryGameLayout::StaticClass(), TEXT("Push"), G.Root()); G.Default(Push, TEXT("Tag"), TEXT("(TagName=\"UI.Layer.Modal\")")); G.Link(G.Get(TEXT("ConfirmationClass")), G.Pin(Push, TEXT("WidgetClass")));
        G.Link(G.Pin(HasRoot, UEdGraphSchema_K2::PN_Then), G.Pin(Push, UEdGraphSchema_K2::PN_Execute));
        auto* Dialog = G.Cast(ConfirmationClass, G.Return(Push)); auto* Assign = G.Set(TEXT("ActiveDialog"), Dialog->GetCastResultPin()); G.Exec(Push, Assign);
        auto* Init = G.Call(ConfirmationClass, TEXT("InitializeRequest"), G.Get(TEXT("ActiveDialog"))); G.Link(G.Get(TEXT("RequestSerial")), G.Pin(Init, TEXT("Request"))); G.Exec(Assign, Init);
        auto* Bind = G.Bind<UK2Node_AddDelegate>(ConfirmationClass, TEXT("OnCompleted"), G.Get(TEXT("ActiveDialog")), TEXT("HandleConfirmationResult")); G.Exec(Init, Bind);
        auto* Valid = G.Valid(Dismiss, G.Pin(Dismiss, TEXT("Dialog")));
        auto* Remove = G.Bind<UK2Node_RemoveDelegate>(ConfirmationClass, TEXT("OnCompleted"), G.Pin(Dismiss, TEXT("Dialog")), TEXT("HandleConfirmationResult")); G.Link(G.Pin(Valid, UEdGraphSchema_K2::PN_Then), G.Pin(Remove, UEdGraphSchema_K2::PN_Execute));
        auto* Clear = G.Set(TEXT("ActiveDialog")); G.Exec(Remove, Clear); G.Pop(Clear, G.Pin(Dismiss, TEXT("Dialog")));
        auto* EqualDialog = G.Call(UKismetMathLibrary::StaticClass(), TEXT("EqualEqual_ObjectObject")); G.Link(G.Get(TEXT("ActiveDialog")), G.Pin(EqualDialog, TEXT("A"))); G.Link(G.Pin(Completed, TEXT("Dialog")), G.Pin(EqualDialog, TEXT("B")));
        auto* EqualSerial = G.Call(UKismetMathLibrary::StaticClass(), TEXT("EqualEqual_IntInt")); G.Link(G.Get(TEXT("RequestSerial")), G.Pin(EqualSerial, TEXT("A"))); G.Link(G.Pin(Completed, TEXT("Request")), G.Pin(EqualSerial, TEXT("B")));
        auto* Both = G.Call(UKismetMathLibrary::StaticClass(), TEXT("BooleanAND")); G.Link(G.Return(EqualDialog), G.Pin(Both, TEXT("A"))); G.Link(G.Return(EqualSerial), G.Pin(Both, TEXT("B")));
        auto* Matched = G.Branch(Completed, G.Return(Both));
        auto* Unbind = G.Bind<UK2Node_RemoveDelegate>(ConfirmationClass, TEXT("OnCompleted"), G.Get(TEXT("ActiveDialog")), TEXT("HandleConfirmationResult")); G.Link(G.Pin(Matched, UEdGraphSchema_K2::PN_Then), G.Pin(Unbind, UEdGraphSchema_K2::PN_Execute));
        auto* ResetDialog = G.Set(TEXT("ActiveDialog")); G.Exec(Unbind, ResetDialog);
        auto* IsActive = G.Call(UCommonActivatableWidget::StaticClass(), TEXT("IsActivated")); auto* Accepted = G.Call(UKismetMathLibrary::StaticClass(), TEXT("BooleanAND")); G.Link(G.Return(IsActive), G.Pin(Accepted, TEXT("A"))); G.Link(G.Pin(Completed, TEXT("bAccepted")), G.Pin(Accepted, TEXT("B")));
        auto* Accept = G.Branch(ResetDialog, G.Return(Accepted)); auto* Continue = G.CallSelf(TEXT("Resume")); G.Link(G.Pin(Accept, UEdGraphSchema_K2::PN_Then), G.Pin(Continue, UEdGraphSchema_K2::PN_Execute));
        auto* Activated = G.Event(UCommonActivatableWidget::StaticClass(), TEXT("BP_OnActivated"));
        auto* Increment = G.Call(UKismetMathLibrary::StaticClass(), TEXT("Add_IntInt")); G.Link(G.Get(TEXT("RequestSerial")), G.Pin(Increment, TEXT("A"))); G.Default(Increment, TEXT("B"), TEXT("1")); auto* Serial = G.Set(TEXT("RequestSerial"), G.Return(Increment)); G.Exec(Activated, Serial);
        for (FName Event : {FName(TEXT("BP_OnDeactivated")),FName(TEXT("Destruct"))})
        {
            auto* E = G.Event(Event == TEXT("Destruct") ? UUserWidget::StaticClass() : UCommonActivatableWidget::StaticClass(), Event);
            auto* D = G.CallSelf(TEXT("DismissConfirmation")); G.Link(G.Get(TEXT("ActiveDialog")), G.Pin(D, TEXT("Dialog"))); G.Exec(E, D);
        }
        auto* ResumeClick = G.Click(TEXT("ResumeButton"), UCommonButtonBase::StaticClass(), TEXT("OnButtonBaseClicked")); auto* ResumeCall = G.CallSelf(TEXT("Resume")); G.Exec(ResumeClick, ResumeCall);
        auto* ConfirmClick = G.Click(TEXT("ConfirmButton"), UCommonButtonBase::StaticClass(), TEXT("OnButtonBaseClicked")); auto* ShowCall = G.CallSelf(TEXT("ShowConfirmation")); G.Exec(ConfirmClick, ShowCall);
        G.Focus(TEXT("ResumeButton"));
    }
}

/** 把指定UI原型迁入保存的UMG控件树与Graph。 */
FString UHodgeUIAuthoringLibrary::MigrateDesignerAssets()
{
    auto Report = MakeShared<FJsonObject>(); Report->SetBoolField(TEXT("success"), false);
    TArray<FString> Errors;
    if (!GEditor || GEditor->PlayWorld) { Report->SetStringField(TEXT("error"), TEXT("Stop PIE before migrating Designer assets")); return Json(Report); }
    const FString Base = TEXT("/Game/Main/UI/");
    const TArray<FString> Paths = {TEXT("Foundation/WBP_HodgePrimaryLayout"),TEXT("HUD/WBP_HodgeHUD"),TEXT("HUD/WBP_PlayerVitals"),TEXT("HUD/WBP_AbilityBar"),TEXT("Menu/WBP_HodgePauseMenu"),TEXT("Menu/WBP_HodgeConfirmation")};
    bool Complete = true;
    for (const auto& Path : Paths)
    {
        auto* BP = LoadObject<UWidgetBlueprint>(nullptr, *(Base + Path));
        if (!BP || BP->GetOutermost()->GetMetaData()->GetValue(BP, TEXT("HodgeUMGVersion")) != FString(TEXT("1"))) { Complete = false; }
    }
    if (Complete) { Report->SetBoolField(TEXT("success"), true); Report->SetBoolField(TEXT("alreadyMigrated"), true); return Json(Report); }
    TArray<FHodgeHUDAbilitySlot> OldSlots;
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (auto* Old = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("HUD/WBP_AbilityBar"))))
    { if (auto* CDO = Old->GeneratedClass ? Old->GeneratedClass->GetDefaultObject() : nullptr)
      { if (auto* P = FindFProperty<FArrayProperty>(CDO->GetClass(),TEXT("Slots"))) { OldSlots = *P->ContainerPtrToValuePtr<TArray<FHodgeHUDAbilitySlot>>(CDO); } } }
    if (OldSlots.IsEmpty()) { OldSlots = {{FText::FromString(TEXT("Attack [LMB]")),HodgeGameplayTags::InputTag_Ability_Melee},{FText::FromString(TEXT("Jump [Space]")),HodgeGameplayTags::InputTag_Jump}}; }
    auto* ButtonBP = DesignerBlueprint(Base + TEXT("Foundation/WBP_UIButton"), UCommonButtonBase::StaticClass(), Errors);
    if (ButtonBP)
    {
        auto* Border = ButtonBP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(),TEXT("ButtonBackground")); Border->SetPadding(FMargin(12,8)); Border->SetBrushColor(FLinearColor(.16f,.19f,.24f,1));
        Border->AddChild(DesignerText(ButtonBP->WidgetTree, TEXT("ButtonLabel"), TEXT("Button"))); ButtonBP->WidgetTree->RootWidget = Border;
        CompileDesigner(ButtonBP, Errors); BuildButtonGraph(ButtonBP, Errors); CompileDesigner(ButtonBP, Errors);
    }
    auto* RootBP = DesignerBlueprint(Base + Paths[0], UHodgePrimaryGameLayout::StaticClass(), Errors);
    auto* HUDBP = DesignerBlueprint(Base + Paths[1], UHodgeHUDLayout::StaticClass(), Errors);
    auto* VitalsBP = DesignerBlueprint(Base + Paths[2], UCommonUserWidget::StaticClass(), Errors);
    auto* BarBP = DesignerBlueprint(Base + Paths[3], UCommonUserWidget::StaticClass(), Errors);
    auto* MenuBP = DesignerBlueprint(Base + Paths[4], UHodgeActivatableWidget::StaticClass(), Errors);
    auto* ConfirmBP = DesignerBlueprint(Base + Paths[5], UHodgeActivatableWidget::StaticClass(), Errors);
    auto* SlotBP = DesignerBlueprint(Base + TEXT("HUD/WBP_AbilitySlot"), UCommonUserWidget::StaticClass(), Errors);
    if (!ButtonBP || !RootBP || !HUDBP || !VitalsBP || !BarBP || !MenuBP || !ConfirmBP || !SlotBP) { Errors.Add(TEXT("Migration setup failed")); }
    if (Errors.IsEmpty())
    {
        auto* Overlay = RootBP->WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(),TEXT("RootLayers")); RootBP->WidgetTree->RootWidget = Overlay;
        for (FName Name : {FName(TEXT("GameLayer")),FName(TEXT("GameMenuLayer")),FName(TEXT("MenuLayer")),FName(TEXT("ModalLayer"))})
        {
            auto* Stack = RootBP->WidgetTree->ConstructWidget<UCommonActivatableWidgetStack>(UCommonActivatableWidgetStack::StaticClass(),Name); Stack->bIsVariable = true; Stack->SetTransitionDuration(0.f);
            auto* Slot = Overlay->AddChildToOverlay(Stack); Slot->SetHorizontalAlignment(HAlign_Fill); Slot->SetVerticalAlignment(VAlign_Fill);
        }
        auto* HUD = HUDBP->WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(),TEXT("HUDRoot")); HUDBP->WidgetTree->RootWidget = HUD;
        for (const auto& Pair : {TPair<FName,FGameplayTag>(TEXT("PlayerVitalsPoint"),FGameplayTag::RequestGameplayTag(TEXT("UI.Slot.PlayerVitals"))),TPair<FName,FGameplayTag>(TEXT("AbilityBarPoint"),FGameplayTag::RequestGameplayTag(TEXT("UI.Slot.AbilityBar")))})
        {
            auto* Point = HUDBP->WidgetTree->ConstructWidget<UUIExtensionPointWidget>(UUIExtensionPointWidget::StaticClass(),Pair.Key); Point->SetExtensionPointTag(Pair.Value);
            auto* Slot = HUD->AddChildToOverlay(Point); Slot->SetVerticalAlignment(VAlign_Bottom); Slot->SetHorizontalAlignment(Pair.Key == TEXT("PlayerVitalsPoint") ? HAlign_Left : HAlign_Right);
            Slot->SetPadding(FMargin(32));
        }
        auto* HintSlot = HUD->AddChildToOverlay(DesignerText(HUDBP->WidgetTree,TEXT("MenuHint"),TEXT("Esc  Menu"),16)); HintSlot->SetHorizontalAlignment(HAlign_Right); HintSlot->SetVerticalAlignment(VAlign_Top); HintSlot->SetPadding(FMargin(20));
        auto* Size = VitalsBP->WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(),TEXT("VitalsSize")); Size->SetWidthOverride(280); VitalsBP->WidgetTree->RootWidget = Size;
        auto* Background = VitalsBP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(),TEXT("VitalsBackground")); Background->SetPadding(FMargin(16)); Background->SetBrushColor(FLinearColor(.025f,.035f,.05f,.85f)); Size->AddChild(Background);
        auto* Column = VitalsBP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(),TEXT("VitalsContent")); Background->AddChild(Column);
        Column->AddChild(DesignerText(VitalsBP->WidgetTree,TEXT("HealthText"),TEXT("HP 75 / 100"),20));
        auto* Progress = VitalsBP->WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(),TEXT("HealthBar")); Progress->bIsVariable = true; Progress->SetFillColorAndOpacity(FLinearColor(.15f,.7f,.45f)); Progress->SetPercent(.75f); Column->AddChild(Progress);
        CompileDesigner(VitalsBP, Errors); BuildVitalsGraph(VitalsBP, Errors); CompileDesigner(VitalsBP, Errors);
        auto* SlotColumn = SlotBP->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(),TEXT("SkillContent")); SlotBP->WidgetTree->RootWidget = SlotColumn;
        DesignerButton(SlotBP->WidgetTree,SlotColumn,ButtonBP->GeneratedClass,TEXT("AbilityButton")); SlotColumn->AddChild(DesignerText(SlotBP->WidgetTree,TEXT("CooldownText"),TEXT("0.8s"),14));
        CompileDesigner(SlotBP, Errors); BuildSlotGraph(SlotBP,ButtonBP->GeneratedClass,Errors); CompileDesigner(SlotBP, Errors);
        auto* BarBackground = BarBP->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(),TEXT("AbilityBackground")); BarBackground->SetPadding(FMargin(12)); BarBackground->SetBrushColor(FLinearColor(.025f,.035f,.05f,.85f)); BarBP->WidgetTree->RootWidget = BarBackground;
        auto* Box = BarBP->WidgetTree->ConstructWidget<UDynamicEntryBox>(UDynamicEntryBox::StaticClass(),TEXT("AbilityEntries")); Box->bIsVariable = true; Box->SetEntrySpacing(FVector2D(12,0)); BarBackground->AddChild(Box);
        FindFProperty<FClassProperty>(Box->GetClass(),TEXT("EntryWidgetClass"))->SetPropertyValue_InContainer(Box,SlotBP->GeneratedClass);
        CompileDesigner(BarBP, Errors); BuildBarGraph(BarBP,SlotBP->GeneratedClass,Errors); CompileDesigner(BarBP, Errors);
        if (auto* P = FindFProperty<FArrayProperty>(BarBP->GeneratedClass,TEXT("Slots"))) { *P->ContainerPtrToValuePtr<TArray<FHodgeHUDAbilitySlot>>(BarBP->GeneratedClass->GetDefaultObject()) = OldSlots; }
        auto* MenuContent = DesignerPanel(MenuBP->WidgetTree,380); MenuContent->AddChild(DesignerText(MenuBP->WidgetTree,TEXT("MenuTitle"),TEXT("Game menu"),24));
        DesignerButton(MenuBP->WidgetTree,MenuContent,ButtonBP->GeneratedClass,TEXT("ResumeButton")); DesignerButton(MenuBP->WidgetTree,MenuContent,ButtonBP->GeneratedClass,TEXT("ConfirmButton"));
        MenuContent->AddChild(DesignerText(MenuBP->WidgetTree,TEXT("BackHint"),TEXT("Esc / B  Back"),14));
        auto* ConfirmContent = DesignerPanel(ConfirmBP->WidgetTree,400); ConfirmContent->AddChild(DesignerText(ConfirmBP->WidgetTree,TEXT("ConfirmationTitle"),TEXT("Return to the game?"),22));
        DesignerButton(ConfirmBP->WidgetTree,ConfirmContent,ButtonBP->GeneratedClass,TEXT("AcceptButton")); DesignerButton(ConfirmBP->WidgetTree,ConfirmContent,ButtonBP->GeneratedClass,TEXT("CancelButton"));
        CompileDesigner(ConfirmBP, Errors); BuildConfirmationGraph(ConfirmBP, Errors); CompileDesigner(ConfirmBP, Errors);
        CompileDesigner(MenuBP, Errors); BuildMenuGraph(MenuBP,ConfirmBP->GeneratedClass,Errors); CompileDesigner(MenuBP, Errors);
        CompileDesigner(RootBP, Errors); CompileDesigner(HUDBP, Errors);
        auto* HUDDefaults = HUDBP->GeneratedClass->GetDefaultObject();
        FindFProperty<FEnumProperty>(HUDDefaults->GetClass(),TEXT("InputConfig"))->GetUnderlyingProperty()->SetIntPropertyValue(FindFProperty<FEnumProperty>(HUDDefaults->GetClass(),TEXT("InputConfig"))->ContainerPtrToValuePtr<void>(HUDDefaults),int64(EHodgeWidgetInputMode::Game));
        FindFProperty<FSoftClassProperty>(HUDDefaults->GetClass(),TEXT("EscapeMenuClass"))->SetPropertyValue_InContainer(HUDDefaults,FSoftObjectPtr(MenuBP->GeneratedClass.Get()));
        for (auto* BP : {MenuBP,ConfirmBP})
        {
            auto* CDO = BP->GeneratedClass->GetDefaultObject(); auto* Mode = FindFProperty<FEnumProperty>(CDO->GetClass(),TEXT("InputConfig")); Mode->GetUnderlyingProperty()->SetIntPropertyValue(Mode->ContainerPtrToValuePtr<void>(CDO),int64(EHodgeWidgetInputMode::Menu));
            FindFProperty<FBoolProperty>(CDO->GetClass(),TEXT("bIsBackHandler"))->SetPropertyValue_InContainer(CDO,true);
        }
        auto SetLabel = [](UWidgetBlueprint* BP, FName Name, const TCHAR* Label)
        { if (auto* Widget = BP->WidgetTree->FindWidget(Name)) { FindFProperty<FTextProperty>(Widget->GetClass(),TEXT("LabelText"))->SetPropertyValue_InContainer(Widget,FText::FromString(Label)); } };
        SetLabel(MenuBP,TEXT("ResumeButton"),TEXT("Continue")); SetLabel(MenuBP,TEXT("ConfirmButton"),TEXT("Continue with confirmation"));
        SetLabel(ConfirmBP,TEXT("AcceptButton"),TEXT("Confirm")); SetLabel(ConfirmBP,TEXT("CancelButton"),TEXT("Cancel"));
        if (Errors.IsEmpty())
        {
            for (auto* BP : {ButtonBP,RootBP,HUDBP,VitalsBP,SlotBP,BarBP,ConfirmBP,MenuBP})
            { BP->GetOutermost()->GetMetaData()->SetValue(BP,TEXT("HodgeUMGVersion"),TEXT("1")); BP->MarkPackageDirty(); if (!Save(BP)) { Errors.Add(TEXT("Save failed: ") + BP->GetName()); } }
        }
    }
    TArray<TSharedPtr<FJsonValue>> Messages; for (const auto& E : Errors) { Messages.Add(MakeShared<FJsonValueString>(E)); }
    Report->SetArrayField(TEXT("errors"),Messages); Report->SetBoolField(TEXT("success"),Errors.IsEmpty()); return Json(Report);
}

/** 配置引用HUD界面。 */
FString UHodgeUIAuthoringLibrary::ConfigureReferenceHUD()
{
    auto Report = MakeShared<FJsonObject>();
    TArray<FString> Errors;
    if (GEditor && GEditor->PlayWorld)
    { Report->SetStringField(TEXT("error"), TEXT("Stop PIE before editing HUD assets")); return Json(Report); }
    auto* HUD = LoadObject<UWidgetBlueprint>(nullptr, TEXT("/Game/Main/UI/HUD/WBP_HodgeHUD.WBP_HodgeHUD"));
    auto* Vitals = LoadObject<UWidgetBlueprint>(nullptr, TEXT("/Game/Main/UI/HUD/WBP_PlayerVitals.WBP_PlayerVitals"));
    auto* Menu = LoadObject<UWidgetBlueprint>(nullptr, TEXT("/Game/Main/UI/Menu/WBP_HodgeEndMenu.WBP_HodgeEndMenu"));
    auto* Icon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Main/UI/Art/T_EndIcon.T_EndIcon"));
    if (!HUD || !Vitals || !Menu || !Icon)
    { Report->SetStringField(TEXT("error"), TEXT("Missing formal HUD, vitals, end menu or icon")); return Json(Report); }
    if (HUD->GetOutermost()->GetMetaData()->GetValue(HUD, TEXT("HodgeReferenceHUD")) == FString(TEXT("1")))
    { Report->SetBoolField(TEXT("alreadyConfigured"), true); return Json(Report); }

    auto* Root = Cast<UOverlay>(HUD->WidgetTree->FindWidget(TEXT("HUDRoot")));
    auto* Point = HUD->WidgetTree->FindWidget(TEXT("PlayerVitalsPoint"));
    auto* Size = Cast<USizeBox>(Vitals->WidgetTree->FindWidget(TEXT("VitalsSize")));
    auto* Background = Cast<UBorder>(Vitals->WidgetTree->FindWidget(TEXT("VitalsBackground")));
    auto* Column = Cast<UVerticalBox>(Vitals->WidgetTree->FindWidget(TEXT("VitalsContent")));
    auto* Bar = Cast<UProgressBar>(Vitals->WidgetTree->FindWidget(TEXT("HealthBar")));
    auto* HealthText = Cast<UTextBlock>(Vitals->WidgetTree->FindWidget(TEXT("HealthText")));
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (!Root || !Point || !Size || !Background || !Column || !Bar || !HealthText)
    { Report->SetStringField(TEXT("error"), TEXT("Formal Designer structure differs from expected HUD")); return Json(Report); }

    HUD->Modify(); Vitals->Modify();
    Root->ClearChildren();
    auto* Canvas = HUD->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("ReferenceHUDCanvas"));
    auto* CanvasSlot = Root->AddChildToOverlay(Canvas);
    CanvasSlot->SetHorizontalAlignment(HAlign_Fill); CanvasSlot->SetVerticalAlignment(VAlign_Fill);
    auto* VitalsSlot = Canvas->AddChildToCanvas(Point);
    VitalsSlot->SetAnchors(FAnchors(.5f, 1.f)); VitalsSlot->SetAlignment(FVector2D(.5f, 1.f));
    VitalsSlot->SetPosition(FVector2D(0, -12)); VitalsSlot->SetAutoSize(true);

    Size->SetWidthOverride(320); Size->SetHeightOverride(20);
    Background->SetPadding(FMargin(0)); Background->SetBrushColor(FLinearColor::Transparent);
    Column->ClearChildren();
    auto* Row = Vitals->WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("HealthRow"));
    Column->AddChild(Row);
    auto* BarSlot = Row->AddChildToOverlay(Bar); BarSlot->SetPadding(FMargin(0, 3, 52, 3));
    BarSlot->SetHorizontalAlignment(HAlign_Fill); BarSlot->SetVerticalAlignment(VAlign_Fill);
    FProgressBarStyle Style = Bar->GetWidgetStyle();
    Style.BackgroundImage.DrawAs = ESlateBrushDrawType::Box; Style.BackgroundImage.Margin = FMargin(0);
    Style.BackgroundImage.TintColor = FSlateColor(FLinearColor(.07f, .09f, .08f, .65f));
    Style.FillImage.DrawAs = ESlateBrushDrawType::Box; Style.FillImage.Margin = FMargin(0);
    Style.FillImage.TintColor = FSlateColor(FLinearColor::White); Style.EnableFillAnimation = false;
    Bar->SetWidgetStyle(Style); Bar->SetFillColorAndOpacity(FLinearColor(.86f, .96f, .68f, 1));
    auto* TextSlot = Row->AddChildToOverlay(HealthText); TextSlot->SetPadding(FMargin(0, 0, 52, 0));
    TextSlot->SetHorizontalAlignment(HAlign_Fill); TextSlot->SetVerticalAlignment(VAlign_Center);
    auto Font = HealthText->GetFont(); Font.Size = 12; HealthText->SetFont(Font);
    HealthText->SetColorAndOpacity(FSlateColor(FLinearColor(.06f, .075f, .06f, 1))); HealthText->SetJustification(ETextJustify::Center);
    HealthText->SetShadowOffset(FVector2D(0, 0));
    auto* Level = DesignerText(Vitals->WidgetTree, TEXT("LevelText"), TEXT("Lv. 1"), 12);
    auto* LevelSlot = Row->AddChildToOverlay(Level); LevelSlot->SetHorizontalAlignment(HAlign_Right); LevelSlot->SetVerticalAlignment(VAlign_Center);
    Level->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, .9f)); Level->SetShadowOffset(FVector2D(1, 1));

    auto* Button = HUD->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("EndButton"));
    Button->bIsVariable = true; Button->SetToolTipText(FText::FromString(TEXT("终焉 · Esc")));
    auto* ButtonSlot = Canvas->AddChildToCanvas(Button);
    ButtonSlot->SetAnchors(FAnchors(1.f, 0.f)); ButtonSlot->SetAlignment(FVector2D(1, 0));
    ButtonSlot->SetPosition(FVector2D(-26, 20)); ButtonSlot->SetSize(FVector2D(64, 70));
    FButtonStyle ButtonStyle = Button->GetStyle();
    for (auto* Brush : {&ButtonStyle.Normal, &ButtonStyle.Hovered, &ButtonStyle.Pressed})
    { Brush->DrawAs = ESlateBrushDrawType::Box; Brush->Margin = FMargin(0); }
    ButtonStyle.Normal.TintColor = FSlateColor(FLinearColor::Transparent);
    ButtonStyle.Hovered.TintColor = FSlateColor(FLinearColor(1, 1, 1, .13f));
    ButtonStyle.Pressed.TintColor = FSlateColor(FLinearColor(.72f, .84f, .65f, .26f));
    Button->SetStyle(ButtonStyle);
    auto* Content = HUD->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("EndButtonContent"));
    Button->AddChild(Content);
    auto* IconSize = HUD->WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("EndIconSize"));
    IconSize->SetWidthOverride(34); IconSize->SetHeightOverride(34);
    Content->AddChildToVerticalBox(IconSize)->SetHorizontalAlignment(HAlign_Center);
    auto* Image = HUD->WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("EndIcon"));
    Image->SetBrushFromTexture(Icon); IconSize->AddChild(Image);
    auto* Label = DesignerText(HUD->WidgetTree, TEXT("EndLabel"), TEXT("终焉"), 16);
    Label->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, .85f)); Label->SetShadowOffset(FVector2D(1, 1));
    auto* LabelSlot = Content->AddChildToVerticalBox(Label); LabelSlot->SetHorizontalAlignment(HAlign_Center); LabelSlot->SetPadding(FMargin(0, 4, 0, 0));
    for (auto* Widget : {static_cast<UWidget*>(Row), static_cast<UWidget*>(HealthText), static_cast<UWidget*>(Level), static_cast<UWidget*>(Content)})
    { Widget->SetVisibility(ESlateVisibility::HitTestInvisible); }

    // 只替换本次血条 Graph；其他页面与能力资产保持原有配置。
    for (UEdGraph* Graph : Vitals->UbergraphPages) { Graph->Nodes.Empty(); }
    CompileDesigner(Vitals, Errors, false); BuildVitalsGraph(Vitals, Errors, true); CompileDesigner(Vitals, Errors, false);
    CompileDesigner(HUD, Errors, false);
    FDesignerGraph Graph(HUD, Errors);
    auto* Click = Graph.Click(TEXT("EndButton"), UButton::StaticClass(), TEXT("OnClicked"));
    auto* Toggle = Graph.Call(UHodgeHUDLayout::StaticClass(), TEXT("ToggleHUDMenu")); Graph.Exec(Click, Toggle);
    CompileDesigner(HUD, Errors, false);
    auto* DefaultHUD = HUD->GeneratedClass->GetDefaultObject();
    FindFProperty<FSoftClassProperty>(DefaultHUD->GetClass(), TEXT("EscapeMenuClass"))->SetPropertyValue_InContainer(DefaultHUD, FSoftObjectPtr(Menu->GeneratedClass.Get()));
    if (Errors.IsEmpty())
    {
        HUD->GetOutermost()->GetMetaData()->SetValue(HUD, TEXT("HodgeReferenceHUD"), TEXT("1"));
        HUD->MarkPackageDirty(); Vitals->MarkPackageDirty();
        if (!Save(HUD) || !Save(Vitals)) { Errors.Add(TEXT("Failed to save reference HUD assets")); }
    }
    TArray<TSharedPtr<FJsonValue>> Messages; for (const auto& Error : Errors) { Messages.Add(MakeShared<FJsonValueString>(Error)); }
    Report->SetArrayField(TEXT("errors"), Messages); Report->SetBoolField(TEXT("success"), Errors.IsEmpty()); return Json(Report);
}

/** 检查DesignerAssets。 */
FString UHodgeUIAuthoringLibrary::InspectDesignerAssets()
{
    auto Report = MakeShared<FJsonObject>(); TArray<TSharedPtr<FJsonValue>> Items;
    for (const FString& Path : {TEXT("Foundation/WBP_HodgePrimaryLayout"),TEXT("HUD/WBP_HodgeHUD"),TEXT("HUD/WBP_PlayerVitals"),TEXT("HUD/WBP_AbilityBar"),TEXT("HUD/WBP_AbilitySlot"),TEXT("Foundation/WBP_UIButton"),TEXT("Menu/WBP_HodgePauseMenu"),TEXT("Menu/WBP_HodgeConfirmation")})
    {
        auto* BP = LoadObject<UWidgetBlueprint>(nullptr, *(TEXT("/Game/Main/UI/") + Path)); auto Item = MakeShared<FJsonObject>(); Item->SetStringField(TEXT("path"),Path);
        Item->SetStringField(TEXT("parent"),BP ? BP->ParentClass->GetPathName() : TEXT("missing")); int32 Count = 0;
        if (BP && BP->WidgetTree) { BP->WidgetTree->ForEachWidget([&Count](UWidget*){++Count;}); }
        Item->SetNumberField(TEXT("widgets"),Count); Item->SetNumberField(TEXT("status"),BP ? int32(BP->Status) : -1);
        int32 Nodes = 0; if (BP) { for (UEdGraph* G : BP->UbergraphPages) { Nodes += G->Nodes.Num(); } for (UEdGraph* G : BP->FunctionGraphs) { Nodes += G->Nodes.Num(); } }
        Item->SetNumberField(TEXT("nodes"),Nodes); Items.Add(MakeShared<FJsonValueObject>(Item));
    }
    Report->SetArrayField(TEXT("assets"),Items); return Json(Report);
}

/** 配置DesignerPreviews。 */
FString UHodgeUIAuthoringLibrary::ConfigureDesignerPreviews()
{
    auto R = MakeShared<FJsonObject>(); R->SetBoolField(TEXT("success"),false);
    if (!GEditor || GEditor->PlayWorld) { return Json(R); }
    for (const FString& Path : {TEXT("HUD/WBP_PlayerVitals"),TEXT("HUD/WBP_AbilityBar"),TEXT("HUD/WBP_AbilitySlot"),TEXT("Foundation/WBP_UIButton")})
    {
        auto* BP = LoadObject<UWidgetBlueprint>(nullptr,*(TEXT("/Game/Main/UI/") + Path));
        if (!BP || !BP->GeneratedClass) { return Json(R); }
        BP->GeneratedClass->GetDefaultObject<UUserWidget>()->DesignSizeMode = EDesignPreviewSizeMode::Desired;
        BP->MarkPackageDirty(); if (!Save(BP)) { return Json(R); }
    }
    auto* Root = LoadObject<UWidgetBlueprint>(nullptr,TEXT("/Game/Main/UI/Foundation/WBP_HodgePrimaryLayout"));
    // 先检查对象与上下文，避免在依赖失效后继续处理。
    if (!Root || !Root->WidgetTree) { return Json(R); }
    Root->WidgetTree->ForEachWidget([](UWidget* Widget)
    { if (auto* Stack = Cast<UCommonActivatableWidgetContainerBase>(Widget)) { Stack->SetTransitionDuration(0.f); } });
    Root->MarkPackageDirty(); if (!Save(Root)) { return Json(R); }
    auto* Confirm = LoadObject<UWidgetBlueprint>(nullptr,TEXT("/Game/Main/UI/Menu/WBP_HodgeConfirmation"));
    if (!Confirm) { return Json(R); }
    if (Confirm->GetOutermost()->GetMetaData()->GetValue(Confirm,TEXT("HodgeInactivePopGuard")) != FString(TEXT("1")))
    {
        TArray<FString> Errors; FDesignerGraph Graph(Confirm,Errors);
        for (UEdGraph* Function : Confirm->FunctionGraphs)
        {
            if (Function->GetFName() != TEXT("Finish")) { continue; }
            Graph.Graph = Function;
            UK2Node_IfThenElse* RootBranch = nullptr;
            for (UEdGraphNode* Node : Function->Nodes)
            {
                auto* Branch = Cast<UK2Node_IfThenElse>(Node);
                if (!Branch) { continue; }
                auto* Condition = Branch->FindPin(UEdGraphSchema_K2::PN_Condition);
                if (Condition && Condition->LinkedTo.Num() && Condition->LinkedTo[0]->GetOwningNode()->IsA<UK2Node_CallFunction>()) { RootBranch = Branch; break; }
            }
            if (RootBranch)
            {
                auto* Condition = RootBranch->FindPin(UEdGraphSchema_K2::PN_Condition); auto* ValidRoot = Condition->LinkedTo[0];
                auto* Active = Graph.Call(UCommonActivatableWidget::StaticClass(),TEXT("IsActivated"));
                auto* Both = Graph.Call(UKismetMathLibrary::StaticClass(),TEXT("BooleanAND"));
                Condition->BreakAllPinLinks(); Graph.Link(ValidRoot,Graph.Pin(Both,TEXT("A"))); Graph.Link(Graph.Return(Active),Graph.Pin(Both,TEXT("B"))); Graph.Link(Graph.Return(Both),Condition);
            }
            else { return Json(R); }
        }
        if (!CompileDesigner(Confirm,Errors) || !Errors.IsEmpty()) { return Json(R); }
        Confirm->GetOutermost()->GetMetaData()->SetValue(Confirm,TEXT("HodgeInactivePopGuard"),TEXT("1"));
        if (!Save(Confirm)) { return Json(R); }
    }
    R->SetBoolField(TEXT("success"),true); return Json(R);
}

/** 只补充既有画布中的 IndicatorLayer，保留用户其他控件与布局。 */
bool UHodgeUIAuthoringLibrary::ConfigureLockOnHUD(UWidgetBlueprint* Blueprint)
{
    if (!Blueprint || !Blueprint->WidgetTree || (GEditor && GEditor->PlayWorld)) { return false; }
    auto* Canvas = Cast<UCanvasPanel>(Blueprint->WidgetTree->FindWidget(TEXT("ReferenceHUDCanvas")));
    if (!Canvas) { return false; }
    auto* Existing = Blueprint->WidgetTree->FindWidget(TEXT("LockOnIndicatorLayer"));
    if (Existing) { return Existing->IsA<UIndicatorLayer>() && Existing->GetParent() == Canvas; }
    Blueprint->Modify();
    auto* Layer = Blueprint->WidgetTree->ConstructWidget<UIndicatorLayer>(UIndicatorLayer::StaticClass(),TEXT("LockOnIndicatorLayer"));
    auto* Slot = Canvas->AddChildToCanvas(Layer); Slot->SetAnchors(FAnchors(0.f,0.f,1.f,1.f)); Slot->SetOffsets(FMargin(0)); Slot->SetZOrder(-1);
    FCompilerResultsLog Results; FKismetEditorUtilities::CompileBlueprint(Blueprint,EBlueprintCompileOptions::None,&Results);
    Blueprint->MarkPackageDirty(); return Results.NumErrors == 0;
}

bool UHodgeUIAuthoringLibrary::ConfigureLockOnIndicator(UWidgetBlueprint* Blueprint)
{
    if (!Blueprint || !Blueprint->ParentClass || !Blueprint->ParentClass->IsChildOf(UHodgeLockOnIndicatorWidget::StaticClass()) || !Blueprint->WidgetTree || (GEditor && GEditor->PlayWorld)) { return false; }
    if (Blueprint->WidgetTree->RootWidget) { return true; }
    Blueprint->Modify();
    auto* Box = Blueprint->WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(),TEXT("MarkerSize"));
    Box->SetWidthOverride(36.f); Box->SetHeightOverride(36.f);
    auto* Text = Blueprint->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),TEXT("MarkerGlyph"));
    Text->SetText(FText::FromString(TEXT("◇"))); Text->SetJustification(ETextJustify::Center);
    auto Font = Text->GetFont(); Font.Size = 26; Font.TypefaceFontName = TEXT("Bold"); Text->SetFont(Font);
    Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    auto* Slot = Cast<USizeBoxSlot>(Box->AddChild(Text));
    Slot->SetHorizontalAlignment(HAlign_Center); Slot->SetVerticalAlignment(VAlign_Center);
    Blueprint->WidgetTree->RootWidget = Box;
    FCompilerResultsLog Results; FKismetEditorUtilities::CompileBlueprint(Blueprint,EBlueprintCompileOptions::None,&Results);
    Blueprint->MarkPackageDirty(); return Results.NumErrors == 0;
}
