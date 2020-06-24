// Cor3 Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallRunHit.generated.h"

UCLASS()
class PROJECTA_API AWallRunHit : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWallRunHit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent *MyComp;

	UFUNCTION()
	void OnWallHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);
};
