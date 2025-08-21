// ============================================================================
// Private/Utils/AZBlueprintFunctionLibrary.cpp - Version Simple qui Compile
// ============================================================================

#include "Utils/AZBlueprintFunctionLibrary.h"
#include "Types/AZEnums.h"
#include "Types/AZStructs.h"
#include "Utils/AZLogger.h"

// ============================================================================
// DEFINE LOG CATEGORY (une seule fois)
// ============================================================================
DEFINE_LOG_CATEGORY(LogApoZ);

// ============================================================================
// IMPLÉMENTATIONS DES STRUCTURES SIMPLES
// ============================================================================

// FAZSurvivalStats
FAZSurvivalStats::FAZSurvivalStats()
{
    Hunger = 100.0f;
    Thirst = 100.0f;
    Fatigue = 0.0f;
    BodyTemperature = 37.0f;
}

bool FAZSurvivalStats::IsHealthy() const
{
    return Hunger > 25.0f && 
           Thirst > 15.0f && 
           Fatigue < 80.0f && 
           BodyTemperature >= 36.0f && 
           BodyTemperature <= 38.0f;
}

void FAZSurvivalStats::ApplyEffects(float& SpeedMultiplier, float& StaminaMultiplier) const
{
    SpeedMultiplier = 1.0f;
    StaminaMultiplier = 1.0f;
    
    // Effet de la faim
    if (Hunger < 25.0f)
    {
        float HungerRatio = Hunger / 25.0f;
        SpeedMultiplier *= FMath::Lerp(0.7f, 1.0f, HungerRatio);
        StaminaMultiplier *= FMath::Lerp(0.5f, 1.0f, HungerRatio);
    }
    
    // Effet de la soif
    if (Thirst < 15.0f)
    {
        float ThirstRatio = Thirst / 15.0f;
        SpeedMultiplier *= FMath::Lerp(0.6f, 1.0f, ThirstRatio);
        StaminaMultiplier *= FMath::Lerp(0.3f, 1.0f, ThirstRatio);
    }
    
    // Effet de la fatigue
    if (Fatigue > 80.0f)
    {
        float FatigueRatio = (Fatigue - 80.0f) / 20.0f;
        SpeedMultiplier *= FMath::Lerp(1.0f, 0.5f, FatigueRatio);
        StaminaMultiplier *= FMath::Lerp(1.0f, 0.2f, FatigueRatio);
    }
    
    // Clamp final values
    SpeedMultiplier = FMath::Clamp(SpeedMultiplier, 0.1f, 1.0f);
    StaminaMultiplier = FMath::Clamp(StaminaMultiplier, 0.1f, 1.0f);
}

// FAZLocomotionConfig
FAZLocomotionConfig::FAZLocomotionConfig()
{
    // Vitesses de base (unités UE5 par seconde)
    BaseSpeeds.Add(EAZMovementSpeed::Walk, 200.0f);
    BaseSpeeds.Add(EAZMovementSpeed::Jog, 350.0f);
    BaseSpeeds.Add(EAZMovementSpeed::Run, 500.0f);
    BaseSpeeds.Add(EAZMovementSpeed::Sprint, 650.0f);

    // Multiplicateurs de stance
    StanceSpeedMultipliers.Add(EAZMovementStance::Standing, 1.0f);
    StanceSpeedMultipliers.Add(EAZMovementStance::Crouching, 0.75f);
    StanceSpeedMultipliers.Add(EAZMovementStance::Prone, 0.25f);
    StanceSpeedMultipliers.Add(EAZMovementStance::Sliding, 1.2f);
}

float FAZLocomotionConfig::GetFinalSpeed(EAZMovementStance Stance, EAZMovementSpeed Speed) const
{
    const float* BaseSpeed = BaseSpeeds.Find(Speed);
    const float* StanceMultiplier = StanceSpeedMultipliers.Find(Stance);
    
    if (BaseSpeed && StanceMultiplier)
    {
        return (*BaseSpeed) * (*StanceMultiplier);
    }
    
    // Fallback values
    AZ_LOG_WARN("GetFinalSpeed: Could not find values for Stance %d, Speed %d", (int32)Stance, (int32)Speed);
    return 350.0f; // Default jog speed
}

