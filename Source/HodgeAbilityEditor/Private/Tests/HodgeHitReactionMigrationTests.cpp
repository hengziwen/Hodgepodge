/** @file HodgeHitReactionMigrationTests.cpp @brief 验证无损映射、拒绝边界、预览过期与真实备份保存。 */
#if WITH_DEV_AUTOMATION_TESTS
#include "HodgeHitReactionMigration.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/Skeleton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "UObject/Linker.h"
#include "UObject/LinkerInstancingContext.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectHash.h"
#include "UObject/UnrealType.h"

namespace
{
	/** 每例只创建带唯一身份的测试包，退出时仅删除本例准确列出的文件。 */
	struct FMigrationFixture
	{
		FString Root = TEXT("/Game/Tests/Combat/MigrationAutomation_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
		FString SourcePackageName = Root / TEXT("DA_Profile");
		FString ResourcePackageName = Root / TEXT("Resources");
		FString BackupDirectory = Root / TEXT("Backup");
		TStrongObjectPtr<UHodgeHitReactionProfile> Profile;
		TStrongObjectPtr<UPackage> ResourcePackage;
		TArray<FString> ReadbackPackageNames;
		USkeleton* Skeleton = nullptr;

		FMigrationFixture()
		{
			Profile.Reset(NewObject<UHodgeHitReactionProfile>(CreatePackage(*SourcePackageName), TEXT("DA_Profile"), RF_Public | RF_Standalone | RF_Transactional));
			ResourcePackage.Reset(CreatePackage(*ResourcePackageName));
			Skeleton = NewObject<USkeleton>(ResourcePackage.Get(), TEXT("TestSkeleton"), RF_Public | RF_Standalone);
			auto* Left = MakeMontage(TEXT("Left"));
			auto* Right = MakeMontage(TEXT("Right"));
			auto* AirStart = MakeMontage(TEXT("AirStart"));
			auto* AirLoop = MakeMontage(TEXT("AirLoop"));
			auto* AirLanding = MakeMontage(TEXT("AirLanding"));
			auto* GetUp = MakeMontage(TEXT("GetUp"));
			for (auto Type : {EHodgeImpactType::HitStun, EHodgeImpactType::Knockback, EHodgeImpactType::Launch,
				EHodgeImpactType::Knockdown, EHodgeImpactType::AirHit, EHodgeImpactType::Slam})
			{
				FHodgeHitReactionAnimation Entry;
				Entry.Type = Type;
				if (Type == EHodgeImpactType::HitStun || Type == EHodgeImpactType::Knockback)
				{
					Entry.Montage = Right;
					Entry.DefaultDuration = 0.46f;
					Entry.bUseLateralVariants = true;
					FHodgeHitReactionVariant L, R;
					L.Direction = EHodgeHitPresentationDirection::Left; L.Montage = Left;
					R.Direction = EHodgeHitPresentationDirection::Right; R.Montage = Right;
					Entry.Variants = {L, R};
				}
				else
				{
					Entry.GetUpMontage = GetUp;
					if (Type == EHodgeImpactType::Knockdown) { Entry.Montage = MakeMontage(TEXT("Downed")); }
					else
					{
						Entry.Montage = AirStart;
						Entry.AirLoopMontage = AirLoop;
						Entry.LandingMontage = AirLanding;
						Entry.AirLoopTransitionTime = 0.23f;
					}
				}
				Profile->Animations.Add(Entry);
			}
			Profile->DownedDuration = 1.7f;
			Profile->MaxAirHits = 9;
			Profile->bEnableGroundRecovery = true;
			Profile->DodgeRecoveryDelay = 0.17f;
			FAssetRegistryModule::AssetCreated(Profile.Get());
			FAssetRegistryModule::AssetCreated(Skeleton);
		}

		/** 合成独立的已知骨架原地片段，不读取或修改项目已有动画。 */
		UAnimMontage* MakeMontage(FName Name)
		{
			auto* Montage = NewObject<UAnimMontage>(ResourcePackage.Get(), Name, RF_Public | RF_Standalone);
			Montage->SetSkeleton(Skeleton);
			// 仅为合成测试素材设置长度，不访问已弃用的 SequenceLength 成员。
			auto* LengthProperty = FindFProperty<FFloatProperty>(UAnimSequenceBase::StaticClass(), TEXT("SequenceLength"));
			check(LengthProperty);
			LengthProperty->SetPropertyValue_InContainer(Montage, 1.f);
			FAssetRegistryModule::AssetCreated(Montage);
			return Montage;
		}

		/** 正式执行要求源包已保存；测试只保存本例新建包。 */
		bool SaveSource()
		{
			const FString ResourceFilename = FPackageName::LongPackageNameToFilename(ResourcePackageName, FPackageName::GetAssetPackageExtension());
			FSavePackageArgs Args;
			Args.TopLevelFlags = RF_Public | RF_Standalone;
			Args.SaveFlags = SAVE_NoError;
			if (!IFileManager::Get().MakeDirectory(*FPaths::GetPath(ResourceFilename), true) ||
				!UPackage::SavePackage(ResourcePackage.Get(), Skeleton, *ResourceFilename, Args)) { return false; }
			const FString Filename = FPackageName::LongPackageNameToFilename(SourcePackageName, FPackageName::GetAssetPackageExtension());
			if (!IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true)) { return false; }
			return UPackage::SavePackage(Profile->GetOutermost(), Profile.Get(), *Filename, Args);
		}

