// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectibleObjective.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NoiseEmitter.h"
#include "TeleportStealthCharacter.h"
#include "TimerManager.h"

// Sets default values
ACollectibleObjective::ACollectibleObjective()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
    RootComponent = SceneRoot;

    ObjectiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Objective Mesh"));
    ObjectiveMesh->SetupAttachment(SceneRoot);
    ObjectiveMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PickupCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Pickup Collider"));
    PickupCollider->SetupAttachment(SceneRoot);
    PickupCollider->InitSphereRadius(70.f);
    PickupCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupCollider->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
    PickupCollider->SetGenerateOverlapEvents(true);
    
    NoiseEmitter = CreateDefaultSubobject<UNoiseEmitter>(TEXT("Noise Emitter"));

}

// Called when the game starts or when spawned
void ACollectibleObjective::BeginPlay()
{
	Super::BeginPlay();
	
    PickupCollider->OnComponentBeginOverlap.AddDynamic(this,&ACollectibleObjective::OnPickupOverlap);
}

// Called every frame
void ACollectibleObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACollectibleObjective::OnPickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (bHasBeenCollected || !OtherActor)
        return;

    ATeleportStealthCharacter* Player = Cast<ATeleportStealthCharacter>(OtherActor);

    if (!Player)
        return;

    bHasBeenCollected = true;

    Player->CollectObjective(ObjectiveName,ObjectiveValue);

    if (bGeneratePickupNoise && NoiseEmitter)
    {
        NoiseEmitter->EmitNoiseAtLocation(EGameplayNoiseType::ObjectivePickup,GetActorLocation());
    }

    CompletePickup();
}


void ACollectibleObjective::CompletePickup()
{
    PickupCollider->SetCollisionEnabled(
        ECollisionEnabled::NoCollision
    );

    ObjectiveMesh->SetVisibility(false);

    if (DestroyDelay <= 0.f)
    {
        Destroy();
        return;
    }

    SetLifeSpan(DestroyDelay);
}




