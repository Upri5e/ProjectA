// Fill out your copyright notice in the Description page of Project Settings.
#include "MainCharacter.h"

#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/TimelineComponent.h"

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
		FVector PlayerDirection = GetActorForwardVector(); //MIGHT CHANGE IMPORTANT
		GetCharacterMovement()->GravityScale = 0;
		GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 1));
		GetCharacterMovement()->AddForce(PlayerDirection + 200000);
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
	JumpCounter = 0;
	if (OtherActor->ActorHasTag(TEXT("Runable")) && GetCharacterMovement()->IsFalling())
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlapping"));
		OnWall = true;
		MyTimeline->Play();
	}
}

void AMainCharacter::OnWallEndOverlap(class UPrimitiveComponent *OverlappedComp, class AActor *OtherActor, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	MyTimeline->Stop();
	if (OtherActor->ActorHasTag(TEXT("Runable")))
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT Overlapping"));
		GetCharacterMovement()->GravityScale = 1;
		GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 0));
		OnWall = false;
	}
}
