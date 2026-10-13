/** @file HodgeSurvivor.h
 * @brief 声明生存玩法实验角色与模式相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "HodgeSurvivor.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UAnimationAsset;
class UMaterialInterface;
class UStaticMesh;
class UCanvasPanel;
class UVerticalBox;
class UTextBlock;
class UButton;
class UProgressBar;

/** 封装Survivor玩家角色的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API AHodgeSurvivorHero : public ACharacter
{
    GENERATED_BODY()
public:
    /** 构造Survivor玩家角色并初始化其默认状态。 */
    AHodgeSurvivorHero();
    /** 推进当前对象的随帧更新。
     * @param DeltaSeconds 本次更新的时间步长，单位秒。
     */
    virtual void Tick(float DeltaSeconds) override;
    /** 将角色输入组件绑定到既有动作与标签配置。
     * @param Input 输入待处理的数据。
     */
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
    /** 离开世界运行阶段并解除本对象持有的监听或资源。
     * @param Reason Reason，由调用方传入。
     */
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    /** 推进当前实验角色的冲刺入口。 */
    UFUNCTION(BlueprintCallable) void Dash();
    // Dash冲刺冷却的数值或阈值。
    UPROPERTY(BlueprintReadOnly) float DashCooldown = 0;
    // Invulnerability的数值或阈值。
    UPROPERTY(BlueprintReadOnly) float Invulnerability = 0;
private:
    /** 沿当前朝向推进实验对象的前向操作。
     * @param Value 值，由调用方传入。
     */
    void Forward(const FInputActionValue& Value);
    /** 沿当前朝向推进实验对象的右向操作。
     * @param Value 值，由调用方传入。
     */
    void Right(const FInputActionValue& Value);
    // 映射的数据或对象引用。
    UPROPERTY() TObjectPtr<UInputMappingContext> Mapping;
    // 前向动作的数据或对象引用。
    UPROPERTY() TObjectPtr<UInputAction> ForwardAction;
    // Right动作的数据或对象引用。
    UPROPERTY() TObjectPtr<UInputAction> RightAction;
    // Idle的数据或对象引用。
    UPROPERTY() TObjectPtr<UAnimationAsset> Idle;
    // 奔跑的数据或对象引用。
    UPROPERTY() TObjectPtr<UAnimationAsset> Run;
    // 控制是否Running的状态标记。
    bool bRunning = false;
};

/** 定义Survivor状态的可选状态与配置取值。 */
UENUM(BlueprintType)
enum class EHodgeSurvivorState : uint8 { /* Playing选项。 */ Playing, /* Upgrade选项。 */ Upgrade, /* Paused选项。 */ Paused, /* Defeat选项。 */ Defeat, /* Victory选项。 */ Victory };

/** 保存Survivor敌人的数据和接口契约。 */
struct FHodgeSurvivorEnemy
{
    // 本次操作关联的Actor。
    TWeakObjectPtr<AActor> Actor;
    // 当前生命值。
    float Health = 30;
    // 速度的数值或阈值。
    float Speed = 130;
    // 控制是否Elite的状态标记。
    bool bElite = false;
};

/** 保存SurvivorOrb的数据和接口契约。 */
struct FHodgeSurvivorOrb
{
    // 本次操作关联的Actor。
    TWeakObjectPtr<AActor> Actor;
    // 值的数值或阈值。
    int32 Value = 1;
};

