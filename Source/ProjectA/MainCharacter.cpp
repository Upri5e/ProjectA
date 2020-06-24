// Fill out your copyright notice in the Description page of Project Settings.
#include "MainCharacter.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/TimelineComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Math/Vector.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WallRunningCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("WallRunningCapsule"));
	WallRunningCapsule->InitCapsuleSize(55.f, 96.0f);
	WallRunningCapsule->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	WallRunningCapsule->SetupAttachment(RootComponent);

	//Add our on timeline finished function
	MyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MyTimeline"));

	MyTimeline->SetLooping(true);
	MyTimeline->SetIgnoreTimeDilation(true);
}
// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->SetPlaneConstraintEnabled(true);
	if (fCurve)
	{
		//Declare our delegate function to be binded with TimelineFloatReturn
		FOnTimelineFloat InterpFunction{};
		//Declare our delegate function to be binded with OnTimelineFinished
		FOnTimelineEventStatic TimelineFinished{};

		InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
		TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));

		MyTimeline->AddInterpFloat(fCurve, InterpFunction, FName("Alpha"));
		MyTimeline->SetTimelineFinishedFunc(TimelineFinished);
	}
	WallRunningCapsule->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::OnWallBeginOverlap);
	WallRunningCapsule->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::OnWallEndOverlap);
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
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &AMainCharacter::Interact);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMainCharacter::DoubleJump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &AMainCharacter::EndingJump);
}

void AMainCharacter::Landed(const FHitResult &Hit)
{
	JumpCounter = 0;
}

void AMainCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue);
}

void AMainCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector(), AxisValue);
}

void AMainCharacter::Interact()
{

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* OwnerController = OwnerPawn->GetController();
	if (OwnerController == nullptr) return;

	//Get player view point
	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	DrawDebugCamera(GetWorld(), Location, Rotation, 90, 2, FColor::Red, true);


void AMainCharacter::EndingJump()
{
	ACharacter::StopJumping();
	GetCharacterMovement()->GravityScale = 1;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 0));
	OnWall = false;

}

void AMainCharacter::DoubleJump()
{
	if (JumpCounter <= 1)
	{
		JumpCounter++;
		ACharacter::LaunchCharacter(FVector(0, 0, JumpHeight), false, true);
	}
}

void AMainCharacter::TimelineFloatReturn(float value)
{
	float KeyPressed = GetWorld()->GetFirstPlayerController()->GetInputKeyTimeDown(TEXT("Spacebar"));
	if (KeyPressed > 0 && OnWall)
	{
		CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		FVector PlayerDirection = CameraManager->GetActorForwardVector();
		GetCharacterMovement()->GravityScale = 0;
		GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 1));
		GetCharacterMovement()->AddForce(PlayerDirection + DirectionForce);
		// JumpCounter = 1;
		// if (GetCharacterMovement()->Velocity.GetSafeNormal().Size() < GetCharacterMovement()->MaxAcceleration)
		// {
		// }
	}
	else
	{
		GetCharacterMovement()->GravityScale = 1;
		GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 0));
		OnWall = false;
	}
}

void AMainCharacter::OnTimelineFinished()
{
}

void AMainCharacter::OnWallBeginOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	//JumpCounter = 0;
	if (OtherActor->ActorHasTag(TEXT("Runable")) && GetCharacterMovement()->IsFalling())
	{
		OnWall = true;
		MyTimeline->Play();
	}
}

void AMainCharacter::OnWallEndOverlap(class UPrimitiveComponent *OverlappedComp, class AActor *OtherActor, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	MyTimeline->Stop();
	if (OtherActor->ActorHasTag(TEXT("Runable")))
	{
		GetCharacterMovement()->GravityScale = 1;
		GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 0));
		OnWall = false;
	}
}
