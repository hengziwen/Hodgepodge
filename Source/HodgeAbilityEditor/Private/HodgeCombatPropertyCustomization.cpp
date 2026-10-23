/** @file HodgeCombatPropertyCustomization.cpp @brief 只重排原生属性行，保留条件显示、约束和多对象编辑行为。 */
#include "HodgeCombatPropertyCustomization.h"

#include "Animation/HodgeCombatAnimNotifies.h"
#include "Combat/HodgeHitReactionTypes.h"
#include "Data/HodgeHitReactionProfile.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "PropertyHandle.h"
#include "UObject/UnrealType.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "HodgeCombatPropertyCustomization"

namespace
{
	/** 所有控件继续绑定原属性，不缓存配置值或建立第二份可编辑状态。 */
	class FHodgeCombatPropertyCustomization : public IPropertyTypeCustomization
	{
	public:
		/** 高级清单只决定排列；未列入的可编辑字段保留常规入口。 */
		FHodgeCombatPropertyCustomization(FName InGroupName, FText InGroupLabel, TSet<FName> InAdvancedProperties,
			FText InHelpText = FText::GetEmpty());
		virtual void CustomizeHeader(TSharedRef<IPropertyHandle> Handle, FDetailWidgetRow& Row,
			IPropertyTypeCustomizationUtils& Utils) override;
		virtual void CustomizeChildren(TSharedRef<IPropertyHandle> Handle, IDetailChildrenBuilder& Builder,
			IPropertyTypeCustomizationUtils& Utils) override;

	private:
		FName GroupName;
		FText GroupLabel;
		TSet<FName> AdvancedProperties;
		FText HelpText;
	};

	FHodgeCombatPropertyCustomization::FHodgeCombatPropertyCustomization(FName InGroupName, FText InGroupLabel,
		TSet<FName> InAdvancedProperties, FText InHelpText)
		: GroupName(InGroupName), GroupLabel(MoveTemp(InGroupLabel)), AdvancedProperties(MoveTemp(InAdvancedProperties)),
		HelpText(MoveTemp(InHelpText))
	{
	}

	void FHodgeCombatPropertyCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> Handle,
		FDetailWidgetRow& Row, IPropertyTypeCustomizationUtils& Utils)
	{
		// 使用原生结构标题，数组元素、多对象值及重置行为由属性系统处理。
		Row.NameContent()[Handle->CreatePropertyNameWidget()].ValueContent()[Handle->CreatePropertyValueWidget()];
	}

	void FHodgeCombatPropertyCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> Handle,
		IDetailChildrenBuilder& Builder, IPropertyTypeCustomizationUtils& Utils)
	{
		// 制作提示仅说明原模式归属，不替换枚举控件或改变已有默认值。
		if (!HelpText.IsEmpty())
		{
			Builder.AddCustomRow(HelpText).WholeRowContent()[SNew(STextBlock).AutoWrapText(true).Text(HelpText)];
		}
		uint32 Count = 0;
		Handle->GetNumChildren(Count);
		TArray<TSharedRef<IPropertyHandle>> AdvancedHandles;
		for (uint32 Index = 0; Index < Count; ++Index)
		{
			const TSharedPtr<IPropertyHandle> Child = Handle->GetChildHandle(Index);
			const FProperty* Property = Child.IsValid() ? Child->GetProperty() : nullptr;
			// 运行态击飞资格和旧 Break 序列化数据不能被定制面板重新暴露。
			if (!Property || !Child->IsValidHandle() || !Property->HasAnyPropertyFlags(CPF_Edit) ||
				Property->HasAnyPropertyFlags(CPF_Transient))
			{
				continue;
			}
			if (AdvancedProperties.Contains(Property->GetFName()))
			{
				AdvancedHandles.Add(Child.ToSharedRef());
			}
			else
			{
				Builder.AddProperty(Child.ToSharedRef());
			}
		}
		if (!AdvancedHandles.IsEmpty())
		{
			// 原生属性行继续解析原 EditCondition、Clamp、Categories 和嵌套定制。
			IDetailGroup& AdvancedGroup = Builder.AddGroup(GroupName, GroupLabel);
			for (const TSharedRef<IPropertyHandle>& Child : AdvancedHandles)
			{
				AdvancedGroup.AddPropertyRow(Child);
			}
		}
	}
}

