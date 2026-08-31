#pragma once

#include "HodgeAlsMovementDirection.generated.h"

UENUM(BlueprintType)
enum class EHodgeAlsMovementDirection : uint8
{
	Forward,
	Backward,
	Left,
	Right
};

USTRUCT(BlueprintType)
struct HODGEPODGE_API FHodgeAlsMovementDirectionCache
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bForward : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bBackward : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bLeft : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HODGEALS")
	uint8 bRight : 1 {false};

public:
	constexpr FHodgeAlsMovementDirectionCache() = default;

	// ReSharper disable once CppNonExplicitConvertingConstructor
	constexpr FHodgeAlsMovementDirectionCache(const EHodgeAlsMovementDirection MovementDirection)
	{
		bForward = MovementDirection == EHodgeAlsMovementDirection::Forward;
		bBackward = MovementDirection == EHodgeAlsMovementDirection::Backward;
		bLeft = MovementDirection == EHodgeAlsMovementDirection::Left;
		bRight = MovementDirection == EHodgeAlsMovementDirection::Right;
	}
};
