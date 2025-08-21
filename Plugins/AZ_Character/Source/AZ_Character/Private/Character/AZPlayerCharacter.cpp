#include "Character/AZPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "AZ_Core/Public/Utils/AZLogger.h"
#include "AZ_Core/Public/Utils/AZBlueprintFunctionLibrary.h"

DEFINE_LOG_CATEGORY(LogAZCharacter);

// ============================================================================
// CONSTRUCTEUR - TRUE FPS + NETWORK EXTRÊME + PRONE
// ============================================================================

AAZPlayerCharacter::AAZPlayerCharacter(const FObjectInitializer& ObjectInitializer) 
    : Super(ObjectInitializer)
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = false; // TRUE FPS: pas de rotation auto
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 700.0f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 400.0f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Configure crouch
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCharacterMovement()->SetCrouchedHalfHeight(60.0f);

    // TRUE FPS: Use controller rotation for character
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;    // ← TRUE pour True FPS !
    bUseControllerRotationRoll = false;

    // ============================================================================
    // SETUP CAMERA FPS
    // ============================================================================

    // Create FPS camera
    FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
    FPSCamera->SetupAttachment(RootComponent);
    FPSCamera->SetRelativeLocation(FVector(15.0f, 0.0f, 64.0f)); // Position avec recul pour éviter l'intérieur
    FPSCamera->bUsePawnControlRotation = true;

    // ============================================================================
    // SETUP MESH UNIQUE - TRUE FPS CORPS COMPLET
    // ============================================================================

    // Configure le mesh principal pour True FPS
    GetMesh()->SetOwnerNoSee(false);        // ← LE JOUEUR VOIT SON CORPS !
    GetMesh()->SetOnlyOwnerSee(false);      // ← Les autres voient aussi
    GetMesh()->SetCastShadow(true);         // ← Ombres réalistes
    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -97.0f));

    // ============================================================================
    // NETWORK SETTINGS EXTRÊMES - ANTI-ROLLBACK
    // ============================================================================

    // Enable replication basique
    bReplicates = true;
    SetReplicateMovement(true);
    
    // Configure pour le multijoueur
    SetCanBeDamaged(true);
    SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

    // Configure network settings pour CharacterMovement - SETTINGS EXTREMES
    UCharacterMovementComponent* CharMovement = GetCharacterMovement();
    if (CharMovement)
    {
        // Désactiver le smoothing réseau complètement
        CharMovement->NetworkMaxSmoothUpdateDistance = 1000.0f;     // Très tolérant
        CharMovement->NetworkNoSmoothUpdateDistance = 1000.0f;      // Très tolérant
        CharMovement->NetworkMinTimeBetweenClientAckGoodMoves = 0.1f;   // Très relax
        CharMovement->NetworkMinTimeBetweenClientAdjustments = 0.1f;    // Très relax
        
        // Pas de smoothing
        CharMovement->NetworkSimulatedSmoothLocationTime = 0.0f;    // Pas de smoothing !
        CharMovement->NetworkSimulatedSmoothRotationTime = 0.0f;    // Pas de smoothing !
        
        // Max speed for network
        CharMovement->MaxWalkSpeed = 600.0f;
        
        // Désactiver les corrections
        CharMovement->NetworkLargeClientCorrectionDistance = 1000.0f; // Très tolérant
        
        // Désactiver la prédiction
        CharMovement->bNetworkAlwaysReplicateTransformUpdateTimestamp = false;
        CharMovement->bEnablePhysicsInteraction = false;
        CharMovement->bNetworkSmoothingComplete = false; // Désactiver complètement
    }

    // ============================================================================
    // INITIALISER LES PROPRIÉTÉS
    // ============================================================================

    // Default values
    CurrentStance = EAZMovementStance::Standing;
    CurrentSpeed = EAZMovementSpeed::Jog;
    LeanAngle = 0.0f;
    HeadBobIntensity = 0.5f;  // Réduit pour éviter les conflits réseau
    HeadBobSpeed = 8.0f;
    MaxLeanDistance = 30.0f;

    // Dans le constructeur, après les autres initialisations :
    OriginalCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
    ProneCapsuleHalfHeight = 30.0f;
    bIsProne = false;
    
    // Initialize locomotion config
    LocomotionConfig = FAZLocomotionConfig();
    
    // Initialize survival stats
    SurvivalStats = FAZSurvivalStats();

    // Set this character to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    AZ_LOG(Log, "AZPlayerCharacter constructed successfully (True FPS + Server RPC)");
}

// ============================================================================
// REPLICATION SETUP
// ============================================================================

void AAZPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Répliquer SEULEMENT la vitesse (le plus critique)
    DOREPLIFETIME(AAZPlayerCharacter, CurrentSpeed);
    
    // PAS CurrentStance ni LeanAngle pour éviter les rollbacks
}

void AAZPlayerCharacter::OnRep_CurrentSpeed()
{
    // Debug plus visible
    AZ_LOG(Warning, "=== CLIENT REPNOTIFY: Speed replicated to %s ===", 
        *UAZBlueprintFunctionLibrary::MovementSpeedToString(CurrentSpeed));
    
    // Forcer la mise à jour de la vitesse quand elle change
    UpdateMovementSpeed();
    
    // Debug supplémentaire
    float CurrentMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
    AZ_LOG(Warning, "=== CLIENT: MaxWalkSpeed is now %.0f ===", CurrentMaxSpeed);
}

// ============================================================================
// LIFECYCLE
// ============================================================================

void AAZPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Store initial camera location for head bob
    InitialCameraLocation = FPSCamera->GetRelativeLocation();
    CurrentCameraHeight = InitialCameraLocation.Z;
    TargetCameraHeight = CurrentCameraHeight;

    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
                AZ_LOG(Log, "Input mapping context added successfully");
            }
            else
            {
                AZ_LOG(Warning, "DefaultMappingContext is null! Create Input Mapping Context asset.");
            }
        }
    }

    // Initialize movement speed
    UpdateMovementSpeed();
    UpdateCameraHeight();

    AZ_LOG(Log, "AZPlayerCharacter BeginPlay completed - True FPS Ready!");
}

void AAZPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update True FPS systems every frame
    UpdateHeadBob(DeltaTime);
    UpdateLean(DeltaTime);
    UpdateCameraHeight();

    // Update survival stats less frequently (every 0.5 seconds)
    static float SurvivalUpdateTimer = 0.0f;
    SurvivalUpdateTimer += DeltaTime;
    if (SurvivalUpdateTimer >= 0.5f)
    {
        UpdateSurvivalStats(SurvivalUpdateTimer);
        SurvivalUpdateTimer = 0.0f;
    }
}

// ============================================================================
// INPUT SETUP - AVEC NETWORK FIX
// ============================================================================

void AAZPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // IMPORTANT: Bind input seulement si localement contrôlé !
    if (!IsLocallyControlled())
    {
        AZ_LOG(Warning, "Skipping input setup for non-locally controlled pawn");
        return;
    }

    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Moving
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAZPlayerCharacter::Move);
        }

        // Looking
        if (LookAction)
        {
            EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAZPlayerCharacter::Look);
        }

        // Jumping
        if (JumpAction)
        {
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AAZPlayerCharacter::StartJump);
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AAZPlayerCharacter::StopJump);
        }

        // Crouching
        if (CrouchAction)
        {
            EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AAZPlayerCharacter::StartCrouch);
            EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AAZPlayerCharacter::StopCrouch);
        }

        // Prone
        if (ProneAction)
        {
            EnhancedInputComponent->BindAction(ProneAction, ETriggerEvent::Started, this, &AAZPlayerCharacter::ToggleProne);
        }

        // Sprint
        if (SprintAction)
        {
            EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AAZPlayerCharacter::StartSprint);
            EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AAZPlayerCharacter::StopSprint);
        }

        // Lean
        if (LeanLeftAction)
        {
            EnhancedInputComponent->BindAction(LeanLeftAction, ETriggerEvent::Started, this, &AAZPlayerCharacter::StartLeanLeft);
            EnhancedInputComponent->BindAction(LeanLeftAction, ETriggerEvent::Completed, this, &AAZPlayerCharacter::StopLeanLeft);
        }

        if (LeanRightAction)
        {
            EnhancedInputComponent->BindAction(LeanRightAction, ETriggerEvent::Started, this, &AAZPlayerCharacter::StartLeanRight);
            EnhancedInputComponent->BindAction(LeanRightAction, ETriggerEvent::Completed, this, &AAZPlayerCharacter::StopLeanRight);
        }

        AZ_LOG(Log, "Input actions bound successfully (True FPS + Network)");
    }
    else
    {
        AZ_LOG(Error, "Failed to cast to EnhancedInputComponent!");
    }
}

// ============================================================================
// INPUT FUNCTIONS - AVEC VERIFICATION RESEAU
// ============================================================================

