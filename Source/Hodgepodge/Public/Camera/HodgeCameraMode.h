/** @file HodgeCameraMode.h
 * @brief 声明相机模式与混合栈相关的类型、接口与现有处理流程。
 */
// 111 屎山代码来袭

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "HodgeCameraMode.generated.h"

class AActor;
class UCanvas;
class UHodgeCameraComponent;

/**
 * 摄像机模式之间切换时使用的混合函数
 */
UENUM(BlueprintType)
enum class EHodgeCameraModeBlendFunction : uint8
{
	// 线性插值，摄像机模式按照固定速度进行切换
	Linear,

	// 快速进入目标状态，然后逐渐减速接近目标，平滑程度由指数控制
	EaseIn,

	// 平滑进入目标状态，但接近目标时不进行减速，平滑程度由指数控制
	EaseOut,

	// 进入和退出都进行平滑处理，平滑程度由指数控制
	EaseInOut,

	// 枚举数量标记，仅供内部使用并隐藏在编辑器中
	COUNT UMETA(Hidden)
};


/**
 * 摄像机模式计算出来的最终视图数据，用于多个摄像机模式之间进行混合
 */
struct FHodgeCameraModeView
{
public:
	// 初始化默认摄像机视图数据
	/** 构造相机Mode视图并初始化其默认状态。 */
	FHodgeCameraModeView();

	// 将另一个摄像机视图按照指定权重混合到当前视图
	/** 根据给定权重混合两份状态或表现数据。
	 * @param Other Other，由调用方传入。
	 * @param OtherWeight OtherWeight，由调用方传入。
	 */
	void Blend(const FHodgeCameraModeView& Other, float OtherWeight);

public:
	// 摄像机最终位置
	FVector Location;

	// 摄像机最终旋转
	FRotator Rotation;

	// 控制器视角旋转，通常用于保存玩家实际控制方向
	FRotator ControlRotation;

	// 摄像机视野角度
	float FieldOfView;
};


/**
 * 所有摄像机模式的基类，每一种具体摄像机状态都可以继承该类实现
 */
UCLASS(Abstract, NotBlueprintable)
class HODGEPODGE_API UHodgeCameraMode : public UObject
{
	GENERATED_BODY()

public:
	// 构造摄像机模式
	/** 构造相机模式与混合栈并初始化其默认状态。 */
	UHodgeCameraMode();

	// 获取当前摄像机模式所属的 HodgeCameraComponent
	/** 获取相机模式与混合组件。
	 * @return 相机模式与混合组件的当前值或引用。
	 */
	UHodgeCameraComponent* GetHodgeCameraComponent() const;

	// 获取当前摄像机模式所在的 World
	/** 获取世界。
	 * @return 世界的当前值或引用。
	 */
	virtual UWorld* GetWorld() const override;

	// 获取当前摄像机模式对应的目标 Actor
	/** 获取目标Actor对象。
	 * @return 目标Actor对象的当前值或引用。
	 */
	AActor* GetTargetActor() const;

	// 获取当前摄像机模式计算出来的视图数据
	/** 获取相机Mode视图。
	 * @return 相机Mode视图的当前值或引用。
	 */
	const FHodgeCameraModeView& GetCameraModeView() const { return View; }

	// 摄像机模式被压入摄像机模式栈并激活时调用
	/** 处理Activation。 */
	virtual void OnActivation()
	{
	};
	/** 每次晋升栈顶时准备动态配置，包括仍在混合栈中的模式再次成为主导。 */
	virtual void OnBecomeTop() {}
	/** 电影源时间退出期间继续更新下层游戏镜头，普通模式仍沿用Lyra移除规则。 */
	virtual bool KeepUnderlyingModes() const { return false; }
	/** 本次切换可覆盖混合时长，不改变类默认配置。 */
	void SetTransitionBlendTime(float Seconds) { TransitionBlendTime = Seconds; }

	// 摄像机模式从摄像机模式栈中移除并停用时调用
	/** 处理Deactivation。 */
	virtual void OnDeactivation()
	{
	};

	// 更新摄像机模式，包括视图计算和混合权重更新
	/** 更新相机模式与混合栈。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	void UpdateCameraMode(float DeltaTime);

	// 获取当前摄像机模式的混合持续时间
	/** 获取混合时间。
	 * @return 混合时间的当前值或引用。
	 */
	float GetBlendTime() const { return TransitionBlendTime >= 0.f ? TransitionBlendTime : BlendTime; }

	// 获取当前摄像机模式的混合权重
	/** 获取混合Weight。
	 * @return 混合Weight的当前值或引用。
	 */
	float GetBlendWeight() const { return BlendWeight; }

	// 设置当前摄像机模式的混合权重
	/** 设置混合Weight。
	 * @param Weight Weight，由调用方传入。
	 */
	void SetBlendWeight(float Weight);

	// 获取当前摄像机模式的类型标签
	/** 获取相机类型标签。
	 * @return 相机类型标签的当前值或引用。
	 */
	FGameplayTag GetCameraTypeTag() const
	{
		return CameraTypeTag;
	}

	// 绘制当前摄像机模式的调试信息
	/** 按既有调试设置绘制当前状态或检测几何。
	 * @param Canvas 画布，由调用方传入。
	 */
	virtual void DrawDebug(UCanvas* Canvas) const;

protected:
	// 获取摄像机计算旋转和位置时使用的中心点位置
	/** 获取掉头位置。
	 * @return 掉头位置的当前值或引用。
	 */
	virtual FVector GetPivotLocation() const;

