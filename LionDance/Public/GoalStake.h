// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "PlumStake.h"
#include "GoalStake.generated.h"

UCLASS()
class LIONDANCE_API AGoalStake : public APlumStake
{
    GENERATED_BODY()
public:
    AGoalStake();
protected:
    UPROPERTY(VisibleAnywhere) class UBoxComponent* GoalTrigger;
    UFUNCTION() void OnGoalOverlap(UPrimitiveComponent* C, AActor* Other,
        UPrimitiveComponent* OC, int32 i, bool b, const FHitResult& H);
};
