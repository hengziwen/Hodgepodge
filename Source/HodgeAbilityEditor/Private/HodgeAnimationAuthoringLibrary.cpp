#include "HodgeAnimationAuthoringLibrary.h"

#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimMontage.h"
#include "Animation/Skeleton.h"
#include "AnimGraphNode_ApplyAdditive.h"
#include "AnimGraphNode_IdentityPose.h"
#include "AnimGraphNode_LayeredBoneBlend.h"
#include "AnimGraphNode_SaveCachedPose.h"
#include "AnimGraphNode_Slot.h"
#include "AnimGraphNode_UseCachedPose.h"
#include "AnimBlueprintExtension.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphSchema_K2.h"
#include "Editor.h"
#include "Engine/World.h"
#include "Factories.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Pawn.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Select.h"
#include "K2Node_VariableGet.h"
#include "Animation/HodgeAnimInstance.h"
#include "AnimGraphNode_Base.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "KismetCompiler.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Serialization/ArchiveReplaceObjectRef.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "ScopedTransaction.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"
#include "UObject/UnrealType.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAnimationAuthoringLibrary)

namespace HodgeAnimationAuthoring
{
	template<typename T>
	T* FindOrAddPoseNode(UEdGraph* Graph, FName Name, int32 X, int32 Y)
	{
		if (T* Existing = FindObject<T>(Graph, *Name.ToString())) { return Existing; }
		T* Node = NewObject<T>(Graph, Name, RF_Transactional);
		Graph->AddNode(Node, false, false);
		Node->CreateNewGuid();
		Node->NodePosX = X; Node->NodePosY = Y;
		Node->PostPlacedNewNode();
		Node->AllocateDefaultPins();
		return Node;
	}

	bool IsWorkAsset(const UObject* Object)
	{
		return Object && (Object->GetOutermost()->GetName().StartsWith(TEXT("/Game/CodexText/LyraAnimation/"))
			|| Object->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/")));
	}

	class FAnimationTextFactory final : public FCustomizableTextObjectFactory
	{
	public:
		FAnimationTextFactory() : FCustomizableTextObjectFactory(GWarn) {}
		UAnimBlueprint* Blueprint = nullptr;

	protected:
		virtual bool CanCreateClass(UClass* ObjectClass, bool& bOmitSubObjs) const override
		{
			bOmitSubObjs = false;
			return ObjectClass && ObjectClass->IsChildOf(UAnimBlueprint::StaticClass());
		}

		virtual void ProcessConstructedObject(UObject* Object) override
		{
			Blueprint = Cast<UAnimBlueprint>(Object);
		}
	};

	bool ImportProperty(UObject* Object, const FString& PropertyPath, const FString& Value)
	{
		TArray<FString> Segments;
		PropertyPath.ParseIntoArray(Segments, TEXT("."), true);
		if (!Object || Segments.IsEmpty()) { return false; }
		void* Container = Object;
		UStruct* Struct = Object->GetClass();
		FProperty* Property = nullptr;
		for (int32 Index = 0; Index < Segments.Num(); ++Index)
		{
			Property = Struct->FindPropertyByName(*Segments[Index]);
			if (!Property) { return false; }
			if (Index + 1 < Segments.Num())
			{
				if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
				{
					Container = Property->ContainerPtrToValuePtr<void>(Container);
					Struct = StructProperty->Struct;
				}
				else if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(Property))
				{
					UObject* Nested = ObjectProperty->GetObjectPropertyValue(Property->ContainerPtrToValuePtr<void>(Container));
					// 只允许节点/CDO 自己拥有的子对象，不能沿资产引用修改原始资源。
					if (!Nested || !Nested->IsIn(Object)) { return false; }
					Nested->Modify();
					Container = Nested;
					Struct = Nested->GetClass();
				}
				else { return false; }
			}
		}
		Object->Modify();
		void* Address = Property->ContainerPtrToValuePtr<void>(Container);
		const TCHAR* Result = Property->ImportText_Direct(*Value, Address, Object, PPF_None, GWarn);
		return Result != nullptr;
	}
}

