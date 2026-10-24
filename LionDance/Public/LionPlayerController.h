// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LionPlayerController.generated.h"
//void TryShowReplayHUD();

UCLASS()
class LIONDANCE_API ALionPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
	void TryShowReplayHUD();   // 新增

    // ===== UI 钩子(在 BP_LionPlayerController 里用 UMG 实现) =====
    UFUNCTION(BlueprintImplementableEvent) void ShowMainMenu();
    UFUNCTION(BlueprintImplementableEvent) void HideMainMenu();
    UFUNCTION(BlueprintImplementableEvent) void ShowChargeBar(bool bShow);
    UFUNCTION(BlueprintImplementableEvent) void UpdateChargeUI(float Alpha);
    UFUNCTION(BlueprintImplementableEvent) void ShowFailScreen();
    UFUNCTION(BlueprintImplementableEvent) void ShowSuccessScreen();
    UFUNCTION(BlueprintImplementableEvent) void HideResultScreen();
    UFUNCTION(BlueprintImplementableEvent) void SetHeadOverlayVisible(bool bVisible); // 戴/摘狮头遮罩(无动画)
    UFUNCTION(BlueprintImplementableEvent) void ShowReplayHUD();                      // 回放世界:显示"退出回放"

    // ===== 输入模式切换 =====
    UFUNCTION(BlueprintCallable) void EnterUIMode();    // 显鼠标
    UFUNCTION(BlueprintCallable) void EnterGameMode();  // 隐鼠标锁视口

    // ===== 按钮调用 =====
    UFUNCTION(BlueprintCallable) void RequestStartGame();
    UFUNCTION(BlueprintCallable) void RequestReplay();
    UFUNCTION(BlueprintCallable) void RequestReturnToMenu();
};