/** 封装SurvivorMode的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API AHodgeSurvivorMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    /** 构造SurvivorMode并初始化其默认状态。 */
    AHodgeSurvivorMode();
    /** 开始Play。 */
    virtual void StartPlay() override;
    /** 推进当前对象的随帧更新。
     * @param DeltaSeconds 本次更新的时间步长，单位秒。
     */
    virtual void Tick(float DeltaSeconds) override;
    /** 离开世界运行阶段并解除本对象持有的监听或资源。
     * @param Reason Reason，由调用方传入。
     */
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    /** 按当前实验配置选择升级项。
     * @param Slot 槽位，由调用方传入。
     */
    UFUNCTION(BlueprintCallable) void ChooseUpgrade(int32 Slot);
    /** 切换当前实验场景的暂停状态。 */
    UFUNCTION(BlueprintCallable) void TogglePause();
    /** 重置实验玩法的当前运行阶段。 */
    UFUNCTION(BlueprintCallable) void RestartRun();
    /** 退出当前实验运行并返回菜单状态。 */
    UFUNCTION(BlueprintCallable) void ReturnToMenu();
    /** 授予Experience配置。
     * @param Amount 数量，由调用方传入。
     */
    UFUNCTION(BlueprintCallable) void GrantExperience(int32 Amount);
    /** 向当前测试玩家提交验证用伤害。
     * @param Amount 数量，由调用方传入。
     */
    UFUNCTION(BlueprintCallable) void DamageHero(float Amount);
    /** 生成敌人。
     * @param bElite 是否Elite的控制标记。
     */
    UFUNCTION(BlueprintCallable) void SpawnEnemy(bool bElite = false);
    /** 获取敌人数量。
     * @return 敌人数量的当前值或引用。
     */
    UFUNCTION(BlueprintPure) int32 GetEnemyCount() const { return Enemies.Num(); }
    /** 获取Orb数量。
     * @return Orb数量的当前值或引用。
     */
    UFUNCTION(BlueprintPure) int32 GetOrbCount() const { return Orbs.Num(); }
    /** 获取Upgrade文字。
     * @param Slot 槽位，由调用方传入。
     * @return Upgrade文字的当前值或引用。
     */
    UFUNCTION(BlueprintPure) FString GetUpgradeText(int32 Slot) const;
    // Round持续时间的数值或阈值。
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Survivor") float RoundDuration = 180;
    // 生成Interval的数值或阈值。
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Survivor") float SpawnInterval = 1.15f;
    // 接口类型的数据或对象引用。
    UPROPERTY(EditDefaultsOnly, Category="Survivor") TSubclassOf<UUserWidget> InterfaceClass;
    // 敌人材质的数据或对象引用。
    UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EnemyMaterial;
    // Elite材质的数据或对象引用。
    UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EliteMaterial;
    // Energy材质的数据或对象引用。
    UPROPERTY(EditDefaultsOnly, Category="Survivor") TObjectPtr<UMaterialInterface> EnergyMaterial;
    // 当前状态数据。
    UPROPERTY(BlueprintReadOnly) EHodgeSurvivorState State = EHodgeSurvivorState::Playing;
    // 当前生命值。
    UPROPERTY(BlueprintReadOnly) float Health = 100;
    // 最大生命值。
    UPROPERTY(BlueprintReadOnly) float MaxHealth = 100;
    // 本次等待或执行已经经过的时间。
    UPROPERTY(BlueprintReadOnly) float Elapsed = 0;
    // Kills的数值或阈值。
    UPROPERTY(BlueprintReadOnly) int32 Kills = 0;
    // 配置或运行实例的等级。
    UPROPERTY(BlueprintReadOnly) int32 Level = 1;
    // Experience配置的数值或阈值。
    UPROPERTY(BlueprintReadOnly) int32 Experience = 0;
    // 下次等级XP的数值或阈值。
    UPROPERTY(BlueprintReadOnly) int32 NextLevelXP = 5;
    // 伤害的数值或阈值。
    UPROPERTY(BlueprintReadOnly) float Damage = 22;
    // 攻击Interval的数值或阈值。
    UPROPERTY(BlueprintReadOnly) float AttackInterval = .65f;
    // Pickup半径的数值或阈值。
    UPROPERTY(BlueprintReadOnly) float PickupRadius = 320;
    // Pulse等级的数值或阈值。
    UPROPERTY(BlueprintReadOnly) int32 PulseLevel = 0;
    // 玩家角色的数据或对象引用。
    UPROPERTY(BlueprintReadOnly) TObjectPtr<AHodgeSurvivorHero> Hero;
    // 接口的数据或对象引用。
    UPROPERTY(BlueprintReadOnly) TObjectPtr<UUserWidget> Interface;
    // Choices的记录集合。
    TArray<int32> Choices;
