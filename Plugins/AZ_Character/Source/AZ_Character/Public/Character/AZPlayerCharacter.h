#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AZ_Core/Public/Types/AZTypes.h"
#include "AZPlayerCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogAZCharacter, Log, All);

UCLASS(config=Game)
class AZ_CHARACTER_API AAZPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAZPlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
    // ============================================================================
    // COMPONENTS - TRUE FPS SETUP
    // ============================================================================

    /** First person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FPSCamera;

    // NOTE: Pas de FPSMesh séparé ! On utilise GetMesh() pour le corps complet

    // ============================================================================
    // INPUT ACTIONS
    // ============================================================================

    /** Move Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    /** Look Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    /** Jump Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    /** Crouch Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* CrouchAction;

    /** Prone Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* ProneAction;

    /** Sprint Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* SprintAction;

    /** Lean Left Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LeanLeftAction;

    /** Lean Right Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LeanRightAction;

    /** Input Mapping Context */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    // ============================================================================
    // MOVEMENT PROPERTIES - REPLICATION MINIMALE
    // ============================================================================

    /** Current movement stance */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AZ|Movement")
    EAZMovementStance CurrentStance;

    /** Current movement speed - RÉPLIQUÉ avec notification */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentSpeed, Category = "AZ|Movement")
    EAZMovementSpeed CurrentSpeed;

    /** Locomotion configuration */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AZ|Movement")
    FAZLocomotionConfig LocomotionConfig;

    /** Current lean angle (-1 to 1) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AZ|Movement")
    float LeanAngle;

    /** Target lean angle for smooth interpolation */
    float TargetLeanAngle = 0.0f;

    /** Current lean angle for smooth interpolation */
    float CurrentLeanAngle = 0.0f;

    /** Maximum lean distance in units */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AZ|Movement")
    float MaxLeanDistance;

    // ============================================================================
    // PRONE SYSTEM
    // ============================================================================

    /** Is currently in prone position */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AZ|Movement")
    bool bIsProne = false;

    /** Original capsule half height */
    float OriginalCapsuleHalfHeight = 96.0f;

    /** Prone capsule half height */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AZ|Movement")
    float ProneCapsuleHalfHeight = 30.0f;

    // ============================================================================
    // TRUE FPS - HEAD BOB & CAMERA
    // ============================================================================

    /** Head bob intensity */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AZ|Camera", meta = (ClampMin = "0.0", ClampMax = "5.0"))
    float HeadBobIntensity;

    /** Head bob speed multiplier */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AZ|Camera", meta = (ClampMin = "1.0", ClampMax = "20.0"))
    float HeadBobSpeed;

    /** Initial camera location for head bob calculations */
    FVector InitialCameraLocation;

    /** Current camera height for stance transitions */
    float CurrentCameraHeight;

    /** Target camera height for smooth transitions */
    float TargetCameraHeight;

    /** Head bob time accumulator */
    float HeadBobTime = 0.0f;

    // ============================================================================
    // INPUT STATE TRACKING
    // ============================================================================

    bool bIsCrouchPressed = false;
    bool bIsSprintPressed = false;
    bool bIsLeanLeftPressed = false;
    bool bIsLeanRightPressed = false;

    // ============================================================================
    // SURVIVAL STATS
    // ============================================================================

    /** Current survival statistics */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AZ|Survival")
    FAZSurvivalStats SurvivalStats;

    // ============================================================================
    // LIFECYCLE
    // ============================================================================

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ============================================================================
    // REPLICATION CALLBACKS
    // ============================================================================

    /** Called when CurrentSpeed is replicated */
    UFUNCTION()
    void OnRep_CurrentSpeed();

    // ============================================================================
    // INPUT FUNCTIONS
    // ============================================================================

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump(const FInputActionValue& Value);
    void StopJump(const FInputActionValue& Value);
    void StartCrouch(const FInputActionValue& Value);
    void StopCrouch(const FInputActionValue& Value);
    void ToggleProne(const FInputActionValue& Value);
    void StartSprint(const FInputActionValue& Value);
    void StopSprint(const FInputActionValue& Value);
    void StartLeanLeft(const FInputActionValue& Value);
    void StopLeanLeft(const FInputActionValue& Value);
    void StartLeanRight(const FInputActionValue& Value);
    void StopLeanRight(const FInputActionValue& Value);

    // ============================================================================
    // MOVEMENT FUNCTIONS - AVEC SERVER RPC SIMPLE
    // ============================================================================

    /** Change movement stance */
    UFUNCTION(BlueprintCallable, Category = "AZ|Movement")
    void SetMovementStance(EAZMovementStance NewStance);

    /** Change movement speed - AVEC SERVER RPC */
    UFUNCTION(BlueprintCallable, Category = "AZ|Movement")
    void SetMovementSpeed(EAZMovementSpeed NewSpeed);

    /** Set lean angle */
    UFUNCTION(BlueprintCallable, Category = "AZ|Movement")
    void SetLeanAngle(float Angle);

    // SERVER RPC SEULEMENT POUR LA VITESSE
    UFUNCTION(Server, Reliable, Category = "AZ|Movement")
    void ServerSetMovementSpeed(EAZMovementSpeed NewSpeed);
    void ServerSetMovementSpeed_Implementation(EAZMovementSpeed NewSpeed);

    /** Update movement speed based on current stance and speed */
    void UpdateMovementSpeed();

    // ============================================================================
    // PRONE FUNCTIONS
    // ============================================================================

    /** Enter prone position */
    UFUNCTION(BlueprintCallable, Category = "AZ|Movement")
    void StartProne();

    /** Exit prone position */
    UFUNCTION(BlueprintCallable, Category = "AZ|Movement")
    void StopProne();

    /** Check if we can go prone */
    UFUNCTION(BlueprintCallable, Category = "AZ|Movement")
    bool CanGoProne() const;

    /** Check if we can exit prone */
    UFUNCTION(BlueprintCallable, Category = "AZ|Movement")
    bool CanExitProne() const;

    // ============================================================================
    // TRUE FPS FUNCTIONS
    // ============================================================================

    /** Update head bob effect */
    void UpdateHeadBob(float DeltaTime);

    /** Update lean effect */
    void UpdateLean(float DeltaTime);

    /** Update camera height based on stance */
    void UpdateCameraHeight();

    /** Update survival statistics */
    void UpdateSurvivalStats(float DeltaTime);

public:
    // ============================================================================
    // ACCESSORS
    // ============================================================================

    /** Returns FPSCamera subobject **/
    FORCEINLINE UCameraComponent* GetFPSCamera() const { return FPSCamera; }

    /** Get current movement stance */
    FORCEINLINE EAZMovementStance GetCurrentStance() const { return CurrentStance; }

    /** Get current movement speed */
    FORCEINLINE EAZMovementSpeed GetCurrentSpeed() const { return CurrentSpeed; }

    /** Get survival stats */
    FORCEINLINE FAZSurvivalStats GetSurvivalStats() const { return SurvivalStats; }

    /** Is currently prone */
    FORCEINLINE bool IsProne() const { return bIsProne; }
};