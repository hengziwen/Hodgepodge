# Module 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityEditor.Build.cs

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/HodgeAbilityEditor.Build.cs](../../../Source/HodgeAbilityEditor/HodgeAbilityEditor.Build.cs)

定义候选（多行签名仅展示首行）：


## HodgeAbilityEditorModule.cpp

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeAbilityEditorModule.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorModule.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：


## HodgeAbilityEditorTests.cpp

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeAbilityEditorTests.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorTests.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：

- L11: `bool FHodgeAbilityEditorTest::RunTest(const FString& Parameters)`

## HodgeAbilityEditorToolkit.cpp

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeAbilityEditorToolkit.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorToolkit.cpp)

项目内直接 include（不是运行调用关系）：[AbilitySystem/HodgeTimelineEvaluator.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeTimelineEvaluator.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：

- L33: `FHodgeAbilityEditorToolkit::~FHodgeAbilityEditorToolkit()`
- L40: `FText FHodgeAbilityEditorToolkit::GetBaseToolkitName() const { return LOCTEXT("Name","Hodge Ability Editor"); }`
- L41: `FText FHodgeAbilityEditorToolkit::GetToolkitName() const { return Definition ? GetLabelForObject(Definition) : GetBaseToolkitName(); }`
- L42: `FText FHodgeAbilityEditorToolkit::GetToolkitToolTipText() const { return Definition ? GetToolTipTextForObject(Definition) : GetBaseToolkitName(); }`
- L43: `void FHodgeAbilityEditorToolkit::Init(UHodgeAbilityDefinition* InDefinition,const TSharedPtr<IToolkitHost>& Host)`
- L76: `void FHodgeAbilityEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& Manager)`
- L88: `void FHodgeAbilityEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& Manager)`
- L93: `TSharedRef<SDockTab> FHodgeAbilityEditorToolkit::SpawnMain(const FSpawnTabArgs& Args)`
- L145: `void FHodgeAbilityEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)`
- L151: `void FHodgeAbilityEditorToolkit::GetSaveableObjects(TArray<UObject*>& Objects) const`
- L157: `void FHodgeAbilityEditorToolkit::SaveAsset_Execute()`
- L163: `float FHodgeAbilityEditorToolkit::Duration() const { return Definition?FMath::Max(0.f,Definition->GetDuration()):0.f; }`
- L164: `float FHodgeAbilityEditorToolkit::Snap(float Value) const { return bSnap ? FMath::RoundToFloat(Value*30.f)/30.f : Value; }`
- L165: `int32 FHodgeAbilityEditorToolkit::SelectedIndex() const`
- L169: `void FHodgeAbilityEditorToolkit::Select(int32 Index)`
- L175: `void FHodgeAbilityEditorToolkit::RebuildEventDetails()`
- L186: `void FHodgeAbilityEditorToolkit::EventEdited(const FPropertyChangedEvent& Event)`
- L196: `void FHodgeAbilityEditorToolkit::CommitTimeline()`
- L205: `void FHodgeAbilityEditorToolkit::SyncTimeline()`
- L215: `void FHodgeAbilityEditorToolkit::ObjectChanged(UObject* Object,FPropertyChangedEvent& Event)`
- L219: `void FHodgeAbilityEditorToolkit::Refresh()`
- L229: `void FHodgeAbilityEditorToolkit::PostUndo(bool bSuccess) { if(bSuccess){Refresh();if(DefinitionDetails){DefinitionDetails->ForceRefresh();}} }`
- L230: `void FHodgeAbilityEditorToolkit::Seek(float Value)`
- L238: `FReply FHodgeAbilityEditorToolkit::TogglePlay()`
- L256: `void FHodgeAbilityEditorToolkit::TickPreview(float Delta)`
- L274: `FReply FHodgeAbilityEditorToolkit::Validate()`
- L282: `FText FHodgeAbilityEditorToolkit::StatusText() const`
- L286: `FReply FHodgeAbilityEditorToolkit::AddEvent(bool bWindow)`
- L303: `FReply FHodgeAbilityEditorToolkit::DuplicateEvent()`
- L317: `FReply FHodgeAbilityEditorToolkit::DeleteEvent()`
- L327: `FReply FHodgeAbilityEditorToolkit::MakeUniqueTimeline()`

## HodgeAbilityEditorToolkit.h

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeAbilityEditorToolkit.h](../../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorToolkit.h)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   2: #include "CoreMinimal.h"
   3: #include "Toolkits/AssetEditorToolkit.h"
   4: #include "EditorUndoClient.h"
   5: #include "UObject/GCObject.h"
   6: #include "Data/HodgeAbilityTimeline.h"
   7: class UHodgeAbilityDefinition;
   8: class USkeletalMesh;
   9: class IDetailsView;
  10: class IStructureDetailsView;
  11: class FStructOnScope;
  12: class SHodgeAbilityPreview;
  13: class FHodgeAbilityEditorToolkit : public FAssetEditorToolkit, public FGCObject, public FEditorUndoClient
  14: {
  15: public:
  16:     virtual ~FHodgeAbilityEditorToolkit() override;
  17:     void Init(UHodgeAbilityDefinition* InDefinition, const TSharedPtr<IToolkitHost>& Host);
  18:     virtual FName GetToolkitFName() const override { return "HodgeAbilityEditor"; }
  19:     virtual FText GetBaseToolkitName() const override;
  20:     virtual FText GetToolkitName() const override;
  21:     virtual FText GetToolkitToolTipText() const override;
  22:     virtual FString GetWorldCentricTabPrefix() const override { return TEXT("Hodge Ability"); }
  23:     virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor(.1f,.5f,.6f); }
  24:     virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& Manager) override;
  25:     virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& Manager) override;
  26:     virtual void GetSaveableObjects(TArray<UObject*>& Objects) const override;
  27:     virtual void SaveAsset_Execute() override;
  28:     virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
  29:     virtual FString GetReferencerName() const override { return TEXT("FHodgeAbilityEditorToolkit"); }
  30:     virtual void PostUndo(bool bSuccess) override;
  31:     virtual void PostRedo(bool bSuccess) override { PostUndo(bSuccess); }
  32:     TObjectPtr<UHodgeAbilityDefinition> Definition = nullptr;
  33:     TObjectPtr<UHodgeAbilityTimeline> Timeline = nullptr;
  34:     float Time = 0.f;
  35:     bool bPlaying = false;
  36:     bool bSnap = true;
  37:     FName SelectedID;
  38:     TArray<int32> ActiveWindows;
  39:     float Duration() const;
  40:     int32 SelectedIndex() const;
  41:     void Select(int32 Index);
  42:     void Seek(float Value);
  43:     void TickPreview(float Delta);
  44:     void Refresh();
  45:     void CommitTimeline();
  46:     float Snap(float Value) const;
  47:     FReply AddEvent(bool bWindow);
  48:     FReply DeleteEvent();
  49:     FReply DuplicateEvent();
  50:     FReply TogglePlay();
  51:     FReply Validate();
  52:     FReply MakeUniqueTimeline();
  53:     FText StatusText() const;
  54: private:
  55:     friend class FHodgeAbilityEditorTest;
  56:     TSharedRef<SDockTab> SpawnMain(const FSpawnTabArgs& Args);
  57:     void RebuildEventDetails();
  58:     void EventEdited(const FPropertyChangedEvent& Event);
  59:     void ObjectChanged(UObject* Object, FPropertyChangedEvent& Event);
  60:     void SyncTimeline();
  61:     TSharedPtr<IDetailsView> DefinitionDetails;
  62:     TSharedPtr<IDetailsView> TimelineDetails;
  63:     TSharedPtr<IStructureDetailsView> EventDetails;
  64:     TSharedPtr<FStructOnScope> EventCopy;
  65:     TSharedPtr<SHodgeAbilityPreview> Preview;
  66:     TArray<FString> EventLog;
  67:     FString ValidationMessage;
  68:     FDelegateHandle PropertyChangedHandle;
  69:     bool bApplyingEdit = false;
  70: };
