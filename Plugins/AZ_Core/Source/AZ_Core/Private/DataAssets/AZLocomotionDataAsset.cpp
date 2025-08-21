#include "DataAssets/AZLocomotionDataAsset.h"
#include "Utils/AZLogger.h"

UAZLocomotionDataAsset::UAZLocomotionDataAsset()
{
	// Configuration de locomotion par défaut
	LocomotionConfig = FAZLocomotionConfig();

	// Hauteurs de caméra par défaut (en cm depuis le sol)
	CameraHeights.Add(EAZMovementStance::Standing, 175.0f);
	CameraHeights.Add(EAZMovementStance::Crouching, 120.0f);
	CameraHeights.Add(EAZMovementStance::Prone, 30.0f);
	CameraHeights.Add(EAZMovementStance::Sliding, 100.0f);

	// Temps de transition en secondes
	TransitionTimes.Add(EAZMovementStance::Standing, 0.2f);
	TransitionTimes.Add(EAZMovementStance::Crouching, 0.3f);
	TransitionTimes.Add(EAZMovementStance::Prone, 0.8f);
	TransitionTimes.Add(EAZMovementStance::Sliding, 0.1f);
    
	AZ_LOG(Log, "AZLocomotionDataAsset created with default values");
}