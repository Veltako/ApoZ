#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/AZEnums.h"
#include "Types/AZStructs.h"
#include "AZLocomotionDataAsset.generated.h"

UCLASS(BlueprintType)
class AZ_CORE_API UAZLocomotionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion")
	FAZLocomotionConfig LocomotionConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TMap<EAZMovementStance, float> CameraHeights;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TMap<EAZMovementStance, float> TransitionTimes;

	UAZLocomotionDataAsset();
};