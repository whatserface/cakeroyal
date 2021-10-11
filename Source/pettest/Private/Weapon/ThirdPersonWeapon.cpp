// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ThirdPersonWeapon.h"
#include "Components/WeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogTPPWeapon, All, All)

AThirdPersonWeapon::AThirdPersonWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->bOwnerNoSee = true;
	SetRootComponent(WeaponMesh);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

void AThirdPersonWeapon::BeginPlay()
{
	Super::BeginPlay();

	check(WeaponMesh);
	checkf(WeaponInfo.Bullets > 0, TEXT("Bullets count couldn't be less or equal zero"));

	Bullets = WeaponInfo.Bullets;
}

void AThirdPersonWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AThirdPersonWeapon, Bullets, COND_OwnerOnly);
	DOREPLIFETIME(AThirdPersonWeapon, MyPawn);
}

void AThirdPersonWeapon::StartFire()
{
	//
}

void AThirdPersonWeapon::StopFire()
{
	//Third Person is abstract class, hence we don't need anything here
}

void AThirdPersonWeapon::MakeShot()
{
	//
}

void AThirdPersonWeapon::ReduceAmmo()
{
	if (!HasAuthority()) return;

	if (Bullets == 0)
	{
		UE_LOG(LogTPPWeapon, Warning, TEXT("Clip is empty"));
		return;
	}
	Bullets--;
	if (Bullets == 0)
	{
		StopFire();
		Reload();
	}
}

bool AThirdPersonWeapon::CanShoot()
{
	if (!GetWorld() || IsAmmoEmpty())
	{
		StopFire();
		return false;
	}
	return true;
}

void AThirdPersonWeapon::LogAmmo()
{
	if (!HasAuthority()) return;

	UE_LOG(LogTPPWeapon, Display, TEXT("Bullets: %i"), Bullets);
}

void AThirdPersonWeapon::Reload_Implementation()
{
	Bullets = WeaponInfo.Bullets;
}

void AThirdPersonWeapon::GetWeaponBullets(float& AmmoPercent) const
{
	if (Bullets != 0)
	{
		AmmoPercent = (float)(WeaponInfo.Bullets - Bullets) / (float)(WeaponInfo.Bullets);
	}
	else
	{
		AmmoPercent = 1.0f;
	}
}
