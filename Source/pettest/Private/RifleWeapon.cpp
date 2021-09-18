// Fill out your copyright notice in the Description page of Project Settings.


#include "RifleWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "PlayerCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogRifleWeapon, All, All)

ARifleWeapon::ARifleWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetIsReplicated(true);
	check(SetRootComponent(WeaponMesh));
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

void ARifleWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void ARifleWeapon::StartFire_Implementation()
{
	check(GetWorld());
	check(HasAuthority());
	check(GetOwner());
	if (!GetWorld() || !HasAuthority() || !GetOwner()) { UE_LOG(LogRifleWeapon, Warning, TEXT("Something went nuts"));  return; }

	FVector TraceStart, TraceEnd;
	FRotator EyesRotation;
	GetOwner()->GetActorEyesViewPoint(TraceStart, EyesRotation);
	TraceEnd = TraceStart + EyesRotation.Vector() * ShootLength;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams))
	{
		TraceEnd = HitResult.ImpactPoint;
		if (!HitResult.GetActor()) return;

		const auto OwnerPawn = Cast<APawn>(GetOwner());
		if (!OwnerPawn) return;

		HitResult.GetActor()->TakeDamage(DamageAmount, FDamageEvent{}, OwnerPawn->GetController(), this);
		UE_LOG(LogRifleWeapon, Display, TEXT("Damage was applied to: %s"), *HitResult.GetActor()->GetName());
	}
	else
	{
		UE_LOG(LogRifleWeapon, Warning, TEXT("Trace didn't hit anyone"));
	}
	UKismetSystemLibrary::DrawDebugArrow(this, TraceStart, TraceEnd, 5.0f, FLinearColor::Red, 5.0f, 1.0f);
}

void ARifleWeapon::StopFire_Implementation()
{
	//
}

bool ARifleWeapon::AttachToPlayer_Validate(const FName& SocketName)
{
	return HasAuthority();
}

void ARifleWeapon::AttachToPlayer_Implementation(const FName& SocketName)
{
	const auto Player = Cast<APlayerCharacter>(GetOwner());
	if (!Player) { UE_LOG(LogRifleWeapon, Warning, TEXT("When attach the cast was failed")); return; }

	FAttachmentTransformRules TransformRules(EAttachmentRule::KeepRelative, false);
	AttachToComponent(Player->GetLocalMesh(), TransformRules, SocketName);
}
