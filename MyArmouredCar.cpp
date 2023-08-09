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

	PEI->BindAction(InputActions->InputAccelerate, ETriggerEvent::Triggered , this, &AMyArmouredCar::Accelerate);
	PEI->BindAction(InputActions->InputAccelerate, ETriggerEvent::Canceled, this, &AMyArmouredCar::Accelerate);


}


// Called every frame
void AMyArmouredCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GetVehicleMovementComponent()->SetThrottleInput(1.f);
}

void AMyArmouredCar::Look(const FInputActionValue& Value){
	FVector2D LookValue = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Log, TEXT("LOOK"));
	if (Controller != nullptr) {
		UE_LOG(LogTemp, Log, TEXT("Controller"));
		AddControllerYawInput(LookValue.X);
		AddControllerPitchInput(LookValue.Y);
	}
}

void AMyArmouredCar::Accelerate(const FInputActionValue& Value) {


	float Magnitude = Value.GetMagnitude();
	UE_LOG(LogTemp, Display, TEXT("ThrottleDetected"));
	GetVehicleMovementComponent()->SetThrottleInput(Magnitude);


	
}

void AMyArmouredCar::Steer(const FInputActionValue& Value) {

	float Magnitude = Value.GetMagnitude();
	GetVehicleMovementComponent()->SetSteeringInput(Magnitude);
}