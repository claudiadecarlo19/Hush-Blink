// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectibleObjective.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
class UNoiseEmitter;

UCLASS()
class TELEPORTSTEALTH_API ACollectibleObjective : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectibleObjective();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objective")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objective")
	UStaticMeshComponent* ObjectiveMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objective")
	USphereComponent* PickupCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objective")
	UNoiseEmitter* NoiseEmitter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FName ObjectiveName = TEXT("Valuable");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	int32 ObjectiveValue = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bGeneratePickupNoise = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	float DestroyDelay = 0.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bHasBeenCollected = false;

	UFUNCTION()
	void OnPickupOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void CompletePickup();
};
