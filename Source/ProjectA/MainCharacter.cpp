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
#include "Camera/PlayerCameraManager.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
// Sets default values
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// WallRunningCapsule = GetCapsuleComponent()->
	// WallRunningCapsule->InitCapsuleSize(55.f, 96.0f);
	// WallRunningCapsule->SetCollisionProfileName(TEXT("Pawn"));
	// WallRunningCapsule->SetupAttachment(RootComponent);

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
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMainCharacter::OnHit);

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
		MyTimeline->SetLooping(true);
	}
	// WallRunningCapsule->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::OnWallBeginOverlap);
	// WallRunningCapsule->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::OnWallEndOverlap);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ClampHorizontalVelocity();
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
	ForwardAxis = AxisValue;
	AddMovementInput(GetActorForwardVector(), ForwardAxis);
}

void AMainCharacter::MoveRight(float AxisValue)
{
	RightAxis = AxisValue;
	AddMovementInput(GetActorRightVector(), RightAxis);
}

void AMainCharacter::EndingJump()
{

	ACharacter::StopJumping();
	WallRunExpire();
}

void AMainCharacter::DoubleJump()
{
	if (JumpCounter <= 1)
	{
		ACharacter::LaunchCharacter(FindLaunchVelocity(), false, true);
		JumpCounter++;

		if (OnWall)
		{
			EndWallRun(EWallRunEndReason::JumpedOfWall);
		}
	}
}

void AMainCharacter::BeginWallRun()
{
	GetCharacterMovement()->AirControl = 1.f;
	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 1));
	OnWall = true;

	MyTimeline->Play();
}

void AMainCharacter::EndWallRun(EWallRunEndReason Reason)
{
	switch (Reason)
	{
	case FellOffWall:
		JumpCounter = 1;
		break;
	case JumpedOfWall:
		JumpCounter = 0;
		break;
	}
	GetCharacterMovement()->AirControl = 0.05;
	GetCharacterMovement()->GravityScale = 1;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 0));
	OnWall = false;
	MyTimeline->Stop();
}

void AMainCharacter::FindDirectionAndSide(FVector WallNormal, EWallRunSide &Side, FVector &Direction)
{
	FVector2D SlopeOfVector;
	EWallRunSide SideLocal;
	FVector SideVector;
	FVector SideCrossProduct;
	FVector2D ActorRightVector = FVector2D(GetActorRightVector().X, GetActorRightVector().Y);
	FVector2D TDWallNormal = FVector2D(WallNormal.X, WallNormal.Y);
	if (SlopeOfVector.DotProduct(ActorRightVector, TDWallNormal) > 0)
	{
		SideLocal = EWallRunSide::Right;
	}
	else
	{
		SideLocal = EWallRunSide::Left;
	}
	switch (SideLocal)
	{
	case EWallRunSide::Right:
		SideVector = FVector(0, 0, 1);
		break;
	case EWallRunSide::Left:
		SideVector = FVector(0, 0, -1);
		break;
	}
	Direction = SideCrossProduct.CrossProduct(WallNormal, SideVector);
	Side = SideLocal;
}

bool AMainCharacter::CanSurfaceBeWallRan(FVector SurfaceNormal) const
{
	FVector SurfaceDotProduct;
	if (SurfaceNormal.Z < -0.05)
	{
		return false;
	}
	else
	{
		FVector SurfaceNormalized = FVector(SurfaceNormal.X, SurfaceNormal.Y, 0).GetSafeNormal();
		if (SurfaceDotProduct.DotProduct(SurfaceNormalized, SurfaceNormal) < GetCharacterMovement()->GetWalkableFloorAngle())
		{
			return true;
		}
		return false;
	}
}

FVector AMainCharacter::FindLaunchVelocity() const
{
	FVector LaunchDirection;
	if (OnWall)
	{
		FVector WallRunSideVector;
		switch (WallRunSide)
		{
		case EWallRunSide::Right:
			WallRunSideVector = FVector(0, 0, -1);
			break;

		case EWallRunSide::Left:
			WallRunSideVector = FVector(0, 0, 1);
			break;
		}
		LaunchDirection.CrossProduct(WallRunDirection, WallRunSideVector);
	}
	else
	{
		// if (GetCharacterMovement()->IsFalling())
		// {
		// 	FVector ToTheRight = GetActorRightVector() * RightAxis;
		// 	FVector ForwardVector = GetActorForwardVector() * ForwardAxis;
		// 	LaunchDirection = ForwardVector + ToTheRight;
		// }
	}
	return (LaunchDirection + FVector(0, 0, 1)) * GetCharacterMovement()->JumpZVelocity;
}

bool AMainCharacter::AreRequiredKeysDown() const
{
	if (ForwardAxis > 0)
	{
		switch (WallRunSide)
		{
		case EWallRunSide::Right:
			if (RightAxis < 0)
			{
				return true;
			}
			break;
		case EWallRunSide::Left:
			if (RightAxis > 0)
			{
				return true;
			}
			break;
		}
	}
	return false;
}

