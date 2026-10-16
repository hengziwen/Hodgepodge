# Combat 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeAttackFacingTypes.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Combat/HodgeAttackFacingTypes.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeAttackFacingTypes.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeAttackFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackFacingTypes.h)

定义候选（多行签名仅展示首行）：

- L7: `bool FHodgeAttackFacingConfig::Validate(FString& Error) const`
- L16: `bool HodgeAttackFacing::ResolveYaw(float StartYaw, float DesiredYaw, float MaxAngle, float& OutYaw)`
- L21: `bool FHodgeAttackFacingTargetData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)`

## HodgeAttackRule.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Combat/HodgeAttackRule.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeAttackRule.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeAttackRule.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackRule.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)

定义候选（多行签名仅展示首行）：

- L9: `bool FHodgeAttackPhaseLedger::TryReserve(FName Group, FName Phase, float AcceptedDamage)`
- L18: `void UHodgeAttackRule::Initialize(UHodgeGameplayAbility_Definition* InSource, const UHodgeAttackRuleConfig* InConfig)`
- L27: `bool UHodgeAttackRule::IsCurrent() const`
- L37: `bool UHodgeAttackRule::Accepts(const FHodgeAttackSettlement& Settlement) const`

## HodgeCharacterFacingTypes.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Combat/HodgeCharacterFacingTypes.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeCharacterFacingTypes.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)

定义候选（多行签名仅展示首行）：

- L8: `bool HodgeFacing::NormalizeDirection(const FVector& Input, FVector& Output)`
- L17: `FHodgeMoveIntentSnapshot HodgeFacing::MakeMoveIntent(FVector2D Input, float ControlYaw, double Time, int32 Generation)`
- L33: `bool HodgeFacing::IsBaseDriver(EHodgeCharacterFacingDriver Driver)`

## HodgeDamageRules.cpp

检测和 Execution 共用目标、ASC、死亡/Health、自伤/友伤及队伍规则。

源码：[Source/Hodgepodge/Private/Combat/HodgeDamageRules.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeDamageRules.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)、[AbilitySystem/HodgeGameplayTags.h](../../../Source/Hodgepodge/Public/AbilitySystem/HodgeGameplayTags.h)、[AbilitySystem/AttributeSet/HodgeHealthSet.h](../../../Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeHealthSet.h)

定义候选（多行签名仅展示首行）：

- L31: `bool FHodgeDamageRules::CanDamage(const AActor* Source, const AActor* Target, bool bAllowFriendlyFire)`

## HodgeHavocRules.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Combat/HodgeHavocRules.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeHavocRules.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHavocRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeHavocRules.h)

定义候选（多行签名仅展示首行）：

- L4: `float FHodgeHavocFuelClock::Settle(double Now, float Rate)`
- L12: `bool FHodgeHavocProcGate::TryReserve(double Now, double Interval)`
- L18: `float FHodgeHavocDamageRules::GetMultiplier(bool bHavoc, float Bonus)`
- L23: `bool FHodgeHavocHoldSession::Begin(double Now)`
- L29: `bool FHodgeHavocHoldSession::IsReady(double Now,float Threshold) const`
- L34: `bool FHodgeHavocHoldSession::HasExpired(double Now,float Threshold,float Buffer) const`

## HodgeHitDetection.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeHitDetection.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

定义候选（多行签名仅展示首行）：

- L162: `bool FHodgeHitVolumeConfig::Validate(TArray<FText>& Errors) const`
- L194: `UHodgeHitDetectionProfile::UHodgeHitDetectionProfile()`
- L200: `bool UHodgeHitDetectionProfile::Validate(TArray<FText>& Errors) const`
- L226: `EDataValidationResult UHodgeHitDetectionProfile::IsDataValid(FDataValidationContext& Context) const`
- L239: `bool UHodgeSocketSweepStrategy::Capture(const USceneComponent* Component, const FHodgeHitSource& Source,`
- L274: `void UHodgeSocketSweepStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`
- L302: `bool UHodgeBoxSweepStrategy::Capture(const USceneComponent* Component, const FHodgeHitSource& Source,`
- L316: `void UHodgeBoxSweepStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`
- L348: `bool UHodgeShapeQueryStrategy::Capture(const USceneComponent*, const FHodgeHitSource&, FHodgeHitGeometry&) const`
- L355: `void UHodgeShapeQueryStrategy::Detect(UWorld* World, const FHodgeHitSource& Source,`

## HodgeHitReactionTypes.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Combat/HodgeHitReactionTypes.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeHitReactionTypes.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)

定义候选（多行签名仅展示首行）：

- L25: `int32 HodgeHitReaction::BodyRank(FGameplayTag Tag)`
- L36: `int32 HodgeHitReaction::JudgementRank(FGameplayTag Tag)`
- L47: `int32 HodgeHitReaction::ResolveBody(const UAbilitySystemComponent* ASC, FGameplayTag& OutTag)`
- L64: `bool HodgeHitReaction::CanImpact(int32 AttackRank, int32 BodyRank)`
- L70: `bool HodgeHitReaction::CanUseMontage(const UAnimMontage* Montage, const USkeletalMesh* Mesh)`
- L77: `bool FHodgeImpactSpec::IsMovement() const`
- L84: `bool FHodgeImpactSpec::Validate(TArray<FText>& Errors) const`
- L99: `bool FHodgeHitReactionConfig::IsConfigured() const`
- L105: `bool FHodgeHitReactionConfig::Validate(TArray<FText>& Errors) const`

