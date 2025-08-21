#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AZBlueprintFunctionLibrary.generated.h"

// Forward declarations pour éviter les includes circulaires
enum class EAZMovementStance : uint8;
enum class EAZMovementSpeed : uint8;
enum class EAZWeaponType : uint8;
enum class EAZZombieType : uint8;
enum class EAZSurvivalStatType : uint8;
struct FAZSurvivalStats;
struct FAZLocomotionConfig;
struct FAZGridItemInfo;

/**
 * Fonctions utilitaires Blueprint pour ApoZ
 */
UCLASS()
class AZ_CORE_API UAZBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ============================================================================
    // CONVERSION ET UTILITAIRES
    // ============================================================================

    /** Convertit une stance en string lisible */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Utilities")
    static FString MovementStanceToString(EAZMovementStance Stance);

    /** Convertit une vitesse en string lisible */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Utilities")
    static FString MovementSpeedToString(EAZMovementSpeed Speed);

    /** Convertit un type d'arme en string */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Utilities")
    static FString WeaponTypeToString(EAZWeaponType WeaponType);

    /** Convertit un type de zombie en string */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Utilities")
    static FString ZombieTypeToString(EAZZombieType ZombieType);

    // ============================================================================
    // VALIDATION ET LOGIQUE MÉTIER
    // ============================================================================

    /** Vérifie si une stance permet de sprinter */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Movement")
    static bool CanSprintInStance(EAZMovementStance Stance);

    /** Vérifie si une stance permet de tirer avec précision */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Movement")
    static bool CanAimAccuratelyInStance(EAZMovementStance Stance);

    /** Vérifie si une transition de stance est possible */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Movement")
    static bool CanTransitionToStance(EAZMovementStance FromStance, EAZMovementStance ToStance);

    // ============================================================================
    // CALCULS DE GAMEPLAY
    // ============================================================================

    /** Calcule le modificateur de vitesse basé sur le poids porté */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Survival")
    static float CalculateWeightSpeedModifier(float CurrentWeight, float MaxWeight);

    /** Calcule le modificateur de stamina basé sur les stats de survie */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Survival")
    static float CalculateStaminaModifier(const FAZSurvivalStats& SurvivalStats);

    /** Calcule le niveau de bruit généré par le mouvement */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Stealth")
    static float CalculateMovementNoise(EAZMovementStance Stance, EAZMovementSpeed Speed, float WeightModifier = 1.0f);

    /** Calcule la vitesse finale avec tous les modificateurs */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Movement")
    static float CalculateFinalMovementSpeed(
        EAZMovementStance Stance,
        EAZMovementSpeed Speed,
        const FAZLocomotionConfig& Config,
        float WeightModifier = 1.0f,
        float SurvivalModifier = 1.0f
    );

    // ============================================================================
    // UTILITAIRES GRID INVENTORY
    // ============================================================================

    /** Vérifie si un item peut être placé à une position dans la grille */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Inventory")
    static bool CanPlaceItemInGrid(
        const FAZGridItemInfo& ItemInfo,
        FIntPoint GridPosition,
        FIntPoint GridSize,
        bool bRotated = false
    );

    /** Calcule l'espace total occupé par un item dans la grille */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Inventory")
    static int32 CalculateItemGridArea(const FAZGridItemInfo& ItemInfo, bool bRotated = false);

    // ============================================================================
    // UTILITAIRES SURVIE
    // ============================================================================

    /** Vérifie si les stats de survie sont critiques */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Survival")
    static bool IsSurvivalStatCritical(const FAZSurvivalStats& Stats, EAZSurvivalStatType StatType);

    /** Retourne la stat de survie la plus critique */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Survival")
    static EAZSurvivalStatType GetMostCriticalSurvivalStat(const FAZSurvivalStats& Stats);

    // ============================================================================
    // UTILITAIRES MATHÉMATIQUES
    // ============================================================================

    /** Lerp avec easing (smooth step) */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Math")
    static float SmoothLerp(float A, float B, float Alpha);

    /** Applique une courbe de fatigue exponentielle */
    UFUNCTION(BlueprintPure, Category = "ApoZ|Math")
    static float ApplyFatigueCurve(float LinearValue, float Steepness = 2.0f);
};