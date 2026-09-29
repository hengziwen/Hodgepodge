#pragma once
#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "EditorUndoClient.h"
#include "UObject/GCObject.h"
#include "Data/HodgeAbilityTimeline.h"
class UHodgeAbilityDefinition;
class USkeletalMesh;
class IDetailsView;
class IStructureDetailsView;
class FStructOnScope;
class SHodgeAbilityPreview;
class FHodgeAbilityEditorToolkit : public FAssetEditorToolkit, public FGCObject, public FEditorUndoClient
{
public:
    virtual ~FHodgeAbilityEditorToolkit() override;
    void Init(UHodgeAbilityDefinition* InDefinition, const TSharedPtr<IToolkitHost>& Host);
    virtual FName GetToolkitFName() const override { return "HodgeAbilityEditor"; }
    virtual FText GetBaseToolkitName() const override;
    virtual FText GetToolkitName() const override;
    virtual FText GetToolkitToolTipText() const override;
    virtual FString GetWorldCentricTabPrefix() const override { return TEXT("Hodge Ability"); }
    virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor(.1f,.5f,.6f); }
    virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& Manager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& Manager) override;
    virtual void GetSaveableObjects(TArray<UObject*>& Objects) const override;
    virtual void SaveAsset_Execute() override;
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override { return TEXT("FHodgeAbilityEditorToolkit"); }
    virtual void PostUndo(bool bSuccess) override;
    virtual void PostRedo(bool bSuccess) override { PostUndo(bSuccess); }
    TObjectPtr<UHodgeAbilityDefinition> Definition = nullptr;
    TObjectPtr<UHodgeAbilityTimeline> Timeline = nullptr;
    float Time = 0.f;
    bool bPlaying = false;
    bool bSnap = true;
    FName SelectedID;
    TArray<int32> ActiveWindows;
    float Duration() const;
    int32 SelectedIndex() const;
    void Select(int32 Index);
    void Seek(float Value);
    void TickPreview(float Delta);
    void Refresh();
    void CommitTimeline();
    float Snap(float Value) const;
    FReply AddEvent(bool bWindow);
    FReply DeleteEvent();
    FReply DuplicateEvent();
    FReply TogglePlay();
    FReply Validate();
    FReply MakeUniqueTimeline();
    FText StatusText() const;
private:
    friend class FHodgeAbilityEditorTest;
    TSharedRef<SDockTab> SpawnMain(const FSpawnTabArgs& Args);
    void RebuildEventDetails();
    void EventEdited(const FPropertyChangedEvent& Event);
    void ObjectChanged(UObject* Object, FPropertyChangedEvent& Event);
    void SyncTimeline();
    TSharedPtr<IDetailsView> DefinitionDetails;
    TSharedPtr<IDetailsView> TimelineDetails;
    TSharedPtr<IStructureDetailsView> EventDetails;
    TSharedPtr<FStructOnScope> EventCopy;
    TSharedPtr<SHodgeAbilityPreview> Preview;
    TArray<FString> EventLog;
    FString ValidationMessage;
    FDelegateHandle PropertyChangedHandle;
    bool bApplyingEdit = false;
};
