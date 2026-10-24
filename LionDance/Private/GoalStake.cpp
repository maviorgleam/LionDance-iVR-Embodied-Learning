// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalStake.h"
#include "Components/BoxComponent.h"
#include "LionCharacter.h"
#include "LionGameMode.h"
#include "Kismet/GameplayStatics.h"

AGoalStake::AGoalStake()
{
    GoalTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("GoalTrigger"));
    GoalTrigger->SetupAttachment(RootComponent);
    GoalTrigger->SetRelativeLocation(FVector(0,0,120));
    GoalTrigger->SetBoxExtent(FVector(50,50,40));
    GoalTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    GoalTrigger->OnComponentBeginOverlap.AddDynamic(this, &AGoalStake::OnGoalOverlap);
}
void AGoalStake::OnGoalOverlap(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (Other && Other->IsA(ALionCharacter::StaticClass()))
        if (auto* GM = Cast<ALionGameMode>(UGameplayStatics::GetGameMode(this)))
            GM->OnPlayerSucceeded();
}
