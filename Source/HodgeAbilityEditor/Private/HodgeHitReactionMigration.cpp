/** @file HodgeHitReactionMigration.cpp @brief 在明确拒绝有损情况的前提下保存备份并迁移现有 Profile。 */
#include "HodgeHitReactionMigration.h"

#include "Animation/AnimMontage.h"
#include "Animation/Skeleton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "HAL/FileManager.h"
#include "Misc/DataValidation.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"
#include "Modules/ModuleManager.h"
#include "ScopedTransaction.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

namespace
{
	/** 使用完整序列化属性指纹识别预览后编辑；临时编辑器状态不参与比较。 */
	FString MakeFingerprint(const UHodgeHitReactionProfile* Profile, bool bIncludeLayout = true)
	{
		FString Snapshot = Profile->GetPathName() + Profile->GetClass()->GetPathName();
		for (TFieldIterator<FProperty> It(Profile->GetClass()); It; ++It)
		{
			const FProperty* Property = *It;
			if (Property->HasAnyPropertyFlags(CPF_Transient | CPF_DuplicateTransient | CPF_NonPIEDuplicateTransient) ||
				(!bIncludeLayout && (Property->GetFName() == GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, Compact) ||
				Property->GetFName() == GET_MEMBER_NAME_CHECKED(UHodgeHitReactionProfile, bUseCompactAnimations)))) { continue; }
			FString Value;
			Property->ExportText_InContainer(0, Value, Profile, nullptr, const_cast<UHodgeHitReactionProfile*>(Profile), PPF_None);
			Snapshot += TEXT("\n") + Property->GetName() + TEXT("=") + Value;
		}
		const FTCHARToUTF8 Bytes(*Snapshot);
		return FMD5::HashBytes(reinterpret_cast<const uint8*>(Bytes.Get()), Bytes.Length());
	}

	/** 固定已有类型名，诊断不依赖枚举显示语言或资产命名。 */
	FString ImpactName(EHodgeImpactType Type)
	{
		return StaticEnum<EHodgeImpactType>()->GetNameStringByValue(int64(Type));
	}

	/** 仍存在于磁盘或当前会话中的目标都不可覆盖。 */
	bool BackupExists(const FString& PackageName)
	{
		return FindPackage(nullptr, *PackageName) != nullptr || FPackageName::DoesPackageExist(PackageName);
	}

	/** 比较结构属性而非内存填充字节，避免浮点或对象句柄布局引起误判。 */
	bool SameCompact(const FHodgeCompactReactionAnimations& A, const FHodgeCompactReactionAnimations& B)
	{
		return FHodgeCompactReactionAnimations::StaticStruct()->CompareScriptStruct(&A, &B, PPF_None);
	}

	/** 完整校验候选，沿用资产的既有 IsDataValid 契约，不新增平行验证规则。 */
	bool ValidateCandidate(UHodgeHitReactionProfile* Candidate, TArray<FText>& Errors)
	{
		FDataValidationContext Context;
		const EDataValidationResult Result = Candidate->IsDataValid(Context);
		for (const auto& Issue : Context.GetIssues())
		{
			if (Issue.Severity == EMessageSeverity::Error)
			{ Errors.Add(FText::FromString(TEXT("精简候选校验失败：") + Issue.Message.ToString())); }
		}
		if (Result != EDataValidationResult::Valid && Context.GetNumErrors() == 0)
		{ Errors.Add(FText::FromString(TEXT("精简候选未通过完整资产有效性检查。"))); }
		return Errors.IsEmpty();
	}

	/** 比较所有实际消费的阶段数据；地面不会消费空中或起身引用。 */
	bool SamePresentation(const FHodgeHitReactionAnimation& A, const FHodgeHitReactionAnimation& B,
		EHodgeImpactType RequestedType)
	{
		const bool bGround = RequestedType == EHodgeImpactType::HitStun || RequestedType == EHodgeImpactType::Knockback;
		const bool bAir = RequestedType == EHodgeImpactType::Launch || RequestedType == EHodgeImpactType::AirHit ||
			RequestedType == EHodgeImpactType::Slam;
		return A.Montage == B.Montage && A.MotionSource == B.MotionSource &&
			A.SustainSection == B.SustainSection && A.RecoverySection == B.RecoverySection &&
			(!bGround || A.DefaultDuration == B.DefaultDuration) &&
			(!bAir || (A.AirLoopMontage == B.AirLoopMontage && A.LandingMontage == B.LandingMontage &&
				A.AirLoopTransitionTime == B.AirLoopTransitionTime && A.LandingMotionSource == B.LandingMotionSource)) &&
			(bGround || (A.GetUpMontage == B.GetUpMontage && A.GetUpMotionSource == B.GetUpMotionSource));
	}

	/** 输入分支只取四向、左右与奇偶；力度变体已先限制为唯一最低合法阈值。 */
	void CheckResolvedBehavior(const UHodgeHitReactionProfile* Source, const UHodgeHitReactionProfile* Candidate,
		TArray<FText>& Errors)
	{
		for (auto Type : {EHodgeImpactType::HitStun, EHodgeImpactType::Knockback, EHodgeImpactType::Launch,
			EHodgeImpactType::Knockdown, EHodgeImpactType::AirHit, EHodgeImpactType::Slam})
		{
			if (Source->SupportsImpact(Type) != Candidate->SupportsImpact(Type))
			{
				Errors.Add(FText::FromString(FString::Printf(TEXT("%s 支持范围会改变；AirHit 与 Slam 共用 Push，不能隐式补齐缺失类型。"), *ImpactName(Type))));
				continue;
			}
			bool bMismatch = false;
			for (const auto Level : {EHodgeReactionLevel::None, EHodgeReactionLevel::Light, EHodgeReactionLevel::Heavy, EHodgeReactionLevel::Break})
			{
				for (const FVector Direction : {FVector::ForwardVector, -FVector::ForwardVector, FVector::RightVector, -FVector::RightVector})
				{
					for (const auto Side : {EHodgeHitSide::Automatic, EHodgeHitSide::Left, EHodgeHitSide::Right})
					{
						for (int32 Variation : {0, 1})
						{
							FHodgeHitReactionAnimation Before, After;
							const bool bBefore = Source->ResolvePresentation(Type, Direction, 0.1f, Side, Variation, Before, Level);
							const bool bAfter = Candidate->ResolvePresentation(Type, Direction, 0.1f, Side, Variation, After, Level);
							if (bBefore != bAfter || (bBefore && !SamePresentation(Before, After, Type))) { bMismatch = true; break; }
						}
						if (bMismatch) { break; }
					}
					if (bMismatch) { break; }
				}
				if (bMismatch) { break; }
			}
			if (bMismatch)
			{
				Errors.Add(FText::FromString(FString::Printf(TEXT("%s 的方向、左右选片、控制时长或阶段资源解析会改变；Compact 的 Heavy HitStun 改查 Heavy，且 AirHit/Slam 共用 Push。"), *ImpactName(Type))));
			}
		}
	}

	/** 显式保存唯一目标包；失败返回真实错误，不触发保存全部资产。 */
	bool SaveAsset(UHodgeHitReactionProfile* Profile)
	{
		FString Filename;
		if (!FPackageName::TryConvertLongPackageNameToFilename(Profile->GetOutermost()->GetName(), Filename,
			FPackageName::GetAssetPackageExtension()) || !IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true)) { return false; }
		FSavePackageArgs Args;
		Args.TopLevelFlags = RF_Public | RF_Standalone;
		Args.SaveFlags = SAVE_NoError;
		return UPackage::SavePackage(Profile->GetOutermost(), Profile, *Filename, Args);
	}
}

