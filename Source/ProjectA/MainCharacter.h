// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UENUM()
enum class EWallRunSide
{
	Right,
	Left
};
UENUM()
enum EWallRunEndReason
{
	FellOffWall,
	JumpedOfWall
};
class MeleeWeapon;
UCLASS()
class PROJECTA_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Wall Running")
	class UCapsuleComponent *WallRunningCapsule;

	class UTimelineComponent *MyTimeline;

	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	class UInventoryComponent* Inventory;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	virtual void Landed(const FHitResult &Hit) override;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void Interact();

	void EndingJump();
	void DoubleJump();
	void Sprint();
	void StopSprinting();
	void Dash();
	void StopDashing();
	void ResetDash();

	UPROPERTY(EditAnywhere, Category = "CharacterMovement:Walking")
	float SpeedMultiplier = 1.5;

	UPROPERTY(EditAnywhere, Category = Dashing)
	float DashDistance = 6000;

	UPROPERTY(EditAnywhere, Category = Dashing)
	float DashCooldown = 1;

	UPROPERTY(EditAnywhere, Category = Dashing)
	float DashStop = .1f;

	FTimerHandle DashHandle;
	bool CanDash = true;

	UPROPERTY(EditAnywhere, Category = Dashing)
	int MaxDashes = 2;

	int DashCount = 0;

	UFUNCTION(BlueprintCallable)
	void PrimaryAttack();
	
	void SecondaryAttack();

	void BeginWallRun();
	void EndWallRun(EWallRunEndReason Reason);
	FTimerHandle UnusedHandler;

	void AttachWeapons();

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;
public:
	UFUNCTION()
	void OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);

	bool CanSurfaceBeWallRan(FVector SurfaceNormal) const;

	FVector FindLaunchVelocity() const;

	bool AreRequiredKeysDown() const;

	FVector2D GetHorizontalVelocity() const;

	void SetHorizontalVelocity(FVector2D HorizontalVelocity);

	void UpdateWallRun();

	void ClampHorizontalVelocity();

	UFUNCTION()
	void TimelineFloatReturn(float value);

	UFUNCTION()
	void OnTimelineFinished();

	void FindDirectionAndSide(FVector WallNormal, EWallRunSide &Side, FVector &Direction);

	enum EWallRunSide WallRunSide;

	int JumpCounter = 0;

	bool OnWall = false;

	FVector WallRunDirection;

	float RightAxis;

	float ForwardAxis;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
	class UCurveFloat *fCurve;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
	float DirectionForce = 20000;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
	float FallOffWallTime = 3;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
	float FallOffWallSpeed = 0.01;

	UPROPERTY(EditAnywhere, Category = CharacterInfo)
	float Sensitivity = 60;

	UPROPERTY(EditAnywhere)
	float MaxInteractRange = 500;

	class UCapsuleComponent *RunCapsule;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	float JumpHeight = 420;

	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(class UItem* Item);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMeleeWeapon> Sword;

	AMeleeWeapon* SwordL;
	AMeleeWeapon* SwordR;
};
