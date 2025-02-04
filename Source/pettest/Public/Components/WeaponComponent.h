// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCoreTypes.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class AThirdPersonWeapon;
class AFirstPersonWeapon;
class APlayerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PETTEST_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	FOnAmmoChanged OnAmmoChanged;
	FOnTraceAppeared OnTraceAppeared;

	bool CanShoot() const;

	UFUNCTION(Server, Unreliable)
	void StartFire();

	UFUNCTION(Server, Unreliable)
	void StopFire();

	int32 GetMaxAmmo() const;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Weapon")
	void Reload();
	
	float GetFireRate() const;
	bool CanReload() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(Transient, Replicated)
	APlayerCharacter* MyPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AThirdPersonWeapon> TPPWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName WeaponSocketName = "WeaponSocket";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UAnimMontage* ReloadMontageTPP;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UAnimMontage* ReloadMontageFPP;
	
	virtual void BeginPlay() override;

private:
	/** Because of the replication unpredictability not always the*/
	FTimerHandle SpawnFPPWeaponTimerHandle;

	UPROPERTY(Replicated)
	AThirdPersonWeapon* TPPWeapon = nullptr;
	
	AFirstPersonWeapon* FPPWeapon = nullptr;

	UPROPERTY(Replicated)
	bool bReloadAnimInProgress = false;

	UFUNCTION(Client, Unreliable, Category = "Weapon")
	void SpawnFPPWeapon();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayReloadAnim();

	UFUNCTION(Client, Unreliable, Category = "VFX")
	void TraceAppeared(FVector TraceEnd);

	UFUNCTION(Server, Reliable, WithValidation)
	void StopSpawnTimer();

	void SpawnTPPWeapon();
	void AmmoChanged(int32 AmmoBullets);

	void OnReloadFinished(USkeletalMeshComponent* MeshComp);
	void InitAnimations();
};