## HodgeLegacyAttackRuleAdapter.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Combat/HodgeLegacyAttackRuleAdapter.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeLegacyAttackRuleAdapter.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeLegacyAttackRuleAdapter.h](../../../Source/Hodgepodge/Public/Combat/HodgeLegacyAttackRuleAdapter.h)、[Data/HodgeAbilityDefinition.h](../../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[Data/HodgeHavocAttackRule.h](../../../Source/Hodgepodge/Public/Data/HodgeHavocAttackRule.h)、[Data/HodgeResonanceAttackRule.h](../../../Source/Hodgepodge/Public/Data/HodgeResonanceAttackRule.h)、[AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h](../../../Source/Hodgepodge/Public/AbilitySystem/Abilities/HodgeGameplayAbility_Definition.h)、[AbilitySystem/Effects/HodgeResonanceEnergyEffect.h](../../../Source/Hodgepodge/Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h)

定义候选（多行签名仅展示首行）：

- L36: `void HodgeLegacyAttackRules::InitializeDefaults(UHodgeAbilityDefinition& Definition)`
- L40: `void HodgeLegacyAttackRules::CreateRules(const UHodgeAbilityDefinition& Definition, UHodgeGameplayAbility_Definition* Source, TArray<TObjectPtr<UHodgeAttackRule>>& OutRules)`
- L49: `void HodgeLegacyAttackRules::Validate(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors)`

## HodgeMovementActionTypes.cpp

模块或基础类型入口。

源码：[Source/Hodgepodge/Private/Combat/HodgeMovementActionTypes.cpp](../../../Source/Hodgepodge/Private/Combat/HodgeMovementActionTypes.cpp)

项目内直接 include（不是运行调用关系）：[Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)

定义候选（多行签名仅展示首行）：

- L19: `bool FHodgeSprintInputSession::Begin(double Time, int32 Generation)`
- L28: `void FHodgeSprintInputSession::Release() { bHeld = false; State = EHodgeSprintInputState::Idle; }`
- L30: `void FHodgeSprintInputSession::Consume() { State = bHeld ? EHodgeSprintInputState::ConsumedUntilRelease : EHodgeSprintInputState::Idle; }`
- L33: `bool FHodgeSprintInputSession::Qualifies(double Time, float Threshold) const`
- L40: `bool FHodgeDashDirections::Select(FVector Desired, FVector ActorForward, bool bEnableBackward, float BackwardHalfAngle, FHodgeDashDirections& Out)`

## HodgeAttackFacingTypes.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Combat/HodgeAttackFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackFacingTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "Abilities/GameplayAbilityTargetTypes.h"
   5: #include "GameplayTagContainer.h"
   6: #include "HodgeAttackFacingTypes.generated.h"
   9: UENUM(BlueprintType)
  10: enum class EHodgeFacingExecutionMode : uint8 { PhysicalTurn, MontageWarp };
  13: UENUM()
  14: enum class EHodgeAttackFacingSource : uint8 { Actor, Camera, MoveInput, LockedTarget };
  17: USTRUCT(BlueprintType)
  18: struct HODGEPODGE_API FHodgeAttackFacingConfig
  19: {
  20: 	GENERATED_BODY()
  21: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bEnabled = false;
  23: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName WarpTargetName = TEXT("HodgeAttackFacing");
  24: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1",ClampMax="179",Units="deg")) float MaxCorrectionAngle = 90.f;
  25: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="1",Units="deg/s")) float MaxRotationRate = 1080.f;
  26: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float RotationTimeMultiplier = 1.f;
  27: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.1",Units="s")) float TargetDataTimeout = 1.f;
  29: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0",ClampMax="90",Units="deg")) float StartYawTolerance = 45.f;
  31: 	bool Validate(FString& Error) const;
  33: 	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0",ClampMax="90",Units="deg")) float TargetYawTolerance = 30.f;
  34: };
  37: USTRUCT()
  38: struct HODGEPODGE_API FHodgeAttackFacingTargetData : public FGameplayAbilityTargetData
  39: {
  40: 	GENERATED_BODY()
  41: 	UPROPERTY() TWeakObjectPtr<AActor> SourceAvatar;
  42: 	UPROPERTY() TWeakObjectPtr<AActor> LockedTarget;
  43: 	UPROPERTY() float StartYaw = 0.f;
  44: 	UPROPERTY() float DesiredYaw = 0.f;
  45: 	UPROPERTY() EHodgeAttackFacingSource Source = EHodgeAttackFacingSource::Actor;
  46: 	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
  47: 	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
  48: };
  49: template<> struct TStructOpsTypeTraits<FHodgeAttackFacingTargetData> : TStructOpsTypeTraitsBase2<FHodgeAttackFacingTargetData>
  50: { enum { WithNetSerializer = true, WithCopy = true }; };
  52: namespace HodgeAttackFacing
  53: {
  55: 	HODGEPODGE_API bool ResolveYaw(float StartYaw, float DesiredYaw, float MaxAngle, float& OutYaw);
  56: }
