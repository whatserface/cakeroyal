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
	WeaponMesh->SetCastShadow(true);
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
}

void AThirdPersonWeapon::StartFire()
{
	//
}

void AThirdPersonWeapon::StopFire()
{
	//Third Person is abstract class, hence we don't need anything here
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
		OnReload.Execute();
		UE_LOG(LogTemp, Display, TEXT("On reload broadcasting"));
	}
}

bool AThirdPersonWeapon::CanShoot()
{
	if (!HasAuthority() || !GetOwner() || !GetWorld() || IsAmmoEmpty() || !GetOwner()) return false;

	const auto WeaponComponent = GetOwner()->FindComponentByClass<UWeaponComponent>();
	return WeaponComponent && WeaponComponent->CanShoot();
}

void AThirdPersonWeapon::LogAmmo()
{
	if (HasAuthority()) UE_LOG(LogTPPWeapon, Display, TEXT("Bullets: %i"), Bullets);
}

void AThirdPersonWeapon::Reload()
{
	if (HasAuthority()) 
	{
		Bullets = WeaponInfo.Bullets;
		UE_LOG(LogTemp, Display, TEXT("Reloaded bullets on server!!"));
	}
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
