// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "MyArmouredCar.generated.h"

/**
 * Storing variables that determine the state of the armoured car
 */
UCLASS()
class ARMOUREDCAR_API AMyArmouredCar : public AWheeledVehiclePawn
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AMyArmouredCar();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//creating player input component
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	class UMyInputConfigData* InputActions;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	class UCameraComponent* ThirdPersonCamera;


	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Transform")
	float TurretRotation;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Transform")
	float GunElevation;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Transform")
	float GunReciol;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Defaults", Replicated)
	//Aim distance in cm. 1000000cm = 10km
	//Trace distance for "getLookingAt" and "getAimingAT" functions 
	float MaxAimDistance = 1000000;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Defaults")
	//Elevation in degrees
	float GunElevationLimit = 20;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Defaults")
	//Gun depression in degrees
	float GunDepressionLimit = 10;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Defaults")
	//Rate in degrees per second
	float GunElevationRate = 20;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Defaults")
	//Rate in degrees per second
	float TurretRotationRate = 40;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	class UMyRewindComponent* RewindComponent;

	UChaosVehicleMovementComponent* MovementComponent;

	void Look(const FInputActionValue& Value);

	void Accelerate(const FInputActionValue& Value);

	void Steer(const FInputActionValue& Value);

	void Brake(const FInputActionValue& Value);

	void myDrawDebugLine(const FVector& Start, const FVector& End, FColor Colour);

	FVector getLookingAT();

	FVector getAimingAT();

	void setTurretRotation(const float& rotation);

	void setGunElevation(const float& elevation);

	float getTurretRotationFromCamera(const FVector& LookingAt);

	float getGunEvevationFromCamera(const FVector& LookingAt);

	FVector getRelativeLookingAt(const FVector& LookingAt);

	void interpGunElevation(float DeltaTime, const float& TargetLocation);

	void interpTurretRotation(float DeltaTime, const float& TargetRotation);

	void rewind();
};
