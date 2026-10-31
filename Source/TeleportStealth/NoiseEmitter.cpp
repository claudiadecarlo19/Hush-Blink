// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseEmitter.h"
#include "Perception/AISense_Hearing.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UNoiseEmitter::UNoiseEmitter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

    WalkNoise.Radius = 300.f;
    WalkNoise.Cooldown = 0.35f;
    WalkNoise.Tag = TEXT("Walk");

    SprintNoise.Radius = 700.f;
    SprintNoise.Cooldown = 0.22f;
    SprintNoise.Tag = TEXT("Sprint");

    LandingNoise.Radius = 550.f;
    LandingNoise.Cooldown = 0.2f;
    LandingNoise.Tag = TEXT("Landing");

    TeleportNoise.Radius = 1600.f;
    TeleportNoise.Cooldown = 0.f;
    TeleportNoise.Tag = TEXT("Teleport");
    
    RespawningNoise.Radius = 1200.f;
    RespawningNoise.Cooldown = 0.f;
    RespawningNoise.Tag = TEXT("Teleport");
    
    ObjectivePickupNoise.Radius = 1400.f;
    ObjectivePickupNoise.Cooldown = 0.f;
    ObjectivePickupNoise.Tag = TEXT("Teleport");

    ThrowableNoise.Radius = 1000.f;
    ThrowableNoise.Cooldown = 0.1f;
    ThrowableNoise.Tag = TEXT("Throwable");
}

bool UNoiseEmitter::EmitNoise(EGameplayNoiseType NoiseType)
{
    const AActor* Owner = GetOwner();

    if (!Owner)
    {
        return false;
    }

    return EmitNoiseAtLocation(
        NoiseType,
        Owner->GetActorLocation()
    );
}

bool UNoiseEmitter::EmitNoiseAtLocation(EGameplayNoiseType NoiseType, const FVector& NoiseLocation)
{
    AActor* Owner = GetOwner();

    if (!Owner || !GetWorld())
    {
        return false;
    }

    const FGameplayNoiseSettings& Settings =
        GetNoiseSettings(NoiseType);

    if (!Settings.bEnabled || Settings.Radius <= 0.f)
    {
        return false;
    }

    const float CurrentTime = GetWorld()->GetTimeSeconds();

    if (const float* LastTime = LastEmissionTimes.Find(NoiseType))
    {
        if (CurrentTime - *LastTime < Settings.Cooldown)
        {
            return false;
        }
    }

    LastEmissionTimes.Add(NoiseType, CurrentTime);

    UAISense_Hearing::ReportNoiseEvent(
        GetWorld(),
        NoiseLocation,
        1.f,
        Owner,
        Settings.Radius,
        Settings.Tag
    );
    
    if (Settings.Sound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, Settings.Sound, NoiseLocation, Settings.Volume, Settings.Pitch);
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("Noise emitted: %s | Radius: %.0f | Location: %s"),
        *Settings.Tag.ToString(),
        Settings.Radius,
        *NoiseLocation.ToString()
    );

    if (bDrawDebugNoise)
    {
        DrawDebugSphere(
            GetWorld(),
            NoiseLocation,
            Settings.Radius,
            32,
            GetDebugColour(NoiseType),
            false,
            DebugDuration,
            0,
            2.f
        );
    }

    return true;
}

const FGameplayNoiseSettings& UNoiseEmitter::GetNoiseSettings(EGameplayNoiseType NoiseType) const
{
    switch (NoiseType)
    {
        case EGameplayNoiseType::Walk:
            return WalkNoise;

        case EGameplayNoiseType::Sprint:
            return SprintNoise;

        case EGameplayNoiseType::Landing:
            return LandingNoise;

        case EGameplayNoiseType::Teleport:
            return TeleportNoise;
        
        case EGameplayNoiseType::Respawning:
            return RespawningNoise;
            
        case EGameplayNoiseType::ObjectivePickup:
            return ObjectivePickupNoise;

        case EGameplayNoiseType::Throwable:
            return ThrowableNoise;

        default:
            return WalkNoise;
    }
}

FColor UNoiseEmitter::GetDebugColour(EGameplayNoiseType NoiseType) const
{
    switch (NoiseType)
    {
        case EGameplayNoiseType::Walk:
            return FColor::Green;

        case EGameplayNoiseType::Sprint:
            return FColor::Yellow;

        case EGameplayNoiseType::Landing:
            return FColor::Orange;

        case EGameplayNoiseType::Teleport:
            return FColor::Red;
        
        case EGameplayNoiseType::Respawning:
            return FColor::Black;
        
        case EGameplayNoiseType::ObjectivePickup:
            return FColor::Purple;

        case EGameplayNoiseType::Throwable:
            return FColor::Blue;

        default:
            return FColor::White;
    }
}


// Called when the game starts
void UNoiseEmitter::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UNoiseEmitter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
