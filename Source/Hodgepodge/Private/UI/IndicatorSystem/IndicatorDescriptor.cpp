// Copyright Epic Games, Inc. All Rights Reserved.

// UIndicatorDescriptor 与 FIndicatorProjection 定义。
#include "UI/IndicatorSystem/IndicatorDescriptor.h"

// LocalPlayer 提供 World → Pixel 的投影辅助函数。
#include "Engine/LocalPlayer.h"

// 场景视图与 FSceneViewProjectionData 定义。
// 投影过程中需要使用当前摄像机的 ViewOrigin、ViewProjection 等信息。
#include "SceneView.h"

// IndicatorManagerComponent 定义。
// Descriptor 可以通过保存的 Manager 主动注销自己。
#include "UI/IndicatorSystem/HodgeIndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorDescriptor)

// 根据 IndicatorDescriptor 中描述的目标组件、投影模式、锚点和偏移等信息，
// 将世界空间中的 Indicator 目标转换为最终屏幕空间位置。
bool FIndicatorProjection::Project(const UIndicatorDescriptor& IndicatorDescriptor,
                                   const FSceneViewProjectionData& InProjectionData, const FVector2f& ScreenSize,
                                   FVector& OutScreenPositionWithDepth)
{
	// 获取当前 Indicator 绑定的 SceneComponent。
	// Indicator 的世界空间定位最终都依赖该 Component。
	if (USceneComponent* Component = IndicatorDescriptor.GetSceneComponent())
	{
		// 保存当前 Indicator 最基础的世界空间位置。
		// 使用 Optional 表示该位置理论上可能不存在。
		TOptional<FVector> WorldLocation;

		// 如果 Descriptor 指定了 Component Socket。
		if (IndicatorDescriptor.GetComponentSocketName() != NAME_None)
		{
			// 使用指定 Socket 的世界空间位置作为 Indicator 基础位置。
			WorldLocation = Component->GetSocketTransform(IndicatorDescriptor.GetComponentSocketName()).GetLocation();
		}
		else
		{
			// 没有指定 Socket 时，直接使用 SceneComponent 自身的世界空间位置。
			WorldLocation = Component->GetComponentLocation();
		}

		// 在基础世界坐标上叠加 Descriptor 配置的世界空间偏移。
		const FVector ProjectWorldLocation = WorldLocation.GetValue() + IndicatorDescriptor.GetWorldPositionOffset();

		// 获取当前 Indicator 使用的投影模式。
		const EActorCanvasProjectionMode ProjectionMode = IndicatorDescriptor.GetProjectionMode();

		// 根据不同投影模式选择不同的世界位置 / 包围盒投影方式。
		switch (ProjectionMode)
		{
		// 直接将 SceneComponent / Socket 的某个世界空间点投影到屏幕。
		case EActorCanvasProjectionMode::ComponentPoint:
			{
				// 确保已经成功获取基础世界空间位置。
				if (WorldLocation.IsSet())
				{
					// 保存世界坐标投影后的屏幕像素坐标。
					FVector2D OutScreenSpacePosition;

					// 将 ProjectWorldLocation 从世界空间投影到屏幕像素空间。
					// 返回值表示该投影点是否位于摄像机前方。
					const bool bInFrontOfCamera = ULocalPlayer::GetPixelPoint(
						InProjectionData, ProjectWorldLocation, OutScreenSpacePosition, &ScreenSize);

					// 应用屏幕空间 X 偏移。
					// 当目标位于摄像机背后时翻转 X 偏移方向，
					// 使屏幕后方目标的 Indicator 方向表现保持合理。
					OutScreenSpacePosition.X += IndicatorDescriptor.GetScreenSpaceOffset().X * (
						bInFrontOfCamera ? 1 : -1);

					// 应用屏幕空间 Y 偏移。
					OutScreenSpacePosition.Y += IndicatorDescriptor.GetScreenSpaceOffset().Y;

					// 如果目标位于摄像机背后，但投影结果却落在当前屏幕矩形内部，
					// 则需要把这个位置重新推向屏幕外侧。
					// 这样后续执行屏幕边缘 Clamp 时，才能得到正确的方向。
					if (!bInFrontOfCamera && FBox2f(FVector2f::Zero(), ScreenSize).IsInside(
						(FVector2f)OutScreenSpacePosition))
					{
						// 计算从屏幕中心指向当前投影位置的单位方向。
						const FVector2f CenterToPosition = (FVector2f(OutScreenSpacePosition) - (ScreenSize / 2)).
							GetSafeNormal();

						// 沿该方向把位置推到屏幕范围之外，
						// 避免摄像机背后的目标错误地显示在屏幕中央区域。
						OutScreenSpacePosition = FVector2D((ScreenSize / 2) + CenterToPosition * ScreenSize);
					}

					// 输出最终屏幕坐标。
					// X / Y 表示屏幕像素位置，
					// Z 保存摄像机 ViewOrigin 到投影世界点之间的距离，用于后续深度排序等逻辑。
					OutScreenPositionWithDepth = FVector(OutScreenSpacePosition.X, OutScreenSpacePosition.Y,
					                                     FVector::Dist(InProjectionData.ViewOrigin,
					                                                   ProjectWorldLocation));

					// 当前 Indicator 成功完成投影。
					return true;
				}

				// 没有有效世界位置时投影失败。
				return false;
			}

		// 根据 Component / Actor 投影到屏幕后的二维包围盒计算 Indicator 位置。
		case EActorCanvasProjectionMode::ComponentScreenBoundingBox:
		case EActorCanvasProjectionMode::ActorScreenBoundingBox:
			{
				// 保存用于计算屏幕包围盒的世界空间 BoundingBox。
				FBox IndicatorBox;

				// ActorScreenBoundingBox 使用整个 Actor 所有组件组成的包围盒。
				if (ProjectionMode == EActorCanvasProjectionMode::ActorScreenBoundingBox)
				{
					IndicatorBox = Component->GetOwner()->GetComponentsBoundingBox();
				}
				else
				{
					// ComponentScreenBoundingBox 只使用当前 SceneComponent 自身的包围盒。
					IndicatorBox = Component->Bounds.GetBox();
				}

				// LL / UR 保存世界包围盒投影到屏幕后形成的二维范围。
				FVector2D LL, UR;

				// 将整个世界空间 BoundingBox 投影到屏幕，
				// 获得它在屏幕空间中的二维 BoundingBox。
				// 返回值表示目标是否位于摄像机前方。
				const bool bInFrontOfCamera = ULocalPlayer::GetPixelBoundingBox(
					InProjectionData, IndicatorBox, LL, UR, &ScreenSize);

				// 获取 Descriptor 配置的包围盒归一化锚点。
				const FVector& BoundingBoxAnchor = IndicatorDescriptor.GetBoundingBoxAnchor();

				// 获取投影完成后需要额外应用的屏幕空间偏移。
				const FVector2D& ScreenSpaceOffset = IndicatorDescriptor.GetScreenSpaceOffset();

				// 保存最终屏幕位置以及用于排序的深度值。
				FVector ScreenPositionWithDepth;

				// 根据 BoundingBoxAnchor.X，
				// 在屏幕包围盒的左边界 LL.X 与右边界 UR.X 之间插值得到最终 X。
				// 同时叠加屏幕空间 X 偏移。
				ScreenPositionWithDepth.X = FMath::Lerp(LL.X, UR.X, BoundingBoxAnchor.X) + ScreenSpaceOffset.X * (
					bInFrontOfCamera ? 1 : -1);

				// 根据 BoundingBoxAnchor.Y，
				// 在屏幕包围盒上下边界之间插值得到最终 Y，
				// 然后叠加屏幕空间 Y 偏移。
				ScreenPositionWithDepth.Y = FMath::Lerp(LL.Y, UR.Y, BoundingBoxAnchor.Y) + ScreenSpaceOffset.Y;

				// Z 保存摄像机到 ProjectWorldLocation 的距离，
				// 后续可以作为 Indicator 深度排序依据。
				ScreenPositionWithDepth.Z = FVector::Dist(InProjectionData.ViewOrigin, ProjectWorldLocation);

				// 提取当前计算得到的二维屏幕位置。
				const FVector2f ScreenSpacePosition = FVector2f(FVector2D(ScreenPositionWithDepth));

				// 如果目标位于摄像机背后，但计算出的二维位置仍然位于屏幕内部，
				// 则将其沿屏幕中心 → 当前点的方向推到屏幕外。
				if (!bInFrontOfCamera && FBox2f(FVector2f::Zero(), ScreenSize).IsInside(ScreenSpacePosition))
				{
					// 计算从屏幕中心指向当前投影点的单位方向。
					const FVector2f CenterToPosition = (ScreenSpacePosition - (ScreenSize / 2)).GetSafeNormal();

					// 沿该方向得到位于屏幕范围之外的位置。
					const FVector2f ScreenPositionFromBehind = (ScreenSize / 2) + CenterToPosition * ScreenSize;

					// 使用修正后的屏幕 X 坐标。
					ScreenPositionWithDepth.X = ScreenPositionFromBehind.X;

					// 使用修正后的屏幕 Y 坐标。
					ScreenPositionWithDepth.Y = ScreenPositionFromBehind.Y;
				}

				// 输出最终计算结果。
				OutScreenPositionWithDepth = ScreenPositionWithDepth;

				// 投影成功。
				return true;
			}

		// 根据 Component / Actor 的世界空间 BoundingBox 中某个锚点进行投影。
		case EActorCanvasProjectionMode::ActorBoundingBox:
		case EActorCanvasProjectionMode::ComponentBoundingBox:
			{
				// 保存当前用于计算锚点的世界空间 BoundingBox。
				FBox IndicatorBox;

				// ActorBoundingBox 使用整个 Actor 所有组件组合后的世界空间包围盒。
				if (ProjectionMode == EActorCanvasProjectionMode::ActorBoundingBox)
				{
					IndicatorBox = Component->GetOwner()->GetComponentsBoundingBox();
				}
				else
				{
					// ComponentBoundingBox 只使用当前 SceneComponent 自身的世界空间包围盒。
					IndicatorBox = Component->Bounds.GetBox();
				}

				// 根据 BoundingBoxAnchor 在世界空间包围盒中计算实际投影点。
				//
				// BoundingBoxAnchor 默认使用 (0.5, 0.5, 0.5)，
				// 减去 (0.5, 0.5, 0.5) 后得到相对于包围盒中心的归一化偏移，
				// 再乘包围盒尺寸并加到 Center 上得到最终世界坐标。
				const FVector ProjectBoxPoint = IndicatorBox.GetCenter() + (IndicatorBox.GetSize() * (
					IndicatorDescriptor.GetBoundingBoxAnchor() - FVector(0.5)));

				// 保存世界锚点投影后的屏幕像素位置。
				FVector2D OutScreenSpacePosition;

				// 将世界空间包围盒锚点投影到屏幕像素空间。
				// 返回值表示投影点是否位于摄像机前方。
				const bool bInFrontOfCamera = ULocalPlayer::GetPixelPoint(
					InProjectionData, ProjectBoxPoint, OutScreenSpacePosition, &ScreenSize);

				// 叠加屏幕空间 X 偏移。
				// 摄像机背后的目标会翻转 X 偏移方向。
				OutScreenSpacePosition.X += IndicatorDescriptor.GetScreenSpaceOffset().X * (bInFrontOfCamera ? 1 : -1);

				// 叠加屏幕空间 Y 偏移。
				OutScreenSpacePosition.Y += IndicatorDescriptor.GetScreenSpaceOffset().Y;

				// 如果目标位于摄像机背后，
				// 但投影位置仍然落在屏幕内部，则将位置推到屏幕外侧。
				if (!bInFrontOfCamera && FBox2f(FVector2f::Zero(), ScreenSize).IsInside(
					(FVector2f)OutScreenSpacePosition))
				{
					// 计算屏幕中心指向当前投影位置的方向。
					const FVector2f CenterToPosition = (FVector2f(OutScreenSpacePosition) - (ScreenSize / 2)).
						GetSafeNormal();

					// 沿当前方向将 Indicator 投影位置推到屏幕范围之外。
					OutScreenSpacePosition = FVector2D((ScreenSize / 2) + CenterToPosition * ScreenSize);
				}

				// 输出最终屏幕 X / Y 以及摄像机到包围盒锚点之间的距离。
				OutScreenPositionWithDepth = FVector(OutScreenSpacePosition.X, OutScreenSpacePosition.Y,
				                                     FVector::Dist(InProjectionData.ViewOrigin, ProjectBoxPoint));

				// 投影成功。
				return true;
			}
		}
	}

	// 没有有效 SceneComponent 或没有匹配到有效投影结果时返回失败。
	return false;
}

// 设置当前 IndicatorDescriptor 所属的 IndicatorManagerComponent。
void UIndicatorDescriptor::SetIndicatorManagerComponent(UHodgeIndicatorManagerComponent* InManager)
{
	// Make sure nobody has set this.
	// 确保当前 Descriptor 之前还没有被其他 IndicatorManager 接管。
	// 一个 Descriptor 在当前设计中只应该归属于一个 Manager。
	if (ensure(ManagerPtr.IsExplicitlyNull()))
	{
		// 保存当前 Descriptor 所属 Manager 的弱引用。
		ManagerPtr = InManager;
	}
}

// 主动将当前 Indicator 从所属 IndicatorManager 中注销。
void UIndicatorDescriptor::UnregisterIndicator()
{
	// 尝试取得当前 Descriptor 所属的 Manager。
	if (UHodgeIndicatorManagerComponent* Manager = ManagerPtr.Get())
	{
		// 请求 Manager 移除当前 Descriptor。
		// Manager 会负责广播 OnIndicatorRemoved 并从 Indicators 数组中删除当前对象。
		Manager->RemoveIndicator(this);
	}
}
