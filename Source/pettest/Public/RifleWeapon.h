// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonWeapon.h"
#include "RifleWeapon.generated.h"

class USkeletalMeshComponent;
class AThirdPersonWeapon;

UCLASS()
class PETTEST_API ARifleWeapon : public AThirdPersonWeapon
{
	GENERATED_BODY()
	
public:	
	ARifleWeapon();

	UFUNCTION(Server, Unreliable, Category = "Shooting")
	virtual void StartFire();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Shooting")
	void Recoil(APawn* ApplyTo);

	UFUNCTION(Client, Reliable, Category = "Shooting")
	void Client_Recoil(APawn* ApplyTo);
};
