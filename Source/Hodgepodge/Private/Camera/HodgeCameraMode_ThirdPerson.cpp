// 111屎山代码来袭

#include "Camera/HodgeCameraMode_ThirdPerson.h"

#include "Camera/HodgeCameraAssistInterface.h"
#include "Camera/HodgePenetrationAvoidanceFeeler.h"
#include "Curves/CurveVector.h"
#include "Engine/Canvas.h"
#include "GameFramework/CameraBlockingVolume.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCameraMode_ThirdPerson)

namespace HodgeCameraMode_ThirdPerson_Statics
{
	// 用于标记可以被相机碰撞检测忽略的 Actor。
	static const FName NAME_IgnoreCameraCollision = TEXT("IgnoreCameraCollision");
}

UHodgeCameraMode_ThirdPerson::UHodgeCameraMode_ThirdPerson()
{
	// 默认不使用目标偏移曲线。
	TargetOffsetCurve = nullptr;

	// 第0条是主射线，负责实际的相机碰撞检测。
	PenetrationAvoidanceFeelers.Add(
		FHodgePenetrationAvoidanceFeeler(FRotator(+00.0f, +00.0f, 0.0f), 1.00f, 1.00f, 14.f, 0));

	// 向右偏16度的预测射线。
	PenetrationAvoidanceFeelers.Add(
		FHodgePenetrationAvoidanceFeeler(FRotator(+00.0f, +16.0f, 0.0f), 0.75f, 0.75f, 00.f, 3));

	// 向左偏16度的预测射线。
	PenetrationAvoidanceFeelers.Add(
		FHodgePenetrationAvoidanceFeeler(FRotator(+00.0f, -16.0f, 0.0f), 0.75f, 0.75f, 00.f, 3));

	// 向右偏32度的预测射线。
	PenetrationAvoidanceFeelers.Add(
		FHodgePenetrationAvoidanceFeeler(FRotator(+00.0f, +32.0f, 0.0f), 0.50f, 0.50f, 00.f, 5));

	// 向左偏32度的预测射线。
	PenetrationAvoidanceFeelers.Add(
		FHodgePenetrationAvoidanceFeeler(FRotator(+00.0f, -32.0f, 0.0f), 0.50f, 0.50f, 00.f, 5));

	// 向上偏20度的预测射线。
	PenetrationAvoidanceFeelers.Add(
		FHodgePenetrationAvoidanceFeeler(FRotator(+20.0f, +00.0f, 0.0f), 1.00f, 1.00f, 00.f, 4));

	// 向下偏20度的预测射线。
	PenetrationAvoidanceFeelers.Add(
		FHodgePenetrationAvoidanceFeeler(FRotator(-20.0f, +00.0f, 0.0f), 0.50f, 0.50f, 00.f, 4));
}

void UHodgeCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	// 根据当前目标更新相机相关状态。
	UpdateForTarget(DeltaTime);

	// 更新蹲伏产生的相机偏移。
	UpdateCrouchOffset(DeltaTime);

	// 获取目标的基础位置，并叠加当前蹲伏偏移。
	FVector PivotLocation = GetPivotLocation() + CurrentCrouchOffset;
	FRotator PivotRotation = GetPivotRotation();

	// 限制相机上下旋转角度，防止视角翻转。
	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	// 设置相机的基础位置。
	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// 根据当前俯仰角计算第三人称相机偏移。
	if (!bUseRuntimeFloatCurves)
	{
		if (TargetOffsetCurve)
		{
			// 使用俯仰角从 CurveVector 中获取对应的局部空间偏移。
			const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);

			// 将局部偏移按照当前相机旋转转换到世界空间。
			View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
		}
	}
	else
	{
		// 使用三条 RuntimeFloatCurve 分别计算 XYZ 偏移。
		FVector TargetOffset(0.0f);

		// 根据俯仰角计算 X 轴偏移。
		TargetOffset.X = TargetOffsetX.GetRichCurveConst()->Eval(PivotRotation.Pitch);

		// 根据俯仰角计算 Y 轴偏移。
		TargetOffset.Y = TargetOffsetY.GetRichCurveConst()->Eval(PivotRotation.Pitch);

		// 根据俯仰角计算 Z 轴偏移。
		TargetOffset.Z = TargetOffsetZ.GetRichCurveConst()->Eval(PivotRotation.Pitch);

		// 将局部偏移转换到世界空间并应用到相机位置。
		View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
	}

	// 调整最终相机位置，防止相机穿透场景。
	UpdatePreventPenetration(DeltaTime);
}

