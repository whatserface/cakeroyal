// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class AThirdPersonWeapon;
class AFirstPersonWeapon;

USTRUCT(BlueprintType)
struct FTPWeapon
{
	GENERATED_USTRUCT_BODY()

	AFirstPersonWeapon* FPPWeapon;
	AThirdPersonWeapon* TPPWeapon;
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
	TSubclassOf<AThirdPersonWeapon> TPPWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName WeaponSocketName = "WeaponSocket";
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(Replicated)
	AThirdPersonWeapon* TPPWeapon = nullptr;
	
	AFirstPersonWeapon* FPPWeapon = nullptr;

	UFUNCTION(Server, Unreliable, Category = "Shooting")
	void SpawnTPPWeapon();
	
	UFUNCTION(Client, Unreliable, Category = "Shooting")
	void SpawnFPPWeapon();
};