UAnimBlueprint* UHodgeAnimationAuthoringLibrary::ImportAnimationBlueprintText(const FString& AssetPath, const FString& TextFilename, UClass* ParentClass, USkeleton* TargetSkeleton)
{
	if (!AssetPath.StartsWith(TEXT("/Game/CodexText/LyraAnimation/")) || !FPackageName::IsValidLongPackageName(AssetPath))
	{
		return nullptr;
	}
	if (ParentClass && (!ParentClass->IsChildOf(UAnimInstance::StaticClass()) || !TargetSkeleton)) { return nullptr; }
	const FString ObjectPath = AssetPath + TEXT(".") + FPackageName::GetLongPackageAssetName(AssetPath);
	if (FPackageName::DoesPackageExist(AssetPath) || FindObject<UObject>(nullptr, *ObjectPath)) { return nullptr; }
	FString InputPath = FPaths::ConvertRelativePathToFull(TextFilename);
	FPaths::NormalizeFilename(InputPath);
	FString WorkDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("LyraAnimationWork/"));
	FPaths::NormalizeDirectoryName(WorkDirectory);
	if (!InputPath.StartsWith(WorkDirectory + TEXT("/"), ESearchCase::IgnoreCase)) { return nullptr; }
	FString Text;
	if (!FFileHelper::LoadFileToString(Text, *InputPath)) { return nullptr; }
	UPackage* Package = CreatePackage(*AssetPath);
	UAnimBlueprint* Blueprint = nullptr;
	if (ParentClass)
	{
		// 先生成类壳，使导入的自身类型引脚和成员引用能解析到当前副本。
		Blueprint = Cast<UAnimBlueprint>(FKismetEditorUtilities::CreateBlueprint(ParentClass, Package,
			*FPackageName::GetLongPackageAssetName(AssetPath), BPTYPE_Normal, UAnimBlueprint::StaticClass(),
			UAnimBlueprintGeneratedClass::StaticClass(), TEXT("HodgeAnimationAuthoring")));
		if (!Blueprint) { return nullptr; }
		Blueprint->TargetSkeleton = TargetSkeleton;
		TArray<UObject*> ExistingChildren;
		GetObjectsWithOuter(Blueprint, ExistingChildren, false);
		for (UObject* Child : ExistingChildren)
		{
			if (Child->IsA<UEdGraph>())
			{
				Child->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional | REN_DoNotDirty);
			}
		}
		Blueprint->FunctionGraphs.Empty();
		Blueprint->UbergraphPages.Empty();
		const int32 Start = Text.Find(TEXT("\n"));
		const int32 End = Text.Find(TEXT("End Object"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		if (Start == INDEX_NONE || End <= Start) { return nullptr; }
		const FString Properties = Text.Mid(Start + 1, End - Start - 1);
		FImportObjectParams Parameters;
		Parameters.DestData = reinterpret_cast<uint8*>(Blueprint);
		Parameters.SourceText = *Properties;
		Parameters.ObjectStruct = Blueprint->GetClass();
		Parameters.SubobjectRoot = Blueprint;
		Parameters.SubobjectOuter = Blueprint;
		Parameters.Warn = GWarn;
		Parameters.bShouldCallEditChange = false;
		if (!ImportObjectProperties(Parameters)) { return nullptr; }
		UAnimBlueprintExtension::RefreshExtensions(Blueprint);
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
	}
	else
	{
		HodgeAnimationAuthoring::FAnimationTextFactory Factory;
		Factory.ProcessBuffer(Package, RF_Public | RF_Standalone | RF_Transactional, Text);
		Blueprint = Factory.Blueprint;
	}
	if (!Blueprint || Blueprint->GetPathName() != ObjectPath || !Blueprint->ParentClass) { return nullptr; }
	FAssetRegistryModule::AssetCreated(Blueprint);
	Blueprint->MarkPackageDirty();
	return Blueprint;
}

