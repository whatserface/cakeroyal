// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "PlayerCharacter.h"
#include "RifleWeapon.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
}

void UWeaponComponent::SpawnWeapon() 
{
	if (!GetWorld() || !GetOwner()) return;

	const auto Player = Cast<APlayerCharacter>(GetOwner());
	if (!Player) return;

	Weapon = GetWorld()->SpawnActor<ARifleWeapon>(ARifleWeapon::StaticClass());
	if (!Weapon) return;

	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(Player->GetMesh(), TransformRules, WeaponSocketName);
	Weapon->SetOwner(Player);
}

void UWeaponComponent::StartFire_Implementation()
{
	if (!Weapon) return;

	Weapon->StartFire();
}

void UWeaponComponent::StopFire_Implementation()
{
	if (!Weapon) return;

	Weapon->StopFire();
}
