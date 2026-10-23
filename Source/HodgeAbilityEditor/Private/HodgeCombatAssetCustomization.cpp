/** @file HodgeCombatAssetCustomization.cpp @brief 原属性负责编辑，检查与显式迁移各自输出结果。 */
#include "HodgeCombatAssetCustomization.h"

#include "Data/HodgeAbilityDefinition.h"
#include "Data/HodgeHitReactionProfile.h"
#include "Data/HodgePawnData.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "HodgeHitReactionMigration.h"
#include "Misc/DataValidation.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "HodgeCombatAssetCustomization"

namespace
{
	/** 一个定制实例只持有弱资产引用，关闭面板后回调不能继续写资产。 */
	class FHodgeCombatAssetCustomization : public IDetailCustomization
	{
	public:
		enum class EKind { Definition, PawnData, Profile };
		explicit FHodgeCombatAssetCustomization(EKind InKind);
		virtual void CustomizeDetails(IDetailLayoutBuilder& Builder) override;

	private:
		/** 原生行保留 EditCondition、Undo、数值约束及多选行为。 */
		void MoveProperties(IDetailLayoutBuilder& Builder, FName CategoryName, const FText& Label,
			int32 Order, std::initializer_list<FName> Names, bool bCollapsed = false);
		void AddTools(IDetailLayoutBuilder& Builder);
		FReply CheckAssets();
		FReply PreviewMigration();
		FReply ConvertProfile();
		void ChangeBackupDirectory(const FText& Value);
		bool HasSingleProfile() const;
		bool CanConvert() const;
		FText GetCheckResult() const;
		FText GetMigrationResult() const;

		EKind Kind;
		TArray<TWeakObjectPtr<UObject>> Objects;
		FString BackupDirectory;
		FText CheckResult;
		FText MigrationResult;
		TUniquePtr<FHodgeHitReactionMigrationPlan> Plan;
	};

	FHodgeCombatAssetCustomization::FHodgeCombatAssetCustomization(EKind InKind) : Kind(InKind)
	{
	}

	void FHodgeCombatAssetCustomization::MoveProperties(IDetailLayoutBuilder& Builder, FName CategoryName,
		const FText& Label, int32 Order, std::initializer_list<FName> Names, bool bCollapsed)
	{
		IDetailCategoryBuilder& Category = Builder.EditCategory(CategoryName, Label);
		Category.SetSortOrder(Order);
		Category.InitiallyCollapsed(bCollapsed);
		for (const FName Name : Names)
		{
			const TSharedRef<IPropertyHandle> Handle = Builder.GetProperty(Name);
			if (Handle->IsValidHandle())
			{
				Category.AddProperty(Handle, EPropertyLocation::Common);
			}
		}
	}

	void FHodgeCombatAssetCustomization::CustomizeDetails(IDetailLayoutBuilder& Builder)
	{
		Builder.GetObjectsBeingCustomized(Objects);
		if (Kind == EKind::Definition)
		{
			MoveProperties(Builder, TEXT("HodgeAttackIntent"), LOCTEXT("AttackIntent", "1. 攻击分段 · 命中意图"), 0, {
				TEXT("AbilityTag"), TEXT("AbilityClass"), TEXT("ExecutionRoute"), TEXT("ExecutionBodyTag"),
				TEXT("DefaultHitConfig"), TEXT("bAllowHitRecoveryAttack")});
			MoveProperties(Builder, TEXT("HodgeAttackTiming"), LOCTEXT("AttackTiming", "2. 动画时机 · Montage"), 1, {
				TEXT("ExecutionConfig")});
			MoveProperties(Builder, TEXT("HodgeCombatCompatibility"), LOCTEXT("Compatibility", "兼容配置与迁移"), 90, {
				TEXT("DarkFlowGainPerPhase"), TEXT("DarkFlowGainOnCast"), TEXT("bEnhancedNormalAttack"),
				TEXT("ResonanceEnergyGainPerPhase"), TEXT("ResonanceEnergyGainEffect")}, true);
		}
		else if (Kind == EKind::PawnData)
		{
			MoveProperties(Builder, TEXT("HodgeTargetReaction"), LOCTEXT("PawnReaction", "目标 · 基础与受击规则"), 0, {
				TEXT("PawnClass"), TEXT("StatProfile"), TEXT("AbilitySets"), TEXT("HitReactionProfile"), TEXT("Poise")});
		}
		else
		{
			MoveProperties(Builder, TEXT("HodgeTargetReaction"), LOCTEXT("ProfileReaction", "1. 目标 · 受击动画"), 0, {
				TEXT("bUseCompactAnimations"), TEXT("Compact")});
			MoveProperties(Builder, TEXT("HodgeTargetFeedback"), LOCTEXT("TargetFeedback", "2. 目标 · 普通接触"), 1, {
				TEXT("LightFeedbackMontage"), TEXT("DirectionalLightFeedback"), TEXT("ContactEffect"),
				TEXT("ContactSound"), TEXT("LightFeedbackCue")});
			MoveProperties(Builder, TEXT("HodgeTargetRecovery"), LOCTEXT("TargetRecovery", "3. 目标 · 倒地起身与恢复"), 2, {
				TEXT("DownedDuration"), TEXT("bAutoGetUp"), TEXT("ReplacementBlendOut"), TEXT("bEnableGroundRecovery"),
				TEXT("DodgeRecoveryDelay"), TEXT("AttackRecoveryDelay"), TEXT("MovementRecoveryDelay")});
			MoveProperties(Builder, TEXT("HodgeCombatAdvanced"), LOCTEXT("Advanced", "高级 · 安全、运动与恢复尾部"), 80, {
				TEXT("MaxControlDuration"), TEXT("MaxAirborneDuration"), TEXT("MaxLaunchHeight"), TEXT("MaxLaunchSpeed"),
				TEXT("MaxAirHits"), TEXT("FallbackToHitStun"), TEXT("InterruptibleAbilityTags"),
				TEXT("bSustainGroundHitStun"), TEXT("bAllowNaturalRecoveryTail"), TEXT("bAllowRootMotionRecoveryTail"),
				TEXT("GroundReplayInterval"), TEXT("KnockbackStrengthCurve")}, true);
			MoveProperties(Builder, TEXT("HodgeCombatCompatibility"), LOCTEXT("Compatibility", "兼容配置与迁移"), 90, {
				TEXT("Animations")}, true);
		}
		AddTools(Builder);
	}

