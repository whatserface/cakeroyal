#pragma once
#include "Weapon/FirstPersonWeapon.h"
#include "MyCoreTypes.generated.h"

//weapon
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, int32, int32);

class AFirstPersonWeapon;

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = "0", ClampMax = "5000"))
	int32 Bullets = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AFirstPersonWeapon> FPPWeaponClass;
};

//health
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float);
DECLARE_MULTICAST_DELEGATE(FOnDeath);