```

## HodgeAttackRule.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Combat/HodgeAttackRule.h](../../../Source/Hodgepodge/Public/Combat/HodgeAttackRule.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "CoreMinimal.h"
   4: #include "GameplayTagContainer.h"
   5: #include "GameplayAbilitySpecHandle.h"
   6: #include "HodgeAttackRule.generated.h"
   8: class UHodgeGameplayAbility_Definition;
   9: class UHodgeAbilityDefinition;
  10: class UAbilitySystemComponent;
  11: class UHodgeAttackRuleConfig;
  14: struct HODGEPODGE_API FHodgeHitGroupKey
  15: {
  16: 	FName Group;
  17: 	FName Phase;
  18: 	bool operator==(const FHodgeHitGroupKey& Other) const { return Group == Other.Group && Phase == Other.Phase; }
  19: 	friend uint32 GetTypeHash(const FHodgeHitGroupKey& Key) { return HashCombine(GetTypeHash(Key.Group), GetTypeHash(Key.Phase)); }
  20: };
  23: struct HODGEPODGE_API FHodgeAttackPhaseLedger
  24: {
  25: 	bool TryReserve(FName Group, FName Phase, float AcceptedDamage);
  27: 	void Reset() { Phases.Reset(); }
  28: private:
  29: 	TSet<FHodgeHitGroupKey> Phases;
  30: };
  33: struct HODGEPODGE_API FHodgeAttackSettlement
  34: {
  35: 	FGuid ExecutionId;
  36: 	FGuid SettlementId;
  37: 	FGameplayAbilitySpecHandle AbilityHandle;
  38: 	FName HitGroup;
  39: 	FName AttackPhase;
  40: 	TWeakObjectPtr<UAbilitySystemComponent> TargetASC;
  41: 	float AcceptedDamage = 0.f;
  42: };
  45: UCLASS(Abstract)
  46: class HODGEPODGE_API UHodgeAttackRule : public UObject
  47: {
  48: 	GENERATED_BODY()
  49: public:
  51: 	void Initialize(UHodgeGameplayAbility_Definition* InSource, const UHodgeAttackRuleConfig* InConfig);
  53: 	virtual void OnSettlement(const FHodgeAttackSettlement& Settlement) {}
  54: 	virtual void OnExecutionEvent(FGameplayTag Tag) {}
  55: protected:
  56: 	bool IsCurrent() const;
  57: 	bool Accepts(const FHodgeAttackSettlement& Settlement) const;
  58: 	UPROPERTY(Transient) TWeakObjectPtr<UHodgeGameplayAbility_Definition> Source;
  59: 	UPROPERTY(Transient) TObjectPtr<const UHodgeAttackRuleConfig> Config;
  60: private:
  61: 	FGuid ExecutionId;
  62: 	TWeakObjectPtr<UAbilitySystemComponent> BoundASC;
  63: 	TWeakObjectPtr<AActor> BoundAvatar;
  64: };
  67: UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
  68: class HODGEPODGE_API UHodgeAttackRuleConfig : public UObject
  69: {
  70: 	GENERATED_BODY()
  71: public:
  73: 	virtual UHodgeAttackRule* CreateRule(UObject* Outer) const PURE_VIRTUAL(UHodgeAttackRuleConfig::CreateRule, return nullptr;);
  75: 	virtual void ValidateRule(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors) const {}
  76: };
```

## HodgeCharacterFacingTypes.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "HodgeCharacterFacingTypes.generated.h"
  10: UENUM(BlueprintType)
  11: enum class EHodgeCharacterFacingDriver : uint8 { Movement, Controller, ActionDirection };
  14: UENUM(BlueprintType)
  15: enum class EHodgeLocomotionStyle : uint8 {                  FreeDirectional,                    ReservedStrafe };
  18: UENUM(BlueprintType)
  19: enum class EHodgeFacingRequestStatus : uint8 {                Invalid,                  Pending,               Applied,                 Blocked,                Released,              Rejected };
  22: USTRUCT(BlueprintType)
  23: struct FHodgeFacingRequestHandle
  24: {
  25: 	GENERATED_BODY()
  27: 	UPROPERTY(BlueprintReadOnly) FGuid Id;
  29: 	UPROPERTY(BlueprintReadOnly) int32 AvatarGeneration = 0;
  33: 	bool IsValid() const { return Id.IsValid() && AvatarGeneration > 0; }
  34: };
  37: USTRUCT(BlueprintType)
  38: struct FHodgeMoveIntentSnapshot
  39: {
  40: 	GENERATED_BODY()
  42: 	UPROPERTY(BlueprintReadOnly) FVector2D RawInput2D = FVector2D::ZeroVector;
  44: 	UPROPERTY(BlueprintReadOnly) float ControllerYawAtSample = 0.f;
  46: 	UPROPERTY(BlueprintReadOnly) FVector DesiredDirectionWorld = FVector::ZeroVector;
  48: 	UPROPERTY(BlueprintReadOnly) float InputMagnitude = 0.f;
  50: 	UPROPERTY(BlueprintReadOnly) double SampleTime = 0.;
  52: 	UPROPERTY(BlueprintReadOnly) int32 AvatarGeneration = 0;
  53: };
  56: USTRUCT(BlueprintType)
  57: struct FHodgeFacingPresentationSnapshot
  58: {
  59: 	GENERATED_BODY()
  61: 	UPROPERTY(BlueprintReadOnly) EHodgeLocomotionStyle Style = EHodgeLocomotionStyle::FreeDirectional;
  63: 	UPROPERTY(BlueprintReadOnly) EHodgeCharacterFacingDriver Driver = EHodgeCharacterFacingDriver::Movement;
  65: 	UPROPERTY(BlueprintReadOnly) bool bYawLocked = false;
  67: 	UPROPERTY(BlueprintReadOnly) bool bRecoveringFacing = false;
  69: 	UPROPERTY(BlueprintReadOnly) int32 StateVersion = 0;
  70: };
  72: namespace HodgeFacing
  73: {
  79: 	HODGEPODGE_API bool NormalizeDirection(const FVector& Input, FVector& Output);
  87: 	HODGEPODGE_API FHodgeMoveIntentSnapshot MakeMoveIntent(FVector2D Input, float ControlYaw, double Time, int32 Generation);
  92: 	HODGEPODGE_API bool IsBaseDriver(EHodgeCharacterFacingDriver Driver);
  93: }
