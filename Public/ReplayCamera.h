// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "ReplayCamera.generated.h"

class UUserWidget;

UCLASS()
class LIONDANCE_API AReplayCamera : public ACameraActor
{
    GENERATED_BODY()
public:
    AReplayCamera();
    virtual void BeginPlay() override;
    virtual void Tick(float Dt) override;

    // 相机相对角色的偏移(后上方)与跟随平滑速度
    UPROPERTY(EditAnywhere, Category="Replay") FVector OffsetFromLion = FVector(-500, 200, 250);
    UPROPERTY(EditAnywhere, Category="Replay") float   FollowInterpSpeed = 4.f;

    // ★回放 HUD 蓝图类:在关卡里的 ReplayCamera 实例(或 BP_ReplayCamera)Details 里设为 WBP_ReplayHUD
    UPROPERTY(EditAnywhere, Category="Replay") TSubclassOf<UUserWidget> ReplayHUDClass;

private:
    TWeakObjectPtr<class APawn> Lion;
    UPROPERTY() UUserWidget* ReplayHUDInstance = nullptr;

    void ActivateReplayView(); // 回放世界:切视角 + 建HUD + 露鼠标(延迟+自重试)
};

/*
#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "ReplayCamera.generated.h"

UCLASS()
class LIONDANCE_API AReplayCamera : public ACameraActor
{
    GENERATED_BODY()
public:
    AReplayCamera();
    virtual void BeginPlay() override;
    virtual void Tick(float Dt) override;

    UPROPERTY(EditAnywhere) FVector OffsetFromLion = FVector(-500, 200, 250); // 后上方
    UPROPERTY(EditAnywhere) float FollowInterpSpeed = 4.f;

private:
    //TWeakObjectPtr<class APawn> Lion;
    void ActivateReplayView(); // 延迟切视角,避开 BeginPlay 时序问题
};
*/