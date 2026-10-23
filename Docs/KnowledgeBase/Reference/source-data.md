# Data 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAbilityDefinition.cpp

Montage/Blend/Timeline、HitWindows/HitPoints、Volume、独立/连段路由及输入；校验几何、权威消息与手持覆盖。

源码：[Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilityDefinition.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[Animation/HodgeRootMotionModifier_AttackFacing.h](../../../Source/Hodgepodge/Public/Animation/HodgeRootMotionModifier_AttackFacing.h)、[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)、[Animation/HodgeAnimNotifyState_CinematicCamera.h](../../../Source/Hodgepodge/Public/Animation/HodgeAnimNotifyState_CinematicCamera.h)、[Data/HodgeCinematicCameraProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h)、[Combat/HodgeAttackRule.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackRule.h)、[Combat/HodgeLegacyAttackRuleAdapter.h](../../../Source/Hodgepodge/Public/Combat/HodgeLegacyAttackRuleAdapter.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L24: `UHodgeAbilityDefinition::UHodgeAbilityDefinition()`
- L31: `FAlphaBlend FHodgeAbilityBlendSettings::MakeBlend() const`
- L41: `bool FHodgeAbilityBlendSettings::IsValid() const`
- L49: `float UHodgeAbilityDefinition::GetDuration() const`
- L55: `bool UHodgeAbilityDefinition::GetFacingWarpWindow(float& OutStart, float& OutEnd) const`
- L66: `bool UHodgeAbilityDefinition::ValidateDefinition(TArray<FText>& Errors) const`
- L163: `EDataValidationResult UHodgeAbilityDefinition::IsDataValid(FDataValidationContext& Context) const`
- L196: `bool UHodgeAbilityDefinition::CreateAttackRules(UHodgeGameplayAbility_Definition* Source, TArray<TObjectPtr<UHodgeAttackRule>>& OutRules) const`

## HodgeAbilitySet.cpp

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

源码：[Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)、[AbilitySystem/Abilities/HodgeGameplayAbility.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility.h)、[AbilitySystem/HodgeAbilitySystemComponent.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeAbilitySystemComponent.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

定义候选（多行签名仅展示首行）：

- L19: `void FHodgeAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)`
- L29: `void FHodgeAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)`
- L39: `void FHodgeAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)`
- L45: `void FHodgeAbilitySet_GrantedHandles::TakeFromAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC)`
- L90: `UHodgeAbilitySet::UHodgeAbilitySet(const FObjectInitializer& ObjectInitializer)`
- L122: `FHodgeAbilityGrantResult UHodgeAbilitySet::GrantInternal(UHodgeAbilitySystemComponent* ASC,`
- L177: `void FHodgeAbilitySet_GrantedHandles::Append(FHodgeAbilitySet_GrantedHandles&& Other)`
- L186: `bool FHodgeAbilitySet_GrantedHandles::AreValidFor(const UHodgeAbilitySystemComponent* ASC) const`
- L197: `void UHodgeAbilitySet::GiveToAbilitySystem(UHodgeAbilitySystemComponent* ASC,`
- L208: `FHodgeAbilityGrantResult UHodgeAbilitySet::TryGiveToAbilitySystem(UHodgeAbilitySystemComponent* ASC,`
- L216: `FHodgeAbilityGrantResult UHodgeAbilitySet::TryGiveBatchToAbilitySystem(const TArray<const UHodgeAbilitySet*>& Sets,`
- L250: `bool UHodgeAbilitySet::AreAttributeSetTypesConflicting(const UClass* First, const UClass* Second)`
- L259: `bool UHodgeAbilitySet::ValidateForGrant(const UHodgeAbilitySystemComponent* ASC, TArray<FText>& Errors, bool bForInitialization) const`
- L305: `bool UHodgeAbilitySet::ValidateBatchForGrant(const TArray<const UHodgeAbilitySet*>& Sets,`

## HodgeAssetManager.cpp

资产入口、GameData 缓存、启动任务、同步加载、加载进度。Cue 初始化钩子仍需接通。（PreloadPrimaryAssetBundles 已随未提交改动回退，当前不存在。）

源码：[Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)、[Data/HodgeAssetManagerStartupJob.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h)

定义候选（多行签名仅展示首行）：

- L13: `const FName FHodgeBundles::Equipped("Equipped");`
- L48: `UHodgeAssetManager::UHodgeAssetManager()`
- L55: `UHodgeAssetManager& UHodgeAssetManager::Get()`
- L75: `void UHodgeAssetManager::DumpLoadedAssets()`
- L91: `const UHodgeGameData& UHodgeAssetManager::GetGameData()`
- L98: `const UHodgePawnData* UHodgeAssetManager::GetDefaultPawnData() const`
- L105: `UObject* UHodgeAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)`
- L141: `bool UHodgeAssetManager::ShouldLogAssetLoads()`
- L153: `void UHodgeAssetManager::AddLoadedAsset(const UObject* Asset)`
- L167: `void UHodgeAssetManager::StartInitialLoading()`
- L189: `UPrimaryDataAsset* UHodgeAssetManager::LoadGameDataOfClass(`
- L292: `void UHodgeAssetManager::DoAllStartupJobs()`
- L382: `void UHodgeAssetManager::InitializeGameplayCueManager()`
- L394: `void UHodgeAssetManager::UpdateInitialGameContentLoadPercent(`
- L403: `void UHodgeAssetManager::PreBeginPIE(bool bStartSimulate)`

## HodgeAssetManagerStartupJob.cpp

封装启动任务与进度权重，供 AssetManager 执行启动工作。

源码：[Source/Hodgepodge/Private/Data/HodgeAssetManagerStartupJob.cpp](../../../Source/Hodgepodge/Private/Data/HodgeAssetManagerStartupJob.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeAssetManagerStartupJob.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h)

定义候选（多行签名仅展示首行）：

- L16: `TSharedPtr<FStreamableHandle> FHodgeAssetManagerStartupJob::DoJob() const`

## HodgeCharacterStatProfile.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgeCharacterStatProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeCharacterStatProfile.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/Stats/HodgeCharacterBaseStatEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeCharacterBaseStatEffect.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

定义候选（多行签名仅展示首行）：

- L15: `UHodgeCharacterStatProfile::UHodgeCharacterStatProfile()`
- L21: `bool UHodgeCharacterStatProfile::Evaluate(int32 Level, float& Health, float& Damage) const`
- L31: `bool UHodgeCharacterStatProfile::Validate(TArray<FText>& Errors) const`
- L60: `EDataValidationResult UHodgeCharacterStatProfile::IsDataValid(FDataValidationContext& Context) const`

## HodgeCinematicCameraProfile.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgeCinematicCameraProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeCinematicCameraProfile.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeCinematicCameraProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h)

定义候选（多行签名仅展示首行）：

- L27: `bool UHodgeCinematicCameraProfile::Validate(TArray<FText>& Errors) const`
- L67: `bool UHodgeCinematicCameraProfile::ValidateMesh(const USkeletalMeshComponent* Mesh, TArray<FText>& Errors) const`
- L82: `bool UHodgeCinematicCameraProfile::Sample(float NormalizedTime, FHodgeCinematicCameraSample& Out) const`
- L97: `float UHodgeCinematicCameraProfile::EvaluateWeight(float Position, float WindowStart, float WindowEnd) const`
- L106: `EDataValidationResult UHodgeCinematicCameraProfile::IsDataValid(FDataValidationContext& Context) const`

## HodgeComboDefinition.cpp

跳转 DataTable、输入缓存、结束后连段记忆与 bAllowAfterExecutionEnded 续段许可。

源码：[Source/Hodgepodge/Private/Data/HodgeComboDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeComboDefinition.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)

定义候选（多行签名仅展示首行）：

- L11: `const FHodgeComboRow* UHodgeComboDefinition::FindNode(FGameplayTag Tag) const`
- L19: `bool UHodgeComboDefinition::ValidateDefinition(TArray<FText>& Errors) const`
- L100: `EDataValidationResult UHodgeComboDefinition::IsDataValid(FDataValidationContext& Context) const`

## HodgeEquipmentStatProfile.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgeEquipmentStatProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeEquipmentStatProfile.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeEquipmentStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/Stats/HodgeEquipmentStatEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Stats/HodgeEquipmentStatEffect.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/AttributeSet/HodgeCombatSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeCombatSet.h)

定义候选（多行签名仅展示首行）：

- L15: `UHodgeEquipmentStatProfile::UHodgeEquipmentStatProfile()`
- L21: `bool UHodgeEquipmentStatProfile::Evaluate(int32 Level, float& Health, float& Damage) const`
- L31: `bool UHodgeEquipmentStatProfile::Validate(TArray<FText>& Errors) const`
- L60: `EDataValidationResult UHodgeEquipmentStatProfile::IsDataValid(FDataValidationContext& Context) const`

## HodgeExperienceActionSet.cpp

复用 GameFeature 插件和动作配置的数据资产。

源码：[Source/Hodgepodge/Private/Data/HodgeExperienceActionSet.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceActionSet.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeExperienceActionSet.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h)

定义候选（多行签名仅展示首行）：

- L19: `UHodgeExperienceActionSet::UHodgeExperienceActionSet()`
- L26: `EDataValidationResult UHodgeExperienceActionSet::IsDataValid(FDataValidationContext& Context) const`
- L65: `void UHodgeExperienceActionSet::UpdateAssetBundleData()`

## HodgeExperienceDefinition.cpp

声明玩法所需插件、默认 PawnData、直接 Actions 和组合 ActionSets。

源码：[Source/Hodgepodge/Private/Data/HodgeExperienceDefinition.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceDefinition.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)

定义候选（多行签名仅展示首行）：

- L19: `UHodgeExperienceDefinition::UHodgeExperienceDefinition()`
- L26: `EDataValidationResult UHodgeExperienceDefinition::IsDataValid(FDataValidationContext& Context) const`
- L100: `void UHodgeExperienceDefinition::UpdateAssetBundleData()`

## HodgeExperienceManager.cpp

管理编辑器等场景的 GameFeature 使用/停用协调，不是挂载在 GameState 的组件。

源码：[Source/Hodgepodge/Private/Data/HodgeExperienceManager.cpp](../../../Source/Hodgepodge/Private/Data/HodgeExperienceManager.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeExperienceManager.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceManager.h)

定义候选（多行签名仅展示首行）：

- L15: `void UHodgeExperienceManager::OnPlayInEditorBegun()`
- L25: `void UHodgeExperienceManager::NotifyOfPluginActivation(const FString PluginURL)`
- L45: `bool UHodgeExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)`

## HodgeGameData.cpp

全局伤害、治疗、动态 Tag GE 的软类引用配置；需编辑器核对实际赋值。

源码：[Source/Hodgepodge/Private/Data/HodgeGameData.cpp](../../../Source/Hodgepodge/Private/Data/HodgeGameData.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)、[Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)

定义候选（多行签名仅展示首行）：

- L13: `UHodgeGameData::UHodgeGameData()`
- L18: `const UHodgeGameData& UHodgeGameData::Get()`

## HodgeHavocAttackRule.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgeHavocAttackRule.cpp](../../../Source/Hodgepodge/Private/Data/HodgeHavocAttackRule.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeHavocAttackRule.h](../../../Source/Hodgepodge/Public/Data/HodgeHavocAttackRule.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_HavocRules.h)、[AbilitySystem/AttributeSet/HodgeHavocSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHavocSet.h)、[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)

定义候选（多行签名仅展示首行）：

- L14: `UHodgeAttackRule* UHodgeHavocAttackRuleConfig::CreateRule(UObject* Outer) const { return NewObject<UHodgeHavocAttackRule>(Outer); }`
- L16: `void UHodgeHavocAttackRuleConfig::ValidateRule(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const`
- L29: `void UHodgeHavocAttackRule::OnSettlement(const FHodgeAttackSettlement& Settlement)`
- L42: `void UHodgeHavocAttackRule::OnExecutionEvent(FGameplayTag Tag)`

## HodgeHitReactionProfile.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgeHitReactionProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeHitReactionProfile.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeHitReactionProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h)

定义候选（多行签名仅展示首行）：

- L12: `const FHodgeHitReactionAnimation* UHodgeHitReactionProfile::FindAnimation(EHodgeImpactType Type) const`
- L18: `bool UHodgeHitReactionProfile::Validate(TArray<FText>& Errors) const`
- L54: `EDataValidationResult UHodgeHitReactionProfile::IsDataValid(FDataValidationContext& Context) const`

## HodgeLockOnProfile.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgeLockOnProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeLockOnProfile.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeLockOnProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)、[Camera/HodgeCameraMode_LockOn.h](../../../Source/Hodgepodge/Public/Camera/HodgeCameraMode_LockOn.h)、[UI/Targeting/HodgeLockOnIndicatorWidget.h](../../../Source/Hodgepodge/Public/UI/Targeting/HodgeLockOnIndicatorWidget.h)

定义候选（多行签名仅展示首行）：

- L7: `UHodgeLockOnProfile::UHodgeLockOnProfile()`
- L9: `bool UHodgeLockOnProfile::Validate(FString& Error) const`
- L26: `EDataValidationResult UHodgeLockOnProfile::IsDataValid(FDataValidationContext& Context) const`

## HodgePawnData.cpp

PawnClass、AbilitySets、ComboDefinition、DefaultWeaponDefinition、输入/相机/关系映射配置，编辑器校验统一在主 cpp。

源码：[Source/Hodgepodge/Private/Data/HodgePawnData.cpp](../../../Source/Hodgepodge/Private/Data/HodgePawnData.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)、[Data/HodgeHitReactionProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)、[Animation/HodgeCombatAnimNotifies.h](../../../Source/Hodgepodge/Public/Animation/HodgeCombatAnimNotifies.h)、[Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L23: `UHodgePawnData::UHodgePawnData(const FObjectInitializer& ObjectInitializer)`
- L34: `EDataValidationResult UHodgePawnData::IsDataValid(FDataValidationContext& Context) const`

## HodgeResonanceAttackRule.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgeResonanceAttackRule.cpp](../../../Source/Hodgepodge/Private/Data/HodgeResonanceAttackRule.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeResonanceAttackRule.h](../../../Source/Hodgepodge/Public/Data/HodgeResonanceAttackRule.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/AttributeSet/HodgeResonanceSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)、[AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)

定义候选（多行签名仅展示首行）：

- L12: `UHodgeResonanceAttackRuleConfig::UHodgeResonanceAttackRuleConfig() { GainEffect = UHodgeResonanceEnergyEffect::StaticClass(); }`
- L14: `UHodgeAttackRule* UHodgeResonanceAttackRuleConfig::CreateRule(UObject* Outer) const { return NewObject<UHodgeResonanceAttackRule>(Outer); }`
- L16: `void UHodgeResonanceAttackRuleConfig::ValidateRule(const UHodgeAbilityDefinition&, TArray<FText>& Errors) const`
- L23: `void UHodgeResonanceAttackRule::OnSettlement(const FHodgeAttackSettlement& Settlement)`

## HodgeSprintAbilityProfile.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Data/HodgeSprintAbilityProfile.cpp](../../../Source/Hodgepodge/Private/Data/HodgeSprintAbilityProfile.cpp)

项目内直接 include（不是运行调用关系）：[Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)

定义候选（多行签名仅展示首行）：

- L11: `bool UHodgeSprintAbilityProfile::Validate(TArray<FText>& Errors) const`
- L57: `bool UHodgeSprintAbilityProfile::IsHandoffWindow(double DashAge) const`
- L60: `bool UHodgeSprintAbilityProfile::IsAuthorityHandoffWindow(double DashAge) const`
- L63: `bool UHodgeSprintAbilityProfile::CanMoveCancel(double DashAge) const`
- L67: `EDataValidationResult UHodgeSprintAbilityProfile::IsDataValid(FDataValidationContext& Context) const`

## HodgeAbilityDefinition.h

Montage/Blend/Timeline、HitWindows/HitPoints、Volume、独立/连段路由及输入；校验几何、权威消息与手持覆盖。

源码：[Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)、[Combat/HodgeAttackFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackFacingTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Engine/DataAsset.h"
   8: #include "AlphaBlend.h"
   9: #include "Animation/AnimMontage.h"
  10: #include "GameplayTagContainer.h"
  11: #include "Combat/HodgeHitDetection.h"
  12: #include "Combat/HodgeAttackFacingTypes.h"
  13: #include "ScalableFloat.h"
  14: #include "HodgeAbilityDefinition.generated.h"
  16: class UHodgeGameplayAbility;
  17: class UHodgeAttackRuleConfig;
  18: class UHodgeAttackRule;
  19: class UHodgeGameplayAbility_Definition;
  20: class UAnimMontage;
  23: UENUM(BlueprintType)
  24: enum class EHodgeAbilityExecutionRoute : uint8 {                   ComboCoordinated,                      Standalone };
  27: USTRUCT(BlueprintType)
  28: struct HODGEPODGE_API FHodgeAbilityBlendSettings
  29: {
  30: 	GENERATED_BODY()
  32: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float Time = 0.1f;
  34: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EMontageBlendMode Mode = EMontageBlendMode::Standard;
  36: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EAlphaBlendOption Curve = EAlphaBlendOption::Linear;
  38: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UCurveFloat> CustomCurve;
  42: 	FAlphaBlend MakeBlend() const;
  46: 	bool IsValid() const;
  47: };
  50: USTRUCT(BlueprintType)
  51: struct FHodgeAbilityExecutionConfig
  52: {
  53: 	GENERATED_BODY()
  55: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
  57: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01")) float PlayRate = 1.f;
  59: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings BlendIn;
  61: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings NaturalBlendOut;
  63: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FHodgeAbilityBlendSettings StopBlendOut;
  64: };
  67: UCLASS(BlueprintType, Const)
  68: class HODGEPODGE_API UHodgeAbilityDefinition : public UPrimaryDataAsset
  69: {
  70: 	GENERATED_BODY()
  71: public:
  73: 	UHodgeAbilityDefinition();
  75: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag AbilityTag;
  77: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat", meta=(Categories="State.Combat.Body")) FGameplayTag ExecutionBodyTag;
  79: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UHodgeGameplayAbility> AbilityClass;
  81: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FHodgeAbilityExecutionConfig ExecutionConfig;
  83: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EHodgeAbilityExecutionRoute ExecutionRoute = EHodgeAbilityExecutionRoute::ComboCoordinated;
  86: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="InputTag")) FGameplayTag InputTag;
  88: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Variant") FGameplayTagContainer RequiredSourceTags;
  89: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Variant") FGameplayTagContainer BlockedSourceTags;
  91: 	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category="Hodge|Rules") TArray<TObjectPtr<UHodgeAttackRuleConfig>> AttackRules;
  93: 	bool CreateAttackRules(UHodgeGameplayAbility_Definition* Source, TArray<TObjectPtr<UHodgeAttackRule>>& OutRules) const;
  96: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc", meta=(ClampMin="0")) float DarkFlowGainPerPhase=0.f;
  98: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc", meta=(ClampMin="0")) float DarkFlowGainOnCast=0.f;
 100: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Havoc") bool bEnhancedNormalAttack=false;
 102: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|MoveCancel", meta=(EditCondition="ExecutionRoute == EHodgeAbilityExecutionRoute::Standalone", Categories="Status.Attack.Cancel"))
 103: 	FGameplayTag StandaloneMoveCancelWindowTag;
 105: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|MoveCancel", meta=(EditCondition="ExecutionRoute == EHodgeAbilityExecutionRoute::Standalone", ClampMin="0.01", ClampMax="1"))
 106: 	float StandaloneMoveIntentThreshold = .1f;
 108: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Facing") FHodgeAttackFacingConfig FacingWarp;
 110: 	bool GetFacingWarpWindow(float& OutStart, float& OutEnd) const;
 112: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat") FHodgeHitEffectConfig DefaultHitConfig;
 114: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Resonance") FScalableFloat ResonanceEnergyGainPerPhase = 0.f;
 116: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Resonance") TSubclassOf<UGameplayEffect> ResonanceEnergyGainEffect;
 120: 	UFUNCTION(BlueprintPure) float GetDuration() const;
 125: 	bool ValidateDefinition(TArray<FText>& Errors) const;
 126: #if WITH_EDITOR
 131: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
 132: #endif
 133: };
```

## HodgeAbilitySet.h

权威端批量授予属性集、技能和 GE，并用句柄集合撤销。

源码：[Source/Hodgepodge/Public/Data/HodgeAbilitySet.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilitySet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "ActiveGameplayEffectHandle.h"
   9: #include "Engine/DataAsset.h"
  10: #include "AttributeSet.h"
  11: #include "GameplayTagContainer.h"
  13: #include "GameplayAbilitySpecHandle.h"
  14: #include "HodgeAbilitySet.generated.h"
  16: class UAttributeSet;
  17: class UGameplayEffect;
  18: class UHodgeAbilitySystemComponent;
  19: class UHodgeGameplayAbility;
  20: class UObject;
  21: class UHodgeAbilityDefinition;
  24: enum class EHodgeAbilityGrantContext : uint8
  25: {
  26: 	AvatarBound,
  27: 	OwnerInitialized
  28: };
  31: USTRUCT(BlueprintType)
  32: struct FHodgeAbilityGrantResult
  33: {
  34: 	GENERATED_BODY()
  36: 	UPROPERTY(BlueprintReadOnly) bool bSucceeded = false;
  38: 	UPROPERTY(BlueprintReadOnly) bool bHasIrreversibleEffects = false;
  40: 	UPROPERTY(BlueprintReadOnly) FString Error;
  41: };
  44: USTRUCT(BlueprintType)
  45: struct FHodgeAbilitySet_Definition
  46: {
  47:  GENERATED_BODY()
  49:  UPROPERTY(EditDefaultsOnly) TObjectPtr<UHodgeAbilityDefinition> Definition;
  51:  UPROPERTY(EditDefaultsOnly, meta=(ClampMin="1")) int32 AbilityLevel = 1;
  52: };
  60: USTRUCT(BlueprintType)
  61: struct FHodgeAbilitySet_GameplayAbility
  62: {
  63: 	GENERATED_BODY()
  65: public:
  67: 	UPROPERTY(EditDefaultsOnly)
  68: 	TSubclassOf<UHodgeGameplayAbility> Ability = nullptr;
  71: 	UPROPERTY(EditDefaultsOnly)
  72: 	int32 AbilityLevel = 1;
  75: 	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
  76: 	FGameplayTag InputTag;
  77: };
  85: USTRUCT(BlueprintType)
  86: struct FHodgeAbilitySet_GameplayEffect
  87: {
  88: 	GENERATED_BODY()
  90: public:
  92: 	UPROPERTY(EditDefaultsOnly)
  93: 	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;
  96: 	UPROPERTY(EditDefaultsOnly)
  97: 	float EffectLevel = 1.0f;
  98: };
 105: USTRUCT(BlueprintType)
 106: struct FHodgeAbilitySet_AttributeSet
 107: {
 108: 	GENERATED_BODY()
 110: public:
 112: 	UPROPERTY(EditDefaultsOnly)
 113: 	TSubclassOf<UAttributeSet> AttributeSet;
 114: };
 122: USTRUCT(BlueprintType)
 123: struct FHodgeAbilitySet_GrantedHandles
 124: {
 125: 	GENERATED_BODY()
 127: public:
 132: 	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
 138: 	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
 144: 	void AddAttributeSet(UAttributeSet* Set);
 150: 	void TakeFromAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC);
 154: 	void Append(FHodgeAbilitySet_GrantedHandles&& Other);
 159: 	bool AreValidFor(const UHodgeAbilitySystemComponent* ASC) const;
 163: 	int32 GetAbilityCount() const { return AbilitySpecHandles.Num(); }
 165: protected:
 167: 	UPROPERTY()
 168: 	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
 171: 	UPROPERTY()
 172: 	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;
 175: 	UPROPERTY()
 176: 	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
 177: };
 188: UCLASS(BlueprintType, Const)
 189: class UHodgeAbilitySet : public UPrimaryDataAsset
 190: {
 191: 	GENERATED_BODY()
 193: public:
 197: 	UHodgeAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 201: 	const TArray<FHodgeAbilitySet_Definition>& GetGrantedDefinitions() const { return GrantedAbilityDefinitions; }
 203: 	const TArray<FHodgeAbilitySet_GameplayAbility>& GetGrantedAbilities() const { return GrantedGameplayAbilities; }
 205: 	const TArray<FHodgeAbilitySet_AttributeSet>& GetGrantedAttributes() const { return GrantedAttributes; }
 207: 	static bool AreAttributeSetTypesConflicting(const UClass* First, const UClass* Second);
 215: 	void GiveToAbilitySystem(UHodgeAbilitySystemComponent* HodgeASC, FHodgeAbilitySet_GrantedHandles* OutGrantedHandles,
 216: 	                         UObject* SourceObject = nullptr) const;
 223: 	bool ValidateForGrant(const UHodgeAbilitySystemComponent* ASC, TArray<FText>& Errors, bool bForInitialization = false) const;
 231: 	static bool ValidateBatchForGrant(const TArray<const UHodgeAbilitySet*>& Sets, const UHodgeAbilitySystemComponent* ASC,
 232: 		TArray<FText>& Errors, bool bForInitialization = false);
 240: 	FHodgeAbilityGrantResult TryGiveToAbilitySystem(UHodgeAbilitySystemComponent* ASC,
 241: 		FHodgeAbilitySet_GrantedHandles& OutHandles, UObject* SourceObject = nullptr, bool bForInitialization = false,
 242: 		EHodgeAbilityGrantContext Context = EHodgeAbilityGrantContext::AvatarBound) const;
 244: 	static FHodgeAbilityGrantResult TryGiveBatchToAbilitySystem(const TArray<const UHodgeAbilitySet*>& Sets,
 245: 		UHodgeAbilitySystemComponent* ASC, FHodgeAbilitySet_GrantedHandles& OutHandles,
 246: 		UObject* SourceObject = nullptr, bool bForInitialization = false,
 247: 		EHodgeAbilityGrantContext Context = EHodgeAbilityGrantContext::AvatarBound);
 249: protected:
 251: 	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
 252: 	TArray<FHodgeAbilitySet_GameplayAbility> GrantedGameplayAbilities;
 255: 	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities", meta=(TitleProperty="Definition"))
 256: 	TArray<FHodgeAbilitySet_Definition> GrantedAbilityDefinitions;
 259: 	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
 260: 	TArray<FHodgeAbilitySet_GameplayEffect> GrantedGameplayEffects;
 263: 	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
 264: 	TArray<FHodgeAbilitySet_AttributeSet> GrantedAttributes;
 265: private:
 267: 	FHodgeAbilityGrantResult GrantInternal(UHodgeAbilitySystemComponent* ASC,
 268: 		FHodgeAbilitySet_GrantedHandles& OutHandles, UObject* SourceObject, EHodgeAbilityGrantContext Context) const;
 269: };
```

## HodgeAssetManager.h

资产入口、GameData 缓存、启动任务、同步加载、加载进度。Cue 初始化钩子仍需接通。（PreloadPrimaryAssetBundles 已随未提交改动回退，当前不存在。）

源码：[Source/Hodgepodge/Public/Data/HodgeAssetManager.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManager.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Engine/AssetManager.h"
  10: #include "HodgeAssetManagerStartupJob.h"
  11: #include "Templates/SubclassOf.h"
  12: #include "HodgeGameData.h"
  13: #include "HodgePawnData.h"
  14: #include "HodgeAssetManager.generated.h"
  18: struct FHodgeBundles
  19: {
  21:     static const FName Equipped;
  22: };
  31: UCLASS(Config = Game)
  32: class HODGEPODGE_API UHodgeAssetManager : public UAssetManager
  33: {
  34:     GENERATED_BODY()
  36: public:
  38:     UHodgeAssetManager();
  44:     static UHodgeAssetManager& Get();
  47:     template <typename AssetType>
  53:     static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);
  56:     template <typename AssetType>
  62:     static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);
  66:     static void DumpLoadedAssets();
  72:     const UHodgeGameData& GetGameData();
  78:     const UHodgePawnData* GetDefaultPawnData() const;
  80: protected:
  83:     template <typename GameDataClass>
  88:     const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
  89:     {
  91:        if (TObjectPtr<UPrimaryDataAsset> const* pResult = GameDataMap.Find(GameDataClass::StaticClass()))
  92:        {
  93:           return *CastChecked<GameDataClass>(*pResult);
  94:        }
  97:        return *CastChecked<const GameDataClass>(
  98:           LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
  99:     }
 107:     static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
 113:     static bool ShouldLogAssetLoads();
 119:     void AddLoadedAsset(const UObject* Asset);
 124:     virtual void StartInitialLoading() override;
 126: #if WITH_EDITOR
 131:     virtual void PreBeginPIE(bool bStartSimulate) override;
 132: #endif
 142:     UPrimaryDataAsset* LoadGameDataOfClass(
 143:         TSubclassOf<UPrimaryDataAsset> DataClass,
 144:         const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath,
 145:         FPrimaryAssetType PrimaryAssetType
 146:     );
 148: protected:
 151:     UPROPERTY(Config)
 152:     TSoftObjectPtr<UHodgeGameData> HodgeGameDataPath;
 155:     UPROPERTY(Transient)
 156:     TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;
 159:     UPROPERTY(Config)
 160:     TSoftObjectPtr<UHodgePawnData> DefaultPawnData;
 162: private:
 166:     void DoAllStartupJobs();
 170:     void InitializeGameplayCueManager();
 176:     void UpdateInitialGameContentLoadPercent(float GameContentPercent);
 179:     TArray<FHodgeAssetManagerStartupJob> StartupJobs;
 181: private:
 184:     UPROPERTY()
 185:     TSet<TObjectPtr<const UObject>> LoadedAssets;
 188:     FCriticalSection LoadedAssetsCritical;
 189: };
 192: template <typename AssetType>
 198: AssetType* UHodgeAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
 199: {
 200:     AssetType* LoadedAsset = nullptr;
 203:     const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
 206:     if (AssetPath.IsValid())
 207:     {
 209:        LoadedAsset = AssetPointer.Get();
 212:        if (!LoadedAsset)
 213:        {
 214:           LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
 215:           ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
 216:        }
 219:        if (LoadedAsset && bKeepInMemory)
 220:        {
 221:           Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
 222:        }
 223:     }
 225:     return LoadedAsset;
 226: }
 229: template <typename AssetType>
 235: TSubclassOf<AssetType> UHodgeAssetManager::GetSubclass(
 236:     const TSoftClassPtr<AssetType>& AssetPointer,
 237:     bool bKeepInMemory)
 238: {
 239:     TSubclassOf<AssetType> LoadedSubclass;
 242:     const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
 245:     if (AssetPath.IsValid())
 246:     {
 248:        LoadedSubclass = AssetPointer.Get();
 251:        if (!LoadedSubclass)
 252:        {
 253:           LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
 254:           ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
 255:        }
 258:        if (LoadedSubclass && bKeepInMemory)
 259:        {
 260:           Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
 261:        }
 262:     }
 264:     return LoadedSubclass;
 265: }
```

## HodgeAssetManagerStartupJob.h

封装启动任务与进度权重，供 AssetManager 执行启动工作。

源码：[Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h](../../../Source/Hodgepodge/Public/Data/HodgeAssetManagerStartupJob.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "Engine/StreamableManager.h"
  16: DECLARE_DELEGATE_OneParam(
  17: 	FHodgeAssetManagerStartupJobSubstepProgress,
  18: 	float
  19: );
  35: struct FHodgeAssetManagerStartupJob
  36: {
  46: 	FHodgeAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;
  65: 	TFunction<
  66: 		void(
  67: 			const FHodgeAssetManagerStartupJob&,
  68: 			TSharedPtr<FStreamableHandle>&
  69: 		)
  70: 	> JobFunc;
  80: 	FString JobName;
  92: 	float JobWeight;
 101: 	mutable double LastUpdate = 0;
 118: 	FHodgeAssetManagerStartupJob(
 119: 		const FString& InJobName,
 120: 		const TFunction<
 121: 			void(
 122: 				const FHodgeAssetManagerStartupJob&,
 123: 				TSharedPtr<FStreamableHandle>&
 124: 			)
 125: 		>& InJobFunc,
 126: 		float InJobWeight
 127: 	)
 128: 		: JobFunc(InJobFunc)
 129: 		  , JobName(InJobName)
 130: 		  , JobWeight(InJobWeight)
 131: 	{
 132: 	}
 146: 	TSharedPtr<FStreamableHandle> DoJob() const;
 160: 	void UpdateSubstepProgress(float NewProgress) const
 161: 	{
 162: 		SubstepProgressDelegate.ExecuteIfBound(NewProgress);
 163: 	}
 179: 	void UpdateSubstepProgressFromStreamable(
 180: 		TSharedRef<FStreamableHandle> StreamableHandle
 181: 	) const
 182: 	{
 185: 		if (SubstepProgressDelegate.IsBound())
 186: 		{
 198: 			double Now = FPlatformTime::Seconds();
 209: 			if (Now - LastUpdate > 1.0 / 60)
 210: 			{
 219: 				SubstepProgressDelegate.Execute(
 220: 					StreamableHandle->GetProgress()
 221: 				);
 226: 				LastUpdate = Now;
 227: 			}
 228: 		}
 229: 	}
 230: };
```

## HodgeCharacterStatProfile.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCharacterStatProfile.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Engine/DataAsset.h"
   8: #include "ScalableFloat.h"
   9: #include "HodgeCharacterStatProfile.generated.h"
  11: class UGameplayEffect;
  14: UCLASS(BlueprintType, Const)
  15: class HODGEPODGE_API UHodgeCharacterStatProfile : public UDataAsset
  16: {
  17: 	GENERATED_BODY()
  18: public:
  20: 	UHodgeCharacterStatProfile();
  22: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MinLevel = 1;
  24: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MaxLevel = 90;
  26: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1")) int32 ConfigurationVersion = 1;
  28: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat MaxHealth = FScalableFloat(100.f);
  30: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat BaseDamage = FScalableFloat(20.f);
  32: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UGameplayEffect> InitializationEffect;
  39: 	bool Evaluate(int32 Level, float& Health, float& Damage) const;
  44: 	bool Validate(TArray<FText>& Errors) const;
  45: #if WITH_EDITOR
  50: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  51: #endif
  52: };
```

## HodgeCinematicCameraProfile.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeCinematicCameraProfile.h)

项目内直接 include（不是运行调用关系）：[Camera/HodgeCinematicCameraTypes.h](../../../Source/Hodgepodge/Public/Camera/HodgeCinematicCameraTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Engine/DataAsset.h"
   4: #include "Camera/HodgeCinematicCameraTypes.h"
   5: #include "Engine/Scene.h"
   6: #include "HodgeCinematicCameraProfile.generated.h"
   7: class UCurveVector;
   8: class UCurveFloat;
   9: class USkeletalMeshComponent;
  10: class UCameraShakeBase;
  11: class USoundBase;
  14: USTRUCT(BlueprintType)
  15: struct HODGEPODGE_API FHodgeCinematicCameraEvent
  16: {
  17: 	GENERATED_BODY()
  19: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName EventId;
  21: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Time = .5f;
  23: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UCameraShakeBase> CameraShake;
  24: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ShakeScale = 1.f;
  26: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USoundBase> Sound;
  27: };
  30: USTRUCT(BlueprintType)
  31: struct HODGEPODGE_API FHodgeCinematicCameraShot
  32: {
  33: 	GENERATED_BODY()
  35: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ShotId;
  37: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="1")) float Start = 0.f;
  39: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="1")) float End = 1.f;
  41: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName Anchor;
  43: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName LookAt;
  45: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector StartOffset = FVector(180.f, 70.f, 15.f);
  47: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector EndOffset = FVector(140.f, 35.f, 10.f);
  49: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector LookAtOffset = FVector::ZeroVector;
  51: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector2D FOV = FVector2D(60.f, 45.f);
  53: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UCurveVector> PositionCurve;
  55: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UCurveFloat> FOVCurve;
  57: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="1")) float BlendFraction = .2f;
  58: };
  61: UCLASS(BlueprintType)
  62: class HODGEPODGE_API UHodgeCinematicCameraProfile : public UDataAsset
  63: {
  64: 	GENERATED_BODY()
  65: public:
  67: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shots") TArray<FHodgeCinematicCameraShot> Shots;
  69: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation") TArray<FHodgeCinematicCameraEvent> LocalEvents;
  71: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation") FPostProcessSettings PostProcess;
  72: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation", meta=(ClampMin="0", ClampMax="1")) float PostProcessWeight = 0.f;
  74: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend") FVector2D BlendFractions = FVector2D(.16f, .2f);
  76: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend", meta=(Units="s", ClampMin="0")) float CancelBlendSeconds = .25f;
  78: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input") bool bSuppressLookInput = true;
  80: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Safety") float ProbeRadius = 8.f;
  81: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Safety") float MinLookAtDistance = 35.f;
  83: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Safety") FVector2D PitchLimits = FVector2D(-70.f, 70.f);
  85: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Safety") FName FallbackAnchor;
  87: 	bool Validate(TArray<FText>& Errors) const;
  89: 	bool ValidateMesh(const USkeletalMeshComponent* Mesh, TArray<FText>& Errors) const;
  91: 	bool Sample(float NormalizedTime, FHodgeCinematicCameraSample& Out) const;
  93: 	float EvaluateWeight(float Position, float WindowStart, float WindowEnd) const;
  94: #if WITH_EDITOR
  96: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  97: #endif
  98: };
```

## HodgeComboDefinition.h

跳转 DataTable、输入缓存、结束后连段记忆与 bAllowAfterExecutionEnded 续段许可。

源码：[Source/Hodgepodge/Public/Data/HodgeComboDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Engine/DataAsset.h"
   8: #include "Engine/DataTable.h"
   9: #include "GameplayTagContainer.h"
  10: #include "HodgeComboDefinition.generated.h"
  13: USTRUCT(BlueprintType)
  14: struct FHodgeComboTransition
  15: {
  16: 	GENERATED_BODY()
  18: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TriggerInputIntentTag;
  20: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TriggerEventTag;
  22: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag TargetComboTag;
  24: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer RequiredWindowTags;
  27: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAllowAfterExecutionEnded = false;
  29: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer RequiredSourceTags;
  31: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer BlockedSourceTags;
  33: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 TransitionPriority = 0;
  34: };
  37: USTRUCT(BlueprintType)
  38: struct FHodgeComboRow : public FTableRowBase
  39: {
  40: 	GENERATED_BODY()
  42: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag ComboTag;
  44: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag AbilityTag;
  46: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTagContainer GrantedTags;
  48: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FHodgeComboTransition> Transitions;
  49: };
  52: USTRUCT(BlueprintType)
  53: struct FHodgeComboInputBinding
  54: {
  55: 	GENERATED_BODY()
  57: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag InputTag;
  59: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FGameplayTag IntentTag;
  60: };
  63: UCLASS(BlueprintType, Const)
  64: class HODGEPODGE_API UHodgeComboDefinition : public UPrimaryDataAsset
  65: {
  66: 	GENERATED_BODY()
  67: public:
  69: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UDataTable> ComboTable;
  71: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag EntryComboTag;
  73: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FHodgeComboInputBinding> InputBindings;
  75: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float InputBufferSeconds = 0.3f;
  77: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float ComboRetentionSeconds = 1.f;
  79: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag MoveCancelWindowTag;
  81: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float MoveIntentThreshold = 0.1f;
  86: 	const FHodgeComboRow* FindNode(FGameplayTag Tag) const;
  91: 	bool ValidateDefinition(TArray<FText>& Errors) const;
  92: #if WITH_EDITOR
  97: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  98: #endif
  99: };
```

## HodgeEquipmentStatProfile.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeEquipmentStatProfile.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Engine/DataAsset.h"
   8: #include "ScalableFloat.h"
   9: #include "HodgeEquipmentStatProfile.generated.h"
  11: class UGameplayEffect;
  14: UCLASS(BlueprintType, Const)
  15: class HODGEPODGE_API UHodgeEquipmentStatProfile : public UDataAsset
  16: {
  17: 	GENERATED_BODY()
  18: public:
  20: 	UHodgeEquipmentStatProfile();
  22: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MinLevel = 1;
  24: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="1000")) int32 MaxLevel = 90;
  26: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat MaxHealthBonus = FScalableFloat(0.f);
  28: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat BaseDamageBonus = FScalableFloat(0.f);
  30: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UGameplayEffect> AttributeEffect;
  37: 	bool Evaluate(int32 Level, float& Health, float& Damage) const;
  42: 	bool Validate(TArray<FText>& Errors) const;
  43: #if WITH_EDITOR
  48: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  49: #endif
  50: };
```

## HodgeExperienceActionSet.h

复用 GameFeature 插件和动作配置的数据资产。

源码：[Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceActionSet.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Engine/DataAsset.h"
  10: #include "HodgeExperienceActionSet.generated.h"
  12: class UGameFeatureAction;
  19: UCLASS(BlueprintType, NotBlueprintable)
  20: class HODGEPODGE_API UHodgeExperienceActionSet : public UPrimaryDataAsset
  21: {
  22: 	GENERATED_BODY()
  24: public:
  26: 	UHodgeExperienceActionSet();
  29: #if WITH_EDITOR
  35: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  36: #endif
  40: #if WITH_EDITORONLY_DATA
  43: 	virtual void UpdateAssetBundleData() override;
  44: #endif
  47: public:
  49: 	UPROPERTY(EditAnywhere, Instanced, Category="Actions to Perform")
  50: 	TArray<TObjectPtr<UGameFeatureAction>> Actions;
  54: 	UPROPERTY(EditAnywhere, Category="Feature Dependencies")
  55: 	TArray<FString> GameFeaturesToEnable;
  56: };
```

## HodgeExperienceDefinition.h

声明玩法所需插件、默认 PawnData、直接 Actions 和组合 ActionSets。

源码：[Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Engine/DataAsset.h"
  10: #include "HodgeExperienceDefinition.generated.h"
  12: class UGameFeatureAction;
  13: class UHodgePawnData;
  14: class UHodgeExperienceActionSet;
  22: UCLASS(BlueprintType, Const)
  23: class HODGEPODGE_API UHodgeExperienceDefinition : public UPrimaryDataAsset
  24: {
  25: 	GENERATED_BODY()
  27: public:
  29: 	UHodgeExperienceDefinition();
  32: #if WITH_EDITOR
  38: 	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
  39: #endif
  43: #if WITH_EDITORONLY_DATA
  46: 	virtual void UpdateAssetBundleData() override;
  47: #endif
  50: public:
  52: 	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
  53: 	TArray<FString> GameFeaturesToEnable;
  57: 	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
  58: 	TObjectPtr<const UHodgePawnData> DefaultPawnData;
  61: 	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
  62: 	TArray<TObjectPtr<UGameFeatureAction>> Actions;
  66: 	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
  67: 	TArray<TObjectPtr<UHodgeExperienceActionSet>> ActionSets;
  68: };
```

## HodgeExperienceManager.h

管理编辑器等场景的 GameFeature 使用/停用协调，不是挂载在 GameState 的组件。

源码：[Source/Hodgepodge/Public/Data/HodgeExperienceManager.h](../../../Source/Hodgepodge/Public/Data/HodgeExperienceManager.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Subsystems/EngineSubsystem.h"
  10: #include "HodgeExperienceManager.generated.h"
  16: UCLASS()
  17: class HODGEPODGE_API UHodgeExperienceManager : public UEngineSubsystem
  18: {
  19: 	GENERATED_BODY()
  21: public:
  22: #if WITH_EDITOR
  26: 	void OnPlayInEditorBegun();
  32: 	static void NotifyOfPluginActivation(const FString PluginURL);
  39: 	static bool RequestToDeactivatePlugin(const FString PluginURL);
  41: #else
  47: 	static void NotifyOfPluginActivation(const FString PluginURL)
  48: 	{
  49: 	}
  56: 	static bool RequestToDeactivatePlugin(const FString PluginURL) { return true; }
  58: #endif
  60: private:
  62: 	TMap<FString, int32> GameFeaturePluginRequestCountMap;
  63: };
```

## HodgeGameData.h

全局伤害、治疗、动态 Tag GE 的软类引用配置；需编辑器核对实际赋值。

源码：[Source/Hodgepodge/Public/Data/HodgeGameData.h](../../../Source/Hodgepodge/Public/Data/HodgeGameData.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   8: #include "CoreMinimal.h"
   9: #include "Engine/DataAsset.h"
  10: #include "HodgeGameData.generated.h"
  12: class UGameplayEffect;
  17: UCLASS(BlueprintType, Const, Meta = (DisplayName = "Hodge Game Data", ShortTooltip = "包含全局游戏数据的数据资产"))
  18: class HODGEPODGE_API UHodgeGameData : public UPrimaryDataAsset
  19: {
  20: public:
  22: 	UHodgeGameData();
  25: 	static const UHodgeGameData& Get();
  27: 	GENERATED_BODY()
  29: 	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects",
  30: 		meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
  32: 	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;
  35: 	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects",
  36: 		meta = (DisplayName = "Heal Gameplay Effect (SetByCaller)"))
  38: 	TSoftClassPtr<UGameplayEffect> HealGameplayEffect_SetByCaller;
  41: 	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects")
  42: 	TSoftClassPtr<UGameplayEffect> DynamicTagGameplayEffect;
  43: };
```

## HodgeHavocAttackRule.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Data/HodgeHavocAttackRule.h](../../../Source/Hodgepodge/Public/Data/HodgeHavocAttackRule.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeAttackRule.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackRule.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Combat/HodgeAttackRule.h"
   4: #include "HodgeHavocAttackRule.generated.h"
   7: UCLASS()
   8: class HODGEPODGE_API UHodgeHavocAttackRuleConfig : public UHodgeAttackRuleConfig
   9: {
  10: 	GENERATED_BODY()
  11: public:
  12: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float DarkFlowGainPerPhase = 0.f;
  13: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float DarkFlowGainOnCast = 0.f;
  14: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bEnhancedNormalAttack = false;
  15: 	virtual UHodgeAttackRule* CreateRule(UObject* Outer) const override;
  16: 	virtual void ValidateRule(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const override;
  17: };
  20: UCLASS()
  21: class HODGEPODGE_API UHodgeHavocAttackRule : public UHodgeAttackRule
  22: {
  23: 	GENERATED_BODY()
  24: public:
  25: 	virtual void OnSettlement(const FHodgeAttackSettlement& Settlement) override;
  26: 	virtual void OnExecutionEvent(FGameplayTag Tag) override;
  27: private:
  28: 	FHodgeAttackPhaseLedger Phases;
  29: 	bool bCastCommitted = false;
  30: };
```

## HodgeHitReactionProfile.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeHitReactionProfile.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "Engine/DataAsset.h"
   7: #include "Combat/HodgeHitReactionTypes.h"
   8: #include "HodgeHitReactionProfile.generated.h"
  11: USTRUCT(BlueprintType)
  12: struct HODGEPODGE_API FHodgeHitReactionAnimation
  13: {
  14: 	GENERATED_BODY()
  16: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeImpactType Type = EHodgeImpactType::HitStun;
  18: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
  20: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> AirLoopMontage;
  22: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> LandingMontage;
  24: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UAnimMontage> GetUpMontage;
  26: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float DefaultDuration = 0.4f;
  28: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bAllowWithoutMontage = false;
  29: };
  32: UCLASS(BlueprintType, Const)
  33: class HODGEPODGE_API UHodgeHitReactionProfile : public UPrimaryDataAsset
  34: {
  35: 	GENERATED_BODY()
  36: public:
  38: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FHodgeHitReactionAnimation> Animations;
  40: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<EHodgeImpactType> FallbackToHitStun;
  42: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UAnimMontage> LightFeedbackMontage;
  44: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTag LightFeedbackCue;
  46: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FGameplayTagContainer InterruptibleAbilityTags;
  48: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float DownedDuration = 1.f;
  50: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float MaxControlDuration = 6.f;
  52: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01", Units="s")) float MaxAirborneDuration = 4.f;
  54: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", Units="cm")) float MaxLaunchHeight = 600.f;
  56: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1", Units="cm/s")) float MaxLaunchSpeed = 2500.f;
  58: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) int32 MaxAirHits = 6;
  60: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bAutoGetUp = true;
  65: 	const FHodgeHitReactionAnimation* FindAnimation(EHodgeImpactType Type) const;
  70: 	bool Validate(TArray<FText>& Errors) const;
  71: #if WITH_EDITOR
  76: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  77: #endif
  78: };
```

## HodgeLockOnProfile.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Engine/DataAsset.h"
   4: #include "HodgeLockOnProfile.generated.h"
   5: class UHodgeCameraMode;
   6: class UUserWidget;
   8: UCLASS(BlueprintType)
   9: class HODGEPODGE_API UHodgeLockOnProfile : public UDataAsset
  10: {
  11: 	GENERATED_BODY()
  12: public:
  13: 	UHodgeLockOnProfile();
  15: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="1",Units="cm")) float AcquireDistance = 1500.f;
  16: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="1",Units="cm")) float KeepDistance = 2000.f;
  17: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="1",ClampMax="89",Units="deg")) float AcquireHalfAngle = 60.f;
  18: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="0.01",Units="s")) float KeepCheckInterval = .1f;
  19: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="0",Units="s")) float DistanceGrace = .35f;
  20: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target", meta=(ClampMin="0",Units="s")) float OcclusionGrace = 1.f;
  22: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target") FVector ScoreWeights = FVector(.55,.30,.15);
  24: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Target") TEnumAsByte<ECollisionChannel> SightChannel = ECC_Visibility;
  25: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="0.01",Units="s")) float GestureWindow = .12f;
  26: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="1",Units="deg")) float SwitchAngle = 18.f;
  27: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="1",Units="deg/s")) float SwitchSpeed = 150.f;
  28: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="0",ClampMax="1")) float DirectionConsistency = .8f;
  29: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="0",Units="s")) float SwitchDebounce = .25f;
  30: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="0.01",Units="s")) float RearmWindow = .05f;
  31: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(ClampMin="0",Units="deg")) float RearmAngle = 3.f;
  32: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Network", meta=(ClampMin="0.1",Units="s")) float CommandTimeout = 1.5f;
  33: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Network", meta=(ClampMin="0.1",Units="s")) float ReferenceTimeout = 1.f;
  35: 	UPROPERTY(meta=(DeprecatedProperty,DeprecationMessage="软锁定使用自由环绕与 CameraScreenDeadZone")) float FramingYawLimit = 8.f;
  37: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera") bool bEnableSoftCameraAssist = true;
  38: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input") bool bEnableMouseFlickSwitch = true;
  39: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="0",Units="s")) float CameraManualHold = .15f;
  40: 	UPROPERTY(meta=(DeprecatedProperty,DeprecationMessage="使用 CameraScreenDeadZone")) float CameraYawDeadZone = 20.f;
  41: 	UPROPERTY(meta=(DeprecatedProperty,DeprecationMessage="使用 CameraScreenDeadZone")) float CameraPitchDeadZone = 12.f;
  42: 	UPROPERTY(meta=(DeprecatedProperty,DeprecationMessage="使用 CameraYawStrength 和 CameraPitchStrength")) float CameraRecenterSharpness = 3.f;
  43: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera") FVector2D PitchLimits = FVector2D(-55.f,55.f);
  44: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="1",Units="deg/s")) float CameraYawSpeed = 120.f;
  45: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="1",Units="deg/s")) float CameraPitchSpeed = 60.f;
  46: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="0",Units="s")) float CameraBlendTime = .2f;
  48: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Acquire", meta=(ClampMin="0.05",Units="s")) float CameraAcquireDuration = .35f;
  49: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Acquire", meta=(ClampMin="1",Units="deg/s")) float CameraAcquireSpeed = 720.f;
  51: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Tracking") FVector2D CameraScreenDeadZone = FVector2D(.12,.16);
  52: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Tracking") FVector2D CameraScreenSoftZone = FVector2D(.6,.65);
  54: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Tracking", meta=(ClampMin="0")) float CameraYawStrength = 5.f;
  55: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Tracking", meta=(ClampMin="0")) float CameraPitchStrength = 2.f;
  57: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Input", meta=(ClampMin="0",ClampMax="1")) float CameraMouseSuppression = 1.f;
  58: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Input", meta=(ClampMin="0.01",Units="s")) float CameraAssistRecoveryTime = .35f;
  60: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Safety", meta=(ClampMin="1",Units="cm")) float CameraMinTargetDistance = 100.f;
  61: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera") TSubclassOf<UHodgeCameraMode> CameraMode;
  62: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI") TSoftClassPtr<UUserWidget> IndicatorClass;
  64: 	bool Validate(FString& Error) const;
  65: #if WITH_EDITOR
  66: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  67: #endif
  68: };
