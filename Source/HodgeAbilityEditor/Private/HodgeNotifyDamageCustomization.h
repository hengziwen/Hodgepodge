/** @file HodgeNotifyDamageCustomization.h @brief 仅隐藏攻击通知显式伤害中被独立反应入口覆盖的旧字段。 */
#pragma once
#include "IPropertyTypeCustomization.h"

class IPropertyTypeIdentifier;

/** 不修改反射布局或序列化数据，Definition默认反应继续使用原生详情。 */
class FHodgeNotifyDamageCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	/** 仅匹配FHodgeAnimHitConfig.Damage，包含单次、持续及AdditionalHits元素。 */
	static TSharedRef<IPropertyTypeIdentifier> MakeIdentifier();
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> Handle, FDetailWidgetRow& Row,
		IPropertyTypeCustomizationUtils& Utils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> Handle, IDetailChildrenBuilder& Builder,
		IPropertyTypeCustomizationUtils& Utils) override;
};