void AAZPlayerCharacter::Move(const FInputActionValue& Value)
{
    // Vérification supplémentaire réseau
    if (!IsLocallyControlled() || !Controller)
        return;

    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // TRUE FPS: Mouvement relatif à la caméra
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    
        // get right vector 
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement 
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AAZPlayerCharacter::Look(const FInputActionValue& Value)
{
    // Vérification supplémentaire réseau
    if (!IsLocallyControlled() || !Controller)
        return;

    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AAZPlayerCharacter::StartJump(const FInputActionValue& Value)
{
    Jump();
}

void AAZPlayerCharacter::StopJump(const FInputActionValue& Value)
{
    StopJumping();
}

void AAZPlayerCharacter::StartCrouch(const FInputActionValue& Value)
{
    bIsCrouchPressed = true;
    
    if (CurrentStance == EAZMovementStance::Standing)
    {
        SetMovementStance(EAZMovementStance::Crouching);
    }
    else if (CurrentStance == EAZMovementStance::Crouching)
    {
        SetMovementStance(EAZMovementStance::Standing);
    }
}

void AAZPlayerCharacter::StopCrouch(const FInputActionValue& Value)
{
    bIsCrouchPressed = false;
}

void AAZPlayerCharacter::ToggleProne(const FInputActionValue& Value)
{
    if (CurrentStance == EAZMovementStance::Prone)
    {
        SetMovementStance(EAZMovementStance::Standing);
    }
    else
    {
        SetMovementStance(EAZMovementStance::Prone);
    }
}

void AAZPlayerCharacter::StartSprint(const FInputActionValue& Value)
{
    bIsSprintPressed = true;
    
    if (UAZBlueprintFunctionLibrary::CanSprintInStance(CurrentStance))
    {
        SetMovementSpeed(EAZMovementSpeed::Sprint);
    }
}

void AAZPlayerCharacter::StopSprint(const FInputActionValue& Value)
{
    bIsSprintPressed = false;
    SetMovementSpeed(EAZMovementSpeed::Jog);
}

void AAZPlayerCharacter::StartLeanLeft(const FInputActionValue& Value)
{
    bIsLeanLeftPressed = true;
    SetLeanAngle(-1.0f);
}

void AAZPlayerCharacter::StopLeanLeft(const FInputActionValue& Value)
{
    bIsLeanLeftPressed = false;
    if (!bIsLeanRightPressed)
    {
        SetLeanAngle(0.0f);
    }
}

void AAZPlayerCharacter::StartLeanRight(const FInputActionValue& Value)
{
    bIsLeanRightPressed = true;
    SetLeanAngle(1.0f);
}

void AAZPlayerCharacter::StopLeanRight(const FInputActionValue& Value)
{
    bIsLeanRightPressed = false;
    if (!bIsLeanLeftPressed)
    {
        SetLeanAngle(0.0f);
    }
}

// ============================================================================
// MOVEMENT FUNCTIONS - AVEC SERVER RPC
// ============================================================================

void AAZPlayerCharacter::SetMovementStance(EAZMovementStance NewStance)
{
    if (CurrentStance == NewStance)
        return;

    // Check if transition is valid
    if (!UAZBlueprintFunctionLibrary::CanTransitionToStance(CurrentStance, NewStance))
    {
        AZ_LOG(Warning, "Invalid stance transition from %s to %s", 
            *UAZBlueprintFunctionLibrary::MovementStanceToString(CurrentStance),
            *UAZBlueprintFunctionLibrary::MovementStanceToString(NewStance));
        return;
    }

    EAZMovementStance OldStance = CurrentStance;
    CurrentStance = NewStance;

    // Handle UE5 crouch system
    if (NewStance == EAZMovementStance::Crouching)
    {
        Crouch();
    }
    else if (OldStance == EAZMovementStance::Crouching)
    {
        UnCrouch();
    }

    switch (NewStance)
    {
    case EAZMovementStance::Standing:
        if (bIsProne) StopProne();
        if (bIsCrouched) UnCrouch();
        break;
            
    case EAZMovementStance::Crouching:
        if (bIsProne) StopProne();
        Crouch();
        break;
            
    case EAZMovementStance::Prone:
        if (bIsCrouched) UnCrouch();
        StartProne();
        break;
            
    default:
        break;
    }

    // Update movement speed with new stance
    UpdateMovementSpeed();
    
    // Update camera height for True FPS
    UpdateCameraHeight();

    AZ_LOG(Log, "Stance changed from %s to %s", 
        *UAZBlueprintFunctionLibrary::MovementStanceToString(OldStance),
        *UAZBlueprintFunctionLibrary::MovementStanceToString(NewStance));
}

void AAZPlayerCharacter::SetMovementSpeed(EAZMovementSpeed NewSpeed)
{
    // Si on est un client, envoyer au serveur
    if (!HasAuthority() && IsLocallyControlled())
    {
        AZ_LOG(Warning, "CLIENT: Sending speed change to server: %s", 
            *UAZBlueprintFunctionLibrary::MovementSpeedToString(NewSpeed));
        ServerSetMovementSpeed(NewSpeed);
        return;
    }

    // Si on est le serveur, appliquer directement
    if (HasAuthority())
    {
        ServerSetMovementSpeed(NewSpeed);
    }
}

void AAZPlayerCharacter::ServerSetMovementSpeed_Implementation(EAZMovementSpeed NewSpeed)
{
    if (CurrentSpeed == NewSpeed)
        return;

    // Check if we can sprint in current stance
    if (NewSpeed == EAZMovementSpeed::Sprint && !UAZBlueprintFunctionLibrary::CanSprintInStance(CurrentStance))
    {
        AZ_LOG(Warning, "Cannot sprint in stance: %s", 
            *UAZBlueprintFunctionLibrary::MovementStanceToString(CurrentStance));
        return;
    }

    EAZMovementSpeed OldSpeed = CurrentSpeed;
    CurrentSpeed = NewSpeed; // Cette variable est répliquée avec RepNotify !
    
    UpdateMovementSpeed();

    AZ_LOG(Warning, "=== SERVER: Speed changed from %s to %s ===", 
        *UAZBlueprintFunctionLibrary::MovementSpeedToString(OldSpeed),
        *UAZBlueprintFunctionLibrary::MovementSpeedToString(NewSpeed));
}

void AAZPlayerCharacter::SetLeanAngle(float Angle)
{
    TargetLeanAngle = FMath::Clamp(Angle, -1.0f, 1.0f);
    LeanAngle = TargetLeanAngle;
}

void AAZPlayerCharacter::UpdateMovementSpeed()
{
    float FinalSpeed = UAZBlueprintFunctionLibrary::CalculateFinalMovementSpeed(
        CurrentStance,
        CurrentSpeed,
        LocomotionConfig,
        1.0f,  // Weight modifier (TODO: implement inventory weight)
        1.0f   // Survival modifier (TODO: implement survival effects)
    );

    GetCharacterMovement()->MaxWalkSpeed = FinalSpeed;
    
    AZ_LOG(Log, "Movement speed updated to %.0f units/s", FinalSpeed);
}

// ============================================================================
// NOUVELLES FONCTIONS PRONE
// ============================================================================

void AAZPlayerCharacter::StartProne()
{
    if (bIsProne || !CanGoProne())
        return;

    bIsProne = true;
    
    // Réduire la taille de la capsule
    GetCapsuleComponent()->SetCapsuleHalfHeight(ProneCapsuleHalfHeight);
    
    // Ajuster la position pour éviter de traverser le sol
    FVector CurrentLocation = GetActorLocation();
    float HeightDifference = OriginalCapsuleHalfHeight - ProneCapsuleHalfHeight;
    CurrentLocation.Z -= HeightDifference;
    SetActorLocation(CurrentLocation);
    
    AZ_LOG(Log, "Entered prone position");
}

void AAZPlayerCharacter::StopProne()
{
    if (!bIsProne)
        return;

    // Vérifier si on peut sortir de prone (pas d'obstacle au-dessus)
    if (!CanExitProne())
    {
        AZ_LOG(Warning, "Cannot exit prone - obstacle above");
        return;
    }

    bIsProne = false;
    
    // Restaurer la taille originale de la capsule
    GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalCapsuleHalfHeight);
    
    // Ajuster la position
    FVector CurrentLocation = GetActorLocation();
    float HeightDifference = OriginalCapsuleHalfHeight - ProneCapsuleHalfHeight;
    CurrentLocation.Z += HeightDifference;
    SetActorLocation(CurrentLocation);
    
    AZ_LOG(Log, "Exited prone position");
}

bool AAZPlayerCharacter::CanGoProne() const
{
    // Vérifier qu'on est au sol
    return GetCharacterMovement()->IsMovingOnGround();
}

bool AAZPlayerCharacter::CanExitProne() const
{
    if (!bIsProne)
        return true;

    // Faire un trace pour vérifier qu'il n'y a pas d'obstacle au-dessus
    FVector Start = GetActorLocation();
    FVector End = Start + FVector(0, 0, OriginalCapsuleHalfHeight - ProneCapsuleHalfHeight + 10.0f);
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    
    FHitResult HitResult;
    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        Start,
        End,
        FQuat::Identity,
        ECC_WorldStatic,
        FCollisionShape::MakeCapsule(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), 10.0f),
        QueryParams
    );
    
    return !bHit; // Peut sortir si pas d'obstacle
}

// ============================================================================
// TRUE FPS - CAMERA & VISUAL EFFECTS
// ============================================================================

void AAZPlayerCharacter::UpdateHeadBob(float DeltaTime)
{
    if (!FPSCamera || HeadBobIntensity <= 0.0f)
        return;

    // Only bob when moving
    float Speed = GetVelocity().Size();
    if (Speed < 10.0f)
    {
        HeadBobTime = 0.0f;
        return;
    }

    // Update head bob time
    HeadBobTime += DeltaTime * HeadBobSpeed * (Speed / 100.0f);

    // Calculate bob offset
    float BobOffset = FMath::Sin(HeadBobTime) * HeadBobIntensity;
    
    // Apply stance-based modifier
    float StanceModifier = 1.0f;
    switch (CurrentStance)
    {
        case EAZMovementStance::Standing:   StanceModifier = 1.0f; break;
        case EAZMovementStance::Crouching:  StanceModifier = 0.5f; break;
        case EAZMovementStance::Prone:      StanceModifier = 0.1f; break;
        case EAZMovementStance::Sliding:    StanceModifier = 2.0f; break;
    }

    BobOffset *= StanceModifier;

    // Apply to camera
    FVector NewLocation = InitialCameraLocation;
    NewLocation.Z = CurrentCameraHeight + BobOffset;
    FPSCamera->SetRelativeLocation(NewLocation);
}

void AAZPlayerCharacter::UpdateLean(float DeltaTime)
{
    if (!FPSCamera)
        return;

    // Smooth lerp to target lean angle
    CurrentLeanAngle = FMath::FInterpTo(CurrentLeanAngle, TargetLeanAngle, DeltaTime, 5.0f);

    // Calculate lean offset
    float LeanOffset = CurrentLeanAngle * MaxLeanDistance;

    // Apply lean to camera position
    FVector CurrentLocation = FPSCamera->GetRelativeLocation();
    CurrentLocation.Y = LeanOffset;
    FPSCamera->SetRelativeLocation(CurrentLocation);

    // Apply lean rotation to camera
    FRotator CurrentRotation = FPSCamera->GetRelativeRotation();
    CurrentRotation.Roll = CurrentLeanAngle * 15.0f; // Max 15 degrees lean
    FPSCamera->SetRelativeRotation(CurrentRotation);
}

void AAZPlayerCharacter::UpdateCameraHeight()
{
    if (!FPSCamera)
        return;

    // Set target height based on stance - TRUE FPS HEIGHTS
    switch (CurrentStance)
    {
        case EAZMovementStance::Standing:
            TargetCameraHeight = 64.0f;    // Hauteur normale des yeux
            break;
        case EAZMovementStance::Crouching:
            TargetCameraHeight = 35.0f;    // Position accroupie
            break;
        case EAZMovementStance::Prone:
            TargetCameraHeight = -10.0f;   // Position allongée
            break;
        case EAZMovementStance::Sliding:
            TargetCameraHeight = 25.0f;    // Position glissade
            break;
    }

    // Smooth transition
    CurrentCameraHeight = FMath::FInterpTo(CurrentCameraHeight, TargetCameraHeight, GetWorld()->GetDeltaSeconds(), 4.0f);
}

void AAZPlayerCharacter::UpdateSurvivalStats(float DeltaTime)
{
    // Simulate survival stat decay
    SurvivalStats.Hunger = FMath::Max(0.0f, SurvivalStats.Hunger - (0.5f * DeltaTime / 60.0f)); // 0.5% per minute
    SurvivalStats.Thirst = FMath::Max(0.0f, SurvivalStats.Thirst - (1.0f * DeltaTime / 60.0f)); // 1% per minute
    
    // Fatigue increases with activity
    float ActivityLevel = GetVelocity().Size() / GetCharacterMovement()->MaxWalkSpeed;
    float FatigueIncrease = ActivityLevel * (2.0f * DeltaTime / 60.0f); // Up to 2% per minute when sprinting
    SurvivalStats.Fatigue = FMath::Min(100.0f, SurvivalStats.Fatigue + FatigueIncrease);

    // Log critical stats
    if (SurvivalStats.Hunger <= 15.0f || SurvivalStats.Thirst <= 10.0f || SurvivalStats.Fatigue >= 90.0f)
    {
        EAZSurvivalStatType CriticalStat = UAZBlueprintFunctionLibrary::GetMostCriticalSurvivalStat(SurvivalStats);
        AZ_LOG(Warning, "Critical survival stat: %s", 
            *UEnum::GetValueAsString(CriticalStat));
    }
}