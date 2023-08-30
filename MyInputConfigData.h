// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "MyInputConfigData.generated.h"

/**
 * Stores pointers to input actions for native binding
 */
UCLASS()
class ARMOUREDCAR_API UMyInputConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAccelerate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputLook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputSteer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputBrake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Rangefind;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* SwitchView;
};
