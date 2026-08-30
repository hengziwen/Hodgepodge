/**
 * @file HodgeHeroCharacter.cpp
 * @brief AHodgeHeroCharacter 类的实现
 *
 * 实现了英雄角色的:
 * - ALS 相机组件创建与输入接入(参考 AAlsCharacterExample)
 * - GAS 双初始化入口(PossessedBy + OnRep_PlayerState)
 */

#include "Character/HodgeHeroCharacter.h"

#include "AbilitySystem/HodgeGameplayTags.h"
#include "AlsCameraComponent.h"
#include "Core/PlayState/HodgePlayerStateBase.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Input/HodgeInputComponentBase.h"
#include "Utility/AlsVector.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHeroCharacter)

/**
 * @brief 构造函数
 *
 * 创建 ALS 相机组件并挂载到 Mesh,设置默认相对旋转。
 */
AHodgeHeroCharacter::AHodgeHeroCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Camera = CreateDefaultSubobject<UAlsCameraComponent>(FName{TEXTVIEW("Camera")});
	Camera->SetupAttachment(GetMesh());
	Camera->SetRelativeRotation_Direct({0.0f, 90.0f, 0.0f});
}

/**
 * @brief 创建玩家输入组件
 *
 * 返回 UHodgeInputComponentBase,使角色的输入组件具备
 * Hodgepodge 输入框架的绑定能力(BindNativeAction 等)。
 * 命名 "PawnInputComponent0" 以匹配 APawn 的默认输入组件槽位名。
 */
UInputComponent* AHodgeHeroCharacter::CreatePlayerInputComponent()
{
	return NewObject<UHodgeInputComponentBase>(this, UHodgeInputComponentBase::StaticClass(),
	                                           TEXT("PawnInputComponent0"));
}

/**
 * @brief 控制器变化时同步输入映射上下文
 *
 * 当角色被 Possess 或 Unpossess 时:
 * - 从旧的 PlayerController 移除 InputMappingContext
 * - 向新的 PlayerController 添加 InputMappingContext
 * 同时将控制器的输入缩放统一重置为 1.0。
 */
void AHodgeHeroCharacter::NotifyControllerChanged()
{
	UE_LOG(LogTemp, Warning,
	       TEXT("[HodgeHeroCharacter] NotifyControllerChanged. Controller valid=%s, InputMappingContext valid=%s"),
	       GetController() != nullptr ? TEXT("true") : TEXT("false"),
	       InputMappingContext.Get() != nullptr ? TEXT("true") : TEXT("false"));

	const auto* PreviousPlayer{Cast<APlayerController>(PreviousController)};
	if (IsValid(PreviousPlayer))
	{
		auto* InputSubsystem{
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPlayer->GetLocalPlayer())
		};
		if (IsValid(InputSubsystem))
		{
			InputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}

	auto* NewPlayer{Cast<APlayerController>(GetController())};
	if (IsValid(NewPlayer))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		auto* InputSubsystem{
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer())
		};
		if (IsValid(InputSubsystem))
		{
			FModifyContextOptions Options;
			Options.bNotifyUserSettings = true;

			InputSubsystem->AddMappingContext(InputMappingContext, 0, Options);
		}
	}

	Super::NotifyControllerChanged();
}

/**
 * @brief 计算相机视图
 *
 * 当 ALS 相机组件激活时使用相机组件的视图,否则回退到父类逻辑。
 */
void AHodgeHeroCharacter::CalcCamera(const float DeltaTime, FMinimalViewInfo& ViewInfo)
{
	if (Camera->IsActive())
	{
		Camera->GetViewInfo(ViewInfo);
		return;
	}

	Super::CalcCamera(DeltaTime, ViewInfo);
}

/**
 * @brief 设置玩家输入组件
 *
 * 将 ALS 移动所需的各个输入动作绑定到对应的处理函数。
 * 输入动作通过 UHodgeInputConfig 按 InputTag 查找并绑定,
 * 因此要求输入组件为 UHodgeInputComponentBase,且已配置 InputConfig。
 */
void AHodgeHeroCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	UE_LOG(LogTemp, Warning,
	       TEXT(
		       "[HodgeHeroCharacter] SetupPlayerInputComponent called. Input valid=%s, InputConfig valid=%s, InputMappingContext valid=%s"
	       ),
	       Input != nullptr ? TEXT("true") : TEXT("false"),
	       InputConfig.Get() != nullptr ? TEXT("true") : TEXT("false"),
	       InputMappingContext.Get() != nullptr ? TEXT("true") : TEXT("false"));

	auto* HodgeInput{Cast<UHodgeInputComponentBase>(Input)};
	if (!IsValid(HodgeInput))
	{
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "[HodgeHeroCharacter] InputComponent is NOT a UHodgeInputComponentBase (it's %s)! Input binding skipped."
		       ),
		       *GetNameSafe(Input));
		return;
	}

	if (!IsValid(InputConfig))
	{
		UE_LOG(LogTemp, Error, TEXT("[HodgeHeroCharacter] InputConfig is NULL! Input binding skipped."));
		return;
	}

	// Look
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnLookMouse, true);
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Look, ETriggerEvent::Canceled, this,
	                             &ThisClass::Input_OnLookMouse, true);

	// Move
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnMove, true);
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Move, ETriggerEvent::Canceled, this,
	                             &ThisClass::Input_OnMove, true);

	// Sprint
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Sprint, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnSprint, true);
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Sprint, ETriggerEvent::Canceled, this,
	                             &ThisClass::Input_OnSprint, true);

	// Walk
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Walk, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnWalk, true);

	// Crouch
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnCrouch, true);

	// Jump
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Jump, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnJump, true);
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Jump, ETriggerEvent::Canceled, this,
	                             &ThisClass::Input_OnJump, true);

	// Aim
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Aim, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnAim, true);
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Aim, ETriggerEvent::Canceled, this,
	                             &ThisClass::Input_OnAim, true);

	// Ragdoll
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Ragdoll, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnRagdoll, true);

	// Roll
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Roll, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnRoll, true);

	// Rotation Mode
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_RotationMode, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnRotationMode, true);

	// View Mode
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_ViewMode, ETriggerEvent::Triggered, this,
	                             &ThisClass::Input_OnViewMode, true);

	// Switch Shoulder
	HodgeInput->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_SwitchShoulder, ETriggerEvent::Triggered,
	                             this, &ThisClass::Input_OnSwitchShoulder, true);
}

/**
 * @brief 鼠标视角输入处理
 *
 * 将鼠标输入转换为控制器的 Pitch/Yaw,应用鼠标灵敏度。
 */
void AHodgeHeroCharacter::Input_OnLookMouse(const FInputActionValue& ActionValue)
{
	const FVector2f Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpMouseSensitivity);
	AddControllerYawInput(Value.X * LookRightMouseSensitivity);
}

/**
 * @brief 移动输入处理
 *
 * 将二维移动输入转换为基于视角方向的移动向量。
 */
void AHodgeHeroCharacter::Input_OnMove(const FInputActionValue& ActionValue)
{
	const auto Value{UAlsVector::ClampMagnitude012D(ActionValue.Get<FVector2D>())};

	const auto ForwardDirection{UAlsVector::AngleToDirectionXY(UE_REAL_TO_FLOAT(GetViewState().Rotation.Yaw))};
	const auto RightDirection{UAlsVector::PerpendicularCounterClockwiseXY(ForwardDirection)};

	AddMovementInput(ForwardDirection * Value.Y + RightDirection * Value.X);
}

/**
 * @brief 冲刺输入处理
 *
 * 根据输入值在冲刺/跑步之间切换目标步态。
 */
void AHodgeHeroCharacter::Input_OnSprint(const FInputActionValue& ActionValue)
{
	SetDesiredGait(ActionValue.Get<bool>() ? AlsGaitTags::Sprinting : AlsGaitTags::Running);
}

/**
 * @brief 走路/跑步切换输入处理
 */
void AHodgeHeroCharacter::Input_OnWalk()
{
	if (GetDesiredGait() == AlsGaitTags::Walking)
	{
		SetDesiredGait(AlsGaitTags::Running);
	}
	else if (GetDesiredGait() == AlsGaitTags::Running)
	{
		SetDesiredGait(AlsGaitTags::Walking);
	}
}

/**
 * @brief 下蹲输入处理
 *
 * 在站立/下蹲目标姿态之间切换。
 */
void AHodgeHeroCharacter::Input_OnCrouch()
{
	if (GetDesiredStance() == AlsStanceTags::Standing)
	{
		SetDesiredStance(AlsStanceTags::Crouching);
	}
	else if (GetDesiredStance() == AlsStanceTags::Crouching)
	{
		SetDesiredStance(AlsStanceTags::Standing);
	}
}