bool FHodgeHitReactionMigrationPlan::CanConvert() const
{
	return Profile.IsValid() && !bAlreadyCompact && Errors.IsEmpty() && !SourceFingerprint.IsEmpty() && !BackupPackageName.IsEmpty();
}

bool FHodgeHitReactionMigration::BuildPlan(UHodgeHitReactionProfile* Profile, const FString& BackupDirectory,
	FHodgeHitReactionMigrationPlan& OutPlan)
{
	OutPlan = FHodgeHitReactionMigrationPlan();
	OutPlan.Profile = Profile;
	if (!Profile)
	{
		OutPlan.Errors.Add(FText::FromString(TEXT("未选择可读取的受击 Profile。")));
		return false;
	}
	OutPlan.SourceFingerprint = MakeFingerprint(Profile);
	OutPlan.bAlreadyCompact = Profile->bUseCompactAnimations;
	auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	TArray<FName> Referencers;
	Registry.GetReferencers(Profile->GetOutermost()->GetFName(), Referencers);
	for (const FName Referencer : Referencers) { OutPlan.Referencers.Add(Referencer.ToString()); }
	OutPlan.Referencers.Sort();
	if (OutPlan.bAlreadyCompact)
	{
		OutPlan.Changes.Add(FText::FromString(TEXT("已使用 Compact：仅报告当前配置，不转换、不创建备份、不保存。")));
		return true;
	}
	FString Directory = BackupDirectory;
	Directory.RemoveFromEnd(TEXT("/"));
	FText PathReason;
	if (!Directory.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidLongPackageName(Directory, false, &PathReason))
	{ OutPlan.Errors.Add(FText::FromString(TEXT("备份目录必须是有效的新 /Game/... 长包目录：") + PathReason.ToString())); }
	else
	{
		OutPlan.BackupPackageName = Directory / Profile->GetName();
		FString BackupFilename;
		if (!FPackageName::TryConvertLongPackageNameToFilename(OutPlan.BackupPackageName, BackupFilename,
			FPackageName::GetAssetPackageExtension()))
		{ OutPlan.Errors.Add(FText::FromString(TEXT("备份路径没有已挂载的磁盘目录。"))); }
		if (OutPlan.BackupPackageName == Profile->GetOutermost()->GetName() || BackupExists(OutPlan.BackupPackageName))
		{ OutPlan.Errors.Add(FText::FromString(TEXT("备份目标与源包相同、已存在或已加载，停止以避免覆盖：") + OutPlan.BackupPackageName)); }
	}
	if (!Profile->IsAsset() || Profile->HasAnyFlags(RF_Transient) || Profile->GetOutermost() == GetTransientPackage() ||
		!FPackageName::IsValidLongPackageName(Profile->GetOutermost()->GetName()))
	{ OutPlan.Errors.Add(FText::FromString(TEXT("目标必须是可持久保存的独立 Profile 资产。"))); }
	if (Profile->GetClass() != UHodgeHitReactionProfile::StaticClass())
	{ OutPlan.Errors.Add(FText::FromString(TEXT("派生 Profile 的额外编辑回调未定义迁移契约，请保留兼容布局。"))); }
	TArray<FText> SourceErrors;
	Profile->Validate(SourceErrors);
	for (const FText& Error : SourceErrors) { OutPlan.Errors.Add(FText::FromString(TEXT("原配置无效：") + Error.ToString())); }
	if (Profile->GroundReplayInterval != 0.f || Profile->bAllowRootMotionRecoveryTail)
	{ OutPlan.Errors.Add(FText::FromString(TEXT("旧重播间隔或根运动恢复尾部不能无损表示为 Compact，保留高级布局。"))); }

	USkeleton* SharedSkeleton = nullptr;
	bool bGetUpAssigned = false, bTransitionAssigned = false;
	for (const auto& Entry : Profile->Animations)
	{
		const FString Name = ImpactName(Entry.Type);
		if (Entry.bAllowWithoutMontage || !Entry.SustainSection.IsNone() || !Entry.RecoverySection.IsNone() ||
			Entry.MotionSource != EHodgeReactionMotionSource::Impact || Entry.LandingMotionSource != EHodgeReactionMotionSource::Impact ||
			Entry.GetUpMotionSource != EHodgeReactionMotionSource::Impact)
		{ OutPlan.Errors.Add(FText::FromString(Name + TEXT(" 含无动画许可、Sustain/Recovery 或根运动阶段来源，Compact 无法无损表示。"))); }
		for (const UAnimMontage* Montage : {Entry.Montage.Get(), Entry.AirLoopMontage.Get(), Entry.LandingMontage.Get(), Entry.GetUpMontage.Get()})
		{
			if (!Montage) { continue; }
			if (!Montage->GetSkeleton() || Montage->HasRootMotion() || !FMath::IsFinite(Montage->GetPlayLength()) || Montage->GetPlayLength() <= 0.f)
			{ OutPlan.Errors.Add(FText::FromString(TEXT("缺失已知骨架、片长无效或仍有根运动：") + Montage->GetPathName())); }
			else if (SharedSkeleton && SharedSkeleton != Montage->GetSkeleton())
			{ OutPlan.Errors.Add(FText::FromString(TEXT("阶段资源使用不同骨架，迁移不会猜测兼容或重定向：") + Montage->GetPathName())); }
			else { SharedSkeleton = Montage->GetSkeleton(); }
		}
		const bool bGround = Entry.Type == EHodgeImpactType::HitStun || Entry.Type == EHodgeImpactType::Knockback;
		if (bGround)
		{
			auto& Pair = Entry.Type == EHodgeImpactType::HitStun ? OutPlan.Compact.Light : OutPlan.Compact.Heavy;
			Pair.Left = Entry.Montage;
			Pair.Right = Entry.Montage;
			Pair.ControlDuration = Entry.DefaultDuration;
			TSet<EHodgeHitPresentationDirection> Directions;
			for (const auto& Variant : Entry.Variants)
			{
				if ((Variant.Direction != EHodgeHitPresentationDirection::Left && Variant.Direction != EHodgeHitPresentationDirection::Right) ||
					Variant.MinimumStrength != 0.1f || Directions.Contains(Variant.Direction))
				{ OutPlan.Errors.Add(FText::FromString(Name + TEXT(" 存在方向或力度矩阵歧义，仅支持每侧一个 0.1 最低阈值动作。"))); continue; }
				Directions.Add(Variant.Direction);
				if (Variant.Direction == EHodgeHitPresentationDirection::Left) { Pair.Left = Variant.Montage; }
				else { Pair.Right = Variant.Montage; }
				if (Variant.Montage)
				{
					if (!Variant.Montage->GetSkeleton() || Variant.Montage->GetSkeleton() != SharedSkeleton || Variant.Montage->HasRootMotion())
					{ OutPlan.Errors.Add(FText::FromString(TEXT("方向动作缺骨架、骨架不一致或含根运动：") + Variant.Montage->GetPathName())); }
				}
			}
		}
		else
		{
			if (!Entry.Variants.IsEmpty())
			{ OutPlan.Errors.Add(FText::FromString(Name + TEXT(" 包含方向/力度变体，精简空中和倒地布局无法表示。"))); }
			if (bGetUpAssigned && OutPlan.Compact.GetUp != Entry.GetUpMontage)
			{ OutPlan.Errors.Add(FText::FromString(Name + TEXT(" 的 GetUp 与其他阶段不一致，不能合并为公共起身。"))); }
			else { OutPlan.Compact.GetUp = Entry.GetUpMontage; bGetUpAssigned = true; }
			if (Entry.Type == EHodgeImpactType::Knockdown) { OutPlan.Compact.Downed = Entry.Montage; }
			else
			{
				auto& Family = Entry.Type == EHodgeImpactType::Launch ? OutPlan.Compact.Fly : OutPlan.Compact.Push;
				Family.Start = Entry.Montage;
				Family.Loop = Entry.AirLoopMontage;
				Family.Landing = Entry.LandingMontage;
				if (bTransitionAssigned && OutPlan.Compact.AirLoopTransitionTime != Entry.AirLoopTransitionTime)
				{ OutPlan.Errors.Add(FText::FromString(Name + TEXT(" 的空中转段时间不一致，不能合并为公共时机。"))); }
				else { OutPlan.Compact.AirLoopTransitionTime = Entry.AirLoopTransitionTime; bTransitionAssigned = true; }
			}
		}
	}
	const FHodgeCompactReactionAnimations Empty;
	if (!SameCompact(Profile->Compact, Empty) && !SameCompact(Profile->Compact, OutPlan.Compact))
	{ OutPlan.Errors.Add(FText::FromString(TEXT("隐藏的 Compact 已有手调且与映射不同，停止以避免覆盖。"))); }
	if (!OutPlan.Errors.IsEmpty()) { return false; }
	TStrongObjectPtr<UHodgeHitReactionProfile> Candidate(DuplicateObject<UHodgeHitReactionProfile>(Profile, GetTransientPackage(),
		MakeUniqueObjectName(GetTransientPackage(), Profile->GetClass(), TEXT("HodgeHitReactionMigrationCandidate"))));
	Candidate->ClearFlags(RF_Public | RF_Standalone | RF_Transactional);
	Candidate->SetFlags(RF_Transient);
	Candidate->bUseCompactAnimations = true;
	Candidate->Compact = OutPlan.Compact;
	ValidateCandidate(Candidate.Get(), OutPlan.Errors);
	CheckResolvedBehavior(Profile, Candidate.Get(), OutPlan.Errors);
	if (!OutPlan.Errors.IsEmpty()) { return false; }
	for (TFieldIterator<FProperty> It(FHodgeCompactReactionAnimations::StaticStruct()); It; ++It)
	{
		FString Before, After;
		It->ExportText_InContainer(0, Before, &Profile->Compact, nullptr, Profile, PPF_None);
		It->ExportText_InContainer(0, After, &OutPlan.Compact, nullptr, Profile, PPF_None);
		if (Before != After)
		{ OutPlan.Changes.Add(FText::FromString(TEXT("Compact.") + It->GetName() + TEXT("：") + Before + TEXT(" → ") + After)); }
	}
	OutPlan.Changes.Add(FText::FromString(TEXT("bUseCompactAnimations：false → true；旧 Animations 及其余字段保持原值。")));
	OutPlan.Changes.Add(FText::FromString(TEXT("先保存备份 ") + OutPlan.BackupPackageName + TEXT("，成功后自动保存目标资产。")));
	return true;
}

