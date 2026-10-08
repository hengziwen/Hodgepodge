#include "SHodgeAbilityTimeline.h"
#include "HodgeAbilityEditorToolkit.h"
#include "Rendering/DrawElements.h"
#include "Styling/AppStyle.h"
#include "InputCoreTypes.h"

namespace
{
    constexpr float LabelWidth = 95.f;
    constexpr float HeaderHeight = 32.f;
    constexpr float RowHeight = 42.f;
}
FVector2D SHodgeAbilityTimeline::ComputeDesiredSize(float) const
{
    auto E = Editor.Pin();
    return FVector2D(600.f, HeaderHeight + RowHeight * (E && E->Timeline ? FMath::Max(1,E->Timeline->Events.Num()) : 1));
}
float SHodgeAbilityTimeline::TimeAt(const FGeometry& G, float X) const
{
    auto E = Editor.Pin();
    return ViewStart + (X - LabelWidth) / FMath::Max(1.f,float(G.GetLocalSize().X)-LabelWidth) * (E ? E->Duration() / Zoom : 1.f);
}
float SHodgeAbilityTimeline::XAt(const FGeometry& G, float Time) const
{
    auto E = Editor.Pin();
    return LabelWidth + (Time - ViewStart) / FMath::Max(.001f,E ? E->Duration()/Zoom : 1.f)
        * (G.GetLocalSize().X - LabelWidth);
}
int32 SHodgeAbilityTimeline::OnPaint(const FPaintArgs& Args, const FGeometry& G, const FSlateRect& Cull,
    FSlateWindowElementList& Out, int32 Layer, const FWidgetStyle& Style, bool bEnabled) const
{
    auto E = Editor.Pin();
    if (!E) { return Layer; }
    auto Box = [&](float X, float Y, float W, float H, FLinearColor Color, int32 L)
    {
        FSlateDrawElement::MakeBox(Out,L,G.ToPaintGeometry(FVector2D(W,H),FSlateLayoutTransform(FVector2D(X,Y))),
            FAppStyle::GetBrush("WhiteBrush"), ESlateDrawEffect::None,Color);
    };
    auto Text = [&](float X,float Y,const FString& Value,FLinearColor Color)
    {
        FSlateDrawElement::MakeText(Out,Layer+3,G.ToPaintGeometry(FVector2D(1,1),FSlateLayoutTransform(FVector2D(X,Y))),
            Value,FAppStyle::GetFontStyle("SmallFont"),ESlateDrawEffect::None,Color);
    };
    Out.PushClip(FSlateClippingZone(G));
    Box(0,0,G.GetLocalSize().X,G.GetLocalSize().Y,FLinearColor(.035f,.035f,.035f),Layer);
    const float Span = E->Duration()/Zoom;
    for (int32 Step=0;Step<=10;++Step)
    {
        const float T = ViewStart + Span*Step/10.f;
        const float X = XAt(G,T);
        Box(X,0,1,G.GetLocalSize().Y,FLinearColor(.15f,.15f,.15f),Layer+1);
        Text(X+3,5,FString::Printf(TEXT("%.2fs"),T),FLinearColor(.7f,.7f,.7f));
    }
    Text(5,5,TEXT("Events"),FLinearColor::White);
    if (E->Timeline)
    {
        for (int32 Index=0;Index<E->Timeline->Events.Num();++Index)
        {
            const auto& Event = E->Timeline->Events[Index];
            const float Y = HeaderHeight + Index*RowHeight;
            const bool bSelected = Event.EventID == E->SelectedID;
            Box(0,Y,G.GetLocalSize().X,RowHeight-1,bSelected ? FLinearColor(.13f,.17f,.2f) : FLinearColor(.06f,.06f,.06f),Layer);
            Text(5,Y+8,FString::Printf(TEXT("%s %d"),Event.Kind==EHodgeTimelineEventKind::Window?TEXT("Window"):TEXT("Point"),Index+1),bSelected ? FLinearColor::White : FLinearColor(.7f,.7f,.7f));
            const bool bWindow = Event.Kind == EHodgeTimelineEventKind::Window;
            const bool bInvalid = Event.StartTime < 0.f || Event.StartTime > E->Duration()
                || (bWindow && (Event.EndTime <= Event.StartTime || Event.EndTime > E->Duration()+.001f));
            const float Start = XAt(G,Event.StartTime);
            const float End = XAt(G,bWindow ? Event.EndTime : Event.StartTime);
            const FLinearColor Color = bInvalid ? FLinearColor(.85f,.2f,.15f)
                : bSelected ? FLinearColor(1.f,.75f,.25f) : bWindow ? FLinearColor(.3f,.7f,.5f) : FLinearColor(.2f,.65f,.8f);
            const float Left = FMath::Max(LabelWidth,bWindow ? Start : Start-3);
            const float Right = FMath::Min(float(G.GetLocalSize().X),bWindow ? End : Start+3);
            if (Right >= Left) { Box(Left,Y+5,FMath::Max(3.f,Right-Left),22,Color,Layer+2); }
            if (Right>Left+30) { Text(Left+5,Y+8,Event.EventID.ToString(),FLinearColor(.02f,.02f,.02f)); }
            if (bWindow)
            {
                if (Start>=LabelWidth) { Box(Start,Y+5,3,22,FLinearColor::White,Layer+3); }
                if (End>=LabelWidth) { Box(End-3,Y+5,3,22,FLinearColor::White,Layer+3); }
            }
        }
    }
    const float CursorX = XAt(G,E->Time);
    if (CursorX >= LabelWidth) { Box(CursorX,0,2,G.GetLocalSize().Y,FLinearColor(1.f,.65f,.15f),Layer+4); }
    Out.PopClip();
    return Layer+4;
}
FReply SHodgeAbilityTimeline::OnMouseButtonDown(const FGeometry& G, const FPointerEvent& Mouse)
{
    auto E = Editor.Pin();
    if (!E || Mouse.GetEffectingButton()!=EKeys::LeftMouseButton) { return FReply::Unhandled(); }
    const FVector2D P = G.AbsoluteToLocal(Mouse.GetScreenSpacePosition());
    E->Seek(E->Time);
    const int32 Index = FMath::FloorToInt((P.Y-HeaderHeight)/RowHeight);
    if (P.Y >= HeaderHeight && E->Timeline && E->Timeline->Events.IsValidIndex(Index))
    {
        E->Select(Index);
        const auto& Event = E->Timeline->Events[Index];
        const float StartX=XAt(G,Event.StartTime), EndX=XAt(G,Event.EndTime);
        if (P.X>=LabelWidth && ((Event.Kind==EHodgeTimelineEventKind::Window && P.X>=StartX-6 && P.X<=EndX+6)
            || (Event.Kind==EHodgeTimelineEventKind::Point && FMath::Abs(P.X-StartX)<9)))
        {
            OriginalStart=Event.StartTime; OriginalEnd=Event.EndTime; DragTime=TimeAt(G,P.X);
            DragMode=Event.Kind==EHodgeTimelineEventKind::Point ? 1 : FMath::Abs(P.X-StartX)<7 ? 2 : FMath::Abs(P.X-EndX)<7 ? 3 : 1;
            Transaction=MakeUnique<FScopedTransaction>(NSLOCTEXT("HodgeAbilityEditor","Drag","Move timeline event"));
            E->Timeline->SetFlags(RF_Transactional);
            E->Timeline->Modify();
            return FReply::Handled().CaptureMouse(SharedThis(this)).SetUserFocus(SharedThis(this));
        }
        if (P.X<LabelWidth) { return FReply::Handled().SetUserFocus(SharedThis(this)); }
    }
    DragMode=4;
    E->Seek(E->Snap(TimeAt(G,P.X)));
    return FReply::Handled().CaptureMouse(SharedThis(this)).SetUserFocus(SharedThis(this));
}
FReply SHodgeAbilityTimeline::OnMouseMove(const FGeometry& G,const FPointerEvent& Mouse)
{
    auto E=Editor.Pin();
    if (!HasMouseCapture() || !E) { return FReply::Unhandled(); }
    const float T=E->Snap(TimeAt(G,G.AbsoluteToLocal(Mouse.GetScreenSpacePosition()).X));
    if (DragMode==4) { E->Seek(T); }
    else if (E->Timeline && E->Timeline->Events.IsValidIndex(E->SelectedIndex()))
    {
        auto& Event=E->Timeline->Events[E->SelectedIndex()];
        if (DragMode==1)
        {
            const float Width=Event.Kind==EHodgeTimelineEventKind::Window ? OriginalEnd-OriginalStart : 0.f;
            Event.StartTime=FMath::Clamp(E->Snap(OriginalStart+T-DragTime),0.f,FMath::Max(0.f,E->Duration()-Width));
            if (Event.Kind==EHodgeTimelineEventKind::Window) { Event.EndTime=Event.StartTime+Width; }
        }
        else if (DragMode==2) { Event.StartTime=FMath::Clamp(T,0.f,FMath::Max(0.f,Event.EndTime-.002f)); }
        else if (DragMode==3) { Event.EndTime=FMath::Clamp(T,Event.StartTime+.002f,FMath::Max(Event.StartTime+.002f,E->Duration())); }
        E->Seek(E->Time);
    }
    return FReply::Handled();
}
void SHodgeAbilityTimeline::FinishDrag()
{
    if (Transaction) { if (auto E=Editor.Pin()) { E->CommitTimeline(); } Transaction.Reset(); }
    DragMode=0;
}
FReply SHodgeAbilityTimeline::OnMouseButtonUp(const FGeometry&,const FPointerEvent& Mouse)
{
    if (Mouse.GetEffectingButton()!=EKeys::LeftMouseButton || !HasMouseCapture()) { return FReply::Unhandled(); }
    FinishDrag();
    return FReply::Handled().ReleaseMouseCapture();
}
void SHodgeAbilityTimeline::OnMouseCaptureLost(const FCaptureLostEvent& Event) { FinishDrag(); }
FReply SHodgeAbilityTimeline::OnMouseWheel(const FGeometry& G,const FPointerEvent& Mouse)
{
    auto E=Editor.Pin(); if (!E) { return FReply::Unhandled(); }
    if (!Mouse.IsControlDown() && !Mouse.IsShiftDown()) { return FReply::Unhandled(); }
    if (Mouse.IsShiftDown()) { ViewStart-=Mouse.GetWheelDelta()*E->Duration()/Zoom*.1f; }
    else { Zoom=FMath::Clamp(Zoom*FMath::Pow(1.25f,Mouse.GetWheelDelta()),1.f,20.f); }
    ViewStart=FMath::Clamp(ViewStart,0.f,E->Duration()-E->Duration()/Zoom);
    return FReply::Handled();
}
FReply SHodgeAbilityTimeline::OnKeyDown(const FGeometry&,const FKeyEvent& Key)
{
    if (auto E=Editor.Pin())
    {
        if (Key.GetKey()==EKeys::Delete) { return E->DeleteEvent(); }
        if (Key.GetKey()==EKeys::SpaceBar) { return E->TogglePlay(); }
    }
    return FReply::Unhandled();
}
