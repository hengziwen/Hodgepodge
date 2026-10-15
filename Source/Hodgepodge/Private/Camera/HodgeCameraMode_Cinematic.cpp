/** @file HodgeCameraMode_Cinematic.cpp @brief 实现角色局部多分镜、近景安全回退和独立操作旋转。 */
#include "Camera/HodgeCameraMode_Cinematic.h"
#include "Camera/HodgeCameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Component/HodgeHeroComponent.h"
#include "Data/HodgeCinematicCameraProfile.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraShakeBase.h"
#include "UI/Subsystem/HodgeUIManagerSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCameraMode_Cinematic)

namespace
{
	/** 显式根部和备用骨骼策略，位置使用姿势但不继承骨骼旋转。 */
	FVector ResolveAnchor(const AActor* Actor, const USkeletalMeshComponent* Mesh, FName Bone, FName Fallback)
	{
		if (Bone.IsNone()) { return Actor->GetActorLocation(); }
		return Mesh->GetSocketLocation(Mesh->DoesSocketExist(Bone) ? Bone : Fallback);
	}
	/** 将局部配置变成当前角色水平坐标架中的世界构图。 */
	FHodgeCameraModeView MakeShotView(const AActor* Actor, const USkeletalMeshComponent* Mesh,
		const UHodgeCinematicCameraProfile* Profile, const FHodgeCinematicCameraSample& Sample, FRotator PreviousRotation)
	{
		const auto& Shot = Profile->Shots[Sample.ShotIndex];
		const FRotator Basis(0.f,Actor->GetActorRotation().Yaw,0.f);
		FHodgeCameraModeView Result;
		Result.Location = ResolveAnchor(Actor,Mesh,Shot.Anchor,Profile->FallbackAnchor)+Basis.RotateVector(Sample.CameraOffset);
		const FVector Look = ResolveAnchor(Actor,Mesh,Shot.LookAt,Profile->FallbackAnchor)+Basis.RotateVector(Sample.LookAtOffset);
		const FVector Direction = Look-Result.Location;
		Result.Rotation = Direction.IsNearlyZero() ? PreviousRotation : Direction.Rotation();
		Result.Rotation.Pitch = FMath::ClampAngle(Result.Rotation.Pitch,Profile->PitchLimits.X,Profile->PitchLimits.Y);
		Result.Rotation.Roll=0.f; Result.FieldOfView=Sample.FieldOfView;
		return Result;
	}
}

UHodgeCameraMode_Cinematic::UHodgeCameraMode_Cinematic()
{
	BlendTime=0.f;
}

bool UHodgeCameraMode_Cinematic::KeepUnderlyingModes() const
{
	const auto* Hero=UHodgeHeroComponent::FindHeroComponent(GetTargetActor());
	return Hero && Hero->GetCinematicCameraState().bActive;
}

