/** @file HodgeAnimationAuthoringLibrary.cpp
 * @brief 实现动画/行为树编辑器制作函数库相关的类型、接口与现有处理流程。
 */
#include "HodgeAnimationAuthoringLibrary.h"
#include "Data/HodgeAbilityDefinition.h"
#include "Animation/HodgeRootMotionModifier_AttackFacing.h"
#include "AnimNotifyState_MotionWarping.h"

#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Engine/SkeletalMesh.h"
#include "Model.h"
#include "Character/HodgeEnemyCharacter.h"
#include "Equipment/HodgeEquipmentManagerComponent.h"
#include "GameFeatureAction_AddComponents.h"
#include "Builders/CubeBuilder.h"
#include "Components/BrushComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"
#include "AnimGraphNode_SequencePlayer.h"
#include "AnimGraphNode_Root.h"
#include "AI/HodgeBTTask_ExecuteAbility.h"
#include "Animation/HodgeEnemyAnimInstance.h"
#include "Animation/HodgeWingAnimInstance.h"
#include "Animation/HodgeAnimNotifyState_WingAction.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/HodgeWingComponent.h"
#include "HodgeCombatValidationLibrary.h"
#include "Animation/HodgeWingProfile.h"
#include "AnimGraphNode_LocalRefPose.h"
#include "AnimGraphNode_LocalToComponentSpace.h"
#include "AnimGraphNode_ComponentToLocalSpace.h"
#include "AnimGraphNode_ModifyBone.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BehaviorTreeGraph.h"
#include "EdGraphSchema_BehaviorTree.h"
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
#include "K2Node_IfThenElse.h"
#include "K2Node_VariableGet.h"
#include "Animation/HodgeAnimInstance.h"
#include "AnimGraphNode_Base.h"
#include "AnimGraphNode_BlendListByBool.h"
#include "AnimGraphNode_TransitionResult.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "KismetCompiler.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Serialization/ArchiveReplaceObjectRef.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "ScopedTransaction.h"
#include "Misc/ScopeExit.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"
#include "UObject/UnrealType.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAnimationAuthoringLibrary)

FString UHodgeAnimationAuthoringLibrary::ConfigureWingBoxHit(UAnimMontage* Montage, UHodgeWingProfile* Profile, FName AnchorBone, float Margin)
{
	if (!GEditor || GEditor->PlayWorld || !Montage || !Profile || !Profile->Mesh || AnchorBone.IsNone() ||
		!FMath::IsFinite(Margin) || Margin < 0.f || Profile->PlaybackMode != EHodgeWingPlaybackMode::AnimationAsset ||
		!Montage->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/Montages/")) ||
		!Profile->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Hero/Wing/"))) { return TEXT("Invalid wing box authoring context"); }
	const auto* Binding = Profile->AnimationBindings.FindByPredicate([Montage](const auto& Item) { return Item.SourceAnimation == Montage; });
	if (!Binding || !Binding->WingAnimation || Profile->Mesh->GetRefSkeleton().FindBoneIndex(AnchorBone) == INDEX_NONE)
	{ return TEXT("Missing mapped wing animation or anchor bone"); }
	FAnimNotifyEvent* WingEvent = nullptr;
	UHodgeAnimNotifyState_HitCheck* Primary = nullptr;
	for (auto& Event : Montage->Notifies)
	{
		auto* Hit = Cast<UHodgeAnimNotifyState_HitCheck>(Event.NotifyStateClass);
		if (!Hit) { continue; }
		if (Hit->Hit.Source == EHodgeAnimHitSource::NamedComponent && Hit->Hit.ComponentName == TEXT("WingMesh"))
		{
			if (WingEvent) { return TEXT("Multiple wing hit windows require explicit selection"); }
			WingEvent = &Event;
		}
		else if (Hit->Hit.Source == EHodgeAnimHitSource::CharacterMeshSocket) { Primary = Hit; }
	}
	if (!WingEvent || !Primary) { return TEXT("Expected one existing wing window and a shared primary strike"); }
	const float Start = WingEvent->GetTime();
	const float End = Start + WingEvent->GetDuration();
	if (Start < 0.f || End <= Start || End > Montage->GetPlayLength() || !Binding->IsWindowOpen(Start) || !Binding->IsWindowOpen(End - .0001f))
	{ return TEXT("Wing damage window must be inside its presentation window"); }
	const auto* RenderData = Profile->Mesh->GetResourceForRendering();
	if (!RenderData || RenderData->LODRenderData.IsEmpty()) { return TEXT("Wing LOD0 render data unavailable"); }
	TStrongObjectPtr<USkeletalMeshComponent> Mesh(NewObject<USkeletalMeshComponent>(GetTransientPackage()));
	Mesh->SetSkeletalMeshAsset(Profile->Mesh);
	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Mesh->SetWorldTransform(Profile->MeshOffset);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetVisibility(false);
	Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	Mesh->RegisterComponentWithWorld(GEditor->GetEditorWorldContext().World());
	ON_SCOPE_EXIT { Mesh->DestroyComponent(); };
	Mesh->SetAnimation(Binding->WingAnimation);
	const int32 Samples = FMath::Clamp(FMath::CeilToInt((End - Start) * 60.f) + 1, 2, 241);
	FBox Bounds(ForceInit);
	int64 VertexSamples = 0;
	for (int32 Sample = 0; Sample < Samples; ++Sample)
	{
		const float Time = FMath::Lerp(Start, End, float(Sample) / (Samples - 1));
		Mesh->SetPosition(Binding->EvaluateWingTime(Time), false);
		Mesh->TickAnimation(0.f, false);
		Mesh->RefreshBoneTransforms();
		TArray<FMatrix44f> RefToLocal;
		Mesh->GetCurrentRefToLocalMatrices(RefToLocal, 0);
		TArray<FVector3f> Vertices;
		const auto& LOD = RenderData->LODRenderData[0];
		const auto* Weights = LOD.GetSkinWeightVertexBuffer();
		if (!Weights || RefToLocal.IsEmpty()) { return TEXT("Wing CPU skin weights or pose matrices unavailable"); }
		// 直接计算骨骼蒙皮顶点，不依赖临时组件的渲染 MeshObject。
		USkinnedMeshComponent::ComputeSkinnedPositions(Mesh.Get(), Vertices, RefToLocal, LOD, *Weights);
		FTransform Anchor = Mesh->GetSocketTransform(AnchorBone);
		Anchor.SetScale3D(FVector::OneVector);
		// 原始特效槽不承担实体判定，实体与遮罩翼面都纳入包围范围。
		for (const auto& Section : RenderData->LODRenderData[0].RenderSections)
		{
			if (Section.MaterialIndex == 0) { continue; }
			for (uint32 Index = Section.BaseVertexIndex; Index < Section.BaseVertexIndex + Section.NumVertices; ++Index)
			{
				if (!Vertices.IsValidIndex(Index)) { return TEXT("Wing CPU skin vertex data incomplete"); }
				const FVector Point = Anchor.InverseTransformPosition(Mesh->GetComponentTransform().TransformPosition(FVector(Vertices[Index])));
				if (Point.ContainsNaN()) { return TEXT("Nonfinite wing skin vertex"); }
				Bounds += Point;
				++VertexSamples;
			}
		}
	}
	if (!Bounds.IsValid || VertexSamples == 0) { return TEXT("No physical wing vertices sampled"); }
	const FVector Center = Bounds.GetCenter();
	const FVector Extent = Bounds.GetExtent() + FVector(Margin);
	auto* Notify = CastChecked<UHodgeAnimNotifyState_HitCheck>(WingEvent->NotifyStateClass);
	FHodgeAnimHitConfig Config = Notify->Hit;
	Config.Shape = EHodgeHitShape::Box;
	Config.BoneOrSocket = AnchorBone;
	Config.BoxHalfExtent = Extent;
	Config.LocalTransform = FTransform(FQuat::Identity, Center);
	Config.TransformPolicy = EHodgeHitTransformPolicy::Follow;
	Config.HitGroup = Primary->Hit.HitGroup.IsNone() ? FName(TEXT("PoverWingFinisher")) : Primary->Hit.HitGroup;
	Config.AttackPhase = Primary->Hit.AttackPhase.IsNone() ? FName(TEXT("Strike")) : Primary->Hit.AttackPhase;
	TArray<FText> Errors;
	if (!Config.Validate(Errors)) { return TEXT("Measured wing box configuration failed validation"); }
	// 测量与预校验全部成功后再提交，避免失败留下一半资产配置。
	Montage->Modify(); Notify->Modify(); Primary->Modify(); Profile->Modify();
	Notify->Hit = Config;
	Primary->Hit.HitGroup = Config.HitGroup;
	Primary->Hit.AttackPhase = Config.AttackPhase;
	Profile->bEnableAuthorityHitSampling = true;
	Montage->RefreshCacheData(); Montage->MarkPackageDirty(); Profile->MarkPackageDirty();
	return FString::Printf(TEXT("{\"begin\":%.6f,\"end\":%.6f,\"samples\":%d,\"vertices\":%lld,\"center\":[%.6f,%.6f,%.6f],\"half_extent\":[%.6f,%.6f,%.6f],\"margin\":%.6f}"),
		Start, End, Samples, VertexSamples, Center.X, Center.Y, Center.Z, Extent.X, Extent.Y, Extent.Z, Margin);
}

namespace HodgeAnimationAuthoring
{
	template<typename T>
	/** 查找Or添加姿势节点。
	 * @param Graph 图，由调用方传入。
	 * @param Name 名称，由调用方传入。
	 * @param X X，由调用方传入。
	 * @param Y Y，由调用方传入。
	 * @return Or添加姿势节点的当前值或引用。
	 */
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

	/** 判断Work资产。
	 * @param Object 对象，由调用方传入。
	 * @return true表示满足该查询条件，false表示不满足。
	 */
	bool IsWorkAsset(const UObject* Object)
	{
		return Object && (Object->GetOutermost()->GetName().StartsWith(TEXT("/Game/CodexText/LyraAnimation/"))
			|| Object->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Enemies/"))
			|| Object->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/"))
			|| Object->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Hero/Wing/")));
	}

	/** 封装动画文字Factory的状态与操作接口。 */
	class FAnimationTextFactory final : public FCustomizableTextObjectFactory
	{
	public:
		/** 构造动画文字Factory并初始化其默认状态。 */
		FAnimationTextFactory() : FCustomizableTextObjectFactory(GWarn) {}
		// Blueprint的数据或对象引用。
		UAnimBlueprint* Blueprint = nullptr;

	protected:
		/** 检查是否允许创建类型。
		 * @param ObjectClass 对象类型，由调用方传入。
		 * @param bOmitSubObjs 是否OmitSubObjs的控制标记。
		 * @return true表示满足该查询条件，false表示不满足。
		 */
		virtual bool CanCreateClass(UClass* ObjectClass, bool& bOmitSubObjs) const override
		{
			bOmitSubObjs = false;
			return ObjectClass && ObjectClass->IsChildOf(UAnimBlueprint::StaticClass());
		}

		/** 记录文本导入过程中构造的对象。
		 * @param Object 对象，由调用方传入。
		 */
		virtual void ProcessConstructedObject(UObject* Object) override
		{
			Blueprint = Cast<UAnimBlueprint>(Object);
		}
	};

	/** 按引擎属性导入规则写入目标配置字段。
	 * @param Object 对象，由调用方传入。
	 * @param PropertyPath 属性路径，由调用方传入。
	 * @param Value 值，由调用方传入。
	 * @return true表示本次检查或操作成功，false表示失败。
	 */
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

/** 配置翅膀动画。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureWingActionWindow(UAnimSequenceBase* Source, float Begin, float End)
{
	if (!Source || (GEditor && GEditor->PlayWorld) || !FMath::IsFinite(Begin) || !FMath::IsFinite(End) ||
		Begin < 0.f || End <= Begin || End > Source->GetPlayLength()) { return false; }
	Source->Modify();
	for (auto& Event : Source->Notifies)
	{
		if (auto* Window = Cast<UHodgeAnimNotifyState_WingAction>(Event.NotifyStateClass); Window && Window->WindowId == TEXT("AttackWing"))
		{
			Event.Link(Source, Begin);
			Event.SetDuration(End - Begin);
			Event.EndLink.Link(Source, End);
			Source->RefreshCacheData();
			return true;
		}
	}
	// 只追加本功能的窗口，不删除用户已有的命中和手持通知。
	FAnimNotifyEvent Event;
	Event.NotifyStateClass = NewObject<UHodgeAnimNotifyState_WingAction>(Source);
	Event.NotifyName = TEXT("AttackWing");
	Event.MontageTickType = EMontageNotifyTickType::BranchingPoint;
	Event.Link(Source, Begin);
	Event.SetDuration(End - Begin);
	Event.EndLink.Link(Source, End);
	Source->Notifies.Add(Event);
	Source->RefreshCacheData();
	return true;
}

bool UHodgeAnimationAuthoringLibrary::ConfigureWingAttackPreview(UBlueprint* Blueprint, UHodgeWingProfile* Profile)
{
	if (!Blueprint || !Profile || Profile->PlaybackMode != EHodgeWingPlaybackMode::AnimationAsset ||
		!Blueprint->SimpleConstructionScript || (GEditor && GEditor->PlayWorld)) { return false; }
	USCS_Node* WingNode = nullptr;
	USCS_Node* CoordinatorNode = nullptr;
	for (USCS_Node* Node : Blueprint->SimpleConstructionScript->GetAllNodes())
	{
		if (Node->GetVariableName() == TEXT("SkeletalMesh")) { WingNode = Node; }
		if (Node->GetVariableName() == TEXT("WingPresentation")) { CoordinatorNode = Node; }
	}
	auto* Mesh = WingNode ? Cast<USkeletalMeshComponent>(WingNode->ComponentTemplate) : nullptr;
	if (!Mesh || Mesh->GetSkeletalMeshAsset() != Profile->Mesh) { return false; }
	Blueprint->Modify();
	if (!CoordinatorNode)
	{
		CoordinatorNode = Blueprint->SimpleConstructionScript->CreateNode(UHodgeWingComponent::StaticClass(), TEXT("WingPresentation"));
		Blueprint->SimpleConstructionScript->AddNode(CoordinatorNode);
	}
	auto* Coordinator = Cast<UHodgeWingComponent>(CoordinatorNode->ComponentTemplate);
	if (!Coordinator) { return false; }
	Coordinator->Profile = Profile;
	Coordinator->ExistingMeshComponentName = TEXT("SkeletalMesh");
	Mesh->Modify();
	Mesh->SetHiddenInGame(true);
	Mesh->SetVisibility(false);
	Mesh->AnimationData.bSavedPlaying = false;
	Mesh->AnimationData.bSavedLooping = false;
	// 身体仍使用用户选择的源动画，仅打开编辑器动画更新。
	if (auto* Character = Cast<ACharacter>(Blueprint->GeneratedClass->GetDefaultObject()))
	{
		Character->GetMesh()->SetUpdateAnimationInEditor(true);
	}
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	return true;
}

bool UHodgeAnimationAuthoringLibrary::ConfigureWingAnimation(UAnimBlueprint* Blueprint)
{
	if (!Blueprint || !Blueprint->ParentClass->IsChildOf(UHodgeWingAnimInstance::StaticClass()) ||
		!Blueprint->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Hero/Wing/")) || (GEditor && GEditor->PlayWorld)) { return false; }
	UEdGraph* Graph = nullptr;
	for (UEdGraph* Candidate : Blueprint->FunctionGraphs)
	{ if (Candidate && Candidate->GetFName() == TEXT("AnimGraph")) { Graph = Candidate; break; } }
	if (!Graph) { return false; }
	using namespace HodgeAnimationAuthoring;
	auto* Root = FindOrAddPoseNode<UAnimGraphNode_Root>(Graph, TEXT("WingOutput"), 1500, 0);
	for (UEdGraphNode* Node : TArray<UEdGraphNode*>(Graph->Nodes))
	{ if (Node != Root && Node->IsA<UAnimGraphNode_Root>()) { FBlueprintEditorUtils::RemoveNode(Blueprint, Node, true); } }
	auto* Ref = FindOrAddPoseNode<UAnimGraphNode_LocalRefPose>(Graph, TEXT("WingReference"), 0, 0);
	auto* ToComponent = FindOrAddPoseNode<UAnimGraphNode_LocalToComponentSpace>(Graph, TEXT("WingToComponent"), 200, 0);
	auto* ToLocal = FindOrAddPoseNode<UAnimGraphNode_ComponentToLocalSpace>(Graph, TEXT("WingToLocal"), 1250, 0);
	const auto* Schema = Graph->GetSchema();
	const auto Link = [Schema](UEdGraphNode* A, FName Out, UEdGraphNode* B, FName In)
	{
		auto* X = A->FindPin(Out); auto* Y = B->FindPin(In);
		return X && Y && (X->LinkedTo.Contains(Y) || Schema->TryCreateConnection(X, Y));
	};
	bool bOK = Link(Ref, TEXT("Pose"), ToComponent, TEXT("LocalPose"));
	UEdGraphNode* Previous = ToComponent;
	const FName Bones[] = {TEXT("Wing000"), TEXT("Wing003"), TEXT("Wing006")};
	const FName Variables[] = {TEXT("WingRootRotation"), TEXT("WingMidRotation"), TEXT("WingTipRotation")};
	for (int32 I = 0; I < 3; ++I)
	{
		auto* Modify = FindOrAddPoseNode<UAnimGraphNode_ModifyBone>(Graph, *FString::Printf(TEXT("WingBone%d"), I), 450 + I * 260, 0);
		Modify->Node.BoneToModify.BoneName = Bones[I];
		Modify->Node.RotationMode = BMM_Additive;
		Modify->Node.RotationSpace = BCS_BoneSpace;
		Modify->Node.TranslationMode = BMM_Ignore;
		Modify->Node.ScaleMode = BMM_Ignore;
		Modify->Node.Alpha = 1.f;
		for (int32 P = 0; P < Modify->ShowPinForProperties.Num(); ++P)
		{ if (Modify->ShowPinForProperties[P].PropertyName == TEXT("Rotation")) { Modify->SetPinVisibility(true, P); break; } }
		Modify->ReconstructNode();
		auto* Get = FindOrAddPoseNode<UK2Node_VariableGet>(Graph, *FString::Printf(TEXT("WingRotation%d"), I), 450 + I * 260, -200);
		Get->VariableReference.SetSelfMember(Variables[I]); Get->ReconstructNode();
		bOK &= Link(Get, Variables[I], Modify, TEXT("Rotation")) &&
			Link(Previous, I == 0 ? TEXT("ComponentPose") : TEXT("Pose"), Modify, TEXT("ComponentPose"));
		Previous = Modify;
	}
	bOK &= Link(Previous, TEXT("Pose"), ToLocal, TEXT("ComponentPose")) && Link(ToLocal, TEXT("Pose"), Root, TEXT("Result"));
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	return bOK;
}

/** 配置翅膀Experience配置。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureWingExperience(UBlueprint* Experience, TSubclassOf<AActor> ActorClass, TSubclassOf<UActorComponent> ComponentClass)
{
	if (!Experience || !Experience->GeneratedClass || !ActorClass || !ComponentClass ||
		!ComponentClass->IsChildOf(UHodgeWingComponent::StaticClass()) ||
		Experience->GetOutermost()->GetName() != TEXT("/Game/Main/Experiences/Exp_HodgeDefaultExperience") || (GEditor && GEditor->PlayWorld)) { return false; }
	UObject* CDO = Experience->GeneratedClass->GetDefaultObject();
	auto* Property = FindFProperty<FArrayProperty>(CDO->GetClass(), TEXT("Actions"));
	if (!Property) { return false; }
	FScriptArrayHelper Actions(Property, Property->ContainerPtrToValuePtr<void>(CDO));
	for (int32 I = 0; I < Actions.Num(); ++I)
	{
		auto* Action = Cast<UGameFeatureAction_AddComponents>(CastFieldChecked<FObjectPropertyBase>(Property->Inner)->GetObjectPropertyValue(Actions.GetRawPtr(I)));
		if (!Action) { continue; }
		for (const auto& Entry : Action->ComponentList)
		{ if (Entry.ActorClass.Get() == ActorClass && Entry.ComponentClass.Get() == ComponentClass) { return true; } }
		Experience->Modify(); CDO->Modify(); Action->Modify();
		FGameFeatureComponentEntry Entry; Entry.ActorClass = ActorClass.Get(); Entry.ComponentClass = ComponentClass.Get();
		Entry.bClientComponent = true; Entry.bServerComponent = true;
		Action->ComponentList.Add(Entry);
		FBlueprintEditorUtils::MarkBlueprintAsModified(Experience);
		return true;
	}
	return false;
}

/** 配置翅膀Finisher。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureWingFinisher(UAnimMontage* Montage, UHodgeWingProfile* Profile)
{
	if (!Montage || !Profile || !Montage->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/Montages/")) ||
		!Profile->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Hero/Wing/")) || (GEditor && GEditor->PlayWorld)) { return false; }
	for (const auto& Event : Montage->Notifies)
	{
		const auto* Existing = Cast<UHodgeAnimNotifyState_HitCheck>(Event.NotifyStateClass);
		if (Existing && Existing->Hit.Source == EHodgeAnimHitSource::NamedComponent && Existing->Hit.ComponentName == TEXT("WingMesh"))
		{ return Profile->AttackMontage == Montage; }
	}
	FHodgeAnimHitConfig* Primary = nullptr;
	UObject* PrimaryNotify = nullptr;
	float Start = TNumericLimits<float>::Max();
	float End = 0.f;
	for (auto& Event : Montage->Notifies)
	{
		auto* State = Cast<UHodgeAnimNotifyState_HitCheck>(Event.NotifyStateClass);
		auto* Point = Cast<UHodgeAnimNotify_Hit>(Event.Notify);
		FHodgeAnimHitConfig* Hit = State ? &State->Hit : (Point ? &Point->Hit : nullptr);
		if (Hit && Event.GetTime() < Start)
		{
			Primary = Hit; PrimaryNotify = State ? static_cast<UObject*>(State) : Point;
			Start = Event.GetTime(); End = Start + (State ? Event.GetDuration() : .08f);
		}
	}
	if (!Primary || Start < .03f || End <= Start || End + .2f > Montage->GetPlayLength()) { return false; }
	Montage->Modify(); PrimaryNotify->Modify(); Profile->Modify();
	if (Primary->HitGroup.IsNone()) { Primary->HitGroup = TEXT("PoverWingFinisher"); }
	if (Primary->AttackPhase.IsNone()) { Primary->AttackPhase = TEXT("Strike"); }
	FHodgeAnimHitConfig Config = *Primary;
	Config.Source = EHodgeAnimHitSource::NamedComponent;
	Config.ComponentName = TEXT("WingMesh"); Config.BoneOrSocket = TEXT("WingHitTip");
	Config.WeaponSourceTag = FGameplayTag(); Config.Shape = EHodgeHitShape::Sphere; Config.Radius = 18.f;
	Config.LocalTransform = FTransform::Identity; Config.TransformPolicy = EHodgeHitTransformPolicy::Follow;
	Profile->AttackMontage = Montage;
	Profile->WindupStart = FMath::Max(0.f, Start - .22f);
	Profile->StrikeStart = Start - .02f;
	Profile->StrikePeak = (Start + End) * .5f;
	Profile->RecoveryEnd = End + .2f;
	const bool bOK = UHodgeCombatValidationLibrary::AddHitNotify(Montage, Start, End, Config, false);
	Montage->MarkPackageDirty(); Profile->MarkPackageDirty();
	return bOK;
}

/** 导入动画蓝图文本并恢复其中的对象与图节点。 */
UAnimBlueprint* UHodgeAnimationAuthoringLibrary::ImportAnimationBlueprintText(const FString& AssetPath, const FString& TextFilename, UClass* ParentClass, USkeleton* TargetSkeleton)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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

/** 设置动画默认。 */
bool UHodgeAnimationAuthoringLibrary::SetAnimationDefault(UBlueprint* Blueprint, const FString& PropertyName, const FString& Value)
{
	if (!HodgeAnimationAuthoring::IsWorkAsset(Blueprint) || !Blueprint->GeneratedClass) { return false; }
	Blueprint->Modify();
	if (!HodgeAnimationAuthoring::ImportProperty(Blueprint->GeneratedClass->GetDefaultObject(), PropertyName, Value)) { return false; }
	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
	return true;
}

/** 设置动画节点属性。 */
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

/** 按指定映射更新动画图中的资源引用。 */
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

/** 编译当前动画蓝图并返回诊断结果。 */
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

/** 获取骨架骨骼Names。 */
TArray<FName> UHodgeAnimationAuthoringLibrary::GetSkeletonBoneNames(USkeleton* Skeleton)
{
	TArray<FName> Names;
	if (Skeleton)
	{
		for (const FMeshBoneInfo& Bone : Skeleton->GetReferenceSkeleton().GetRefBoneInfo()) { Names.Add(Bone.Name); }
	}
	return Names;
}

/** 设置骨架槽位分组。 */
bool UHodgeAnimationAuthoringLibrary::SetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot, FName Group)
{
	if (!Skeleton || (!Skeleton->GetPathName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/")) &&
		!Skeleton->GetPathName().StartsWith(TEXT("/Game/Main/Character/Enemies/"))) || Slot.IsNone() || Group.IsNone()) { return false; }
	Skeleton->Modify();
	Skeleton->RegisterSlotNode(Slot);
	Skeleton->SetSlotGroupName(Slot, Group);
	Skeleton->MarkPackageDirty();
	return Skeleton->GetSlotGroupName(Slot) == Group;
}

/** 获取骨架槽位分组。 */
FName UHodgeAnimationAuthoringLibrary::GetSkeletonSlotGroup(USkeleton* Skeleton, FName Slot)
{
	return Skeleton ? Skeleton->GetSlotGroupName(Slot) : NAME_None;
}

/** 设置玩家角色受击蒙太奇槽位。 */
bool UHodgeAnimationAuthoringLibrary::SetHeroReactionMontageSlot(UAnimMontage* Montage, FName Slot)
{
	if (!Montage || (!Montage->GetPathName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/HitReactions/")) &&
		!Montage->GetPathName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/Movement/"))) || Slot.IsNone()) { return false; }
	Montage->Modify();
	for (auto& Track : Montage->SlotAnimTracks) { Track.SlotName = Slot; }
	Montage->RefreshCacheData();
	Montage->MarkPackageDirty();
	return !Montage->SlotAnimTracks.IsEmpty();
}

/** 设置玩家角色受击骨架。 */
bool UHodgeAnimationAuthoringLibrary::SetHeroReactionSkeleton(UAnimationAsset* Asset, USkeleton* Skeleton)
{
	if (!Asset || (!Asset->GetPathName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/HitReactions/")) &&
		!Asset->GetPathName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/Movement/"))) ||
		!Skeleton || Skeleton->GetPathName() != TEXT("/Game/Main/Character/Hero/Anim/Model/SK_Pover_LyraLab.SK_Pover_LyraLab")) { return false; }
	if (Asset->GetSkeleton() == Skeleton) { return true; }
	Asset->Modify();
	return Asset->ReplaceSkeleton(Skeleton, false);
}

/** 配置玩家角色命中硬直持续时间。 */
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

/** 配置玩家角色命中受击图。 */
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

/** 配置玩家角色Dash冲刺脚部IK。 */
FString UHodgeAnimationAuthoringLibrary::ConfigureHeroDashFootIK(UAnimBlueprint* LayerBlueprint)
{
	if (!LayerBlueprint || !GEditor || GEditor->PlayWorld ||
		LayerBlueprint->GetPathName() != TEXT("/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase.ABP_Pover_LocomotionBase"))
	{ return TEXT("ERROR: unexpected layer or PIE active"); }
	UEdGraph* Graph = FindObject<UEdGraph>(LayerBlueprint, TEXT("FullBody_SkeletalControls"));
	auto* Main = Graph ? FindObject<UK2Node_CallFunction>(Graph, TEXT("K2Node_CallFunction_0")) : nullptr;
	auto* LegIK = Graph ? FindObject<UEdGraphNode>(Graph, TEXT("AnimGraphNode_LegIK_0")) : nullptr;
	auto* Placement = Graph ? FindObject<UK2Node_CallFunction>(Graph, TEXT("K2Node_CallFunction_7")) : nullptr;
	if (!Main || !LegIK || !Placement || !Main->FindPin(TEXT("ReturnValue")) || !LegIK->FindPin(TEXT("Alpha")) || !Placement->FindPin(TEXT("A")))
	{ return TEXT("ERROR: original IK graph anchors missing"); }
	const FScopedTransaction Transaction(NSLOCTEXT("HodgeAnimation", "DashFootIK", "Keep Dash foot placement continuous"));
	LayerBlueprint->Modify(); Graph->Modify(); LegIK->Modify(); Placement->Modify();
	auto* Get = HodgeAnimationAuthoring::FindOrAddPoseNode<UK2Node_VariableGet>(Graph, TEXT("HodgeDashFootIKAlpha"), 200, -350);
	Get->VariableReference.SetExternalMember(TEXT("LocomotionFootIKAlpha"), UHodgeAnimInstance::StaticClass()); Get->ReconstructNode();
	auto* Self = Get->FindPin(UEdGraphSchema_K2::PN_Self); auto* Value = Get->FindPin(TEXT("LocomotionFootIKAlpha"));
	if (!Self || !Value || !Graph->GetSchema()->TryCreateConnection(Main->FindPin(TEXT("ReturnValue")), Self))
	{ return TEXT("ERROR: cannot bind main IK snapshot"); }
	for (UEdGraphPin* Target : {LegIK->FindPin(TEXT("Alpha")), Placement->FindPin(TEXT("A"))})
	{
		Graph->GetSchema()->BreakPinLinks(*Target, true);
		if (!Graph->GetSchema()->TryCreateConnection(Value, Target)) { return TEXT("ERROR: cannot bind IK alpha"); }
	}
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(LayerBlueprint);
	return TEXT("OK DashFootIK=continuous OtherFullBody=weighted");
}

/** 配置玩家角色移动蒙太奇同步。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureHeroMovementMontageSync(UAnimMontage* Montage)
{
	if (!Montage || !GEditor || GEditor->PlayWorld || !Montage->GetPathName().StartsWith(TEXT("/Game/Main/Character/Hero/Anim/Movement/"))) { return false; }
	Montage->Modify(); Montage->SyncGroup = TEXT("HodgeSprint"); Montage->SyncSlotIndex = 0;
	Montage->PostEditChange(); Montage->MarkPackageDirty();
	return Montage->MarkerData.AuthoredSyncMarkers.Num() >= 2;
}

/** 配置玩家角色Sprint奔跑同步。 */
FString UHodgeAnimationAuthoringLibrary::ConfigureHeroSprintSync(UAnimBlueprint* LayerBlueprint)
{
	if (!LayerBlueprint || !GEditor || GEditor->PlayWorld || LayerBlueprint->GetPathName() !=
		TEXT("/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase.ABP_Pover_LocomotionBase")) { return TEXT("ERROR: unexpected layer or PIE"); }
	UEdGraph* Graph = FindObject<UEdGraph>(LayerBlueprint, TEXT("FullBody_CycleState"));
	if (!Graph) { return TEXT("ERROR: linked cycle graph missing"); }
	if (FindObject<UEdGraphNode>(Graph, TEXT("HodgeSprintCycleBlend"))) { return TEXT("OK dedicated Sprint cycle already configured"); }
	UAnimGraphNode_Base* Original = nullptr;
	for (UEdGraphNode* Node : Graph->Nodes)
	{ if (Node->GetClass()->GetName().Contains(TEXT("SequencePlayer"))) { Original = Cast<UAnimGraphNode_Base>(Node); break; } }
	auto* OriginalPose = Original ? Original->FindPin(TEXT("Pose")) : nullptr;
	if (!OriginalPose || OriginalPose->LinkedTo.IsEmpty()) { return TEXT("ERROR: cycle pose output missing"); }
	const FScopedTransaction Transaction(NSLOCTEXT("HodgeAnimation", "SprintSync", "Synchronize Sprint foot phase"));
	LayerBlueprint->Modify(); Graph->Modify(); Original->Modify();
	const auto Destinations = OriginalPose->LinkedTo;
	auto* Sprint = CastChecked<UAnimGraphNode_Base>(StaticDuplicateObject(Original, Graph, TEXT("HodgeSprintCyclePlayer")));
	Graph->AddNode(Sprint, false, false); Sprint->CreateNewGuid(); Sprint->NodePosY += 350;
	for (UEdGraphPin* Pin : Sprint->Pins) { Pin->LinkedTo.Reset(); }
	if (!HodgeAnimationAuthoring::ImportProperty(Sprint, TEXT("Node.GroupName"), UHodgeAnimInstance::ResolveCycleSyncGroup(true).ToString()) ||
		!HodgeAnimationAuthoring::ImportProperty(Sprint, TEXT("Node.GroupRole"), TEXT("CanBeLeader")) ||
		!HodgeAnimationAuthoring::ImportProperty(Sprint, TEXT("Node.Method"), TEXT("SyncGroup"))) { return TEXT("ERROR: Sprint sync properties missing"); }
	Sprint->ReconstructNode();
	auto* Blend = HodgeAnimationAuthoring::FindOrAddPoseNode<UAnimGraphNode_BlendListByBool>(Graph, TEXT("HodgeSprintCycleBlend"), Original->NodePosX + 350, Original->NodePosY);
	HodgeAnimationAuthoring::ImportProperty(Blend, TEXT("Node.BlendTime"), TEXT("(0.12,0.16)")); Blend->ReconstructNode();
	auto* Main = HodgeAnimationAuthoring::FindOrAddPoseNode<UK2Node_CallFunction>(Graph, TEXT("HodgeSyncMain"), -1100, -650);
	Main->SetFromFunction(LayerBlueprint->GeneratedClass->FindFunctionByName(TEXT("GetMainAnimBPThreadSafe"))); Main->ReconstructNode();
	auto* Mode = HodgeAnimationAuthoring::FindOrAddPoseNode<UK2Node_VariableGet>(Graph, TEXT("HodgeCycleSyncSprint"), -800, -650);
	Mode->VariableReference.SetExternalMember(TEXT("bSprinting"), UHodgeAnimInstance::StaticClass()); Mode->ReconstructNode();
	const auto* Schema = Graph->GetSchema(); Schema->BreakPinLinks(*OriginalPose, true);
	if (!Schema->TryCreateConnection(OriginalPose, Blend->FindPin(TEXT("BlendPose_1"))) ||
		!Schema->TryCreateConnection(Sprint->FindPin(TEXT("Pose")), Blend->FindPin(TEXT("BlendPose_0"))) ||
		!Schema->TryCreateConnection(Main->GetReturnValuePin(), Mode->FindPin(UEdGraphSchema_K2::PN_Self)) ||
		!Schema->TryCreateConnection(Mode->FindPin(TEXT("bSprinting")), Blend->FindPin(TEXT("bActiveValue")))) { return TEXT("ERROR: Sprint pose blend wiring failed"); }
	for (auto* Destination : Destinations)
	{ if (!Schema->TryCreateConnection(Blend->FindPin(TEXT("Pose")), Destination)) { return TEXT("ERROR: cycle blend output failed"); } }
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(LayerBlueprint);
	return TEXT("OK DedicatedSprintGroup=HodgeSprint OriginalLocomotion=preserved");
}

/** 配置玩家角色朝向Modes。 */
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

/** 配置玩家角色Sprint奔跑Animations。 */
FString UHodgeAnimationAuthoringLibrary::ConfigureHeroSprintAnimations(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint)
{
	if (!MainBlueprint || !LayerBlueprint || !GEditor || GEditor->PlayWorld ||
		MainBlueprint->GetPathName() != TEXT("/Game/Main/Character/Hero/Anim/ABP_Pover_Base.ABP_Pover_Base") ||
		LayerBlueprint->GetPathName() != TEXT("/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase.ABP_Pover_LocomotionBase"))
	{ return TEXT("ERROR: unexpected asset or PIE active"); }
	MainBlueprint->Modify(); LayerBlueprint->Modify();
	auto ReadMain = [LayerBlueprint](UEdGraph* Graph, FName Property) -> UEdGraphPin*
	{
		using namespace HodgeAnimationAuthoring;
		auto* Call = FindOrAddPoseNode<UK2Node_CallFunction>(Graph, TEXT("HodgeSprintMainAnim"), -1200, -500);
		UFunction* Function = LayerBlueprint->GeneratedClass ? LayerBlueprint->GeneratedClass->FindFunctionByName(TEXT("GetMainAnimBPThreadSafe")) : nullptr;
		if (!Function) { return nullptr; }
		Call->SetFromFunction(Function); Call->ReconstructNode();
		auto* Get = FindOrAddPoseNode<UK2Node_VariableGet>(Graph, FName(TEXT("HodgeSprint_") + Property.ToString()), -1000, -500);
		Get->VariableReference.SetExternalMember(Property, UHodgeAnimInstance::StaticClass()); Get->ReconstructNode();
		auto* Object = Call->FindPin(TEXT("ReturnValue")); auto* Self = Get->FindPin(UEdGraphSchema_K2::PN_Self);
		if (!Object || !Self || (!Object->LinkedTo.Contains(Self) && !Graph->GetSchema()->TryCreateConnection(Object, Self))) { return nullptr; }
		return Get->FindPin(Property);
	};
	int32 Cycles = 0, Pivots = 0, Gates = 0;
	TArray<UEdGraph*> Graphs; LayerBlueprint->GetAllGraphs(Graphs);
	for (auto* Graph : Graphs)
	{
		const auto Nodes = Graph->Nodes;
		for (UEdGraphNode* Node : Nodes)
		{
			auto* Select = Cast<UK2Node_Select>(Node);
			if (!Select || !Select->GetEnum() || Select->GetEnum()->GetName() != TEXT("AnimEnum_CardinalDirection")) { continue; }
			const bool bCycle = Graph->GetFName() == TEXT("UpdateCycleAnim");
			const bool bPivot = Graph->GetName().Contains(TEXT("Pivot"));
			if (!bCycle && !bPivot) { continue; }
			if (bCycle) { ++Cycles; } else { ++Pivots; }
			const FName Name(TEXT("HodgeSprintSequence_") + Node->GetName());
			if (FindObject<UK2Node_Select>(Graph, *Name.ToString())) { continue; }
			auto* Mode = ReadMain(Graph, TEXT("bSprinting"));
			auto* Sequence = ReadMain(Graph, bCycle ? TEXT("SprintCycle") : TEXT("SprintTurn"));
			if (!Mode || !Sequence) { return TEXT("ERROR: sprint snapshot binding failed"); }
			auto* Original = Select->GetReturnValuePin(); const auto Destinations = Original->LinkedTo;
			auto* Branch = HodgeAnimationAuthoring::FindOrAddPoseNode<UK2Node_Select>(Graph, Name, Node->NodePosX + 250, Node->NodePosY);
			HodgeAnimationAuthoring::ImportProperty(Branch, TEXT("IndexPinType"), TEXT("(PinCategory=\"bool\",PinSubCategory=\"\")")); Branch->ReconstructNode();
			TArray<UEdGraphPin*> Options; Branch->GetOptionPins(Options);
			if (Options.Num() != 2) { return TEXT("ERROR: sprint resource selector invalid"); }
			for (auto* Pin : Options) { Pin->PinType = Original->PinType; }
			Branch->GetReturnValuePin()->PinType = Original->PinType;
			const auto* Schema = Graph->GetSchema(); Schema->BreakPinLinks(*Original, true);
			if (!Schema->TryCreateConnection(Original, Options[0]) || !Schema->TryCreateConnection(Sequence, Options[1]) ||
				!Schema->TryCreateConnection(Mode, Branch->GetIndexPin())) { return TEXT("ERROR: sprint resource selector connection failed"); }
			for (auto* Destination : Destinations)
			{ if (!Schema->TryCreateConnection(Branch->GetReturnValuePin(), Destination)) { return TEXT("ERROR: sprint output connection failed"); } }
		}
	}
	Graphs.Reset(); MainBlueprint->GetAllGraphs(Graphs);
	for (auto* Graph : Graphs)
	{
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			auto* Get = Cast<UK2Node_VariableGet>(Node);
			if (!Get || Get->VariableReference.GetMemberName() != TEXT("EnablePivot")) { continue; }
			auto* Original = Get->FindPin(TEXT("EnablePivot"));
			if (!Original) { return TEXT("ERROR: Pivot gate output missing"); }
			const auto Destinations = Original->LinkedTo; Graph->GetSchema()->BreakPinLinks(*Original, true);
			Get->Modify(); Get->VariableReference.SetSelfMember(TEXT("bSprintPivotAllowed"), FGuid()); Get->ReconstructNode();
			auto* Output = Get->FindPin(TEXT("bSprintPivotAllowed"));
			if (!Output) { return TEXT("ERROR: Sprint Pivot output missing"); }
			for (auto* Destination : Destinations)
			{ if (!Graph->GetSchema()->TryCreateConnection(Output, Destination)) { return TEXT("ERROR: Sprint Pivot gate connection failed"); } }
			++Gates;
		}
	}
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainBlueprint); FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(LayerBlueprint);
	return FString::Printf(TEXT("OK Cycles=%d Pivots=%d Gates=%d"), Cycles, Pivots, Gates);
}

/** 配置玩家角色Sprint奔跑掉头Timing。 */
FString UHodgeAnimationAuthoringLibrary::ConfigureHeroSprintPivotTiming(UAnimBlueprint* MainBlueprint, UAnimBlueprint* LayerBlueprint)
{
	if (!MainBlueprint || MainBlueprint->GetPathName() != TEXT("/Game/Main/Character/Hero/Anim/ABP_Pover_Base.ABP_Pover_Base") ||
		!LayerBlueprint || !GEditor || GEditor->PlayWorld || LayerBlueprint->GetPathName() !=
		TEXT("/Game/Main/Character/Hero/Anim/Layer/ABP_Pover_LocomotionBase.ABP_Pover_LocomotionBase")) { return TEXT("ERROR: unexpected layer or PIE"); }
	LayerBlueprint->Modify();
	TArray<UEdGraph*> Graphs; LayerBlueprint->GetAllGraphs(Graphs);
	int32 Evaluators = 0;
	for (auto* Graph : Graphs)
	{
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (!Node->GetClass()->GetName().Contains(TEXT("SequenceEvaluator")) || (Graph->GetFName() != TEXT("PivotA") && Graph->GetFName() != TEXT("PivotB"))) { continue; }
			HodgeAnimationAuthoring::ImportProperty(Node, TEXT("Node.GroupName"), TEXT("None"));
			HodgeAnimationAuthoring::ImportProperty(Node, TEXT("Node.Method"), TEXT("DoNotSync"));
			HodgeAnimationAuthoring::ImportProperty(Node, TEXT("Node.BecomeRelevantFunction.FunctionName"), TEXT("SetUpPivotAnim"));
			++Evaluators;
		}
		const bool bCycle = Graph->GetFName() == TEXT("UpdateCycleAnim");
		const FName AdvanceName = bCycle ? TEXT("HodgeAdvanceSprintCycle") : TEXT("HodgeAdvanceSprintPivot");
		if ((!bCycle && Graph->GetFName() != TEXT("UpdatePivotAnim")) || FindObject<UK2Node_CallFunction>(Graph, *AdvanceName.ToString())) { continue; }
		UK2Node_FunctionEntry* Entry = nullptr;
		for (UEdGraphNode* Node : Graph->Nodes) { if (auto* Candidate = Cast<UK2Node_FunctionEntry>(Node)) { Entry = Candidate; break; } }
		if (!Entry) { return TEXT("ERROR: Pivot update entry missing"); }
		const auto* Schema = Graph->GetSchema();
		auto* Exec = Entry->FindPin(UEdGraphSchema_K2::PN_Then);
		auto* Context = Entry->FindPin(TEXT("Context")); auto* Ref = Entry->FindPin(TEXT("Node"));
		if (!Exec || !Context || !Ref) { return TEXT("ERROR: Pivot callback inputs missing"); }
		const auto OldExec = Exec->LinkedTo;
		auto* Main = HodgeAnimationAuthoring::FindOrAddPoseNode<UK2Node_CallFunction>(Graph, TEXT("HodgePivotMainAnim"), -1800, -400);
		Main->SetFromFunction(LayerBlueprint->GeneratedClass->FindFunctionByName(TEXT("GetMainAnimBPThreadSafe"))); Main->ReconstructNode();
		auto* Advance = HodgeAnimationAuthoring::FindOrAddPoseNode<UK2Node_CallFunction>(Graph, AdvanceName, -1600, -400);
		Advance->SetFromFunction(UHodgeAnimInstance::StaticClass()->FindFunctionByName(bCycle ? TEXT("UpdateSprintCycle") : TEXT("UpdateSprintPivot"))); Advance->ReconstructNode();
		auto* Branch = HodgeAnimationAuthoring::FindOrAddPoseNode<UK2Node_IfThenElse>(Graph, TEXT("HodgePivotTimingBranch"), -1300, -400);
		Schema->BreakPinLinks(*Exec, true);
		if (!Schema->TryCreateConnection(Exec, Advance->GetExecPin()) ||
			!Schema->TryCreateConnection(Main->GetReturnValuePin(), Advance->FindPin(UEdGraphSchema_K2::PN_Self)) ||
			!Schema->TryCreateConnection(Context, Advance->FindPin(TEXT("Context"))) || !Schema->TryCreateConnection(Ref, Advance->FindPin(TEXT("Node"))) ||
			!Schema->TryCreateConnection(Advance->GetThenPin(), Branch->GetExecPin()) ||
			!Schema->TryCreateConnection(Advance->GetReturnValuePin(), Branch->GetConditionPin())) { return TEXT("ERROR: Pivot timing connection failed"); }
		for (auto* Destination : OldExec) { if (!Schema->TryCreateConnection(Branch->GetElsePin(), Destination)) { return TEXT("ERROR: legacy Pivot restoration failed"); } }
		// True 路径结束本次更新，False 保留原八向距离匹配。
	}
	MainBlueprint->Modify(); Graphs.Reset(); MainBlueprint->GetAllGraphs(Graphs);
	int32 Exits = 0;
	for (auto* Graph : Graphs)
	{
		// 此已有出口通往 Cycle，保留 LinkedLayerChanged 并补上单一 Turn 的时间出口。
		if (!Graph->GetPathName().Contains(TEXT("LocomotionSM.AnimStateTransitionNode_19.Transition"))) { continue; }
		if (FindObject<UK2Node_CallFunction>(Graph, TEXT("HodgeSprintPivotExit"))) { ++Exits; continue; }
		UAnimGraphNode_TransitionResult* Result = nullptr;
		for (UEdGraphNode* Node : Graph->Nodes) { if (auto* Candidate = Cast<UAnimGraphNode_TransitionResult>(Node)) { Result = Candidate; break; } }
		auto* Output = Result ? Result->FindPin(TEXT("bCanEnterTransition")) : nullptr;
		if (!Output || Output->LinkedTo.Num() != 1) { return TEXT("ERROR: Pivot Cycle exit missing"); }
		auto* Original = Output->LinkedTo[0]; const auto* Schema = Graph->GetSchema();
		auto* Exit = HodgeAnimationAuthoring::FindOrAddPoseNode<UK2Node_CallFunction>(Graph, TEXT("HodgeSprintPivotExit"), -600, 200);
		Exit->SetFromFunction(UHodgeAnimInstance::StaticClass()->FindFunctionByName(TEXT("ShouldExitSprintPivot"))); Exit->ReconstructNode();
		auto* Or = HodgeAnimationAuthoring::FindOrAddPoseNode<UK2Node_CallFunction>(Graph, TEXT("HodgeSprintPivotExitOr"), -300, 200);
		Or->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(TEXT("BooleanOR"))); Or->ReconstructNode();
		Schema->BreakPinLinks(*Output, true);
		if (!Schema->TryCreateConnection(Original, Or->FindPin(TEXT("A"))) || !Schema->TryCreateConnection(Exit->GetReturnValuePin(), Or->FindPin(TEXT("B"))) ||
			!Schema->TryCreateConnection(Or->GetReturnValuePin(), Output)) { return TEXT("ERROR: Pivot Cycle timing exit connection failed"); }
		++Exits;
	}
	if (Exits != 1) { return TEXT("ERROR: expected one Pivot Cycle timing exit"); }
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainBlueprint); FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(LayerBlueprint);
	return FString::Printf(TEXT("OK PivotEvaluators=%d TimingExits=%d"), Evaluators, Exits);
}

