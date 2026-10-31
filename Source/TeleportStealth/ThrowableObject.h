// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableObject.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNoiseEmitter;

UCLASS()
class TELEPORTSTEALTH_API AThrowableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowableObject();
	
    void Launch(const FVector& Direction, AActor* ThrowingActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Throwable")
    UStaticMeshComponent* RockMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Throwable")
    UNoiseEmitter* NoiseEmitter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable")
    float ThrowImpulse = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable")
    float MinimumImpactSpeed = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable")
    float PickupEnableDelay = 0.75f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable")
    float ImpactNoiseCooldown = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable")
    float LifeSpanAfterThrow = 20.f;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    bool bCanBePickedUp = true;
    bool bCanGenerateImpactNoise = true;
    bool bWasThrown = false;

    FTimerHandle PickupTimerHandle;
    FTimerHandle ImpactNoiseTimerHandle;

    UFUNCTION()
    void OnPickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnRockHit(
        UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse,
        const FHitResult& Hit
    );

    void EnablePickup();
    void ResetImpactNoise();

};