void UHodgeCameraMode_ThirdPerson::UpdateForTarget(float DeltaTime)
{
	// 判断当前相机目标是否为角色。
	if (const ACharacter* TargetCharacter = Cast<ACharacter>(GetTargetActor()))
	{
		// 角色处于蹲伏状态时调整相机高度。
		if (TargetCharacter->bIsCrouched)
		{
			// 获取角色默认对象，用于读取默认站立/蹲伏眼睛高度。
			const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();

			// 计算蹲伏后相对于站立状态的眼睛高度变化。
			const float CrouchedHeightAdjustment = TargetCharacterCDO->CrouchedEyeHeight - TargetCharacterCDO->
				BaseEyeHeight;

			// 设置蹲伏状态下目标相机偏移。
			SetTargetCrouchOffset(FVector(0.f, 0.f, CrouchedHeightAdjustment));

			return;
		}
	}

	// 非蹲伏状态下恢复为零偏移。
	SetTargetCrouchOffset(FVector::ZeroVector);
}

void UHodgeCameraMode_ThirdPerson::DrawDebug(UCanvas* Canvas) const
{
	// 绘制父类相机模式的调试信息。
	Super::DrawDebug(Canvas);

#if ENABLE_DRAW_DEBUG
	// 获取用于屏幕调试绘制的管理器。
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	// 输出本次相机穿透检测命中的 Actor。
	for (int i = 0; i < DebugActorsHitDuringCameraPenetration.Num(); i++)
	{
		DisplayDebugManager.DrawString(
			FString::Printf(TEXT("HitActorDuringPenetration[%d]: %s")
			                , i
			                , *DebugActorsHitDuringCameraPenetration[i]->GetName()));
	}

	// 记录本次绘制调试信息的时间。
	LastDrawDebugTime = GetWorld()->GetTimeSeconds();
#endif
}