```

## HodgePawnData.h

PawnClass、AbilitySets、ComboDefinition、DefaultWeaponDefinition、输入/相机/关系映射配置，编辑器校验统一在主 cpp。

源码：[Source/Hodgepodge/Public/Data/HodgePawnData.h](../../../Source/Hodgepodge/Public/Data/HodgePawnData.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   6: #pragma once
   9: #include "CoreMinimal.h"
  12: #include "Engine/DataAsset.h"
  14: #include "HodgePawnData.generated.h"
  17: class UHodgeAbilitySet;
  18: class UHodgeComboDefinition;
  19: class UHodgeEquipmentDefinition;
  20: class UHodgeCharacterStatProfile;
  21: class UHodgeHitReactionProfile;
  22: class UHodgeLockOnProfile;
  25: class UHodgeCameraMode;
  26: class UHodgeSprintAbilityProfile;
  29: class UHodgeInputConfig;
  32: class UHodgeAbilityTagRelationshipMapping;
  40: UCLASS()
  41: class HODGEPODGE_API UHodgePawnData : public UPrimaryDataAsset
  42: {
  43: 	GENERATED_BODY()
  45: public:
  50: 	UHodgePawnData(const FObjectInitializer& ObjectInitializer);
  51: #if WITH_EDITOR
  56: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
  57: #endif
  59: public:
  62: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Pawn")
  63: 	TSubclassOf<APawn> PawnClass;
  66: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Equipment")
  67: 	TSubclassOf<UHodgeEquipmentDefinition> DefaultWeaponDefinition;
  69: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Attributes")
  70: 	TObjectPtr<UHodgeCharacterStatProfile> StatProfile;
  72: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Combat")
  73: 	TObjectPtr<UHodgeHitReactionProfile> HitReactionProfile;
  75: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Movement")
  76: 	TObjectPtr<UHodgeSprintAbilityProfile> SprintAbilityProfile;
  79: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Targeting")
  80: 	TObjectPtr<UHodgeLockOnProfile> LockOnProfile;
  84: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Abilities")
  85: 	TArray<TObjectPtr<UHodgeAbilitySet>> AbilitySets;
  88: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hodge|Abilities")
  89: 	TObjectPtr<UHodgeComboDefinition> ComboDefinition;
  93: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Abilities")
  94: 	TObjectPtr<UHodgeAbilityTagRelationshipMapping> TagRelationshipMapping;
  98: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Input")
  99: 	TObjectPtr<UHodgeInputConfig> InputConfig;
 103: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hodge|Camera")
 104: 	TSubclassOf<UHodgeCameraMode> DefaultCameraMode;
 105: };
```

