// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/FirstPersonWeapon.h"
#include "Components/SkeletalMeshComponent.h"

AFirstPersonWeapon::AFirstPersonWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->bOnlyOwnerSee = true;
	WeaponMesh->SetCastShadow(false);
	SetRootComponent(WeaponMesh);

	bReplicates = false;
}

void AFirstPersonWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}
