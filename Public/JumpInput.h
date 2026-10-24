#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "JumpInput.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UJumpInput : public UInterface { GENERATED_BODY() };

class LIONDANCE_API IJumpInput
{
    GENERATED_BODY()
public:
    // 开始蓄力:PC=按下空格; VR=玩家开始下蹲
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Lion")
    void BeginCharge();

    // 更新蓄力进度 0~1:PC=按住时长/最大时长; VR=下蹲深度比例
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Lion")
    void UpdateChargeAlpha(float Alpha);

    // 起跳:PC=松开空格(传最终Alpha); VR=蹬地起跳瞬间
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Lion")
    void CommitJump(float Alpha);
};