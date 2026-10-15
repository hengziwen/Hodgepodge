/** @file HodgeAbilityIconWidget.h @brief 技能图标消费共享GAS快照，冷却恢复与能量环相互独立。 */
#pragma once
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "HodgeAbilityIconWidget.generated.h"
class UImage;
class UTextBlock;
class UButton;
class UTexture2D;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UHodgeGameplayUIDataSource;
struct FHodgeUIAbilityDisplayState;
struct FHodgeUIResonanceSnapshot;

/** 只读表现结果，不替代GAS资源与冷却事实。 */
USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAbilityIconVisualState
{
	GENERATED_BODY()
	/** 当前剩余冷却，单位秒。 */
	UPROPERTY(BlueprintReadOnly) float CooldownRemaining=0.f;
	/** 冷却亮色从下到上恢复，0为CD刚开始，1为CD结束。 */
	UPROPERTY(BlueprintReadOnly) float CooldownReadyPercent=1.f;
	/** 顶部起点顺时针显示的真实能量比例。 */
	UPROPERTY(BlueprintReadOnly) float EnergyPercent=0.f;
	/** 冷却与充能百分比相互独立。 */
	UPROPERTY(BlueprintReadOnly) bool bCoolingDown=false;
	/** 是否允许同Tag按钮提交技能。 */
	UPROPERTY(BlueprintReadOnly) bool bCanActivate=false;
	/** 授予及必要资源是否准备完成。 */
	UPROPERTY(BlueprintReadOnly) bool bReady=false;
};

/** 共鸣技能与解放共用图标表现，父HUD负责布局，数据源负责生命周期。 */
UCLASS(Blueprintable)
class HODGEPODGE_API UHodgeAbilityIconWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/** 输入标签指向已授予技能，不绑定某个GA运行实例。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability") FGameplayTag InputTag;
	/** 大招启用独立能量环，普通共鸣技能关闭。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability") bool bShowEnergyRing=false;
	/** 资产保持强引用，Tick不加载软资源。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") TObjectPtr<UTexture2D> IconTexture;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") TObjectPtr<UMaterialInterface> IconMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") TObjectPtr<UMaterialInterface> EnergyRingMaterial;
	/** 保留旧大招Widget样式字段，名称用于悬浮提示。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") FText Caption;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") FText KeyLabel;
	/** 金色能量环可调，不修改技能属性。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") FLinearColor FillColor=FLinearColor(1.f,.85f,.35f,1.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style") int32 FontSize=22;
	/** 图标为正方形，键位单独占据下方区域。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style", meta=(ClampMin="32",ClampMax="256")) float IconSize=72.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style", meta=(ClampMin="0",ClampMax="1")) float CooldownDarkness=.25f;
	/** 仅Designer使用，运行时始终读取真实数据。 */
	UPROPERTY(EditAnywhere, Category="Preview", meta=(ClampMin="0",ClampMax="1")) float PreviewEnergyPercent=.65f;
	UPROPERTY(EditAnywhere, Category="Preview", meta=(ClampMin="0",ClampMax="1")) float PreviewCooldownReadyPercent=1.f;
	/** 读取本帧实际提交材质的状态，方便调试与PIE验证。 */
	UFUNCTION(BlueprintPure, Category="Hodge|UI") FHodgeAbilityIconVisualState GetVisualState() const { return VisualState; }
	/** 纯表现计算处理未初始化与异常值，不支付成本或创建CD。 */
	static FHodgeAbilityIconVisualState EvaluateState(const FHodgeUIAbilityDisplayState& Ability,const FHodgeUIResonanceSnapshot& Resource,bool bEnergy);
protected:
	/** 绑定同Tag点击，初始化每控件MID。 */
	virtual void NativeConstruct() override;
	/** 同步样式及设计期预览。 */
	virtual void SynchronizeProperties() override;
	/** 显示帧读取GAS，不累计另一份CD时间。 */
	virtual void NativeTick(const FGeometry& Geometry,float DeltaTime) override;
	/** 只解绑自身，不关闭共享数据源。 */
	virtual void NativeDestruct() override;
private:
	/** 名称用于可选Designer布局绑定。 */
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UImage> SkillIcon;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UImage> EnergyRing;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> CooldownText;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> KeyText;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UButton> AbilityButton;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> IconMID;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> RingMID;
	UPROPERTY(Transient) TObjectPtr<UHodgeGameplayUIDataSource> Data;
	FHodgeAbilityIconVisualState VisualState;
	/** 沿用Lyra材质进度条的MID参数更新方式。 */
	void ApplyStyle();
	/** 统一更新图标、数字与独立充能环。 */
	void RefreshDisplay();
	/** Button和E/R按键共用数据源输入入口。 */
	UFUNCTION() void RequestAbility();
};
