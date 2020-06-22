// Cor3 Studios

#include "WallRunHit.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AWallRunHit::AWallRunHit()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Use a sphere as a simple collision representation
	MyComp = CreateDefaultSubobject<CapsuleComponent>(TEXT("BoxComp"));
	// MyComp->SetSimulatePhysics(true);
	// MyComp->SetNotifyRigidBodyCollision(true);

	// MyComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");

	// Set as root component
	//RootComponent = MyComp;
}

// Called when the game starts or when spawned
void AWallRunHit::BeginPlay()
{
	Super::BeginPlay();
	MyComp->OnComponentHit.AddDynamic(this, &AWallRunHit::OnWallHit);
}

// Called every frame
void AWallRunHit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWallRunHit::OnWallHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *OtherActor->GetName()));
	}
}
