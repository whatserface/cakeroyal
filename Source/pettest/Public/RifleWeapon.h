// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RifleWeapon.generated.h"

UCLASS()
class PETTEST_API ARifleWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ARifleWeapon();

	UFUNCTION(Server, Unreliable, Category = "Shooting")
	void StartFire();
	UFUNCTION(Server, Unreliable, Category = "Shooting")
	void StopFire();
protected:
	virtual void BeginPlay() override;

};
