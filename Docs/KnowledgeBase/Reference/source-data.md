> 2026-10-05 文件组织更新：原 `_Montage.cpp`、`_Validation.cpp` 已合入对应主 `.cpp`；下文保留原快照片段标题，源码链接指向合并后的文件。历史 snapshot.json 的路径/hash 不重写。

# Data 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityDefinition.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

定义候选（多行签名仅展示首行）：

- L11: `FAlphaBlend FHodgeAbilityBlendSettings::MakeBlend() const`
- L19: `bool FHodgeAbilityBlendSettings::IsValid() const`
- L25: `float UHodgeAbilityDefinition::GetDuration() const`
- L29: `bool UHodgeAbilityDefinition::ValidateDefinition(TArray<FText>& Errors) const`
- L65: `EDataValidationResult UHodgeAbilityDefinition::IsDataValid(FDataValidationContext& Context) const`

## HodgeAbilitySet.cpp

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

源码：[Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)、[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)

定义候选（多行签名仅展示首行）：

- L11: `void FHodgeAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)`
- L20: `void FHodgeAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)`
- L29: `void FHodgeAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)`
- L35: `void FHodgeAbilitySet_GrantedHandles::TakeFromAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC)`
- L76: `UHodgeAbilitySet::UHodgeAbilitySet(const FObjectInitializer& ObjectInitializer)`
- L82: `void UHodgeAbilitySet::GiveToAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC,`

## HodgeAbilityTimeline.cpp

技能逻辑时间轴数据资产（统一事件模型：单一 Events[]，Kind = Window / Point）。只描述“何时发生什么”，不含业务判断；没有 Montage 字段、不做 Bundle 收集。

源码：[Source/Hodgepodge/Private/Data/HodgeAbilityTimeline.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilityTimeline.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)、[AbilitySystem/HodgeTimelineEvaluator.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeTimelineEvaluator.h)

定义候选（多行签名仅展示首行）：

- L27: `bool UHodgeAbilityTimeline::ValidateForPlayback(TArray<FText>& OutErrors, float DurationOverride) const`
- L38: `bool UHodgeAbilityTimeline::ValidateEntries(TArray<FText>& OutErrors, float EffectiveDuration, bool bCheckDuration) const`
- L240: `EDataValidationResult UHodgeAbilityTimeline::IsDataValid(FDataValidationContext& Context) const`
- L350: `void UHodgeAbilityTimeline::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)`

## HodgeAssetManager.cpp

资产入口、GameData 缓存、启动任务、同步加载、加载进度。Cue 初始化钩子仍需接通。（PreloadPrimaryAssetBundles 已随未提交改动回退，当前不存在。）

源码：[Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeAssetManagerStartupJob.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h)

定义候选（多行签名仅展示首行）：

- L10: `const FName FHodgeBundles::Equipped("Equipped");`
- L44: `UHodgeAssetManager::UHodgeAssetManager()`
- L50: `UHodgeAssetManager& UHodgeAssetManager::Get()`
- L69: `void UHodgeAssetManager::DumpLoadedAssets()`
- L84: `const UHodgeGameData& UHodgeAssetManager::GetGameData()`
- L90: `const UHodgePawnData* UHodgeAssetManager::GetDefaultPawnData() const`
- L96: `UObject* UHodgeAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)`
- L130: `bool UHodgeAssetManager::ShouldLogAssetLoads()`
- L141: `void UHodgeAssetManager::AddLoadedAsset(const UObject* Asset)`
- L154: `void UHodgeAssetManager::StartInitialLoading()`
- L175: `UPrimaryDataAsset* UHodgeAssetManager::LoadGameDataOfClass(`
- L277: `void UHodgeAssetManager::DoAllStartupJobs()`
- L366: `void UHodgeAssetManager::InitializeGameplayCueManager()`
- L377: `void UHodgeAssetManager::UpdateInitialGameContentLoadPercent(`
- L385: `void UHodgeAssetManager::PreBeginPIE(bool bStartSimulate)`

## HodgeAssetManagerStartupJob.cpp

