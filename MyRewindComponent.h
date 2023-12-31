// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyRewindComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARMOUREDCAR_API UMyRewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyRewindComponent();

	void rewind();

	//units in Hertz (per second) 
	UPROPERTY(EditDefaultsOnly, Category = "Rewind Parameters", meta = (ToolTip = "Set the rewind sample rate in Hertz (per second)"))
	float SampleRate = 200;

	//units in seconds
	UPROPERTY(EditDefaultsOnly, Category = "Rewind Parameters", meta = (ToolTip = "Set the duration of the rewind buffer in seconds"))
	float SampleBufferTime = 3;

	//Allows for interpolation between data points for smoother movement
	UPROPERTY(EditDefaultsOnly, Category = "Rewind Parameters", meta = (ToolTip = "Whether rewind should interoplate between data points"))
	bool Interpolate = 0;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void record();

	void replay();

	TArray<FVector> LocationArray;

	TArray<FRotator> RotationArray;

	float SampleBufferLength = SampleBufferTime * SampleRate;

	bool Rewind = false;

	bool Record = true;

	bool BufferIsFull = false;

	int index;

	FVector PreviousLocation;

	FRotator PreviousRotation;

	FTimerHandle SampleTimer;

	FTimerDelegate SampleDelegate;

	FTimerHandle RewindTimer;

	FTimerHandle RewindDelegate;
};
