// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class ARifleWeapon;

USTRUCT(BlueprintType)
struct FTPWeapon
{
	GENERATED_USTRUCT_BODY()

	ARifleWeapon* FPPWeapon;
	ARifleWeapon* TPPWeapon;
	bool DestroyAndDetach(AActor* DestroyWeapon)
	{
		if (!DestroyWeapon) return false;

		DestroyWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		DestroyWeapon->Destroy();
		return true;
	}
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PETTEST_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	UFUNCTION(Server, Unreliable, Category = "Shooting")
	void StartFire();
	
	void StopFire();
	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ARifleWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName WeaponSocketName = "WeaponSocket";
	
	UPROPERTY(Replicated)
	FTPWeapon ObservedWeapons;
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(Replicated)
	ARifleWeapon* Weapon;
	
	UFUNCTION(Server, Unreliable, Category = "Shooting")
	void SpawnWeapon();
};