/** 配置动画实验场景PIE。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabPIE(int32 PlayerCount, bool bListenServer)
{
	if (!GEditor || GEditor->PlayWorld || PlayerCount < 1 || PlayerCount > 2 ||
		!HodgeAnimationAuthoring::IsWorkAsset(GEditor->GetEditorWorldContext().World())) { return false; }
	ULevelEditorPlaySettings* Settings = GetMutableDefault<ULevelEditorPlaySettings>();
	Settings->SetPlayNumberOfClients(PlayerCount);
	Settings->SetPlayNetMode(bListenServer ? PIE_ListenServer : PIE_Standalone);
	return true;
}

/** 配置动画实验场景游戏Mode。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureAnimationLabGameMode(UBlueprint* Blueprint, TSubclassOf<APawn> PawnClass)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
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


/** 设置敌人网格骨架。 */
bool UHodgeAnimationAuthoringLibrary::SetEnemyMeshSkeleton(USkeletalMesh* Mesh, USkeleton* Skeleton)
{
	if (!Mesh || !Skeleton || !Mesh->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Enemies/"))) { return false; }
	Mesh->Modify(); Mesh->SetSkeleton(Skeleton); Mesh->PostEditChange(); return true;
}

/** 配置敌人行为树。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureEnemyBehaviorTree(UBehaviorTree* Tree, UBlackboardData* BB, FGameplayTag AttackTag, bool bComboIntent)
{
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Tree || !BB || !AttackTag.IsValid() || !Tree->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Enemies/"))) { return false; }
	Tree->Modify(); BB->Modify(); BB->Keys.Reset();
	auto Key = [BB](FName Name, UClass* Type)
	{
		FBlackboardEntry Entry; Entry.EntryName = Name; Entry.KeyType = NewObject<UBlackboardKeyType>(BB, Type);
		if (auto* ObjectKey = Cast<UBlackboardKeyType_Object>(Entry.KeyType)) { ObjectKey->BaseClass = AActor::StaticClass(); }
		BB->Keys.Add(Entry);
	};
	Key(TEXT("TargetActor"), UBlackboardKeyType_Object::StaticClass());
	Key(TEXT("LastKnownLocation"), UBlackboardKeyType_Vector::StaticClass());
	for (FName Name : {FName(TEXT("CanAct")), FName(TEXT("HasLineOfSight")), FName(TEXT("HasLastKnownLocation")), FName(TEXT("InAttackRange"))})
	{ Key(Name, UBlackboardKeyType_Bool::StaticClass()); }
	BB->PostEditChange(); Tree->BlackboardAsset = BB;
	auto* Root = NewObject<UBTComposite_Selector>(Tree);
	// 统一战斗组件。
	auto* Combat = NewObject<UBTComposite_Selector>(Tree);
	auto* Attack = NewObject<UBTComposite_Sequence>(Tree);
	Tree->RootNode = Root;
	auto Attach = [](UBTCompositeNode* Parent, UBTNode* Node) -> FBTCompositeChild&
	{
		FBTCompositeChild Child; Child.ChildComposite = Cast<UBTCompositeNode>(Node); Child.ChildTask = Cast<UBTTaskNode>(Node);
		return Parent->Children.Add_GetRef(Child);
	};
	auto Condition = [Tree, BB](FBTCompositeChild& Child, FName Name, EBTFlowAbortMode::Type Abort)
	{
		auto* Decorator = NewObject<UBTDecorator_Blackboard>(Tree);
		auto* Property = FindFProperty<FStructProperty>(Decorator->GetClass(), TEXT("BlackboardKey"));
		auto* Selector = Property->ContainerPtrToValuePtr<FBlackboardKeySelector>(Decorator);
		Selector->SelectedKeyName = Name; Selector->ResolveSelectedKey(*BB);
		FindFProperty<FByteProperty>(Decorator->GetClass(), TEXT("FlowAbortMode"))->SetPropertyValue_InContainer(Decorator, uint8(Abort));
		FindFProperty<FByteProperty>(Decorator->GetClass(), TEXT("OperationType"))->SetPropertyValue_InContainer(Decorator, 0);
		static_cast<UBTNode*>(Decorator)->InitializeFromAsset(*Tree);
		Decorator->BuildDescription(); Child.Decorators.Add(Decorator);
	};
	Condition(Attach(Root, Combat), TEXT("CanAct"), EBTFlowAbortMode::Both);
	Attach(Root, NewObject<UBTTask_Wait>(Tree));
	Condition(Attach(Combat, Attack), TEXT("InAttackRange"), EBTFlowAbortMode::LowerPriority);
	auto* Ability = NewObject<UHodgeBTTask_ExecuteAbility>(Tree);
	Ability->RequestType = bComboIntent ? EHodgeCombatActionRequestType::ComboIntent : EHodgeCombatActionRequestType::StandaloneAbility;
	if (bComboIntent) { Ability->IntentTag = AttackTag; } else { Ability->AbilityTag = AttackTag; }
	Ability->TargetActor.SelectedKeyName = TEXT("TargetActor");
	Attach(Attack, Ability);
	auto* Recovery = NewObject<UBTTask_Wait>(Tree); Recovery->WaitTime = .65f; Recovery->RandomDeviation = .05f; Attach(Attack, Recovery);
	auto Move = [Tree, BB](FName Name)
	{
		auto* Task = NewObject<UBTTask_MoveTo>(Tree);
		auto* Selector = FindFProperty<FStructProperty>(Task->GetClass(), TEXT("BlackboardKey"))->ContainerPtrToValuePtr<FBlackboardKeySelector>(Task);
		Selector->SelectedKeyName = Name; Selector->ResolveSelectedKey(*BB);
		Task->AcceptableRadius = 130.f;
		Task->bReachTestIncludesAgentRadius = false;
		Task->bReachTestIncludesGoalRadius = false;
		return Task;
	};
	Condition(Attach(Combat, Move(TEXT("TargetActor"))), TEXT("HasLineOfSight"), EBTFlowAbortMode::Both);
	Condition(Attach(Combat, Move(TEXT("LastKnownLocation"))), TEXT("HasLastKnownLocation"), EBTFlowAbortMode::Both);
	Attach(Combat, NewObject<UBTTask_Wait>(Tree));
	// 从运行节点重建可编辑图，保存后不会出现空图覆盖运行树。
	auto* Graph = Cast<UBehaviorTreeGraph>(FBlueprintEditorUtils::CreateNewGraph(Tree, TEXT("BehaviorTreeGraph"), UBehaviorTreeGraph::StaticClass(), UEdGraphSchema_BehaviorTree::StaticClass()));
	Tree->BTGraph = Graph; Graph->LockUpdates(); Graph->GetSchema()->CreateDefaultNodesForGraph(*Graph);
	Graph->OnCreated(); Graph->Initialize(); Graph->UnlockUpdates(); Graph->UpdateAsset();
	Tree->MarkPackageDirty(); BB->MarkPackageDirty(); return Tree->RootNode != nullptr;
}

/** 配置敌人动画。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureEnemyAnimation(UAnimBlueprint* Blueprint, UAnimSequence* Idle, UAnimSequence* Run, FName SpineBone)
{
	if (!Blueprint || !Idle || !Run || !Blueprint->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Enemies/"))) { return false; }
	UEdGraph* Graph = nullptr;
	for (UEdGraph* Candidate : Blueprint->FunctionGraphs) { if (Candidate && Candidate->GetFName() == TEXT("AnimGraph")) { Graph = Candidate; break; } }
	if (!Graph) { return false; }
	using namespace HodgeAnimationAuthoring;
	auto* Root = FindOrAddPoseNode<UAnimGraphNode_Root>(Graph, TEXT("EnemyRoot"), 1500, 0);
	for (UEdGraphNode* Node : Graph->Nodes)
	{ if (Node != Root && Node->IsA<UAnimGraphNode_Root>()) { FBlueprintEditorUtils::RemoveNode(Blueprint, Node, true); break; } }
	auto* IdleNode = FindOrAddPoseNode<UAnimGraphNode_SequencePlayer>(Graph, TEXT("EnemyIdle"), 0, 250);
	auto* RunNode = FindOrAddPoseNode<UAnimGraphNode_SequencePlayer>(Graph, TEXT("EnemyRun"), 0, 0);
	IdleNode->Node.SetSequence(Idle); RunNode->Node.SetSequence(Run);
	auto* Blend = FindOrAddPoseNode<UAnimGraphNode_BlendListByBool>(Graph, TEXT("EnemyMovement"), 300, 100);
	auto* PosesProperty = FindFProperty<FArrayProperty>(FAnimNode_BlendListBase::StaticStruct(), TEXT("BlendPose"));
	FScriptArrayHelper Poses(PosesProperty, PosesProperty->ContainerPtrToValuePtr<void>(&Blend->Node)); Poses.Resize(2);
	auto* TimesProperty = FindFProperty<FArrayProperty>(FAnimNode_BlendListBase::StaticStruct(), TEXT("BlendTime"));
	FScriptArrayHelper Times(TimesProperty, TimesProperty->ContainerPtrToValuePtr<void>(&Blend->Node)); Times.Resize(2);
	for (int32 I = 0; I < 2; ++I) { CastFieldChecked<FFloatProperty>(TimesProperty->Inner)->SetPropertyValue(Times.GetRawPtr(I), .15f); }
	Blend->ReconstructNode();
	auto* Moving = FindOrAddPoseNode<UK2Node_VariableGet>(Graph, TEXT("EnemyMoving"), 0, -250);
	Moving->VariableReference.SetSelfMember(TEXT("bEnemyMoving")); Moving->ReconstructNode();
	auto* Full = FindOrAddPoseNode<UAnimGraphNode_Slot>(Graph, TEXT("EnemyFullBody"), 600, 100); Full->Node.SlotName = TEXT("FullBobySlot");
	auto* Cache = FindOrAddPoseNode<UAnimGraphNode_SaveCachedPose>(Graph, TEXT("EnemyBaseCache"), 800, 100); Cache->CacheName = TEXT("EnemyBase");
	auto* Base = FindOrAddPoseNode<UAnimGraphNode_UseCachedPose>(Graph, TEXT("EnemyBaseUse"), 950, 0); Base->SaveCachedPoseNode = Cache;
	auto* FeedbackBase = FindOrAddPoseNode<UAnimGraphNode_UseCachedPose>(Graph, TEXT("EnemyFeedbackBase"), 600, 350); FeedbackBase->SaveCachedPoseNode = Cache;
	auto* Upper = FindOrAddPoseNode<UAnimGraphNode_Slot>(Graph, TEXT("EnemyLightFeedback"), 800, 350); Upper->Node.SlotName = TEXT("UpperBobySlot");
	auto* Bone = FindOrAddPoseNode<UAnimGraphNode_LayeredBoneBlend>(Graph, TEXT("EnemyUpperBlend"), 1150, 100);
	Bone->Node.BlendPoses.SetNum(1); Bone->Node.BlendWeights.SetNum(1); Bone->Node.LayerSetup.SetNum(1); Bone->Node.BlendWeights[0] = 1.f;
	FBranchFilter Filter; Filter.BoneName = SpineBone; Filter.BlendDepth = 3; Bone->Node.LayerSetup[0].BranchFilters = {Filter}; Bone->ReconstructNode();
	const auto* Schema = Graph->GetSchema();
	auto Link = [Schema](UEdGraphNode* A, const TCHAR* Out, UEdGraphNode* B, const TCHAR* In)
	{ auto* X = A->FindPin(Out); auto* Y = B->FindPin(In); return X && Y && (X->LinkedTo.Contains(Y) || Schema->TryCreateConnection(X, Y)); };
	const bool bOK = Link(Moving, TEXT("bEnemyMoving"), Blend, TEXT("bActiveValue")) &&
		Link(RunNode, TEXT("Pose"), Blend, TEXT("BlendPose_0")) && Link(IdleNode, TEXT("Pose"), Blend, TEXT("BlendPose_1")) &&
		Link(Blend, TEXT("Pose"), Full, TEXT("Source")) && Link(Full, TEXT("Pose"), Cache, TEXT("Pose")) &&
		Link(FeedbackBase, TEXT("Pose"), Upper, TEXT("Source")) && Link(Base, TEXT("Pose"), Bone, TEXT("BasePose")) && Link(Upper, TEXT("Pose"), Bone, TEXT("BlendPoses_0")) &&
		Link(Bone, TEXT("Pose"), Root, TEXT("Result"));
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	return bOK;
}


/** 配置敌人动画资产。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureEnemyAnimationAsset(UAnimationAsset* Asset, USkeleton* Skeleton, FName Slot)
{
	if (!Asset || !Skeleton || !Asset->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Enemies/")) ||
		!Skeleton->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Character/Enemies/"))) { return false; }
	Asset->Modify();
	if (Asset->GetSkeleton() != Skeleton && !Asset->ReplaceSkeleton(Skeleton, false)) { return false; }
	if (auto* Montage = Cast<UAnimMontage>(Asset); Montage && !Slot.IsNone())
	{
		for (auto& Track : Montage->SlotAnimTracks) { Track.SlotName = Slot; }
		Montage->RefreshCacheData();
	}
	Asset->MarkPackageDirty(); return true;
}


/** 配置敌人导航边界。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureEnemyNavigationBounds(ANavMeshBoundsVolume* Volume, FVector Size)
{
	if (!Volume || !Volume->GetWorld() || Volume->GetWorld()->WorldType != EWorldType::Editor ||
		!Volume->GetOutermost()->GetName().StartsWith(TEXT("/Game/Main/Maps/L_EnemyAIValidation")) ||
		Size.ContainsNaN() || Size.GetMin() <= 0.f) { return false; }
	Volume->Modify();
	// 先检查对象与上下文，避免在依赖失效后继续处理。
	if (!Volume->Brush) { Volume->Brush = NewObject<UModel>(Volume, NAME_None, RF_Transactional); Volume->Brush->Initialize(nullptr, true); }
	Volume->GetBrushComponent()->Brush = Volume->Brush;
	auto* Builder = NewObject<UCubeBuilder>(Volume);
	Builder->X = Size.X; Builder->Y = Size.Y; Builder->Z = Size.Z;
	Volume->BrushBuilder = Builder;
	if (!static_cast<UBrushBuilder*>(Builder)->Build(Volume->GetWorld(), Volume)) { return false; }
	Volume->PostEditChange();
	if (auto* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Volume->GetWorld()))
	{ Nav->OnNavigationBoundsUpdated(Volume); Nav->Build(); }
	return Volume->GetComponentsBoundingBox(true).GetSize().GetMin() > 0.f;
}


/** 配置敌人Experience配置Components。 */
bool UHodgeAnimationAuthoringLibrary::ConfigureEnemyExperienceComponents(UBlueprint* Experience)
{
	if (!Experience || !Experience->GeneratedClass || Experience->GetOutermost()->GetName() != TEXT("/Game/Main/Experiences/Exp_HodgeDefaultExperience")) { return false; }
	auto* CDO = Experience->GeneratedClass->GetDefaultObject();
	auto* Property = FindFProperty<FArrayProperty>(CDO->GetClass(), TEXT("Actions"));
	if (!Property) { return false; }
	FScriptArrayHelper Actions(Property, Property->ContainerPtrToValuePtr<void>(CDO));
	for (int32 I = 0; I < Actions.Num(); ++I)
	{
		auto* Action = Cast<UGameFeatureAction_AddComponents>(CastFieldChecked<FObjectPropertyBase>(Property->Inner)->GetObjectPropertyValue(Actions.GetRawPtr(I)));
		if (!Action) { continue; }
		for (const auto& Entry : Action->ComponentList)
		{ if (Entry.ActorClass.Get() == AHodgeEnemyCharacter::StaticClass() && Entry.ComponentClass.Get() == UHodgeEquipmentManagerComponent::StaticClass()) { return true; } }
		Experience->Modify(); CDO->Modify(); Action->Modify();
		FGameFeatureComponentEntry Entry; Entry.ActorClass = AHodgeEnemyCharacter::StaticClass();
		Entry.ComponentClass = UHodgeEquipmentManagerComponent::StaticClass(); Entry.bClientComponent = true; Entry.bServerComponent = true;
		Action->ComponentList.Add(Entry);
		FBlueprintEditorUtils::MarkBlueprintAsModified(Experience); return true;
	}
	return false;
}

/** 只维护本功能的一条通知，已有其他通知与轨道保持原样。 */
FString UHodgeAnimationAuthoringLibrary::ConfigureAttackFacingWarp(UHodgeAbilityDefinition* Definition, float StartTime, float EndTime)
{
 if (!Definition || !Definition->ExecutionConfig.Montage || StartTime<0.f || EndTime<=StartTime) { return TEXT("Invalid definition or window"); }
 auto* Montage=Definition->ExecutionConfig.Montage.Get(); Montage->Modify(); Definition->Modify();
 UAnimNotifyState_MotionWarping* State=nullptr; FAnimNotifyEvent* Event=nullptr;
 for (auto& Notify : Montage->Notifies)
 {
  auto* Candidate=Cast<UAnimNotifyState_MotionWarping>(Notify.NotifyStateClass);
  if (Candidate && Candidate->RootMotionModifier && Candidate->RootMotionModifier->IsA<UHodgeRootMotionModifier_AttackFacing>()) { State=Candidate; Event=&Notify; break; }
 }
 if (!State)
 {
  State=NewObject<UAnimNotifyState_MotionWarping>(Montage,NAME_None,RF_Transactional);
  State->RootMotionModifier=NewObject<UHodgeRootMotionModifier_AttackFacing>(State,NAME_None,RF_Transactional);
  Event=&Montage->Notifies.AddDefaulted_GetRef(); Event->NotifyStateClass=State; Event->NotifyName=TEXT("Hodge Attack Facing Warp");
 }
 auto* Modifier=Cast<UHodgeRootMotionModifier_AttackFacing>(State->RootMotionModifier);
 Modifier->WarpTargetName=Definition->FacingWarp.WarpTargetName; Modifier->bWarpTranslation=false; Modifier->bWarpRotation=true;
 Modifier->RotationType=EMotionWarpRotationType::Default; Modifier->RotationMethod=EMotionWarpRotationMethod::SlerpWithClampedRate;
 Modifier->WarpMaxRotationRate=Definition->FacingWarp.MaxRotationRate;
 Event->Link(Montage,StartTime); Event->SetDuration(EndTime-StartTime); Event->EndLink.Link(Montage,EndTime);
 Definition->FacingWarp.bEnabled=true; Montage->RefreshCacheData(); Montage->MarkPackageDirty(); Definition->MarkPackageDirty();
 TArray<FText> Errors; Definition->ValidateDefinition(Errors);
 FString Result; for (const auto& Error : Errors) { Result+=Error.ToString()+TEXT("\n"); }
 return Errors.IsEmpty() ? TEXT("OK") : Result;
}