```

## HodgeCombatActionRequest.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Combat/HodgeCombatActionRequest.h](../../../Source/Hodgepodge/Public/Combat/HodgeCombatActionRequest.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "GameplayTagContainer.h"
   8: #include "GameplayAbilitySpecHandle.h"
   9: #include "HodgeCombatActionRequest.generated.h"
  12: UENUM(BlueprintType)
  13: enum class EHodgeCombatActionRequestType : uint8 {                 StandaloneAbility,               ComboIntent };
  16: UENUM(BlueprintType)
  17: enum class EHodgeCombatActionRequestStatus : uint8 {                  Queued,                 Started,                 Completed,                Cancelled,                 Expired,              Rejected };
  20: USTRUCT(BlueprintType)
  21: struct HODGEPODGE_API FHodgeCombatActionRequestState
  22: {
  23: 	GENERATED_BODY()
  25: 	UPROPERTY(BlueprintReadOnly) FGuid RequestId;
  27: 	UPROPERTY(BlueprintReadOnly) EHodgeCombatActionRequestType Type = EHodgeCombatActionRequestType::StandaloneAbility;
  29: 	UPROPERTY(BlueprintReadOnly) FGameplayTag ActionTag;
  31: 	UPROPERTY(BlueprintReadOnly) EHodgeCombatActionRequestStatus Status = EHodgeCombatActionRequestStatus::Rejected;
  33: 	UPROPERTY(BlueprintReadOnly) FGameplayAbilitySpecHandle AbilityHandle;
  35: 	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
  37: 	UPROPERTY(BlueprintReadOnly) FString Reason;
  41: 	bool IsTerminal() const { return Status != EHodgeCombatActionRequestStatus::Queued && Status != EHodgeCombatActionRequestStatus::Started; }
  42: };
  44: DECLARE_MULTICAST_DELEGATE_OneParam(FHodgeCombatActionRequestChanged, const FHodgeCombatActionRequestState&);
  46: DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHodgeCombatActionRequestChangedBP, const FHodgeCombatActionRequestState&, State);
```

## HodgeDamageRules.h

检测和 Execution 共用目标、ASC、死亡/Health、自伤/友伤及队伍规则。

