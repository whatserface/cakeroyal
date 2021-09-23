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

void AThirdPersonWeapon::SpawnWeaponFPP_Implementation()
{
	if (!GetWorld() || !GetOwner()) return;

	const auto Player = Cast<APlayerCharacter>(GetOwner());

	if (!Player)
	{
		UE_LOG(LogTPPWeapon, Warning, TEXT("When spawning fpp weapon cast was failed"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Player;
	SpawnParams.Instigator = Player;
	const auto FWeapon = GetWorld()->SpawnActor<AFirstPersonWeapon>(WeaponInfo.FPPWeaponClass, SpawnParams);
	if (!FWeapon) { UE_LOG(LogTPPWeapon, Warning, TEXT("FPP Weapon Actor couldn't be spawned")); return; }
	Player->SetFPPWeapon(FWeapon);
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, false);
	FWeapon->AttachToComponent(Player->GetMesh(), TransformRules, SocketAttachName);
}
