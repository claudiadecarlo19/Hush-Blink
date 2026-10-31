
#include "GenerateSearchLocationTask.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "NoiseTrackingAIController.h"


UGenerateSearchLocationTask::UGenerateSearchLocationTask()
{
	NodeName = TEXT("Generate Search Location");
}


EBTNodeResult::Type UGenerateSearchLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	ANoiseTrackingAIController* Controller =Cast<ANoiseTrackingAIController>(OwnerComp.GetAIOwner());

	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	Controller->GenerateSearchLocation();

	return EBTNodeResult::Succeeded;
}



