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
class UNiagaraComponent;
class UNiagaraSystem;
class UWeaponComponent;
class USoundCue;

UCLASS(Abstract, Blueprintable)
class PETTEST_API AThirdPersonWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AThirdPersonWeapon();

	FOnAmmoChanged OnAmmoChanged;
	FOnTraceAppeared OnTraceAppeared; //required for rifle, though it's really shouldn't be here, but the design of project is just kinda fucked up, so yeah...

	void Reload();

	virtual void StartFire();
	virtual void StopFire();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool CanShoot();

	bool CanReload() const { return Bullets != WeaponInfo.Bullets; }
	bool IsAmmoEmpty() const { return Bullets == 0; }
	FWeaponInfo GetWeaponInfo() const { return WeaponInfo; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = "0", ClampMax = "1000"))
	float Damage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = "0", ClampMax = "75000"))
	float ShootLength = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FWeaponInfo WeaponInfo;

	UPROPERTY(Replicated, Transient)
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX", meta = (Tooltip = "Keep it in sync with first person weapon!"))
	FName MuzzleSocketName = "MuzzleFlashSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* FireSound;

	UFUNCTION(Client, Unreliable, WithValidation)
	void Client_InvokeAmmoChanged(int32 Ammo);

	virtual void BeginPlay() override;
	void ReduceAmmo();
	
	void LogAmmo();

private:
	UPROPERTY(Replicated, Transient)
	int32 Bullets;
};

