// 111 屎山代码来袭


#include "Camera/HodgeCameraMode.h"

#include "Camera/HodgeCameraComponent.h"
#include "Camera/HodgePlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Canvas.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeCameraMode)


//////////////////////////////////////////////////////////////////////////
// FHodgeCameraModeView
//////////////////////////////////////////////////////////////////////////

// 初始化摄像机视图数据
FHodgeCameraModeView::FHodgeCameraModeView()
	: Location(ForceInit)
	  , Rotation(ForceInit)
	  , ControlRotation(ForceInit)
	  , FieldOfView(HODGE_CAMERA_DEFAULT_FOV)
{
}

// 按照指定权重将另一个摄像机视图混合到当前视图
void FHodgeCameraModeView::Blend(const FHodgeCameraModeView& Other, float OtherWeight)
{
	// 权重小于等于 0 时，不需要进行任何混合
	if (OtherWeight <= 0.0f)
	{
		return;
	}
	// 权重大于等于 1 时，直接完全使用目标视图
	else if (OtherWeight >= 1.0f)
	{
		*this = Other;
		return;
	}

	// 线性插值计算摄像机位置
	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	// 计算两个旋转之间的最短角度差
	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();

	// 按照混合权重插值计算最终摄像机旋转
	Rotation = Rotation + (OtherWeight * DeltaRotation);

	// 计算两个控制器旋转之间的最短角度差
	const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();

	// 按照混合权重插值计算最终控制器旋转
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

	// 线性插值计算摄像机视野角度
	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}


//////////////////////////////////////////////////////////////////////////
// UHodgeCameraMode
//////////////////////////////////////////////////////////////////////////

// 初始化摄像机模式的默认参数
UHodgeCameraMode::UHodgeCameraMode()
{
	// 设置默认水平视野角
	FieldOfView = HODGE_CAMERA_DEFAULT_FOV;

	// 设置允许的最小俯仰角
	ViewPitchMin = HODGE_CAMERA_DEFAULT_PITCH_MIN;

	// 设置允许的最大俯仰角
	ViewPitchMax = HODGE_CAMERA_DEFAULT_PITCH_MAX;

	// 设置摄像机模式默认混合时间为 0.5 秒
	BlendTime = 0.5f;

	// 默认使用 EaseOut 混合曲线
	BlendFunction = EHodgeCameraModeBlendFunction::EaseOut;

	// 设置混合曲线指数
	BlendExponent = 4.0f;

	// 默认混合进度为完成状态
	BlendAlpha = 1.0f;

	// 默认混合权重为完全生效
	BlendWeight = 1.0f;
}

// 获取当前摄像机模式所属的 HodgeCameraComponent
UHodgeCameraComponent* UHodgeCameraMode::GetHodgeCameraComponent() const
{
	// CameraMode 的 Outer 就是创建它的 HodgeCameraComponent
	return CastChecked<UHodgeCameraComponent>(GetOuter());
}

// 获取当前摄像机模式所在的 World
UWorld* UHodgeCameraMode::GetWorld() const
{
	// 类默认对象没有实际 World，运行时对象则从 Outer 获取 World
	return HasAnyFlags(RF_ClassDefaultObject) ? nullptr : GetOuter()->GetWorld();
}

// 获取当前摄像机模式的目标 Actor
AActor* UHodgeCameraMode::GetTargetActor() const
{
	// 获取当前摄像机模式所属的摄像机组件
	const UHodgeCameraComponent* HodgeCameraComponent = GetHodgeCameraComponent();

	// 摄像机组件负责提供当前真正的目标 Actor
	return HodgeCameraComponent->GetTargetActor();
}

