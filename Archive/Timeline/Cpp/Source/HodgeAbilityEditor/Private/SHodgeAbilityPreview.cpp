#include "SHodgeAbilityPreview.h"
#include "HodgeAbilityEditorToolkit.h"
#include "Animation/AnimMontage.h"
#include "Animation/Skeleton.h"
#include "AnimPreviewInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"

class FHodgeAbilityPreviewClient : public FEditorViewportClient
{
    TWeakPtr<FHodgeAbilityEditorToolkit> Editor;
public:
    FHodgeAbilityPreviewClient(FPreviewScene* Scene, const TSharedRef<SEditorViewport>& View,
        TWeakPtr<FHodgeAbilityEditorToolkit> InEditor)
        : FEditorViewportClient(nullptr, Scene, View), Editor(InEditor)
    {
        SetViewLocation(FVector(260, 260, 150));
        SetViewRotation((FVector(0,0,85)-GetViewLocation()).Rotation());
        SetViewMode(VMI_Lit);
        EngineShowFlags.SetGrid(false);
        SetRealtime(false);
    }
    virtual void Tick(float DeltaSeconds) override
    {
        FEditorViewportClient::Tick(DeltaSeconds);
        if (auto Pinned = Editor.Pin()) { Pinned->TickPreview(DeltaSeconds); }
    }
};

void SHodgeAbilityPreview::Construct(const FArguments& Args, TSharedRef<FHodgeAbilityEditorToolkit> InEditor)
{
    Editor = InEditor;
    Scene = MakeShared<FAdvancedPreviewScene>(FPreviewScene::ConstructionValues());
    Scene->SetFloorVisibility(true);
    Component = NewObject<USkeletalMeshComponent>(GetTransientPackage(), NAME_None, RF_Transient);
    Component->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    Scene->AddComponent(Component, FTransform::Identity);
    SEditorViewport::Construct(SEditorViewport::FArguments());
}
SHodgeAbilityPreview::~SHodgeAbilityPreview()
{
    if (Client) { Client->Viewport = nullptr; }
    if (Scene && Component) { Scene->RemoveComponent(Component); }
}
TSharedRef<FEditorViewportClient> SHodgeAbilityPreview::MakeEditorViewportClient()
{
    Client = MakeShared<FHodgeAbilityPreviewClient>(Scene.Get(), SharedThis(this), Editor);
    return Client.ToSharedRef();
}
void SHodgeAbilityPreview::SetMesh(USkeletalMesh* Mesh)
{
    if (!Mesh) { return; }
    Component->SetSkeletalMesh(Mesh);
    Component->SetAnimInstanceClass(UAnimPreviewInstance::StaticClass());
    if (auto* Instance = Cast<UAnimPreviewInstance>(Component->GetAnimInstance()))
    {
        Instance->SetAnimationAsset(CurrentMontage.Get(), false, 1.f);
        Instance->MontagePreview_PreviewNormal(0, false);
        Instance->SetPlaying(false);
    }
    SetTime(0.f);
}
void SHodgeAbilityPreview::SetMontage(UAnimMontage* Montage)
{
    if (CurrentMontage == Montage) { return; }
    CurrentMontage = Montage;
    if (!Montage)
    {
        if (auto* Instance = Cast<UAnimPreviewInstance>(Component->GetAnimInstance())) { Instance->SetAnimationAsset(nullptr); }
        Invalidate();
        return;
    }
    USkeletalMesh* Mesh = Montage->GetPreviewMesh();
    if (!Mesh && Montage->GetSkeleton()) { Mesh = Montage->GetSkeleton()->GetPreviewMesh(); }
    if (Mesh) { SetMesh(Mesh); }
    else if (auto* Instance = Cast<UAnimPreviewInstance>(Component->GetAnimInstance()))
    { Instance->SetAnimationAsset(Montage, false, 1.f); Instance->MontagePreview_PreviewNormal(0, false);
        Instance->SetPlaying(false); }
}
void SHodgeAbilityPreview::SetTime(float Time)
{
    if (auto* Instance = Cast<UAnimPreviewInstance>(Component->GetAnimInstance()))
    {
        // 显式定位不派发动画 Notify；预览不推进游戏世界或提取角色位移。
        Instance->SetPosition(Time, false);
        Component->TickAnimation(0.f, false);
        Component->RefreshBoneTransforms();
        Component->UpdateBounds();
        Component->MarkRenderTransformDirty();
        Component->MarkRenderDynamicDataDirty();
        // 预览世界不 Tick，必须主动提交骨骼与包围盒的帧末渲染更新。
        Scene->GetWorld()->SendAllEndOfFrameUpdates();
    }
    // Slate 唤醒不等于视口画面失效；非实时视口需显式请求重绘。
    if (Client) { Client->Invalidate(false, false); }
    Invalidate();
}
void SHodgeAbilityPreview::SetPlaying(bool bPlaying)
{
    if (Client) { Client->SetRealtime(bPlaying); }
}

FString SHodgeAbilityPreview::GetMeshPath() const
{
    return Component && Component->GetSkeletalMeshAsset() ? Component->GetSkeletalMeshAsset()->GetPathName() : FString();
}
