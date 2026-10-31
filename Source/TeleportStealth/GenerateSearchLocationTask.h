// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GenerateSearchLocationTask.generated.h"

/**
 * 
 */
UCLASS()
class TELEPORTSTEALTH_API UGenerateSearchLocationTask : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UGenerateSearchLocationTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory) override;
};
