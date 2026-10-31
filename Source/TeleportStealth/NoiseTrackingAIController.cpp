#include "NoiseTrackingAIController.h"

#include "NoiseTrackingEnemy.h"
#include "TeleportStealthCharacter.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"

#include "NavigationSystem.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"


ANoiseTrackingAIController::ANoiseTrackingAIController()
{
    PrimaryActorTick.bCanEverTick = false;

    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));

    HearingConfiguration = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Configuration"));
    HearingConfiguration->HearingRange = HearingRange;
    HearingConfiguration->SetMaxAge(HearingMemory);
    HearingConfiguration->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfiguration->DetectionByAffiliation.bDetectFriendlies = true;
    HearingConfiguration->DetectionByAffiliation.bDetectNeutrals = true;

    PerceptionComponent->ConfigureSense(*HearingConfiguration);
    PerceptionComponent->SetDominantSense(HearingConfiguration->GetSenseImplementation());
    SetPerceptionComponent(*PerceptionComponent);
}

void ANoiseTrackingAIController::BeginPlay()
{
    Super::BeginPlay();

    NavigationSystem =UNavigationSystemV1::GetCurrent(GetWorld());

    if (HearingConfiguration)
    {
        HearingConfiguration->HearingRange = HearingRange;
        HearingConfiguration->SetMaxAge(HearingMemory);
        PerceptionComponent->ConfigureSense(*HearingConfiguration);
    }

    if (BlackboardData)
    {
        UseBlackboard(BlackboardData,BlackboardComponent);
    }

    if (BehaviourTree)
    {
        RunBehaviorTree(BehaviourTree);
    }

    if (PerceptionComponent)
    {
        PerceptionComponent->OnTargetPerceptionUpdated
            .AddDynamic(this,&ANoiseTrackingAIController::OnTargetPerceptionUpdated);
    }
}

void ANoiseTrackingAIController::GeneratePatrolLocation()
{
    if (!NavigationSystem ||!GetPawn() ||!BlackboardComponent)
        return;

    FNavLocation RandomLocation;
    
    const bool bFoundLocation = NavigationSystem->GetRandomReachablePointInRadius(
        GetPawn()->GetActorLocation(),PatrolRadius,RandomLocation
        );

    if (bFoundLocation)
    {
        BlackboardComponent->SetValueAsVector(TEXT("PatrolLocation"),RandomLocation.Location);
    }

}

void ANoiseTrackingAIController::GenerateSearchLocation()
{
    if (!NavigationSystem ||!BlackboardComponent)
        return;

    const FVector NoiseLocation = BlackboardComponent->GetValueAsVector(TEXT("LastNoiseLocation"));

    FNavLocation RandomLocation;

    const bool bFoundLocation = NavigationSystem
            ->GetRandomReachablePointInRadius(NoiseLocation,SearchRadius,RandomLocation);

    if (bFoundLocation)
    {
        BlackboardComponent->SetValueAsVector(TEXT("SearchLocation"),RandomLocation.Location);
        BlackboardComponent->SetValueAsBool(TEXT("IsSearching"),true);
    }
}

void ANoiseTrackingAIController::ClearNoiseInvestigation()
{
    if (!BlackboardComponent)
        return;

    BlackboardComponent->SetValueAsBool(TEXT("HasHeardNoise"),false);
    BlackboardComponent->SetValueAsBool(TEXT("IsSearching"),false);
    BlackboardComponent->ClearValue(TEXT("NoiseSource"));
    BlackboardComponent->ClearValue(TEXT("NoiseTag"));
}

void ANoiseTrackingAIController::TryAttackPlayer()
{
    if (!BlackboardComponent || !GetPawn())
        return;

    AActor* NoiseSource =Cast<AActor>(BlackboardComponent->GetValueAsObject(TEXT("NoiseSource")));

    ATeleportStealthCharacter* Player =Cast<ATeleportStealthCharacter>(NoiseSource);

    if (!Player)
        return;

    ANoiseTrackingEnemy* Enemy =Cast<ANoiseTrackingEnemy>(GetPawn());

    if (!Enemy)
        return;
    
    Enemy->TryAttackPlayer(Player);
    BlackboardComponent->SetValueAsBool(TEXT("HasHeardNoise"),false);
}


void ANoiseTrackingAIController::OnTargetPerceptionUpdated(
    AActor* Actor,
    FAIStimulus Stimulus)
{
    if (!BlackboardComponent)
        return;

    if (Stimulus.Type != UAISense::GetSenseID<UAISense_Hearing>())
        return;

    if (!Stimulus.WasSuccessfullySensed())
        return;
    
    LastHeardNoiseLocation = Stimulus.StimulusLocation;
    LastNoiseActor = Actor;

    BlackboardComponent->SetValueAsVector(TEXT("LastNoiseLocation"),LastHeardNoiseLocation);
    BlackboardComponent->SetValueAsBool(TEXT("HasHeardNoise"),true);
    BlackboardComponent->SetValueAsBool(TEXT("IsSearching"),false);
    BlackboardComponent->SetValueAsObject(TEXT("NoiseSource"),Actor);
    BlackboardComponent->SetValueAsName(TEXT("NoiseTag"),Stimulus.Tag);

    UE_LOG(LogTemp,Warning,TEXT("Enemy heard noise: %s at %s"),*Stimulus.Tag.ToString(),*LastHeardNoiseLocation.ToString());
}
