// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LionGameInstance.generated.h"

UCLASS()
class LIONDANCE_API ULionGameInstance : public UGameInstance
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) void StartReplayRecording();
    UFUNCTION(BlueprintCallable) void StopReplayRecording();
    UFUNCTION(BlueprintCallable) void PlayLastReplay();
    UFUNCTION(BlueprintCallable) void ReturnToMenuLevel(); // 重载关卡=回到实时菜单

    UPROPERTY(EditDefaultsOnly, Category="Replay") FString ReplayName = TEXT("LionReplay");
    UPROPERTY(EditDefaultsOnly, Category="Replay") FName   MainLevelName = TEXT("MainLevel");
};
