// Copyright Epic Games, Inc. All Rights Reserved.

// HeroComponent 本体定义。
#include "Component/HodgeHeroComponent.h"

// GameFramework 组件委托相关定义。
#include "Components/GameFrameworkComponentDelegates.h"

// 编辑器 MessageLog 日志系统。
#include "Logging/MessageLog.h"

// Enhanced Input 本地玩家子系统。
#include "EnhancedInputSubsystems.h"

// 项目自定义 PlayerController。
#include "Core/PlayerController/HodgePlayerControllerBase.h"

// 项目自定义 PlayerState。
#include "Core/PlayState/HodgePlayerState.h"

// 项目自定义 LocalPlayer。
#include "Core/LocalPlayer/HodgeLocalPlayerBase.h"

// PawnExtensionComponent：负责 PawnData、ASC 和初始化状态协调。
#include "Component/HodgePawnExtensionComponent.h"

// PawnData：保存 Pawn 的 Ability、Input、Camera 等配置。
#include "Data/HodgePawnData.h"

// 项目战斗角色基类。
#include "Character/HodgeCombatCharacter.h"

// 项目自定义 AbilitySystemComponent。
#include "AbilitySystem/HodgeAbilitySystemComponent.h"

// 项目输入配置资产。
#include "Input/HodgeInputConfig.h"

// 项目自定义 InputComponent。
#include "Input/HodgeInputComponent.h"

// 项目自定义 CameraComponent。
#include "Camera/HodgeCameraComponent.h"

// 项目原生 GameplayTag 定义。
#include "AbilitySystem/HodgeGameplayTags.h"

// GameFrameworkComponentManager：负责 InitState 和扩展事件。
#include "Components/GameFrameworkComponentManager.h"

// 玩家可映射输入配置。
#include "PlayerMappableInputConfig.h"

// 项目相机模式。
#include "Camera/HodgeCameraMode.h"

// Enhanced Input 用户输入设置。
#include "UserSettings/EnhancedInputUserSettings.h"

// Enhanced Input MappingContext。
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeHeroComponent)

#if WITH_EDITOR
// 编辑器 MessageLog 中用于关联 UObject 的 Token。
#include "Misc/UObjectToken.h"
#endif  // WITH_EDITOR

// HeroComponent 内部使用的常量。
namespace HodgeHero
{
	// 手柄水平视角每秒最大旋转速率。
	static const float LookYawRate = 300.0f;

	// 手柄垂直视角每秒最大旋转速率。
	static const float LookPitchRate = 165.0f;
};

// 输入完成初始化后广播的 GameFramework 扩展事件名称。
const FName UHodgeHeroComponent::NAME_BindInputsNow("BindInputsNow");

// HeroComponent 在 InitState 系统中的 Feature 名称。
const FName UHodgeHeroComponent::NAME_ActorFeatureName("Hero");

// HeroComponent 构造函数。
UHodgeHeroComponent::UHodgeHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 默认没有 GameplayAbility 覆盖相机模式。
	AbilityCameraMode = nullptr;

	// 默认输入尚未完成绑定。
	bReadyToBindInputs = false;
}

// 组件注册到 Actor 时调用。
void UHodgeHeroComponent::OnRegister()
{
	// 先执行父类组件注册逻辑。
	Super::OnRegister();

	// HeroComponent 必须挂载在 Pawn 类型 Actor 上。
	if (!GetPawn<APawn>())
	{
		// 运行时输出错误日志，提示组件挂载类型错误。
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "[UHodgeHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."
		       ));

#if WITH_EDITOR
		// 只在编辑器环境中额外显示 MessageLog 错误。
		if (GIsEditor)
		{
			// 编辑器错误提示文本。
			static const FText Message = NSLOCTEXT("HodgeHeroComponent", "NotOnPawnError",
			                                       "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");

			// MessageLog 分类名称。
			static const FName HeroMessageLogName = TEXT("HodgeHeroComponent");

			// 创建错误信息，并附加当前组件对象与提示文本。
			FMessageLog(HeroMessageLogName).Error()
			                               ->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
			                               ->AddToken(FTextToken::Create(Message));

			// 打开对应 MessageLog 面板。
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		// 尽早将 HeroComponent 注册到 GameFramework InitState 系统。
		RegisterInitStateFeature();
	}
}