// 获取摄像机计算位置时使用的中心点
FVector UHodgeCameraMode::GetPivotLocation() const
{
	// 获取当前摄像机跟随的目标 Actor
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	// 如果目标是 Pawn，则使用 Pawn 专用的视角位置计算方式
	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// Character 需要额外处理蹲伏导致的胶囊体高度变化
		if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetPawn))
		{
			// 获取 Character 类默认对象，用于获取未蹲伏时的默认参数
			const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();
			check(TargetCharacterCDO);

			// 获取当前 Character 的胶囊体
			const UCapsuleComponent* CapsuleComp = TargetCharacter->GetCapsuleComponent();
			check(CapsuleComp);

			// 获取 Character 默认对象的胶囊体
			const UCapsuleComponent* CapsuleCompCDO = TargetCharacterCDO->GetCapsuleComponent();
			check(CapsuleCompCDO);

			// 获取默认胶囊体半高
			const float DefaultHalfHeight = CapsuleCompCDO->GetUnscaledCapsuleHalfHeight();

			// 获取当前胶囊体实际半高
			const float ActualHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();

			// 根据蹲伏后的胶囊体高度变化计算摄像机高度修正值
			const float HeightAdjustment = (DefaultHalfHeight - ActualHalfHeight) + TargetCharacterCDO->BaseEyeHeight;

			// 在角色位置基础上向上偏移到适合摄像机跟随的高度
			return TargetCharacter->GetActorLocation() + (FVector::UpVector * HeightAdjustment);
		}

		// 普通 Pawn 直接使用 Pawn 自己计算出的视角位置
		return TargetPawn->GetPawnViewLocation();
	}

	// 非 Pawn 类型的目标直接使用 Actor 世界位置
	return TargetActor->GetActorLocation();
}

// 获取摄像机计算时使用的中心旋转
FRotator UHodgeCameraMode::GetPivotRotation() const
{
	// 获取当前摄像机跟随的目标 Actor
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	// Pawn 使用自身的视角旋转
	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// 获取 Pawn 当前视角旋转
		return TargetPawn->GetViewRotation();
	}

	// 非 Pawn 类型直接使用 Actor 自身旋转
	return TargetActor->GetActorRotation();
}

// 更新摄像机模式的视图和混合状态
void UHodgeCameraMode::UpdateCameraMode(float DeltaTime)
{
	// 更新当前摄像机模式的具体视图数据
	UpdateView(DeltaTime);

	// 更新当前摄像机模式的混合进度和权重
	UpdateBlending(DeltaTime);
}

// 更新当前摄像机模式产生的摄像机视图
void UHodgeCameraMode::UpdateView(float DeltaTime)
{
	// 获取摄像机计算所需的中心位置
	FVector PivotLocation = GetPivotLocation();

	// 获取摄像机计算所需的中心旋转
	FRotator PivotRotation = GetPivotRotation();

	// 将俯仰角限制在当前 CameraMode 配置的范围内
	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	// 使用中心位置作为当前摄像机位置
	View.Location = PivotLocation;

	// 使用限制后的中心旋转作为摄像机旋转
	View.Rotation = PivotRotation;

	// 默认情况下控制器旋转与摄像机旋转保持一致
	View.ControlRotation = View.Rotation;

	// 使用当前 CameraMode 配置的视野角
	View.FieldOfView = FieldOfView;
}

