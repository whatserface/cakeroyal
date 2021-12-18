// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonWeapon.h"
#include "RifleWeapon.generated.h"

class USkeletalMeshComponent;
class AThirdPersonWeapon;
class APlayerCharacter;
class UNiagaraSystem;
class UPhysicalMaterial;

UCLASS()
class PETTEST_API ARifleWeapon : public AThirdPersonWeapon
{
	GENERATED_BODY()
	
public:	
	ARifleWeapon();

	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* TraceFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	FString TraceTargetName = "LaserEnd";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TMap<UPhysicalMaterial*, UNiagaraSystem*> ImpactDataMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting")
	float BulletSpreadDefault = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting")
	float BulletSpreadAloft = 23.f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooting")
	void Recoil(APawn* ApplyTo);

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooting")
	void Redo_Recoil(APawn* ApplyTo);

	UFUNCTION(Client, Reliable, Category = "Shooting")
	void Client_Recoil(APawn* ApplyTo, bool IsGoingUp);

	virtual void MakeShot() override;

	UFUNCTION(NetMulticast, Unreliable)
	void PlayImpactFX(const FHitResult& Hit);

private:
	FTimerHandle ShootingTimer;

	UFUNCTION(NetMulticast, Unreliable)
	void SpawnTraceFX(const FVector& TraceStart, const FVector& TraceEnd);

	void SetFXActive(bool IsActive);
};