// 判断 HeroComponent 是否可以从当前 InitState 切换到目标 InitState。
bool UHodgeHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                             FGameplayTag DesiredState) const
{
	// InitState 流程必须依赖有效的 ComponentManager。
	check(Manager);

	// 获取当前组件所属 Pawn。
	APawn* Pawn = GetPawn<APawn>();

	// 未进入任何状态 → Spawned。
	if (!CurrentState.IsValid() && DesiredState == HodgeGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		// 只要 Pawn 已经真实存在，就允许进入 Spawned。
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned → DataAvailable。
	else if (CurrentState == HodgeGameplayTags::InitState_Spawned && DesiredState ==
		HodgeGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		// HeroComponent 初始化依赖 PlayerState。
		if (!GetPlayerState<AHodgePlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		// Authority 或 AutonomousProxy 需要等待 Controller 与 PlayerState 的拥有关系建立完成。
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			// 获取当前 Pawn 的 Controller。
			AController* Controller = GetController<AController>();

			// 检查 Controller 是否存在、是否持有 PlayerState，以及 PlayerState 的 Owner 是否就是该 Controller。
			const bool bHasControllerPairedWithPS = (Controller != nullptr) &&
				(Controller->PlayerState != nullptr) &&
				(Controller->PlayerState->GetOwner() == Controller);

			// Controller 与 PlayerState 尚未完成配对时不能继续初始化。
			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		// 判断 Pawn 是否由本地客户端控制。
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		// 判断 Pawn 是否由 AI/Bot 控制。
		const bool bIsBot = Pawn->IsBotControlled();

		// 真实本地玩家还需要等待本地输入系统准备完成。
		if (bIsLocallyControlled && !bIsBot)
		{
			// 获取项目自定义 PlayerController。
			AHodgePlayerControllerBase* HodgePC = GetController<AHodgePlayerControllerBase>();

			// The input component and local player is required when locally controlled.
			// 本地玩家必须已经拥有 InputComponent、PlayerController 和 LocalPlayer。
			if (!Pawn->InputComponent || !HodgePC || !HodgePC->GetLocalPlayer())
			{
				return false;
			}
		}

		// 当前阶段所有依赖已经准备完成。
		return true;
	}

	// DataAvailable → DataInitialized。
	else if (CurrentState == HodgeGameplayTags::InitState_DataAvailable && DesiredState ==
		HodgeGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		// 等待 PlayerState 和 PawnExtensionComponent 完成初始化。
		AHodgePlayerState* HodgePS = GetPlayerState<AHodgePlayerState>();

		// HeroComponent 必须等 PawnExtensionComponent 先到达 DataInitialized。
		return HodgePS && Manager->HasFeatureReachedInitState(Pawn, UHodgePawnExtensionComponent::NAME_ActorFeatureName,
		                                                      HodgeGameplayTags::InitState_DataInitialized);
	}

	// DataInitialized → GameplayReady。
	else if (CurrentState == HodgeGameplayTags::InitState_DataInitialized && DesiredState ==
		HodgeGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		// 当前没有额外检查，DataInitialized 后直接允许进入 GameplayReady。
		return true;
	}

	// 其他未定义的状态切换全部禁止。
	return false;
}

// InitState 真正发生状态切换时执行对应初始化逻辑。
void UHodgeHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                                FGameplayTag DesiredState)
{
	// HeroComponent 的核心初始化工作放在 DataAvailable → DataInitialized 阶段。
	if (CurrentState == HodgeGameplayTags::InitState_DataAvailable && DesiredState ==
		HodgeGameplayTags::InitState_DataInitialized)
	{
		// 获取当前 Pawn。
		APawn* Pawn = GetPawn<APawn>();

		// 获取当前玩家的 PlayerState。
		AHodgePlayerState* HodgePS = GetPlayerState<AHodgePlayerState>();

		// Pawn 和 PlayerState 都必须有效。
		if (!ensure(Pawn && HodgePS))
		{
			return;
		}

		// 准备获取当前 Pawn 的 PawnData。
		const UHodgePawnData* PawnData = nullptr;

		// 查找负责 Pawn 核心初始化的 PawnExtensionComponent。
		if (UHodgePawnExtensionComponent* PawnExtComp = UHodgePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			// 从 PawnExtensionComponent 获取当前 PawnData。
			PawnData = PawnExtComp->GetPawnData<UHodgePawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// PlayerState 保存玩家跨 Pawn、跨死亡持续存在的数据。

			// The ability system component and attribute sets live on the player state.
			// ASC 和 AttributeSet 放在 PlayerState 上，使换 Pawn 后能力数据仍然可以继续保留。

			// 将 PlayerState 上的 ASC 初始化到当前 Pawn，并让 PlayerState 作为 OwnerActor。
			PawnExtComp->InitializeAbilitySystem(HodgePS->GetHodgeAbilitySystemComponent(), HodgePS);
		}

		// 如果当前 Pawn 由项目 PlayerController 控制。
		if (AHodgePlayerControllerBase* HodgePC = GetController<AHodgePlayerControllerBase>())
		{
			// 输入组件已经创建时开始初始化玩家输入。
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// Hook up the delegate for all pawns, in case we spectate later
		// 为 Pawn 绑定相机模式查询委托，即使未来进入观战也能复用同一套相机逻辑。
		if (PawnData)
		{
			// 查找 Pawn 上的 HodgeCameraComponent。
			if (UHodgeCameraComponent* CameraComponent = UHodgeCameraComponent::FindCameraComponent(Pawn))
			{
				// 让 CameraComponent 通过 HeroComponent 动态决定当前 CameraMode。
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}

// 同一个 Actor 上其他 InitState Feature 状态变化时收到通知。
void UHodgeHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// 这里只关注 PawnExtensionComponent 的初始化状态变化。
	if (Params.FeatureName == UHodgePawnExtensionComponent::NAME_ActorFeatureName)
	{
		// PawnExtensionComponent 已经到达 DataInitialized。
		if (Params.FeatureState == HodgeGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			// PawnExtension 初始化完成后，HeroComponent 再尝试继续推进自己的 InitState。
			CheckDefaultInitialization();
		}
	}
}

// 按项目规定的默认状态链持续推进 HeroComponent 初始化。
void UHodgeHeroComponent::CheckDefaultInitialization()
{
	// HeroComponent 标准 InitState 状态链。
	static const TArray<FGameplayTag> StateChain = {
		HodgeGameplayTags::InitState_Spawned, HodgeGameplayTags::InitState_DataAvailable,
		HodgeGameplayTags::InitState_DataInitialized, HodgeGameplayTags::InitState_GameplayReady
	};

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	// 从当前状态开始持续尝试向后推进，直到某个阶段的前置条件尚未满足。
	ContinueInitStateChain(StateChain);
}

// 游戏开始时启动 HeroComponent 的 InitState 初始化流程。
void UHodgeHeroComponent::BeginPlay()
{
	// 先执行父类 BeginPlay。
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	// 监听 PawnExtensionComponent 的 InitState 变化。
	BindOnActorInitStateChanged(UHodgePawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	// 首先尝试进入 Spawned，表示组件已经完成基础创建。
	ensure(TryToChangeInitState(HodgeGameplayTags::InitState_Spawned));

	// 然后继续尝试推进后续初始化状态。
	CheckDefaultInitialization();
}

// HeroComponent 生命周期结束时调用。
void UHodgeHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 从 GameFramework InitState 系统中注销当前 Feature。
	UnregisterInitStateFeature();

	// 执行父类 EndPlay。
	Super::EndPlay(EndPlayReason);
}

// 初始化本地玩家的 Enhanced Input。
void UHodgeHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	// 输入组件必须存在。
	check(PlayerInputComponent);

	// 获取当前 Pawn。
	const APawn* Pawn = GetPawn<APawn>();

	// Pawn 无效时无法初始化输入。
	if (!Pawn)
	{
		return;
	}

	// 获取当前 Pawn 的 PlayerController。
	const APlayerController* PC = GetController<APlayerController>();

	// 本地玩家输入初始化要求 PlayerController 有效。
	check(PC);

	// 获取项目自定义 LocalPlayer。
	const UHodgeLocalPlayerBase* LP = Cast<UHodgeLocalPlayerBase>(PC->GetLocalPlayer());

	// 本地玩家必须存在。
	check(LP);

	// 获取 Enhanced Input 本地玩家子系统。
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	// Enhanced Input 子系统必须有效。
	check(Subsystem);

	// 清空当前 LocalPlayer 已经添加的所有 MappingContext。
	Subsystem->ClearAllMappings();

	// 查找 PawnExtensionComponent。
	if (const UHodgePawnExtensionComponent* PawnExtComp =
		UHodgePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		// 获取当前 PawnData。
		if (const UHodgePawnData* PawnData = PawnExtComp->GetPawnData<UHodgePawnData>())
		{
			// 获取 PawnData 配置的 InputConfig。
			if (const UHodgeInputConfig* InputConfig = PawnData->InputConfig)
			{
				// 遍历 HeroComponent 自身配置的默认 MappingContext。
				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					// 获取已经加载完成的 InputMappingContext。
					if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
					{
						// 当前 Mapping 配置要求注册到玩家设置系统。
						if (Mapping.bRegisterWithSettings)
						{
							// 获取 Enhanced Input 的用户设置对象。
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								// 将 MappingContext 注册到用户输入设置，支持玩家重映射等功能。
								Settings->RegisterInputMappingContext(IMC);
							}

							// 创建 MappingContext 添加选项。
							FModifyContextOptions Options = {};

							// 添加 MappingContext 时不忽略当前已经按住的按键。
							Options.bIgnoreAllPressedKeysUntilRelease = false;

							// Actually add the config to the local player
							// 真正将 MappingContext 添加到当前 LocalPlayer。
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				// The Hodge Input Component has some additional functions to map Gameplay Tags to an Input Action.
				// HodgeInputComponent 扩展了 GameplayTag → InputAction 的绑定能力。

				// If you want this functionality but still want to change your input component class, make it a subclass
				// 如果希望替换 InputComponent，同时继续使用这套输入 Tag 系统，则新组件必须继承 UHodgeInputComponent。

				// of the UHodgeInputComponent or modify this component accordingly.
				// 否则需要自行实现等价的 GameplayTag 输入绑定逻辑。

				// 将当前 InputComponent 转换为项目自定义 HodgeInputComponent。
				UHodgeInputComponent* HodgeIC = Cast<UHodgeInputComponent>(PlayerInputComponent);

				// 确保 Pawn 使用的是正确的 InputComponent 类型。
				if (ensureMsgf(
					HodgeIC,
					TEXT(
						"Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UHodgeInputComponent or a subclass of it."
					)))
				{
					// Add the key mappings that may have been set by the player
					// 应用玩家可能已经修改过的按键映射配置。
					HodgeIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// 在这里真正建立 Ability InputAction → GameplayTag 的绑定关系。

					// be triggered directly by these input actions Triggered events. 
					// 输入事件触发后，会通过 GameplayTag 将输入转发到 ASC。

					// 保存本次 Ability 输入绑定产生的句柄。
					TArray<uint32> BindHandles;

					// 批量绑定 Ability 类型输入的 Pressed 和 Released 回调。
					HodgeIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed,
					                            &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					// 将 Move InputTag 对应的 InputAction 绑定到角色移动函数。
					HodgeIC->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Move, ETriggerEvent::Triggered,
					                          this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);

					// 将鼠标 Look InputTag 绑定到鼠标视角函数。
					HodgeIC->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Look_Mouse,
					                          ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse,
					                          /*bLogIfNotFound=*/ false);

					// 将手柄 Look InputTag 绑定到摇杆视角函数。
					HodgeIC->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Look_Stick,
					                          ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick,
					                          /*bLogIfNotFound=*/ false);

					// 将 Crouch InputTag 绑定到蹲伏处理函数。
					HodgeIC->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered,
					                          this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);

					// 将 AutoRun InputTag 绑定到自动奔跑处理函数。
					HodgeIC->BindNativeAction(InputConfig, HodgeGameplayTags::InputTag_AutoRun,
					                          ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun,
					                          /*bLogIfNotFound=*/ false);
				}
			}
		}
	}

	// 正常情况下玩家输入初始化只应该完成一次。
	if (ensure(!bReadyToBindInputs))
	{
		// 标记 HeroComponent 已经允许 GameFeature 等系统继续动态追加输入。
		bReadyToBindInputs = true;
	}

	// 向 PlayerController 广播 BindInputsNow 扩展事件。
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		const_cast<APlayerController*>(PC), NAME_BindInputsNow);

	// 向 Pawn 广播 BindInputsNow 扩展事件。
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

