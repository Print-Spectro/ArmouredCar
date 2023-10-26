// Fill out your copyright notice in the Description page of Project Settings.

#include "MyArmouredCar.h"
#include "WheeledVehiclePawn.h"

//input
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "MyInputConfigData.h"
#include "MyInputConfigData.h"

//components
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "ChaosVehicleMovementComponent.h"
#include "MyRewindComponent.h"
#include "Components/TimelineComponent.h"
#include "Blueprint/UserWidget.h" //crosshair
	//Audio
	#include "Components/AudioComponent.h"

//debug
#include "DrawDebugHelpers.h"

//tools
#include "Kismet/KismetMathLibrary.h" 
#include "Kismet/GameplayStatics.h"

//Projectile to spawn
#include "MyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AMyArmouredCar::AMyArmouredCar() {
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));//Creating spring arm
	SpringArmComp->SetupAttachment(GetMesh());//Setting up attachment to the mesh

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArmComp);

	RewindComponent = CreateDefaultSubobject<UMyRewindComponent>(TEXT("RewindComponent"));

	FireTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("FireTimeline"));

	TurretAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("TurretRotationAudioComponent"));

	EngineAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudioComponent"));

	//const ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Script/Engine.CurveFloat'/Game/VehicleAssets/VehicleCurves/MyRecoilCurve.MyRecoilCurve'"));
}

void AMyArmouredCar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMyArmouredCar::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat progressFunction;
	progressFunction.BindUFunction(this, "setGunRecoil"); // The function EffectProgress gets called
	if (RecoilCurve) {
		FireTimelineComponent->AddInterpFloat(RecoilCurve, progressFunction, TEXT("RecoilTrack"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("AMyArmouredCar::BeginPlay: RecoilCurve not set"));
	}
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
	PEI->BindAction(InputActions->Rangefind, ETriggerEvent::Triggered, this, &AMyArmouredCar::rewind);
	PEI->BindAction(InputActions->InputFire, ETriggerEvent::Triggered, this, &AMyArmouredCar::fire);

}

// Called every frame
void AMyArmouredCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Setting the turret to follow the camera each tick
	FVector LookingAt = getLookingAT();
	interpTurretRotation(DeltaTime, getTurretRotationFromCamera(LookingAt));
	//setGunElevation(getGunEvevationFromCamera(LookingAt));
	interpGunElevation(DeltaTime, getGunEvevationFromCamera(LookingAt));
	//TurretRotation = ThirdPersonCamera->GetComponentRotation().Yaw - GetMesh()->GetComponentRotation().Yaw;
	FString FloatAsString = FString::Printf(TEXT("%f"), TurretRotation);
	//UE_LOG(LogTemp, Display, TEXT("%f"), TurretRotation);
	myDrawDebugLine(GetMesh()->GetComponentLocation(), ThirdPersonCamera->GetComponentLocation(), FColor::Red);



	if (!CanFire) {
		float NewPercent = UKismetMathLibrary::FInterpTo_Constant(ReloadPercent, 1, DeltaTime, 1.f/ReloadDelay);
		ReloadPercent = NewPercent;
	}
}

void AMyArmouredCar::Look(const FInputActionValue& Value){
	FVector2D LookValue = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Log, TEXT("LOOK"));
	if (Controller != nullptr) {
		//UE_LOG(LogTemp, Log, TEXT("Controller"));
		AddControllerYawInput(LookValue.X);
		AddControllerPitchInput(LookValue.Y);
	}
}

void AMyArmouredCar::Accelerate(const FInputActionValue& Value) {
	//Accelerate input sets throttle. Allows for linear input binding to trigger
	float Magnitude = Value.GetMagnitude();
	//UE_LOG(LogTemp, Display, TEXT("ThrottleDetected"));
	GetVehicleMovementComponent()->SetThrottleInput(Magnitude);
	//vehicle will brake if trying to drive forward while moving backwards, this allows for greater deceleration to change direction. 
	if (GetVehicleMovementComponent()->GetForwardSpeed() < -0.1) {
		GetVehicleMovementComponent()->SetBrakeInput(Magnitude);
	}
	if (GetVehicleMovementComponent()->GetForwardSpeed() > -0.1) {
		GetVehicleMovementComponent()->SetBrakeInput(0);
	}
}

