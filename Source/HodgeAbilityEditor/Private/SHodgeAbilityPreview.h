#pragma once
#include "CoreMinimal.h"
#include "SEditorViewport.h"
#include "AdvancedPreviewScene.h"
#include "EditorViewportClient.h"
class FHodgeAbilityEditorToolkit;
class USkeletalMeshComponent;
class UAnimMontage;
class USkeletalMesh;

class SHodgeAbilityPreview : public SEditorViewport
{
public:
    SLATE_BEGIN_ARGS(SHodgeAbilityPreview) {} SLATE_END_ARGS()
    void Construct(const FArguments& Args, TSharedRef<FHodgeAbilityEditorToolkit> InEditor);
    virtual ~SHodgeAbilityPreview() override;
    void SetMontage(UAnimMontage* Montage);
    void SetMesh(USkeletalMesh* Mesh);
    void SetTime(float Time);
    void SetPlaying(bool bPlaying);
    FString GetMeshPath() const;
protected:
    virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
private:
    friend class FHodgeAbilityEditorTest;
    TWeakPtr<FHodgeAbilityEditorToolkit> Editor;
    TSharedPtr<FAdvancedPreviewScene> Scene;
    TSharedPtr<FEditorViewportClient> Client;
    USkeletalMeshComponent* Component = nullptr;
    TWeakObjectPtr<UAnimMontage> CurrentMontage;
};
