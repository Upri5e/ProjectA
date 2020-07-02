// Cor3 Studios 2020


#include "ItemPickUp.h"

// Sets default values
AItemPickUp::AItemPickUp()
{
	//BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	//BoxCollider->bGenerateOverlapEvents = true;
	//BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AItemPickUp::TriggerEnter);
	//BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AItemPickUp::TriggerExit);

	//RootComponent = BoxCollider;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AItemPickUp::BeginPlay()
{
	Super::BeginPlay();
	
	//if (PlayerController != nullptr) 
	//{
	//	if (PlayerController->bIsPickingUp && bItemIsWithinRange)
	//	{
	//		PickUp();
	//	}
	//}
}

void AItemPickUp::PickUp()
{
	/*PlayerController->Inventory.AddItem(Item);*/
	Destroy();
}

void AItemPickUp::GetPlayer(AActor* Player)
{
	PlayerController = Cast<AMainCharacter>(Player);
}

void AItemPickUp::TriggerEnter(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bItemIsWithinRange = true;
}

void AItemPickUp::TriggerExit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bItemIsWithinRange = false;
}

