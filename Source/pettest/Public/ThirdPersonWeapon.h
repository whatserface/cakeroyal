// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirstPersonWeapon.h"
#include "GameFramework/Actor.h"
#include "ThirdPersonWeapon.generated.h"

class USkeletalMeshComponent;
class AFirstPersonWeapon;

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float Damage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float ShootLength = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 MaxAmmo = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AFirstPersonWeapon> FPPWeaponClass;
};

UCLASS()
class PETTEST_API AThirdPersonWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AThirdPersonWeapon();
	
	virtual void StartFire();

	FWeaponInfo GetWeaponInfo() const { return WeaponInfo; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FWeaponInfo WeaponInfo;
};
