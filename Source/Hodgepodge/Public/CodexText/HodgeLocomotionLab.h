/** @file HodgeLocomotionLab.h
 * @brief 声明移动动画实验场景相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/GameMode/HodgeGameModeBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeLocomotionLab.generated.h"

/** Opt-in experiment. Existing HeroComponent still owns movement and look input. */
/** 封装移动表现实验场景组件的状态与操作接口。 */
UCLASS(Blueprintable, ClassGroup=(Hodge), meta=(BlueprintSpawnableComponent))
class HODGEPODGE_API UHodgeLocomotionLabComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    /** 构造移动表现实验场景组件并初始化其默认状态。 */
    UHodgeLocomotionLabComponent();
    /** 进入世界运行阶段并接入本对象的生命周期依赖。 */
    virtual void BeginPlay() override;
    /** 更新组件。
     * @param DeltaTime 本次更新的时间步长，单位秒。
     * @param TickType 帧更新类型，由调用方传入。
     * @param ThisTickFunction This帧更新Function，由调用方传入。
     */
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    // Walk速度的数值或阈值。
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion") float WalkSpeed = 180.f;
    // 奔跑速度的数值或阈值。
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion") float RunSpeed = 420.f;
    // 控制是否战斗朝向的状态标记。
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Locomotion") bool bCombatFacing = true;
    // 控制是否行走的状态标记。
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Locomotion") bool bWalking = false;
    /** 设置战斗朝向。
     * @param bEnabled 是否启用的控制标记。
     */
    UFUNCTION(BlueprintCallable) void SetCombatFacing(bool bEnabled);
    /** 设置行走。
     * @param bEnabled 是否启用的控制标记。
     */
    UFUNCTION(BlueprintCallable) void SetWalking(bool bEnabled);
    /** 设置键盘Controls启用。
     * @param bEnabled 是否启用的控制标记。
     */
    UFUNCTION(BlueprintCallable) void SetKeyboardControlsEnabled(bool bEnabled) { bKeyboardControlsEnabled = bEnabled; }
private:
    // 控制是否键盘Controls启用的状态标记。
    bool bKeyboardControlsEnabled = true;
};

/** Uses the normal Hodge Experience/PlayerState/ASC initialization in an isolated map. */
/** 封装移动表现实验场景Mode的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API AHodgeLocomotionLabMode : public AHodgeGameModeBase
{
    GENERATED_BODY()
public:
    /** 初始化游戏。
     * @param MapName 映射名称，由调用方传入。
     * @param Options Options，由调用方传入。
     * @param ErrorMessage 错误消息，由调用方传入。
     */
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};

/** Editor-only authoring implementation; no editor modules are linked in Game builds. */
/** 封装移动表现实验场景制作的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeLocomotionLabAuthoring : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /** 重映射复制。
     * @param Asset 资产，由调用方传入。
     * @param Sources Sources，由调用方传入。
     * @param Copies Copies，由调用方传入。
     * @return true表示本次检查或操作成功，false表示失败。
     */
    UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
    static bool RemapCopy(UObject* Asset, const TArray<UObject*>& Sources, const TArray<UObject*>& Copies);
    /** 配置地面混合。
     * @param Blueprint Blueprint，由调用方传入。
     * @param BlendSpace 混合Space，由调用方传入。
     * @return true表示本次检查或操作成功，false表示失败。
     */
    UFUNCTION(BlueprintCallable, Category="CodexText|Editor")
    static bool ConfigureGroundBlend(UObject* Blueprint, UObject* BlendSpace);
};
