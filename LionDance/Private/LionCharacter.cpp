// Fill out your copyright notice in the Description page of Project Settings.


#include "LionCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "PlumStake.h"
#include "LionPlayerController.h"
#include "LionGameMode.h"
#include "Kismet/GameplayStatics.h"

ALionCharacter::ALionCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->AirControl = 0.f;   // 空中不可改向,踩空必摔(真物理)
    GetCharacterMovement()->GravityScale = 1.f;

	GetCharacterMovement()->MaxWalkSpeed = 220.f;               // ← 新增：默认600太快
    GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;// ← 新增：落桩快速停住

    bReplicates = true;            // 回放系统需要
    SetReplicateMovement(true);
}

void ALionCharacter::BeginPlay()
{
    Super::BeginPlay();
    LastSafeLocation = GetActorLocation();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
        if (auto* Subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
            if (MappingContext) Subsys->AddMappingContext(MappingContext, 0);
}

void ALionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(JumpAction, ETriggerEvent::Started,   this, &ALionCharacter::OnJumpStarted);
        EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ALionCharacter::OnJumpReleased);
        EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALionCharacter::OnLook);
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALionCharacter::OnMove);
    }
}

void ALionCharacter::OnJumpStarted(const FInputActionValue&)
{
    if (bJumpEnabled && GetCharacterMovement()->IsMovingOnGround())
        Execute_BeginCharge(this);
}
void ALionCharacter::OnJumpReleased(const FInputActionValue&)
{
    if (bIsCharging) Execute_CommitJump(this, ChargeAlpha);
}
void ALionCharacter::OnLook(const FInputActionValue& V)
{
    const FVector2D Axis = V.Get<FVector2D>();
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y);
}
void ALionCharacter::OnMove(const FInputActionValue& V)
{
    if (!Controller) return;
    const FVector2D Axis = V.Get<FVector2D>();
    const FRotator YawRot(0, GetControlRotation().Yaw, 0);
    AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::X), Axis.Y);
    AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y), Axis.X);
}

void ALionCharacter::JumpStart()
{
    if (GetCharacterMovement()->IsMovingOnGround())
        Execute_BeginCharge(this);
}

void ALionCharacter::JumpRelease()
{
    if (bIsCharging) Execute_CommitJump(this, ChargeAlpha);
}

void ALionCharacter::BeginCharge_Implementation()
{
    bIsCharging = true; ChargeTime = 0.f; ChargeAlpha = 0.f;
}
void ALionCharacter::UpdateChargeAlpha_Implementation(float Alpha)
{
    ChargeAlpha = FMath::Clamp(Alpha, 0.f, 1.f);
    if (auto* PC = Cast<ALionPlayerController>(GetController())) PC->UpdateChargeUI(ChargeAlpha);
}
void ALionCharacter::CommitJump_Implementation(float Alpha)
{
    bIsCharging = false;
    if (auto* PC = Cast<ALionPlayerController>(GetController())) PC->UpdateChargeUI(0.f);
    LaunchCharacter(ComputeLaunchVelocity(Alpha), true, true);
}

FVector ALionCharacter::ComputeLaunchVelocity(float Alpha) const
{
    const float Speed = FMath::Lerp(MinLaunchSpeed, MaxLaunchSpeed, FMath::Clamp(Alpha,0.f,1.f));
    const FRotator YawRot(0, CameraRotation.Yaw, 0);
    const FVector ForwardXY = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    const float Rad = FMath::DegreesToRadians(LaunchAngleDeg);
    FVector Dir = ForwardXY * FMath::Cos(Rad) + FVector::UpVector * FMath::Sin(Rad);
    return Dir.GetSafeNormal() * Speed;
}

void ALionCharacter::Tick(float Dt)
{
    Super::Tick(Dt);

    // PC专用:按住时长→Alpha。VR端不需要这段(由头显驱动 UpdateChargeAlpha)
    //if (bIsCharging)
    //{
    //    ChargeTime = FMath::Min(ChargeTime + Dt, MaxChargeTime);
    //    Execute_UpdateChargeAlpha(this, ChargeTime / MaxChargeTime);
    //}

	//迭代:只在非VR时用时长驱动；VR时这段不跑，改由VR组件驱动Alpha
	if (!bUseVRInput && bIsCharging)
    {
        ChargeTime = FMath::Min(ChargeTime + Dt, MaxChargeTime);
        Execute_UpdateChargeAlpha(this, ChargeTime / MaxChargeTime);
    }

    // 兜底:掉出地图无限下落(永远不会触发 Landed)时判 负
    if (GetActorLocation().Z < KillZ)
        if (auto* GM = Cast<ALionGameMode>(UGameplayStatics::GetGameMode(this)))
            GM->OnPlayerFailed();
}

void ALionCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    if (!bJumpEnabled) return; // 菜单/结算阶段的落地不判定

    // 落地瞬间:踩中的是梅花桩→安全;否则(地面/任何非桩物体)→失败
    if (Hit.GetActor() && Hit.GetActor()->IsA(APlumStake::StaticClass()))
    {
        LastSafeLocation = GetActorLocation(); // 记录安全落点
    }
    else
    {
        if (auto* GM = Cast<ALionGameMode>(UGameplayStatics::GetGameMode(this)))
            GM->OnPlayerFailed();
    }
}

void ALionCharacter::RespawnToSafe()
{
    GetCharacterMovement()->StopMovementImmediately();
    SetActorLocation(LastSafeLocation, false, nullptr, ETeleportType::TeleportPhysics);
    bIsCharging = false; ChargeTime = 0.f;
}

