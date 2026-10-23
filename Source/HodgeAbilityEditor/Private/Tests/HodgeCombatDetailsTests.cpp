/** @file HodgeCombatDetailsTests.cpp @brief 用真实详情树验证常规入口、兼容布局与原生条件不会因重排失效。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "Animation/HodgeCombatAnimNotifies.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeHitReactionProfile.h"
#include "Data/HodgePawnData.h"
#include "DetailWidgetRow.h"
#include "IDetailPropertyRow.h"
#include "IDetailTreeNode.h"
#include "IPropertyRowGenerator.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "Types/SlateAttributeMetaData.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "Widgets/SWidget.h"

namespace
{
	/** 保存实际生成节点及其分类路径，区分隐藏属性与仅有反射声明的属性。 */
	struct FHodgeGeneratedDetailRow
	{
		TSharedRef<IDetailTreeNode> Node;
		FString Path;
	};

	/** 默认只读取真正可见节点；检查运行字段时另取全部节点防止误暴露。 */
	void CollectDetailRows(const TArray<TSharedRef<IDetailTreeNode>>& Nodes,
		TArray<FHodgeGeneratedDetailRow>& Out, const FString& ParentPath, bool bIgnoreVisibility)
	{
		for (const TSharedRef<IDetailTreeNode>& Node : Nodes)
		{
			const FString Path = ParentPath + TEXT("/") + Node->GetNodeName().ToString();
			Out.Add({Node, Path});
			TArray<TSharedRef<IDetailTreeNode>> Children;
			Node->GetChildren(Children, bIgnoreVisibility);
			CollectDetailRows(Children, Out, Path, bIgnoreVisibility);
		}
	}

	/** 保持生成器存活以使用原生控件，不在测试中替换模块的真实注册。 */
	struct FHodgeGeneratedDetails
	{
		TSharedRef<IPropertyRowGenerator> Generator;
		TArray<FHodgeGeneratedDetailRow> VisibleRows;
		TArray<FHodgeGeneratedDetailRow> AllRows;

		explicit FHodgeGeneratedDetails(const TArray<UObject*>& Objects)
			: Generator(FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor")
				.CreatePropertyRowGenerator(FPropertyRowGeneratorArgs()))
		{
			Generator->SetObjects(Objects);
			CollectDetailRows(Generator->GetRootTreeNodes(), VisibleRows, FString(), false);
			CollectDetailRows(Generator->GetRootTreeNodes(), AllRows, FString(), true);
		}

		/** 用属性所属结构识别同名字段，避免把通知作者开关与运行快照混为一项。 */
		static bool Matches(const FHodgeGeneratedDetailRow& Row, const UStruct* Owner, FName Name)
		{
			const TSharedPtr<IPropertyHandle> Handle = Row.Node->CreatePropertyHandle();
			const FProperty* Property = Handle.IsValid() ? Handle->GetProperty() : nullptr;
			return Property && Property->GetOwnerStruct() == Owner && Property->GetFName() == Name;
		}

		const FHodgeGeneratedDetailRow* Find(const UStruct* Owner, FName Name) const
		{
			return VisibleRows.FindByPredicate([Owner, Name](const FHodgeGeneratedDetailRow& Row)
			{ return Matches(Row, Owner, Name); });
		}

		int32 Count(const UStruct* Owner, FName Name, bool bIgnoreVisibility = false) const
		{
			const TArray<FHodgeGeneratedDetailRow>& Rows = bIgnoreVisibility ? AllRows : VisibleRows;
			return Rows.FilterByPredicate([Owner, Name](const FHodgeGeneratedDetailRow& Row)
			{ return Matches(Row, Owner, Name); }).Num();
		}
	};

	/** 验证用户能找到原属性且所在分类正确，而非只检查元数据中的分类名。 */
	void ExpectCategory(FAutomationTestBase& Test, const FHodgeGeneratedDetails& Details,
		const UStruct* Owner, FName Property, const TCHAR* Category)
	{
		const FHodgeGeneratedDetailRow* Row = Details.Find(Owner, Property);
		if (Test.TestTrue(FString::Printf(TEXT("%s has an actual visible row"), *Property.ToString()), Row != nullptr))
		{
			Test.TestTrue(FString::Printf(TEXT("%s is under %s"), *Property.ToString(), Category),
				Row->Path.Contains(FString::Printf(TEXT("/%s/"), Category), ESearchCase::CaseSensitive));
		}
	}

	/** 刷新真实整行控件后检查可编辑状态，避免将 Slate 缓存初值当作条件结果。 */
	void ExpectValueEnabled(FAutomationTestBase& Test, const FHodgeGeneratedDetails& Details,
		const UStruct* Owner, FName Property, bool bExpectedEnabled)
	{
		const FHodgeGeneratedDetailRow* Row = Details.Find(Owner, Property);
		if (!Test.TestTrue(FString::Printf(TEXT("%s has a visible row for enabled-state check"), *Property.ToString()), Row != nullptr))
		{
			return;
		}
		const TSharedPtr<IDetailPropertyRow> NativeRow = Row->Node->GetRow();
		if (Test.TestTrue(FString::Printf(TEXT("%s retains a native property row"), *Property.ToString()), NativeRow.IsValid()))
		{
			TSharedPtr<SWidget> NameWidget, ValueWidget;
			FDetailWidgetRow WidgetRow;
			NativeRow->GetDefaultWidgets(NameWidget, ValueWidget, WidgetRow);
			Test.TestEqual(FString::Printf(TEXT("%s retains its native row edit-condition result"), *Property.ToString()),
				WidgetRow.EditConditionValue.Get(true), bExpectedEnabled);
		}
		const FNodeWidgets Widgets = Row->Node->CreateNodeWidgets();
		if (Test.TestTrue(FString::Printf(TEXT("%s creates a native value widget"), *Property.ToString()), Widgets.ValueWidget.IsValid()))
		{
			// CreateNodeWidgets 已绑定整行条件，但 SWidget::IsEnabled 只读缓存；先执行 Slate 的原生属性更新。
			FSlateAttributeMetaData::UpdateAllAttributes(*Widgets.ValueWidget,
				FSlateAttributeMetaData::EInvalidationPermission::AllowInvalidationIfConstructed);
			Test.TestEqual(FString::Printf(TEXT("%s retains its native enabled state"), *Property.ToString()),
				Widgets.ValueWidget->IsEnabled(), bExpectedEnabled);
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeCombatAssetDetailsTest, "Hodge.Editor.CombatDetails.AssetSources",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 新旧布局保持互斥，常规资源、运动安全和兼容配置均能沿真实分类找到。 */
bool FHodgeCombatAssetDetailsTest::RunTest(const FString& Parameters)
{
	TStrongObjectPtr<UHodgeHitReactionProfile> Profile(NewObject<UHodgeHitReactionProfile>());
	Profile->bUseCompactAnimations = true;
	Profile->Animations.AddDefaulted();
	const FHodgeGeneratedDetails CompactDetails({Profile.Get()});
	ExpectCategory(*this, CompactDetails, UHodgeHitReactionProfile::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, Compact), TEXT("HodgeTargetReaction"));
	ExpectCategory(*this, CompactDetails, UHodgeHitReactionProfile::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, ContactEffect), TEXT("HodgeTargetFeedback"));
	ExpectCategory(*this, CompactDetails, UHodgeHitReactionProfile::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, DownedDuration), TEXT("HodgeTargetRecovery"));
	ExpectCategory(*this, CompactDetails, UHodgeHitReactionProfile::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, MaxLaunchHeight), TEXT("HodgeCombatAdvanced"));
	TestEqual(TEXT("Compact layout hides the old animation rows"), CompactDetails.Count(UHodgeHitReactionProfile::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, Animations)), 0);
	TestEqual(TEXT("Compact layout hides root-motion recovery configuration"), CompactDetails.Count(UHodgeHitReactionProfile::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, bAllowRootMotionRecoveryTail)), 0);
	Profile->bUseCompactAnimations = false;
	const FHodgeGeneratedDetails LegacyDetails({Profile.Get()});
	ExpectCategory(*this, LegacyDetails, UHodgeHitReactionProfile::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, Animations), TEXT("HodgeCombatCompatibility"));
	TestEqual(TEXT("Legacy layout hides Compact rows instead of showing competing values"), LegacyDetails.Count(UHodgeHitReactionProfile::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, Compact)), 0);
	TestTrue(TEXT("Legacy layout retains the root-motion recovery control"), LegacyDetails.Find(UHodgeHitReactionProfile::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, bAllowRootMotionRecoveryTail)) != nullptr);

	// 原默认 Reaction 仍是作者入口；旧收益仅移动到兼容分类而不清空原值。
	TStrongObjectPtr<UHodgeAbilityDefinition> Definition(NewObject<UHodgeAbilityDefinition>());
	Definition->DefaultHitConfig.Reaction.InputMode = EHodgeReactionInputMode::PoiseDriven;
	Definition->DarkFlowGainPerPhase = 9.f;
	const FHodgeGeneratedDetails DefinitionDetails({Definition.Get()});
	ExpectCategory(*this, DefinitionDetails, UHodgeAbilityDefinition::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeAbilityDefinition, DefaultHitConfig), TEXT("HodgeAttackIntent"));
	ExpectCategory(*this, DefinitionDetails, UHodgeAbilityDefinition::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeAbilityDefinition, ExecutionConfig), TEXT("HodgeAttackTiming"));
	ExpectCategory(*this, DefinitionDetails, UHodgeAbilityDefinition::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgeAbilityDefinition, DarkFlowGainPerPhase), TEXT("HodgeCombatCompatibility"));
	TestEqual(TEXT("Definition retains its single default Reaction row"), DefinitionDetails.Count(FHodgeHitEffectConfig::StaticStruct(),
		GET_MEMBER_NAME_CHECKED(FHodgeHitEffectConfig, Reaction)), 1);
	TestEqual(TEXT("Reaction runtime launch flag is never generated as an author row"), DefinitionDetails.Count(FHodgeHitReactionConfig::StaticStruct(),
		GET_MEMBER_NAME_CHECKED(FHodgeHitReactionConfig, bCanLaunch), true), 0);
	TestEqual(TEXT("Reorganizing details does not change legacy gain"), Definition->DarkFlowGainPerPhase, 9.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeCombatPoiseDetailsTest, "Hodge.Editor.CombatDetails.PoiseConditions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 削韧关闭不影响分段击飞速度，旧 Break 不暴露，多对象值保持原生差异状态。 */
bool FHodgeCombatPoiseDetailsTest::RunTest(const FString& Parameters)
{
	TStrongObjectPtr<UHodgePawnData> First(NewObject<UHodgePawnData>());
	TStrongObjectPtr<UHodgePawnData> Second(NewObject<UHodgePawnData>());
	First->Poise.bEnabled = Second->Poise.bEnabled = false;
	First->Poise.LaunchHorizontalSpeed = 180.f;
	Second->Poise.LaunchHorizontalSpeed = 240.f;
	const FHodgeGeneratedDetails Details({First.Get(), Second.Get()});
	ExpectCategory(*this, Details, UHodgePawnData::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UHodgePawnData, Poise), TEXT("HodgeTargetReaction"));
	ExpectValueEnabled(*this, Details, FHodgePoiseSettings::StaticStruct(),
		GET_MEMBER_NAME_CHECKED(FHodgePoiseSettings, Window), false);
	ExpectValueEnabled(*this, Details, FHodgePoiseSettings::StaticStruct(),
		GET_MEMBER_NAME_CHECKED(FHodgePoiseSettings, LaunchHorizontalSpeed), true);
	ExpectValueEnabled(*this, Details, FHodgePoiseSettings::StaticStruct(),
		GET_MEMBER_NAME_CHECKED(FHodgePoiseSettings, LaunchVerticalSpeed), true);
	for (FName Name : {GET_MEMBER_NAME_CHECKED(FHodgePoiseSettings, BreakThreshold),
		GET_MEMBER_NAME_CHECKED(FHodgePoiseSettings, BreakProtection), GET_MEMBER_NAME_CHECKED(FHodgePoiseSettings, BreakImpact)})
	{
		TestEqual(FString::Printf(TEXT("Serialized %s has no details row"), *Name.ToString()),
			Details.Count(FHodgePoiseSettings::StaticStruct(), Name, true), 0);
	}
	const FHodgeGeneratedDetailRow* Launch = Details.Find(FHodgePoiseSettings::StaticStruct(),
		GET_MEMBER_NAME_CHECKED(FHodgePoiseSettings, LaunchHorizontalSpeed));
	if (Launch)
	{
		float Value = 0.f;
		TestTrue(TEXT("Different target speeds retain the native multiple-values result"),
			Launch->Node->CreatePropertyHandle()->GetValue(Value) == FPropertyAccess::MultipleValues);
	}
	TestEqual(TEXT("First selected asset retains its speed"), First->Poise.LaunchHorizontalSpeed, 180.f);
	TestEqual(TEXT("Second selected asset retains its speed"), Second->Poise.LaunchHorizontalSpeed, 240.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeCombatNotifyDetailsTest, "Hodge.Editor.CombatDetails.NotifySources",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 通知仅保留分段击飞作者开关，伤害中的旧 Reaction 继续定向隐藏。 */
bool FHodgeCombatNotifyDetailsTest::RunTest(const FString& Parameters)
{
	TStrongObjectPtr<UHodgeAnimNotify_Hit> Point(NewObject<UHodgeAnimNotify_Hit>());
	Point->Hit.bUseDefaultDamage = false;
	Point->Hit.bUseDefaultReaction = false;
	Point->Hit.ReactionOverride.InputMode = EHodgeReactionInputMode::PoiseDriven;
	Point->Hit.Damage.Reaction.ReservedPoiseDamage = 37.f;
	Point->AdditionalHits.Add(Point->Hit);
	TStrongObjectPtr<UHodgeAnimNotifyState_HitCheck> Window(NewObject<UHodgeAnimNotifyState_HitCheck>());
	Window->Hit = Point->Hit;
	for (UObject* Object : {static_cast<UObject*>(Point.Get()), static_cast<UObject*>(Window.Get())})
	{
		const FHodgeGeneratedDetails Details({Object});
		const int32 ExpectedRecords = Object == Point.Get() ? 2 : 1;
		TestEqual(TEXT("Every actual hit record has one visible author Can Launch"), Details.Count(FHodgeAnimHitConfig::StaticStruct(),
			GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, bCanLaunch)), ExpectedRecords);
		TestEqual(TEXT("Every explicit reaction override remains visible"), Details.Count(FHodgeAnimHitConfig::StaticStruct(),
			GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, ReactionOverride)), ExpectedRecords);
		TestEqual(TEXT("Explicit damage continues to expose its actual GE"), Details.Count(FHodgeHitEffectConfig::StaticStruct(),
			GET_MEMBER_NAME_CHECKED(FHodgeHitEffectConfig, DamageEffect)), ExpectedRecords);
		TestEqual(TEXT("Notify damage never regenerates its ignored nested Reaction"), Details.Count(FHodgeHitEffectConfig::StaticStruct(),
			GET_MEMBER_NAME_CHECKED(FHodgeHitEffectConfig, Reaction), true), 0);
		TestEqual(TEXT("No runtime reaction launch flag becomes an author control"), Details.Count(FHodgeHitReactionConfig::StaticStruct(),
			GET_MEMBER_NAME_CHECKED(FHodgeHitReactionConfig, bCanLaunch), true), 0);
	}
	// 切回继承后覆盖结构应隐藏，分段资格仍由同一个通知作者开关提供。
	Window->Hit.bUseDefaultReaction = true;
	Window->Hit.bUseDefaultDamage = true;
	const FHodgeGeneratedDetails InheritedDetails({Window.Get()});
	TestEqual(TEXT("Inherited reaction hides its override row"), InheritedDetails.Count(FHodgeAnimHitConfig::StaticStruct(),
		GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, ReactionOverride)), 0);
	TestEqual(TEXT("Inherited damage hides its override row"), InheritedDetails.Count(FHodgeAnimHitConfig::StaticStruct(),
		GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, Damage)), 0);
	TestEqual(TEXT("Inherited defaults do not hide the segment launch switch"), InheritedDetails.Count(FHodgeAnimHitConfig::StaticStruct(),
		GET_MEMBER_NAME_CHECKED(FHodgeAnimHitConfig, bCanLaunch)), 1);
	TestEqual(TEXT("Hidden primary damage Reaction preserves its serialized value"), Point->Hit.Damage.Reaction.ReservedPoiseDamage, 37.f);
	TestEqual(TEXT("Hidden additional damage Reaction preserves its serialized value"), Point->AdditionalHits[0].Damage.Reaction.ReservedPoiseDamage, 37.f);
	return true;
}
#endif
