// Fill out your copyright notice in the Description page of Project Settings.


#include "RifleWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "PlayerCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogRifleWeapon, All, All)

ARifleWeapon::ARifleWeapon() : Super() {}

void ARifleWeapon::StartFire_Implementation()
{
	check(GetWorld());
	check(HasAuthority());
	check(GetOwner());
	if (!GetWorld() || !HasAuthority() || !GetOwner()) { UE_LOG(LogRifleWeapon, Warning, TEXT("Something went nuts"));  return; }

	const auto OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	FVector TraceStart, TraceEnd;
	FRotator EyesRotation;
	OwnerPawn->GetActorEyesViewPoint(TraceStart, EyesRotation);
	TraceEnd = TraceStart + EyesRotation.Vector() * WeaponInfo.ShootLength;

	FCollisionQueryParams CollisionParams;
	TArray<const AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());
	CollisionParams.AddIgnoredActors(ActorsToIgnore);
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams))
	{
		TraceEnd = HitResult.ImpactPoint;
		if (!HitResult.GetActor()) return;

		HitResult.GetActor()->TakeDamage(WeaponInfo.Damage, FDamageEvent{}, OwnerPawn->GetController(), this);
		UE_LOG(LogRifleWeapon, Display, TEXT("Damage was applied to: %s"), *HitResult.GetActor()->GetName());
	}
	else
	{
		UE_LOG(LogRifleWeapon, Warning, TEXT("Trace didn't hit anyone"));
	}
	UKismetSystemLibrary::DrawDebugArrow(this, TraceStart, TraceEnd, 5.0f, FLinearColor::Red, 5.0f, 1.0f);
	Client_Recoil(OwnerPawn);
}

void ARifleWeapon::Client_Recoil_Implementation(APawn* ApplyTo)
{
	Recoil(ApplyTo);
}
