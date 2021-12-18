// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ThirdPersonWeapon.h"
#include "Components/WeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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

	if (!GetOwner()) return;
	
	WeaponComponent = GetOwner()->FindComponentByClass<UWeaponComponent>();
}

void AThirdPersonWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(AThirdPersonWeapon, WeaponComponent, COND_InitialOnly);
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
	if (!HasAuthority() || !WeaponComponent) return;

	if (Bullets == 0)
	{
		UE_LOG(LogTPPWeapon, Warning, TEXT("Clip is empty"));
		return;
	}
	--Bullets;
	ForceNetUpdate();
	Client_InvokeAmmoChanged(Bullets);
	if (Bullets == 0)
	{
		StopFire();
	}
}

bool AThirdPersonWeapon::CanShoot()
{
	if (!HasAuthority() || !GetOwner() || !GetWorld() || IsAmmoEmpty() || !GetOwner()) return false;

	return WeaponComponent && WeaponComponent->CanShoot();
}

void AThirdPersonWeapon::LogAmmo()
{
	if (HasAuthority()) UE_LOG(LogTPPWeapon, Display, TEXT("Bullets: %i"), Bullets);
}

bool AThirdPersonWeapon::Client_InvokeAmmoChanged_Validate(int32 Ammo)
{
	return 0 <= Ammo && Ammo <= WeaponInfo.Bullets;
}

void AThirdPersonWeapon::Client_InvokeAmmoChanged_Implementation(int32 Ammo)
{
	OnAmmoChanged.Execute(Ammo);
}

void AThirdPersonWeapon::Reload()
{
	if (HasAuthority()) Bullets = WeaponInfo.Bullets;
}
