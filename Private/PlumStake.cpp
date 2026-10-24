// Fill out your copyright notice in the Description page of Project Settings.


#include "PlumStake.h"
#include "Components/StaticMeshComponent.h"

APlumStake::APlumStake()
{
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    Mesh->SetCollisionProfileName(TEXT("BlockAll")); // 角色能站上去
}
