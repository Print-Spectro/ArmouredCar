// Fill out your copyright notice in the Description page of Project Settings.

#include "MyArmouredCar.h"
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "ChaosVehicleMovementComponent.h"
#include "WheeledVehiclePawn.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h" //used for rotating vector using quaternion
#include "MyInputConfigData.h"



AMyArmouredCar::AMyArmouredCar() {
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));//Creating spring arm
	SpringArmComp->SetupAttachment(GetMesh());//Setting up attachment to the mesh

	//MovementComponent = 

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArmComp);
}

void AMyArmouredCar::BeginPlay()
{
	Super::BeginPlay();

	//AnimInstance Cast<>


}

void AMyArmouredCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Get player controller
	APlayerController* PC = Cast<APlayerController>(GetController());

	// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	//Clear out existing mapping, and add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	//Getting the EnhancedInputComponent
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	//Binding the actions
	PEI->BindAction(InputActions->InputSteer, ETriggerEvent::Triggered, this, &AMyArmouredCar::Steer);
	PEI->BindAction(InputActions->InputSteer, ETriggerEvent::Completed, this, &AMyArmouredCar::Steer);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &AMyArmouredCar::Look);
	PEI->BindAction(InputActions->InputBrake, ETriggerEvent::Triggered, this, &AMyArmouredCar::Brake);
	PEI->BindAction(InputActions->InputBrake, ETriggerEvent::Completed, this, &AMyArmouredCar::Brake);
	PEI->BindAction(InputActions->InputAccelerate, ETriggerEvent::Triggered , this, &AMyArmouredCar::Accelerate);
	PEI->BindAction(InputActions->InputAccelerate, ETriggerEvent::Completed, this, &AMyArmouredCar::Accelerate);


}


// Called every frame
void AMyArmouredCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GetVehicleMovementComponent()->SetThrottleInput(1.f);

	//Setting the turret to follow the camera each tick
	FVector LookingAt = getLookingAT();
	setTurretRotatoin(getTurretRotationFromCamera(LookingAt));
	//setGunElevation(getGunEvevationFromCamera(LookingAt));
	interpGunElevation(DeltaTime, getGunEvevationFromCamera(LookingAt));

	//TurretRotation = ThirdPersonCamera->GetComponentRotation().Yaw - GetMesh()->GetComponentRotation().Yaw;
	FString FloatAsString = FString::Printf(TEXT("%f"), TurretRotation);
	UE_LOG(LogTemp, Display, TEXT("%f"), TurretRotation);
	myDrawDebugLine(GetMesh()->GetComponentLocation(), ThirdPersonCamera->GetComponentLocation(), FColor::Red);
	//GetLookingAt();
	getAimingAT();
}

void AMyArmouredCar::Look(const FInputActionValue& Value){
	FVector2D LookValue = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Log, TEXT("LOOK"));
	if (Controller != nullptr) {
		//UE_LOG(LogTemp, Log, TEXT("Controller"));
		AddControllerYawInput(LookValue.X);
		AddControllerPitchInput(LookValue.Y);
	}
}

void AMyArmouredCar::Accelerate(const FInputActionValue& Value) {
	//Accelerate input sets throttle. Allows for linear input binding to trigg

	float Magnitude = Value.GetMagnitude();
	//UE_LOG(LogTemp, Display, TEXT("ThrottleDetected"));
	GetVehicleMovementComponent()->SetThrottleInput(Magnitude);


	
}

void AMyArmouredCar::Steer(const FInputActionValue& Value) {

	float Magnitude = Value.GetMagnitude();
	GetVehicleMovementComponent()->SetSteeringInput(Magnitude);
}


void AMyArmouredCar::Brake(const FInputActionValue& Value) {
	//Setting brake to the brake float value. Allows for linear input binding with xBox controller for example

	GetVehicleMovementComponent()->SetBrakeInput(Value.GetMagnitude());
}

FVector AMyArmouredCar::getLookingAT() {
	//Line trace visible geometry from the player camera and return hit location or the end of the linetrace if no geometry is hit.
	FHitResult OutHitResult;
	FVector Start = ThirdPersonCamera->GetComponentLocation(); //LineTrace from the camera
	FVector End = Start + ThirdPersonCamera->GetForwardVector() * MaxAimDistance;
	bool HitResult = GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility);

	if (HitResult) {

		myDrawDebugLine(Start, OutHitResult.ImpactPoint, FColor::Green);

		return OutHitResult.ImpactPoint;

	}
	else {
		return End;
	}
}