	void FHodgeCombatAssetCustomization::AddTools(IDetailLayoutBuilder& Builder)
	{
		IDetailCategoryBuilder& Checks = Builder.EditCategory(TEXT("HodgeCombatChecks"), LOCTEXT("Checks", "检查 · 只读验证"));
		Checks.SetSortOrder(100);
		Checks.AddCustomRow(LOCTEXT("CheckFilter", "检查 Validate"))
		.WholeRowContent()[SNew(SButton).Text(LOCTEXT("Check", "检查当前资产（不保存）"))
			.OnClicked(this, &FHodgeCombatAssetCustomization::CheckAssets)];
		Checks.AddCustomRow(LOCTEXT("CheckResults", "检查结果"))
		.WholeRowContent()[SNew(STextBlock).AutoWrapText(true)
			.Text(this, &FHodgeCombatAssetCustomization::GetCheckResult)];
		if (Kind != EKind::Profile) { return; }

		IDetailCategoryBuilder& Migration = Builder.EditCategory(TEXT("HodgeCombatCompatibility"));
		Migration.AddCustomRow(LOCTEXT("MigrationHelpFilter", "迁移 Compact 兼容"))
		.WholeRowContent()[SNew(STextBlock).AutoWrapText(true).Text(LOCTEXT("MigrationHelp",
			"仅转换当前选中的一份 Profile。先预览，再备份并转换；无损映射失败即停止。攻击模式与通知不会自动修改。"))];
		Migration.AddCustomRow(LOCTEXT("BackupFilter", "备份目录 Backup"))
		.NameContent()[SNew(STextBlock).Text(LOCTEXT("Backup", "新备份目录"))]
		.ValueContent().MinDesiredWidth(320)[SNew(SEditableTextBox)
			.HintText(LOCTEXT("BackupHint", "/Game/.../Backups/新批次"))
			.OnTextChanged(this, &FHodgeCombatAssetCustomization::ChangeBackupDirectory)];
		Migration.AddCustomRow(LOCTEXT("MigrationFilter", "预览 转换 Compact"))
		.WholeRowContent()[SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()[SNew(SButton).Text(LOCTEXT("Preview", "只读迁移预览"))
				.IsEnabled(this, &FHodgeCombatAssetCustomization::HasSingleProfile)
				.OnClicked(this, &FHodgeCombatAssetCustomization::PreviewMigration)]
			+ SHorizontalBox::Slot().AutoWidth().Padding(8, 0)[SNew(SButton).Text(LOCTEXT("Convert", "备份并转换、保存当前目标"))
				.IsEnabled(this, &FHodgeCombatAssetCustomization::CanConvert)
				.OnClicked(this, &FHodgeCombatAssetCustomization::ConvertProfile)]];
		Migration.AddCustomRow(LOCTEXT("MigrationResults", "迁移读取结果 引用者 差异 备份目标"))
		.WholeRowContent()[SNew(STextBlock).AutoWrapText(true)
			.Text(this, &FHodgeCombatAssetCustomization::GetMigrationResult)];
	}

