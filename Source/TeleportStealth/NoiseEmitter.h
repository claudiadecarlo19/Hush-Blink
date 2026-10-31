// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoiseEmitter.generated.h"

class USoundBase;

UENUM(BlueprintType)
enum class EGameplayNoiseType : uint8
{
    Walk                UMETA(DisplayName = "Walk"),
    Sprint              UMETA(DisplayName = "Sprint"),
    Landing             UMETA(DisplayName = "Landing"),
    Throwable           UMETA(DisplayName = "Throwable"),
    Respawning          UMETA(DisplayName = "Respawning"),
    ObjectivePickup     UMETA(DisplayName = "Objective Pickup"),
    Teleport            UMETA(DisplayName = "Teleport"),
};


USTRUCT(BlueprintType)
struct FGameplayNoiseSettings
{
    GENERATED_BODY()

    // Effective gameplay range of the noise.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius = 500.f;

    // Minimum time before another noise of the same type can be emitted.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Cooldown = 0.f;

    // Identifier that the enemy AI will later be able to inspect.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Tag = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEnabled = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* Sound = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float Volume = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float Pitch = 1.f;
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TELEPORTSTEALTH_API UNoiseEmitter : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNoiseEmitter();
    
    UFUNCTION(BlueprintCallable, Category = "Noise")
    bool EmitNoise(EGameplayNoiseType NoiseType);

    UFUNCTION(BlueprintCallable, Category = "Noise")
    bool EmitNoiseAtLocation(
        EGameplayNoiseType NoiseType,
        const FVector& NoiseLocation
    );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Settings")
    FGameplayNoiseSettings WalkNoise;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Settings")
    FGameplayNoiseSettings SprintNoise;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Settings")
    FGameplayNoiseSettings LandingNoise;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Settings")
    FGameplayNoiseSettings TeleportNoise;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Settings")
    FGameplayNoiseSettings RespawningNoise;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Settings")
    FGameplayNoiseSettings ObjectivePickupNoise;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Settings")
    FGameplayNoiseSettings ThrowableNoise;

    // Temporary visualisation for testing the system before the enemy exists.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Debug")
    bool bDrawDebugNoise = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise|Debug")
    float DebugDuration = 0.5f;
    
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
private:
    TMap<EGameplayNoiseType, float> LastEmissionTimes;

    const FGameplayNoiseSettings& GetNoiseSettings(
        EGameplayNoiseType NoiseType
    ) const;

    FColor GetDebugColour(EGameplayNoiseType NoiseType) const;
    
};