FVector AMyArmouredCar::getAimingAT() {
	FHitResult OutHitResult;

	FVector Start = GetMesh()->GetBoneLocation(FName("Gunshield"));

	FQuat RotationQuat = GetMesh()->GetBoneQuaternion(FName("Gunshield"));

	FVector End = Start + UKismetMathLibrary::Quat_RotateVector(RotationQuat, FVector(MaxAimDistance, 0.f, 0.f));
	
	bool HitResult = GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility);

	if (HitResult) {
		DrawDebugLine(
			GetWorld(),
			Start,
			End,
			FColor::Red,
			false, // Persistent lines
			0.1    // Lifetime
		);
		return OutHitResult.ImpactPoint;

	}
	else {
		myDrawDebugLine(Start, End, FColor::Red);
		return End;
	}
}

void AMyArmouredCar::myDrawDebugLine(const FVector& Start, const FVector& End, FColor Colour = FColor::Red) {
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		Colour,
		false, // Persistent lines
		0.1    // Lifetime
	);


}

FVector AMyArmouredCar::getRelativeLookingAt(const FVector& LookingAt) {
	//Getting the world rotation of the vehicle to rotate the looking at vector so that it has the same orientation as the vehicle
	FQuat MeshAngle = GetMesh()->GetComponentQuat();
	//Taking the world looking at and making it relative to the turret bone by subtracting the vehicle location and rotating by the inverse of the vehicle rotation.
	FVector RelativeLookingAt = UKismetMathLibrary::Quat_RotateVector(MeshAngle.Inverse(), LookingAt - GetMesh()->GetBoneLocation(FName("Turret")));
	
	return RelativeLookingAt;
}

float AMyArmouredCar::getTurretRotationFromCamera(const FVector& LookingAt) {
	//Takes the looking at vector (location) from the getLookingAt function 
	//Returns the rotation of the turret bone so that it looks at the same spot as the player camera

	//Getting the world rotation of the vehicle to rotate the looking at vector so that it has the same orientation as the vehicle
	FQuat MeshAngle = GetMesh()->GetComponentQuat();
	//Taking the world looking at and making it relative to the turret bone by subtracting the vehicle location and rotating by the inverse of the vehicle rotation.
	FVector RelativeLookingAt = UKismetMathLibrary::Quat_RotateVector(MeshAngle.Inverse(), LookingAt - GetMesh()->GetBoneLocation(FName("Turret")));

	//Calculating the angle between the turret bone and the looking at location in the plane of the vehicle
	FRotator Rotation = FRotationMatrix::MakeFromX(RelativeLookingAt).Rotator();
	//We only want the Yaw
	float rotation = Rotation.Yaw;
	return rotation;
}

float AMyArmouredCar::getGunEvevationFromCamera(const FVector& LookingAt) {
	//Calculates and returns the rotation of the turret bone so that it looks at the same spot as the player camera
	//Getting the world rotation of the vehicle to rotate the looking at vector so that it has the same orientation as the vehicle
	FQuat MeshAngle = GetMesh()->GetComponentQuat();
	//Taking the world looking at and making it relative to the turret bone by subtracting the vehicle location and rotating by the inverse of the vehicle rotation.
	FVector RelativeLookingAt = UKismetMathLibrary::Quat_RotateVector(MeshAngle.Inverse(), LookingAt - GetMesh()->GetBoneLocation(FName("Gunshield")));
	//float rotation = FMath::Atan(RelativeLookingAt.Y / RelativeLookingAt.X) * 360/(2*3.14159265358979);
	FRotator Rotation = FRotationMatrix::MakeFromX(RelativeLookingAt).Rotator();
	float elevation = Rotation.Pitch;
	return elevation;
}

void AMyArmouredCar::interpGunElevation(float DeltaTime, const float& TargetLocation) {

	float NewLocation = FMath::FInterpConstantTo(GunElevation, TargetLocation, DeltaTime, GunElevationRate);
	setGunElevation(NewLocation);
}

void AMyArmouredCar::setTurretRotatoin(const float& rotation) {
	TurretRotation = remainder(rotation, 360);
}

void AMyArmouredCar::setGunElevation(const float& elevation) {
	//Sets the value of gun elevation exposed to the animation instance
	//Only sets elevation if withing max and min elevation default values.
	if ((GunElevationLimit > elevation) && (elevation > -GunDepressionLimit)) {
		GunElevation = elevation;
	}
}

