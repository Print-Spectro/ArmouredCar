//Actor component that allows rewinding through time


#include "MyRewindComponent.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyArmouredCar.h"

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
}


// Called every frame
void UMyRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Rewind) {
		//Interpolate between current and target location, setting location each frame.
		if (Interpolate)
		{
			float Velocity = abs(FVector::Distance(PreviousLocation, LocationArray[index]) * SampleRate);
			float RotationVelocity = abs((RotationArray[index].GetManhattanDistance(PreviousRotation)) * SampleRate);
			FVector InterpLocation = FMath::VInterpConstantTo(GetOwner()->GetActorLocation(), LocationArray[index], DeltaTime, Velocity);
			FRotator InterpRotation = FMath::RInterpConstantTo(GetOwner()->GetActorRotation(), RotationArray[index], DeltaTime, RotationVelocity);

			GetOwner()->SetActorLocation(InterpLocation, true, nullptr, ETeleportType::ResetPhysics);
			GetOwner()->SetActorRotation(InterpRotation, ETeleportType::ResetPhysics);
		}else if (!Interpolate)
		{
			GetOwner()->SetActorLocation(LocationArray[index], true, nullptr, ETeleportType::ResetPhysics);
			GetOwner()->SetActorRotation(RotationArray[index], ETeleportType::ResetPhysics);
		}
	}
}

void UMyRewindComponent::rewind() {
	//Rewind through the list of saved positions
	Record = false;
	Rewind = true;
	index = LocationArray.Num()-1;
	PreviousLocation = GetOwner()->GetActorLocation();
	PreviousRotation = GetOwner()->GetActorRotation();
	Cast<AMyArmouredCar>(GetOwner())->GetMesh()->SetSimulatePhysics(0);
	GetWorld()->GetTimerManager().SetTimer(RewindTimer, this, &UMyRewindComponent::replay, 1 / SampleRate, true, 1 / SampleRate);
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
		LocationArray.Empty();
		RotationArray.Empty();
		Cast<AMyArmouredCar>(GetOwner())->GetMesh()->SetSimulatePhysics(1);	
	}
}

