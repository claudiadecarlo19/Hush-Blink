#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NoiseTrackingAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Hearing;
class UBlackboardData;
class UBehaviorTree;
class UBlackboardComponent;
class UNavigationSystemV1;

UCLASS()
class TELEPORTSTEALTH_API ANoiseTrackingAIController : public AAIController
{
    GENERATED_BODY()

public:
    ANoiseTrackingAIController();

    virtual void BeginPlay() override;

    void GeneratePatrolLocation();

    void GenerateSearchLocation();

    void ClearNoiseInvestigation();

    void TryAttackPlayer();

public:

    UPROPERTY(EditAnywhere, Category = "AI|Perception")
    UAISenseConfig_Hearing* HearingConfiguration;

    UPROPERTY(EditAnywhere, Category = "AI|Perception")
    float HearingRange = 2500.f;

    UPROPERTY(EditAnywhere, Category = "AI|Perception")
    float HearingMemory = 4.f;

    UPROPERTY(VisibleAnywhere, Category = "AI|Navigation")
    float PatrolRadius = 1500.f;

    UPROPERTY(EditAnywhere, Category = "AI|Navigation")
    float SearchRadius = 600.f;

    UPROPERTY(EditAnywhere, Category = "AI")
    UBlackboardData* BlackboardData;

    UPROPERTY(EditAnywhere, Category = "AI")
    UBehaviorTree* BehaviourTree;
    
    UPROPERTY()
    UBlackboardComponent* BlackboardComponent;

    UPROPERTY()
    UNavigationSystemV1* NavigationSystem;

private:

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor,FAIStimulus Stimulus);

    FVector LastHeardNoiseLocation;

    TWeakObjectPtr<AActor> LastNoiseActor;
};

