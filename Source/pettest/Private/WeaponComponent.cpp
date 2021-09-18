// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "PlayerCharacter.h"
#include "RifleWeapon.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All)

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	SpawnWeapon();
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogWeaponComponent, Display, TEXT("End play reason: %s"), *StaticEnum<EEndPlayReason::Type>()->GetNameStringByValue(EndPlayReason));
	if(Weapon)
	{
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Weapon->Destroy();
	}
	Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::SpawnWeapon() 
{
	if (!GetWorld() || !GetOwner() || !GetOwner()->HasAuthority()) return;

	const auto Player = Cast<APlayerCharacter>(GetOwner());
	if (!Player) { UE_LOG(LogWeaponComponent, Warning, TEXT("Cast was failed")); return; }

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Player;
	SpawnParams.Instigator = Player;
	Weapon = GetWorld()->SpawnActor<ARifleWeapon>(WeaponClass, SpawnParams);
	if (!Weapon) { UE_LOG(LogWeaponComponent, Warning, TEXT("Actor couldn't be spawned")); return; }

	Weapon->AttachToPlayer(WeaponSocketName);
	UE_LOG(LogWeaponComponent, Display, TEXT("Spawn: %s was successful"), *Weapon->GetName());
}

void UWeaponComponent::StartFire_Implementation()
{
	if (!Weapon) { UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon's pointer is null")); return; }
	
	UE_LOG(LogWeaponComponent, Display, TEXT("FIRED"));
	Weapon->StartFire();
}

void UWeaponComponent::StopFire()
{
	if (!Weapon) return;

	Weapon->StopFire();
}
