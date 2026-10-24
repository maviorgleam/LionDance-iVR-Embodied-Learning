#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlumStake.generated.h"

UCLASS()
class LIONDANCE_API APlumStake : public AActor
{
    GENERATED_BODY()
public:
    APlumStake();
protected:
    UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* Mesh;
};