## HodgeResonanceAttackRule.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Data/HodgeResonanceAttackRule.h](../../../Source/Hodgepodge/Public/Data/HodgeResonanceAttackRule.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeAttackRule.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackRule.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "Combat/HodgeAttackRule.h"
   4: #include "ScalableFloat.h"
   5: #include "HodgeResonanceAttackRule.generated.h"
   6: class UGameplayEffect;
   9: UCLASS()
  10: class HODGEPODGE_API UHodgeResonanceAttackRuleConfig : public UHodgeAttackRuleConfig
  11: {
  12: 	GENERATED_BODY()
  13: public:
  14: 	UHodgeResonanceAttackRuleConfig();
  15: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FScalableFloat GainPerPhase = 0.f;
  16: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSubclassOf<UGameplayEffect> GainEffect;
  17: 	virtual UHodgeAttackRule* CreateRule(UObject* Outer) const override;
  18: 	virtual void ValidateRule(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const override;
  19: };
  22: UCLASS()
  23: class HODGEPODGE_API UHodgeResonanceAttackRule : public UHodgeAttackRule
  24: {
  25: 	GENERATED_BODY()
  26: public:
  27: 	virtual void OnSettlement(const FHodgeAttackSettlement& Settlement) override;
  28: private:
  29: 	FHodgeAttackPhaseLedger Phases;
  30: };
```

## HodgeSprintAbilityProfile.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h](../../../Source/Hodgepodge/Public/Data/HodgeSprintAbilityProfile.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Engine/DataAsset.h"
   8: #include "GameplayTagContainer.h"
   9: #include "HodgeSprintAbilityProfile.generated.h"
  11: class UAnimMontage;
  12: class UAnimSequence;
  13: class UGameplayEffect;
  16: UCLASS(BlueprintType, Const)
  17: class HODGEPODGE_API UHodgeSprintAbilityProfile : public UDataAsset
  18: {
  19: 	GENERATED_BODY()
  20: public:
  22: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") float Duration = .35f;
  24: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") float Distance = 280.f;
  26: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash|Animation", meta=(ClampMin="0.1")) float MontagePlayRate = 1.f;
  28: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash|Recovery") bool bAllowMoveCancel = true;
  30: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash|Recovery", meta=(Units="s")) float MoveCancelOpenTime = .35f;
  32: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Handoff", meta=(Units="s")) float HandoffOpenTime = .35f;
  34: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Handoff", meta=(Units="s")) float HandoffCloseTime = 1.2f;
  36: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Handoff", meta=(Units="s")) float HandoffNetworkGrace = .5f;
  38: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.01", ClampMax="1")) float MoveIntentThreshold = .1f;
  40: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") bool bEnableBackwardVariant = true;
  42: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash", meta=(ClampMin="0", ClampMax="89")) float BackwardConeAngle = 45.f;
  44: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") bool bInstantFacingAtStart = true;
  46: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") float FacingPreparationTimeout = .5f;
  48: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") TObjectPtr<UAnimMontage> ForwardMontage;
  50: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") TObjectPtr<UAnimMontage> BackwardMontage;
  52: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dash") FGameplayTag AttackCancelWindow;
  54: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float InvulnerabilityStart = .04f;
  56: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float InvulnerabilityEnd = .22f;
  58: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float PerfectStart = .04f;
  60: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") float PerfectEnd = .10f;
  62: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Defense") TSubclassOf<UGameplayEffect> PerfectRewardEffect;
  64: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float HoldThreshold = .22f;
  66: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintMaxSpeed = 720.f;
  68: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintAcceleration = 2400.f;
  70: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintBraking = 2000.f;
  72: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float SprintTurnRate = 540.f;
  74: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float NoMoveIntentGrace = .08f;
  76: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") float BlockedExitDelay = .4f;
  78: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint") bool bAllowSprintPivot = true;
  80: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation") TObjectPtr<UAnimSequence> SprintCycle;
  82: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.1", ClampMax="3")) float SprintCyclePlayRate = 1.f;
  84: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="1", Units="cm/s")) float SprintCycleReferenceSpeed = 720.f;
  86: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.01")) float SprintCycleMinPlayRate = .5f;
  88: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.01")) float SprintCycleMaxPlayRate = 1.5f;
  90: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation") TObjectPtr<UAnimSequence> SprintTurn;
  92: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot") TObjectPtr<UAnimMontage> SprintPivotMontage;
  94: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="90", ClampMax="180")) float PivotTriggerAngle = 150.f;
  96: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="0")) float PivotMinimumSpeed = 250.f;
  98: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="0", Units="s")) float PivotReentryDelay = .25f;
 100: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sprint|Pivot", meta=(ClampMin="0.01", Units="s")) float PivotRecoveryEndTime = .9f;
 102: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0.1", ClampMax="3")) float SprintTurnPlayRate = 1.f;
 104: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(ClampMin="0", Units="s")) float SprintTurnBlendOutTime = .12f;
 109: 	bool IsHandoffWindow(double DashAge) const;
 114: 	bool IsAuthorityHandoffWindow(double DashAge) const;
 119: 	bool CanMoveCancel(double DashAge) const;
 124: 	bool Validate(TArray<FText>& Errors) const;
 125: #if WITH_EDITOR
 130: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
 131: #endif
 132: };
```
