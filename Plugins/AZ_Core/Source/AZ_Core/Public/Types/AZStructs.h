#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AZEnums.h"
#include "AZStructs.generated.h"

// Survival Stats
USTRUCT(BlueprintType)
struct AZ_CORE_API FAZSurvivalStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "100"))
    float Hunger = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "100"))
    float Thirst = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "100"))
    float Fatigue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "35", ClampMax = "42"))
    float BodyTemperature = 37.0f;

    FAZSurvivalStats();
    bool IsHealthy() const;
    void ApplyEffects(float& SpeedMult, float& StaminaMult) const;
};

// Locomotion Config
USTRUCT(BlueprintType)
struct AZ_CORE_API FAZLocomotionConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Speeds")
    TMap<EAZMovementSpeed, float> BaseSpeeds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stance Modifiers")
    TMap<EAZMovementStance, float> StanceSpeedMultipliers;

    FAZLocomotionConfig();
    float GetFinalSpeed(EAZMovementStance Stance, EAZMovementSpeed Speed) const;
};

// Grid Item Info (pour inventaire Tarkov-style)
USTRUCT(BlueprintType)
struct AZ_CORE_API FAZGridItemInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FIntPoint GridSize = FIntPoint(1, 1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanRotate = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
    float Weight = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
    int32 Value = 1;

    FAZGridItemInfo();
};

// DataTable Row pour items
USTRUCT(BlueprintType)
struct AZ_CORE_API FAZItemTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FAZGridItemInfo GridInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> IconTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UStaticMesh> WorldMesh;

    FAZItemTableRow();
};