		/** 以新包身份读取原文件，不能从已经加载的源对象返回缓存值。 */
		UHodgeHitReactionProfile* ReadSavedProfile(const FString& SavedPackageName, const FString& InstanceName)
		{
			const FString NewPackageName = Root / InstanceName;
			if (FindPackage(nullptr, *NewPackageName) || !FPackageName::DoesPackageExist(SavedPackageName)) { return nullptr; }
			ReadbackPackageNames.Add(NewPackageName);
			FLinkerInstancingContext Context;
			Context.AddPackageMapping(*SavedPackageName, *NewPackageName);
			UPackage* Loaded = LoadPackage(CreatePackage(*NewPackageName), *SavedPackageName, LOAD_None, nullptr, &Context);
			return Loaded ? FindObject<UHodgeHitReactionProfile>(Loaded, TEXT("DA_Profile")) : nullptr;
		}

		/** 先释放冷读句柄和测试资产的编辑器保活，再删除本例明确列出的磁盘文件。 */
		void ReleasePackage(const FString& PackageName)
		{
			if (UPackage* Package = FindPackage(nullptr, *PackageName))
			{
				ResetLoaders(Package);
				TArray<UObject*> Assets;
				GetObjectsWithOuter(Package, Assets, false);
				for (UObject* Asset : Assets)
				{
					if (Asset->IsAsset()) { FAssetRegistryModule::AssetDeleted(Asset); }
					Asset->ClearFlags(RF_Public | RF_Standalone);
					Asset->SetFlags(RF_Transient);
				}
				Package->SetDirtyFlag(false);
			}
		}

		~FMigrationFixture()
		{
			for (const FString& PackageName : ReadbackPackageNames) { ReleasePackage(PackageName); }
			// 清理限于随机测试根下三个已知包，不递归删除目录，也不触及用户内容。
			for (const FString PackageName : {SourcePackageName, BackupDirectory / TEXT("DA_Profile"), ResourcePackageName})
			{
				ReleasePackage(PackageName);
				const FString Filename = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
				if (PackageName.StartsWith(Root + TEXT("/")) && IFileManager::Get().FileExists(*Filename))
				{ IFileManager::Get().Delete(*Filename, false, true); }
			}
		}
	};