FVector2D AMainCharacter::GetHorizontalVelocity() const
{
	return FVector2D(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y);
}

void AMainCharacter::SetHorizontalVelocity(FVector2D HorizontalVelocity)
{
	GetCharacterMovement()->Velocity.X = HorizontalVelocity.X;
	GetCharacterMovement()->Velocity.Y = HorizontalVelocity.Y;
}

void AMainCharacter::OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	if (!OnWall)
	{
		if (CanSurfaceBeWallRan(Hit.ImpactNormal) && GetCharacterMovement()->IsFalling())
		{
			EWallRunSide Side;
			FVector Direction;
			FindDirectionAndSide(Hit.ImpactNormal, Side, Direction);
			WallRunSide = Side;
			if (AreRequiredKeysDown())
			{
				BeginWallRun();
			}
		}
	}
}

void AMainCharacter::UpdateWallRun()
{
	FVector EndLocation;
	FHitResult OutHit;
	FVector CrossPro;
	FVector Product;
	if (AreRequiredKeysDown())
	{
		switch (WallRunSide)
		{
		case EWallRunSide::Left:

			CrossPro = FVector(0, 0, -1);
			break;

		case EWallRunSide::Right:
			CrossPro = FVector(0, 0, 1);
			break;
		}
		EndLocation = Product.CrossProduct(WallRunDirection, CrossPro) + GetActorLocation() * 200;
		if (GetWorld()->LineTraceSingleByChannel(OutHit, GetActorLocation(), EndLocation, ECollisionChannel::ECC_GameTraceChannel1))
		{
			EWallRunSide Side;
			FVector Direction;
			FVector WallRunUpdateDirection;
			FindDirectionAndSide(OutHit.ImpactNormal, Side, Direction);
			if (Side != WallRunSide)
			{
				EndWallRun(EWallRunEndReason::FellOffWall);
			}
			else
			{
				WallRunDirection = Direction;
				WallRunUpdateDirection = WallRunDirection * GetCharacterMovement()->MaxWalkSpeed;
				GetCharacterMovement()->Velocity = FVector(WallRunUpdateDirection.X, WallRunUpdateDirection.Y, 0);
			}
		}
		else
		{
			EndWallRun(EWallRunEndReason::FellOffWall);
		}
	}
	else
	{
		EndWallRun(EWallRunEndReason::FellOffWall);
	}
}

void AMainCharacter::ClampHorizontalVelocity()
{
	if (GetCharacterMovement()->IsFalling())
	{
		float OurSpeed = GetHorizontalVelocity().Size() / GetCharacterMovement()->MaxWalkSpeed;
		if (OurSpeed > 1)
		{
			FVector2D FinalVelocity = GetHorizontalVelocity() / OurSpeed;
			SetHorizontalVelocity(FinalVelocity);
		}
	}
}
void AMainCharacter::WallRunExpire()
{
	// GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 0));
	// MyTimeline->Stop();
	// GetCharacterMovement()->GravityScale = FMath::Lerp(GetCharacterMovement()->GravityScale, 1.f, FallOffWallSpeed);
	// OnWall = false;
}
void AMainCharacter::TimelineFloatReturn(float value)
{
	// 	float KeyPressed = GetWorld()->GetFirstPlayerController()->GetInputKeyTimeDown(TEXT("Spacebar"));
	// 	if (KeyPressed > 0 && OnWall)
	// 	{

	// 		FVector PlayerDirection = GetOwner()->GetActorForwardVector();
	// 		GetCharacterMovement()->GravityScale = 0;
	// 		GetCharacterMovement()->SetPlaneConstraintNormal(FVector(1, 0, 1));
	// 		GetCharacterMovement()->AddForce(PlayerDirection + DirectionForce);
	// 	}
	// 	else
	// 	{
	// 		WallRunExpire();
	// 	}
	UpdateWallRun();
}

void AMainCharacter::OnTimelineFinished()
{
}

// void AMainCharacter::OnWallBeginOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
// {
// 	JumpCounter = 0;
// 	if (OtherActor->ActorHasTag(TEXT("Runable")) && GetCharacterMovement()->IsFalling())
// 	{
// 		OnWall = true;
// 		MyTimeline->Play();

// 		GetWorld()->GetTimerManager().SetTimer(UnusedHandler, this, &AMainCharacter::WallRunExpire, FallOffWallTime);
// 	}
// }

// void AMainCharacter::OnWallEndOverlap(class UPrimitiveComponent *OverlappedComp, class AActor *OtherActor, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
// {
// 	if (OtherActor->ActorHasTag(TEXT("Runable")))
// 	{
// 		WallRunExpire();
// 	}
// }