/**
 * @brief 跳跃输入处理
 *
 * 优先处理布娃娃停止、地面攀爬,其次处理下蹲站立,最后执行跳跃。
 */
void AHodgeHeroCharacter::Input_OnJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
	{
		if (StopRagdolling())
		{
			return;
		}

		if (StartMantlingGrounded())
		{
			return;
		}

		if (GetStance() == AlsStanceTags::Crouching)
		{
			SetDesiredStance(AlsStanceTags::Standing);
			return;
		}

		Jump();
	}
	else
	{
		StopJumping();
	}
}

/**
 * @brief 瞄准输入处理
 *
 * 根据输入值设置目标瞄准状态。
 */
void AHodgeHeroCharacter::Input_OnAim(const FInputActionValue& ActionValue)
{
	SetDesiredAiming(ActionValue.Get<bool>());
}

/**
 * @brief 布娃娃输入处理
 *
 * 若当前不在布娃娃状态则开始布娃娃;否则停止布娃娃。
 */
void AHodgeHeroCharacter::Input_OnRagdoll()
{
	if (!StopRagdolling())
	{
		StartRagdolling();
	}
}

/**
 * @brief 翻滚输入处理
 */
void AHodgeHeroCharacter::Input_OnRoll()
{
	static constexpr auto PlayRate{1.3f};

	StartRolling(PlayRate);
}

/**
 * @brief 旋转模式输入处理
 *
 * 在朝向移动方向/朝向视角方向之间切换目标旋转模式。
 */
void AHodgeHeroCharacter::Input_OnRotationMode()
{
	SetDesiredRotationMode(GetDesiredRotationMode() == AlsRotationModeTags::VelocityDirection
		                       ? AlsRotationModeTags::ViewDirection
		                       : AlsRotationModeTags::VelocityDirection);
}

/**
 * @brief 视角模式输入处理
 *
 * 在第三人称/第一人称视角之间切换。
 */
void AHodgeHeroCharacter::Input_OnViewMode()
{
	SetViewMode(GetViewMode() == AlsViewModeTags::ThirdPerson
		            ? AlsViewModeTags::FirstPerson
		            : AlsViewModeTags::ThirdPerson);
}

/**
 * @brief 切换肩部输入处理
 */
void AHodgeHeroCharacter::Input_OnSwitchShoulder()
{
	Camera->SetRightShoulder(!Camera->IsRightShoulder());
}

/**
 * @brief 服务器端角色被 Possess 时的回调(重写自 ACharacter)
 *
 * 这是 GAS 服务器端初始化的入口:
 * 1. 调用 Super::PossessedBy() 执行父类逻辑
 *    (AAlsCharacter 会调用 RefreshMeshProperties 刷新网格属性)
 * 2. 获取 AHodgePlayerStateBase 的 PlayerState
 * 3. 调用 InitializeAbilitySystemForCharacter(this) 完成 GAS 初始化
 *
 * @param NewController 新的控制器指针
 */
void AHodgeHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AHodgePlayerStateBase* PS = GetPlayerState<AHodgePlayerStateBase>())
	{
		PS->InitializeAbilitySystemForCharacter(this);
	}
}

/**
 * @brief 客户端 PlayerState 复制到位时的回调(重写自 ACharacter)
 *
 * 这是 GAS 客户端初始化的入口:
 * 1. 调用 Super::OnRep_PlayerState() 执行父类逻辑
 * 2. 获取 AHodgePlayerStateBase 的 PlayerState
 * 3. 调用 InitializeAbilitySystemForCharacter(this) 完成 GAS 初始化
 *
 * 注意:此函数仅在客户端调用,因为 OnRep_ 是属性复制回调。
 */
void AHodgeHeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AHodgePlayerStateBase* PS = GetPlayerState<AHodgePlayerStateBase>())
	{
		PS->InitializeAbilitySystemForCharacter(this);
	}
}

/**
 * @brief 调试显示
 *
 * 当 ALS 相机组件激活时,先显示相机调试信息,再显示角色调试信息。
 */
void AHodgeHeroCharacter::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused,
                                       float& VerticalLocation)
{
	if (Camera->IsActive())
	{
		Camera->DisplayDebug(Canvas, DisplayInfo, VerticalLocation);
	}

	Super::DisplayDebug(Canvas, DisplayInfo, Unused, VerticalLocation);
}
