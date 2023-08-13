// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRewindComponent.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UMyRewindComponent::UMyRewindComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// Binding my sample timer to the record function so I can sample the transform of the vehicle. Giving the timer a pointer doesn't seem to be working...
	SampleDelegate.BindLambda([&]{
				record();
		});
	// ...
}


// Called when the game starts
void UMyRewindComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetWorld()->GetTimerManager().IsTimerActive(SampleTimer)) {
		GetWorld()->GetTimerManager().SetTimer(SampleTimer, SampleDelegate, 1/SampleRate, true);
	}
	//GetWorld()->GetTimerManager().SetTimer(SampleTimer, this, UMyRewindComponent::record(), 1/SampleRate, true, Interval);
	// ...
	
}


// Called every frame
void UMyRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Rewind) {
		//Interpolate between current and target location, setting location each frame.
		FVector InterpLocation = FMath::VInterpConstantTo(GetOwner()->GetActorLocation(), LocationArray[index], DeltaTime, FVector::Distance(PreviousLocation, LocationArray[index])*SampleRate);
		FRotator InterpRotation = FMath::RInterpConstantTo(GetOwner()->GetActorRotation(), RotationArray[index], DeltaTime, ((RotationArray[index] - PreviousRotation).GetNormalized().Euler() * SampleRate).Size());
	
		GetOwner()->SetActorLocation(InterpLocation, true, nullptr, ETeleportType::TeleportPhysics);
		GetOwner()->SetActorRotation(InterpRotation, ETeleportType::TeleportPhysics);
	
	}


}

void UMyRewindComponent::rewind() {
	//Rewind through the list of saved positions
	Record = false;
	Rewind = true;
	index = LocationArray.Num()-1;
	PreviousLocation = GetOwner()->GetActorLocation();
	PreviousRotation = GetOwner()->GetActorRotation();
	GetWorld()->GetTimerManager().SetTimer(RewindTimer, this, &UMyRewindComponent::replay, 1 / SampleRate, true, 0);
}

void UMyRewindComponent::record() {
	//Records the state parameters of the parent actor to arrays 


	//Allows switching on an off recording
	if (!Record) {
		return;
	}

	//When the buffer is full, based on the set parameters, we remove the first element every time we add an element
	if (LocationArray.Num() > SampleBufferLength - 1) {
		LocationArray.RemoveAt(0);
		RotationArray.RemoveAt(0);
		BufferIsFull = true;
	}
	LocationArray.Add(GetOwner()->GetActorLocation());
	RotationArray.Add(GetOwner()->GetActorRotation());

}

void UMyRewindComponent::replay() {
	//switch on and off rewind
	if (!Rewind) {
		return;
	}
	//Iterate through arrays starting at the end once every 1/sample rate seconds: framerate independent 
	index--;
	PreviousLocation = LocationArray[index + 1];
	PreviousRotation = RotationArray[index + 1];
	if (index <= 0) {
		GetWorld()->GetTimerManager().ClearTimer(RewindTimer);
		Record = true;
		Rewind = false;
		
	}

}