void UHodgeCameraMode_ThirdPerson::UpdatePreventPenetration(float DeltaTime)
{
	// 未开启相机防穿透时直接跳过检测。
	if (!bPreventPenetration)
	{
		return;
	}

	// 获取当前相机跟随的目标 Actor。
	AActor* TargetActor = GetTargetActor();

	// 尝试将目标转换为 Pawn。
	APawn* TargetPawn = Cast<APawn>(TargetActor);

	// 获取 Pawn 对应的 Controller。
	AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;

	// 尝试从 Controller 获取相机辅助接口。
	IHodgeCameraAssistInterface* TargetControllerAssist = Cast<IHodgeCameraAssistInterface>(TargetController);

	// 尝试从目标 Actor 获取相机辅助接口。
	IHodgeCameraAssistInterface* TargetActorAssist = Cast<IHodgeCameraAssistInterface>(TargetActor);

	// 询问目标 Actor 是否指定了专门用于防穿透检测的目标。
	TOptional<AActor*> OptionalPPTarget = TargetActorAssist
		                                      ? TargetActorAssist->GetCameraPreventPenetrationTarget()
		                                      : TOptional<AActor*>();

	// 如果目标没有指定特殊对象，则直接使用当前目标 Actor。
	AActor* PPActor = OptionalPPTarget.IsSet() ? OptionalPPTarget.GetValue() : TargetActor;

	// 如果使用了特殊防穿透目标，则获取它对应的相机辅助接口。
	IHodgeCameraAssistInterface* PPActorAssist = OptionalPPTarget.IsSet()
		                                             ? Cast<IHodgeCameraAssistInterface>(PPActor)
		                                             : nullptr;

	// 获取防穿透目标的根碰撞组件。
	const UPrimitiveComponent* PPActorRootComponent = Cast<UPrimitiveComponent>(PPActor->GetRootComponent());

	if (PPActorRootComponent)
	{
		// 尝试自动选择一个更适合作为安全起点的位置，尽量减少瞄准时的相机平移。
		FVector ClosestPointOnLineToCapsuleCenter;
		FVector SafeLocation = PPActor->GetActorLocation();

		// 获取角色碰撞体中心距离瞄准线最近的点。
		FMath::PointDistToLine(SafeLocation, View.Rotation.Vector(), View.Location, ClosestPointOnLineToCapsuleCenter);

		// 调整安全位置的高度，使其尽量与瞄准线保持一致并限制在碰撞体范围内。
		float const PushInDistance = PenetrationAvoidanceFeelers[0].Extent + CollisionPushOutDistance;
		float const MaxHalfHeight = PPActor->GetSimpleCollisionHalfHeight() - PushInDistance;

		// 将安全位置限制在目标碰撞体的垂直范围内。
		SafeLocation.Z = FMath::Clamp(ClosestPointOnLineToCapsuleCenter.Z, SafeLocation.Z - MaxHalfHeight,
		                              SafeLocation.Z + MaxHalfHeight);

		// 获取目标碰撞体到瞄准线最近点的平方距离。
		float DistanceSqr;
		PPActorRootComponent->GetSquaredDistanceToCollision(ClosestPointOnLineToCapsuleCenter, DistanceSqr,
		                                                    SafeLocation);

		// 将安全位置稍微向碰撞体内部推入，避免射线检测一开始就处于穿透状态。
		if (PenetrationAvoidanceFeelers.Num() > 0)
		{
			SafeLocation += (SafeLocation - ClosestPointOnLineToCapsuleCenter).GetSafeNormal() * PushInDistance;
		}

		// 从安全位置向期望相机位置进行碰撞检测。
		bool const bSingleRayPenetrationCheck = !bDoPredictiveAvoidance;

		PreventCameraPenetration(*PPActor, SafeLocation, View.Location, DeltaTime, AimLineToDesiredPosBlockedPct,
		                         bSingleRayPenetrationCheck);

		// 收集所有可能需要通知的相机辅助对象。
		IHodgeCameraAssistInterface* AssistArray[] = {TargetControllerAssist, TargetActorAssist, PPActorAssist};

		// 相机被阻挡到指定程度时通知相关对象。
		if (AimLineToDesiredPosBlockedPct < ReportPenetrationPercent)
		{
			for (IHodgeCameraAssistInterface* Assist : AssistArray)
			{
				if (Assist)
				{
					// 通知辅助对象当前相机距离目标过近。
					Assist->OnCameraPenetratingTarget();
				}
			}
		}
	}
}

