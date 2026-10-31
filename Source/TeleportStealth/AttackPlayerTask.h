// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AttackPlayerTask.generated.h"

/**
 * 
 */
UCLASS()
class TELEPORTSTEALTH_API UAttackPlayerTask : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UAttackPlayerTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory) override;
};
