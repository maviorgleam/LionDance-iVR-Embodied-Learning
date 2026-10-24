// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VRJumpInputComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIONDANCE_API UVRJumpInputComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UVRJumpInputComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    // 把当前站立高度设为基准(校准)。开局自动调一次;也可绑手柄按钮手动重校
    UFUNCTION(BlueprintCallable, Category="VR") void CalibrateStandingHeight();

    // —— 手感参数(需戴真机调,以下为起始值) ——
    UPROPERTY(EditAnywhere, Category="VR") float MaxCrouchDepth      = 40.f; // 蹲多深算满蓄(cm)
    UPROPERTY(EditAnywhere, Category="VR") float CrouchEnterThreshold = 8.f; // 下蹲超过这么多才开始蓄力(cm)
    UPROPERTY(EditAnywhere, Category="VR") float JumpExitThreshold    = 5.f; // 回升到离基准这么近视为起跳(cm)
    UPROPERTY(EditAnywhere, Category="VR") float JumpUpVelocity      = 120.f;// 上升速度超过此值视为蹬地起跳(cm/s)
    UPROPERTY(EditAnywhere, Category="VR") float PostJumpLock         = 0.5f;// 起跳后锁定,防抖动重复触发(秒)

protected:
    UPROPERTY() class ACharacter*        OwnerChar = nullptr;
    UPROPERTY() class UCameraComponent*  Cam       = nullptr;

    bool  bCalibrated       = false;
    float StandingHeight    = 0.f;
    float LastHeight        = 0.f;
    bool  bCharging         = false;
    float MaxAlphaThisCharge = 0.f;
    float LockTimer         = 0.f;

    float GetHMDHeight() const;  // 当前HMD离地高度(cm)
    void  UpdateLookYaw();       // 把HMD世界Yaw写入ControlRotation
};