封装启动任务与进度权重，供 AssetManager 执行启动工作。

源码：[Source/Hodgepodge/Private/Data/HodgeAssetManagerStartupJob.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAssetManagerStartupJob.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAssetManagerStartupJob.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h)

定义候选（多行签名仅展示首行）：

- L13: `TSharedPtr<FStreamableHandle> FHodgeAssetManagerStartupJob::DoJob() const`

## HodgeComboDefinition.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgeComboDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeComboDefinition.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)

定义候选（多行签名仅展示首行）：

- L7: `const FHodgeComboRow* UHodgeComboDefinition::FindNode(FGameplayTag Tag) const`
- L12: `bool UHodgeComboDefinition::ValidateDefinition(TArray<FText>& Errors) const`
- L49: `EDataValidationResult UHodgeComboDefinition::IsDataValid(FDataValidationContext& Context) const`

## HodgeExperienceActionSet.cpp

复用 GameFeature 插件和动作配置的数据资产。

源码：[Source/Hodgepodge/Private/Data/HodgeExperienceActionSet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceActionSet.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeExperienceActionSet.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h)

定义候选（多行签名仅展示首行）：

- L16: `UHodgeExperienceActionSet::UHodgeExperienceActionSet()`
- L23: `EDataValidationResult UHodgeExperienceActionSet::IsDataValid(FDataValidationContext& Context) const`
- L62: `void UHodgeExperienceActionSet::UpdateAssetBundleData()`

## HodgeExperienceDefinition.cpp

声明玩法所需插件、默认 PawnData、直接 Actions 和组合 ActionSets。

源码：[Source/Hodgepodge/Private/Data/HodgeExperienceDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceDefinition.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)

定义候选（多行签名仅展示首行）：

- L16: `UHodgeExperienceDefinition::UHodgeExperienceDefinition()`
- L23: `EDataValidationResult UHodgeExperienceDefinition::IsDataValid(FDataValidationContext& Context) const`
- L97: `void UHodgeExperienceDefinition::UpdateAssetBundleData()`

## HodgeExperienceManager.cpp

管理编辑器等场景的 GameFeature 使用/停用协调，不是挂载在 GameState 的组件。

源码：[Source/Hodgepodge/Private/Data/HodgeExperienceManager.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceManager.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeExperienceManager.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceManager.h)

定义候选（多行签名仅展示首行）：

- L12: `void UHodgeExperienceManager::OnPlayInEditorBegun()`
- L22: `void UHodgeExperienceManager::NotifyOfPluginActivation(const FString PluginURL)`
- L42: `bool UHodgeExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)`

## HodgeGameData.cpp

全局伤害、治疗、动态 Tag GE 的软类引用配置；需编辑器核对实际赋值。

源码：[Source/Hodgepodge/Private/Data/HodgeGameData.cpp](../../../Source/Hodgepodge/Private/Data/HodgeGameData.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)

定义候选（多行签名仅展示首行）：

- L8: `UHodgeGameData::UHodgeGameData()`
- L12: `const UHodgeGameData& UHodgeGameData::Get()`

## HodgePawnData.cpp

PawnClass、AbilitySets、TagRelationshipMapping、InputConfig、DefaultCameraMode 配置。

