// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "JumpInput.h"
#include "LionCharacter.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class LIONDANCE_API ALionCharacter : public ACharacter, public IJumpInput
{
    GENERATED_BODY()
public:
    ALionCharacter();
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void Landed(const FHitResult& Hit) override;

    // ===== IJumpInput 实现(VR/PC 共用) =====
    virtual void BeginCharge_Implementation() override;
    virtual void UpdateChargeAlpha_Implementation(float Alpha) override;
    virtual void CommitJump_Implementation(float Alpha) override;

    UFUNCTION(BlueprintCallable, Category="Lion") void SetJumpEnabled(bool bEnabled) { bJumpEnabled = bEnabled; }
    UFUNCTION(BlueprintCallable, Category="Lion") void RespawnToSafe();


	UFUNCTION(BlueprintCallable, Category="Lion") bool IsJumpEnabled() const { return bJumpEnabled; }

protected:
    virtual void BeginPlay() override;


    UPROPERTY(EditAnywhere, Category="Input") UInputMappingContext* MappingContext;
    UPROPERTY(EditAnywhere, Category="Input") UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, Category="Input") UInputAction* LookAction;
    UPROPERTY(EditAnywhere, Category="Input") UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category="Jump") float MaxChargeTime = 1.0f;
    UPROPERTY(EditAnywhere, Category="Jump") float MinLaunchSpeed = 450.f;
    UPROPERTY(EditAnywhere, Category="Jump") float MaxLaunchSpeed = 750.f;
    UPROPERTY(EditAnywhere, Category="Jump") float LaunchAngleDeg = 50.f;
    UPROPERTY(EditAnywhere, Category="Jump") float KillZ = 120.f;

	// ★VR开关：true=用VR动作驱动蓄力(关掉PC按时长蓄力)
    UPROPERTY(EditAnywhere, Category="Input") bool bUseVRInput = false;

    bool  bIsCharging = false;
    bool  bJumpEnabled = false;
    float ChargeTime = 0.f;
    float ChargeAlpha = 0.f;
    FVector LastSafeLocation;

    FVector ComputeLaunchVelocity(float Alpha) const;

    void OnJumpStarted(const FInputActionValue& V);
    void OnJumpReleased(const FInputActionValue& V);
    void OnLook(const FInputActionValue& V);
    void OnMove(const FInputActionValue& V);
    

    UFUNCTION(BlueprintCallable)
    void JumpStart();
    
    UFUNCTION(BlueprintCallable)
    void JumpRelease();
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FRotator CameraRotation;
};