源码：[Source/Hodgepodge/Public/Combat/HodgeDamageRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeDamageRules.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   8: class AActor;
  11: struct HODGEPODGE_API FHodgeDamageRules
  12: {
  19: 	static bool CanDamage(const AActor* Source, const AActor* Target, bool bAllowFriendlyFire);
  20: };
```

## HodgeHavocRules.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Combat/HodgeHavocRules.h](../../../Source/Hodgepodge/Public/Combat/HodgeHavocRules.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "CoreMinimal.h"
   6: struct HODGEPODGE_API FHodgeHavocFuelClock
   7: {
   9: 	void Start(double Now) { LastTime=Now; bStarted=FMath::IsFinite(Now); }
  11: 	float Settle(double Now, float Rate);
  13: 	void Reset() { bStarted=false; LastTime=0.; }
  14: private:
  15: 	double LastTime=0.;
  16: 	bool bStarted=false;
  17: };
  20: struct HODGEPODGE_API FHodgeHavocProcGate
  21: {
  23: 	bool TryReserve(double Now, double Interval);
  25: 	void Reset() { bReserved=false; LastTime=0.; }
  26: private:
  27: 	double LastTime=0.;
  28: 	bool bReserved=false;
  29: };
  32: struct HODGEPODGE_API FHodgeHavocDamageRules
  33: {
  34: 	static float GetMultiplier(bool bHavoc, float Bonus);
  35: };
  38: struct HODGEPODGE_API FHodgeHavocHoldSession
  39: {
  41: 	bool Begin(double Now);
  43: 	bool IsReady(double Now,float Threshold) const;
  45: 	bool HasExpired(double Now,float Threshold,float Buffer) const;
  47: 	void Release() { bHeld=false; StartTime=0.; }
  48: 	bool IsHeld() const { return bHeld; }
  49: private:
  50: 	double StartTime=0.;
  51: 	bool bHeld=false;
  52: };
```

## HodgeHitDetection.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Combat/HodgeHitDetection.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitDetection.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Engine/DataAsset.h"
   8: #include "Engine/EngineTypes.h"
   9: #include "GameplayTagContainer.h"
  10: #include "Combat/HodgeHitReactionTypes.h"
  11: #include "HodgeHitDetection.generated.h"
  13: class UGameplayEffect;
  14: class USceneComponent;
  15: class UHodgeHitDetectionStrategy;
  16: class UHodgeWeaponInstance;
  17: struct FCollisionQueryParams;
  20: UENUM(BlueprintType)
  21: enum class EHodgeHitGeometryMode : uint8 {                     ExistingSource,                       ConfiguredShape };
  23: UENUM(BlueprintType)
  24: enum class EHodgeHitShape : uint8 {             Sphere,             Box,             Capsule };
  26: UENUM(BlueprintType)
  27: enum class EHodgeHitAnchorKind : uint8 {                  AvatarRoot,                       RegisteredSource,               ExecutionTransform,               ExecutionTarget };
  29: UENUM(BlueprintType)
  30: enum class EHodgeHitTransformPolicy : uint8 {                 Follow,                      SnapshotOnEventEnter };
  32: UENUM(BlueprintType)
  33: enum class EHodgeHitQueryMode : uint8 {             Sweep,             Overlap };
  35: UENUM(BlueprintType)
  36: enum class EHodgeHitSampleMode : uint8 {                 EveryFrame,                      OnceOnEnter };
  38: UENUM(BlueprintType)
  39: enum class EHodgeHitFilterFrame : uint8 {                 Caster,               DetectionAnchor };
  41: UENUM(BlueprintType)
  42: enum class EHodgeHitTargetPolicy : uint8 {                      AnyInVolume,                       LockedTargetInVolume,                      ConfirmedTarget };
  44: UENUM(BlueprintType)
  45: enum class EHodgeHitResultKind : uint8 {             Sweep,             Overlap,                      ConfirmedTarget };
  48: USTRUCT(BlueprintType)
  49: struct HODGEPODGE_API FHodgeHitVolumeConfig
  50: {
  51: 	GENERATED_BODY()
  53: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitGeometryMode GeometryMode = EHodgeHitGeometryMode::ExistingSource;
  55: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  56: 	EHodgeHitShape Shape = EHodgeHitShape::Sphere;
  58: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  59: 	EHodgeHitAnchorKind AnchorKind = EHodgeHitAnchorKind::AvatarRoot;
  61: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  62: 	EHodgeHitTransformPolicy TransformPolicy = EHodgeHitTransformPolicy::Follow;
  64: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape", EditConditionHides))
  65: 	FTransform LocalTransform = FTransform::Identity;
  67: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Sphere", EditConditionHides, ClampMin="0.1", Units="cm"))
  68: 	float SphereRadius = 100.f;
  70: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Box", EditConditionHides, Units="cm"))
  71: 	FVector BoxHalfExtent = FVector(100.f);
  73: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Capsule", EditConditionHides, ClampMin="0.1", Units="cm"))
  74: 	float CapsuleRadius = 50.f;
  76: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="GeometryMode==EHodgeHitGeometryMode::ConfiguredShape && Shape==EHodgeHitShape::Capsule", EditConditionHides, ClampMin="0.1", Units="cm"))
  77: 	float CapsuleHalfHeight = 100.f;
  79: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AnchorSocket;
  81: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName AnchorKey;
  84: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm")) float MaxAnchorDistance = 5000.f;
  89: 	bool Validate(TArray<FText>& Errors) const;
  90: };
  93: USTRUCT(BlueprintType)
  94: struct HODGEPODGE_API FHodgeHitSource
  95: {
  96: 	GENERATED_BODY()
  99: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Combat.Source"))
 100: 	FGameplayTag SourceTag;
 104: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 105: 	FName ComponentName;
 109: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
 110: 	int32 WeaponActorIndex = 0;
 114: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 115: 	TArray<FName> Sockets;
 118: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="2", ClampMax="32"))
 119: 	int32 SegmentSamples = 5;
 123: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 124: 	FVector LocalOffset = FVector::ZeroVector;
 127: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1", Units="cm"))
 128: 	float Radius = 10.f;
 129: };
 132: struct FHodgeHitGeometry
 133: {
 135: 	TArray<FVector> Points;
 137: 	FTransform Transform = FTransform::Identity;
 139: 	FVector BoxExtent = FVector::ZeroVector;
 141: 	EHodgeHitShape Shape = EHodgeHitShape::Sphere;
 143: 	float Radius = 0.f;
 145: 	float HalfHeight = 0.f;
 146: };
 149: UCLASS(BlueprintType, Const)
 150: class HODGEPODGE_API UHodgeHitDetectionProfile : public UDataAsset
 151: {
 152: 	GENERATED_BODY()
 153: public:
 155: 	UHodgeHitDetectionProfile();
 159: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 160: 	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes;
 163: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 164: 	bool bRequireLineOfSight = true;
 167: 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 168: 	TEnumAsByte<ECollisionChannel> ObstructionChannel = ECC_Visibility;
 172: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="180"))
 173: 	float HalfAngleDegrees = 180.f;
 177: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", Units="cm"))
 178: 	float MaxSweepDistance = 300.f;
 182: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="32"))
 183: 	int32 RotationSubsteps = 8;
 185: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitQueryMode QueryMode = EHodgeHitQueryMode::Sweep;
 187: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitSampleMode SampleMode = EHodgeHitSampleMode::EveryFrame;
 189: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) EHodgeHitFilterFrame FilterFrame = EHodgeHitFilterFrame::Caster;
 192: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bContinuousMotion = false;
 198: 	bool Validate(TArray<FText>& Errors) const;
 199: #if WITH_EDITOR
 204: 	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
 205: #endif
 206: };
 209: USTRUCT(BlueprintType)
 210: struct HODGEPODGE_API FHodgeHitDetectionRequest
 211: {
 212: 	GENERATED_BODY()
 215: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Source"))
 216: 	FGameplayTag SourceTag;
 219: 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
 220: 	TObjectPtr<UHodgeHitDetectionProfile> Profile;
 222: 	UPROPERTY(EditAnywhere, BlueprintReadWrite) FHodgeHitVolumeConfig Volume;
 224: 	UPROPERTY() FTransform RuntimeAnchor = FTransform::Identity;
 226: 	UPROPERTY() bool bHasRuntimeAnchor = false;
 228: 	UPROPERTY() TWeakObjectPtr<AActor> RuntimeTarget;
 230: 	UPROPERTY() EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
 232: 	UPROPERTY() float MaxTargetDistance = 2000.f;
 235: 	UPROPERTY() TWeakObjectPtr<USceneComponent> DirectComponent;
 237: 	UPROPERTY() FHodgeHitSource DirectSource;
 240: 	UPROPERTY()
 241: 	TArray<TWeakObjectPtr<AActor>> IgnoredActors;
 242: };
 245: USTRUCT(BlueprintType)
 246: struct HODGEPODGE_API FHodgeHitDetectionBatch
 247: {
 248: 	GENERATED_BODY()
 251: 	UPROPERTY(BlueprintReadOnly) FGuid ExecutionId;
 253: 	UPROPERTY(BlueprintReadOnly) int32 OccurrenceId = INDEX_NONE;
 255: 	UPROPERTY() uint64 SessionHandle = 0;
 257: 	UPROPERTY(BlueprintReadOnly) int32 SampleSequence = 0;
 259: 	UPROPERTY(BlueprintReadOnly) double SampleTime = 0.0;
 261: 	UPROPERTY(BlueprintReadOnly) TArray<FHitResult> Hits;
 263: 	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<USceneComponent> SourceComponent;
 265: 	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<UHodgeWeaponInstance> Weapon;
 267: 	UPROPERTY(BlueprintReadOnly) FVector SourceOrigin = FVector::ZeroVector;
 269: 	UPROPERTY(BlueprintReadOnly) EHodgeHitResultKind ResultKind = EHodgeHitResultKind::Sweep;
 270: };
 273: USTRUCT(BlueprintType)
 274: struct HODGEPODGE_API FHodgeHitEffectConfig
 275: {
 276: 	GENERATED_BODY()
 278: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Defense") bool bCanBeDodged = true;
 280: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Defense") bool bCanTriggerPerfectDodge = true;
 282: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction") FHodgeHitReactionConfig Reaction;
 284: 	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSubclassOf<UGameplayEffect> DamageEffect;
 286: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float DamageMultiplier = 1.f;
 288: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="GameplayEffect.DamageType")) FGameplayTag DamageType;
 290: 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="GameplayEffect.DamageElement")) FGameplayTag DamageElement;
 292: 	UPROPERTY(BlueprintReadOnly) EHodgeHitTargetPolicy TargetPolicy = EHodgeHitTargetPolicy::AnyInVolume;
 294: 	UPROPERTY(BlueprintReadOnly) float RepeatHitInterval = 0.f;
 296: 	UPROPERTY(BlueprintReadOnly) FName HitGroup;
 298: 	UPROPERTY(BlueprintReadOnly) FName AttackPhase;
 300: 	UPROPERTY(BlueprintReadOnly) bool bAllowFriendlyFire = false;
 301: };
 304: UCLASS(Abstract, BlueprintType)
 305: class HODGEPODGE_API UHodgeHitDetectionStrategy : public UObject
 306: {
 307: 	GENERATED_BODY()
 308: public:
 315: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 316: 		FHodgeHitGeometry& OutGeometry) const PURE_VIRTUAL(UHodgeHitDetectionStrategy::Capture, return false;);
 326: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 327: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 328: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const
 329: 		PURE_VIRTUAL(UHodgeHitDetectionStrategy::Detect, );
 330: };
 333: UCLASS()
 334: class HODGEPODGE_API UHodgeSocketSweepStrategy : public UHodgeHitDetectionStrategy
 335: {
 336: 	GENERATED_BODY()
 337: public:
 344: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 345: 		FHodgeHitGeometry& OutGeometry) const override;
 355: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 356: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 357: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 358: };
 361: UCLASS()
 362: class HODGEPODGE_API UHodgeBoxSweepStrategy : public UHodgeHitDetectionStrategy
 363: {
 364: 	GENERATED_BODY()
 365: public:
 372: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 373: 		FHodgeHitGeometry& OutGeometry) const override;
 383: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 384: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 385: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 386: };
 389: UCLASS()
 390: class HODGEPODGE_API UHodgeShapeQueryStrategy : public UHodgeHitDetectionStrategy
 391: {
 392: 	GENERATED_BODY()
 393: public:
 400: 	virtual bool Capture(const USceneComponent* Component, const FHodgeHitSource& Source,
 401: 		FHodgeHitGeometry& OutGeometry) const override;
 411: 	virtual void Detect(UWorld* World, const FHodgeHitSource& Source, const UHodgeHitDetectionProfile* Profile,
 412: 		const FHodgeHitGeometry& Previous, const FHodgeHitGeometry& Current,
 413: 		const FCollisionQueryParams& Params, TArray<FHitResult>& OutHits) const override;
 414: };
