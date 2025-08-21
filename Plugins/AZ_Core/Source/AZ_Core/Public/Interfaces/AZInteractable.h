#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AZInteractable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UAZInteractable : public UInterface
{
	GENERATED_BODY()
};

class AZ_CORE_API IAZInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AActor* InteractingActor) const;
	virtual bool CanInteract_Implementation(AActor* InteractingActor) const { return true; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* InteractingActor);
	virtual void OnInteract_Implementation(AActor* InteractingActor) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionText() const;
	virtual FText GetInteractionText_Implementation() const { return FText::FromString("Interact"); }
};