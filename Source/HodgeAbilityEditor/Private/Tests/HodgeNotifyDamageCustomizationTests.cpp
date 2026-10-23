/** @file HodgeNotifyDamageCustomizationTests.cpp @brief 用真实属性行生成器验证上下文隐藏且保留底层数据。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Data/HodgeAbilityDefinition.h"
#include "IDetailTreeNode.h"
#include "IPropertyRowGenerator.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "UObject/UnrealType.h"

namespace
{
	/** 读取编辑器实际生成的属性行，不通过反射字段是否存在来假装界面已经隐藏。 */
	void CountEffectProperties(const TArray<TSharedRef<IDetailTreeNode>>& Nodes, int32& Reactions, int32& Effects)
	{
		for (const auto& Node : Nodes)
		{
			const auto Handle = Node->CreatePropertyHandle();
			const auto* Property = Handle.IsValid() ? Handle->GetProperty() : nullptr;
			if (Property && Property->GetOwnerStruct() == FHodgeHitEffectConfig::StaticStruct())
			{
				Reactions += Property->GetFName() == GET_MEMBER_NAME_CHECKED(FHodgeHitEffectConfig, Reaction) ? 1 : 0;
				Effects += Property->GetFName() == GET_MEMBER_NAME_CHECKED(FHodgeHitEffectConfig, DamageEffect) ? 1 : 0;
			}
			TArray<TSharedRef<IDetailTreeNode>> Children;
			Node->GetChildren(Children, true);
			CountEffectProperties(Children, Reactions, Effects);
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeNotifyDamageDetailsTest, "Hodge.Editor.NotifyDamageAuthoring",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 单次、持续和附加分段都隐藏重复入口，Definition默认配置保持可见。 */
bool FHodgeNotifyDamageDetailsTest::RunTest(const FString& Parameters)
{
	auto& Properties = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	auto* Point = NewObject<UHodgeAnimNotify_Hit>();
	Point->Hit.bUseDefaultDamage = false;
	Point->Hit.Damage.Reaction.ReservedPoiseDamage = 37.f;
	Point->AdditionalHits.Add(Point->Hit);
	auto* Window = NewObject<UHodgeAnimNotifyState_HitCheck>();
	Window->Hit.bUseDefaultDamage = false;
	auto* Definition = NewObject<UHodgeAbilityDefinition>();
	for (UObject* Object : {static_cast<UObject*>(Point), static_cast<UObject*>(Window), static_cast<UObject*>(Definition)})
	{
		const auto Generator = Properties.CreatePropertyRowGenerator(FPropertyRowGeneratorArgs());
		Generator->SetObjects({Object});
		int32 Reactions = 0, Effects = 0;
		CountEffectProperties(Generator->GetRootTreeNodes(), Reactions, Effects);
		TestTrue(TEXT("Damage effect rows remain editable"), Effects > 0);
		if (Object == Definition) { TestTrue(TEXT("Definition default reaction remains editable"), Reactions > 0); }
		else { TestEqual(TEXT("Notify damage has no duplicate reaction rows"), Reactions, 0); }
	}
	TestEqual(TEXT("Hiding rows preserves the original serialized field value"), Point->Hit.Damage.Reaction.ReservedPoiseDamage, 37.f);
	TestEqual(TEXT("Additional segment data remains intact"), Point->AdditionalHits[0].Damage.Reaction.ReservedPoiseDamage, 37.f);
	return true;
}
#endif
