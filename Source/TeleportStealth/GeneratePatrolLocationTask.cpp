#include "GeneratePatrolLocationTask.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "NoiseTrackingAIController.h"


UGeneratePatrolLocationTask::UGeneratePatrolLocationTask()
{
	NodeName = TEXT("Generate Patrol Location");
}


EBTNodeResult::Type UGeneratePatrolLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	ANoiseTrackingAIController* Controller = Cast<ANoiseTrackingAIController>(OwnerComp.GetAIOwner());

	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	Controller->GeneratePatrolLocation();

	return EBTNodeResult::Succeeded;
}