// FAZGridItemInfo
FAZGridItemInfo::FAZGridItemInfo()
{
    GridSize = FIntPoint(1, 1);
    bCanRotate = true;
    Weight = 0.1f;
    Value = 1;
    // MaxStack n'existe pas dans le header, on l'enlève
}

// FAZItemTableRow
FAZItemTableRow::FAZItemTableRow()
{
    ItemName = TEXT("Unknown Item");
    GridInfo = FAZGridItemInfo();
    IconTexture = nullptr;
    WorldMesh = nullptr;
}

// ============================================================================
// CONVERSION ET UTILITAIRES
// ============================================================================

FString UAZBlueprintFunctionLibrary::MovementStanceToString(EAZMovementStance Stance)
{
    switch (Stance)
    {
        case EAZMovementStance::Standing:   return TEXT("Standing");
        case EAZMovementStance::Crouching:  return TEXT("Crouching");
        case EAZMovementStance::Prone:      return TEXT("Prone");
        case EAZMovementStance::Sliding:    return TEXT("Sliding");
        default:                            return TEXT("Unknown");
    }
}

FString UAZBlueprintFunctionLibrary::MovementSpeedToString(EAZMovementSpeed Speed)
{
    switch (Speed)
    {
        case EAZMovementSpeed::Walk:    return TEXT("Walk");
        case EAZMovementSpeed::Jog:     return TEXT("Jog");
        case EAZMovementSpeed::Run:     return TEXT("Run");
        case EAZMovementSpeed::Sprint:  return TEXT("Sprint");
        default:                        return TEXT("Unknown");
    }
}

FString UAZBlueprintFunctionLibrary::WeaponTypeToString(EAZWeaponType WeaponType)
{
    switch (WeaponType)
    {
        case EAZWeaponType::None:           return TEXT("None");
        case EAZWeaponType::Pistol:         return TEXT("Pistol");
        case EAZWeaponType::SMG:            return TEXT("SMG");
        case EAZWeaponType::AssaultRifle:   return TEXT("Assault Rifle");
        case EAZWeaponType::SniperRifle:    return TEXT("Sniper Rifle");
        case EAZWeaponType::Shotgun:        return TEXT("Shotgun");
        case EAZWeaponType::LMG:            return TEXT("LMG");
        case EAZWeaponType::Melee:          return TEXT("Melee");
        default:                            return TEXT("Unknown");
    }
}

FString UAZBlueprintFunctionLibrary::ZombieTypeToString(EAZZombieType ZombieType)
{
    switch (ZombieType)
    {
        case EAZZombieType::Walker:     return TEXT("Walker");
        case EAZZombieType::Runner:     return TEXT("Runner");
        case EAZZombieType::Crawler:    return TEXT("Crawler");
        case EAZZombieType::Bloater:    return TEXT("Bloater");
        case EAZZombieType::Screamer:   return TEXT("Screamer");
        case EAZZombieType::Tank:       return TEXT("Tank");
        default:                        return TEXT("Unknown");
    }
}

// ============================================================================
// VALIDATION ET LOGIQUE MÉTIER
// ============================================================================

bool UAZBlueprintFunctionLibrary::CanSprintInStance(EAZMovementStance Stance)
{
    return Stance == EAZMovementStance::Standing || Stance == EAZMovementStance::Sliding;
}

bool UAZBlueprintFunctionLibrary::CanAimAccuratelyInStance(EAZMovementStance Stance)
{
    switch (Stance)
    {
        case EAZMovementStance::Standing:   return true;
        case EAZMovementStance::Crouching:  return true;
        case EAZMovementStance::Prone:      return true;
        case EAZMovementStance::Sliding:    return false;
        default:                            return false;
    }
}

