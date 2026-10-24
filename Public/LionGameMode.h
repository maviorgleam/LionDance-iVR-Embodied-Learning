// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LionGameMode.generated.h"

UENUM(BlueprintType)
enum class ELionPhase : uint8 { Menu, Playing, Failed, Succeeded };

UCLASS()
class LIONDANCE_API ALionGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ALionGameMode();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable) void StartGameFlow();    // 菜单→开始玩
    UFUNCTION(BlueprintCallable) void OnPlayerFailed();   // 坠落
    UFUNCTION(BlueprintCallable) void OnPlayerSucceeded();// 到终点

    UPROPERTY(EditDefaultsOnly) float ResultHoldTime = 5.0f; // 结算停留(超时回菜单)
    //bgm
	UPROPERTY(EditDefaultsOnly, Category="Audio")
    USoundBase* StartGameSound;
	UPROPERTY(EditDefaultsOnly, Category="Audio") 
	USoundBase* FailSound;
    UPROPERTY(EditDefaultsOnly, Category="Audio") 
	USoundBase* SuccessSound;

protected:
    ELionPhase Phase = ELionPhase::Menu;
    FTimerHandle TimerHandle;

    void SetPhase(ELionPhase New);
    void AutoReturnToMenu();

    class ALionPlayerController* GetLionPC() const;
    class ALionCharacter* GetLion() const;
};