bool UHodgeAnimationAuthoringLibrary::SetAnimationDefault(UBlueprint* Blueprint, const FString& PropertyName, const FString& Value)
{
	if (!HodgeAnimationAuthoring::IsWorkAsset(Blueprint) || !Blueprint->GeneratedClass) { return false; }
	Blueprint->Modify();
	if (!HodgeAnimationAuthoring::ImportProperty(Blueprint->GeneratedClass->GetDefaultObject(), PropertyName, Value)) { return false; }
	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
	return true;
}

bool UHodgeAnimationAuthoringLibrary::SetAnimationNodeProperty(UBlueprint* Blueprint, const FString& NodePath, const FString& PropertyPath, const FString& Value)
{
	if (!HodgeAnimationAuthoring::IsWorkAsset(Blueprint)) { return false; }
	UObject* Node = FindObject<UObject>(nullptr, *NodePath);
	if (!Node || !Node->IsIn(Blueprint) || !Node->IsA<UEdGraphNode>()) { return false; }
	Blueprint->Modify();
	if (!HodgeAnimationAuthoring::ImportProperty(Node, PropertyPath, Value)) { return false; }
	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
	return true;
}

bool UHodgeAnimationAuthoringLibrary::RemapAnimationReferences(UBlueprint* Blueprint, const TArray<UObject*>& Sources, const TArray<UObject*>& Destinations)
{
	if (!HodgeAnimationAuthoring::IsWorkAsset(Blueprint) || Sources.Num() != Destinations.Num()) { return false; }
	TMap<UObject*, UObject*> Replacements;
	for (int32 Index = 0; Index < Sources.Num(); ++Index)
	{
		if (!Sources[Index] || !Destinations[Index]) { return false; }
		Replacements.Add(Sources[Index], Destinations[Index]);
	}
	Blueprint->Modify();
	TArray<UObject*> Objects;
	GetObjectsWithOuter(Blueprint, Objects, true);
	Objects.Add(Blueprint);
	for (UObject* Object : Objects)
	{
		Object->Modify();
		FArchiveReplaceObjectRef<UObject> Archive(Object, Replacements,
			EArchiveReplaceObjectFlags::IgnoreOuterRef | EArchiveReplaceObjectFlags::IgnoreArchetypeRef);
	}
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	return true;
}

FString UHodgeAnimationAuthoringLibrary::CompileAnimationBlueprint(UBlueprint* Blueprint)
{
	if (!HodgeAnimationAuthoring::IsWorkAsset(Blueprint)) { return TEXT("ERRORS=1\nAsset is outside the animation work directory."); }
	if (UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(Blueprint))
	{
		UAnimBlueprintExtension::RefreshExtensions(AnimBlueprint);
	}
	FCompilerResultsLog Results;
	FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::SkipGarbageCollection, &Results);
	FString Output = FString::Printf(TEXT("ERRORS=%d WARNINGS=%d\n"), Results.NumErrors, Results.NumWarnings);
	for (const TSharedRef<FTokenizedMessage>& Message : Results.Messages)
	{
		Output += Message->ToText().ToString() + TEXT("\n");
	}
	return Output;
}

TArray<FName> UHodgeAnimationAuthoringLibrary::GetSkeletonBoneNames(USkeleton* Skeleton)
{
	TArray<FName> Names;
	if (Skeleton)
	{
		for (const FMeshBoneInfo& Bone : Skeleton->GetReferenceSkeleton().GetRefBoneInfo()) { Names.Add(Bone.Name); }
	}
	return Names;
}

bool UHodgeAnimationAuthoringLibrary::SetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot, FName Group)
{
	if (!Skeleton || !Skeleton->GetPathName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/")) || Slot.IsNone() || Group.IsNone()) { return false; }
	Skeleton->Modify();
	Skeleton->RegisterSlotNode(Slot);
	Skeleton->SetSlotGroupName(Slot, Group);
	Skeleton->MarkPackageDirty();
	return Skeleton->GetSlotGroupName(Slot) == Group;
}

FName UHodgeAnimationAuthoringLibrary::GetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot)
{
	return Skeleton ? Skeleton->GetSlotGroupName(Slot) : NAME_None;
}