```

## HodgeHitReactionTypes.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeHitReactionTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "GameplayTagContainer.h"
   8: #include "NativeGameplayTags.h"
   9: #include "HodgeHitReactionTypes.generated.h"
  11: class UAbilitySystemComponent;
  12: class UAnimMontage;
  13: class USkeletalMesh;
  15: namespace HodgeHitReactionTags
  16: {
  17: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_Normal);
  18: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_Skill);
  19: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_SuperArmor);
  20: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Body_Vajra);
  21: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_Normal);
  22: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_Skill);
  23: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_SuperArmor);
  24: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Judgement_Vajra);
  25: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Controlled);
  26: }
  29: UENUM(BlueprintType)
  30: enum class EHodgeImpactType : uint8 {                HitStun,               Knockback,               Launch,               Knockdown,                AirHit,               Slam };
  33: UENUM(BlueprintType)
  34: enum class EHodgeImpactDirection : uint8 {                   AwayFromSource,               SourceForward,               WorldDirection };
  37: UENUM(BlueprintType)
  38: enum class EHodgeHitAcceptancePolicy : uint8 {                AcceptedDamage,                     ExplicitControl };
  41: UENUM(BlueprintType)
  42: enum class EHodgeHitReactionPhase : uint8 {                     None,                     Controlled,                   Airborne,                 Downed,                    GettingUp };
  45: UENUM(BlueprintType)
  46: enum class EHodgeHitReactionOutcome : uint8
  47: {
  49: 	NoRequest,                 NoDamageOutput,              Rejected,             Dead,               InvalidRequest,                       LowJudgement,
  51: 	NoImpact,                      Unsupported,                NotReady,                          CannotInterrupt,               Busy,               Applied
  52: };
  55: USTRUCT(BlueprintType)
  56: struct HODGEPODGE_API FHodgeImpactSpec
  57: {
  58: 	GENERATED_BODY()
  60: 	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHodgeImpactType Type = EHodgeImpactType::HitStun;
  62: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="s")) float ControlDuration = 0.f;
  64: 	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHodgeImpactDirection Direction = EHodgeImpactDirection::AwayFromSource;
  66: 	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector WorldDirection = FVector::ForwardVector;
  68: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="cm")) float KnockbackDistance = 120.f;
  70: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.01", Units="s")) float MoveDuration = 0.2f;
  72: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="cm/s")) float HorizontalSpeed = 180.f;
  74: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", Units="cm/s")) float VerticalSpeed = 650.f;
  76: 	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bKnockdownOnLanding = false;
  80: 	bool IsMovement() const;
  85: 	bool Validate(TArray<FText>& Errors) const;
  86: };
  89: USTRUCT(BlueprintType)
  90: struct HODGEPODGE_API FHodgeHitReactionConfig
  91: {
  92: 	GENERATED_BODY()
  94: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Attack.Judgement,State.Combat.Body")) FGameplayTag AttackJudgementTag;
  96: 	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FHodgeImpactSpec> Impacts;
  98: 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0")) float ReservedPoiseDamage = 0.f;
 100: 	UPROPERTY(EditAnywhere, BlueprintReadWrite) EHodgeHitAcceptancePolicy HitAcceptancePolicy = EHodgeHitAcceptancePolicy::AcceptedDamage;
 104: 	bool IsConfigured() const;
 109: 	bool Validate(TArray<FText>& Errors) const;
 110: };
 113: USTRUCT(BlueprintType)
 114: struct HODGEPODGE_API FHodgeHitReactionResult
 115: {
 116: 	GENERATED_BODY()
 118: 	UPROPERTY(BlueprintReadOnly) FGuid HitId;
 120: 	UPROPERTY(BlueprintReadOnly) FGameplayTag AttackJudgementTag;
 122: 	UPROPERTY(BlueprintReadOnly) FGameplayTag TargetBodyTag;
 124: 	UPROPERTY(BlueprintReadOnly) int32 AttackRank = 0;
 126: 	UPROPERTY(BlueprintReadOnly) int32 TargetBodyRank = 0;
 128: 	UPROPERTY(BlueprintReadOnly) float ActualDamage = 0.f;
 130: 	UPROPERTY(BlueprintReadOnly) float ReservedPoiseDamage = 0.f;
 132: 	UPROPERTY(BlueprintReadOnly) EHodgeHitReactionOutcome Outcome = EHodgeHitReactionOutcome::NoRequest;
 134: 	UPROPERTY(BlueprintReadOnly) TArray<EHodgeImpactType> AppliedImpacts;
 135: };
 138: USTRUCT(BlueprintType)
 139: struct HODGEPODGE_API FHodgeHitReactionState
 140: {
 141: 	GENERATED_BODY()
 143: 	UPROPERTY(BlueprintReadOnly) int32 Sequence = 0;
 145: 	UPROPERTY(BlueprintReadOnly) EHodgeHitReactionPhase Phase = EHodgeHitReactionPhase::None;
 147: 	UPROPERTY(BlueprintReadOnly) EHodgeImpactType Type = EHodgeImpactType::HitStun;
 149: 	UPROPERTY(BlueprintReadOnly) float StartedAt = 0.f;
 151: 	UPROPERTY(BlueprintReadOnly) float EndsAt = 0.f;
 153: 	UPROPERTY(BlueprintReadOnly) float ChainStartedAt = 0.f;
 155: 	UPROPERTY(BlueprintReadOnly) float AirStartedAt = 0.f;
 157: 	UPROPERTY(BlueprintReadOnly) float AirOriginZ = 0.f;
 159: 	UPROPERTY(BlueprintReadOnly) int32 AirHitCount = 0;
 161: 	UPROPERTY(BlueprintReadOnly) FVector MotionVelocity = FVector::ZeroVector;
 163: 	UPROPERTY(BlueprintReadOnly) float MoveDuration = 0.f;
 165: 	UPROPERTY(BlueprintReadOnly) bool bHasMovement = false;
 167: 	UPROPERTY(BlueprintReadOnly) bool bKnockdownOnLanding = false;
 169: 	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> Montage;
 171: 	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> AirLoopMontage;
 173: 	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> LandingMontage;
 175: 	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAnimMontage> GetUpMontage;
 179: 	bool IsActive() const { return Phase != EHodgeHitReactionPhase::None; }
 180: };
 182: namespace HodgeHitReaction
 183: {
 188: 	HODGEPODGE_API int32 BodyRank(FGameplayTag Tag);
 193: 	HODGEPODGE_API int32 JudgementRank(FGameplayTag Tag);
 199: 	HODGEPODGE_API int32 ResolveBody(const UAbilitySystemComponent* ASC, FGameplayTag& OutTag);
 205: 	HODGEPODGE_API bool CanImpact(int32 AttackRank, int32 BodyRank);
 211: 	HODGEPODGE_API bool CanUseMontage(const UAnimMontage* Montage, const USkeletalMesh* Mesh);
 212: }
```

