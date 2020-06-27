// Cor3 Studios 2020


#include "PickUp.h"

// Sets default values
APickUp::APickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider->CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxCollider->bGenerateOverlapEvents = true;
	BoxCollider->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &APickUp::OnOverlapBegin);
	BoxCollider->SetupAttachment(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);


	BaseMesh->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BoxCollider->SetupAttachment(BoxCollider);
}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUp::PickUp()
{
	//Check if inventory is full
	if (Inventory == MaxSpace)
	{
		return;
	}
	else // Add to inventory
	{
		Inventory++;
		SetActorHiddenInGame(true);
		return;
	}
}

void APickUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Check if the OtherActor is not me or NULL
	if (OtherActor != nullptr) && (OtherActor!= this) && (OtherComp !=nullptr)
	{
	}
}

