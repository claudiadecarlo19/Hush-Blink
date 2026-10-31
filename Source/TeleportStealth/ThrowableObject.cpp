// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableObject.h"
#include "NoiseEmitter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TeleportStealthCharacter.h"
#include "TimerManager.h"

// Sets default values
AThrowableObject::AThrowableObject()
{
 	PrimaryActorTick.bCanEverTick = false;
    
    RockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rock Mesh"));
    RootComponent = RockMesh;
    RockMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    RockMesh->SetCollisionObjectType(ECC_PhysicsBody);
    RockMesh->SetCollisionResponseToAllChannels(ECR_Block);
    RockMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    
    RockMesh->SetGenerateOverlapEvents(true);
    RockMesh->SetNotifyRigidBodyCollision(true);
    RockMesh->SetSimulatePhysics(true);
    
    NoiseEmitter = CreateDefaultSubobject<UNoiseEmitter>(TEXT("Noise Emitter"));
}

void AThrowableObject::Launch(const FVector& Direction, AActor* ThrowingActor)
{
    bWasThrown = true;
    bCanBePickedUp = false;

    SetOwner(ThrowingActor);
    RockMesh->WakeAllRigidBodies();
    FVector NormalizedDirection = Direction.GetSafeNormal();
    RockMesh->AddImpulse(NormalizedDirection * ThrowImpulse);
    GetWorldTimerManager().SetTimer(PickupTimerHandle,this,&AThrowableObject::EnablePickup,PickupEnableDelay,false);
    //SetLifeSpan(LifeSpanAfterThrow);
}

void AThrowableObject::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bCanBePickedUp || !OtherActor)
        return;

    ATeleportStealthCharacter* Player = Cast<ATeleportStealthCharacter>(OtherActor);

    if (!Player)
        return;

    if (Player->TryCollectThrowable())
        Destroy();
}

void AThrowableObject::OnRockHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!bWasThrown || !bCanGenerateImpactNoise)
        return;

    const float ImpactSpeed = RockMesh->GetPhysicsLinearVelocity().Size();

    if (ImpactSpeed < MinimumImpactSpeed)
        return;

    if (NoiseEmitter)
    {
        NoiseEmitter->EmitNoiseAtLocation(EGameplayNoiseType::Throwable,Hit.ImpactPoint);
    }

    bCanGenerateImpactNoise = false;

    GetWorldTimerManager().SetTimer(ImpactNoiseTimerHandle,this,&AThrowableObject::ResetImpactNoise,ImpactNoiseCooldown,false);
}

void AThrowableObject::EnablePickup()
{
    bCanBePickedUp = true;
}

void AThrowableObject::ResetImpactNoise()
{
    bCanGenerateImpactNoise = true;
}

// Called when the game starts or when spawned
void AThrowableObject::BeginPlay()
{
    Super::BeginPlay();

    RockMesh->OnComponentBeginOverlap.AddDynamic(this, &AThrowableObject::OnPickupOverlap);
    RockMesh->OnComponentHit.AddDynamic(this,&AThrowableObject::OnRockHit);
}

// Called every frame
void AThrowableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