bool UAZBlueprintFunctionLibrary::CanTransitionToStance(EAZMovementStance FromStance, EAZMovementStance ToStance)
{
    if (FromStance == ToStance)
        return false;

    if (FromStance == EAZMovementStance::Sliding)
        return ToStance == EAZMovementStance::Standing;

    if (ToStance == EAZMovementStance::Sliding)
        return FromStance == EAZMovementStance::Standing || FromStance == EAZMovementStance::Crouching;

    return true;
}

// ============================================================================
// CALCULS DE GAMEPLAY
// ============================================================================

float UAZBlueprintFunctionLibrary::CalculateWeightSpeedModifier(float CurrentWeight, float MaxWeight)
{
    if (MaxWeight <= 0.0f)
        return 1.0f;

    float WeightRatio = FMath::Clamp(CurrentWeight / MaxWeight, 0.0f, 1.5f);
    
    if (WeightRatio <= 0.8f)
    {
        return 1.0f;
    }
    else if (WeightRatio <= 1.0f)
    {
        float OverweightRatio = (WeightRatio - 0.8f) / 0.2f;
        return FMath::Lerp(1.0f, 0.8f, OverweightRatio);
    }
    else
    {
        float OverloadRatio = FMath::Min((WeightRatio - 1.0f) / 0.5f, 1.0f);
        return FMath::Lerp(0.8f, 0.3f, OverloadRatio);
    }
}

float UAZBlueprintFunctionLibrary::CalculateStaminaModifier(const FAZSurvivalStats& SurvivalStats)
{
    float Modifier = 1.0f;
    
    if (SurvivalStats.Hunger < 25.0f)
    {
        Modifier *= FMath::Lerp(0.5f, 1.0f, SurvivalStats.Hunger / 25.0f);
    }
    
    if (SurvivalStats.Thirst < 15.0f)
    {
        Modifier *= FMath::Lerp(0.3f, 1.0f, SurvivalStats.Thirst / 15.0f);
    }
    
    if (SurvivalStats.Fatigue > 80.0f)
    {
        float FatigueRatio = (SurvivalStats.Fatigue - 80.0f) / 20.0f;
        Modifier *= FMath::Lerp(1.0f, 0.2f, FatigueRatio);
    }
    
    return FMath::Clamp(Modifier, 0.1f, 1.0f);
}

float UAZBlueprintFunctionLibrary::CalculateMovementNoise(EAZMovementStance Stance, EAZMovementSpeed Speed, float WeightModifier)
{
    float BaseNoise = 1.0f;
    
    // Noise par vitesse (hard-coded puisque StaminaCosts n'existe pas)
    switch (Speed)
    {
        case EAZMovementSpeed::Walk:    BaseNoise = 0.3f; break;
        case EAZMovementSpeed::Jog:     BaseNoise = 0.6f; break;
        case EAZMovementSpeed::Run:     BaseNoise = 1.0f; break;
        case EAZMovementSpeed::Sprint:  BaseNoise = 1.5f; break;
    }
    
    // Multiplicateur par stance (hard-coded puisque NoiseMultipliers n'existe pas)
    float StanceMultiplier = 1.0f;
    switch (Stance)
    {
        case EAZMovementStance::Standing:   StanceMultiplier = 1.0f; break;
        case EAZMovementStance::Crouching:  StanceMultiplier = 0.5f; break;
        case EAZMovementStance::Prone:      StanceMultiplier = 0.25f; break;
        case EAZMovementStance::Sliding:    StanceMultiplier = 1.3f; break;
    }
    
    return BaseNoise * StanceMultiplier * WeightModifier;
}

float UAZBlueprintFunctionLibrary::CalculateFinalMovementSpeed(
    EAZMovementStance Stance,
    EAZMovementSpeed Speed,
    const FAZLocomotionConfig& Config,
    float WeightModifier,
    float SurvivalModifier)
{
    float FinalSpeed = Config.GetFinalSpeed(Stance, Speed);
    FinalSpeed *= WeightModifier;
    FinalSpeed *= SurvivalModifier;
    
    return FMath::Max(FinalSpeed, 10.0f);
}

