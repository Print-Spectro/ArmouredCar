// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRewindComponent.h"

// Sets default values for this component's properties
UMyRewindComponent::UMyRewindComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMyRewindComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMyRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (record)
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		if (LocationArray.Num() > 300) {
			LocationArray.RemoveAt(0);
			LocationArray.Add(GetOwner()->GetActorLocation());
		}
		else {
			LocationArray.Add(GetOwner()->GetActorLocation());
		}
		index = (LocationArray).Num()-1;
	}
	
	if (rewind) {
		GetOwner()->SetActorLocation(LocationArray[index], true, nullptr, ETeleportType::TeleportPhysics);
		index--;
		if (index <= 0) {
			rewind = false;
			record = true;
		}
	}
	


}

void UMyRewindComponent::Rewind() {
	record = false;
	rewind = true;
}

