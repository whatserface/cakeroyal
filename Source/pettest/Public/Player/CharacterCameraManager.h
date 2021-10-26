// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CharacterCameraManager.generated.h"

class APlayerCharacter;

UCLASS()
class PETTEST_API ACharacterCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ACharacterCameraManager();
	virtual void UpdateCamera(float DeltaTime) override;

protected:
	UPROPERTY(Transient)
	APlayerCharacter* MyPawn;

	virtual void BeginPlay() override;
};