```

## SHodgeAbilityPreview.cpp

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/SHodgeAbilityPreview.cpp](../../../Source/HodgeAbilityEditor/Private/SHodgeAbilityPreview.cpp)

定义候选（多行签名仅展示首行）：

- L31: `void SHodgeAbilityPreview::Construct(const FArguments& Args, TSharedRef<FHodgeAbilityEditorToolkit> InEditor)`
- L41: `SHodgeAbilityPreview::~SHodgeAbilityPreview()`
- L46: `TSharedRef<FEditorViewportClient> SHodgeAbilityPreview::MakeEditorViewportClient()`
- L51: `void SHodgeAbilityPreview::SetMesh(USkeletalMesh* Mesh)`
- L64: `void SHodgeAbilityPreview::SetMontage(UAnimMontage* Montage)`
- L81: `void SHodgeAbilityPreview::SetTime(float Time)`
- L99: `void SHodgeAbilityPreview::SetPlaying(bool bPlaying)`
- L104: `FString SHodgeAbilityPreview::GetMeshPath() const`

## SHodgeAbilityPreview.h

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/SHodgeAbilityPreview.h](../../../Source/HodgeAbilityEditor/Private/SHodgeAbilityPreview.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   2: #include "CoreMinimal.h"
   3: #include "SEditorViewport.h"
   4: #include "AdvancedPreviewScene.h"
   5: #include "EditorViewportClient.h"
   6: class FHodgeAbilityEditorToolkit;
   7: class USkeletalMeshComponent;
   8: class UAnimMontage;
   9: class USkeletalMesh;
  11: class SHodgeAbilityPreview : public SEditorViewport
  12: {
  13: public:
  14:     SLATE_BEGIN_ARGS(SHodgeAbilityPreview) {} SLATE_END_ARGS()
  15:     void Construct(const FArguments& Args, TSharedRef<FHodgeAbilityEditorToolkit> InEditor);
  16:     virtual ~SHodgeAbilityPreview() override;
  17:     void SetMontage(UAnimMontage* Montage);
  18:     void SetMesh(USkeletalMesh* Mesh);
  19:     void SetTime(float Time);
  20:     void SetPlaying(bool bPlaying);
  21:     FString GetMeshPath() const;
  22: protected:
  23:     virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
  24: private:
  25:     friend class FHodgeAbilityEditorTest;
  26:     TWeakPtr<FHodgeAbilityEditorToolkit> Editor;
  27:     TSharedPtr<FAdvancedPreviewScene> Scene;
  28:     TSharedPtr<FEditorViewportClient> Client;
  29:     USkeletalMeshComponent* Component = nullptr;
  30:     TWeakObjectPtr<UAnimMontage> CurrentMontage;
  31: };
```

