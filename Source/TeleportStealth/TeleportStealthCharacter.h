// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TeleportStealthCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UNoiseEmitter;
class AThrowableObject;
class USceneComponent;
class UInGameUI;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ATeleportStealthCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SprintAction;
	
	/** Teleport Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* TeleportAction;
	
	/** Throw Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ThrowAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputMappingContext* DefaultMappingContext;
	
	
public:

	/** Constructor */
	ATeleportStealthCharacter();	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoSprintStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoSprintEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = 100.0f;
	
	UPROPERTY(EditAnywhere)
	float SprintSpeed = 900.f;
	UPROPERTY(EditAnywhere)
	float WalkSpeed = 500.f;
	
	UPROPERTY(EditAnywhere)
	int32 MaxLives = 3;
	UPROPERTY(EditAnywhere)
	int32 RemainingLives = 3;
	
	FVector RespawnLocation;
	
	void TakeDamage(float DamageAmount = 20.f);
	
	void Die();
	void GameOver();
	
	// Teleport
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float AimTraceDistance = 5000.f;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float MaxTeleportDistance = 2000.f;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportOffset = 90.f;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportCooldown = 10.f;
	FTimerHandle TeleportTimerHandle;
	bool bCanTeleport = true;
	bool bIsAimingTeleport = false;
	bool bHasValidTeleportLocation = false;
	FVector CurrentTeleportLocation;
	
	UPROPERTY(EditAnywhere, Category = "Teleport")
	FVector BeamStartOffset = FVector(40.f, 40.f,0.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	UStaticMeshComponent* TeleportBeam;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	UStaticMeshComponent* TeleportMarker;
	
	void StartTeleportAim();
	void ReleaseTeleportAim();
	void UpdateTeleportAim();
	void ResetTeleport();
	
	float GetTeleportCooldownRemaining();
	
	// Sound system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Noise", meta = (AllowPrivateAccess = "true"))
	UNoiseEmitter* NoiseEmitter;
	void UpdateMovementNoise();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Footsteps")
	float WalkStepDistance = 180.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Footsteps")
	float SprintStepDistance = 230.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Footsteps")
	float MinimumMovementSpeed = 10.f;
	float DistanceSinceLastStep = 0.f;
	FVector PreviousMovementLocation;
	bool bIsSprinting = false;
	bool bWasFalling = false;
	
	// Throw
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Throwable")
	USceneComponent* ThrowOrigin;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Throwable")
	TSubclassOf<AThrowableObject> ThrowableRockClass;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Throwable")
	int32 MaxThrowableCount = 3;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Throwable")
	int32 CurrentThrowableCount = 0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Throwable")
	float ThrowUpwardAngle = 0.15f;
	UFUNCTION()
	void ThrowThrowable();
	bool TryCollectThrowable(); 
	
	// Objectives collection
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Objectives")
	int32 CollectedObjectiveCount = 0;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Objectives")
	int32 CollectedObjectiveValue = 0;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "Objectives")
	int32 RequiredObjectiveCount = 3;
	void CollectObjective(FName ObjectiveName, int32 ObjectiveValue);
	bool HasCollectedAllObjectives() const;
	
	// UI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInGameUI> InGameUiClass;
	UPROPERTY()
	UInGameUI* InGameUI;
	void UpdateInGameUI();
	
	/** Debug damage */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AutoDamageAction;
	void DebugDamage() {TakeDamage();};
};

