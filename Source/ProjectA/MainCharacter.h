// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

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

public:
	UFUNCTION()
	void TimelineFloatReturn(float value);

	UFUNCTION()
	void OnTimelineFinished();

	UFUNCTION()
	void OnWallBeginOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnWallEndOverlap(class UPrimitiveComponent *OverlappedComp, class AActor *OtherActor, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

	int JumpCounter = 0;

	bool OnWall = false;

	class APlayerCameraManager *CameraManager;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
	class UCurveFloat *fCurve;


	UPROPERTY(EditAnywhere, Category = "Wall Running")
	float DirectionForce = 20000;

	UPROPERTY(EditAnywhere, Category = CharacterInfo)
	float Sensitivity = 60;

	UPROPERTY(EditAnywhere)

	float MaxInteractRange = 500;

	class UCapsuleComponent *RunCapsule;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	float JumpHeight = 420;

};