void UHodgeCameraMode_ThirdPerson::PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc,
                                                            FVector& CameraLoc, float const& DeltaTime,
                                                            float& DistBlockedPct, bool bSingleRayOnly)
{
#if ENABLE_DRAW_DEBUG
	// 清空上一帧的碰撞调试结果。
	DebugActorsHitDuringCameraPenetration.Reset();
#endif

	// HardBlockedPct 代表主射线产生的硬限制。
	float HardBlockedPct = DistBlockedPct;

	// SoftBlockedPct 代表预测射线产生的软限制。
	float SoftBlockedPct = DistBlockedPct;

	// 计算从安全位置指向期望相机位置的基础射线。
	FVector BaseRay = CameraLoc - SafeLoc;

	// 根据基础射线方向建立旋转矩阵。
	FRotationMatrix BaseRayMatrix(BaseRay.Rotation());

	// 保存基础射线局部坐标系的上下、前后、左右方向。
	FVector BaseRayLocalUp, BaseRayLocalFwd, BaseRayLocalRight;

	// 获取基础射线局部坐标轴。
	BaseRayMatrix.GetScaledAxes(BaseRayLocalFwd, BaseRayLocalRight, BaseRayLocalUp);

	// 当前帧默认认为没有任何阻挡。
	float DistBlockedPctThisFrame = 1.f;

	// 不启用预测时只检测第0条主射线，否则检测所有 Feelers。
	int32 const NumRaysToShoot = bSingleRayOnly
		                             ? FMath::Min(1, PenetrationAvoidanceFeelers.Num())
		                             : PenetrationAvoidanceFeelers.Num();

	// 创建相机碰撞检测参数。
	FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(CameraPen), false, nullptr/*PlayerCamera*/);

	// 忽略相机跟随的目标 Actor，避免射线直接撞到玩家自己。
	SphereParams.AddIgnoredActor(&ViewTarget);

	// TODO：未来可以通过 IHodgeCameraAssistInterface 获取需要忽略的 Actor。
	// TODO IHodgeCameraTarget.GetIgnoredActorsForCameraPentration();
	//if (IgnoreActorForCameraPenetration)
	//{
	//    SphereParams.AddIgnoredActor(IgnoreActorForCameraPenetration);
	//}

	// 创建球形碰撞体，实际半径会根据 Feeler 配置动态设置。
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(0.f);

	// 获取当前世界。
	UWorld* World = GetWorld();

	// 逐条执行相机穿透检测射线。
	for (int32 RayIdx = 0; RayIdx < NumRaysToShoot; ++RayIdx)
	{
		// 获取当前射线的配置。
		FHodgePenetrationAvoidanceFeeler& Feeler = PenetrationAvoidanceFeelers[RayIdx];

		// 只有计时器到期后才执行本次检测。
		if (Feeler.FramesUntilNextTrace <= 0)
		{
			// 根据当前 Feeler 的角度计算射线终点。
			FVector RayTarget;
			{
				// 根据 Yaw 绕基础射线的局部 Up 轴旋转。
				FVector RotatedRay = BaseRay.RotateAngleAxis(Feeler.AdjustmentRot.Yaw, BaseRayLocalUp);

				// 根据 Pitch 绕基础射线的局部 Right 轴旋转。
				RotatedRay = RotatedRay.RotateAngleAxis(Feeler.AdjustmentRot.Pitch, BaseRayLocalRight);

				// 得到当前 Feelers 对应的射线终点。
				RayTarget = SafeLoc + RotatedRay;
			}

			// 分别考虑世界和 Pawn 的碰撞权重，以便安全地忽略相机目标 Pawn。
			SphereShape.Sphere.Radius = Feeler.Extent;

			// 相机使用 ECC_Camera 通道进行碰撞检测。
			ECollisionChannel TraceChannel = ECC_Camera; //(Feeler.PawnWeight > 0.f) ? ECC_Pawn : ECC_Camera;

			// 使用 Sweep 而不是单纯 LineTrace，可以检测具有半径的相机碰撞体。
			// 多条射线可以避免忽略前方无关碰撞体后把后方真正的障碍物一起忽略。
			// MT-> 将相机作为 Actor 传入，让 CameraBlockingVolume 知道当前检测来自相机。

			// 保存本次 Sweep 的碰撞结果。
			FHitResult Hit;

			// 从安全位置向射线目标位置执行球形 Sweep。
			const bool bHit = World->SweepSingleByChannel(Hit, SafeLoc, RayTarget, FQuat::Identity, TraceChannel,
			                                              SphereShape, SphereParams);

#if ENABLE_DRAW_DEBUG
			// 在最近一次调试绘制后的1秒内持续绘制碰撞检测射线。
			if (World->TimeSince(LastDrawDebugTime) < 1.f)
			{
				// 绘制射线起点的碰撞球。
				DrawDebugSphere(World, SafeLoc, SphereShape.Sphere.Radius, 8, FColor::Red);

				// 绘制实际命中位置或射线终点的碰撞球。
				DrawDebugSphere(World, bHit ? Hit.Location : RayTarget, SphereShape.Sphere.Radius, 8, FColor::Red);

				// 绘制从安全位置到命中点或目标点的检测线。
				DrawDebugLine(World, SafeLoc, bHit ? Hit.Location : RayTarget, FColor::Red);
			}
#endif // ENABLE_DRAW_DEBUG

			// 根据当前 Feeler 配置设置下一次检测的间隔帧数。
			Feeler.FramesUntilNextTrace = Feeler.TraceInterval;

			// 获取本次检测命中的 Actor。
			const AActor* HitActor = Hit.GetActor();

			if (bHit && HitActor)
			{
				// 标记当前碰撞是否应该被忽略。
				bool bIgnoreHit = false;

				// 带有 IgnoreCameraCollision 标签的 Actor 不参与相机碰撞。
				if (HitActor->ActorHasTag(HodgeCameraMode_ThirdPerson_Statics::NAME_IgnoreCameraCollision))
				{
					bIgnoreHit = true;

					// 将该 Actor 加入忽略列表，后续射线也不再检测它。
					SphereParams.AddIgnoredActor(HitActor);
				}

				// 忽略位于 ViewTarget 前方的 CameraBlockingVolume。
				if (!bIgnoreHit && HitActor->IsA<ACameraBlockingVolume>())
				{
					// 获取目标角色的水平朝向。
					const FVector ViewTargetForwardXY = ViewTarget.GetActorForwardVector().GetSafeNormal2D();

					// 获取目标角色的位置。
					const FVector ViewTargetLocation = ViewTarget.GetActorLocation();

					// 计算从目标到碰撞点的方向。
					const FVector HitOffset = Hit.Location - ViewTargetLocation;

					// 获取碰撞点相对于目标的水平单位方向。
					const FVector HitDirectionXY = HitOffset.GetSafeNormal2D();

					// 判断碰撞点是否位于目标角色前方。
					const float DotHitDirection = FVector::DotProduct(ViewTargetForwardXY, HitDirectionXY);

					if (DotHitDirection > 0.0f)
					{
						bIgnoreHit = true;

						// 后续 Sweep 也忽略这个 CameraBlockingVolume。
						SphereParams.AddIgnoredActor(HitActor);
					}
					else
					{
#if ENABLE_DRAW_DEBUG
						// 记录真正阻挡相机的 Actor，用于调试显示。
						DebugActorsHitDuringCameraPenetration.AddUnique(TObjectPtr<const AActor>(HitActor));
#endif
					}
				}

				if (!bIgnoreHit)
				{
					// Pawn 和世界物体使用不同的碰撞权重。
					float const Weight = Cast<APawn>(Hit.GetActor()) ? Feeler.PawnWeight : Feeler.WorldWeight;

					// 获取碰撞发生在射线上的归一化位置。
					float NewBlockPct = Hit.Time;

					// 根据碰撞权重计算最终的阻挡比例。
					NewBlockPct += (1.f - NewBlockPct) * (1.f - Weight);

					// 根据相机推出距离重新计算实际阻挡比例。
					NewBlockPct = ((Hit.Location - SafeLoc).Size() - CollisionPushOutDistance) / (RayTarget - SafeLoc).
						Size();

					// 当前帧取所有 Feelers 中限制最严格的阻挡比例。
					DistBlockedPctThisFrame = FMath::Min(NewBlockPct, DistBlockedPctThisFrame);

					// 当前 Feeler 命中后下一帧立即重新检测。
					Feeler.FramesUntilNextTrace = 0;

#if ENABLE_DRAW_DEBUG
					// 记录命中的 Actor，用于调试显示。
					DebugActorsHitDuringCameraPenetration.AddUnique(TObjectPtr<const AActor>(HitActor));
#endif
				}
			}

			// 第0条射线是主射线，产生硬限制。
			if (RayIdx == 0)
			{
				// 主射线的结果直接作为 HardBlockedPct，不进行平滑过渡。
				HardBlockedPct = DistBlockedPctThisFrame;
			}
			else
			{
				// 预测射线的结果作为 SoftBlockedPct，用于平滑提前拉近相机。
				SoftBlockedPct = DistBlockedPctThisFrame;
			}
		}
		else
		{
			// 当前 Feeler 尚未到检测时间，减少等待帧数。
			--Feeler.FramesUntilNextTrace;
		}
	}

	// 如果刚刚重置了插值，则直接使用当前帧检测结果。
	if (bResetInterpolation)
	{
		DistBlockedPct = DistBlockedPctThisFrame;
	}
	else if (DistBlockedPct < DistBlockedPctThisFrame)
	{
		// 障碍物消失后平滑恢复相机距离。
		if (PenetrationBlendOutTime > DeltaTime)
		{
			DistBlockedPct = DistBlockedPct + DeltaTime / PenetrationBlendOutTime * (DistBlockedPctThisFrame -
				DistBlockedPct);
		}
		else
		{
			// 恢复时间太短时直接设置目标值。
			DistBlockedPct = DistBlockedPctThisFrame;
		}
	}
	else
	{
		// 当前相机正在被障碍物限制。
		if (DistBlockedPct > HardBlockedPct)
		{
			// 主射线是硬限制，不能超过它。
			DistBlockedPct = HardBlockedPct;
		}
		else if (DistBlockedPct > SoftBlockedPct)
		{
			// 预测射线产生的软限制进行平滑进入。
			if (PenetrationBlendInTime > DeltaTime)
			{
				DistBlockedPct = DistBlockedPct - DeltaTime / PenetrationBlendInTime * (DistBlockedPct -
					SoftBlockedPct);
			}
			else
			{
				// 进入时间太短时直接设置软限制值。
				DistBlockedPct = SoftBlockedPct;
			}
		}
	}

	// 将最终阻挡比例限制在0到1之间。
	DistBlockedPct = FMath::Clamp<float>(DistBlockedPct, 0.f, 1.f);

	// 当相机确实被障碍物阻挡时，将相机沿 SafeLoc 到原始 CameraLoc 的方向拉近。
	if (DistBlockedPct < (1.f - ZERO_ANIMWEIGHT_THRESH))
	{
		CameraLoc = SafeLoc + (CameraLoc - SafeLoc) * DistBlockedPct;
	}
}