	FReply FHodgeCombatAssetCustomization::CheckAssets()
	{
		TArray<FString> Lines;
		for (const TWeakObjectPtr<UObject>& Weak : Objects)
		{
			const UObject* Object = Weak.Get();
			if (!Object) { Lines.Add(TEXT("资产已失效，请重新打开面板。")); continue; }
			FDataValidationContext Context(false, EDataValidationUsecase::Manual, {});
			const EDataValidationResult Result = Object->IsDataValid(Context);
			const TCHAR* Status = Result == EDataValidationResult::Valid ? TEXT("配置检查通过") :
				Result == EDataValidationResult::Invalid ? TEXT("配置检查失败") : TEXT("现有校验链未返回有效性结论");
			Lines.Add(Object->GetPathName() + TEXT("：") + Status);
			for (const FDataValidationContext::FIssue& Issue : Context.GetIssues())
			{
				Lines.Add((Issue.Severity == EMessageSeverity::Error ? TEXT("错误：") : TEXT("提示：")) + Issue.Message.ToString());
			}
		}
		Lines.Add(TEXT("检查未保存资产；此结果不代表蓝图编译、PIE 或联机验证。"));
		CheckResult = FText::FromString(FString::Join(Lines, TEXT("\n")));
		return FReply::Handled();
	}

	FReply FHodgeCombatAssetCustomization::PreviewMigration()
	{
		Plan = MakeUnique<FHodgeHitReactionMigrationPlan>();
		UHodgeHitReactionProfile* Profile = HasSingleProfile() ? Cast<UHodgeHitReactionProfile>(Objects[0].Get()) : nullptr;
		FHodgeHitReactionMigration::BuildPlan(Profile, BackupDirectory, *Plan);
		TArray<FString> Lines;
		Lines.Add(TEXT("读取目标：") + GetPathNameSafe(Profile));
		Lines.Add(TEXT("当前布局：") + FString(Profile && Profile->bUseCompactAnimations ? TEXT("Compact") : TEXT("Animations")));
		Lines.Add(TEXT("备份目标：") + Plan->BackupPackageName);
		Lines.Add(TEXT("引用者（资产注册表中的包引用）："));
		Lines.Append(Plan->Referencers);
		Lines.Add(TEXT("差异："));
		for (const FText& Change : Plan->Changes) { Lines.Add(Change.ToString()); }
		for (const FText& Error : Plan->Errors) { Lines.Add(TEXT("停止：") + Error.ToString()); }
		if (Plan->CanConvert()) { Lines.Add(TEXT("预览通过，尚未写入；转换将先保存备份，再自动保存当前目标。")); }
		MigrationResult = FText::FromString(FString::Join(Lines, TEXT("\n")));
		return FReply::Handled();
	}

	FReply FHodgeCombatAssetCustomization::ConvertProfile()
	{
		if (!CanConvert()) { return FReply::Handled(); }
		TArray<FText> Messages;
		const bool bSuccess = FHodgeHitReactionMigration::Execute(*Plan, Messages);
		TArray<FString> Lines{ bSuccess ? TEXT("迁移操作结果：") : TEXT("转换未完成，请处理以下结果后重新预览。") };
		for (const FText& Message : Messages) { Lines.Add(Message.ToString()); }
		MigrationResult = FText::FromString(FString::Join(Lines, TEXT("\n")));
		Plan.Reset();
		return FReply::Handled();
	}

	void FHodgeCombatAssetCustomization::ChangeBackupDirectory(const FText& Value)
	{
		BackupDirectory = Value.ToString().TrimStartAndEnd();
		Plan.Reset();
		MigrationResult = LOCTEXT("PreviewRequired", "备份目录已改变，请重新预览。");
	}

	bool FHodgeCombatAssetCustomization::HasSingleProfile() const
	{
		return Objects.Num() == 1 && Objects[0].IsValid() && Objects[0]->IsA<UHodgeHitReactionProfile>();
	}

	bool FHodgeCombatAssetCustomization::CanConvert() const
	{
		return HasSingleProfile() && Plan && Plan->CanConvert() && Plan->Profile.Get() == Objects[0].Get();
	}

	FText FHodgeCombatAssetCustomization::GetCheckResult() const { return CheckResult; }
	FText FHodgeCombatAssetCustomization::GetMigrationResult() const { return MigrationResult; }
}

namespace HodgeCombatDetails
{
	TSharedRef<IDetailCustomization> MakeAbilityDefinitionCustomization()
	{
		return MakeShared<FHodgeCombatAssetCustomization>(FHodgeCombatAssetCustomization::EKind::Definition);
	}
	TSharedRef<IDetailCustomization> MakePawnDataCustomization()
	{
		return MakeShared<FHodgeCombatAssetCustomization>(FHodgeCombatAssetCustomization::EKind::PawnData);
	}
	TSharedRef<IDetailCustomization> MakeHitReactionProfileCustomization()
	{
		return MakeShared<FHodgeCombatAssetCustomization>(FHodgeCombatAssetCustomization::EKind::Profile);
	}
}

#undef LOCTEXT_NAMESPACE
