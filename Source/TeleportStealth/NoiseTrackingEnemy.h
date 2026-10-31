// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NoiseTrackingEnemy.generated.h"

class ATeleportStealthCharacter;

UCLASS()
class TELEPORTSTEALTH_API ANoiseTrackingEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANoiseTrackingEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	bool TryAttackPlayer(ATeleportStealthCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void EnterEnragedState();

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void ExitEnragedState();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Movement")
	float NormalMovementSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Movement")
	float EnragedMovementSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Attack")
	float AttackRange = 160.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Attack")
	float AttackDamage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Attack")
	float KnockbackStrength = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Attack")
	float KnockbackUpwardStrength = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Attack")
	float AttackCooldown = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Enraged")
	float EnragedDuration = 8.f;

	bool bCanAttack = true;
	bool bIsEnraged = false;

	FTimerHandle AttackCooldownTimer;
	FTimerHandle EnragedTimer;

private:

	void ResetAttack();
};