// 动态为当前玩家添加额外的 Ability 输入配置。
void UHodgeHeroComponent::AddAdditionalInputConfig(const UHodgeInputConfig* InputConfig)
{
	// 保存本次新增输入绑定产生的句柄。
	TArray<uint32> BindHandles;

	// 获取当前 Pawn。
	const APawn* Pawn = GetPawn<APawn>();

	// Pawn 不存在时不能添加输入绑定。
	if (!Pawn)
	{
		return;
	}

	// 获取当前 Pawn 的 PlayerController。
	const APlayerController* PC = GetController<APlayerController>();

	// 动态输入绑定只应该发生在有效 PlayerController 上。
	check(PC);

	// 获取当前本地玩家。
	const ULocalPlayer* LP = PC->GetLocalPlayer();

	// LocalPlayer 必须有效。
	check(LP);

	// 获取 Enhanced Input 本地玩家子系统。
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	// Enhanced Input 子系统必须有效。
	check(Subsystem);

	// 确保 Pawn 存在 PawnExtensionComponent。
	if (const UHodgePawnExtensionComponent* PawnExtComp =
		UHodgePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		// 从 Pawn 上查找项目自定义 InputComponent。
		UHodgeInputComponent* HodgeIC = Pawn->FindComponentByClass<UHodgeInputComponent>();

		// 确保 InputComponent 类型满足项目输入框架要求。
		if (ensureMsgf(
			HodgeIC,
			TEXT(
				"Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UHodgeInputComponent or a subclass of it."
			)))
		{
			// 将额外 InputConfig 中的 Ability 输入动态绑定到当前 HeroComponent。
			HodgeIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed,
			                            &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

// 移除之前动态添加的额外输入配置。
void UHodgeHeroComponent::RemoveAdditionalInputConfig(const UHodgeInputConfig* InputConfig)
{
	//@TODO: Implement me!
	// 当前尚未实现对应输入绑定的解除逻辑。
}

// 返回当前 HeroComponent 是否已经完成基础玩家输入初始化。
bool UHodgeHeroComponent::IsReadyToBindInputs() const
{
	// GameFeature AddInputBinding 等系统会通过这个标记判断是否可以追加输入。
	return bReadyToBindInputs;
}

// Ability 类型输入按下时调用。
void UHodgeHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	// 获取当前 Pawn。
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		// 通过 PawnExtensionComponent 获取 Pawn 使用的 ASC。
		if (const UHodgePawnExtensionComponent* PawnExtComp =
			UHodgePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			// 获取项目自定义 ASC。
			if (UHodgeAbilitySystemComponent* HodgeASC = PawnExtComp->GetHodgeAbilitySystemComponent())
			{
				// 将“某个 InputTag 被按下”的信息转发给 ASC。
				HodgeASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

// Ability 类型输入松开时调用。
void UHodgeHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	// 获取当前 Pawn。
	const APawn* Pawn = GetPawn<APawn>();

	// Pawn 不存在时直接返回。
	if (!Pawn)
	{
		return;
	}

	// 查找 PawnExtensionComponent。
	if (const UHodgePawnExtensionComponent* PawnExtComp =
		UHodgePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		// 获取项目自定义 ASC。
		if (UHodgeAbilitySystemComponent* HodgeASC = PawnExtComp->GetHodgeAbilitySystemComponent())
		{
			// 将“某个 InputTag 被松开”的信息转发给 ASC。
			HodgeASC->AbilityInputTagReleased(InputTag);
		}
	}
}

// 处理角色二维移动输入。
void UHodgeHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	// 获取当前 Pawn。
	APawn* Pawn = GetPawn<APawn>();

	// 获取控制当前 Pawn 的 Controller。
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	// 如果玩家主动产生移动输入，可以在这里关闭自动奔跑。
	if (AHodgePlayerControllerBase* HodgeController = Cast<AHodgePlayerControllerBase>(Controller))
	{
		//HodgeController->SetIsAutoRunning(false);
	}

	// 只有存在 Controller 时才计算基于控制器朝向的移动方向。
	if (Controller)
	{
		// 获取二维移动输入，其中 X 通常表示左右，Y 表示前后。
		const FVector2D Value = InputActionValue.Get<FVector2D>();

		// 只使用 ControlRotation 的 Yaw，忽略 Pitch 和 Roll。
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		// 存在左右输入时处理横向移动。
		if (Value.X != 0.0f)
		{
			// 将角色本地 RightVector 根据控制器水平朝向旋转到世界空间。
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);

			// 向 Pawn 添加横向移动输入。
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		// 存在前后输入时处理纵向移动。
		if (Value.Y != 0.0f)
		{
			// 将角色本地 ForwardVector 根据控制器水平朝向旋转到世界空间。
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);

			// 向 Pawn 添加前后移动输入。
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

// 处理鼠标视角输入。
void UHodgeHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	// 获取当前 Pawn。
	APawn* Pawn = GetPawn<APawn>();

	// Pawn 不存在时无法控制视角。
	if (!Pawn)
	{
		return;
	}

	// 获取鼠标二维视角输入。
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	// 存在水平输入时修改 Controller Yaw。
	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	// 存在垂直输入时修改 Controller Pitch。
	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

// 处理手柄摇杆视角输入。
void UHodgeHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	// 获取当前 Pawn。
	APawn* Pawn = GetPawn<APawn>();

	// Pawn 不存在时无法控制视角。
	if (!Pawn)
	{
		return;
	}

	// 获取摇杆二维输入。
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	// 获取 World，用于取得 DeltaSeconds。
	const UWorld* World = GetWorld();

	// World 必须有效。
	check(World);

	// 存在水平摇杆输入时旋转 Yaw。
	if (Value.X != 0.0f)
	{
		// 摇杆输入按“输入值 × 每秒速率 × DeltaTime”转换成每帧旋转量。
		Pawn->AddControllerYawInput(Value.X * HodgeHero::LookYawRate * World->GetDeltaSeconds());
	}

	// 存在垂直摇杆输入时旋转 Pitch。
	if (Value.Y != 0.0f)
	{
		// 摇杆输入按“输入值 × 每秒速率 × DeltaTime”转换成每帧旋转量。
		Pawn->AddControllerPitchInput(Value.Y * HodgeHero::LookPitchRate * World->GetDeltaSeconds());
	}
}

// 处理蹲伏输入。
void UHodgeHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	// 当前 Pawn 是 HodgeCombatCharacter 时切换蹲伏状态。
	if (AHodgeCombatCharacter* Character = GetPawn<AHodgeCombatCharacter>())
	{
		Character->ToggleCrouch();
	}
}

// 处理自动奔跑输入。
void UHodgeHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
{
	// 获取当前 Pawn。
	if (APawn* Pawn = GetPawn<APawn>())
	{
		// 获取项目自定义 PlayerController。
		if (AHodgePlayerControllerBase* Controller = Cast<AHodgePlayerControllerBase>(Pawn->GetController()))
		{
			// Toggle auto running
			// 在这里切换自动奔跑状态。

			//Controller->SetIsAutoRunning(!Controller->GetIsAutoRunning());
		}
	}
}

// 决定当前 Pawn 应该使用哪个 CameraMode。
TSubclassOf<UHodgeCameraMode> UHodgeHeroComponent::DetermineCameraMode() const
{
	// GameplayAbility 指定了临时 CameraMode 时拥有最高优先级。
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	// 获取当前 Pawn。
	const APawn* Pawn = GetPawn<APawn>();

	// Pawn 无效时无法获取默认 CameraMode。
	if (!Pawn)
	{
		return nullptr;
	}

	// 查找 PawnExtensionComponent。
	if (UHodgePawnExtensionComponent* PawnExtComp = UHodgePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		// 获取 PawnData。
		if (const UHodgePawnData* PawnData = PawnExtComp->GetPawnData<UHodgePawnData>())
		{
			// 没有 Ability 覆盖时使用 PawnData 配置的默认 CameraMode。
			return PawnData->DefaultCameraMode;
		}
	}

	// 没有找到有效 CameraMode。
	return nullptr;
}

// 允许 GameplayAbility 临时覆盖当前 CameraMode。
void UHodgeHeroComponent::SetAbilityCameraMode(TSubclassOf<UHodgeCameraMode> CameraMode,
                                               const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	// 只有传入有效 CameraMode 时才更新。
	if (CameraMode)
	{
		// 保存 Ability 指定的相机模式。
		AbilityCameraMode = CameraMode;

		// 保存这次相机覆盖所属的 AbilitySpecHandle。
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

// 清除 GameplayAbility 设置的 CameraMode 覆盖。
void UHodgeHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	// 只有发起清除请求的 Ability 正是当前相机覆盖拥有者时才允许清除。
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		// 清空 Ability 相机模式。
		AbilityCameraMode = nullptr;

		// 重置当前相机覆盖对应的 AbilitySpecHandle。
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}
