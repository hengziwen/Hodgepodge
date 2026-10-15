/** @file HodgeUIAuthoringLibrary.h
 * @brief 声明UMG编辑器制作与验证函数库相关的类型、接口与现有处理流程。
 */
#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HodgeUIAuthoringLibrary.generated.h"
class APlayerController;

/** 仅在编辑器制作 Main UI 资产并调度 PIE 回归，不进入 Game 模块。 */
UCLASS()
class UHodgeUIAuthoringLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
	/** 为共鸣回路生成可编辑Designer条并挂载血条上方；不改动其他HUD区域。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString ConfigureDarkFlowHUD();
	/** 仅向既有HUD添加右下方可编辑充能控件，不重建血条和终焉按钮。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool ConfigureResonanceHUD(class UWidgetBlueprint* Blueprint, TSubclassOf<class UUserWidget> WidgetClass);
	/** 将旧技能子控件迁为UIExtension插槽，保留原布局和其他HUD内容。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool ConfigureAbilityIconHUD(class UWidgetBlueprint* Blueprint, TSubclassOf<class UUserWidget> SkillClass);
	/** 仅为空原型资产创建序列化Designer树，非空用户布局只校验、不覆盖。 */
	UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool ConfigureAbilityIconDesigner(class UWidgetBlueprint* Blueprint);
    /** 将锁定投影层接到已有 HUD；不重建血条或菜单。 */
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool ConfigureLockOnHUD(class UWidgetBlueprint* Blueprint);
    /** 创建FoundationAssets。
     * @return 本次生成的文字或诊断信息。
     */
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString CreateFoundationAssets();
    /** 把指定UI原型迁入保存的UMG控件树与Graph。
     * @return 本次生成的文字或诊断信息。
     */
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString MigrateDesignerAssets();
    /** 检查DesignerAssets。
     * @return 本次生成的文字或诊断信息。
     */
    UFUNCTION(BlueprintPure, Category="Hodge|Editor|UI") static FString InspectDesignerAssets();
    /** 配置DesignerPreviews。
     * @return 本次生成的文字或诊断信息。
     */
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString ConfigureDesignerPreviews();
    /** 配置引用HUD界面。
     * @return 本次生成的文字或诊断信息。
     */
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static FString ConfigureReferenceHUD();
    /** 检查玩家界面。
     * @param Player 玩家，由调用方传入。
     * @return 本次生成的文字或诊断信息。
     */
    UFUNCTION(BlueprintPure, Category="Hodge|Editor|UI") static FString InspectPlayerUI(APlayerController* Player);
    /** 把验证用界面或属性操作排入后续游戏Tick。
     * @param Player 玩家，由调用方传入。
     * @param Action 动作，由调用方传入。
     * @param Value 值，由调用方传入。
     * @return true表示本次检查或操作成功，false表示失败。
     */
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool QueueUIAction(APlayerController* Player, FName Action, float Value = 0);
    /** 制作可编辑锁定标记；已有 Designer 内容只校验，不覆盖用户样式。 */
    UFUNCTION(BlueprintCallable, Category="Hodge|Editor|UI") static bool ConfigureLockOnIndicator(UWidgetBlueprint* Blueprint);
};