## HodgeLegacyAttackRuleAdapter.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Combat/HodgeLegacyAttackRuleAdapter.h](../../../Source/Hodgepodge/Public/Combat/HodgeLegacyAttackRuleAdapter.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   2: #pragma once
   3: #include "CoreMinimal.h"
   4: class UHodgeAbilityDefinition;
   5: class UHodgeGameplayAbility_Definition;
   6: class UHodgeAttackRule;
   7: namespace HodgeLegacyAttackRules
   8: {
  10: 	void InitializeDefaults(UHodgeAbilityDefinition& Definition);
  12: 	void CreateRules(const UHodgeAbilityDefinition& Definition, UHodgeGameplayAbility_Definition* Source, TArray<TObjectPtr<UHodgeAttackRule>>& OutRules);
  14: 	void Validate(const UHodgeAbilityDefinition& Definition, TArray<FText>& Errors);
  15: }
```

## HodgeMovementActionTypes.h

模块或基础类型入口。

源码：[Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeMovementActionTypes.h)

项目内直接 include（不是运行调用关系）：[Combat/HodgeCharacterFacingTypes.h](../../../Source/Hodgepodge/Public/Combat/HodgeCharacterFacingTypes.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
   4: #pragma once
   6: #include "CoreMinimal.h"
   7: #include "Combat/HodgeCharacterFacingTypes.h"
   8: #include "NativeGameplayTags.h"
   9: #include "HodgeMovementActionTypes.generated.h"
  11: namespace HodgeMovementTags
  12: {
  13: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dashing);
  14: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DashPreparing);
  15: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sprinting);
  16: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SprintAbility);
  17: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PerfectDodge);
  18: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SprintCue);
  19: 	HODGEPODGE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PerfectDodgeCue);
  20: }
  23: UENUM(BlueprintType)
  24: enum class EHodgeDashVariant : uint8 {             Forward,             Backward };
  27: UENUM(BlueprintType)
  28: enum class EHodgeSprintInputState : uint8 {               Idle,                          DashRequested,                   DashActive,                     SprintActive,                          ConsumedUntilRelease };
  31: USTRUCT(BlueprintType)
  32: struct HODGEPODGE_API FHodgeSprintInputSession
  33: {
  34: 	GENERATED_BODY()
  36: 	UPROPERTY(BlueprintReadOnly) int32 SessionId = 0;
  38: 	UPROPERTY(BlueprintReadOnly) int32 AvatarGeneration = 0;
  40: 	UPROPERTY(BlueprintReadOnly) EHodgeSprintInputState State = EHodgeSprintInputState::Idle;
  42: 	UPROPERTY(BlueprintReadOnly) bool bHeld = false;
  44: 	UPROPERTY(BlueprintReadOnly) double PressedAt = 0.;
  50: 	bool Begin(double Time, int32 Generation);
  52: 	void Release();
  54: 	void Consume();
  60: 	bool Qualifies(double Time, float Threshold) const;
  61: };
  64: USTRUCT(BlueprintType)
  65: struct HODGEPODGE_API FHodgeDashDirections
  66: {
  67: 	GENERATED_BODY()
  69: 	UPROPERTY(BlueprintReadOnly) EHodgeDashVariant Variant = EHodgeDashVariant::Forward;
  71: 	UPROPERTY(BlueprintReadOnly) FVector MoveDirection = FVector::ZeroVector;
  73: 	UPROPERTY(BlueprintReadOnly) FVector FacingDirection = FVector::ZeroVector;
  82: 	static bool Select(FVector Desired, FVector ActorForward, bool bEnableBackward, float BackwardHalfAngle, FHodgeDashDirections& Out);
  83: };
  86: USTRUCT(BlueprintType)
  87: struct FHodgeLocomotionState
  88: {
  89: 	GENERATED_BODY()
  91: 	UPROPERTY(BlueprintReadOnly) int32 ActivationKey = 0;
  93: 	UPROPERTY(BlueprintReadOnly) int32 Version = 0;
  95: 	UPROPERTY(BlueprintReadOnly) bool bSprinting = false;
  97: 	UPROPERTY(BlueprintReadOnly) float MaxSpeed = 0.f;
  99: 	UPROPERTY(BlueprintReadOnly) float Acceleration = 0.f;
 101: 	UPROPERTY(BlueprintReadOnly) float Braking = 0.f;
 103: 	UPROPERTY(BlueprintReadOnly) float TurnRate = 0.f;
 105: 	UPROPERTY(BlueprintReadOnly) bool bPivotAllowed = false;
 107: 	UPROPERTY(BlueprintReadOnly) float SpeedScale = 1.f;
 108: };
```
