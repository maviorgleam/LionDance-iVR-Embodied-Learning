// Fill out your copyright notice in the Description page of Project Settings.

#include "VRJumpInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "JumpInput.h"
#include "LionCharacter.h"
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplayTypes.h"

UVRJumpInputComponent::UVRJumpInputComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UVRJumpInputComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerChar = Cast<ACharacter>(GetOwner());
    if (OwnerChar) Cam = OwnerChar->FindComponentByClass<UCameraComponent>();

    // Mac 版没有 EHMDTrackingOrigin::Floor，不调用 SetTrackingOrigin
    // 靠 CalibrateStandingHeight() 把当前高度设为基准，下蹲深度 = 基准 - 当前

    if (Cam && OwnerChar)
    {
        Cam->bLockToHmd = true;
        Cam->bUsePawnControlRotation = false;
        const float HalfH = OwnerChar->GetCapsuleComponent()
                            ? OwnerChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 90.f;
        Cam->SetRelativeLocation(FVector(0.f, 0.f, -HalfH));
    }

    if (GetWorld())
    {
        FTimerHandle H;
        GetWorld()->GetTimerManager().SetTimer(
            H, this, &UVRJumpInputComponent::CalibrateStandingHeight, 1.0f, false);
    }
}

float UVRJumpInputComponent::GetHMDHeight() const
{
    if (GEngine && GEngine->XRSystem.IsValid())
    {
        FQuat Orientation;
        FVector Position;
        if (GEngine->XRSystem->GetCurrentPose(IXRTrackingSystem::HMDDeviceId, Orientation, Position))
        {
            return Position.Z;
        }
    }
    return 0.f;
}

void UVRJumpInputComponent::CalibrateStandingHeight()
{
    StandingHeight = GetHMDHeight();
    LastHeight     = StandingHeight;
    bCalibrated    = (StandingHeight > 1.f);
}

void UVRJumpInputComponent::UpdateLookYaw()
{
    if (!OwnerChar || !Cam) return;
    if (AController* C = OwnerChar->GetController())
    {
        const float Yaw = Cam->GetComponentRotation().Yaw;
        C->SetControlRotation(FRotator(0.f, Yaw, 0.f));
    }
}

void UVRJumpInputComponent::TickComponent(float Dt, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(Dt, TickType, ThisTickFunction);
    if (!OwnerChar || !Cam || !bCalibrated || Dt <= 0.f) return;

    UpdateLookYaw();

    if (LockTimer > 0.f) { LockTimer -= Dt; LastHeight = GetHMDHeight(); return; }

    const float H      = GetHMDHeight();
    const float Crouch = StandingHeight - H;
    const float VelUp  = (H - LastHeight) / Dt;
    LastHeight = H;

    const float Alpha = FMath::Clamp(Crouch / MaxCrouchDepth, 0.f, 1.f);

    if (!bCharging)
    {
        const bool bReady = OwnerChar->GetCharacterMovement()->IsMovingOnGround();
        const ALionCharacter* Lion = Cast<ALionCharacter>(OwnerChar);
        const bool bEnabled = (Lion ? Lion->IsJumpEnabled() : true);
        if (Crouch > CrouchEnterThreshold && bReady && bEnabled)
        {
            bCharging = true;
            MaxAlphaThisCharge = Alpha;
            IJumpInput::Execute_BeginCharge(OwnerChar);
        }
    }
    else
    {
        MaxAlphaThisCharge = FMath::Max(MaxAlphaThisCharge, Alpha);
        IJumpInput::Execute_UpdateChargeAlpha(OwnerChar, Alpha);

        const bool bRoseBack  = (Crouch < JumpExitThreshold);
        const bool bExplosive = (VelUp  > JumpUpVelocity);
        if (bRoseBack || bExplosive)
        {
            IJumpInput::Execute_CommitJump(OwnerChar, MaxAlphaThisCharge);
            bCharging = false;
            LockTimer = PostJumpLock;
        }
    }
}