	// 获取摄像机计算视图时使用的中心点旋转
	/** 获取掉头旋转。
	 * @return 掉头旋转的当前值或引用。
	 */
	virtual FRotator GetPivotRotation() const;

	// 更新当前摄像机模式的视图数据
	/** 更新视图。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void UpdateView(float DeltaTime);

	// 更新当前摄像机模式的混合状态
	/** 更新Blending。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	virtual void UpdateBlending(float DeltaTime);

protected:
	// 摄像机类型标签，用于 Gameplay 查询当前激活的摄像机类型，而不需要关心具体是哪一个 CameraMode
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	FGameplayTag CameraTypeTag;

	// 当前摄像机模式计算得到的视图输出数据
	FHodgeCameraModeView View;

	// 摄像机水平视野角度，单位为度
	UPROPERTY(EditDefaultsOnly, Category = "View",
		Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	// 字段Of视图的数值或阈值。
	float FieldOfView;

	// 摄像机允许的最小俯仰角，单位为度
	UPROPERTY(EditDefaultsOnly, Category = "View",
		Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	// 视图Pitch角最小的数值或阈值。
	float ViewPitchMin;

	// 摄像机允许的最大俯仰角，单位为度
	UPROPERTY(EditDefaultsOnly, Category = "View",
		Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	// 视图Pitch角最大的数值或阈值。
	float ViewPitchMax;

	// 切换到当前摄像机模式需要持续的混合时间
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	// 当前摄像机模式使用的混合曲线类型
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	EHodgeCameraModeBlendFunction BlendFunction;

	// 控制混合曲线形状的指数参数
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	// 当前摄像机模式的线性混合进度，通常范围为 0~1
	float BlendAlpha;

	// 根据 BlendAlpha 和 BlendFunction 计算得到的最终混合权重
	float BlendWeight;
	/** -1使用原有模式配置，仅当前切换持有覆盖值。 */
	float TransitionBlendTime = -1.f;

protected:
	// 为 true 时跳过平滑插值，直接将摄像机放到理想位置，并在下一帧自动恢复
	UPROPERTY(transient)
	uint32 bResetInterpolation : 1;
};


/**
 * 摄像机模式栈，用于管理多个摄像机模式并将它们进行混合
 */
UCLASS()
class UHodgeCameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	// 构造摄像机模式栈
	/** 构造相机Mode堆叠并初始化其默认状态。 */
	UHodgeCameraModeStack();

	// 激活摄像机模式栈
	/** 激活堆叠。 */
	void ActivateStack();

	// 停用摄像机模式栈
	/** 停用堆叠。 */
	void DeactivateStack();

	// 判断摄像机模式栈当前是否处于激活状态
	/** 判断堆叠Activate。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsStackActivate() const { return bIsActive; }

	// 将指定类型的摄像机模式压入栈顶
	/** 按当前条件将相机模式加入混合栈。
	 * @param CameraModeClass 相机Mode类型，由调用方传入。
	 */
	void PushCameraMode(TSubclassOf<UHodgeCameraMode> CameraModeClass, float TransitionSeconds = -1.f);

	// 更新并计算整个摄像机模式栈最终的摄像机视图
	/** 计算堆叠。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 * @param OutCameraModeView 输出相机Mode视图。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool EvaluateStack(float DeltaTime, FHodgeCameraModeView& OutCameraModeView);

	// 绘制摄像机模式栈的调试信息
	/** 按既有调试设置绘制当前状态或检测几何。
	 * @param Canvas 画布，由调用方传入。
	 */
	void DrawDebug(UCanvas* Canvas) const;

	// 获取栈顶摄像机模式的混合权重和摄像机类型标签
	/** 获取混合信息。
	 * @param OutWeightOfTopLayer 输出WeightOfTop层。
	 * @param OutTagOfTopLayer 标签OfTop层，用于对应标签查询或匹配。
	 */
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;
	/** 返回本帧选择的栈顶类型，混合权重另由 GetBlendInfo 查询。 */
	TSubclassOf<UHodgeCameraMode> GetTopCameraModeClass() const;
	/** 只查询已缓存实例，清理不能隐式创建一个新相机。 */
	UHodgeCameraMode* FindExistingMode(TSubclassOf<UHodgeCameraMode> Class) const;

protected:
	// 获取指定摄像机模式类型对应的实例，没有实例时会创建
	/** 获取相机ModeInstance。
	 * @param CameraModeClass 相机Mode类型，由调用方传入。
	 * @return 相机ModeInstance的当前值或引用。
	 */
	UHodgeCameraMode* GetCameraModeInstance(TSubclassOf<UHodgeCameraMode> CameraModeClass);

	// 更新摄像机模式栈中所有模式的状态
	/** 更新堆叠。
	 * @param DeltaTime 本次更新的时间步长，单位秒。
	 */
	void UpdateStack(float DeltaTime);

	// 按照各摄像机模式的混合权重计算最终摄像机视图
	/** 按当前权重混合相机模式栈的输出。
	 * @param OutCameraModeView 输出相机Mode视图。
	 */
	void BlendStack(FHodgeCameraModeView& OutCameraModeView) const;

protected:
	// 摄像机模式栈当前是否处于激活状态
	bool bIsActive;

	// 已经创建过的摄像机模式实例缓存，避免重复创建相同模式
	UPROPERTY()
	TArray<TObjectPtr<UHodgeCameraMode>> CameraModeInstances;

	// 当前正在参与摄像机计算的摄像机模式栈，栈顶通常拥有最高优先级
	UPROPERTY()
	TArray<TObjectPtr<UHodgeCameraMode>> CameraModeStack;
};
