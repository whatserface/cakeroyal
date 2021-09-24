// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonWeapon.h"
#include "Net/UnrealNetwork.h"
#include "PlayerCharacter.h"

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
	
}

void AThirdPersonWeapon::StartFire()
{
	//Third Person is abstract class, hence we don't need anything here
}