void AMyArmouredCar::Steer(const FInputActionValue& Value) {

	float Magnitude = Value.GetMagnitude();
	GetVehicleMovementComponent()->SetSteeringInput(Magnitude);
}


void AMyArmouredCar::Brake(const FInputActionValue& Value) {
	//Setting brake to the brake float value. Allows for linear input binding with xBox controller for example

	GetVehicleMovementComponent()->SetBrakeInput(Value.GetMagnitude());
}

void AMyArmouredCar::fire(const FInputActionValue& Value) {
	if (!CanFire) {
		return;
	}
	CanFire = false;
	ReloadPercent = 0;
	//Spawning projectile
	FActorSpawnParameters SpawnInfo;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AMyProjectile>(ProjectileClass, GetMesh()->GetSocketLocation(FName("BarrelSocket")), GetMesh()->GetSocketRotation(FName("BarrelSocket")), SpawnInfo);
	UProjectileMovementComponent* ProjectileMovement = SpawnedActor->FindComponentByClass<UProjectileMovementComponent>();
	//ProjectileMovement->AddImpulse(GetVelocity(), TEXT("Root"), true);
	FireTimelineComponent->PlayFromStart();
	//Delay before setting can fire to true
	if (!GetWorld()->GetTimerManager().IsTimerActive(ReloadTimer)) {
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &AMyArmouredCar::setCanFireTrue, ReloadDelay, false, ReloadDelay);
	}
	if (ReloadDelay >= ReloadCompleteSoundDuration) {
		GetWorld()->GetTimerManager().SetTimer(ReloadCompleteSoundTimer, this, &AMyArmouredCar::playReloadCompletSound, ReloadDelay, false, ReloadDelay - ReloadCompleteSoundDuration);
	}
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetMesh()->GetBoneLocation("Barrel"));
	//Getting direction of gun to add impulse to vehicle
	FQuat RotationQuat = GetMesh()->GetBoneQuaternion(FName("Gunshield"));
	FVector Impulse =  UKismetMathLibrary::Quat_RotateVector(RotationQuat, FVector(RecoilImpulse, 0.f, 0.f));
	//recoil effect
	GetMesh()->AddImpulse(-Impulse, "Root", false);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("FIRE"));
}

void AMyArmouredCar::setCanFireTrue()
{
	CanFire = true;
}

void AMyArmouredCar::playReloadCompletSound()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadCompleteSound, GetMesh()->GetBoneLocation("Turret"));
}

void AMyArmouredCar::setGunRecoil(float value) {
	GunRecoil = value * MaxGunRecoil;
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
	//Line trace starting at the "gunshield" bone along the line of the gun barrel, returns first hit in the visibility channel
	FHitResult OutHitResult;
	//Start location of the line trace
	FVector Start = GetMesh()->GetBoneLocation(FName("Barrel"));
	//Getting rotation of the barrel bone to line trace along 
	FQuat RotationQuat = GetMesh()->GetBoneQuaternion(FName("Barrel"));
	//rotating finding the end point of the line trace to determine where the barrel is facing
	FVector End = Start + UKismetMathLibrary::Quat_RotateVector(RotationQuat, FVector(MaxAimDistance, 0.f, 0.f));
	//line trace
	bool HitResult = GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility);

	if (HitResult) {
		myDrawDebugLine(Start, OutHitResult.ImpactPoint, FColor::Red);
		return OutHitResult.ImpactPoint;
	}
	else {
		myDrawDebugLine(Start, End, FColor::Red);
		return End;
	}
}