源码：[Source/Hodgepodge/Private/Data/HodgePawnData.cpp](../../../Source/Hodgepodge/Private/Data/HodgePawnData.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

定义候选（多行签名仅展示首行）：

- L8: `UHodgePawnData::UHodgePawnData(const FObjectInitializer& ObjectInitializer)`

## HodgePawnData_Validation.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgePawnData.cpp](../../../Source/Hodgepodge/Private/Data/HodgePawnData.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)、[Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)、[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L10: `EDataValidationResult UHodgePawnData::IsDataValid(FDataValidationContext& Context) const`

## HodgeAbilityDefinition.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Engine/DataAsset.h"
   5: #include "AlphaBlend.h"
   6: #include "Animation/AnimMontage.h"
   7: #include "GameplayTagContainer.h"
   8: #include "HodgeAbilityDefinition.generated.h"
  10: class UHodgeGameplayAbility;
  11: class UHodgeAbilityTimeline;
  12: class UAnimMontage;
  14: USTRUCT(BlueprintType)
  15: struct FHodgeAbilityBlendSettings
  16: {
  17: 	GENERATED_BODY()
  18: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float Time = 0.1f;
  19: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EMontageBlendMode Mode = EMontageBlendMode::Standard;
  20: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EAlphaBlendOption Curve = EAlphaBlendOption::Linear;
  21: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UCurveFloat> CustomCurve;
  22: 	FAlphaBlend MakeBlend() const;
  23: 	bool IsValid() const;
  24: };
  26: USTRUCT(BlueprintType)
  27: struct FHodgeTimelineTaskConfig
  28: {
  29: 	GENERATED_BODY()
  31: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UHodgeAbilityTimeline> Timeline;
  32: };
  34: USTRUCT(BlueprintType)
  35: struct FHodgeAbilityExecutionConfig
  36: {
  37: 	GENERATED_BODY()
  38: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
  39: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01")) float PlayRate = 1.f;
  40: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings BlendIn;
  41: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings NaturalBlendOut;
  42: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings StopBlendOut;
  43: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeTimelineTaskConfig TimelineTaskConfig;
  44: };
  47: UCLASS(BlueprintType, Const)
  48: class HODGEPODGE_API UHodgeAbilityDefinition : public UPrimaryDataAsset
  49: {
  50: 	GENERATED_BODY()
  51: public:
  52: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag AbilityTag;
  53: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UHodgeGameplayAbility> AbilityClass;
  54: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FHodgeAbilityExecutionConfig ExecutionConfig;
  55: 	UFUNCTION(BlueprintPure) float GetDuration() const;
  56: 	bool ValidateDefinition(TArray<FText>& Errors) const;
  57: #if WITH_EDITOR
  58: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  59: #endif
  60: };
```

## HodgeAbilitySet.h

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

源码：[Source/Hodgepodge/Public/Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "ActiveGameplayEffectHandle.h"
   6: #include "Engine/DataAsset.h"
   7: #include "AttributeSet.h"
   8: #include "GameplayTagContainer.h"
  10: #include "GameplayAbilitySpecHandle.h"
  11: #include "HodgeAbilitySet.generated.h"
  13: class UAttributeSet;
  14: class UGameplayEffect;
  15: class UHodgeAbilitySystemComponent;
  16: class UHodgeGameplayAbility;
  17: class UObject;
  18: class UHodgeAbilityDefinition;
  20: USTRUCT(BlueprintType)
  21: struct FHodgeAbilitySet_Definition
  22: {
  23:  GENERATED_BODY()
  24:  UPROPERTY(EditDefaultsOnly) TObjectPtr<UHodgeAbilityDefinition> Definition;
  25:  UPROPERTY(EditDefaultsOnly, meta=(ClampMin="1")) int32 AbilityLevel = 1;
  26: };
  34: USTRUCT(BlueprintType)
  35: struct FHodgeAbilitySet_GameplayAbility
  36: {
  37: 	GENERATED_BODY()
  39: public:
  41: 	UPROPERTY(EditDefaultsOnly)
  42: 	TSubclassOf<UHodgeGameplayAbility> Ability = nullptr;
  45: 	UPROPERTY(EditDefaultsOnly)
  46: 	int32 AbilityLevel = 1;
  49: 	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
  50: 	FGameplayTag InputTag;
  51: };
  59: USTRUCT(BlueprintType)
  60: struct FHodgeAbilitySet_GameplayEffect
  61: {
  62: 	GENERATED_BODY()
  64: public:
  66: 	UPROPERTY(EditDefaultsOnly)
  67: 	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;
  70: 	UPROPERTY(EditDefaultsOnly)
  71: 	float EffectLevel = 1.0f;
  72: };
  79: USTRUCT(BlueprintType)
  80: struct FHodgeAbilitySet_AttributeSet
  81: {
  82: 	GENERATED_BODY()
  84: public:
  86: 	UPROPERTY(EditDefaultsOnly)
  87: 	TSubclassOf<UAttributeSet> AttributeSet;
  88: };
  96: USTRUCT(BlueprintType)
  97: struct FHodgeAbilitySet_GrantedHandles
  98: {
  99: 	GENERATED_BODY()
 101: public:
 103: 	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
 106: 	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
 109: 	void AddAttributeSet(UAttributeSet* Set);
 112: 	void TakeFromAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC);
 114: protected:
 116: 	UPROPERTY()
 117: 	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
 120: 	UPROPERTY()
 121: 	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;
 124: 	UPROPERTY()
 125: 	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
 126: };
 137: UCLASS(BlueprintType, Const)
 138: class UHodgeAbilitySet : public UPrimaryDataAsset
 139: {
 140: 	GENERATED_BODY()
 142: public:
 143: 	UHodgeAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 144: 	const TArray<FHodgeAbilitySet_Definition>& GetGrantedDefinitions() const { return GrantedAbilityDefinitions; }
 147: 	void GiveToAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC, FHodgeAbilitySet_GrantedHandles* OutGrantedHandles,
 148: 	                         UObject* SourceObject = nullptr) const;
 150: protected:
 152: 	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
 153: 	TArray<FHodgeAbilitySet_GameplayAbility> GrantedGameplayAbilities;
 155: 	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities", meta=(TitleProperty="Definition"))
 156: 	TArray<FHodgeAbilitySet_Definition> GrantedAbilityDefinitions;
 159: 	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
 160: 	TArray<FHodgeAbilitySet_GameplayEffect> GrantedGameplayEffects;
 163: 	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
 164: 	TArray<FHodgeAbilitySet_AttributeSet> GrantedAttributes;
 165: };
```

## HodgeAbilityTimeline.h

技能逻辑时间轴数据资产（统一事件模型：单一 Events[]，Kind = Window / Point）。只描述“何时发生什么”，不含业务判断；没有 Montage 字段、不做 Bundle 收集。

源码：[Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityTimeline.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  18: #pragma once
  20: #include "CoreMinimal.h"
  21: #include "Engine/DataAsset.h"
  22: #include "GameplayTagContainer.h"
  23: #include "Templates/SubclassOf.h"
  25: #include "HodgeAbilityTimeline.generated.h"
  27: class UGameplayEffect;
  35: UENUM(BlueprintType)
  36: enum class EHodgeTimelineEventNetPolicy : uint8
  37: {
  39: 	LocalAndAuthority,
  42: 	AuthorityOnly,
  46: 	LocallyControlledOnly
  47: };
  54: UENUM(BlueprintType)
  55: enum class EHodgeTimelineEventKind : uint8
  56: {
  58: 	Window,
  61: 	Point
  62: };
  75: USTRUCT(BlueprintType)
  76: struct FHodgeTimelineEvent
  77: {
  78: 	GENERATED_BODY()
  81: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  82: 	EHodgeTimelineEventKind Kind = EHodgeTimelineEventKind::Window;
  88: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  89: 	FName EventID;
  92: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.0, UIMin=0.0, Units="s"))
  93: 	float StartTime = 0.f;
  96: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
  97: 		meta=(ClampMin=0.0, UIMin=0.0, Units="s",
  98: 			  EditCondition="Kind==EHodgeTimelineEventKind::Window", EditConditionHides))
  99: 	float EndTime = 0.f;
 103: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
 104: 	int32 Priority = 0;
 110: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
 111: 		meta=(Categories="Status",
 112: 			  EditCondition="Kind==EHodgeTimelineEventKind::Window", EditConditionHides))
 113: 	FGameplayTag WindowTag;
 117: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
 118: 		meta=(EditCondition="Kind==EHodgeTimelineEventKind::Window", EditConditionHides))
 119: 	TSubclassOf<UGameplayEffect> WindowEffectClass;
 124: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
 125: 		meta=(Categories="GameplayEvent",
 126: 			  EditCondition="Kind==EHodgeTimelineEventKind::Point", EditConditionHides))
 127: 	FGameplayTag PointEventTag;
 134: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
 135: 		meta=(EditCondition="Kind==EHodgeTimelineEventKind::Point", EditConditionHides))
 136: 	TSubclassOf<UGameplayEffect> PointEffectClass;
 139: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
 140: 		meta=(EditCondition="Kind==EHodgeTimelineEventKind::Point", EditConditionHides))
 141: 	EHodgeTimelineEventNetPolicy NetPolicy = EHodgeTimelineEventNetPolicy::LocalAndAuthority;
 142: };
 152: UCLASS(BlueprintType, Const)
 153: class HODGEPODGE_API UHodgeAbilityTimeline : public UPrimaryDataAsset
 154: {
 155: 	GENERATED_BODY()
 157: public:
 159: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
 160: 	bool bUseMontageDuration = false;
 163: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.01, Units="s", EditCondition="!bUseMontageDuration", EditConditionHides))
 164: 	float Duration = 1.0f;
 168: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty=EventID))
 169: 	TArray<FHodgeTimelineEvent> Events;
 172: 	bool ValidateForPlayback(TArray<FText>& OutErrors, float DurationOverride = -1.f) const;
 174: #if WITH_EDITOR
 177: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
 180: 	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
 182: #endif
 184: private:
 185: 	bool ValidateEntries(TArray<FText>& OutErrors, float EffectiveDuration, bool bCheckDuration) const;
 186: };
