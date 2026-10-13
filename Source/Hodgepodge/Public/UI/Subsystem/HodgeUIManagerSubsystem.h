/** @file HodgeUIManagerSubsystem.h
 * @brief 声明本地玩家UI管理服务相关的类型、接口与现有处理流程。
 */
#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/SoftObjectPtr.h"
#include "HodgeUIManagerSubsystem.generated.h"
class UHodgePrimaryGameLayout;
class UHodgeGameplayUIDataSource;
class ULocalPlayer;
class APlayerController;
struct FStreamableHandle;
DECLARE_MULTICAST_DELEGATE_TwoParams(FHodgeRootLayoutChanged, ULocalPlayer*, UHodgePrimaryGameLayout*);

/** 保存玩家界面布局的数据和接口契约。 */
USTRUCT()
struct FHodgePlayerUILayout
{
    GENERATED_BODY()
    // 当前操作使用的根对象。
    UPROPERTY(Transient) TObjectPtr<UHodgePrimaryGameLayout> Root;
    // 玩法数据的数据或对象引用。
    UPROPERTY(Transient) TObjectPtr<UHodgeGameplayUIDataSource> GameplayData;
    // 当前角色使用的控制器。
    UPROPERTY(Transient) TWeakObjectPtr<APlayerController> Controller;
    // 控制器委托对应的事件通知。
    FDelegateHandle ControllerDelegate;
    // 输入Mode委托对应的事件通知。
    FDelegateHandle InputModeDelegate;
    // 输入Tokens的记录集合。
    TSet<FName> InputTokens;
    // 根节点加载对应操作的配对句柄。
    TSharedPtr<FStreamableHandle> RootLoad;
};

/** 每个本地玩家独立拥有根布局，不管理角色属性或在线会话。 */
UCLASS(Config=Game)
class HODGEPODGE_API UHodgeUIManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    /** 判断是否应创建Subsystem。
     * @param Outer 输出Outer。
     * @return true表示满足该查询条件，false表示不满足。
     */
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    /** 初始化ialize。
     * @param Collection Collection，由调用方传入。
     */
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    /** 在子系统退出时清理其登记内容与监听。 */
    virtual void Deinitialize() override;
    /** 为加入的LocalPlayer建立所需的本地管理状态。
     * @param Player 玩家，由调用方传入。
     */
    void PlayerAdded(ULocalPlayer* Player);
    /** 清理被移除LocalPlayer持有的本地管理状态。
     * @param Player 玩家，由调用方传入。
     */
    void PlayerRemoved(ULocalPlayer* Player);
    /** 获取根节点布局。
     * @param Player 玩家，由调用方传入。
     * @return 根节点布局的当前值或引用。
     */
    UFUNCTION(BlueprintPure, Category="Hodge|UI") UHodgePrimaryGameLayout* GetRootLayout(ULocalPlayer* Player) const;
    /** 判断游戏输入允许。
     * @param Player 玩家，由调用方传入。
     * @return true表示满足该查询条件，false表示不满足。
     */
    UFUNCTION(BlueprintPure, Category="Hodge|UI") bool IsGameInputAllowed(ULocalPlayer* Player) const;
    /** 获取界面管理器。
     * @param WorldContextObject 用于定位所属世界的上下文对象。
     * @return 界面管理器的当前值或引用。
     */
    UFUNCTION(BlueprintPure, Category="Hodge|UI", meta=(WorldContext="WorldContextObject"))
    static UHodgeUIManagerSubsystem* GetUIManager(const UObject* WorldContextObject);
    /** 获取根节点布局关联控制器。
     * @param Controller 当前角色使用的控制器。
     * @return 根节点布局关联控制器的当前值或引用。
     */
    UFUNCTION(BlueprintPure, Category="Hodge|UI") static UHodgePrimaryGameLayout* GetRootLayoutForController(APlayerController* Controller);
    /** 获取玩法数据关联控制器。
     * @param Controller 当前角色使用的控制器。
     * @return 玩法数据关联控制器的当前值或引用。
     */
    UFUNCTION(BlueprintPure, Category="Hodge|UI") static UHodgeGameplayUIDataSource* GetGameplayDataForController(APlayerController* Controller);
    /** 检查当前LocalPlayer是否允许接收玩法输入。
     * @param Controller 当前角色使用的控制器。
     * @return true表示本次检查或操作成功，false表示失败。
     */
    static bool AllowsGameplayInput(const APlayerController* Controller);
    /** 按来源令牌挂起本地玩家的输入。
     * @param Player 玩家，由调用方传入。
     * @param Token 令牌，由调用方传入。
     */
    void SuspendInput(ULocalPlayer* Player, FName Token);
    /** 释放对应来源的输入挂起令牌。
     * @param Player 玩家，由调用方传入。
     * @param Token 令牌，由调用方传入。
     */
    void ResumeInput(ULocalPlayer* Player, FName Token);
    /** 刷新输入状态。
     * @param Player 玩家，由调用方传入。
     */
    void RefreshInputState(ULocalPlayer* Player);
    // 事件根节点布局变化对应的事件通知。
    FHodgeRootLayoutChanged OnRootLayoutChanged;
    /** 获取玩家布局数量。
     * @return 玩家布局数量的当前值或引用。
     */
    int32 GetPlayerLayoutCount() const { return Players.Num(); }
protected:
    // 未配置时使用原生根布局；正式工程指向 Main 内的 Widget Blueprint。
    UPROPERTY(Config, EditDefaultsOnly, Category="UI") TSoftClassPtr<UHodgePrimaryGameLayout> RootLayoutClass;
private:
    // 玩家的记录集合。
    UPROPERTY(Transient) TMap<TObjectPtr<ULocalPlayer>, FHodgePlayerUILayout> Players;
    /** 在控制器切换时重新绑定当前依赖。
     * @param Player 玩家，由调用方传入。
     * @param Controller 当前角色使用的控制器。
     */
    void ControllerChanged(ULocalPlayer* Player, APlayerController* Controller);
    /** 创建根节点。
     * @param Player 玩家，由调用方传入。
     * @param Controller 当前角色使用的控制器。
     */
    void CreateRoot(ULocalPlayer* Player, TWeakObjectPtr<APlayerController> Controller);
    /** 销毁根节点。
     * @param Player 玩家，由调用方传入。
     */
    void DestroyRoot(ULocalPlayer* Player);
    /** 清空玩法输入。
     * @param Player 玩家，由调用方传入。
     */
    void ClearGameplayInput(ULocalPlayer* Player);
};
