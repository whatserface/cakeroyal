// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonWeapon.h"
#include "RifleWeapon.generated.h"

class USkeletalMeshComponent;
class AThirdPersonWeapon;
class ACharacter;
class UNiagaraSystem;
class UPhysicalMaterial;
class UAudioComponent;
class UCameraShakeBase;

UCLASS()
class PETTEST_API ARifleWeapon : public AThirdPersonWeapon
{
	GENERATED_BODY()
	
public:	
	ARifleWeapon();

	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool CanShoot() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	FImpactData DefaultImpactData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* TraceFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	FString TraceTargetName = "LaserEnd";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TMap<UPhysicalMaterial*, FImpactData> ImpactDataMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TSubclassOf<UCameraShakeBase> ShootCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting")
	float BulletSpreadDefault = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting")
	float BulletSpreadAloft = 23.f;

	UFUNCTION(Server, Unreliable)
	void MakeShot();
	
	UFUNCTION(Client, Unreliable)
	void StartFireClient();
	
	UFUNCTION(Client, Unreliable)
	void StopFireClient();

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooting")
	void Recoil(APawn* ApplyTo);

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooting")
	void Redo_Recoil(APawn* ApplyTo);

	UFUNCTION(Client, Reliable, Category = "Shooting")
	void Client_Recoil(APawn* ApplyTo, bool IsGoingUp);

	UFUNCTION(NetMulticast, Unreliable)
	void PlayImpactFX(const FHitResult& Hit);

	virtual void BeginPlay() override;

private:
	FTimerHandle ShootingTimer;

	FTimerHandle SoundNodeDelay;

	/** This timer handle controls whether the time of ShootingRate has passed or not.
	So essentially because of it the player can't fire faster than rate of fire.*/
	FTimerHandle FireRateControllerTimer;
	bool bHasPassedFireRateDelay = true;

	/** If the player didn't made any shot (e.g. was reloading, but tried to shoot), we don't make a delay*/
	bool bMadeAnyShots = false;

	/** As we only need to know once our owner we fill in this placeholder variable just for optimization reasons*/
	UPROPERTY(Transient)
	APlayerCharacter* MyCharacter;

	UPROPERTY()
	UAudioComponent* FireAudioComponent;

	UFUNCTION(NetMulticast, Unreliable)
	void SpawnTraceFX(const FVector& TraceStart, const FVector& TraceEnd);

	UFUNCTION(NetMulticast, Unreliable)
	void InitFX();

	UFUNCTION(NetMulticast, Unreliable)
	void SetFXActive(bool IsActive);

	void ControlFireRateDelay();

	void AddActorCacheToSoundNode();
};
