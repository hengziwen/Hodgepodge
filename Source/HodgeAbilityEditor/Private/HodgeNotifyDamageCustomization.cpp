/** @file HodgeNotifyDamageCustomization.cpp @brief 保留原数据，只收窄攻击通知的重复Reaction编辑面。 */
#include "HodgeNotifyDamageCustomization.h"
#include "Animation/HodgeCombatAnimNotifies.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "UObject/UnrealType.h"

namespace
{
	/** 使用声明属性所属结构识别嵌套上下文，不按资产名称或对象实例猜测。 */
	class FHodgeNotifyDamageIdentifier : public IPropertyTypeIdentifier
	{
	public:
		virtual bool IsPropertyTypeCustomized(const IPropertyHandle& Handle) const override
		{
			const FProperty* Property = Handle.GetProperty();
			return Property && Property->GetOwnerStruct() == FHodgeAnimHitConfig::StaticStruct() &&
				Property->GetFName() == GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, Damage);
		}
	};
}

TSharedRef<IPropertyTypeCustomization> FHodgeNotifyDamageCustomization::MakeInstance()
{
	return MakeShared<FHodgeNotifyDamageCustomization>();
}

TSharedRef<IPropertyTypeIdentifier> FHodgeNotifyDamageCustomization::MakeIdentifier()
{
	return MakeShared<FHodgeNotifyDamageIdentifier>();
}

void FHodgeNotifyDamageCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> Handle, FDetailWidgetRow& Row,
	IPropertyTypeCustomizationUtils& Utils)
{
	// 沿用结构的标题和默认控件，不改变伤害字段的编辑行为。
	Row.NameContent()[Handle->CreatePropertyNameWidget()].ValueContent()[Handle->CreatePropertyValueWidget()];
}

void FHodgeNotifyDamageCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> Handle,
	IDetailChildrenBuilder& Builder, IPropertyTypeCustomizationUtils& Utils)
{
	uint32 Count = 0;
	Handle->GetNumChildren(Count);
	for (uint32 Index = 0; Index < Count; ++Index)
	{
		const auto Child = Handle->GetChildHandle(Index);
		if (!Child.IsValid() || !Child->GetProperty()) { continue; }
		// 仅剔除不会被Melee消费的旧嵌套Reaction，属性本身和已有值不作删除。
		if (Child->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED(FHodgeHitEffectConfig, Reaction)) { continue; }
		Builder.AddProperty(Child.ToSharedRef());
	}
}
