// Fill out your copyright notice in the Description page of Project Settings.


#include "LionPlayerController.h"
#include "LionGameMode.h"
#include "LionGameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"   // 文件顶部确认有

/*void ALionPlayerController::BeginPlay()
{
    Super::BeginPlay();
    // 回放世界:显示"退出回放"HUD。实时游戏世界 IsPlayingReplay()==false,不会误触发
    if (GetWorld() && GetWorld()->IsPlayingReplay())
    {
        ShowReplayHUD();
        EnterUIMode(); // 露出鼠标以便点退出按钮
    }
}
*/

void ALionPlayerController::BeginPlay()
{
    Super::BeginPlay();
    // 延迟 0.3s 检测，避开 DemoNetDriver 未就绪
    FTimerHandle H;
    GetWorldTimerManager().SetTimer(H, this, &ALionPlayerController::TryShowReplayHUD, 0.6f, false);
}

void ALionPlayerController::TryShowReplayHUD()   // ← 新增
{
    
    //test
    UE_LOG(LogTemp, Warning, TEXT("TryShowReplayHUD: World=%s IsReplay=%s"),
        GetWorld() ? TEXT("Valid") : TEXT("Null"),
        GetWorld() && GetWorld()->IsPlayingReplay() ? TEXT("TRUE") : TEXT("FALSE"));

    if (GetWorld() && GetWorld()->IsPlayingReplay())
    {
        ShowReplayHUD();
        EnterUIMode();
    }
}
void ALionPlayerController::EnterUIMode()
{
    bShowMouseCursor = true;
    FInputModeGameAndUI Mode; Mode.SetHideCursorDuringCapture(false);
    SetInputMode(Mode);
}
void ALionPlayerController::EnterGameMode()
{
    bShowMouseCursor = false;
    SetInputMode(FInputModeGameOnly());
}
void ALionPlayerController::RequestStartGame()
{
    if (auto* GM = Cast<ALionGameMode>(UGameplayStatics::GetGameMode(this))) GM->StartGameFlow();
}
void ALionPlayerController::RequestReplay()
{
    if (auto* GI = GetGameInstance<ULionGameInstance>()) GI->PlayLastReplay();
}
void ALionPlayerController::RequestReturnToMenu()
{
    if (auto* GI = GetGameInstance<ULionGameInstance>()) GI->ReturnToMenuLevel();
}

