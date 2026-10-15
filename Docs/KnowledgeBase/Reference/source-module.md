# Module 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityEditor.Build.cs

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/HodgeAbilityEditor.Build.cs](../../../Source/HodgeAbilityEditor/HodgeAbilityEditor.Build.cs)

定义候选（多行签名仅展示首行）：


## HodgeAbilityEditorModule.cpp

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeAbilityEditorModule.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAbilityEditorModule.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：


## HodgeAnimationAuthoringLibrary.cpp

Editor 动画文本导入、属性/引用修正、编译诊断与实验 PIE 配置。

源码：[Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Animation/HodgeRootMotionModifier_AttackFacing.h](../../../Source/Hodgepodge/Public/Animation/HodgeRootMotionModifier_AttackFacing.h)、[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[Equipment/HodgeEquipmentManagerComponent.h](../../../Source/Hodgepodge/Public/Equipment/HodgeEquipmentManagerComponent.h)、[AI/HodgeBTTask_ExecuteAbility.h](../../../Source/Hodgepodge/Public/AI/HodgeBTTask_ExecuteAbility.h)、[Animation/HodgeEnemyAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeEnemyAnimInstance.h)、[Animation/HodgeWingAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingAnimInstance.h)、[Animation/HodgeAnimNotifyState_WingAction.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_WingAction.h)、[Animation/HodgeAnimNotifyState_CinematicCamera.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_CinematicCamera.h)、[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)、[Animation/HodgeAnimNotify_DarkSurgeCommit.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotify_DarkSurgeCommit.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[Data/HodgeCinematicCameraProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[Component/HodgeWingComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeWingComponent.h)、[Animation/HodgeWingProfile.h](../../../Source/Hodgepodge/Public/Animation/HodgeWingProfile.h)、[Animation/HodgeAnimInstance.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimInstance.h)

定义候选（多行签名仅展示首行）：

- L104: `FString UHodgeAnimationAuthoringLibrary::ConfigureWingBoxHit(UAnimMontage* Montage, UHodgeWingProfile* Profile, FName AnchorBone, float Margin)`
- L324: `bool UHodgeAnimationAuthoringLibrary::ConfigureWingActionWindow(UAnimSequenceBase* Source, float Begin, float End)`
- L353: `bool UHodgeAnimationAuthoringLibrary::ConfigureWingMontageGraph(UAnimBlueprint* Blueprint)`
- L379: `bool UHodgeAnimationAuthoringLibrary::ConfigureWingMontageAsset(UAnimMontage* Montage, UAnimSequence* Sequence)`
- L406: `bool UHodgeAnimationAuthoringLibrary::ConfigureHavocMontage(UAnimMontage* Montage, UAnimSequence* Source,`
- L457: `bool UHodgeAnimationAuthoringLibrary::ConfigureHavocCastCommit(UAnimMontage* Montage, float Time)`
- L472: `bool UHodgeAnimationAuthoringLibrary::ConfigureLiberationMontage(UAnimMontage* Montage, UAnimSequence* Source,`
- L512: `bool UHodgeAnimationAuthoringLibrary::ConfigureWingAttackPreview(UBlueprint* Blueprint, UHodgeWingProfile* Profile)`
- L549: `bool UHodgeAnimationAuthoringLibrary::ConfigureWingAnimation(UAnimBlueprint* Blueprint)`
- L598: `bool UHodgeAnimationAuthoringLibrary::ConfigureWingExperience(UBlueprint* Experience, TSubclassOf<AActor> ActorClass, TSubclassOf<UActorComponent> ComponentClass)`
- L624: `bool UHodgeAnimationAuthoringLibrary::ConfigureWingFinisher(UAnimMontage* Montage, UHodgeWingProfile* Profile)`
- L669: `UAnimBlueprint* UHodgeAnimationAuthoringLibrary::ImportAnimationBlueprintText(const FString& AssetPath, const FString& TextFilename, UClass* ParentClass, USkeleton* TargetSkeleton)`
- L737: `bool UHodgeAnimationAuthoringLibrary::SetAnimationDefault(UBlueprint* Blueprint, const FString& PropertyName, const FString& Value)`
- L747: `bool UHodgeAnimationAuthoringLibrary::SetAnimationNodeProperty(UBlueprint* Blueprint, const FString& NodePath, const FString& PropertyPath, const FString& Value)`
- L759: `bool UHodgeAnimationAuthoringLibrary::RemapAnimationReferences(UBlueprint* Blueprint, const TArray<UObject*>& Sources, const TArray<UObject*>& Destinations)`
- L783: `FString UHodgeAnimationAuthoringLibrary::CompileAnimationBlueprint(UBlueprint* Blueprint)`
- L807: `TArray<FName> UHodgeAnimationAuthoringLibrary::GetSkeletonBoneNames(USkeleton* Skeleton)`
- L818: `bool UHodgeAnimationAuthoringLibrary::SetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot, FName Group)`
- L830: `FName UHodgeAnimationAuthoringLibrary::GetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot)`
- L836: `bool UHodgeAnimationAuthoringLibrary::SetHeroReactionMontageSlot(UAnimMontage* Montage, FName Slot)`
- L848: `bool UHodgeAnimationAuthoringLibrary::SetHeroReactionSkeleton(UAnimationAsset* Asset, USkeleton* Skeleton)`
- L859: `bool UHodgeAnimationAuthoringLibrary::ConfigureHeroHitStunDuration(UAnimMontage* Montage, float Duration)`
- L877: `bool UHodgeAnimationAuthoringLibrary::ConfigureHeroHitReactionGraph(UAnimBlueprint* Blueprint)`
- L939: `FString UHodgeAnimationAuthoringLibrary::ConfigureHeroDashFootIK(UAnimBlueprint* LayerBlueprint)`
- L967: `bool UHodgeAnimationAuthoringLibrary::ConfigureHeroMovementMontageSync(UAnimMontage* Montage)`
- L976: `FString UHodgeAnimationAuthoringLibrary::ConfigureHeroSprintSync(UAnimBlueprint* LayerBlueprint)`
- L1016: `FString UHodgeAnimationAuthoringLibrary::ConfigureHeroFacingModes(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint)`
- L1100: `FString UHodgeAnimationAuthoringLibrary::ConfigureHeroSprintAnimations(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint)`
- L1175: `FString UHodgeAnimationAuthoringLibrary::ConfigureHeroSprintPivotTiming(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint)`
- L1245: `bool UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabPIE(int32 PlayerCount, bool bListenServer)`
- L1256: `bool UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabGameMode(UBlueprint* Blueprint, TSubclassOf<APawn> PawnClass)`
- L1308: `bool UHodgeAnimationAuthoringLibrary::SetEnemyMeshSkeleton(USkeletalMesh* Mesh, USkeleton* Skeleton)`
- L1315: `bool UHodgeAnimationAuthoringLibrary::ConfigureEnemyBehaviorTree(UBehaviorTree* Tree, UBlackboardData* BB, FGameplayTag AttackTag, bool bComboIntent)`
- L1382: `bool UHodgeAnimationAuthoringLibrary::ConfigureEnemyAnimation(UAnimBlueprint* Blueprint, UAnimSequence* Idle, UAnimSequence* Run, FName SpineBone)`
- L1426: `bool UHodgeAnimationAuthoringLibrary::ConfigureEnemyAnimationAsset(UAnimationAsset* Asset, USkeleton* Skeleton, FName Slot)`
- L1442: `bool UHodgeAnimationAuthoringLibrary::ConfigureEnemyNavigationBounds(ANavMeshBoundsVolume* Volume, FVector Size)`
- L1463: `bool UHodgeAnimationAuthoringLibrary::ConfigureEnemyExperienceComponents(UBlueprint* Experience)`
- L1486: `FString UHodgeAnimationAuthoringLibrary::ConfigureAttackFacingWarp(UHodgeAbilityDefinition* Definition, float StartTime, float EndTime)`

## HodgeAnimationAuthoringLibrary.h

Editor 动画文本导入、属性/引用修正、编译诊断与实验 PIE 配置。

源码：[Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeAnimationAuthoringLibrary.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Kismet/BlueprintFunctionLibrary.h"
   8: #include "GameplayTagContainer.h"
  10: #include "HodgeAnimationAuthoringLibrary.generated.h"
  12: class UAnimBlueprint;
  13: class UBlueprint;
  14: class USkeleton;
  15: class UAnimMontage;
  16: class UHodgeAbilityDefinition;
  17: class UAnimationAsset;
  18: class APawn;
  19: class UBehaviorTree;
  20: class UBlackboardData;
  21: class UAnimSequence;
  22: class UAnimSequenceBase;
  23: class UHodgeWingProfile;
  24: class USkeletalMesh;
  25: class UHodgeCinematicCameraProfile;
  26: class UHodgeHitDetectionProfile;
  27: class ANavMeshBoundsVolume;
  30: UCLASS()
  31: class HODGEABILITYEDITOR_API UHodgeAnimationAuthoringLibrary : public UBlueprintFunctionLibrary
  32: {
  34: 	GENERATED_BODY()
  36: public:
  38: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Havoc")
  39: 	static bool ConfigureHavocMontage(UAnimMontage* Montage, UAnimSequence* Source,
  40: 		float HitStart, float HitEnd, float RecoveryStart, bool bCircuit, bool bSkill);
  42: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Havoc")
  43: 	static bool ConfigureHavocCastCommit(UAnimMontage* Montage, float Time);
  45: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Liberation")
  46: 	static bool ConfigureLiberationMontage(UAnimMontage* Montage, UAnimSequence* Source, UAnimMontage* WingMontage,
  47: 		UHodgeCinematicCameraProfile* CameraProfile, UHodgeHitDetectionProfile* HitProfile, float HitTime);
  49: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
  50: 	static bool ConfigureWingMontageGraph(UAnimBlueprint* Blueprint);
  52: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
  53: 	static bool ConfigureWingMontageAsset(UAnimMontage* Montage, UAnimSequence* Sequence);
  55: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Facing")
  56: 	static FString ConfigureAttackFacingWarp(UHodgeAbilityDefinition* Definition, float StartTime, float EndTime);
  65: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
  66: 	static FString ConfigureWingBoxHit(UAnimMontage* Montage, UHodgeWingProfile* Profile, FName AnchorBone, float Margin = 4.f);
  73: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
  74: 	static bool ConfigureWingActionWindow(UAnimSequenceBase* Source, float Begin, float End);
  80: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
  81: 	static bool ConfigureWingAttackPreview(UBlueprint* Blueprint, UHodgeWingProfile* Profile);
  87: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
  88: 	static bool ConfigureWingAnimation(UAnimBlueprint* Blueprint);
  94: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
  95: 	static bool ConfigureWingFinisher(UAnimMontage* Montage, UHodgeWingProfile* Profile);
 102: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
 103: 	static bool ConfigureWingExperience(UBlueprint* Experience, TSubclassOf<AActor> ActorClass, TSubclassOf<UActorComponent> ComponentClass);
 112: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
 113: 	static bool ConfigureEnemyBehaviorTree(UBehaviorTree* Tree, UBlackboardData* Blackboard, FGameplayTag AttackTag, bool bComboIntent = false);
 121: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
 122: 	static bool ConfigureEnemyAnimation(UAnimBlueprint* Blueprint, UAnimSequence* Idle, UAnimSequence* Run, FName SpineBone);
 128: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
 129: 	static bool SetEnemyMeshSkeleton(USkeletalMesh* Mesh, USkeleton* Skeleton);
 136: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
 137: 	static bool ConfigureEnemyAnimationAsset(UAnimationAsset* Asset, USkeleton* Skeleton, FName Slot = NAME_None);
 143: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
 144: 	static bool ConfigureEnemyNavigationBounds(ANavMeshBoundsVolume* Volume, FVector Size);
 149: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
 150: 	static bool ConfigureEnemyExperienceComponents(UBlueprint* Experience);
 159: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 160: 	static UAnimBlueprint* ImportAnimationBlueprintText(const FString& AssetPath, const FString& TextFilename, UClass* ParentClass = nullptr, USkeleton* TargetSkeleton = nullptr);
 169: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 170: 	static bool SetAnimationDefault(UBlueprint* Blueprint, const FString& PropertyName, const FString& Value);
 180: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 181: 	static bool SetAnimationNodeProperty(UBlueprint* Blueprint, const FString& NodePath, const FString& PropertyPath, const FString& Value);
 190: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 191: 	static bool RemapAnimationReferences(UBlueprint* Blueprint, const TArray<UObject*>& Sources, const TArray<UObject*>& Destinations);
 198: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 199: 	static FString CompileAnimationBlueprint(UBlueprint* Blueprint);
 207: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 208: 	static bool ConfigureAnimationLabGameMode(UBlueprint* Blueprint, TSubclassOf<APawn> PawnClass);
 216: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 217: 	static bool ConfigureAnimationLabPIE(int32 PlayerCount, bool bListenServer);
 223: 	UFUNCTION(BlueprintPure, Category = "Hodge|Editor|Animation")
 224: 	static TArray<FName> GetSkeletonBoneNames(USkeleton* Skeleton);
 231: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 232: 	static bool ConfigureHeroHitReactionGraph(UAnimBlueprint* Blueprint);
 239: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
 240: 	static FString ConfigureHeroFacingModes(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint);
 247: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
 248: 	static FString ConfigureHeroSprintAnimations(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint);
 254: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
 255: 	static FString ConfigureHeroSprintPivotTiming(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint);
 261: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
 262: 	static FString ConfigureHeroDashFootIK(UAnimBlueprint* LayerBlueprint);
 268: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
 269: 	static FString ConfigureHeroSprintSync(UAnimBlueprint* LayerBlueprint);
 274: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
 275: 	static bool ConfigureHeroMovementMontageSync(UAnimMontage* Montage);
 283: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 284: 	static bool SetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot, FName Group);
 291: 	UFUNCTION(BlueprintPure, Category = "Hodge|Editor|Animation")
 292: 	static FName GetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot);
 299: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 300: 	static bool SetHeroReactionMontageSlot(UAnimMontage* Montage, FName Slot);
 307: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 308: 	static bool SetHeroReactionSkeleton(UAnimationAsset* Asset, USkeleton* Skeleton);
 315: 	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
 316: 	static bool ConfigureHeroHitStunDuration(UAnimMontage* Montage, float Duration);
 317: };
```

## HodgeCombatValidationLibrary.cpp

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.cpp)

项目内直接 include（不是运行调用关系）：[Component/HodgeCharacterRotationComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeCharacterRotationComponent.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Component/HodgeCombatComponentBase.h](../../../Source/Hodgepodge/Public/Component/HodgeCombatComponentBase.h)、[Character/HodgeEnemyCharacter.h](../../../Source/Hodgepodge/Public/Character/HodgeEnemyCharacter.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[AbilitySystem/Effects/HodgeHavocEffects.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeHavocEffects.h)

定义候选（多行签名仅展示首行）：

- L42: `bool UHodgeCombatValidationLibrary::QueueDarkFlowDelta(AActor* Avatar, float Delta)`
- L56: `FVector2D UHodgeCombatValidationLibrary::InspectDarkFlow(AActor* Avatar)`
- L63: `bool UHodgeCombatValidationLibrary::QueueDarkSurgeDispel(AActor* Avatar)`
- L81: `bool UHodgeCombatValidationLibrary::QueueResonanceEnergyDelta(AActor* Avatar, float Delta)`
- L96: `FVector2D UHodgeCombatValidationLibrary::InspectResonanceEnergy(AActor* Avatar)`
- L103: `bool UHodgeCombatValidationLibrary::QueueCinematicSourceProbe(AActor* Avatar, float Position, float PlayRate)`
- L124: `bool UHodgeCombatValidationLibrary::QueueCombatInput(AActor* Avatar, FGameplayTag InputTag)`
- L137: `bool UHodgeCombatValidationLibrary::QueueGameplayEvent(UHodgeAbilitySystemComponent* ASC, FGameplayTag EventTag)`
- L153: `TArray<FName> UHodgeCombatValidationLibrary::InspectFacingSequences(AActor* Avatar)`
- L176: `TArray<FHodgePivotValidationState> UHodgeCombatValidationLibrary::InspectPivotTimes(AActor* Avatar)`
- L201: `TArray<FHodgeSyncValidationState> UHodgeCombatValidationLibrary::InspectLocomotionSync(AActor* Avatar)`
- L228: `bool UHodgeCombatValidationLibrary::SetFacingValidationControllerPermission(AActor* Avatar, bool bAllow)`
- L241: `bool UHodgeCombatValidationLibrary::QueueFacingMode(AActor* Avatar, EHodgeCharacterFacingDriver Driver, bool bRelease)`
- L257: `bool UHodgeCombatValidationLibrary::QueueActionFacing(AActor* Avatar, FVector Direction, bool bRelease, bool bInstant)`
- L275: `bool UHodgeCombatValidationLibrary::QueueAbilityAction(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool bCancel)`
- L347: `bool UHodgeCombatValidationLibrary::AddHitNotify(UAnimMontage* Montage, float Start, float End, const FHodgeAnimHitConfig& Config, bool bSingle)`
- L362: `bool UHodgeCombatValidationLibrary::AddStateNotify(UAnimMontage* Montage, float Start, float End, FGameplayTag Tag)`
- L372: `bool UHodgeCombatValidationLibrary::AddWeaponHandNotify(UAnimMontage* Montage, float Start, float End)`
- L379: `FGameplayTagContainer UHodgeCombatValidationLibrary::InspectAbilityWindows(UHodgeGameplayAbility_Definition* Ability)`
- L384: `int32 UHodgeCombatValidationLibrary::InspectHitSessions(AActor* Avatar)`
- L391: `int32 UHodgeCombatValidationLibrary::InspectPoseLeases(AActor* Avatar)`
- L399: `FHodgeMontageValidationState UHodgeCombatValidationLibrary::InspectMontageState(AActor* Avatar, UAnimMontage* Montage)`
- L425: `bool UHodgeCombatValidationLibrary::ConfigureValidationPIE(int32 Players, bool bDedicated)`
- L436: `bool UHodgeCombatValidationLibrary::ConfigureValidationSections(UAnimMontage* Montage)`
- L455: `bool UHodgeCombatValidationLibrary::NormalizeCombatNotifies(UAnimMontage* Montage)`
- L472: `bool UHodgeCombatValidationLibrary::ConfigureHitReactionValidationMontage(UAnimMontage* Montage, FName SlotName)`
- L484: `AActor* UHodgeCombatValidationLibrary::SpawnHitReactionValidationActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FVector Location)`
- L502: `FString UHodgeCombatValidationLibrary::InspectEnemyBrain(AActor* Avatar)`
- L517: `bool UHodgeCombatValidationLibrary::QueueCombatAction(AActor* Avatar, FGameplayTag ActionTag, bool bComboIntent)`
- L532: `bool UHodgeCombatValidationLibrary::QueueAttackFacingProbe(AActor* Avatar, FName Action)`

## HodgeCombatValidationLibrary.h

模块或基础类型入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeCombatValidationLibrary.h)

项目内直接 include（不是运行调用关系）：[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)、[Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "Kismet/BlueprintFunctionLibrary.h"
   6: #include "GameplayAbilitySpecHandle.h"
   7: #include "Animation/HodgeCombatAnimNotifies.h"
   8: #include "Combat/HodgeCharacterFacingTypes.h"
   9: #include "HodgeCombatValidationLibrary.generated.h"
  11: class UHodgeAbilitySystemComponent;
  12: class UHodgeGameplayAbility_Definition;
  13: class UAnimMontage;
  16: USTRUCT(BlueprintType)
  17: struct FHodgeMontageValidationState
  18: {
  19: 	GENERATED_BODY()
  21: 	UPROPERTY(BlueprintReadOnly) int32 InstanceId = INDEX_NONE;
  23: 	UPROPERTY(BlueprintReadOnly) float Position = 0.f;
  25: 	UPROPERTY(BlueprintReadOnly) float Weight = 0.f;
  27: 	UPROPERTY(BlueprintReadOnly) float DesiredWeight = 0.f;
  29: 	UPROPERTY(BlueprintReadOnly) float BlendTime = 0.f;
  31: 	UPROPERTY(BlueprintReadOnly) bool bPlaying = false;
  33: 	UPROPERTY(BlueprintReadOnly) bool bStopped = true;
  35: 	UPROPERTY(BlueprintReadOnly) FName LocomotionState;
  36: };
  39: USTRUCT(BlueprintType)
  40: struct FHodgePivotValidationState
  41: {
  42: 	GENERATED_BODY()
  44: 	UPROPERTY(BlueprintReadOnly) FName Sequence;
  46: 	UPROPERTY(BlueprintReadOnly) float Time = 0.f;
  48: 	UPROPERTY(BlueprintReadOnly) float Weight = 0.f;
  49: };
  52: USTRUCT(BlueprintType)
  53: struct FHodgeSyncValidationState
  54: {
  55: 	GENERATED_BODY()
  57: 	UPROPERTY(BlueprintReadOnly) FName Group;
  59: 	UPROPERTY(BlueprintReadOnly) FName Sequence;
  61: 	UPROPERTY(BlueprintReadOnly) float Time = 0.f;
  63: 	UPROPERTY(BlueprintReadOnly) float Weight = 0.f;
  65: 	UPROPERTY(BlueprintReadOnly) bool bLeader = false;
  67: 	UPROPERTY(BlueprintReadOnly) bool bMarkerSync = false;
  68: };
  71: UCLASS()
  72: class HODGEABILITYEDITOR_API UHodgeCombatValidationLibrary : public UBlueprintFunctionLibrary
  73: {
  74: 	GENERATED_BODY()
  75: public:
  77: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation") static bool QueueDarkFlowDelta(AActor* Avatar, float Delta);
  79: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation") static FVector2D InspectDarkFlow(AActor* Avatar);
  81: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation") static bool QueueDarkSurgeDispel(AActor* Avatar);
  83: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation") static bool QueueResonanceEnergyDelta(AActor* Avatar, float Delta);
  85: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation") static FVector2D InspectResonanceEnergy(AActor* Avatar);
  87: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation") static bool QueueCinematicSourceProbe(AActor* Avatar, float Position, float PlayRate = 1.f);
  89: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
  90: 	static bool QueueAttackFacingProbe(AActor* Avatar, FName Action);
  98: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
  99: 	static bool QueueAbilityAction(UHodgeAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool bCancel = false);
 105: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 106: 	static bool QueueGameplayEvent(UHodgeAbilitySystemComponent* ASC, FGameplayTag EventTag);
 112: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 113: 	static bool QueueCombatInput(AActor* Avatar, FGameplayTag InputTag);
 120: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 121: 	static bool QueueFacingMode(AActor* Avatar, EHodgeCharacterFacingDriver Driver, bool bRelease = false);
 129: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 130: 	static bool QueueActionFacing(AActor* Avatar, FVector Direction, bool bRelease = false, bool bInstant = false);
 135: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
 136: 	static TArray<FName> InspectFacingSequences(AActor* Avatar);
 141: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
 142: 	static TArray<FHodgePivotValidationState> InspectPivotTimes(AActor* Avatar);
 147: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
 148: 	static TArray<FHodgeSyncValidationState> InspectLocomotionSync(AActor* Avatar);
 154: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 155: 	static bool SetFacingValidationControllerPermission(AActor* Avatar, bool bAllow);
 164: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 165: 	static bool AddHitNotify(UAnimMontage* Montage, float Start, float End, const FHodgeAnimHitConfig& Config, bool bSingle = false);
 173: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 174: 	static bool AddStateNotify(UAnimMontage* Montage, float Start, float End, FGameplayTag Tag);
 181: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 182: 	static bool AddWeaponHandNotify(UAnimMontage* Montage, float Start, float End);
 187: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
 188: 	static FGameplayTagContainer InspectAbilityWindows(UHodgeGameplayAbility_Definition* Ability);
 193: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
 194: 	static int32 InspectHitSessions(AActor* Avatar);
 199: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
 200: 	static int32 InspectPoseLeases(AActor* Avatar);
 206: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
 207: 	static FHodgeMontageValidationState InspectMontageState(AActor* Avatar, UAnimMontage* Montage);
 213: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 214: 	static bool ConfigureValidationPIE(int32 Players, bool bDedicated = false);
 219: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 220: 	static bool ConfigureValidationSections(UAnimMontage* Montage);
 225: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 226: 	static bool NormalizeCombatNotifies(UAnimMontage* Montage);
 232: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 233: 	static bool ConfigureHitReactionValidationMontage(UAnimMontage* Montage, FName SlotName);
 240: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation", meta=(WorldContext="WorldContextObject"))
 241: 	static AActor* SpawnHitReactionValidationActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FVector Location);
 246: 	UFUNCTION(BlueprintPure, Category="Hodge|Editor|Validation")
 247: 	static FString InspectEnemyBrain(AActor* Avatar);
 254: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Validation")
 255: 	static bool QueueCombatAction(AActor* Avatar, FGameplayTag ActionTag, bool bComboIntent = false);
 257: };
```

## HodgeUIAuthoringLibrary.cpp

仅 Editor 的 Main UI 资产制作、原生 PIE 命令调度、Slate 截图与只读检查入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.cpp](../../../Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.cpp)