bool UHodgeAnimationAuthoringLibrary::SetHeroReactionMontageSlot(UAnimMontage* Montage, FName Slot)
{
	if (!Montage || !Montage->GetPathName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/HitReactions/")) || Slot.IsNone()) { return false; }
	Montage->Modify();
	for (auto& Track : Montage->SlotAnimTracks) { Track.SlotName = Slot; }
	Montage->RefreshCacheData();
	Montage->MarkPackageDirty();
	return !Montage->SlotAnimTracks.IsEmpty();
}

bool UHodgeAnimationAuthoringLibrary::SetHeroReactionSkeleton(UAnimationAsset* Asset, USkeleton* Skeleton)
{
	if (!Asset || !Asset->GetPathName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/HitReactions/")) ||
		!Skeleton || Skeleton->GetPathName() != TEXT("/Game/Main/Character/Hero/Anim/Model/SK_Pover_LyraLab.SK_Pover_LyraLab")) { return false; }
	if (Asset->GetSkeleton() == Skeleton) { return true; }
	Asset->Modify();
	return Asset->ReplaceSkeleton(Skeleton, false);
}

bool UHodgeAnimationAuthoringLibrary::ConfigureHeroHitStunDuration(UAnimMontage* Montage, float Duration)
{
	if (!Montage || Montage->GetPathName() != TEXT("/Game/Main/Character/Hero/Anim/HitReactions/AM_Hero_HitStun.AM_Hero_HitStun") ||
		!FMath::IsFinite(Duration) || Duration <= .18f || Montage->SlotAnimTracks.Num() != 1 ||
		Montage->SlotAnimTracks[0].AnimTrack.AnimSegments.Num() != 1) { return false; }
	FAnimSegment& Segment = Montage->SlotAnimTracks[0].AnimTrack.AnimSegments[0];
	if (!Segment.GetAnimReference() || Segment.AnimStartTime + Duration * Segment.AnimPlayRate > Segment.GetAnimReference()->GetPlayLength()) { return false; }
	Montage->Modify();
	Segment.AnimEndTime = Segment.AnimStartTime + Duration * Segment.AnimPlayRate;
	Segment.LoopingCount = 1;
	Montage->SetCompositeLength(Duration);
	Montage->BlendOut.SetBlendTime(.18f);
	Montage->RefreshCacheData();
	Montage->MarkPackageDirty();
	return true;
}

bool UHodgeAnimationAuthoringLibrary::ConfigureHeroHitReactionGraph(UAnimBlueprint* Blueprint)
{
	if (!Blueprint || Blueprint->GetPathName() != TEXT("/Game/Main/Character/Hero/Anim/ABP_Pover_Base.ABP_Pover_Base") ||
		!Blueprint->TargetSkeleton || Blueprint->TargetSkeleton->GetReferenceSkeleton().FindBoneIndex(TEXT("Bip001Spine")) == INDEX_NONE ||
		!GEditor || GEditor->PlayWorld) { return false; }
	UEdGraph* Graph = FindObject<UEdGraph>(Blueprint, TEXT("AnimGraph"));
	if (!Graph) { return false; }
	auto* Slot = FindObject<UAnimGraphNode_Slot>(Graph, TEXT("AnimGraphNode_Slot_1"));
	UEdGraphNode* Aiming = FindObject<UEdGraphNode>(Graph, TEXT("AnimGraphNode_LinkedAnimLayer_0"));
	UEdGraphNode* OriginalAdditive = FindObject<UEdGraphNode>(Graph, TEXT("AnimGraphNode_ApplyAdditive_0"));
	UEdGraphNode* RotateRoot = FindObject<UEdGraphNode>(Graph, TEXT("AnimGraphNode_RotateRootBone_0"));
	UEdGraphNode* Controls = FindObject<UEdGraphNode>(Graph, TEXT("AnimGraphNode_LinkedAnimLayer_3"));
	if (!Slot || Slot->Node.SlotName != TEXT("AdditiveHitReact") || !Aiming || !OriginalAdditive || !RotateRoot || !Controls ||
		!Aiming->FindPin(TEXT("Pose")) || !OriginalAdditive->FindPin(TEXT("Base")) ||
		!RotateRoot->FindPin(TEXT("Pose")) || !Controls->FindPin(TEXT("InPose"))) { return false; }
	const FScopedTransaction Transaction(NSLOCTEXT("HodgeAnimation", "HitReactionGraph", "Configure hero hit reaction graph"));
	Blueprint->Modify(); Graph->Modify();
	for (UEdGraphNode* Node : {static_cast<UEdGraphNode*>(Slot), Aiming, OriginalAdditive, RotateRoot, Controls}) { Node->Modify(); }
	using namespace HodgeAnimationAuthoring;
	auto* Cache = FindOrAddPoseNode<UAnimGraphNode_SaveCachedPose>(Graph, TEXT("HodgeHitReactionBase"), 1260, 280);
	Cache->CacheName = TEXT("HodgeHitReactionBase");
	auto* Base = FindOrAddPoseNode<UAnimGraphNode_UseCachedPose>(Graph, TEXT("HodgeHitReactionBaseUse"), 1510, 160);
	auto* AdditiveBase = FindOrAddPoseNode<UAnimGraphNode_UseCachedPose>(Graph, TEXT("HodgeHitReactionAdditiveBaseUse"), 1510, 430);
	Base->SaveCachedPoseNode = Cache; AdditiveBase->SaveCachedPoseNode = Cache;
	auto* Identity = FindOrAddPoseNode<UAnimGraphNode_IdentityPose>(Graph, TEXT("HodgeHitReactionIdentity"), 1280, 730);
	auto* Apply = FindOrAddPoseNode<UAnimGraphNode_ApplyAdditive>(Graph, TEXT("HodgeHitReactionApplyAdditive"), 1850, 430);
	Apply->Node.Alpha = 1.f;
	auto* Blend = FindOrAddPoseNode<UAnimGraphNode_LayeredBoneBlend>(Graph, TEXT("HodgeHitReactionUpperBodyBlend"), 2120, 190);
	Blend->Node.BlendMode = ELayeredBoneBlendMode::BranchFilter;
	Blend->Node.BlendPoses.SetNum(1); Blend->Node.BlendWeights.SetNum(1); Blend->Node.LayerSetup.SetNum(1);
	Blend->Node.BlendWeights[0] = .35f;
	Blend->Node.LayerSetup[0].BranchFilters.Reset();
	FBranchFilter Filter; Filter.BoneName = TEXT("Bip001Spine"); Filter.BlendDepth = 1;
	Blend->Node.LayerSetup[0].BranchFilters.Add(Filter);
	Blend->Node.bMeshSpaceRotationBlend = true;
	Blend->Node.CurveBlendOption = ECurveBlendOption::UseBasePose;
	Blend->ReconstructNode();
	Slot->NodePosX = 1510; Slot->NodePosY = 700;
	Slot->Node.bAlwaysUpdateSourcePose = true;
	const UEdGraphSchema* Schema = Graph->GetSchema();
	Schema->BreakPinLinks(*OriginalAdditive->FindPin(TEXT("Base")), true);
	Schema->BreakPinLinks(*Controls->FindPin(TEXT("InPose")), true);
	Schema->BreakPinLinks(*Slot->FindPin(TEXT("Source")), true);
	Schema->BreakPinLinks(*Slot->FindPin(TEXT("Pose")), true);
	auto Connect = [Schema](UEdGraphNode* From, const TCHAR* Output, UEdGraphNode* To, const TCHAR* Input)
	{
		UEdGraphPin* A = From->FindPin(Output); UEdGraphPin* B = To->FindPin(Input);
		return A && B && (A->LinkedTo.Contains(B) || Schema->TryCreateConnection(A, B));
	};
	const bool bLinked = Connect(Aiming, TEXT("Pose"), OriginalAdditive, TEXT("Base")) &&
		Connect(RotateRoot, TEXT("Pose"), Cache, TEXT("Pose")) &&
		Connect(Identity, TEXT("Pose"), Slot, TEXT("Source")) &&
		Connect(AdditiveBase, TEXT("Pose"), Apply, TEXT("Base")) &&
		Connect(Slot, TEXT("Pose"), Apply, TEXT("Additive")) &&
		Connect(Base, TEXT("Pose"), Blend, TEXT("BasePose")) &&
		Connect(Apply, TEXT("Pose"), Blend, TEXT("BlendPoses_0")) &&
		Connect(Blend, TEXT("Pose"), Controls, TEXT("InPose"));
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	return bLinked && SetSkeletonSlotGroup(Blueprint->TargetSkeleton, TEXT("AdditiveHitReact"), TEXT("HodgeHitFeedback"));
}

FString UHodgeAnimationAuthoringLibrary::ConfigureHeroFacingModes(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint)
{
	if (!MainBlueprint || !LayerBlueprint || !GEditor || GEditor->PlayWorld ||
		MainBlueprint->GetPathName() != TEXT("/Game/Main/Character/Hero/Anim/ABP_Pover_Base.ABP_Pover_Base") ||
		LayerBlueprint->GetPathName() != TEXT("/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase.ABP_Pover_LocomotionBase"))
	{ return TEXT("ERROR: unexpected asset or PIE is active"); }
	const FScopedTransaction Transaction(NSLOCTEXT("HodgeAnimation", "FacingModes", "Isolate free and strafe locomotion"));
	MainBlueprint->Modify(); LayerBlueprint->Modify();
	int32 DirectionEntries = 0; int32 WarpingEntries = 0;
	TArray<UEdGraph*> Graphs; LayerBlueprint->GetAllGraphs(Graphs);
	auto AddMainProperty = [LayerBlueprint](UEdGraph* Graph, FName Property) -> UEdGraphPin*
	{
		using namespace HodgeAnimationAuthoring;
		auto* MainCall = FindOrAddPoseNode<UK2Node_CallFunction>(Graph, TEXT("HodgeFacingMainAnim"), -1000, -300);
		UFunction* Function = LayerBlueprint->GeneratedClass ? LayerBlueprint->GeneratedClass->FindFunctionByName(TEXT("GetMainAnimBPThreadSafe")) : nullptr;
		if (!Function) { return nullptr; }
		MainCall->SetFromFunction(Function); MainCall->ReconstructNode();
		auto* Get = FindOrAddPoseNode<UK2Node_VariableGet>(Graph, FName(TEXT("HodgeFacing_") + Property.ToString()), -800, -300);
		Get->VariableReference.SetExternalMember(Property, UHodgeAnimInstance::StaticClass()); Get->ReconstructNode();
		UEdGraphPin* Object = MainCall->FindPin(TEXT("ReturnValue")); UEdGraphPin* Self = Get->FindPin(UEdGraphSchema_K2::PN_Self);
		if (!Object || !Self || (!Object->LinkedTo.Contains(Self) && !Graph->GetSchema()->TryCreateConnection(Object, Self))) { return nullptr; }
		return Get->FindPin(Property);
	};
	for (UEdGraph* Graph : Graphs)
	{
		const auto OriginalNodes = Graph->Nodes;
		for (UEdGraphNode* Node : OriginalNodes)
		{
			if (auto* Select = Cast<UK2Node_Select>(Node); Select && Select->GetEnum() &&
				Select->GetEnum()->GetName() == TEXT("AnimEnum_CardinalDirection") &&
				(Graph->GetFName() == TEXT("UpdateCycleAnim") || Graph->GetFName() == TEXT("SetUpStopAnim") || Graph->GetFName() == TEXT("SetUpStartAnim")))
			{
				++DirectionEntries;
				const FName NewName(TEXT("HodgeFacingDirection_") + Node->GetName());
				if (FindObject<UK2Node_Select>(Graph, *NewName.ToString())) { continue; }
				UEdGraphPin* Mode = AddMainProperty(Graph, TEXT("bUseStrafeLocomotion"));
				if (!Mode) { return TEXT("ERROR: cannot bind main facing snapshot"); }
				Node->Modify(); Graph->Modify();
				UEdGraphPin* Index = Select->GetIndexPin(); const auto Sources = Index->LinkedTo;
				auto* Branch = HodgeAnimationAuthoring::FindOrAddPoseNode<UK2Node_Select>(Graph, NewName, Node->NodePosX - 250, Node->NodePosY - 80);
				if (!HodgeAnimationAuthoring::ImportProperty(Branch, TEXT("IndexPinType"), TEXT("(PinCategory=\"bool\",PinSubCategory=\"\")")))
				{ return TEXT("ERROR: cannot initialize mode selector"); }
				Branch->ReconstructNode();
				TArray<UEdGraphPin*> Options; Branch->GetOptionPins(Options);
				if (Options.Num() != 2) { return TEXT("ERROR: mode selector has unexpected pins"); }
				FEdGraphPinType ValueType = Index->PinType;
				ValueType.PinSubCategory = NAME_None; ValueType.PinSubCategoryObject = Select->GetEnum();
				for (UEdGraphPin* Pin : Options) { Pin->PinType = ValueType; }
				Branch->GetReturnValuePin()->PinType = ValueType;
				Options[0]->DefaultValue = Select->GetEnum()->GetNameStringByIndex(0);
				Graph->GetSchema()->BreakPinLinks(*Index, true);
				for (UEdGraphPin* Source : Sources)
				{ if (!Graph->GetSchema()->TryCreateConnection(Source, Options[1])) { return TEXT("ERROR: cannot preserve strafe direction"); } }
				const auto ModeResponse = Graph->GetSchema()->CanCreateConnection(Mode, Branch->GetIndexPin());
				if (!Graph->GetSchema()->TryCreateConnection(Mode, Branch->GetIndexPin()))
				{ return TEXT("ERROR: mode selector: ") + ModeResponse.Message.ToString(); }
				const auto DirectionResponse = Graph->GetSchema()->CanCreateConnection(Branch->GetReturnValuePin(), Index);
				if (!Graph->GetSchema()->TryCreateConnection(Branch->GetReturnValuePin(), Index))
				{ return TEXT("ERROR: direction selector: ") + DirectionResponse.Message.ToString(); }
				Branch->NodeComment = TEXT("Free: Forward; ReservedStrafe: original actor / visual relative direction");
			}
			if (Node->GetClass()->GetName().Contains(TEXT("OrientationWarping")))
			{
				auto* AnimNode = Cast<UAnimGraphNode_Base>(Node);
				if (!AnimNode) { return TEXT("ERROR: unexpected warping node"); }
				AnimNode->Modify();
				for (int32 Index = 0; Index < AnimNode->ShowPinForProperties.Num(); ++Index)
				{ if (AnimNode->ShowPinForProperties[Index].PropertyName == TEXT("Alpha")) { AnimNode->SetPinVisibility(true, Index); break; } }
				UEdGraphPin* Alpha = Node->FindPin(TEXT("Alpha"));
				UEdGraphPin* Weight = AddMainProperty(Graph, TEXT("StrafeLocomotionWeight"));
				if (!Alpha || !Weight) { return TEXT("ERROR: cannot bind warping mode weight"); }
				Graph->GetSchema()->BreakPinLinks(*Alpha, true);
				if (!Graph->GetSchema()->TryCreateConnection(Weight, Alpha)) { return TEXT("ERROR: cannot connect warping mode weight"); }
				++WarpingEntries;
			}
		}
	}
	if (DirectionEntries < 6 || WarpingEntries < 1) { return TEXT("ERROR: required locomotion graphs missing"); }
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainBlueprint);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(LayerBlueprint);
	return FString::Printf(TEXT("OK DirectionEntries=%d WarpingEntries=%d"), DirectionEntries, WarpingEntries);
}