void UHodgeCameraMode_Cinematic::UpdateView(float DeltaTime)
{
	auto* Actor=GetTargetActor(); auto* Hero=UHodgeHeroComponent::FindHeroComponent(Actor);
	const auto* Pawn=Cast<APawn>(Actor); const auto* PC=Pawn ? Pawn->GetController<APlayerController>() : nullptr;
	const auto* Character=Cast<ACharacter>(Actor); const auto* Mesh=Character ? Character->GetMesh() : nullptr;
	const auto* Profile=Hero ? Hero->GetCinematicProfile() : nullptr;
	const auto State=Hero ? Hero->GetCinematicCameraState() : FHodgeCinematicCameraState();
	// 退出层保持末帧视觉构图，操作朝向仍使用控制器真实方向。
	if (PC) { View.ControlRotation=PC->GetControlRotation(); }
	if (!State.bActive || !Profile || !Mesh || !PC) { ReleaseLocalEffects(); return; }
	if (!(BoundHandle==State.Handle))
	{
		BoundHandle=State.Handle;
		if (!GetHodgeCameraComponent()->GetLastCameraModeView(EntryView))
		{
			EntryView.Location=PC->PlayerCameraManager ? PC->PlayerCameraManager->GetCameraLocation() : Actor->GetActorLocation();
			EntryView.Rotation=PC->GetControlRotation(); EntryView.FieldOfView=PC->PlayerCameraManager ? PC->PlayerCameraManager->GetFOVAngle() : 80.f;
		}
		View=EntryView;
	}
	const float T=(State.SourcePosition-State.WindowStart)/(State.WindowEnd-State.WindowStart);
	if (EventExecution!=State.Handle.ExecutionId)
	{
		ReleaseLocalEffects(); ConsumedEvents.Reset(); EventExecution=State.Handle.ExecutionId;
		PreviousEventTime=T;
	}
	// 进度倒退或合法实例恢复不重播已消费事件，首次晚绑定也不补播过去的节点。
	const bool bGameplayView=UHodgeUIManagerSubsystem::AllowsGameplayInput(PC);
	if (!bGameplayView) { ReleaseLocalEffects(); }
	for (const auto& Event : Profile->LocalEvents)
	{
		if (ConsumedEvents.Contains(Event.EventId) || Event.Time<PreviousEventTime || Event.Time>T) { continue; }
		ConsumedEvents.Add(Event.EventId);
		if (!bGameplayView) { continue; }
		if (Event.Sound) { OwnedAudio.Add(UGameplayStatics::SpawnSound2D(this,Event.Sound)); }
		if (Event.CameraShake && PC->PlayerCameraManager)
		{ ShakeManager=PC->PlayerCameraManager; OwnedShakes.Add(PC->PlayerCameraManager->StartCameraShake(Event.CameraShake,Event.ShakeScale)); }
	}
	PreviousEventTime=T;
	FHodgeCinematicCameraSample Sample;
	if (!Profile->Sample(T,Sample)) { Hero->AbortCinematicCamera(State.Handle,TEXT("Unable to sample current shot")); return; }
	FHodgeCameraModeView ShotView=MakeShotView(Actor,Mesh,Profile,Sample,View.Rotation);
	const auto& Shot=Profile->Shots[Sample.ShotIndex];
	const float ShotTime=(T-Shot.Start)/(Shot.End-Shot.Start);
	if (Sample.ShotIndex>0 && Shot.BlendFraction>0.f && ShotTime<Shot.BlendFraction)
	{
		FHodgeCinematicCameraSample Previous;
		if (Profile->Sample(FMath::Max(0.f,Shot.Start-UE_SMALL_NUMBER),Previous))
		{
			FHodgeCameraModeView Mixed=MakeShotView(Actor,Mesh,Profile,Previous,View.Rotation);
			const float Alpha=FMath::Clamp(ShotTime/Shot.BlendFraction,0.f,1.f);
			Mixed.Blend(ShotView,Alpha*Alpha*(3.f-2.f*Alpha)); ShotView=Mixed;
		}
	}
	const FRotator Basis(0.f,Actor->GetActorRotation().Yaw,0.f);
	const FVector Look=ResolveAnchor(Actor,Mesh,Shot.LookAt,Profile->FallbackAnchor)+Basis.RotateVector(Sample.LookAtOffset);
	FCollisionQueryParams Query(SCENE_QUERY_STAT(HodgeCinematicCamera),false,Actor);
	TArray<AActor*> Attached; Actor->GetAttachedActors(Attached,true,true); Query.AddIgnoredActors(Attached);
	FHitResult Hit;
	if (GetWorld()->SweepSingleByChannel(Hit,Look,ShotView.Location,FQuat::Identity,ECC_Camera,FCollisionShape::MakeSphere(Profile->ProbeRadius),Query))
	{
		if (Hit.bStartPenetrating || FVector::Dist(Look,Hit.Location)<Profile->MinLookAtDistance)
		{ Hero->AbortCinematicCamera(State.Handle,TEXT("Close-up blocked; returning to gameplay camera")); return; }
		ShotView.Location=Hit.Location;
	}
	if (FVector::Dist(Look,ShotView.Location)<Profile->MinLookAtDistance)
	{ Hero->AbortCinematicCamera(State.Handle,TEXT("Close-up violates minimum look-at distance")); return; }
	// 只将进入部分与捕获视图混合一次；返回部分交给保留的游戏镜头。
	const float Enter=FMath::Clamp(T/Profile->BlendFractions.X,0.f,1.f);
	const float Exit=FMath::Clamp((1.f-T)/Profile->BlendFractions.Y,0.f,1.f);
	View=ShotView;
	if (Enter<1.f) { View=EntryView; View.Blend(ShotView,Enter*Enter*(3.f-2.f*Enter)); }
	View.ControlRotation=PC->GetControlRotation();
	SourceBlendWeight=Exit*Exit*(3.f-2.f*Exit);
}

void UHodgeCameraMode_Cinematic::UpdateBlending(float DeltaTime)
{
	SetBlendWeight(SourceBlendWeight);
}

void UHodgeCameraMode_Cinematic::ReleaseLocalEffects()
{
	for (const auto& Audio : OwnedAudio) { if (Audio.IsValid()) { Audio->Stop(); } }
	if (ShakeManager.IsValid()) { for (const auto& Shake : OwnedShakes) { if (Shake.IsValid()) { ShakeManager->StopCameraShake(Shake.Get(),false); } } }
	OwnedAudio.Reset(); OwnedShakes.Reset(); ShakeManager.Reset();
}

void UHodgeCameraMode_Cinematic::OnDeactivation()
{
	ReleaseLocalEffects();
}

void UHodgeCameraMode_Cinematic::EndPresentation(const FHodgeAbilityCameraHandle& Handle)
{
	if (BoundHandle==Handle) { ReleaseLocalEffects(); }
}
