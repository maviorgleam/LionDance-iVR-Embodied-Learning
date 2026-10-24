// Fill out your copyright notice in the Description page of Project Settings.


#include "LionGameMode.h"
#include "LionPlayerController.h"
#include "LionCharacter.h"
#include "LionGameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

ALionGameMode::ALionGameMode() {}

void ALionGameMode::BeginPlay()
{
    Super::BeginPlay();
    if (GetWorld() && GetWorld()->IsPlayingReplay()) return; // 回放世界不进菜单流程
    SetPhase(ELionPhase::Menu);
}

ALionPlayerController* ALionGameMode::GetLionPC() const
{ return Cast<ALionPlayerController>(UGameplayStatics::GetPlayerController(this,0)); }
ALionCharacter* ALionGameMode::GetLion() const
{ return Cast<ALionCharacter>(UGameplayStatics::GetPlayerPawn(this,0)); }

void ALionGameMode::SetPhase(ELionPhase New)
{
    Phase = New;
    auto* PC = GetLionPC(); auto* Lion = GetLion();
    if (!PC) return;

    switch (Phase)
    {
    case ELionPhase::Menu:
        PC->EnterUIMode();
        PC->SetHeadOverlayVisible(false);   // 无遮挡
        PC->ShowChargeBar(false);
        PC->HideResultScreen();
        PC->ShowMainMenu();
        if (Lion) Lion->SetJumpEnabled(false);
        break;

    case ELionPhase::Playing:
        PC->HideMainMenu();
        PC->EnterGameMode();
        PC->SetHeadOverlayVisible(true);    // 瞬间戴上狮头遮罩(无过场动画)
        if (auto* GI = GetGameInstance<ULionGameInstance>()) GI->StartReplayRecording();
        if (Lion) Lion->SetJumpEnabled(true);
        PC->ShowChargeBar(true);
        // ← 新增：播放开始游戏音效
        UGameplayStatics::PlaySound2D(this, StartGameSound);
        break;

    case ELionPhase::Failed:
        if (Lion) Lion->SetJumpEnabled(false);
        PC->ShowChargeBar(false);
        if (auto* GI = GetGameInstance<ULionGameInstance>()) GI->StopReplayRecording();
        UGameplayStatics::PlaySound2D(this, FailSound);
        PC->ShowFailScreen();
        PC->EnterUIMode();
        GetWorldTimerManager().SetTimer(TimerHandle, this, &ALionGameMode::AutoReturnToMenu, ResultHoldTime, false);
        break;

    case ELionPhase::Succeeded:
        if (Lion) Lion->SetJumpEnabled(false);
        PC->ShowChargeBar(false);
        if (auto* GI = GetGameInstance<ULionGameInstance>()) GI->StopReplayRecording();
        UGameplayStatics::PlaySound2D(this, SuccessSound);
        PC->ShowSuccessScreen();
        PC->EnterUIMode();
        GetWorldTimerManager().SetTimer(TimerHandle, this, &ALionGameMode::AutoReturnToMenu, ResultHoldTime, false);
        break;
    }
}

void ALionGameMode::StartGameFlow()     { if (Phase==ELionPhase::Menu)    SetPhase(ELionPhase::Playing); }
void ALionGameMode::OnPlayerFailed()    { if (Phase==ELionPhase::Playing) SetPhase(ELionPhase::Failed); }
void ALionGameMode::OnPlayerSucceeded() { if (Phase==ELionPhase::Playing) SetPhase(ELionPhase::Succeeded); }

void ALionGameMode::AutoReturnToMenu()
{
    if (auto* GI = GetGameInstance<ULionGameInstance>()) GI->ReturnToMenuLevel();
}