```

## HodgeAssetManager.h

资产入口、GameData 缓存、启动任务、同步加载、加载进度。Cue 初始化钩子仍需接通。（PreloadPrimaryAssetBundles 已随未提交改动回退，当前不存在。）

源码：[Source/Hodgepodge/Public/Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Engine/AssetManager.h"
   7: #include "HodgeAssetManagerStartupJob.h"
   8: #include "Templates/SubclassOf.h"
   9: #include "HodgeGameData.h"
  10: #include "HodgePawnData.h"
  11: #include "HodgeAssetManager.generated.h"
  15: struct FHodgeBundles
  16: {
  17:     static const FName Equipped;
  18: };
  27: UCLASS(Config = Game)
  28: class HODGEPODGE_API UHodgeAssetManager : public UAssetManager
  29: {
  30:     GENERATED_BODY()
  32: public:
  33:     UHodgeAssetManager();
  36:     static UHodgeAssetManager& Get();
  39:     template <typename AssetType>
  40:     static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);
  43:     template <typename AssetType>
  44:     static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);
  47:     static void DumpLoadedAssets();
  50:     const UHodgeGameData& GetGameData();
  53:     const UHodgePawnData* GetDefaultPawnData() const;
  55: protected:
  58:     template <typename GameDataClass>
  59:     const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
  60:     {
  62:        if (TObjectPtr<UPrimaryDataAsset> const* pResult = GameDataMap.Find(GameDataClass::StaticClass()))
  63:        {
  64:           return *CastChecked<GameDataClass>(*pResult);
  65:        }
  68:        return *CastChecked<const GameDataClass>(
  69:           LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
  70:     }
  74:     static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
  77:     static bool ShouldLogAssetLoads();
  80:     void AddLoadedAsset(const UObject* Asset);
  84:     virtual void StartInitialLoading() override;
  86: #if WITH_EDITOR
  88:     virtual void PreBeginPIE(bool bStartSimulate) override;
  89: #endif
  93:     UPrimaryDataAsset* LoadGameDataOfClass(
  94:         TSubclassOf<UPrimaryDataAsset> DataClass,
  95:         const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath,
  96:         FPrimaryAssetType PrimaryAssetType
  97:     );
  99: protected:
 102:     UPROPERTY(Config)
 103:     TSoftObjectPtr<UHodgeGameData> HodgeGameDataPath;
 106:     UPROPERTY(Transient)
 107:     TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;
 110:     UPROPERTY(Config)
 111:     TSoftObjectPtr<UHodgePawnData> DefaultPawnData;
 113: private:
 116:     void DoAllStartupJobs();
 119:     void InitializeGameplayCueManager();
 122:     void UpdateInitialGameContentLoadPercent(float GameContentPercent);
 125:     TArray<FHodgeAssetManagerStartupJob> StartupJobs;
 127: private:
 130:     UPROPERTY()
 131:     TSet<TObjectPtr<const UObject>> LoadedAssets;
 134:     FCriticalSection LoadedAssetsCritical;
 135: };
 138: template <typename AssetType>
 139: AssetType* UHodgeAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
 140: {
 141:     AssetType* LoadedAsset = nullptr;
 144:     const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
 146:     if (AssetPath.IsValid())
 147:     {
 149:        LoadedAsset = AssetPointer.Get();
 152:        if (!LoadedAsset)
 153:        {
 154:           LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
 155:           ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
 156:        }
 159:        if (LoadedAsset && bKeepInMemory)
 160:        {
 161:           Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
 162:        }
 163:     }
 165:     return LoadedAsset;
 166: }
 169: template <typename AssetType>
 170: TSubclassOf<AssetType> UHodgeAssetManager::GetSubclass(
 171:     const TSoftClassPtr<AssetType>& AssetPointer,
 172:     bool bKeepInMemory)
 173: {
 174:     TSubclassOf<AssetType> LoadedSubclass;
 177:     const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
 179:     if (AssetPath.IsValid())
 180:     {
 182:        LoadedSubclass = AssetPointer.Get();
 185:        if (!LoadedSubclass)
 186:        {
 187:           LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
 188:           ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
 189:        }
 192:        if (LoadedSubclass && bKeepInMemory)
 193:        {
 194:           Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
 195:        }
 196:     }
 198:     return LoadedSubclass;
 199: }
```

