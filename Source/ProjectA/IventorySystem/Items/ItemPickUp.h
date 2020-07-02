// Cor3 Studios 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "MainCharacter.h"
#include "Item.h"
#include "ItemPickUp.generated.h"

class UPrimitiveComponent;

UCLASS()
class PROJECTA_API AItemPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	



	void PickUp();
	
	void GetPlayer(AActor* Player);

	bool bItemIsWithinRange = false;

	UFUNCTION()
	void TriggerEnter(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void TriggerExit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	UPROPERTY()
		UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* BaseMesh;

	UPROPERTY()
		AMainCharacter* PlayerController;

	UPROPERTY()
		UItem* Item;

};