	/** 断言诊断来自指定拒绝原因，而非其他错误碰巧阻止转换。 */
	bool HasMessage(const TArray<FText>& Messages, const FString& Needle)
	{
		return Messages.ContainsByPredicate([&Needle](const FText& Message) { return Message.ToString().Contains(Needle); });
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMigrationPreviewTest, "Hodge.Editor.HitReactionMigration.Preview",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 同等可表达的完整原地家族能够映射，读取预览不得切换布局或创建备份。 */
bool FHodgeMigrationPreviewTest::RunTest(const FString&)
{
	FMigrationFixture Fixture;
	const auto Before = Fixture.Profile->Animations;
	FHodgeHitReactionMigrationPlan Plan;
	TestTrue(TEXT("A fully equivalent in-place family can be previewed"), FHodgeHitReactionMigration::BuildPlan(Fixture.Profile.Get(), Fixture.BackupDirectory, Plan));
	for (const FText& Error : Plan.Errors) { AddError(Error.ToString()); }
	TestTrue(TEXT("Valid preview enables conversion"), Plan.CanConvert());
	TestEqual(TEXT("Light duration is preserved"), Plan.Compact.Light.ControlDuration, 0.46f);
	TestEqual(TEXT("Shared airborne timing is preserved"), Plan.Compact.AirLoopTransitionTime, 0.23f);
	TestEqual(TEXT("Backup target is concrete"), Plan.BackupPackageName, Fixture.BackupDirectory / TEXT("DA_Profile"));
	TestFalse(TEXT("Preview does not switch the original layout"), Fixture.Profile->bUseCompactAnimations);
	TestTrue(TEXT("Preview preserves each original entry"), FHodgeHitReactionAnimation::StaticStruct()->CompareScriptStruct(&Before[0], &Fixture.Profile->Animations[0], PPF_None));
	TestNull(TEXT("Preview does not create the backup package"), FindPackage(nullptr, *Plan.BackupPackageName));
	TestTrue(TEXT("Preview reports actual field differences"), !Plan.Changes.IsEmpty());
	TArray<FText> Messages;
	TestFalse(TEXT("An unsaved source cannot execute"), FHodgeHitReactionMigration::Execute(Plan, Messages));
	TestTrue(TEXT("Unsaved-source refusal is explicit"), HasMessage(Messages, TEXT("源资产尚未保存")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMigrationRejectionTest, "Hodge.Editor.HitReactionMigration.Refusals",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 有损行为、未知骨架及已有手调都必须停止，不借助默认值掩盖差异。 */
bool FHodgeMigrationRejectionTest::RunTest(const FString&)
{
	auto Reject = [this](const TCHAR* Label, TFunction<void(FMigrationFixture&)> Change, const TCHAR* Reason)
	{
		FMigrationFixture Fixture;
		Change(Fixture);
		FHodgeHitReactionMigrationPlan Plan;
		TestFalse(Label, FHodgeHitReactionMigration::BuildPlan(Fixture.Profile.Get(), Fixture.BackupDirectory, Plan));
		TestFalse(TEXT("Rejected preview cannot convert"), Plan.CanConvert());
		TestTrue(TEXT("The specific refusal is visible"), HasMessage(Plan.Errors, Reason));
		TestFalse(TEXT("Rejected preview preserves legacy layout"), Fixture.Profile->bUseCompactAnimations);
	};
	Reject(TEXT("Heavy remapping cannot change the chosen action"), [](FMigrationFixture& F)
	{ F.Profile->Animations[1].DefaultDuration = 0.8f; }, TEXT("解析会改变"));
	Reject(TEXT("A partial Push cannot gain Slam support"), [](FMigrationFixture& F)
	{ F.Profile->Animations.RemoveAll([](const auto& E) { return E.Type == EHodgeImpactType::Slam; }); }, TEXT("支持范围会改变"));
	Reject(TEXT("Different AirHit and Slam resources cannot share Push"), [](FMigrationFixture& F)
	{ F.Profile->Animations.Last().Montage = F.MakeMontage(TEXT("OtherSlam")); }, TEXT("解析会改变"));
	Reject(TEXT("Strength matrices are not flattened"), [](FMigrationFixture& F)
	{ F.Profile->Animations[0].Variants[0].MinimumStrength = 0.5f; }, TEXT("方向或力度矩阵歧义"));
	Reject(TEXT("Front variants are not relabeled as lateral"), [](FMigrationFixture& F)
	{ F.Profile->Animations[0].Variants[0].Direction = EHodgeHitPresentationDirection::Front; }, TEXT("方向或力度矩阵歧义"));
	Reject(TEXT("GetUp differences are not discarded"), [](FMigrationFixture& F)
	{ F.Profile->Animations.Last().GetUpMontage = F.MakeMontage(TEXT("OtherGetUp")); }, TEXT("GetUp 与其他阶段不一致"));
	Reject(TEXT("Root motion declarations require the advanced layout"), [](FMigrationFixture& F)
	{ F.Profile->Animations[0].MotionSource = EHodgeReactionMotionSource::AnimationRootMotion; }, TEXT("根运动阶段来源"));
	Reject(TEXT("An unknown skeleton stops migration"), [](FMigrationFixture& F)
	{ F.Profile->Animations[0].Montage->SetSkeleton(nullptr); }, TEXT("缺失已知骨架"));
	Reject(TEXT("Hidden manual Compact values are not overwritten"), [](FMigrationFixture& F)
	{ F.Profile->Compact.Light.ControlDuration = 0.9f; }, TEXT("Compact 已有手调"));
	Reject(TEXT("A legacy replay interval is not silently ignored"), [](FMigrationFixture& F)
	{ F.Profile->GroundReplayInterval = 0.15f; }, TEXT("旧重播间隔"));
	Reject(TEXT("An existing in-memory backup is not replaced"), [](FMigrationFixture& F)
	{ CreatePackage(*(F.BackupDirectory / TEXT("DA_Profile"))); }, TEXT("已存在或已加载"));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHodgeMigrationExecutionTest, "Hodge.Editor.HitReactionMigration.Execution",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** 保存只作用于本例新建包；旧数组、恢复参数和安全上限在成功后保持不变。 */
bool FHodgeMigrationExecutionTest::RunTest(const FString&)
{
	FMigrationFixture Fixture;
	if (!TestTrue(TEXT("The isolated source package saves successfully"), Fixture.SaveSource())) { return false; }
	FHodgeHitReactionMigrationPlan Plan;
	if (!TestTrue(TEXT("Equivalent source has an executable preview"), FHodgeHitReactionMigration::BuildPlan(Fixture.Profile.Get(), Fixture.BackupDirectory, Plan))) { return false; }
	TArray<FText> Messages;
	Fixture.Profile->MaxAirHits += 1;
	TestFalse(TEXT("Edits after preview invalidate execution"), FHodgeHitReactionMigration::Execute(Plan, Messages));
	TestTrue(TEXT("Stale preview reports the changed configuration"), HasMessage(Messages, TEXT("配置已变化")));
	TestNull(TEXT("Stale preview creates no backup"), FindPackage(nullptr, *Plan.BackupPackageName));
	Fixture.Profile->MaxAirHits -= 1;
	const auto Original = Fixture.Profile->Animations;
	const float OldDownedDuration = Fixture.Profile->DownedDuration;
	const float OldRecoveryDelay = Fixture.Profile->DodgeRecoveryDelay;
	const int32 OldMaxAirHits = Fixture.Profile->MaxAirHits;
	if (!TestTrue(TEXT("Equivalent source converts and saves"), FHodgeHitReactionMigration::Execute(Plan, Messages)))
	{ for (const FText& Message : Messages) { AddError(Message.ToString()); } return false; }
	TestTrue(TEXT("Target selects the compact layout"), Fixture.Profile->bUseCompactAnimations);
	TestTrue(TEXT("Backup is persisted before target success"), FPackageName::DoesPackageExist(Plan.BackupPackageName));
	auto* Backup = FindObject<UHodgeHitReactionProfile>(FindPackage(nullptr, *Plan.BackupPackageName), TEXT("DA_Profile"));
	if (TestNotNull(TEXT("The saved backup is available"), Backup))
	{
		TestFalse(TEXT("Backup retains the original layout"), Backup->bUseCompactAnimations);
		TestEqual(TEXT("Backup retains every legacy entry"), Backup->Animations.Num(), Original.Num());
	}
	TestEqual(TEXT("Target retains every legacy entry"), Fixture.Profile->Animations.Num(), Original.Num());
	for (int32 Index = 0; Index < Original.Num(); ++Index)
	{ TestTrue(TEXT("Target preserves the exact original legacy fields"), FHodgeHitReactionAnimation::StaticStruct()->CompareScriptStruct(&Original[Index], &Fixture.Profile->Animations[Index], PPF_None)); }
	TestEqual(TEXT("Downed duration remains unchanged"), Fixture.Profile->DownedDuration, OldDownedDuration);
	TestEqual(TEXT("Recovery delay remains unchanged"), Fixture.Profile->DodgeRecoveryDelay, OldRecoveryDelay);
	TestEqual(TEXT("Safety limit remains unchanged"), Fixture.Profile->MaxAirHits, OldMaxAirHits);
	TestFalse(TEXT("Successful automatic save clears the target dirty flag"), Fixture.Profile->GetOutermost()->IsDirty());
	// 暂时制造仅在内存中的差异，保证冷读断言不能靠命中旧对象缓存通过。
	Fixture.Profile->DownedDuration = 91.f;
	if (Backup) { Backup->MaxAirHits = 73; }
	auto* DiskSource = Fixture.ReadSavedProfile(Fixture.SourcePackageName, TEXT("ReadSource"));
	auto* DiskBackup = Fixture.ReadSavedProfile(Plan.BackupPackageName, TEXT("ReadBackup"));
	Fixture.Profile->DownedDuration = OldDownedDuration;
	if (Backup) { Backup->MaxAirHits = OldMaxAirHits; }
	if (!TestNotNull(TEXT("Converted source is read from its saved file into a new package"), DiskSource) ||
		!TestNotNull(TEXT("Original backup is read from its saved file into a new package"), DiskBackup)) { return false; }
	TestTrue(TEXT("Disk source is a different object from the edited instance"), DiskSource != Fixture.Profile.Get());
	TestTrue(TEXT("Disk backup is a different object from the created backup instance"), DiskBackup != Backup);
	TestEqual(TEXT("Disk source has the explicit readback package identity"), DiskSource->GetOutermost()->GetName(), Fixture.Root / TEXT("ReadSource"));
	TestEqual(TEXT("Disk backup has the explicit readback package identity"), DiskBackup->GetOutermost()->GetName(), Fixture.Root / TEXT("ReadBackup"));
	TestTrue(TEXT("Converted layout persisted to the source file"), DiskSource->bUseCompactAnimations);
	TestTrue(TEXT("Mapped Compact values persisted exactly"), FHodgeCompactReactionAnimations::StaticStruct()->CompareScriptStruct(&Plan.Compact, &DiskSource->Compact, PPF_None));
	TestFalse(TEXT("Backup file persisted the original layout"), DiskBackup->bUseCompactAnimations);
	const FHodgeCompactReactionAnimations EmptyCompact;
	TestTrue(TEXT("Backup file preserved the original unused Compact values"), FHodgeCompactReactionAnimations::StaticStruct()->CompareScriptStruct(&EmptyCompact, &DiskBackup->Compact, PPF_None));
	for (const auto* DiskProfile : {DiskSource, DiskBackup})
	{
		TestEqual(TEXT("Both files retain all original legacy entries"), DiskProfile->Animations.Num(), Original.Num());
		for (int32 Index = 0; Index < FMath::Min(Original.Num(), DiskProfile->Animations.Num()); ++Index)
		{ TestTrue(TEXT("Persisted legacy data matches the original fields"), FHodgeHitReactionAnimation::StaticStruct()->CompareScriptStruct(&Original[Index], &DiskProfile->Animations[Index], PPF_None)); }
		TestEqual(TEXT("Persisted downed duration did not come from the modified memory instance"), DiskProfile->DownedDuration, OldDownedDuration);
		TestEqual(TEXT("Persisted recovery delay remains unchanged"), DiskProfile->DodgeRecoveryDelay, OldRecoveryDelay);
		TestEqual(TEXT("Persisted safety limit did not come from the modified memory instance"), DiskProfile->MaxAirHits, OldMaxAirHits);
	}
	for (const FString& PackageName : Fixture.ReadbackPackageNames)
	{ TestFalse(TEXT("Readback creates no extra asset file"), IFileManager::Get().FileExists(*FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()))); }
	FHodgeHitReactionMigrationPlan Recheck;
	TestTrue(TEXT("An already compact target can be inspected again"), FHodgeHitReactionMigration::BuildPlan(Fixture.Profile.Get(), Fixture.BackupDirectory, Recheck));
	TestTrue(TEXT("Recheck reports the existing compact layout"), Recheck.bAlreadyCompact);
	TestFalse(TEXT("Recheck never offers conversion"), Recheck.CanConvert());
	Fixture.Profile->MarkPackageDirty();
	TestTrue(TEXT("Repeated execute reports no conversion"), FHodgeHitReactionMigration::Execute(Recheck, Messages));
	TestTrue(TEXT("Repeated execute does not save an unchanged compact target"), Fixture.Profile->GetOutermost()->IsDirty());
	return true;
}
#endif