## SHodgeAbilityTimeline.cpp

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/SHodgeAbilityTimeline.cpp](../../../Source/HodgeAbilityEditor/Private/SHodgeAbilityTimeline.cpp)

定义候选（多行签名仅展示首行）：

- L13: `FVector2D SHodgeAbilityTimeline::ComputeDesiredSize(float) const`
- L18: `float SHodgeAbilityTimeline::TimeAt(const FGeometry& G, float X) const`
- L23: `float SHodgeAbilityTimeline::XAt(const FGeometry& G, float Time) const`
- L29: `int32 SHodgeAbilityTimeline::OnPaint(const FPaintArgs& Args, const FGeometry& G, const FSlateRect& Cull,`
- L87: `FReply SHodgeAbilityTimeline::OnMouseButtonDown(const FGeometry& G, const FPointerEvent& Mouse)`
- L115: `FReply SHodgeAbilityTimeline::OnMouseMove(const FGeometry& G,const FPointerEvent& Mouse)`
- L136: `void SHodgeAbilityTimeline::FinishDrag()`
- L141: `FReply SHodgeAbilityTimeline::OnMouseButtonUp(const FGeometry&,const FPointerEvent& Mouse)`
- L147: `void SHodgeAbilityTimeline::OnMouseCaptureLost(const FCaptureLostEvent& Event) { FinishDrag(); }`
- L148: `FReply SHodgeAbilityTimeline::OnMouseWheel(const FGeometry& G,const FPointerEvent& Mouse)`
- L157: `FReply SHodgeAbilityTimeline::OnKeyDown(const FGeometry&,const FKeyEvent& Key)`

