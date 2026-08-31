#include "ALS/Nodes/HodgeAlsRigUnits.h"

#include "Engine/World.h"
#include "ALS/Utility/HodgeAlsMath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeAlsRigUnits)

FHodgeAlsRigVMFunction_Clamp01Float_Execute()
{
	Result = UHodgeAlsMath::Clamp01(Value);
}

void FHodgeAlsRigVMFunction_ExponentialDecayVector::Initialize()
{
	bInitialized = false;
}

FHodgeAlsRigVMFunction_ExponentialDecayVector_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (!bInitialized)
	{
		bInitialized = true;
		Current = Target;
	}

	Current = UHodgeAlsMath::ExponentialDecay(Current, Target, UE_REAL_TO_FLOAT(ExecuteContext.GetDeltaTime()), Lambda);
}

void FHodgeAlsRigVMFunction_ExponentialDecayQuaternion::Initialize()
{
	bInitialized = false;
}

FHodgeAlsRigVMFunction_ExponentialDecayQuaternion_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (!bInitialized)
	{
		bInitialized = true;
		Current = Target;
	}

	Current = UHodgeAlsMath::ExponentialDecay(Current, Target, UE_REAL_TO_FLOAT(ExecuteContext.GetDeltaTime()), Lambda);
}

FHodgeAlsRigUnit_CalculatePoleVector_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	const auto* Hierarchy{ExecuteContext.Hierarchy};

	if (!IsValid(Hierarchy) ||
	    !CachedItemA.UpdateCache(ItemA, Hierarchy) ||
	    !CachedItemB.UpdateCache(ItemB, Hierarchy) ||
	    !CachedItemC.UpdateCache(ItemC, Hierarchy))
	{
		return;
	}

	const auto NewItemBLocation{Hierarchy->GetGlobalTransformByIndex(CachedItemB, bInitial).GetLocation()};
	FVector NewItemBProjectionLocation;
	FVector NewPoleDirection;

	if (!UHodgeAlsMath::TryCalculatePoleVector(Hierarchy->GetGlobalTransformByIndex(CachedItemA, bInitial).GetLocation(), NewItemBLocation,
	                                      Hierarchy->GetGlobalTransformByIndex(CachedItemC, bInitial).GetLocation(),
	                                      NewItemBProjectionLocation, NewPoleDirection))
	{
		// Reuse the last successful result if a new pole vector can't be calculated.
		bSuccess = false;
		return;
	}

	ItemBLocation = NewItemBLocation;
	ItemBProjectionLocation = NewItemBProjectionLocation;
	PoleDirection = NewPoleDirection;
	bSuccess = true;
}

FHodgeAlsRigVMFunction_IsGameWorld_Execute()
{
	if (!BlockToRun.IsNone())
	{
		BlockToRun = ControlFlowCompletedName;
		return;
	}

#if WITH_EDITOR
	const auto* World{ExecuteContext.GetWorld()};

	BlockToRun = IsValid(World) && World->IsGameWorld()
		             ? FName{GET_MEMBER_NAME_STRING_VIEW_CHECKED(FHodgeAlsRigVMFunction_IsGameWorld, True)}
		             : FName{GET_MEMBER_NAME_STRING_VIEW_CHECKED(FHodgeAlsRigVMFunction_IsGameWorld, False)};
#else
	BlockToRun = FName{GET_MEMBER_NAME_STRING_VIEW_CHECKED(FHodgeAlsRigVMFunction_IsGameWorld, True)};
#endif
}

const TArray<FName>& FHodgeAlsRigVMFunction_IsGameWorld::GetControlFlowBlocks_Impl() const
{
	static const TArray<FName> Blocks{
		FName{GET_MEMBER_NAME_STRING_VIEW_CHECKED(FHodgeAlsRigVMFunction_IsGameWorld, True)},
		FName{GET_MEMBER_NAME_STRING_VIEW_CHECKED(FHodgeAlsRigVMFunction_IsGameWorld, False)},
		ForLoopCompletedPinName
	};

	return Blocks;
}
