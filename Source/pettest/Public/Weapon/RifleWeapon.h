// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonWeapon.h"
#include "RifleWeapon.generated.h"

class USkeletalMeshComponent;
class AThirdPersonWeapon;
class APlayerCharacter;

UCLASS()
class PETTEST_API ARifleWeapon : public AThirdPersonWeapon
{
	GENERATED_BODY()
	
public:	
	ARifleWeapon();

	virtual void StartFire() override;
	virtual void StopFire() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shooting")
	float BulletSpreadDefault = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shooting")
	float BulletSpreadAloft = 11.5f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooting")
	void Recoil(APawn* ApplyTo);

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooting")
	void Redo_Recoil(APawn* ApplyTo);

	UFUNCTION(Client, Reliable, Category = "Shooting")
	void Client_Recoil(APawn* ApplyTo, bool IsGoingUp);

	virtual void MakeShot() override;

private:
	FTimerHandle ShootingTimer;
};
