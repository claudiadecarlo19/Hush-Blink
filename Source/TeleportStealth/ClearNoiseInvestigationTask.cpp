
#include "ClearNoiseInvestigationTask.h"

#include "NoiseTrackingAIController.h"


UClearNoiseInvestigationTask::UClearNoiseInvestigationTask()
{
	NodeName = TEXT("Clear Noise Investigation Task");
}


EBTNodeResult::Type UClearNoiseInvestigationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	ANoiseTrackingAIController* Controller =Cast<ANoiseTrackingAIController>(OwnerComp.GetAIOwner());

	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	Controller->ClearNoiseInvestigation();

	return EBTNodeResult::Succeeded;
}