private:
    /** 设置奔跑状态。
     * @param NewState 新状态，由调用方传入。
     */
    void SetRunState(EHodgeSurvivorState NewState);
    /** 向当前测试敌人提交验证用命中。
     * @param Index 当前集合中的索引。
     * @param Amount 数量，由调用方传入。
     */
    void HitEnemy(int32 Index, float Amount);
    /** 构造形状。
     * @param Location 位置，由调用方传入。
     * @param Scale 缩放，由调用方传入。
     * @param Material 材质，由调用方传入。
     * @param Life Life，由调用方传入。
     * @return 本次计算、查询或创建得到的Actor对象。
     */
    AActor* MakeShape(FVector Location, FVector Scale, UMaterialInterface* Material, float Life = 0);
    // 球体的数据或对象引用。
    UPROPERTY() TObjectPtr<UStaticMesh> Sphere;
    // 敌人网格的数据或对象引用。
    UPROPERTY() TObjectPtr<USkeletalMesh> EnemyMesh;
    // 敌人奔跑的数据或对象引用。
    UPROPERTY() TObjectPtr<UAnimationAsset> EnemyRun;
    // Enemies的记录集合。
    TArray<FHodgeSurvivorEnemy> Enemies;
    // Orbs的记录集合。
    TArray<FHodgeSurvivorOrb> Orbs;
    // 生成Clock的数值或阈值。
    float SpawnClock = 0;
    // 攻击Clock的数值或阈值。
    float AttackClock = 0;
    // HurtClock的数值或阈值。
    float HurtClock = 0;
    // PulseClock的数值或阈值。
    float PulseClock = 0;
    // 接口Clock的数值或阈值。
    float InterfaceClock = 0;
    // 下次Elite的数值或阈值。
    int32 NextElite = 35;
};

/** 封装生存玩法实验HUD的状态与操作接口。 */
UCLASS()
class HODGEPODGE_API UHodgeSurvivorHUD : public UUserWidget
{
    GENERATED_BODY()
public:
    /** 在控件构建时接入界面数据和生命周期。 */
    virtual void NativeConstruct() override;
    /** 在控件析构时解除界面监听与临时资源。 */
    virtual void NativeDestruct() override;
    /** 重新采集当前依赖的数据并更新缓存状态。
     * @param Mode Mode，由调用方传入。
     */
    void Refresh(AHodgeSurvivorMode* Mode);
    /** 选择当前实验玩法的第一个选项。 */
    UFUNCTION() void ChooseOne();
    /** 选择当前实验玩法的第二个选项。 */
    UFUNCTION() void ChooseTwo();
    /** 选择当前实验玩法的第三个选项。 */
    UFUNCTION() void ChooseThree();
    /** 在当前测试中重新尝试未完成的步骤。 */
    UFUNCTION() void Retry();
    /** 取得或打开当前实验的菜单页面。 */
    UFUNCTION() void Menu();
    /** 暂停本对象当前的采样或更新。 */
    UFUNCTION() void Pause();
private:
    /** 读取或设置当前测试使用的模式。
     * @return 本次计算、查询或创建得到的SurvivorMode。
     */
    AHodgeSurvivorMode* Mode() const;
    /** 创建或设置当前界面使用的文字内容。
     * @param Name 名称，由调用方传入。
     * @param Value 值，由调用方传入。
     */
    void Text(FName Name, const FString& Value);
    /** 添加按钮。
     * @param Box 盒体，由调用方传入。
     * @param Label 标签，由调用方传入。
     * @param OutLabel 输出标签。
     * @return 本次计算、查询或创建得到的按钮。
     */
    UButton* AddButton(UVerticalBox* Box, const FString& Label, UTextBlock*& OutLabel);
    // 模态页的数据或对象引用。
    UPROPERTY() TObjectPtr<UCanvasPanel> Modal;
    // 模态页Title的数据或对象引用。
    UPROPERTY() TObjectPtr<UTextBlock> ModalTitle;
    // 模态页Description的数据或对象引用。
    UPROPERTY() TObjectPtr<UTextBlock> ModalDescription;
    // ChoiceButtons的记录集合。
    UPROPERTY() TArray<TObjectPtr<UButton>> ChoiceButtons;
    // ChoiceLabels的记录集合。
    UPROPERTY() TArray<TObjectPtr<UTextBlock>> ChoiceLabels;
    // 重试按钮的数据或对象引用。
    UPROPERTY() TObjectPtr<UButton> RetryButton;
    // 菜单按钮的数据或对象引用。
    UPROPERTY() TObjectPtr<UButton> MenuButton;
    // 暂停按钮的数据或对象引用。
    UPROPERTY() TObjectPtr<UButton> PauseButton;
    // XP条形显示的数据或对象引用。
    UPROPERTY() TObjectPtr<UProgressBar> XPBar;
    // XP文字的数据或对象引用。
    UPROPERTY() TObjectPtr<UTextBlock> XPText;
    // 暂停标签的数据或对象引用。
    UPROPERTY() TObjectPtr<UTextBlock> PauseLabel;
};
