// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplayCamera.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "LionCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AReplayCamera::AReplayCamera()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AReplayCamera::BeginPlay()
{
    Super::BeginPlay();

    // 只有回放世界才切视角/建HUD;实时游戏世界 IsPlayingReplay()==false,不执行
    if (GetWorld() && GetWorld()->IsPlayingReplay())
    {
        FTimerHandle H;
        GetWorldTimerManager().SetTimer(H, this, &AReplayCamera::ActivateReplayView, 0.3f, false);
    }
}

void AReplayCamera::ActivateReplayView()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
        // PlayerController 尚未就绪 → 0.2s 后自重试(避免时序竞态)
        FTimerHandle H;
        GetWorldTimerManager().SetTimer(H, this, &AReplayCamera::ActivateReplayView, 0.2f, false);
        return;
    }

    // 1) 把回放视角切到本相机
    PC->SetViewTargetWithBlend(this, 0.5f);

    // 2) 显示"回放中/退出回放"HUD(不依赖 PC 是不是自定义类)
    if (ReplayHUDClass && !ReplayHUDInstance)
    {
        ReplayHUDInstance = CreateWidget<UUserWidget>(PC, ReplayHUDClass);
        if (ReplayHUDInstance) { ReplayHUDInstance->AddToViewport(); }
    }

    // 3) 露出鼠标,便于点击"退出回放"按钮
    PC->bShowMouseCursor = true;
    FInputModeGameAndUI Mode;
    PC->SetInputMode(Mode);
}

void AReplayCamera::Tick(float Dt)
{
    Super::Tick(Dt);

    if (!Lion.IsValid())
    {
        // ★回放世界里 GetPlayerPawn 常返回 null → 改为直接查找 ALionCharacter
        // 角色 bReplicates=true,会在回放世界被重建,GetAllActorsOfClass 能找到
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(this, ALionCharacter::StaticClass(), Found);
        if (Found.Num() > 0) { Lion = Cast<APawn>(Found[0]); }
        if (!Lion.IsValid()) return; // 角色还没生成,下一帧再试
    }

    const FVector Target = Lion->GetActorLocation() + OffsetFromLion;
    const FVector NewLoc = FMath::VInterpTo(GetActorLocation(), Target, Dt, FollowInterpSpeed);
    SetActorLocation(NewLoc);
    SetActorRotation(UKismetMathLibrary::FindLookAtRotation(NewLoc, Lion->GetActorLocation()));
}



/*
void AReplayCamera::BeginPlay()
{
    Super::BeginPlay();
    if (GetWorld() && GetWorld()->IsPlayingReplay())
    {
        FTimerHandle H;
        GetWorldTimerManager().SetTimer(H, this, &AReplayCamera::ActivateReplayView, 0.3f, false);
    }
}

void AReplayCamera::ActivateReplayView()
{
    if (auto* PC = UGameplayStatics::GetPlayerController(this, 0))
        PC->SetViewTargetWithBlend(this, 0.5f);
}

void AReplayCamera::Tick(float Dt)
{
    Super::Tick(Dt);
    
    APawn* TargetPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!TargetPawn)
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
            TargetPawn = PC->GetPawn();
    
    if (!TargetPawn) return;
    
    const FVector TargetLoc = TargetPawn->GetActorLocation() + OffsetFromLion;
    const FVector NewLoc = FMath::VInterpTo(GetActorLocation(), TargetLoc, Dt, FollowInterpSpeed);
    SetActorLocation(NewLoc);
    SetActorRotation(UKismetMathLibrary::FindLookAtRotation(NewLoc, TargetPawn->GetActorLocation()));
}
*/


/*
AReplayCamera::AReplayCamera() { PrimaryActorTick.bCanEverTick = true; }
//test
void AReplayCamera::BeginPlay()
{
    Super::BeginPlay();
    
    // 回放世界：延迟 0.3s 再切视角
    if (GetWorld() && GetWorld()->IsPlayingReplay())
    {
        FTimerHandle H;
        GetWorldTimerManager().SetTimer(H, this, &AReplayCamera::ActivateReplayView, 0.3f, false);
    }
}
//org
void AReplayCamera::BeginPlay()
{
    Super::BeginPlay();
    Lion = UGameplayStatics::GetPlayerPawn(this, 0);

    // 回放世界:延迟0.3s再切视角(此时PlayerController/回放角色已就绪)
    if (GetWorld() && GetWorld()->IsPlayingReplay())
    {
        FTimerHandle H;
        GetWorldTimerManager().SetTimer(H, this, &AReplayCamera::ActivateReplayView, 0.3f, false);
    }
}


void AReplayCamera::ActivateReplayView()
{
    if (auto* PC = UGameplayStatics::GetPlayerController(this, 0))
        PC->SetViewTargetWithBlend(this, 0.5f);
}

//test
void AReplayCamera::Tick(float Dt)
{
    Super::Tick(Dt);
    
    // 每帧重新获取 Player Pawn（回放世界里缓存的 Lion 会失效）
    APawn* TargetPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    
    // 兜底：如果 GetPlayerPawn 失败，通过 Controller 获取
    if (!TargetPawn)
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
            TargetPawn = PC->GetPawn();
    
    if (!TargetPawn) return;
    
    const FVector TargetLoc = TargetPawn->GetActorLocation() + OffsetFromLion;
    const FVector NewLoc = FMath::VInterpTo(GetActorLocation(), TargetLoc, Dt, FollowInterpSpeed);
    SetActorLocation(NewLoc);
    SetActorRotation(UKismetMathLibrary::FindLookAtRotation(NewLoc, TargetPawn->GetActorLocation()));
}
//org
void AReplayCamera::Tick(float Dt)
{
    Super::Tick(Dt);
    if (!Lion.IsValid()) { Lion = UGameplayStatics::GetPlayerPawn(this,0); return; }

    const FVector Target = Lion->GetActorLocation() + OffsetFromLion;
    const FVector NewLoc = FMath::VInterpTo(GetActorLocation(), Target, Dt, FollowInterpSpeed);
    SetActorLocation(NewLoc);
    SetActorRotation(UKismetMathLibrary::FindLookAtRotation(NewLoc, Lion->GetActorLocation()));
}
*/
