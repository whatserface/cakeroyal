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

	void StartFire();
	void StopFire();

	bool GetAmmoPercent(float& OutAmmoPercent);

	UFUNCTION(Server, Unreliable)
	void Reload();

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UAnimMontage* RifleMontageFPP;
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(Replicated)
	AThirdPersonWeapon* TPPWeapon = nullptr;
	
	AFirstPersonWeapon* FPPWeapon = nullptr;

	UFUNCTION(Server, Reliable, Category = "Shooting")
	void SpawnTPPWeapon();
	
	UFUNCTION(Client, Unreliable, Category = "Shooting")
	void SpawnFPPWeapon();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayReloadAnim();

	void PlayAnimMontage(UAnimMontage* Animation);
};