/*
#include "VRJumpInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "JumpInput.h"
#include "LionCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HeadMountedDisplayTypes.h"

UVRJumpInputComponent::UVRJumpInputComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UVRJumpInputComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerChar = Cast<ACharacter>(GetOwner());
    if (OwnerChar) Cam = OwnerChar->FindComponentByClass<UCameraComponent>();

    // 追踪原点设为「地板」:GetOrientationAndPosition 的 Z 即=头显离真实地板高度
    UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

    // VR 相机设置:让 HMD 直接驱动相机,并把VR原点降到脚底(否则视点会偏高)
    if (Cam && OwnerChar)
    {
        Cam->bLockToHmd = true;                 // HMD 驱动相机(默认即true,显式写明)
        Cam->bUsePawnControlRotation = false;   // VR不由控制器转视角,交给HMD
        const float HalfH = OwnerChar->GetCapsuleComponent()
                            ? OwnerChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 90.f;
        Cam->SetRelativeLocation(FVector(0.f, 0.f, -HalfH)); // VR原点落到胶囊底=脚底
    }

    // 延迟1秒自动校准(给玩家戴稳站直的时间)
    if (GetWorld())
    {
        FTimerHandle H;
        GetWorld()->GetTimerManager().SetTimer(
            H, this, &UVRJumpInputComponent::CalibrateStandingHeight, 1.0f, false);
    }
}

float UVRJumpInputComponent::GetHMDHeight() const
{
    FRotator R; FVector P;
    UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(R, P);
    return P.Z; // 地板追踪原点下 = 离地高度(cm)
}

void UVRJumpInputComponent::CalibrateStandingHeight()
{
    StandingHeight = GetHMDHeight();
    LastHeight     = StandingHeight;
    bCalibrated    = (StandingHeight > 1.f); // 没戴头显时返回~0,不会误校准
}

void UVRJumpInputComponent::UpdateLookYaw()
{
    if (!OwnerChar || !Cam) return;
    if (AController* C = OwnerChar->GetController())
    {
        const float Yaw = Cam->GetComponentRotation().Yaw; // 头显世界朝向
        C->SetControlRotation(FRotator(0.f, Yaw, 0.f));     // 供 ComputeLaunchVelocity 用(玩法不改)
    }
}

void UVRJumpInputComponent::TickComponent(float Dt, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(Dt, TickType, ThisTickFunction);
    if (!OwnerChar || !Cam || !bCalibrated || Dt <= 0.f) return;

    UpdateLookYaw(); // 每帧把朝向喂给控制器

    // 起跳后锁定期:不检测,避免站起过程被反复触发
    if (LockTimer > 0.f) { LockTimer -= Dt; LastHeight = GetHMDHeight(); return; }

    const float H      = GetHMDHeight();
    const float Crouch = StandingHeight - H;      // 蹲得越深越大(cm)
    const float VelUp  = (H - LastHeight) / Dt;   // 上升为正(cm/s)
    LastHeight = H;

    const float Alpha = FMath::Clamp(Crouch / MaxCrouchDepth, 0.f, 1.f);

    if (!bCharging)
    {
        // 只有在地面 + 玩法允许(Playing阶段)时才开始蓄力
        const bool bReady = OwnerChar->GetCharacterMovement()->IsMovingOnGround();
        const ALionCharacter* Lion = Cast<ALionCharacter>(OwnerChar);
        const bool bEnabled = (Lion ? Lion->IsJumpEnabled() : true);
        if (Crouch > CrouchEnterThreshold && bReady && bEnabled)
        {
            bCharging = true;
            MaxAlphaThisCharge = Alpha;
            IJumpInput::Execute_BeginCharge(OwnerChar);
        }
    }
    else
    {
        MaxAlphaThisCharge = FMath::Max(MaxAlphaThisCharge, Alpha); // 记住最深蹲=力度
        IJumpInput::Execute_UpdateChargeAlpha(OwnerChar, Alpha);    // 实时更新UI(下蹲越深条越满)

        const bool bRoseBack  = (Crouch < JumpExitThreshold); // 已站回接近基准
        const bool bExplosive = (VelUp  > JumpUpVelocity);    // 爆发上升(蹬地)
        if (bRoseBack || bExplosive)
        {
            IJumpInput::Execute_CommitJump(OwnerChar, MaxAlphaThisCharge); // 用最深蹲的力度起跳
            bCharging = false;
            LockTimer = PostJumpLock;
        }
    }
}

*/