## SHodgeAbilityTimeline.h

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/SHodgeAbilityTimeline.h](../../../Source/HodgeAbilityEditor/Private/SHodgeAbilityTimeline.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   2: #include "CoreMinimal.h"
   3: #include "Widgets/SLeafWidget.h"
   4: #include "ScopedTransaction.h"
   5: class FHodgeAbilityEditorToolkit;
   7: class SHodgeAbilityTimeline : public SLeafWidget
   8: {
   9: public:
  10:     SLATE_BEGIN_ARGS(SHodgeAbilityTimeline) {} SLATE_END_ARGS()
  11:     void Construct(const FArguments&, TSharedRef<FHodgeAbilityEditorToolkit> InEditor) { Editor = InEditor; }
  12:     virtual FVector2D ComputeDesiredSize(float) const override;
  13:     virtual int32 OnPaint(const FPaintArgs&, const FGeometry&, const FSlateRect&, FSlateWindowElementList&,
  14:         int32, const FWidgetStyle&, bool) const override;
  15:     virtual FReply OnMouseButtonDown(const FGeometry&, const FPointerEvent&) override;
  16:     virtual FReply OnMouseMove(const FGeometry&, const FPointerEvent&) override;
  17:     virtual FReply OnMouseButtonUp(const FGeometry&, const FPointerEvent&) override;
  18:     virtual FReply OnMouseWheel(const FGeometry&, const FPointerEvent&) override;
  19:     virtual void OnMouseCaptureLost(const FCaptureLostEvent&) override;
  20:     virtual bool SupportsKeyboardFocus() const override { return true; }
  21:     virtual FReply OnKeyDown(const FGeometry&, const FKeyEvent&) override;
  22: private:
  23:     float TimeAt(const FGeometry&, float X) const;
  24:     float XAt(const FGeometry&, float Time) const;
  25:     void FinishDrag();
  26:     TWeakPtr<FHodgeAbilityEditorToolkit> Editor;
  27:     float Zoom = 1.f;
  28:     float ViewStart = 0.f;
  29:     int32 DragMode = 0;
  30:     float DragTime = 0.f;
  31:     float OriginalStart = 0.f;
  32:     float OriginalEnd = 0.f;
  33:     TUniquePtr<FScopedTransaction> Transaction;
  34: };
```

## Hodgepodge.Build.cs

模块或基础类型入口。

源码：[Source/Hodgepodge/Hodgepodge.Build.cs](../../../Source/Hodgepodge/Hodgepodge.Build.cs)

定义候选（多行签名仅展示首行）：


## Hodgepodge.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Hodgepodge.cpp](../../../Source/Hodgepodge/Hodgepodge.cpp)

定义候选（多行签名仅展示首行）：


## Hodgepodge.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Hodgepodge.h](../../../Source/Hodgepodge/Hodgepodge.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  10: #pragma once
  12: #include "CoreMinimal.h"
```

## Hodgepodge.Target.cs

模块或基础类型入口。

源码：[Source/Hodgepodge.Target.cs](../../../Source/Hodgepodge.Target.cs)

定义候选（多行签名仅展示首行）：


## HodgepodgeEditor.Target.cs

模块或基础类型入口。

源码：[Source/HodgepodgeEditor.Target.cs](../../../Source/HodgepodgeEditor.Target.cs)

定义候选（多行签名仅展示首行）：
