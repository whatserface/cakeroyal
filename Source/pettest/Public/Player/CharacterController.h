// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

UCLASS()
class PETTEST_API ACharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	ACharacterController();

	void PostLogin();

	virtual void OnRep_Pawn() override;

	UFUNCTION(BlueprintCallable, Category = "Start Spectating Player Controller")
	void StartSpectating();

	UFUNCTION(Client, Reliable)
	void Client_StartSpectating();

	UFUNCTION(BlueprintCallable, Category = "Start Spectating Player Controller")
	void StartPlaying();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    class URespawnComponent* RespawnComponent;
};
