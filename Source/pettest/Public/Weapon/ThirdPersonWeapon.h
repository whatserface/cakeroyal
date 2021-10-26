// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/FirstPersonWeapon.h"
#include "MyCoreTypes.h"
#include "GameFramework/Actor.h"
#include "ThirdPersonWeapon.generated.h"

class USkeletalMeshComponent;
class AFirstPersonWeapon;
class APlayerCharacter;

UCLASS(Abstract, Blueprintable)
class PETTEST_API AThirdPersonWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AThirdPersonWeapon();

	FOnReload OnReload;

	void Reload();

	virtual void StartFire();
	virtual void StopFire();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool CanReload() const { return Bullets != WeaponInfo.Bullets; }
	bool IsAmmoEmpty() const { return Bullets == 0; }
	void GetWeaponBullets(float& OutAmmoPercent) const;
	FWeaponInfo GetWeaponInfo() const { return WeaponInfo; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = "0", ClampMax = "1000"))
	float Damage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = "0", ClampMax = "75000"))
	float ShootLength = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float ShootingRate = -1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FWeaponInfo WeaponInfo;

	virtual void BeginPlay() override;
	void ReduceAmmo();
	virtual void MakeShot() { return; }
	virtual bool CanShoot();
	
	void LogAmmo();

private:
	UPROPERTY(Replicated, Transient)
	int32 Bullets;
};

