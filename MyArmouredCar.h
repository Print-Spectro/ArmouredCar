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

	UPROPERTY()
	class UTimelineComponent* FireTimelineComponent;

public:
	// Sets default values for this character's properties
	AMyArmouredCar();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

// 	UPROPERTY(EditDefaultsOnly)
// 	FTimeline FireTimeline;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//creating player input component
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ReloadPercent = 100;

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
	float GunRecoil;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Defaults")
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

	UPROPERTY(EditAnywhere, Category = "Defaults")
	//How far back the gun recoils when firing in cm
	float MaxGunRecoil = 70;

	UPROPERTY(EditAnywhere, Category = "Defaults")
	//How far back the gun recoils when firing in cm
	float RecoilImpulse = 100000;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	float ReloadDelay = 3;

	UPROPERTY(EditAnywhere, Category = "Defaults")
	//How far back the gun recoils when firing in cm
	bool DrawDebugs = false;

	UPROPERTY(EditAnywhere, Category = "Defaults")
	class UCurveFloat* RecoilCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	class UMyRewindComponent* RewindComponent;

	//Set projectile blueprint to spawn (allows setting of mesh in blueprint)
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	TSubclassOf<class AMyProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	float  ReloadCompleteSoundDuration = 0.6;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	class USoundBase* ReloadCompleteSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	class UAudioComponent* TurretAudioComponent;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category = "Sounds")
	class UAudioComponent* EngineAudioComponent;

	UPROPERTY()
	FTimerHandle ReloadTimer;

	UPROPERTY()
	FTimerHandle ReloadCompleteSoundTimer;

	UPROPERTY(EditDefaultsOnly)
	class UTimelineComponent* ReloadTimeline;

	UPROPERTY()
	bool CanFire = true;

	UPROPERTY()
	UChaosVehicleMovementComponent* MovementComponent;

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void Accelerate(const FInputActionValue& Value);

	UFUNCTION()
	void Steer(const FInputActionValue& Value);

	UFUNCTION()
	void Brake(const FInputActionValue& Value);

	UFUNCTION()
	void fire(const FInputActionValue& Value);

	UFUNCTION()
	void setCanFireTrue();

	UFUNCTION()
	void playReloadCompletSound();

	void myDrawDebugLine(const FVector& Start, const FVector& End, FColor Colour);

	FVector getLookingAT();

	UFUNCTION(BlueprintCallable)
	FVector getAimingAT();

	void setTurretRotation(const float& rotation);

	void setGunElevation(const float& elevation);

	UFUNCTION()
	void setGunRecoil(float Value);

	float getTurretRotationFromCamera(const FVector& LookingAt);

	float getGunEvevationFromCamera(const FVector& LookingAt);

	FVector getRelativeLookingAt(const FVector& LookingAt);

	void interpGunElevation(float DeltaTime, const float& TargetLocation);

	void interpTurretRotation(float DeltaTime, const float& TargetRotation);

	void rewind();

};
