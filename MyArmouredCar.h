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

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Transform")
	float MaxAimDistance = 10000;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Transform")
	float TurretRotation;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Transform")
	float GunElevation;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Transform")
	float GunReciol;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Defaults")
	float GunElevationLimit = 20;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Defaults")
	float GunDepressionLimit = 10;

	UChaosVehicleMovementComponent* MovementComponent;

	void Look(const FInputActionValue& Value);

	void Accelerate(const FInputActionValue& Value);

	void Steer(const FInputActionValue& Value);

	void Brake(const FInputActionValue& Value);

	void Mydrawdebugline(const FVector& Start, const FVector& End, FColor Colour);

	FVector Getlookingat();

	FVector Getaimingat();

	void Setturretrotation(const float& rotation);

	void Setgunelevation(const float& elevation);

	float Getturretrotationfromcamera(const FVector& LookingAt);

	float Getgunevelationfromcamera(const FVector& LookingAt);

	FVector Getrelativelookingat(const FVector& LookingAt);
};
