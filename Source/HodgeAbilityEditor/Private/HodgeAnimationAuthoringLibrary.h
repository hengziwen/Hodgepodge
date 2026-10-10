#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "HodgeAnimationAuthoringLibrary.generated.h"

class UAnimBlueprint;
class UBlueprint;
class USkeleton;
class UAnimMontage;
class UAnimationAsset;
class APawn;

/** 为 CodexText 中的动画副本提供编辑器文本导入、引用修正和编译诊断。 */
UCLASS()
class HODGEABILITYEDITOR_API UHodgeAnimationAuthoringLibrary : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

public:
	/** 仅创建新的 CodexText 动画蓝图，不覆盖已有资产。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static UAnimBlueprint* ImportAnimationBlueprintText(const FString& AssetPath, const FString& TextFilename, UClass* ParentClass = nullptr, USkeleton* TargetSkeleton = nullptr);

	/** 通过 UE 的属性文本格式设置动画副本的默认值。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool SetAnimationDefault(UBlueprint* Blueprint, const FString& PropertyName, const FString& Value);

	/** 配置指定图节点的属性，支持点分隔的结构体属性路径。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool SetAnimationNodeProperty(UBlueprint* Blueprint, const FString& NodePath, const FString& PropertyPath, const FString& Value);

	/** 只遍历给定副本内部的对象，不修改引用指向的原始资产。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool RemapAnimationReferences(UBlueprint* Blueprint, const TArray<UObject*>& Sources, const TArray<UObject*>& Destinations);

	/** 返回本次编译错误数及完整消息，不保存资产。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static FString CompileAnimationBlueprint(UBlueprint* Blueprint);

	/** 为实验 GameMode 创建合法的 Pawn 类覆盖，保留 Hodge 原有初始化流程。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool ConfigureAnimationLabGameMode(UBlueprint* Blueprint, TSubclassOf<APawn> PawnClass);

	/** 仅在实验关卡中设置临时 PIE 模式，不写入项目配置。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool ConfigureAnimationLabPIE(int32 PlayerCount, bool bListenServer);

	UFUNCTION(BlueprintPure, Category = "Hodge|Editor|Animation")
	static TArray<FName> GetSkeletonBoneNames(USkeleton* Skeleton);

	/** 在 Main 主角图中接入全身动作后的上身加法受击，不保存资产。 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool ConfigureHeroHitReactionGraph(UAnimBlueprint* Blueprint);
	/** 隔离方向资源入口，保留原八向分支并添加 Free 选择。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static FString ConfigureHeroFacingModes(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint);
	/** 在既有层的资源入口接入 Sprint，保留八向图和常规动画。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static FString ConfigureHeroSprintAnimations(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static FString ConfigureHeroSprintPivotTiming(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint);
	/** 替换腿部 IK 的全身权重门控，Dash 继续使用脚部贴地。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static FString ConfigureHeroDashFootIK(UAnimBlueprint* LayerBlueprint);
	/** Sprint 循环接入独立同步组，普通移动继续使用既有 Locomotion。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static FString ConfigureHeroSprintSync(UAnimBlueprint* LayerBlueprint);
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static bool ConfigureHeroMovementMontageSync(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool SetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot, FName Group);

	UFUNCTION(BlueprintPure, Category = "Hodge|Editor|Animation")
	static FName GetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot);

	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool SetHeroReactionMontageSlot(UAnimMontage* Montage, FName Slot);

	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool SetHeroReactionSkeleton(UAnimationAsset* Asset, USkeleton* Skeleton);

	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool ConfigureHeroHitStunDuration(UAnimMontage* Montage, float Duration);
};
