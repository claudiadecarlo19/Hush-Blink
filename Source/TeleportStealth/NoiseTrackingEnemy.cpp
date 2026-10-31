// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseTrackingEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TeleportStealthCharacter.h"
#include "TimerManager.h"

ANoiseTrackingEnemy::ANoiseTrackingEnemy()
{
    PrimaryActorTick.bCanEverTick = false;
    
    GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate =FRotator(0.f, 400.f, 0.f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void ANoiseTrackingEnemy::BeginPlay()
{
    Super::BeginPlay();

    GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed;
}


bool ANoiseTrackingEnemy::TryAttackPlayer(
    ATeleportStealthCharacter* Player)
{
    if (!Player || !bCanAttack)
        return false;

    const float Distance = FVector::Distance(GetActorLocation(),Player->GetActorLocation());

    if (Distance > AttackRange)
        return false;

    Player->TakeDamage(AttackDamage);
    
    FVector KnockbackDirection =Player->GetActorLocation() -GetActorLocation();
    KnockbackDirection.Z = 0.f;
    KnockbackDirection.Normalize();
    
    FVector LaunchVelocity =
        KnockbackDirection * KnockbackStrength + FVector::UpVector * KnockbackUpwardStrength;
    
    Player->LaunchCharacter(LaunchVelocity,true,true);
    bCanAttack = false;
    GetWorldTimerManager().SetTimer(AttackCooldownTimer,this,&ANoiseTrackingEnemy::ResetAttack,AttackCooldown,false);
    return true;
}


void ANoiseTrackingEnemy::ResetAttack()
{
    bCanAttack = true;
}


void ANoiseTrackingEnemy::EnterEnragedState()
{
    bIsEnraged = true;

    GetCharacterMovement()->MaxWalkSpeed =EnragedMovementSpeed;
    GetWorldTimerManager().ClearTimer(EnragedTimer);
    GetWorldTimerManager().SetTimer(EnragedTimer,this,&ANoiseTrackingEnemy::ExitEnragedState,EnragedDuration,false);
}


void ANoiseTrackingEnemy::ExitEnragedState()
{
    bIsEnraged = false;
    GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed;
}

