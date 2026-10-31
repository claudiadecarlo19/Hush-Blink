// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GeneratePatrolLocationTask.generated.h"

/**
 * 
 */
UCLASS()
class TELEPORTSTEALTH_API UGeneratePatrolLocationTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGeneratePatrolLocationTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory) override;
};
