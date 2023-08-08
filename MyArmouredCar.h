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

	UChaosVehicleMovementComponent* MovementComponent;

	void Look(const FInputActionValue& Value);

	void Accelerate(const FInputActionValue& Value);

	void Steer(const FInputActionValue& Value);
};