// ============================================================================
// UTILITAIRES GRID INVENTORY
// ============================================================================

bool UAZBlueprintFunctionLibrary::CanPlaceItemInGrid(
    const FAZGridItemInfo& ItemInfo,
    FIntPoint GridPosition,
    FIntPoint GridSize,
    bool bRotated)
{
    FIntPoint ItemSize = ItemInfo.GridSize;
    if (bRotated && ItemInfo.bCanRotate)
    {
        ItemSize = FIntPoint(ItemSize.Y, ItemSize.X);
    }
    
    return (GridPosition.X >= 0) &&
           (GridPosition.Y >= 0) &&
           (GridPosition.X + ItemSize.X <= GridSize.X) &&
           (GridPosition.Y + ItemSize.Y <= GridSize.Y);
}

int32 UAZBlueprintFunctionLibrary::CalculateItemGridArea(const FAZGridItemInfo& ItemInfo, bool bRotated)
{
    FIntPoint Size = ItemInfo.GridSize;
    if (bRotated && ItemInfo.bCanRotate)
    {
        Size = FIntPoint(Size.Y, Size.X);
    }
    return Size.X * Size.Y;
}

// ============================================================================
// UTILITAIRES SURVIE
// ============================================================================

bool UAZBlueprintFunctionLibrary::IsSurvivalStatCritical(const FAZSurvivalStats& Stats, EAZSurvivalStatType StatType)
{
    switch (StatType)
    {
        case EAZSurvivalStatType::Hunger:       return Stats.Hunger <= 15.0f;
        case EAZSurvivalStatType::Thirst:       return Stats.Thirst <= 10.0f;
        case EAZSurvivalStatType::Fatigue:      return Stats.Fatigue >= 90.0f;
        case EAZSurvivalStatType::Temperature:  return Stats.BodyTemperature <= 35.5f || Stats.BodyTemperature >= 39.0f;
        default:                                return false;
    }
}

EAZSurvivalStatType UAZBlueprintFunctionLibrary::GetMostCriticalSurvivalStat(const FAZSurvivalStats& Stats)
{
    if (IsSurvivalStatCritical(Stats, EAZSurvivalStatType::Temperature))
        return EAZSurvivalStatType::Temperature;
    
    if (IsSurvivalStatCritical(Stats, EAZSurvivalStatType::Thirst))
        return EAZSurvivalStatType::Thirst;
    
    if (IsSurvivalStatCritical(Stats, EAZSurvivalStatType::Hunger))
        return EAZSurvivalStatType::Hunger;
    
    if (IsSurvivalStatCritical(Stats, EAZSurvivalStatType::Fatigue))
        return EAZSurvivalStatType::Fatigue;
    
    float LowestValue = FMath::Min3(Stats.Hunger, Stats.Thirst, 100.0f - Stats.Fatigue);
    
    if (LowestValue == Stats.Thirst)
        return EAZSurvivalStatType::Thirst;
    else if (LowestValue == Stats.Hunger)
        return EAZSurvivalStatType::Hunger;
    else
        return EAZSurvivalStatType::Fatigue;
}

// ============================================================================
// UTILITAIRES MATHÉMATIQUES
// ============================================================================

float UAZBlueprintFunctionLibrary::SmoothLerp(float A, float B, float Alpha)
{
    float SmoothAlpha = Alpha * Alpha * (3.0f - 2.0f * Alpha);
    return FMath::Lerp(A, B, SmoothAlpha);
}

float UAZBlueprintFunctionLibrary::ApplyFatigueCurve(float LinearValue, float Steepness)
{
    LinearValue = FMath::Clamp(LinearValue, 0.0f, 1.0f);
    return FMath::Pow(LinearValue, Steepness);
}