// 直接设置摄像机模式当前的混合权重
void UHodgeCameraMode::SetBlendWeight(float Weight)
{
	// 将混合权重限制在 0~1 范围
	BlendWeight = FMath::Clamp(Weight, 0.0f, 1.0f);

	// 直接修改权重后，需要反推出对应的线性混合进度以保持混合曲线一致
	const float InvExponent = (BlendExponent > 0.0f) ? (1.0f / BlendExponent) : 1.0f;

	// 根据当前混合函数将 BlendWeight 转换成 BlendAlpha
	switch (BlendFunction)
	{
	// 线性混合，权重和进度相同
	case EHodgeCameraModeBlendFunction::Linear:
		BlendAlpha = BlendWeight;
		break;

	// EaseIn 混合，根据曲线将权重转换成混合进度
	case EHodgeCameraModeBlendFunction::EaseIn:
		BlendAlpha = FMath::InterpEaseIn(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	// EaseOut 混合，根据曲线将权重转换成混合进度
	case EHodgeCameraModeBlendFunction::EaseOut:
		BlendAlpha = FMath::InterpEaseOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	// EaseInOut 混合，根据曲线将权重转换成混合进度
	case EHodgeCameraModeBlendFunction::EaseInOut:
		BlendAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	// 未知混合函数直接报错
	default:
		checkf(false, TEXT("SetBlendWeight: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

// 每帧更新摄像机模式的混合进度和最终权重
void UHodgeCameraMode::UpdateBlending(float DeltaTime)
{
	// 有混合时间时，根据 DeltaTime 推进混合进度
	if (BlendTime > 0.0f)
	{
		// 按照经过时间占总混合时间的比例增加混合进度
		BlendAlpha += (DeltaTime / BlendTime);

		// 防止混合进度超过 1
		BlendAlpha = FMath::Min(BlendAlpha, 1.0f);
	}
	else
	{
		// 没有混合时间时直接完成混合
		BlendAlpha = 1.0f;
	}

	// 确保混合指数有效
	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;

	// 根据混合曲线计算最终混合权重
	switch (BlendFunction)
	{
	// 线性混合，权重直接等于混合进度
	case EHodgeCameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;

	// EaseIn 曲线计算最终混合权重
	case EHodgeCameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	// EaseOut 曲线计算最终混合权重
	case EHodgeCameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	// EaseInOut 曲线计算最终混合权重
	case EHodgeCameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	// 未知混合函数直接报错
	default:
		checkf(false, TEXT("UpdateBlending: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

// 绘制当前摄像机模式的调试信息
void UHodgeCameraMode::DrawDebug(UCanvas* Canvas) const
{
	// Canvas 必须有效
	check(Canvas);

	// 获取 Canvas 的调试绘制管理器
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	// 设置调试文字颜色
	DisplayDebugManager.SetDrawColor(FColor::White);

	// 显示当前 CameraMode 名称和混合权重
	DisplayDebugManager.DrawString(FString::Printf(TEXT("      HodgeCameraMode: %s (%f)"), *GetName(), BlendWeight));
}


//////////////////////////////////////////////////////////////////////////
// UHodgeCameraModeStack
//////////////////////////////////////////////////////////////////////////

// 初始化摄像机模式栈
UHodgeCameraModeStack::UHodgeCameraModeStack()
{
	// 摄像机模式栈默认处于激活状态
	bIsActive = true;
}

// 激活摄像机模式栈
void UHodgeCameraModeStack::ActivateStack()
{
	// 只有当前未激活时才执行激活逻辑
	if (!bIsActive)
	{
		// 标记摄像机模式栈已经激活
		bIsActive = true;

		// 通知栈中的所有 CameraMode 已经被激活
		for (UHodgeCameraMode* CameraMode : CameraModeStack)
		{
			// CameraMode 必须有效
			check(CameraMode);

			// 通知 CameraMode 执行激活逻辑
			CameraMode->OnActivation();
		}
	}
}

// 停用摄像机模式栈
void UHodgeCameraModeStack::DeactivateStack()
{
	// 只有当前处于激活状态时才执行停用逻辑
	if (bIsActive)
	{
		// 标记摄像机模式栈已经停用
		bIsActive = false;

		// 通知栈中的所有 CameraMode 已经被停用
		for (UHodgeCameraMode* CameraMode : CameraModeStack)
		{
			// CameraMode 必须有效
			check(CameraMode);

			// 通知 CameraMode 执行停用逻辑
			CameraMode->OnDeactivation();
		}
	}
}

// 将指定的 CameraMode 压入栈顶
void UHodgeCameraModeStack::PushCameraMode(TSubclassOf<UHodgeCameraMode> CameraModeClass)
{
	// 没有指定 CameraMode 类型时直接返回
	if (!CameraModeClass)
	{
		return;
	}

	// 获取指定类型的 CameraMode 实例
	UHodgeCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	// 获取当前摄像机模式栈大小
	int32 StackSize = CameraModeStack.Num();

	// 如果已经位于栈顶，则不需要重复压入
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		// 已经是栈顶模式，直接结束
		return;
	}

	// 检查该 CameraMode 是否已经存在于当前栈中
	int32 ExistingStackIndex = INDEX_NONE;

	// 默认假设该 CameraMode 当前对最终视图的贡献权重为 100%
	float ExistingStackContribution = 1.0f;

	// 遍历当前摄像机模式栈寻找相同 CameraMode
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		// 找到已经存在的 CameraMode
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			// 保存该 CameraMode 当前所在的栈索引
			ExistingStackIndex = StackIndex;

			// 计算该 CameraMode 原本对最终结果的贡献
			ExistingStackContribution *= CameraMode->GetBlendWeight();

			break;
		}
		else
		{
			// 计算上方 CameraMode 没有覆盖当前 CameraMode 的剩余权重
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->GetBlendWeight());
		}
	}

	// 如果 CameraMode 已经存在于栈中，则先将旧位置移除
	if (ExistingStackIndex != INDEX_NONE)
	{
		// 从旧位置移除 CameraMode
		CameraModeStack.RemoveAt(ExistingStackIndex);

		// 更新移除后的栈大小
		StackSize--;
	}
	else
	{
		// 新 CameraMode 没有旧贡献，因此初始贡献为 0
		ExistingStackContribution = 0.0f;
	}

	// 判断新 CameraMode 是否需要进行平滑混合
	const bool bShouldBlend = ((CameraMode->GetBlendTime() > 0.0f) && (StackSize > 0));

	// 如果需要混合，则继承原来的贡献权重，否则直接从 100% 开始
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);

	// 设置 CameraMode 初始混合权重
	CameraMode->SetBlendWeight(BlendWeight);

	// 将新的 CameraMode 插入栈顶
	CameraModeStack.Insert(CameraMode, 0);

	// 确保栈底的 CameraMode 始终保持 100% 权重
	CameraModeStack.Last()->SetBlendWeight(1.0f);

	// 如果是第一次加入栈，则通知 CameraMode 已经激活
	if (ExistingStackIndex == INDEX_NONE)
	{
		// 执行 CameraMode 的激活回调
		CameraMode->OnActivation();
	}
}

// 更新并计算整个摄像机模式栈的最终视图
bool UHodgeCameraModeStack::EvaluateStack(float DeltaTime, FHodgeCameraModeView& OutCameraModeView)
{
	// 摄像机模式栈未激活时不进行计算
	if (!bIsActive)
	{
		return false;
	}

	// 更新栈中所有 CameraMode 的视图和混合状态
	UpdateStack(DeltaTime);

	// 按照各 CameraMode 的权重混合出最终视图
	BlendStack(OutCameraModeView);

	// 表示成功计算出摄像机视图
	return true;
}

// 获取指定 CameraMode 类型对应的实例
UHodgeCameraMode* UHodgeCameraModeStack::GetCameraModeInstance(TSubclassOf<UHodgeCameraMode> CameraModeClass)
{
	// CameraModeClass 必须有效
	check(CameraModeClass);

	// 优先查找已经创建过的 CameraMode 实例
	for (UHodgeCameraMode* CameraMode : CameraModeInstances)
	{
		// 找到相同类型的实例后直接复用
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// 没有找到现有实例，需要创建新的 CameraMode
	UHodgeCameraMode* NewCameraMode = NewObject<UHodgeCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	// 将新创建的 CameraMode 加入实例缓存
	CameraModeInstances.Add(NewCameraMode);

	// 返回新创建的 CameraMode
	return NewCameraMode;
}

// 更新摄像机模式栈中所有 CameraMode 的状态
void UHodgeCameraModeStack::UpdateStack(float DeltaTime)
{
	// 获取当前摄像机模式栈大小
	const int32 StackSize = CameraModeStack.Num();

	// 没有任何 CameraMode 时不需要更新
	if (StackSize <= 0)
	{
		return;
	}

	// 记录需要从栈中删除的元素数量
	int32 RemoveCount = 0;

	// 记录开始删除的位置
	int32 RemoveIndex = INDEX_NONE;

	// 从栈顶开始依次更新每个 CameraMode
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		// 获取当前栈中的 CameraMode
		UHodgeCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		// 更新当前 CameraMode 的视图和混合权重
		CameraMode->UpdateCameraMode(DeltaTime);

		// 当前 CameraMode 已经完全生效时，它下面的 CameraMode 都不再有意义
		if (CameraMode->GetBlendWeight() >= 1.0f)
		{
			// 从当前 CameraMode 下一层开始全部可以移除
			RemoveIndex = (StackIndex + 1);

			// 计算需要移除的 CameraMode 数量
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	// 存在需要移除的 CameraMode 时执行清理
	if (RemoveCount > 0)
	{
		// 通知即将从栈中移除的 CameraMode
		for (int32 StackIndex = RemoveIndex; StackIndex < StackSize; ++StackIndex)
		{
			// 获取即将被移除的 CameraMode
			UHodgeCameraMode* CameraMode = CameraModeStack[StackIndex];
			check(CameraMode);

			// 通知 CameraMode 执行停用逻辑
			CameraMode->OnDeactivation();
		}

		// 从摄像机模式栈中删除已经没有作用的 CameraMode
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

// 将摄像机模式栈中的所有 CameraMode 混合成最终视图
void UHodgeCameraModeStack::BlendStack(FHodgeCameraModeView& OutCameraModeView) const
{
	// 获取当前摄像机模式栈大小
	const int32 StackSize = CameraModeStack.Num();

	// 没有任何 CameraMode 时无法计算最终视图
	if (StackSize <= 0)
	{
		return;
	}

	// 从栈底开始作为最终视图的基础
	const UHodgeCameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	// 使用栈底 CameraMode 的视图作为初始结果
	OutCameraModeView = CameraMode->GetCameraModeView();

	// 从栈底向栈顶依次将更高优先级的 CameraMode 混合进来
	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
	{
		// 获取当前需要混合的 CameraMode
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		// 按照当前 CameraMode 的混合权重将其视图叠加到最终结果
		OutCameraModeView.Blend(CameraMode->GetCameraModeView(), CameraMode->GetBlendWeight());
	}
}

// 绘制整个摄像机模式栈的调试信息
void UHodgeCameraModeStack::DrawDebug(UCanvas* Canvas) const
{
	// Canvas 必须有效
	check(Canvas);

	// 获取 Canvas 的调试绘制管理器
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	// 绘制调试信息标题
	DisplayDebugManager.SetDrawColor(FColor::Green);
	DisplayDebugManager.DrawString(FString(TEXT("   --- Camera Modes (Begin) ---")));

	// 遍历当前摄像机模式栈并绘制每个 CameraMode 的调试信息
	for (const UHodgeCameraMode* CameraMode : CameraModeStack)
	{
		// CameraMode 必须有效
		check(CameraMode);

		// 绘制当前 CameraMode 的调试信息
		CameraMode->DrawDebug(Canvas);
	}

	// 绘制调试信息结束标记
	DisplayDebugManager.SetDrawColor(FColor::Green);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("   --- Camera Modes (End) ---")));
}

// 获取栈顶 CameraMode 的混合权重和类型标签
void UHodgeCameraModeStack::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const
{
	// 没有 CameraMode 时返回默认权重和空标签
	if (CameraModeStack.Num() == 0)
	{
		// 没有模式时默认认为权重为 100%
		OutWeightOfTopLayer = 1.0f;

		// 没有模式时返回空 GameplayTag
		OutTagOfTopLayer = FGameplayTag();

		return;
	}
	else
	{
		// 获取栈底的 CameraMode
		UHodgeCameraMode* TopEntry = CameraModeStack.Last();
		check(TopEntry);

		// 获取该 CameraMode 当前的混合权重
		OutWeightOfTopLayer = TopEntry->GetBlendWeight();

		// 获取该 CameraMode 的类型 GameplayTag
		OutTagOfTopLayer = TopEntry->GetCameraTypeTag();
	}
}