void AMyArmouredCar::myDrawDebugLine(const FVector& Start, const FVector& End, FColor Colour = FColor::Red) {
	if (!DrawDebugs) {
		return;
	}
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
	//Getting the world rotation of the vehicle to rotate the looking at vector so that relative looking at is in vehicle rotation space
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
	//Taking the world "LookingAt" and making it relative to the turret bone by subtracting the vehicle location and rotating by the inverse of the vehicle rotation.
	FVector RelativeLookingAt = UKismetMathLibrary::Quat_RotateVector(MeshAngle.Inverse(), LookingAt - GetMesh()->GetBoneLocation(FName("Turret")));
	//Calculating the angle between the turret bone and the looking at location in the plane of the vehicle
	FRotator Rotation = FRotationMatrix::MakeFromX(RelativeLookingAt).Rotator();
	//We only want the Yaw
	float rotation = Rotation.Yaw;
	return rotation;
}

float AMyArmouredCar::getGunEvevationFromCamera(const FVector& LookingAt) {
	//Calculates and returns the rotation of the gunshield bone so that it looks at the same spot as the player camera
	//Getting the world rotation of the vehicle to rotate the looking at vector so that it has the same orientation as the vehicle
	FQuat MeshAngle = GetMesh()->GetComponentQuat();
	//Taking the world looking at and making it relative to the turret bone by subtracting the vehicle location and rotating by the inverse of the vehicle rotation.
	FVector RelativeLookingAt = UKismetMathLibrary::Quat_RotateVector(MeshAngle.Inverse(), LookingAt - GetMesh()->GetBoneLocation(FName("Gunshield")));
	//float rotation = FMath::Atan(RelativeLookingAt.Y / RelativeLookingAt.X) * 360/(2*3.14159265358979);
	FRotator Rotation = FRotationMatrix::MakeFromX(RelativeLookingAt).Rotator();
	float elevation = Rotation.Pitch;
	return elevation;
}

void AMyArmouredCar::interpGunElevation(float DeltaTime, const float& TargetRotation) {
	//linear interpolation from the current gun elevation to a target gun elevation
	float NewRotation = FMath::FInterpConstantTo(GunElevation, TargetRotation, DeltaTime, GunElevationRate);
	setGunElevation(NewRotation);
}

void AMyArmouredCar::interpTurretRotation(float DeltaTime, const float& TargetRotation) {
	//linear interpolation from the current turret rotation to a target rotation with some hacky maths to make it work properly
	float LocalTarget;
	//The shorter of the two rotations between two angles on a circle is always less than 180 degrees, in this instance, we just interpolate like normal
	if (abs(TargetRotation - TurretRotation) <= 180.f) {
		LocalTarget = TargetRotation;
	}
	//If the angle is greater than 180 degrees then we would be taking the longer path, hence we calculate a new local target that takes the shorter path
	else {
		//Here I am converting the angle to its compliment i.e the angle that gives the same position but going in the opposite direction around the circle. -90 degrees becomes 270 degrees.
		LocalTarget = ((TargetRotation < 0) - (TargetRotation > 0)) * (360 - abs(TargetRotation));
		//If the target rotation has the same sign as the turret rotation then we set both to their remainder with respect to 360, to keep the numbers in check (we also know that the interp distance is less than 180).
		if ((TargetRotation > 0) - (TargetRotation < 0) == (TurretRotation > 0) - (TurretRotation < 0)) {
			//Used to keep the numbers in check, without this, the interpolation would break after 1 full rotation
			LocalTarget = TargetRotation;
			setTurretRotation(remainder(TurretRotation, 360));
		}
	}
	float NewRotation = FMath::FInterpConstantTo(TurretRotation, LocalTarget, DeltaTime, TurretRotationRate);
	setTurretRotation(NewRotation);
	//Handling turret rotation sound
	if (LocalTarget != TurretRotation) {
			TurretAudioComponent->SetPaused(false);
	}
	else {
		TurretAudioComponent->SetPaused(true);
	}
}

void AMyArmouredCar::setTurretRotation(const float& rotation) {
	TurretRotation = rotation;
}

void AMyArmouredCar::setGunElevation(const float& elevation) {
	//Sets the value of gun elevation exposed to the animation instance
	//Only sets elevation if within max and min elevation default values.
	if ((GunElevationLimit > elevation) && (elevation > -GunDepressionLimit)) {
		GunElevation = elevation;
	}
}

void AMyArmouredCar::rewind() {
	RewindComponent->rewind();
}