bool UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabPIE(int32 PlayerCount, bool bListenServer)
{
	if (!GEditor || GEditor->PlayWorld || PlayerCount < 1 || PlayerCount > 2 ||
		!HodgeAnimationAuthoring::IsWorkAsset(GEditor->GetEditorWorldContext().World())) { return false; }
	ULevelEditorPlaySettings* Settings = GetMutableDefault<ULevelEditorPlaySettings>();
	Settings->SetPlayNumberOfClients(PlayerCount);
	Settings->SetPlayNetMode(bListenServer ? PIE_ListenServer : PIE_Standalone);
	return true;
}

bool UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabGameMode(UBlueprint* Blueprint, TSubclassOf<APawn> PawnClass)
{
	if (!HodgeAnimationAuthoring::IsWorkAsset(Blueprint) || !PawnClass || !Blueprint->ParentClass ||
		!Blueprint->ParentClass->IsChildOf(AGameModeBase::StaticClass())) { return false; }
	const FName FunctionName(TEXT("GetDefaultPawnClassForController"));
	UFunction* Signature = Blueprint->ParentClass->FindFunctionByName(FunctionName);
	if (!Signature) { return false; }
	Blueprint->Modify();
	UEdGraph* Graph = nullptr;
	for (UEdGraph* FunctionGraph : Blueprint->FunctionGraphs)
	{
		if (FunctionGraph && FunctionGraph->GetFName() == FunctionName) { Graph = FunctionGraph; break; }
	}
	if (!Graph)
	{
		Graph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, FunctionName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
		FBlueprintEditorUtils::AddFunctionGraph(Blueprint, Graph, false, Signature);
	}
	TArray<UK2Node_FunctionEntry*> Entries;
	TArray<UK2Node_FunctionResult*> Results;
	Graph->GetNodesOfClass(Entries);
	Graph->GetNodesOfClass(Results);
	if (Entries.Num() != 1 || Results.Num() != 1) { return false; }
	UK2Node_FunctionEntry* Entry = Entries[0];
	UK2Node_FunctionResult* Result = Results[0];
	// 使用父类签名重建节点，避免手动声明的参数改变 BlueprintNativeEvent 的签名。
	for (UK2Node_FunctionTerminator* Node : { static_cast<UK2Node_FunctionTerminator*>(Entry), static_cast<UK2Node_FunctionTerminator*>(Result) })
	{
		Node->Modify();
		Node->UserDefinedPins.Reset();
		Node->FunctionReference.SetExternalMember(FunctionName, Signature->GetOuterUClass());
		Node->bDisableOrphanPinSaving = true;
		Node->ReconstructNode();
		Node->bDisableOrphanPinSaving = false;
	}
	Entry->SetExtraFlags(0);
	HodgeAnimationAuthoring::ImportProperty(Entry, TEXT("bIsEditable"), TEXT("False"));
	HodgeAnimationAuthoring::ImportProperty(Result, TEXT("bIsEditable"), TEXT("False"));
	UEdGraphPin* ReturnPin = Result->FindPin(TEXT("ReturnValue"));
	UEdGraphPin* ThenPin = Entry->FindPin(UEdGraphSchema_K2::PN_Then);
	UEdGraphPin* ExecutePin = Result->FindPin(UEdGraphSchema_K2::PN_Execute);
	if (!ReturnPin || !ThenPin || !ExecutePin) { return false; }
	const UEdGraphSchema* Schema = Graph->GetSchema();
	Schema->TrySetDefaultObject(*ReturnPin, PawnClass.Get());
	if (!Schema->TryCreateConnection(ThenPin, ExecutePin)) { return false; }
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	return ReturnPin->DefaultObject == PawnClass.Get();
}