namespace HodgeCombatDetails
{
	TSharedRef<IPropertyTypeCustomization> MakeHitReactionConfigCustomization()
	{
		// 模式选择及普通削韧仍使用原字段，显式冲击保留原条件显示。
		return MakeShared<FHodgeCombatPropertyCustomization>(TEXT("HodgeExplicitImpact"),
			LOCTEXT("ExplicitImpact", "高级：显式冲击"), TSet<FName>{
				GET_MEMBER_NAME_CHECKED(FHodgeHitReactionConfig, Impacts)
			}, LOCTEXT("ReactionAuthoringHelp", "常规制作使用 PoiseDriven；Legacy 为存量兼容，ExplicitImpact 为高级专项；Can Launch 在分段通知配置。"));
	}

	TSharedRef<IPropertyTypeCustomization> MakePoiseSettingsCustomization()
	{
		// 击飞速度不附加削韧启用条件，普通目标运动参数保持直接可达。
		return MakeShared<FHodgeCombatPropertyCustomization>(TEXT("HodgePoiseAdvanced"),
			LOCTEXT("PoiseAdvanced", "高级：削韧与运动边界"), TSet<FName>{
				GET_MEMBER_NAME_CHECKED(FHodgePoiseSettings, bAccumulateUnderArmor),
				GET_MEMBER_NAME_CHECKED(FHodgePoiseSettings, MaxSamples),
				GET_MEMBER_NAME_CHECKED(FHodgePoiseSettings, MaxDistance)
			});
	}

	TSharedRef<IPropertyTypeCustomization> MakeAnimHitConfigCustomization()
	{
		// 原 Damage 句柄保留定向定制，避免被忽略的嵌套 Reaction 再次成为作者入口。
		return MakeShared<FHodgeCombatPropertyCustomization>(TEXT("HodgeHitQueryAdvanced"),
			LOCTEXT("HitQueryAdvanced", "高级：查询与命中去重"), TSet<FName>{
				GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, Profile),
				GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, TargetPolicy),
				GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, TargetKey),
				GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, MaxTargetDistance),
				GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, MaxAnchorDistance),
				GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, RepeatHitInterval),
				GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, HitGroup),
				GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, AttackPhase),
				GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, bAllowFriendlyFire)
			});
	}

	TSharedRef<IPropertyTypeCustomization> MakeLegacyReactionAnimationCustomization()
	{
		// 高级专项仍编辑当前旧布局，不自动转 Compact 或改写动画引用。
		return MakeShared<FHodgeCombatPropertyCustomization>(TEXT("HodgeLegacyReactionAdvanced"),
			LOCTEXT("LegacyReactionAdvanced", "高级：旧布局方向、时段与运动来源"), TSet<FName>{
				GET_MEMBER_NAME_CHECKED(FHodgeHitReactionAnimation, Variants),
				GET_MEMBER_NAME_CHECKED(FHodgeHitReactionAnimation, SustainSection),
				GET_MEMBER_NAME_CHECKED(FHodgeHitReactionAnimation, RecoverySection),
				GET_MEMBER_NAME_CHECKED(FHodgeHitReactionAnimation, bUseLateralVariants),
				GET_MEMBER_NAME_CHECKED(FHodgeHitReactionAnimation, MotionSource),
				GET_MEMBER_NAME_CHECKED(FHodgeHitReactionAnimation, LandingMotionSource),
				GET_MEMBER_NAME_CHECKED(FHodgeHitReactionAnimation, GetUpMotionSource),
				GET_MEMBER_NAME_CHECKED(FHodgeHitReactionAnimation, AirLoopTransitionTime)
			});
	}
}

#undef LOCTEXT_NAMESPACE
