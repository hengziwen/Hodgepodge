#include "HodgeAbilityEditorToolkit.h"
#include "SHodgeAbilityPreview.h"
#include "SHodgeAbilityTimeline.h"
#include "AbilitySystem/HodgeTimelineEvaluator.h"
#include "AbilitySystem/HodgeGameplayTags.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Animation/AnimMontage.h"
#include "Engine/SkeletalMesh.h"
#include "Editor.h"
#include "Framework/Commands/GenericCommands.h"
#include "ScopedTransaction.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "IStructureDetailsView.h"
#include "UObject/StructOnScope.h"
#include "UObject/Package.h"
#include "Misc/PackageName.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "HodgeAbilityEditor"
namespace { const FName DefinitionTab("HodgeAbilityEditor_Definition"), PreviewTab("HodgeAbilityEditor_Preview"), TimelineTab("HodgeAbilityEditor_Timeline"), TasksTab("HodgeAbilityEditor_Tasks"), SelectionTab("HodgeAbilityEditor_Selection"), ValidationTab("HodgeAbilityEditor_Validation"); }

FHodgeAbilityEditorToolkit::~FHodgeAbilityEditorToolkit()
{
    bPlaying=false;
    if(EventDetails){EventDetails->GetOnFinishedChangingPropertiesDelegate().RemoveAll(this);}
    FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(PropertyChangedHandle);
    if (GEditor) { GEditor->UnregisterForUndo(this); }
}
FText FHodgeAbilityEditorToolkit::GetBaseToolkitName() const { return LOCTEXT("Name","Hodge Ability Editor"); }
FText FHodgeAbilityEditorToolkit::GetToolkitName() const { return Definition ? GetLabelForObject(Definition) : GetBaseToolkitName(); }
FText FHodgeAbilityEditorToolkit::GetToolkitToolTipText() const { return Definition ? GetToolTipTextForObject(Definition) : GetBaseToolkitName(); }
void FHodgeAbilityEditorToolkit::Init(UHodgeAbilityDefinition* InDefinition,const TSharedPtr<IToolkitHost>& Host)
{
    Definition=InDefinition;
    Definition->SetFlags(RF_Transactional);
    Timeline=Definition->ExecutionConfig.TimelineTaskConfig.Timeline;
    if (Timeline) { Timeline->SetFlags(RF_Transactional); }
    auto& Properties=FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsArgs;
    DetailsArgs.bHideSelectionTip=true;
    DefinitionDetails=Properties.CreateDetailView(DetailsArgs);
    DefinitionDetails->SetObject(Definition);
    TimelineDetails=Properties.CreateDetailView(DetailsArgs);
    TimelineDetails->SetObject(Timeline);
    EventDetails=Properties.CreateStructureDetailView(DetailsArgs,FStructureDetailsViewArgs(),nullptr);
    EventDetails->GetOnFinishedChangingPropertiesDelegate().AddRaw(this,&FHodgeAbilityEditorToolkit::EventEdited);
    const auto Layout=FTabManager::NewLayout("HodgeAbilityEditor_Layout_v2")
        ->AddArea(FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
            ->Split(FTabManager::NewStack()->SetSizeCoefficient(.28f)->AddTab(DefinitionTab,ETabState::OpenedTab))
            ->Split(FTabManager::NewSplitter()->SetSizeCoefficient(.42f)->SetOrientation(Orient_Vertical)
                ->Split(FTabManager::NewStack()->SetSizeCoefficient(.55f)->AddTab(PreviewTab,ETabState::OpenedTab))
                ->Split(FTabManager::NewStack()->SetSizeCoefficient(.45f)->AddTab(TimelineTab,ETabState::OpenedTab)))
            ->Split(FTabManager::NewStack()->SetSizeCoefficient(.30f)
                ->AddTab(TasksTab,ETabState::OpenedTab)->AddTab(SelectionTab,ETabState::OpenedTab)
                ->AddTab(ValidationTab,ETabState::OpenedTab)->SetForegroundTab(TasksTab)));
    InitAssetEditor(Host ? EToolkitMode::WorldCentric : EToolkitMode::Standalone,Host,"HodgeAbilityEditorApp",
        Layout,true,true,Definition);
    if (Timeline) { AddEditingObject(Timeline); }
    GetToolkitCommands()->MapAction(FGenericCommands::Get().Undo, FExecuteAction::CreateLambda([] { GEditor->UndoTransaction(); }));
    GetToolkitCommands()->MapAction(FGenericCommands::Get().Redo, FExecuteAction::CreateLambda([] { GEditor->RedoTransaction(); }));
    GEditor->RegisterForUndo(this);
    PropertyChangedHandle=FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this,&FHodgeAbilityEditorToolkit::ObjectChanged);
    Refresh();
}
void FHodgeAbilityEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& Manager)
{
    FAssetEditorToolkit::RegisterTabSpawners(Manager);
    const TPair<FName,FText> Tabs[] = {
        {DefinitionTab,LOCTEXT("Definition","Ability Definition")}, {PreviewTab,LOCTEXT("Preview","Preview")},
        {TimelineTab,LOCTEXT("Timeline","Timeline")}, {TasksTab,LOCTEXT("Tasks","Ability Tasks")},
        {SelectionTab,LOCTEXT("Selection","Selected Event")}, {ValidationTab,LOCTEXT("Validation","Validation")}};
    for(const auto& Tab:Tabs)
    {
        Manager->RegisterTabSpawner(Tab.Key,FOnSpawnTab::CreateSP(this,&FHodgeAbilityEditorToolkit::SpawnMain)).SetDisplayName(Tab.Value);
    }
}
void FHodgeAbilityEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& Manager)
{
    for(FName Tab:{DefinitionTab,PreviewTab,TimelineTab,TasksTab,SelectionTab,ValidationTab}) { Manager->UnregisterTabSpawner(Tab); }
    FAssetEditorToolkit::UnregisterTabSpawners(Manager);
}
TSharedRef<SDockTab> FHodgeAbilityEditorToolkit::SpawnMain(const FSpawnTabArgs& Args)
{
    auto Self=StaticCastSharedRef<FHodgeAbilityEditorToolkit>(AsShared());
    const FName Tab=Args.GetTabId().TabType;
    auto Button=[](const FText& Text,FOnClicked Click) { return SNew(SButton).Text(Text).OnClicked(Click); };
    TSharedRef<SDockTab> Result=SNew(SDockTab).TabRole(ETabRole::PanelTab);
    if(Tab==DefinitionTab)
    {
        Result->SetContent(SNew(SVerticalBox)
            +SVerticalBox::Slot().AutoHeight().Padding(4)[Button(LOCTEXT("Unique","Create / Copy Timeline Config"),FOnClicked::CreateSP(this,&FHodgeAbilityEditorToolkit::MakeUniqueTimeline))]
            +SVerticalBox::Slot().FillHeight(1)[DefinitionDetails.ToSharedRef()]);
    }
    else if(Tab==TasksTab) { Result->SetContent(TimelineDetails.ToSharedRef()); }
    else if(Tab==SelectionTab) { Result->SetContent(EventDetails->GetWidget().ToSharedRef()); }
    else if(Tab==ValidationTab)
    {
        Result->SetContent(SNew(SVerticalBox)
            +SVerticalBox::Slot().AutoHeight().Padding(4)[Button(LOCTEXT("Validate","Validate"),FOnClicked::CreateSP(this,&FHodgeAbilityEditorToolkit::Validate))]
            +SVerticalBox::Slot().AutoHeight().Padding(8)[SNew(STextBlock).AutoWrapText(true).Text(this,&FHodgeAbilityEditorToolkit::StatusText)]
            +SVerticalBox::Slot().AutoHeight().Padding(8)[SNew(STextBlock).AutoWrapText(true).Text(LOCTEXT("Shared","Edits affect the referenced Timeline asset. Use Create / Copy Timeline Config to make a separate copy."))]);
    }
    else if(Tab==PreviewTab)
    {
        SAssignNew(Preview,SHodgeAbilityPreview,Self);
        Preview->SetMontage(Definition?Definition->ExecutionConfig.Montage.Get():nullptr);
        Preview->SetTime(Time);
        Result->SetContent(SNew(SVerticalBox)
            +SVerticalBox::Slot().AutoHeight().Padding(4)
            [SNew(SHorizontalBox)
                +SHorizontalBox::Slot().AutoWidth()[SNew(SButton).Text_Lambda([this]{return bPlaying?LOCTEXT("Pause","Pause"):LOCTEXT("Play","Play");}).OnClicked(this,&FHodgeAbilityEditorToolkit::TogglePlay)]
                +SHorizontalBox::Slot().AutoWidth()[Button(LOCTEXT("Step","Step"),FOnClicked::CreateLambda([this]{Seek(Time+1.f/30.f);return FReply::Handled();}))]
                +SHorizontalBox::Slot().AutoWidth()[Button(LOCTEXT("Reset","Reset"),FOnClicked::CreateLambda([this]{Seek(0);return FReply::Handled();}))]
                +SHorizontalBox::Slot().AutoWidth().Padding(8,0)[SNew(SBox).WidthOverride(95)[SNew(SSpinBox<float>).MinValue(0.f).MaxValue_Lambda([this]{return Duration();}).Value_Lambda([this]{return Time;}).OnValueChanged_Lambda([this](float T){Seek(T);})]]]
            +SVerticalBox::Slot().AutoHeight().Padding(4)[SNew(SObjectPropertyEntryBox).AllowedClass(USkeletalMesh::StaticClass()).ObjectPath_Lambda([this]{return Preview?Preview->GetMeshPath():FString();}).OnObjectChanged_Lambda([this](const FAssetData& Asset){if(Preview){Preview->SetMesh(Cast<USkeletalMesh>(Asset.GetAsset()));Seek(Time);}})]
            +SVerticalBox::Slot().FillHeight(1)[Preview.ToSharedRef()]);
    }
    else if(Tab==TimelineTab)
    {
        Result->SetContent(SNew(SVerticalBox)
            +SVerticalBox::Slot().AutoHeight().Padding(4)
            [SNew(SHorizontalBox)
                +SHorizontalBox::Slot().AutoWidth()[Button(LOCTEXT("Point","Add Point"),FOnClicked::CreateLambda([this]{return AddEvent(false);}))]
                +SHorizontalBox::Slot().AutoWidth()[Button(LOCTEXT("Window","Add Window"),FOnClicked::CreateLambda([this]{return AddEvent(true);}))]
                +SHorizontalBox::Slot().AutoWidth()[Button(LOCTEXT("Duplicate","Duplicate"),FOnClicked::CreateSP(this,&FHodgeAbilityEditorToolkit::DuplicateEvent))]
                +SHorizontalBox::Slot().AutoWidth()[Button(LOCTEXT("Delete","Delete"),FOnClicked::CreateSP(this,&FHodgeAbilityEditorToolkit::DeleteEvent))]]
            +SVerticalBox::Slot().AutoHeight().Padding(4)
            [SNew(SCheckBox).IsChecked_Lambda([this]{return bSnap?ECheckBoxState::Checked:ECheckBoxState::Unchecked;}).OnCheckStateChanged_Lambda([this](ECheckBoxState S){bSnap=S==ECheckBoxState::Checked;})[SNew(STextBlock).Text(LOCTEXT("Snap","Snap 30 fps | Ctrl+wheel: zoom | Shift+wheel: pan"))]]
            +SVerticalBox::Slot().FillHeight(1)[SNew(SScrollBox)+SScrollBox::Slot()[SNew(SHodgeAbilityTimeline,Self)]]
            +SVerticalBox::Slot().AutoHeight().Padding(4)[SNew(STextBlock).Text_Lambda([this]{return FText::FromString(FString::Printf(TEXT("%.3f / %.3f s   |   %d active windows"),Time,Duration(),ActiveWindows.Num()));})]);
    }
    return Result;
}
void FHodgeAbilityEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
    Collector.AddReferencedObject(Definition);
    Collector.AddReferencedObject(Timeline);
    if (EventCopy) { EventCopy->AddReferencedObjects(Collector); }
}
void FHodgeAbilityEditorToolkit::GetSaveableObjects(TArray<UObject*>& Objects) const
{
    FAssetEditorToolkit::GetSaveableObjects(Objects);
    if (Definition) { Objects.AddUnique(Definition); }
    if (Timeline) { Objects.AddUnique(Timeline); }
}
void FHodgeAbilityEditorToolkit::SaveAsset_Execute()
{
    Validate();
    TArray<FText> Errors;
    if (Definition && Definition->ValidateDefinition(Errors)) { FAssetEditorToolkit::SaveAsset_Execute(); }
}
float FHodgeAbilityEditorToolkit::Duration() const { return Definition?FMath::Max(0.f,Definition->GetDuration()):0.f; }
float FHodgeAbilityEditorToolkit::Snap(float Value) const { return bSnap ? FMath::RoundToFloat(Value*30.f)/30.f : Value; }
int32 FHodgeAbilityEditorToolkit::SelectedIndex() const
{
    return Timeline ? Timeline->Events.IndexOfByPredicate([this](const auto& E){return E.EventID==SelectedID;}) : INDEX_NONE;
}
void FHodgeAbilityEditorToolkit::Select(int32 Index)
{
    SelectedID=Timeline && Timeline->Events.IsValidIndex(Index)?Timeline->Events[Index].EventID:NAME_None;
    RebuildEventDetails();
    if(TabManager){TabManager->TryInvokeTab(SelectionTab);}
}
void FHodgeAbilityEditorToolkit::RebuildEventDetails()
{
    EventCopy.Reset();
    const int32 Index=SelectedIndex();
    if (Timeline && Timeline->Events.IsValidIndex(Index))
    {
        EventCopy=MakeShared<FStructOnScope>(FHodgeTimelineEvent::StaticStruct());
        *reinterpret_cast<FHodgeTimelineEvent*>(EventCopy->GetStructMemory())=Timeline->Events[Index];
    }
    if (EventDetails) { EventDetails->SetStructureData(EventCopy); }
}
void FHodgeAbilityEditorToolkit::EventEdited(const FPropertyChangedEvent& Event)
{
    const int32 Index=SelectedIndex();
    if (bApplyingEdit || !EventCopy || !Timeline || !Timeline->Events.IsValidIndex(Index)) { return; }
    const FScopedTransaction Transaction(LOCTEXT("EditEvent","Edit timeline event"));
    Timeline->Modify();
    Timeline->Events[Index]=*reinterpret_cast<FHodgeTimelineEvent*>(EventCopy->GetStructMemory());
    SelectedID=Timeline->Events[Index].EventID;
    CommitTimeline();
}
void FHodgeAbilityEditorToolkit::CommitTimeline()
{
    if (!Timeline) { return; }
    TGuardValue<bool> Guard(bApplyingEdit,true);
    Timeline->MarkPackageDirty();
    Timeline->PostEditChange();
    if(TimelineDetails){TimelineDetails->ForceRefresh();}
    Refresh();
}
void FHodgeAbilityEditorToolkit::SyncTimeline()
{
    auto* NewTimeline=Definition?Definition->ExecutionConfig.TimelineTaskConfig.Timeline.Get():nullptr;
    if (NewTimeline!=Timeline)
    {
        Timeline=NewTimeline; SelectedID=NAME_None;
        if(TimelineDetails){TimelineDetails->SetObject(Timeline);}
        if (Timeline) { Timeline->SetFlags(RF_Transactional); AddEditingObject(Timeline); }
    }
}
void FHodgeAbilityEditorToolkit::ObjectChanged(UObject* Object,FPropertyChangedEvent& Event)
{
    if (!bApplyingEdit && (Object==Definition || Object==Timeline)) { Refresh(); }
}
void FHodgeAbilityEditorToolkit::Refresh()
{
    bPlaying=false;
    SyncTimeline();
    EventLog.Reset();
    if (Preview) { Preview->SetMontage(Definition?Definition->ExecutionConfig.Montage.Get():nullptr); }
    RebuildEventDetails();
    Seek(Time);
    Validate();
}
void FHodgeAbilityEditorToolkit::PostUndo(bool bSuccess) { if(bSuccess){Refresh();if(DefinitionDetails){DefinitionDetails->ForceRefresh();}} }
void FHodgeAbilityEditorToolkit::Seek(float Value)
{
    bPlaying=false;
    Time=FMath::Clamp(Value,0.f,Duration());
    ActiveWindows.Reset();
    if (Timeline) { FHodgeTimelineEvaluator::EvaluateAt(Timeline->Events,Duration(),Time,ActiveWindows); }
    if (Preview) { Preview->SetPlaying(false); Preview->SetTime(Time); }
}
FReply FHodgeAbilityEditorToolkit::TogglePlay()
{
    if (bPlaying) { Seek(Time); return FReply::Handled(); }
    TArray<FText> Errors;
    if (!Definition || !Definition->ValidateDefinition(Errors)) { Validate(); return FReply::Handled(); }
    if (Time>=Duration()) { Seek(0.f); }
    if (Time==0.f)
    {
        EventLog.Reset();
        TArray<FHodgeTimelineNode> Nodes;
        FHodgeTimelineEvaluator::Collect(Timeline->Events,Duration(),0.f,0.f,true,Nodes);
        FHodgeTimelineEvaluator::Sort(Timeline->Events,Nodes);
        for (const auto& Node:Nodes) { EventLog.Add(FString::Printf(TEXT("0.000 %s"),*Timeline->Events[Node.EventIndex].EventID.ToString())); }
    }
    bPlaying=true;
    if(Preview){Preview->SetPlaying(true);}
    return FReply::Handled();
}
void FHodgeAbilityEditorToolkit::TickPreview(float Delta)
{
    if (!bPlaying || !Timeline || !Definition || !Definition->ExecutionConfig.Montage || !FMath::IsFinite(Delta) || Delta<=0.f) { return; }
    const float Rate=Definition->ExecutionConfig.PlayRate*Definition->ExecutionConfig.Montage->RateScale;
    const float Next=FMath::Min(Time+Delta*Rate,Duration());
    TArray<FHodgeTimelineNode> Nodes;
    FHodgeTimelineEvaluator::EvaluateRange(Timeline->Events,Duration(),Time,Next,Nodes);
    for (const auto& Node:Nodes)
    {
        const TCHAR* Kind=Node.Kind==EHodgeTimelineNodeKind::WindowBegin?TEXT("Begin"):Node.Kind==EHodgeTimelineNodeKind::WindowEnd?TEXT("End"):TEXT("Point");
        EventLog.Add(FString::Printf(TEXT("%.3f %s %s"),Node.Time,Kind,*Timeline->Events[Node.EventIndex].EventID.ToString()));
    }
    if(EventLog.Num()>6){EventLog.RemoveAt(0,EventLog.Num()-6);}
    Time=Next;
    FHodgeTimelineEvaluator::EvaluateAt(Timeline->Events,Duration(),Time,ActiveWindows);
    if(Preview){Preview->SetTime(Time);}
    if(Time>=Duration()){bPlaying=false;if(Preview){Preview->SetPlaying(false);}}
}
FReply FHodgeAbilityEditorToolkit::Validate()
{
    TArray<FText> Errors;
    if(Definition){Definition->ValidateDefinition(Errors);}
    ValidationMessage=Errors.IsEmpty()?TEXT("Valid. Preview: pose and timeline only; no GAS effects, animation notifies or root-motion movement."):TEXT("Invalid: ");
    for(const auto& Error:Errors){ValidationMessage+=Error.ToString()+TEXT("; ");}
    return FReply::Handled();
}
FText FHodgeAbilityEditorToolkit::StatusText() const
{
    return FText::FromString(FString::Printf(TEXT("%.3f / %.3f s | %d active windows\n%s\n%s"),Time,Duration(),ActiveWindows.Num(),*ValidationMessage,*FString::Join(EventLog,TEXT(" | "))));
}
FReply FHodgeAbilityEditorToolkit::AddEvent(bool bWindow)
{
    if(!Timeline || Duration()<=.002f){return FReply::Handled();}
    FScopedTransaction Transaction(LOCTEXT("AddEvent","Add timeline event"));
    Timeline->Modify();
    FHodgeTimelineEvent Event;
    int32 Suffix=1;
    do{Event.EventID=FName(*FString::Printf(TEXT("%s_%d"),bWindow?TEXT("Window"):TEXT("Point"),Suffix++));}
    while(Timeline->Events.ContainsByPredicate([&](const auto& E){return E.EventID==Event.EventID;}));
    Event.Kind=bWindow?EHodgeTimelineEventKind::Window:EHodgeTimelineEventKind::Point;
    Event.StartTime=FMath::Clamp(Snap(Time),0.f,bWindow?FMath::Max(0.f,Duration()-.1f):Duration());
    Event.EndTime=FMath::Min(Event.StartTime+.3f,Duration());
    Event.WindowTag=HodgeGameplayTags::Status_Attack_Cancel_Move;
    Event.PointEventTag=HodgeGameplayTags::GameplayEvent_Attack_Test;
    Timeline->Events.Add(Event);SelectedID=Event.EventID;CommitTimeline();
    return FReply::Handled();
}
FReply FHodgeAbilityEditorToolkit::DuplicateEvent()
{
    const int32 Index=SelectedIndex();
    if(!Timeline || !Timeline->Events.IsValidIndex(Index)){return FReply::Handled();}
    FScopedTransaction Transaction(LOCTEXT("DuplicateEvent","Duplicate timeline event"));
    Timeline->Modify();
    auto Copy=Timeline->Events[Index];
    const FString Base=Copy.EventID.ToString();
    int32 Suffix=1;
    do{Copy.EventID=FName(*FString::Printf(TEXT("%s_Copy%d"),*Base,Suffix++));}
    while(Timeline->Events.ContainsByPredicate([&](const auto& Event){return Event.EventID==Copy.EventID;}));
    Timeline->Events.Add(Copy);SelectedID=Copy.EventID;CommitTimeline();
    return FReply::Handled();
}
FReply FHodgeAbilityEditorToolkit::DeleteEvent()
{
    const int32 Index=SelectedIndex();
    if(Timeline && Timeline->Events.IsValidIndex(Index))
    {
        FScopedTransaction Transaction(LOCTEXT("DeleteEvent","Delete timeline event"));
        Timeline->Modify();Timeline->Events.RemoveAt(Index);SelectedID=NAME_None;CommitTimeline();
    }
    return FReply::Handled();
}
FReply FHodgeAbilityEditorToolkit::MakeUniqueTimeline()
{
    if(!Definition){return FReply::Handled();}
    auto& Tools=FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    FString PackageName,AssetName;
    Tools.CreateUniqueAssetName(Definition->GetOutermost()->GetName()+TEXT("_Timeline"),TEXT(""),PackageName,AssetName);
    UHodgeAbilityTimeline* NewTimeline=nullptr;
    if(Timeline){NewTimeline=Cast<UHodgeAbilityTimeline>(Tools.DuplicateAsset(AssetName,FPackageName::GetLongPackagePath(PackageName),Timeline));}
    else
    {
        NewTimeline=NewObject<UHodgeAbilityTimeline>(CreatePackage(*PackageName),*AssetName,RF_Public|RF_Standalone|RF_Transactional);
        FAssetRegistryModule::AssetCreated(NewTimeline);
    }
    if(NewTimeline)
    {
        NewTimeline->bUseMontageDuration=true;NewTimeline->MarkPackageDirty();
        FScopedTransaction Transaction(LOCTEXT("AssignTimeline","Assign dedicated timeline"));
        Definition->Modify();Definition->ExecutionConfig.TimelineTaskConfig.Timeline=NewTimeline;
        Definition->MarkPackageDirty();Definition->PostEditChange();Refresh();
    }
    return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE
