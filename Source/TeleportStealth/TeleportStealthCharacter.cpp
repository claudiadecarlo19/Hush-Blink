// Copyright Epic Games, Inc. All Rights Reserved.

#include "TeleportStealthCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InGameUI.h"
#include "InputActionValue.h"
#include "TeleportStealth.h"
#include "NoiseEmitter.h"
#include "ThrowableObject.h"

ATeleportStealthCharacter::ATeleportStealthCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	TeleportBeam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Teleport Beam"));
	TeleportBeam->SetupAttachment(RootComponent);
	TeleportBeam->SetVisibility(false);
	
	TeleportMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Teleport Marker"));
	TeleportMarker->SetupAttachment(RootComponent);
	TeleportMarker->SetVisibility(false);
	
	PrimaryActorTick.bCanEverTick = true;
	
	NoiseEmitter = CreateDefaultSubobject<UNoiseEmitter>(TEXT("Noise Emitter"));
	
	ThrowOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("Throw Origin"));
	ThrowOrigin->SetupAttachment(RootComponent);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATeleportStealthCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATeleportStealthCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ATeleportStealthCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATeleportStealthCharacter::Look);
		
		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ATeleportStealthCharacter::DoSprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATeleportStealthCharacter::DoSprintEnd);
		
		// Teleporting
		EnhancedInputComponent->BindAction(TeleportAction, ETriggerEvent::Started, this, &ATeleportStealthCharacter::StartTeleportAim);
		EnhancedInputComponent->BindAction(TeleportAction, ETriggerEvent::Completed, this, &ATeleportStealthCharacter::ReleaseTeleportAim);
		
		// Throwing
		EnhancedInputComponent->BindAction(ThrowAction,ETriggerEvent::Started,this,&ATeleportStealthCharacter::ThrowThrowable);
		
		// debug damage
		EnhancedInputComponent->BindAction(AutoDamageAction, ETriggerEvent::Started, this, &ATeleportStealthCharacter::DebugDamage);
	}
	else
	{
		UE_LOG(LogTeleportStealth, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATeleportStealthCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ATeleportStealthCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ATeleportStealthCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ATeleportStealthCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ATeleportStealthCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ATeleportStealthCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ATeleportStealthCharacter::DoSprintStart()
{
	UE_LOG(LogTemp, Warning, TEXT("sprint start"));
	if (bIsSprinting)
		return;
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

}

void ATeleportStealthCharacter::DoSprintEnd()
{
	if (!bIsSprinting)
		return;
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ATeleportStealthCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	CurrentHealth = MaxHealth;
	RespawnLocation = GetActorLocation();
	
	PreviousMovementLocation = GetActorLocation();
	bWasFalling = GetCharacterMovement()->IsFalling();
	DistanceSinceLastStep = 0.f;
	
	//TeleportMarker = GetWorld()->SpawnActor<ATeleportMarker>(ATeleportMarker);
	
	if (InGameUiClass)
	{
		InGameUI = CreateWidget<UInGameUI>(GetWorld(), InGameUiClass);
		if (InGameUI)
		{
			InGameUI->Player = this;
			InGameUI->AddToViewport();
			InGameUI->UpdateValues();
		}
	}
	
}

void ATeleportStealthCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsAimingTeleport)
	{
		UpdateTeleportAim();
	}
	UpdateMovementNoise();
}


void ATeleportStealthCharacter::TakeDamage(float DamageAmount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health: %f, lives: %d"), CurrentHealth, RemainingLives);
	if (CurrentHealth <= 0.f)
	{
		Die();
	}
	UpdateInGameUI();
}

void ATeleportStealthCharacter::Die()
{
	RemainingLives--;
	UE_LOG(LogTemp, Warning, TEXT("Dead. Remaining lives: %d"), RemainingLives);
	if (RemainingLives <= 0)
	{
		GameOver();
	}
	else
	{
		CurrentHealth = MaxHealth;
		SetActorLocation(RespawnLocation);
		PreviousMovementLocation = RespawnLocation;
		DistanceSinceLastStep = 0.f;
		bWasFalling = false;
		bIsSprinting = false;
		
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		
		if (NoiseEmitter)
			NoiseEmitter->EmitNoiseAtLocation(EGameplayNoiseType::Respawning, RespawnLocation);
	}
	UpdateInGameUI();
}

void ATeleportStealthCharacter::GameOver()
{
	DisableInput(nullptr);
	UE_LOG(LogTemp, Warning, TEXT("Game Over!"));
}

void ATeleportStealthCharacter::StartTeleportAim()
{
	UE_LOG(LogTemp, Warning, TEXT("Teleport"));
	if (!bCanTeleport)
		return;
	UE_LOG(LogTemp, Warning, TEXT("can teleport"));
	bIsAimingTeleport = true;
}

void ATeleportStealthCharacter::ReleaseTeleportAim()
{
	if (!bIsAimingTeleport)
		return;
	
	bIsAimingTeleport = false;
	
	if (TeleportMarker)
		TeleportMarker->SetVisibility(false);
	
	if (!bCanTeleport||!bHasValidTeleportLocation)
		return;
	
	const FVector TeleportStartLocation = GetActorLocation();
	TeleportTo(CurrentTeleportLocation, GetActorRotation());
	
	// Noise system update
	if (NoiseEmitter)
	{
		NoiseEmitter->EmitNoiseAtLocation(EGameplayNoiseType::Teleport, GetActorLocation());
	}
	PreviousMovementLocation = GetActorLocation();
	DistanceSinceLastStep = 0.f;
	
	// Teleport system update
	bCanTeleport = false;
	bHasValidTeleportLocation = false;
	GetWorldTimerManager().SetTimer(TeleportTimerHandle, this, &ATeleportStealthCharacter::ResetTeleport, TeleportCooldown, false);
	UpdateInGameUI();
}

void ATeleportStealthCharacter::UpdateTeleportAim()
{
	if (!TeleportBeam || !TeleportMarker)
		return;
	
	FVector CameraStart = FollowCamera->GetComponentLocation() ;
	FVector CameraEnd = CameraStart + FollowCamera->GetForwardVector() * AimTraceDistance ;
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	TraceParams.AddIgnoredActors(ActorsToIgnore);
	
	FHitResult HitData;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitData, CameraStart, CameraEnd, ECC_Visibility, TraceParams);
	
	if (bHit)
	{
		FVector CandidateLocation = HitData.Location;
		float DistanceFromPlayer = FVector::Distance(GetActorLocation(), CandidateLocation);
		if (DistanceFromPlayer <= MaxTeleportDistance)
		{
			CurrentTeleportLocation = CandidateLocation;
			bHasValidTeleportLocation = true;
			TeleportMarker->SetWorldLocation(CandidateLocation);
		}
		else
		{
			bHasValidTeleportLocation = false;
		}
	}
	else
	{
		bHasValidTeleportLocation = false;
	}
	TeleportMarker->SetVisibility(bHasValidTeleportLocation);
	
}

void ATeleportStealthCharacter::ResetTeleport()
{
	bCanTeleport = true;
	UpdateInGameUI();
}

float ATeleportStealthCharacter::GetTeleportCooldownRemaining()
{
	if (bCanTeleport)
		return 0.f;
	const float Remaining = GetWorldTimerManager().GetTimerRemaining(TeleportTimerHandle);
	return FMath::Max(Remaining, 0.f);
}

void ATeleportStealthCharacter::UpdateInGameUI()
{
	if (InGameUI)
		InGameUI->UpdateValues();
}

void ATeleportStealthCharacter::UpdateMovementNoise()
{
	if (!NoiseEmitter)
		return;
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement)
		return;
	const FVector CurrentLocation = GetActorLocation();
	const bool bIsFalling = Movement->IsFalling();
	
	// Landing detection
	if (bWasFalling && !bIsFalling)
	{
		NoiseEmitter -> EmitNoise(EGameplayNoiseType::Landing);
		DistanceSinceLastStep = 0.f;
	}
	bWasFalling = bIsFalling;
	
	// No footsteps while in air
	if (bIsFalling)
	{
		PreviousMovementLocation = CurrentLocation;
		return;
	}
	
	const float HorizontalSpeed = Movement->Velocity.Size2D();
	if (HorizontalSpeed < MinimumMovementSpeed)
	{
		PreviousMovementLocation = CurrentLocation;
		DistanceSinceLastStep = 0.f;
		return;
	}
	
	const float TravelledDistance = FVector::Dist2D(CurrentLocation, PreviousMovementLocation);
	PreviousMovementLocation = CurrentLocation;
	DistanceSinceLastStep += TravelledDistance;
	
	const float RequiredDistance = bIsSprinting? SprintStepDistance: WalkStepDistance;
	if (DistanceSinceLastStep < RequiredDistance)
		return;
	DistanceSinceLastStep = 0.f;
	NoiseEmitter -> EmitNoise(bIsSprinting? EGameplayNoiseType::Sprint:EGameplayNoiseType::Walk);
}

void ATeleportStealthCharacter::ThrowThrowable()
{
	if (CurrentThrowableCount <= 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("No throwable objects available"));
		return;
	}

	if (!ThrowableRockClass || !ThrowOrigin)
		return;

	FVector SpawnLocation =ThrowOrigin->GetComponentLocation();

	FRotator SpawnRotation =FollowCamera->GetComponentRotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AThrowableObject* Rock = GetWorld()->SpawnActor<AThrowableObject>(
			ThrowableRockClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParameters
		);

	if (!Rock)
		return;

	FVector ThrowDirection = FollowCamera->GetForwardVector() + FVector::UpVector * ThrowUpwardAngle;

	Rock->Launch(ThrowDirection,this);

	CurrentThrowableCount--;
	
	UpdateInGameUI();

	UE_LOG(LogTemp,Warning,TEXT("Throwable used: %d/%d"),CurrentThrowableCount,MaxThrowableCount);
}

bool ATeleportStealthCharacter::TryCollectThrowable()
{
    if (CurrentThrowableCount >= MaxThrowableCount)
    {
        UE_LOG(LogTemp,Warning,TEXT("Throwable inventory full"));
        return false;
    }

    CurrentThrowableCount++;
	UpdateInGameUI();
    UE_LOG(LogTemp,Warning,TEXT("Throwable collected: %d/%d"),CurrentThrowableCount,MaxThrowableCount);
    return true;
}

void ATeleportStealthCharacter::CollectObjective(FName ObjectiveName, int32 ObjectiveValue)
{
	CollectedObjectiveCount++;
	CollectedObjectiveValue += ObjectiveValue;

	UE_LOG(LogTemp,Warning,TEXT("Collected objective: %s | Total: %d/%d | Value: %d"),
		*ObjectiveName.ToString(),
		CollectedObjectiveCount,
		RequiredObjectiveCount,
		CollectedObjectiveValue
	);

	if (HasCollectedAllObjectives())
	{
		UE_LOG(LogTemp,Warning,TEXT("All required objectives collected"));
	}
	UpdateInGameUI();
}

bool ATeleportStealthCharacter::HasCollectedAllObjectives() const
{
	return CollectedObjectiveCount >= RequiredObjectiveCount;
}