bool FHodgeHitReactionMigration::Execute(const FHodgeHitReactionMigrationPlan& Plan, TArray<FText>& OutMessages)
{
	OutMessages.Reset();
	UHodgeHitReactionProfile* Profile = Plan.Profile.Get();
	if (!Profile)
	{ OutMessages.Add(FText::FromString(TEXT("预览目标已失效，请重新检查。"))); return false; }
	if (Profile->bUseCompactAnimations)
	{ OutMessages.Add(FText::FromString(TEXT("目标已使用 Compact，未创建备份、未修改、未保存。"))); return true; }
	if (!Plan.CanConvert() || Plan.SourceFingerprint != MakeFingerprint(Profile))
	{ OutMessages.Add(FText::FromString(TEXT("配置已变化或预览不可转换，请重新检查；目标未修改。"))); return false; }
	if (!FPackageName::DoesPackageExist(Profile->GetOutermost()->GetName()))
	{ OutMessages.Add(FText::FromString(TEXT("源资产尚未保存为磁盘包，请先保存再重新检查；目标未修改。"))); return false; }
	FHodgeHitReactionMigrationPlan Fresh;
	if (!BuildPlan(Profile, FPackageName::GetLongPackagePath(Plan.BackupPackageName), Fresh) ||
		Fresh.BackupPackageName != Plan.BackupPackageName || Fresh.SourceFingerprint != Plan.SourceFingerprint || !SameCompact(Fresh.Compact, Plan.Compact))
	{
		OutMessages = Fresh.Errors;
		OutMessages.Add(FText::FromString(TEXT("资源、备份目标或转换计划与预览不一致，请重新检查；目标未修改。")));
		return false;
	}
	if (!GEditor || GEditor->PlayWorld || GIsTransacting || !GEditor->CanTransact())
	{ OutMessages.Add(FText::FromString(TEXT("需要空闲的编辑器和有效事务环境，停止本资产转换。"))); return false; }
	TStrongObjectPtr<UHodgeHitReactionProfile> SourceGuard(Profile);
	// 先创建独立备份；保存失败时不开始源资产事务，也不清理可能包含诊断线索的副本。
	UPackage* BackupPackage = CreatePackage(*Plan.BackupPackageName);
	TStrongObjectPtr<UHodgeHitReactionProfile> Backup(DuplicateObject<UHodgeHitReactionProfile>(Profile, BackupPackage, Profile->GetFName()));
	if (!Backup.IsValid())
	{ OutMessages.Add(FText::FromString(TEXT("创建备份失败；目标未修改。"))); return false; }
	Backup->ClearFlags(RF_Transient);
	Backup->SetFlags(RF_Public | RF_Standalone);
	FAssetRegistryModule::AssetCreated(Backup.Get());
	BackupPackage->MarkPackageDirty();
	if (!SaveAsset(Backup.Get()))
	{ OutMessages.Add(FText::FromString(TEXT("备份保存失败，目标未修改；保留失败副本供检查：") + Plan.BackupPackageName)); return false; }
	OutMessages.Add(FText::FromString(TEXT("备份已保存：") + Plan.BackupPackageName));
	if (MakeFingerprint(Profile) != Plan.SourceFingerprint)
	{ OutMessages.Add(FText::FromString(TEXT("备份期间源配置发生变化，未转换目标，请重新检查。"))); return false; }
	const FHodgeCompactReactionAnimations OldCompact = Profile->Compact;
	const bool bWasDirty = Profile->GetOutermost()->IsDirty();
	const FString FrozenFingerprint = MakeFingerprint(Profile, false);
	FScopedTransaction Transaction(NSLOCTEXT("HodgeAbilityEditor", "MigrateHitReaction", "迁移受击精简布局"));
	Profile->SetFlags(RF_Transactional);
	Profile->Modify();
	Profile->PreEditChange(nullptr);
	Profile->Compact = Fresh.Compact;
	Profile->bUseCompactAnimations = true;
	Profile->PostEditChange();
	TArray<FText> ValidationErrors;
	const bool bValid = ValidateCandidate(Profile, ValidationErrors) && MakeFingerprint(Profile, false) == FrozenFingerprint;
	if (!bValid || !SaveAsset(Profile))
	{
		// 取消事务只丢弃撤销记录，必须先恢复本次实际写入的两个字段。
		Profile->PreEditChange(nullptr);
		Profile->Compact = OldCompact;
		Profile->bUseCompactAnimations = false;
		Profile->PostEditChange();
		Profile->GetOutermost()->SetDirtyFlag(bWasDirty);
		Transaction.Cancel();
		OutMessages.Append(ValidationErrors);
		OutMessages.Add(FText::FromString(TEXT("转换校验或目标保存失败，内存中已恢复旧布局；磁盘状态需核对，可使用已保存备份恢复：") + Plan.BackupPackageName));
		return false;
	}
	OutMessages.Add(FText::FromString(TEXT("转换并保存成功；旧 Animations 和冻结字段保持不变：") + Profile->GetPathName()));
	return true;
}
