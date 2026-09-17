# Data 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

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
  25: USTRUCT(BlueprintType)
  26: struct FHodgeAbilitySet_GameplayAbility
  27: {
  28: 	GENERATED_BODY()
  30: public:
  32: 	UPROPERTY(EditDefaultsOnly)
  33: 	TSubclassOf<UHodgeGameplayAbility> Ability = nullptr;
  36: 	UPROPERTY(EditDefaultsOnly)
  37: 	int32 AbilityLevel = 1;
  40: 	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
  41: 	FGameplayTag InputTag;
  42: };
  50: USTRUCT(BlueprintType)
  51: struct FHodgeAbilitySet_GameplayEffect
  52: {
  53: 	GENERATED_BODY()
  55: public:
  57: 	UPROPERTY(EditDefaultsOnly)
  58: 	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;
  61: 	UPROPERTY(EditDefaultsOnly)
  62: 	float EffectLevel = 1.0f;
  63: };
  70: USTRUCT(BlueprintType)
  71: struct FHodgeAbilitySet_AttributeSet
  72: {
  73: 	GENERATED_BODY()
  75: public:
  77: 	UPROPERTY(EditDefaultsOnly)
  78: 	TSubclassOf<UAttributeSet> AttributeSet;
  79: };
  87: USTRUCT(BlueprintType)
  88: struct FHodgeAbilitySet_GrantedHandles
  89: {
  90: 	GENERATED_BODY()
  92: public:
  94: 	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
  97: 	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
 100: 	void AddAttributeSet(UAttributeSet* Set);
 103: 	void TakeFromAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC);
 105: protected:
 107: 	UPROPERTY()
 108: 	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
 111: 	UPROPERTY()
 112: 	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;
 115: 	UPROPERTY()
 116: 	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
 117: };
 128: UCLASS(BlueprintType, Const)
 129: class UHodgeAbilitySet : public UPrimaryDataAsset
 130: {
 131: 	GENERATED_BODY()
 133: public:
 134: 	UHodgeAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 137: 	void GiveToAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC, FHodgeAbilitySet_GrantedHandles* OutGrantedHandles,
 138: 	                         UObject* SourceObject = nullptr) const;
 140: protected:
 142: 	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
 143: 	TArray<FHodgeAbilitySet_GameplayAbility> GrantedGameplayAbilities;
 146: 	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
 147: 	TArray<FHodgeAbilitySet_GameplayEffect> GrantedGameplayEffects;
 150: 	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
 151: 	TArray<FHodgeAbilitySet_AttributeSet> GrantedAttributes;
 152: };
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
  17: class UHodgeCameraMode;
  20: class UHodgeInputConfig;
  23: class UHodgeAbilityTagRelationshipMapping;
  31: UCLASS()
  32: class HODGEPODGE_API UHodgePawnData : public UPrimaryDataAsset
  33: {
  34: 	GENERATED_BODY()
  36: public:
  38: 	UHodgePawnData(const FObjectInitializer& ObjectInitializer);
  40: public:
  43: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Pawn")
  44: 	TSubclassOf<APawn> PawnClass;
  48: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Abilities")
  49: 	TArray<TObjectPtr<UHodgeAbilitySet>> AbilitySets;
  53: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Abilities")
  54: 	TObjectPtr<UHodgeAbilityTagRelationshipMapping> TagRelationshipMapping;
  58: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Input")
  59: 	TObjectPtr<UHodgeInputConfig> InputConfig;
  63: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Camera")
  64: 	TSubclassOf<UHodgeCameraMode> DefaultCameraMode;
  65: };
```