项目内直接 include（不是运行调用关系）：[UI/HodgeResonanceEnergyWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeResonanceEnergyWidget.h)、[UI/IndicatorSystem/IndicatorLayer.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorLayer.h)、[UI/Targeting/HodgeLockOnIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/Targeting/HodgeLockOnIndicatorWidget.h)、[UI/IndicatorSystem/HodgeIndicatorManagerComponent.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/HodgeIndicatorManagerComponent.h)、[UI/IndicatorSystem/IndicatorDescriptor.h](../../../Source/Hodgepodge/Public/UI/IndicatorSystem/IndicatorDescriptor.h)、[UI/Foundation/HodgePrimaryGameLayout.h](../../../Source/Hodgepodge/Public/UI/Foundation/HodgePrimaryGameLayout.h)、[UI/Subsystem/HodgeUIManagerSubsystem.h](../../../Source/Hodgepodge/Public/UI/Subsystem/HodgeUIManagerSubsystem.h)、[UI/Extension/UIExtensionPointWidget.h](../../../Source/Hodgepodge/Public/UI/Extension/UIExtensionPointWidget.h)、[Data/HodgeExperienceActionSet.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h)、[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)、[GameFeatures/GameFeatureAction_AddWidget.h](../../../Source/Hodgepodge/Public/GameFeatures/GameFeatureAction_AddWidget.h)、[Component/HodgeExperienceManagerComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeExperienceManagerComponent.h)、[Component/HodgeHealthComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHealthComponent.h)、[Component/HodgeHeroComponent.h](../../../Source/Hodgepodge/Public/Component/HodgeHeroComponent.h)、[Component/HodgePawnExtensionComponent.h](../../../Source/Hodgepodge/Public/Component/HodgePawnExtensionComponent.h)、[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Camera/HodgeCameraMode.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_LockOn.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[Core/PlayerController/HodgePlayerController.h](../../../Source/Hodgepodge/Public/Core/PlayerController/HodgePlayerController.h)、[UI/Data/HodgeGameplayUIDataSource.h](../../../Source/Hodgepodge/Public/UI/Data/HodgeGameplayUIDataSource.h)、[UI/HodgeHUDLayout.h](../../../Source/Hodgepodge/Public/UI/HodgeHUDLayout.h)、[UI/HodgeActivatableWidget.h](../../../Source/Hodgepodge/Public/UI/HodgeActivatableWidget.h)

定义候选（多行签名仅展示首行）：

- L166: `FString UHodgeUIAuthoringLibrary::CreateFoundationAssets()`
- L233: `FString UHodgeUIAuthoringLibrary::InspectPlayerUI(APlayerController* Player)`
- L375: `bool UHodgeUIAuthoringLibrary::QueueUIAction(APlayerController* Player, FName Action, float Value)`
- L1200: `FString UHodgeUIAuthoringLibrary::MigrateDesignerAssets()`
- L1296: `bool UHodgeUIAuthoringLibrary::ConfigureResonanceHUD(UWidgetBlueprint* Blueprint, TSubclassOf<UUserWidget> WidgetClass)`
- L1310: `bool UHodgeUIAuthoringLibrary::ConfigureAbilityIconHUD(UWidgetBlueprint* Blueprint,TSubclassOf<UUserWidget> SkillClass)`
- L1341: `bool UHodgeUIAuthoringLibrary::ConfigureAbilityIconDesigner(UWidgetBlueprint* Blueprint)`
- L1377: `FString UHodgeUIAuthoringLibrary::ConfigureReferenceHUD()`
- L1480: `FString UHodgeUIAuthoringLibrary::InspectDesignerAssets()`
- L1496: `FString UHodgeUIAuthoringLibrary::ConfigureDesignerPreviews()`
- L1547: `bool UHodgeUIAuthoringLibrary::ConfigureLockOnHUD(UWidgetBlueprint* Blueprint)`
- L1561: `bool UHodgeUIAuthoringLibrary::ConfigureLockOnIndicator(UWidgetBlueprint* Blueprint)`
- L1580: `FString UHodgeUIAuthoringLibrary::ConfigureDarkFlowHUD()`

## HodgeUIAuthoringLibrary.h

仅 Editor 的 Main UI 资产制作、原生 PIE 命令调度、Slate 截图与只读检查入口。

源码：[Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.h](../../../Source/HodgeAbilityEditor/Private/HodgeUIAuthoringLibrary.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   5: #include "Kismet/BlueprintFunctionLibrary.h"
   6: #include "HodgeUIAuthoringLibrary.generated.h"
   7: class APlayerController;
  10: UCLASS()
  11: class UHodgeUIAuthoringLibrary : public UBlueprintFunctionLibrary
  12: {
  13:     GENERATED_BODY()
  14: public:
  16: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString ConfigureDarkFlowHUD();
  18: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool ConfigureResonanceHUD(class UWidgetBlueprint* Blueprint, TSubclassOf<class UUserWidget> WidgetClass);
  20: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool ConfigureAbilityIconHUD(class UWidgetBlueprint* Blueprint, TSubclassOf<class UUserWidget> SkillClass);
  22: 	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool ConfigureAbilityIconDesigner(class UWidgetBlueprint* Blueprint);
  24:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool ConfigureLockOnHUD(class UWidgetBlueprint* Blueprint);
  28:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString CreateFoundationAssets();
  32:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString MigrateDesignerAssets();
  36:     UFUNCTION(BlueprintPure, Category="Hodge|Editor|UI") static FString InspectDesignerAssets();
  40:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString ConfigureDesignerPreviews();
  44:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString ConfigureReferenceHUD();
  49:     UFUNCTION(BlueprintPure, Category="Hodge|Editor|UI") static FString InspectPlayerUI(APlayerController* Player);
  56:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool QueueUIAction(APlayerController* Player, FName Action, float Value = 0);
  58:     UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool ConfigureLockOnIndicator(UWidgetBlueprint* Blueprint);
  59: };
```

## Hodgepodge.Build.cs

模块或基础类型入口。

源码：[Source/Hodgepodge/Hodgepodge.Build.cs](../../../Source/Hodgepodge/Hodgepodge.Build.cs)

定义候选（多行签名仅展示首行）：


## Hodgepodge.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Hodgepodge.cpp](../../../Source/Hodgepodge/Hodgepodge.cpp)

定义候选（多行签名仅展示首行）：


## Hodgepodge.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Hodgepodge.h](../../../Source/Hodgepodge/Hodgepodge.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  13: #pragma once
  15: #include "CoreMinimal.h"
```

## Hodgepodge.Target.cs

模块或基础类型入口。

源码：[Source/Hodgepodge.Target.cs](../../../Source/Hodgepodge.Target.cs)

定义候选（多行签名仅展示首行）：


## HodgepodgeEditor.Target.cs

模块或基础类型入口。

源码：[Source/HodgepodgeEditor.Target.cs](../../../Source/HodgepodgeEditor.Target.cs)

定义候选（多行签名仅展示首行）：