## HodgeAssetManagerStartupJob.h

封装启动任务与进度权重，供 AssetManager 执行启动工作。

源码：[Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "Engine/StreamableManager.h"
  13: DECLARE_DELEGATE_OneParam(
  14: 	FHodgeAssetManagerStartupJobSubstepProgress,
  15: 	float
  16: );
  32: struct FHodgeAssetManagerStartupJob
  33: {
  43: 	FHodgeAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;
  62: 	TFunction<
  63: 		void(
  64: 			const FHodgeAssetManagerStartupJob&,
  65: 			TSharedPtr<FStreamableHandle>&
  66: 		)
  67: 	> JobFunc;
  77: 	FString JobName;
  89: 	float JobWeight;
  98: 	mutable double LastUpdate = 0;
 110: 	FHodgeAssetManagerStartupJob(
 111: 		const FString& InJobName,
 112: 		const TFunction<
 113: 			void(
 114: 				const FHodgeAssetManagerStartupJob&,
 115: 				TSharedPtr<FStreamableHandle>&
 116: 			)
 117: 		>& InJobFunc,
 118: 		float InJobWeight
 119: 	)
 120: 		: JobFunc(InJobFunc)
 121: 		  , JobName(InJobName)
 122: 		  , JobWeight(InJobWeight)
 123: 	{
 124: 	}
 135: 	TSharedPtr<FStreamableHandle> DoJob() const;
 146: 	void UpdateSubstepProgress(float NewProgress) const
 147: 	{
 148: 		SubstepProgressDelegate.ExecuteIfBound(NewProgress);
 149: 	}
 162: 	void UpdateSubstepProgressFromStreamable(
 163: 		TSharedRef<FStreamableHandle> StreamableHandle
 164: 	) const
 165: 	{
 168: 		if (SubstepProgressDelegate.IsBound())
 169: 		{
 181: 			double Now = FPlatformTime::Seconds();
 192: 			if (Now - LastUpdate > 1.0 / 60)
 193: 			{
 202: 				SubstepProgressDelegate.Execute(
 203: 					StreamableHandle->GetProgress()
 204: 				);
 209: 				LastUpdate = Now;
 210: 			}
 211: 		}
 212: 	}
 213: };
```

## HodgeComboDefinition.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   1: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Engine/DataAsset.h"
   5: #include "Engine/DataTable.h"
   6: #include "GameplayTagContainer.h"
   7: #include "HodgeComboDefinition.generated.h"
   9: USTRUCT(BlueprintType)
  10: struct FHodgeComboTransition
  11: {
  12: 	GENERATED_BODY()
  13: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TriggerInputIntentTag;
  14: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TriggerEventTag;
  15: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TargetComboTag;
  16: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer RequiredWindowTags;
  17: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer RequiredSourceTags;
  18: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer BlockedSourceTags;
  19: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 TransitionPriority = 0;
  20: };
  22: USTRUCT(BlueprintType)
  23: struct FHodgeComboRow : public FTableRowBase
  24: {
  25: 	GENERATED_BODY()
  26: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag ComboTag;
  27: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag AbilityTag;
  28: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer GrantedTags;
  29: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FHodgeComboTransition> Transitions;
  30: };
  32: USTRUCT(BlueprintType)
  33: struct FHodgeComboInputBinding
  34: {
  35: 	GENERATED_BODY()
  36: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag InputTag;
  37: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag IntentTag;
  38: };
  40: UCLASS(BlueprintType, Const)
  41: class HODGEPODGE_API UHodgeComboDefinition : public UPrimaryDataAsset
  42: {
  43: 	GENERATED_BODY()
  44: public:
  45: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UDataTable> ComboTable;
  46: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag EntryComboTag;
  47: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FHodgeComboInputBinding> InputBindings;
  48: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float InputBufferSeconds = 0.3f;
  49: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag MoveCancelWindowTag;
  50: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float MoveIntentThreshold = 0.1f;
  51: 	const FHodgeComboRow* FindNode(FGameplayTag Tag) const;
  52: 	bool ValidateDefinition(TArray<FText>& Errors) const;
  53: #if WITH_EDITOR
  54: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  55: #endif
  56: };
```

## HodgeExperienceActionSet.h

复用 GameFeature 插件和动作配置的数据资产。

源码：[Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Engine/DataAsset.h"
   7: #include "HodgeExperienceActionSet.generated.h"
   9: class UGameFeatureAction;
  16: UCLASS(BlueprintType, NotBlueprintable)
  17: class HODGEPODGE_API UHodgeExperienceActionSet : public UPrimaryDataAsset
  18: {
  19: 	GENERATED_BODY()
  21: public:
  22: 	UHodgeExperienceActionSet();
  25: #if WITH_EDITOR
  27: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  28: #endif
  32: #if WITH_EDITORONLY_DATA
  34: 	virtual void UpdateAssetBundleData() override;
  35: #endif
  38: public:
  40: 	UPROPERTY(EditAnywhere, Instanced, Category="Actions to Perform")
  41: 	TArray<TObjectPtr<UGameFeatureAction>> Actions;
  45: 	UPROPERTY(EditAnywhere, Category="Feature Dependencies")
  46: 	TArray<FString> GameFeaturesToEnable;
  47: };
```

## HodgeExperienceDefinition.h

声明玩法所需插件、默认 PawnData、直接 Actions 和组合 ActionSets。

源码：[Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Engine/DataAsset.h"
   7: #include "HodgeExperienceDefinition.generated.h"
   9: class UGameFeatureAction;
  10: class UHodgePawnData;
  11: class UHodgeExperienceActionSet;
  19: UCLASS(BlueprintType, Const)
  20: class HODGEPODGE_API UHodgeExperienceDefinition : public UPrimaryDataAsset
  21: {
  22: 	GENERATED_BODY()
  24: public:
  25: 	UHodgeExperienceDefinition();
  28: #if WITH_EDITOR
  30: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  31: #endif
  35: #if WITH_EDITORONLY_DATA
  37: 	virtual void UpdateAssetBundleData() override;
  38: #endif
  41: public:
  43: 	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
  44: 	TArray<FString> GameFeaturesToEnable;
  48: 	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
  49: 	TObjectPtr<const UHodgePawnData> DefaultPawnData;
  52: 	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
  53: 	TArray<TObjectPtr<UGameFeatureAction>> Actions;
  57: 	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
  58: 	TArray<TObjectPtr<UHodgeExperienceActionSet>> ActionSets;
  59: };
```

## HodgeExperienceManager.h

管理编辑器等场景的 GameFeature 使用/停用协调，不是挂载在 GameState 的组件。

源码：[Source/Hodgepodge/Public/Data/HodgeExperienceManager.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceManager.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Subsystems/EngineSubsystem.h"
   7: #include "HodgeExperienceManager.generated.h"
  13: UCLASS()
  14: class HODGEPODGE_API UHodgeExperienceManager : public UEngineSubsystem
  15: {
  16: 	GENERATED_BODY()
  18: public:
  19: #if WITH_EDITOR
  22: 	void OnPlayInEditorBegun();
  25: 	static void NotifyOfPluginActivation(const FString PluginURL);
  28: 	static bool RequestToDeactivatePlugin(const FString PluginURL);
  30: #else
  33: 	static void NotifyOfPluginActivation(const FString PluginURL)
  34: 	{
  35: 	}
  38: 	static bool RequestToDeactivatePlugin(const FString PluginURL) { return true; }
  40: #endif
  42: private:
  44: 	TMap<FString, int32> GameFeaturePluginRequestCountMap;
  45: };
```

## HodgeGameData.h

全局伤害、治疗、动态 Tag GE 的软类引用配置；需编辑器核对实际赋值。

源码：[Source/Hodgepodge/Public/Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   5: #include "CoreMinimal.h"
   6: #include "Engine/DataAsset.h"
   7: #include "HodgeGameData.generated.h"
   9: class UGameplayEffect;
  13: UCLASS(BlueprintType, Const, Meta = (DisplayName = "Hodge Game Data", ShortTooltip = "包含全局游戏数据的数据资产"))
  14: class HODGEPODGE_API UHodgeGameData : public UPrimaryDataAsset
  15: {
  16: public:
  17: 	UHodgeGameData();
  20: 	static const UHodgeGameData& Get();
  22: 	GENERATED_BODY()
  24: 	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects",
  25: 		meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
  26: 	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;
  29: 	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects",
  30: 		meta = (DisplayName = "Heal Gameplay Effect (SetByCaller)"))
  31: 	TSoftClassPtr<UGameplayEffect> HealGameplayEffect_SetByCaller;
  34: 	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects")
  35: 	TSoftClassPtr<UGameplayEffect> DynamicTagGameplayEffect;
  36: };
```

## HodgePawnData.h

PawnClass、AbilitySets、TagRelationshipMapping、InputConfig、DefaultCameraMode 配置。

源码：[Source/Hodgepodge/Public/Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   3: #pragma once
   6: #include "CoreMinimal.h"
   9: #include "Engine/DataAsset.h"
  11: #include "HodgePawnData.generated.h"
  14: class UHodgeAbilitySet;
  15: class UHodgeComboDefinition;
  18: class UHodgeCameraMode;
  21: class UHodgeInputConfig;
  24: class UHodgeAbilityTagRelationshipMapping;
  32: UCLASS()
  33: class HODGEPODGE_API UHodgePawnData : public UPrimaryDataAsset
  34: {
  35: 	GENERATED_BODY()
  37: public:
  39: 	UHodgePawnData(const FObjectInitializer& ObjectInitializer);
  40: #if WITH_EDITOR
  41: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  42: #endif
  44: public:
  47: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Pawn")
  48: 	TSubclassOf<APawn> PawnClass;
  52: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Abilities")
  53: 	TArray<TObjectPtr<UHodgeAbilitySet>> AbilitySets;
  55: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Abilities")
  56: 	TObjectPtr<UHodgeComboDefinition> ComboDefinition;
  60: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Abilities")
  61: 	TObjectPtr<UHodgeAbilityTagRelationshipMapping> TagRelationshipMapping;
  65: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Input")
  66: 	TObjectPtr<UHodgeInputConfig> InputConfig;
  70: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Camera")
  71: 	TSubclassOf<UHodgeCameraMode> DefaultCameraMode;
  72: };
```
