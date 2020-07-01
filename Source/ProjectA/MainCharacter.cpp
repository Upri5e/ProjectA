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
#include "IventorySystem/Items/Item.h"
#include "IventorySystem/Items/InventoryComponent.h"
#include "MeleeWeapon.h"
#include "Camera/PlayerCameraManager.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Add our on timeline finished function
	MyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MyTimeline"));

	MyTimeline->SetLooping(true);
	MyTimeline->SetIgnoreTimeDilation(true);

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->Capacity = 20;

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
	AttachWeapons();
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

	PlayerInputComponent->BindAction(TEXT("Basic Attack"), EInputEvent::IE_Pressed, this, &AMainCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction(TEXT("Alternative Attack"), EInputEvent::IE_Pressed, this, &AMainCharacter::SecondaryAttack);
	PlayerInputComponent->BindAction(TEXT("Dash"), EInputEvent::IE_Pressed, this, &AMainCharacter::Dash);

	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AMainCharacter::Sprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AMainCharacter::StopSprinting);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMainCharacter::DoubleJump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &AMainCharacter::EndingJump);
}

void AMainCharacter::Landed(const FHitResult &Hit)
{
	JumpCounter = 0;
	DashCount = 0;
}

void AMainCharacter::MoveForward(float AxisValue)
{
	ForwardAxis = AxisValue;

	AddMovementInput(GetActorForwardVector(), ForwardAxis);
}

void AMainCharacter::MoveRight(float AxisValue)
{
	RightAxis = AxisValue;

	AddMovementInput(GetActorRightVector(), AxisValue);
}

void AMainCharacter::EndingJump()
{

	ACharacter::StopJumping();
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
			JumpCounter++;
		}
	}
}

void AMainCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed *= SpeedMultiplier;

}

void AMainCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed /= SpeedMultiplier;
}

void AMainCharacter::Dash()
{
	if (CanDash && !OnWall && (DashCount < MaxDashes))
	{
		DashCount++;
		FVector ForwardVec = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetActorForwardVector();
		GetCharacterMovement()->BrakingFrictionFactor = 0;
		LaunchCharacter(FVector(ForwardVec.X, ForwardVec.Y, 0).GetSafeNormal() * DashDistance, true, true);
		CanDash = false;
		GetWorldTimerManager().SetTimer(DashHandle, this, &AMainCharacter::StopDashing, DashStop, false);
	}
}

void AMainCharacter::StopDashing()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetWorldTimerManager().SetTimer(DashHandle, this, &AMainCharacter::ResetDash, DashCooldown, false);
	GetCharacterMovement()->BrakingFrictionFactor = 2;
}

void AMainCharacter::ResetDash()
{
	CanDash = true;
}

void AMainCharacter::SecondaryAttack()
{
	SwordR->AttackSecondary();
}

void AMainCharacter::PrimaryAttack()
{
	SwordL->AttackPrimary();
}

void AMainCharacter::BeginWallRun()
{
	GetCharacterMovement()->AirControl = 1.f;
	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 1));
	OnWall = true;
	JumpCounter = 0;
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

void AMainCharacter::AttachWeapons()
{
	GetMesh()->HideBoneByName(TEXT("pivotA_l"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("pivotB_l"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("pivotA_r"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("pivotB_r"), EPhysBodyOp::PBO_None);

	SwordL = GetWorld()->SpawnActor<AMeleeWeapon>(Sword);
	SwordR = GetWorld()->SpawnActor<AMeleeWeapon>(Sword);

	SwordL->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SwordSocketL"));
	SwordR->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("SwordSocketR"));

	SwordL->SetOwner(this);
	SwordR->SetOwner(this);
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

void AMainCharacter::UseItem(class UItem* Item)
{
	if (Item)
	{
		Item->Use(this);
		Item->OnUse(this); //bp event
	}
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
		FVector CrossPr;
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
		LaunchDirection = CrossPr.CrossProduct(WallRunDirection, WallRunSideVector);
	}
	else
	{
		if (GetCharacterMovement()->IsFalling())
		{
			FVector ToTheRight = GetActorRightVector() * RightAxis;
			FVector ForwardVector = GetActorForwardVector() * ForwardAxis;
			LaunchDirection = ForwardVector + ToTheRight;
			UE_LOG(LogTemp, Warning, TEXT("rightaxis %f, Right actor vector: %s, final: %s"), RightAxis, *GetActorRightVector().ToString(), *ToTheRight.ToString());
			UE_LOG(LogTemp, Warning, TEXT("forward axis: %f, Forward actor vector: %s, final: %s"), ForwardAxis, *GetActorForwardVector().ToString(), *ForwardVector.ToString());
		}
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

void AMainCharacter::TimelineFloatReturn(float value)
{
	UpdateWallRun();
}

void AMainCharacter::OnTimelineFinished()
{
}