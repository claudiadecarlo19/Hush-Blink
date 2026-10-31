// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackPlayerTask.h"

#include "NoiseTrackingAIController.h"

UAttackPlayerTask::UAttackPlayerTask()
{
	NodeName = TEXT("Attack Player");
}


EBTNodeResult::Type UAttackPlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
	ANoiseTrackingAIController* Controller =Cast<ANoiseTrackingAIController>(OwnerComp.GetAIOwner());

	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	Controller->TryAttackPlayer();

	return EBTNodeResult::Succeeded;
}