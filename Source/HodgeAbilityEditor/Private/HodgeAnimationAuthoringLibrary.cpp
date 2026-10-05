#include "HodgeAnimationAuthoringLibrary.h"

#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/Skeleton.h"
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
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "KismetCompiler.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Serialization/ArchiveReplaceObjectRef.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"
#include "UObject/UnrealType.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAnimationAuthoringLibrary)

namespace HodgeAnimationAuthoring
{
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
