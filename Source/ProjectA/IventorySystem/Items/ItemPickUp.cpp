// Cor3 Studios 2020


#include "ItemPickUp.h"

// Sets default values
AItemPickUp::AItemPickUp()
{
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxCollider->SetGenerateOverlapEvents(true);
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AItemPickUp::TriggerEnter);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AItemPickUp::TiggerExit);
	RootComponent = BoxCollider;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(BoxCollider);

}

// Called when the game starts or when spawned
void AItemPickUp::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemPickUp::PickUp()
{
	// check if player is picking up
	if (!bIsPickingUp)
	{
		bIsPickingUp = true;
		UE_LOG(LogTemp, Warning, TEXT("You picked up item!"));
	}
	
}

void AItemPickUp::TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bIsInRange = true;
	UE_LOG(LogTemp, Warning, TEXT("In pickup range!"));
}

void AItemPickUp::TiggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsInRange = false;
	UE_LOG(LogTemp, Warning, TEXT("Out of pick up range!"))
}

