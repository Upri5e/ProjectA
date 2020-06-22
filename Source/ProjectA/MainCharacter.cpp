// Fill out your copyright notice in the Description page of Project Settings.
#include "MainCharacter.h"

#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WallRunningCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("WallRunningCapsule"));
	WallRunningCapsule->InitCapsuleSize(55.f, 96.0f);
	WallRunningCapsule->SetCollisionProfileName(TEXT("Trigger"));
	WallRunningCapsule->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	WallRunningCapsule->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::OnOverlapBegin);
	WallRunningCapsule->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::OnOverlapEnd);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMainCharacter::WallRun);
}

void AMainCharacter::OnOverlapBegin(class UPrimitiveComponent *OverlappedComp, class AActor *OtherActor, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp && OtherActor->ActorHasTag(TEXT("Runable")))
	{
		StickToWall = true;
		UE_LOG(LogTemp, Warning, TEXT("Overlapstart %s"), *OtherActor->GetName());
	}
}

void AMainCharacter::OnOverlapEnd(class UPrimitiveComponent *OverlappedComp, class AActor *OtherActor, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp && OtherActor->ActorHasTag(TEXT("Runable")))
	{
		StickToWall = false;
		GetCharacterMovement()->GravityScale = 1;
		UE_LOG(LogTemp, Warning, TEXT("end %s"), *OtherActor->GetName());
	}
}

void AMainCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue);
}

void AMainCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector(), AxisValue);
}

void AMainCharacter::WallRun()
{
	bool JumpKeyPressed = GetWorld()->GetFirstPlayerController()->GetInputAnalogKeyState(TEXT("Jump"));
	FString jumping = "False";
	if (JumpKeyPressed)
	{
		jumping = "true";
	}
	if (StickToWall && GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->GravityScale = 0.01;
		GetCharacterMovement()->BrakingDecelerationFalling = 0.1;
		UE_LOG(LogTemp, Warning, TEXT("falling %s"), *jumping);
	}
	else
	{
		GetCharacterMovement()->GravityScale = 1;
		UE_LOG(LogTemp, Warning, TEXT("%s"), *jumping);
	}
}
