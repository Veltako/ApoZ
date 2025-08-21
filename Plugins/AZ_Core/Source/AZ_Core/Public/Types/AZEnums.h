#pragma once

#include "CoreMinimal.h"
#include "AZEnums.generated.h"

// Movement
UENUM(BlueprintType)
enum class EAZMovementStance : uint8
{
    Standing    UMETA(DisplayName = "Standing"),
    Crouching   UMETA(DisplayName = "Crouching"), 
    Prone       UMETA(DisplayName = "Prone"),
    Sliding     UMETA(DisplayName = "Sliding")
};

UENUM(BlueprintType)
enum class EAZMovementSpeed : uint8
{
    Walk        UMETA(DisplayName = "Walk"),
    Jog         UMETA(DisplayName = "Jog"),
    Run         UMETA(DisplayName = "Run"),
    Sprint      UMETA(DisplayName = "Sprint")
};

// Survival
UENUM(BlueprintType)
enum class EAZSurvivalStatType : uint8
{
    Hunger          UMETA(DisplayName = "Hunger"),
    Thirst          UMETA(DisplayName = "Thirst"),
    Fatigue         UMETA(DisplayName = "Fatigue"),
    Temperature     UMETA(DisplayName = "Body Temperature")
};

// Weapons
UENUM(BlueprintType)
enum class EAZWeaponType : uint8
{
    None            UMETA(DisplayName = "None"),
    Pistol          UMETA(DisplayName = "Pistol"),
    SMG             UMETA(DisplayName = "SMG"),
    AssaultRifle    UMETA(DisplayName = "Assault Rifle"),
    SniperRifle     UMETA(DisplayName = "Sniper Rifle"),
    Shotgun         UMETA(DisplayName = "Shotgun"),
    LMG             UMETA(DisplayName = "Light Machine Gun"),
    Melee           UMETA(DisplayName = "Melee Weapon")
};

// AI
UENUM(BlueprintType)
enum class EAZZombieType : uint8
{
    Walker      UMETA(DisplayName = "Walker"),
    Runner      UMETA(DisplayName = "Runner"),
    Crawler     UMETA(DisplayName = "Crawler"),
    Bloater     UMETA(DisplayName = "Bloater"),
    Screamer    UMETA(DisplayName = "Screamer"),
    Tank        UMETA(DisplayName = "Tank")
};