void UHodgeCameraMode_ThirdPerson::SetTargetCrouchOffset(FVector NewTargetOffset)
{
	// 新一轮蹲伏偏移插值从0开始。
	CrouchOffsetBlendPct = 0.0f;

	// 保存当前偏移作为本次插值的起点。
	InitialCrouchOffset = CurrentCrouchOffset;

	// 保存新的目标偏移作为本次插值的终点。
	TargetCrouchOffset = NewTargetOffset;
}

void UHodgeCameraMode_ThirdPerson::UpdateCrouchOffset(float DeltaTime)
{
	// 只有插值尚未完成时才继续更新。
	if (CrouchOffsetBlendPct < 1.0f)
	{
		// 根据时间和插值速度推进蹲伏偏移进度。
		CrouchOffsetBlendPct = FMath::Min(CrouchOffsetBlendPct + DeltaTime * CrouchOffsetBlendMultiplier, 1.0f);

		// 使用 EaseInOut 平滑计算当前蹲伏偏移。
		CurrentCrouchOffset = FMath::InterpEaseInOut(InitialCrouchOffset, TargetCrouchOffset, CrouchOffsetBlendPct,
		                                             1.0f);
	}
	else
	{
		// 插值完成后直接使用目标偏移。
		CurrentCrouchOffset = TargetCrouchOffset;

		// 确保插值进度保持完成状态。
		CrouchOffsetBlendPct = 1.0f;
	}
}
