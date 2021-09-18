// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RifleWeapon.generated.h"

class USkeletalMeshComponent;

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

	UFUNCTION(NetMulticast, Unreliable, WithValidation, Category = "Spawn")
	void AttachToPlayer(const FName& SocketName);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Appearance")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting")
	float ShootLength = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting")
	float DamageAmount = 50.0f;
	
	virtual void BeginPlay() override;
};
