// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ClearNoiseInvestigationTask.generated.h"

/**
 * 
 */
UCLASS()
class TELEPORTSTEALTH_API UClearNoiseInvestigationTask : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UClearNoiseInvestigationTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory) override;
};
