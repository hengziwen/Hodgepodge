/** @file HodgeWeaponPresentationProfile.h
 * @brief 声明武器显隐与手持配置相关的类型、接口与现有处理流程。
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HodgeWeaponPresentationProfile.generated.h"

class USkeletalMesh;
class UMaterialInterface;
class UCurveFloat;

/** 单件武器的表现配置，时间使用游戏秒，手部和背部姿势相对角色 Mesh 的插槽。 */
UCLASS(BlueprintType, Const)
class HODGEPODGE_API UHodgeWeaponPresentationProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	// 武器网格的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<USkeletalMesh> WeaponMesh;
	// Materials的记录集合。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<TObjectPtr<UMaterialInterface>> Materials;
	// 手部插槽的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName HandSocket = TEXT("WeaponOnHand");
	// 手部偏移的几何或变换数据。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FTransform HandOffset = FTransform::Identity;
	// Back插槽的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName BackSocket = TEXT("WeaponOnBack");
	// Back变换的几何或变换数据。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ToolTip="Offset relative to the character Mesh BackSocket, not the Pawn root."))
	FTransform BackTransform = FTransform::Identity;
	// 返回Arc偏移的几何或变换数据。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FVector ReturnArcOffset = FVector(0.f, 15.f, 10.f);
	// 返回曲线的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TObjectPtr<UCurveFloat> ReturnCurve;
	// 绘制秒的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float DrawSeconds = .08f;
	// 返回Grace秒的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float ReturnGraceSeconds = .06f;
	// 返回秒的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float ReturnSeconds = .3f;
	// Hover秒的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverSeconds = 2.f;
	// Fade秒的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.01")) float FadeSeconds = .35f;
	// HoverAmplitude的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverAmplitude = 2.f;
	// HoverFrequency的数值或阈值。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0")) float HoverFrequency = .8f;
	// 可见性Parameter的数据或对象引用。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName VisibilityParameter = TEXT("WeaponVisibility");
	/** 检查当前配置并收集诊断。
	 * @param Errors 收集配置或执行失败的诊断。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
	bool Validate(TArray<FText>& Errors) const;
#if WITH_EDITOR
	/** 在编辑器校验当前配置并收集诊断。
	 * @param Context 本次操作的上下文。
	 * @return 当前配置的校验结果。
	 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
