/** @file HodgeAnimationAuthoringLibrary.h
 * @brief 声明动画/行为树编辑器制作函数库相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"

#include "HodgeAnimationAuthoringLibrary.generated.h"

class UAnimBlueprint;
class UBlueprint;
class USkeleton;
class UAnimMontage;
class UHodgeAbilityDefinition;
class UAnimationAsset;
class APawn;
class UBehaviorTree;
class UBlackboardData;
class UAnimSequence;
class UAnimSequenceBase;
class UHodgeWingProfile;
class USkeletalMesh;
class ANavMeshBoundsVolume;

/** 为 CodexText 中的动画副本提供编辑器文本导入、引用修正和编译诊断。 */
UCLASS()
class HODGEABILITYEDITOR_API UHodgeAnimationAuthoringLibrary : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

public:
	/** 为首版攻击添加原生 Warp 通知，不修改命中和恢复窗口。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Facing")
	static FString ConfigureAttackFacingWarp(UHodgeAbilityDefinition* Definition, float StartTime, float EndTime);

	/** 按翼攻击窗口的蒙皮顶点测量并配置跟随翼根的盒体，不改其他通知。
	 * @param Montage 正式主角攻击蒙太奇。
	 * @param Profile 已校准的动画翅膀配置。
	 * @param AnchorBone 实际翼骨架上的锚点骨骼。
	 * @param Margin 盒体各轴追加的厘米余量。
	 * @return 成功返回尺寸、窗口与采样数量 JSON，失败返回诊断且不写配置。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
	static FString ConfigureWingBoxHit(UAnimMontage* Montage, UHodgeWingProfile* Profile, FName AnchorBone, float Margin = 4.f);
	/** 为源动画补充翼显示窗口，保留其他通知。
	 * @param Source 角色源动画或蒙太奇。
	 * @param Begin 窗口起始源秒。
	 * @param End 窗口结束源秒。
	 * @return 配置合法并成功写入时返回 true。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
	static bool ConfigureWingActionWindow(UAnimSequenceBase* Source, float Begin, float End);
	/** 为用户预览配置现有翼网格的协调组件。
	 * @param Blueprint 已校准的 Character 蓝图。
	 * @param Profile 新动画模式的表现配置。
	 * @return 成功配置时返回 true。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
	static bool ConfigureWingAttackPreview(UBlueprint* Blueprint, UHodgeWingProfile* Profile);
	/** 创建独立翼骨骼参考姿势图，不修改主角已有动画图。 */
	/** 配置翅膀动画。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
	static bool ConfigureWingAnimation(UAnimBlueprint* Blueprint);
	/** 配置翅膀Finisher。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param Profile 当前操作使用的配置对象。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
	static bool ConfigureWingFinisher(UAnimMontage* Montage, UHodgeWingProfile* Profile);
	/** 配置翅膀Experience配置。
	 * @param Experience Experience配置，由调用方传入。
	 * @param ActorClass Actor对象类型，由调用方传入。
	 * @param ComponentClass 组件类型，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Wing")
	static bool ConfigureWingExperience(UBlueprint* Experience, TSubclassOf<AActor> ActorClass, TSubclassOf<UActorComponent> ComponentClass);
	/** 只配置新建的 Main/Character/Enemies 资源，不重写玩家或第三方图。 */
	/** 配置敌人行为树。
	 * @param Tree 当前行为树资产或执行组件。
	 * @param Blackboard 黑板，由调用方传入。
	 * @param AttackTag 攻击标签，用于对应标签查询或匹配。
	 * @param bComboIntent 是否连段意图的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
	static bool ConfigureEnemyBehaviorTree(UBehaviorTree* Tree, UBlackboardData* Blackboard, FGameplayTag AttackTag, bool bComboIntent = false);
	/** 配置敌人动画。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @param Idle Idle，由调用方传入。
	 * @param Run 奔跑，由调用方传入。
	 * @param SpineBone Spine骨骼，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
	static bool ConfigureEnemyAnimation(UAnimBlueprint* Blueprint, UAnimSequence* Idle, UAnimSequence* Run, FName SpineBone);
	/** 设置敌人网格骨架。
	 * @param Mesh 网格，由调用方传入。
	 * @param Skeleton 骨架，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
	static bool SetEnemyMeshSkeleton(USkeletalMesh* Mesh, USkeleton* Skeleton);
	/** 配置敌人动画资产。
	 * @param Asset 资产，由调用方传入。
	 * @param Skeleton 骨架，由调用方传入。
	 * @param Slot 槽位，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
	static bool ConfigureEnemyAnimationAsset(UAnimationAsset* Asset, USkeleton* Skeleton, FName Slot = NAME_None);
	/** 配置敌人导航边界。
	 * @param Volume Volume，由调用方传入。
	 * @param Size 大小，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
	static bool ConfigureEnemyNavigationBounds(ANavMeshBoundsVolume* Volume, FVector Size);
	/** 配置敌人Experience配置Components。
	 * @param Experience Experience配置，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Enemy")
	static bool ConfigureEnemyExperienceComponents(UBlueprint* Experience);
	/** 仅创建新的 CodexText 动画蓝图，不覆盖已有资产。 */
	/** 导入动画蓝图文本并恢复其中的对象与图节点。
	 * @param AssetPath 资产路径，由调用方传入。
	 * @param TextFilename 文字Filename，由调用方传入。
	 * @param ParentClass 父项类型，由调用方传入。
	 * @param TargetSkeleton 目标骨架，由调用方传入。
	 * @return 本次计算、查询或创建得到的动画Blueprint。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static UAnimBlueprint* ImportAnimationBlueprintText(const FString& AssetPath, const FString& TextFilename, UClass* ParentClass = nullptr, USkeleton* TargetSkeleton = nullptr);

	/** 通过 UE 的属性文本格式设置动画副本的默认值。 */
	/** 设置动画默认。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @param PropertyName 属性名称，由调用方传入。
	 * @param Value 值，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool SetAnimationDefault(UBlueprint* Blueprint, const FString& PropertyName, const FString& Value);

	/** 配置指定图节点的属性，支持点分隔的结构体属性路径。 */
	/** 设置动画节点属性。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @param NodePath 节点路径，由调用方传入。
	 * @param PropertyPath 属性路径，由调用方传入。
	 * @param Value 值，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool SetAnimationNodeProperty(UBlueprint* Blueprint, const FString& NodePath, const FString& PropertyPath, const FString& Value);

	/** 只遍历给定副本内部的对象，不修改引用指向的原始资产。 */
	/** 按指定映射更新动画图中的资源引用。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @param Sources Sources，由调用方传入。
	 * @param Destinations Destinations，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool RemapAnimationReferences(UBlueprint* Blueprint, const TArray<UObject*>& Sources, const TArray<UObject*>& Destinations);

	/** 返回本次编译错误数及完整消息，不保存资产。 */
	/** 编译当前动画蓝图并返回诊断结果。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @return 本次生成的文字或诊断信息。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static FString CompileAnimationBlueprint(UBlueprint* Blueprint);

	/** 为实验 GameMode 创建合法的 Pawn 类覆盖，保留 Hodge 原有初始化流程。 */
	/** 配置动画实验场景游戏Mode。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @param PawnClass Pawn角色类型，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool ConfigureAnimationLabGameMode(UBlueprint* Blueprint, TSubclassOf<APawn> PawnClass);

	/** 仅在实验关卡中设置临时 PIE 模式，不写入项目配置。 */
	/** 配置动画实验场景PIE。
	 * @param PlayerCount 玩家数量，由调用方传入。
	 * @param bListenServer 是否Listen服务器的控制标记。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool ConfigureAnimationLabPIE(int32 PlayerCount, bool bListenServer);

	/** 获取骨架骨骼Names。
	 * @param Skeleton 骨架，由调用方传入。
	 * @return 骨架骨骼Names的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Editor|Animation")
	static TArray<FName> GetSkeletonBoneNames(USkeleton* Skeleton);

	/** 在 Main 主角图中接入全身动作后的上身加法受击，不保存资产。 */
	/** 配置玩家角色命中受击图。
	 * @param Blueprint Blueprint，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool ConfigureHeroHitReactionGraph(UAnimBlueprint* Blueprint);
	/** 隔离方向资源入口，保留原八向分支并添加 Free 选择。 */
	/** 配置玩家角色朝向Modes。
	 * @param MainBlueprint MainBlueprint，由调用方传入。
	 * @param LayerBlueprint 层Blueprint，由调用方传入。
	 * @return 本次生成的文字或诊断信息。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static FString ConfigureHeroFacingModes(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint);
	/** 在既有层的资源入口接入 Sprint，保留八向图和常规动画。 */
	/** 配置玩家角色Sprint奔跑Animations。
	 * @param MainBlueprint MainBlueprint，由调用方传入。
	 * @param LayerBlueprint 层Blueprint，由调用方传入。
	 * @return 本次生成的文字或诊断信息。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static FString ConfigureHeroSprintAnimations(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint);
	/** 配置玩家角色Sprint奔跑掉头Timing。
	 * @param MainBlueprint MainBlueprint，由调用方传入。
	 * @param LayerBlueprint 层Blueprint，由调用方传入。
	 * @return 本次生成的文字或诊断信息。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static FString ConfigureHeroSprintPivotTiming(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint);
	/** 替换腿部 IK 的全身权重门控，Dash 继续使用脚部贴地。 */
	/** 配置玩家角色Dash冲刺脚部IK。
	 * @param LayerBlueprint 层Blueprint，由调用方传入。
	 * @return 本次生成的文字或诊断信息。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static FString ConfigureHeroDashFootIK(UAnimBlueprint* LayerBlueprint);
	/** Sprint 循环接入独立同步组，普通移动继续使用既有 Locomotion。 */
	/** 配置玩家角色Sprint奔跑同步。
	 * @param LayerBlueprint 层Blueprint，由调用方传入。
	 * @return 本次生成的文字或诊断信息。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static FString ConfigureHeroSprintSync(UAnimBlueprint* LayerBlueprint);
	/** 配置玩家角色移动蒙太奇同步。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|Animation")
	static bool ConfigureHeroMovementMontageSync(UAnimMontage* Montage);

	/** 设置骨架槽位分组。
	 * @param Skeleton 骨架，由调用方传入。
	 * @param Slot 槽位，由调用方传入。
	 * @param Group 分组，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool SetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot, FName Group);

	/** 获取骨架槽位分组。
	 * @param Skeleton 骨架，由调用方传入。
	 * @param Slot 槽位，由调用方传入。
	 * @return 骨架槽位分组的当前值或引用。
	 */
	UFUNCTION(BlueprintPure, Category = "Hodge|Editor|Animation")
	static FName GetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot);

	/** 设置玩家角色受击蒙太奇槽位。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param Slot 槽位，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool SetHeroReactionMontageSlot(UAnimMontage* Montage, FName Slot);

	/** 设置玩家角色受击骨架。
	 * @param Asset 资产，由调用方传入。
	 * @param Skeleton 骨架，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool SetHeroReactionSkeleton(UAnimationAsset* Asset, USkeleton* Skeleton);

	/** 配置玩家角色命中硬直持续时间。
	 * @param Montage 本次动作使用的蒙太奇。
	 * @param Duration 持续时间，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "Hodge|Editor|Animation")
	static bool ConfigureHeroHitStunDuration(UAnimMontage* Montage, float Duration);
};
