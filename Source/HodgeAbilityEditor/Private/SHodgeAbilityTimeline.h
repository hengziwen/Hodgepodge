#pragma once
#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"
#include "ScopedTransaction.h"
class FHodgeAbilityEditorToolkit;

class SHodgeAbilityTimeline : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SHodgeAbilityTimeline) {} SLATE_END_ARGS()
    void Construct(const FArguments&, TSharedRef<FHodgeAbilityEditorToolkit> InEditor) { Editor = InEditor; }
    virtual FVector2D ComputeDesiredSize(float) const override;
    virtual int32 OnPaint(const FPaintArgs&, const FGeometry&, const FSlateRect&, FSlateWindowElementList&,
        int32, const FWidgetStyle&, bool) const override;
    virtual FReply OnMouseButtonDown(const FGeometry&, const FPointerEvent&) override;
    virtual FReply OnMouseMove(const FGeometry&, const FPointerEvent&) override;
    virtual FReply OnMouseButtonUp(const FGeometry&, const FPointerEvent&) override;
    virtual FReply OnMouseWheel(const FGeometry&, const FPointerEvent&) override;
    virtual void OnMouseCaptureLost(const FCaptureLostEvent&) override;
    virtual bool SupportsKeyboardFocus() const override { return true; }
    virtual FReply OnKeyDown(const FGeometry&, const FKeyEvent&) override;
private:
    float TimeAt(const FGeometry&, float X) const;
    float XAt(const FGeometry&, float Time) const;
    void FinishDrag();
    TWeakPtr<FHodgeAbilityEditorToolkit> Editor;
    float Zoom = 1.f;
    float ViewStart = 0.f;
    int32 DragMode = 0;
    float DragTime = 0.f;
    float OriginalStart = 0.f;
    float OriginalEnd = 0.f;
    TUniquePtr<FScopedTransaction> Transaction;
};
