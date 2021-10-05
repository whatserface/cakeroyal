// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RifleWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Player/PlayerCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogRifleWeapon, All, All)

ARifleWeapon::ARifleWeapon() : Super() {}

void ARifleWeapon::StartFire_Implementation()
{
	GetWorldTimerManager().SetTimer(ShootingTimer, this, &ARifleWeapon::MakeShot, ShootingRate, true);
	MakeShot();
}

void ARifleWeapon::StopFire_Implementation()
{
	if (!GetOwner()) return;
	
	GetWorldTimerManager().ClearTimer(ShootingTimer);
	const auto OwnerPawn = Cast<APawn>(GetOwner());
	check(OwnerPawn);
	Client_Recoil(OwnerPawn, false);
}

void ARifleWeapon::MakeShot()
{
	if (!HasAuthority() || !GetOwner() || !GetWorld() || IsAmmoEmpty())
	{
		StopFire();
		return;
	}

	const auto OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	FVector TraceStart, TraceEnd;
	FRotator EyesRotation;
	OwnerPawn->GetActorEyesViewPoint(TraceStart, EyesRotation);
	TraceEnd = TraceStart + EyesRotation.Vector() * ShootLength;
	FCollisionQueryParams CollisionParams;
	TArray<const AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	ActorsToIgnore.Add(this);
	CollisionParams.AddIgnoredActors(ActorsToIgnore);
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams))
	{
		TraceEnd = HitResult.ImpactPoint;
		if (const auto HitActor = HitResult.GetActor()) 
		{
			HitActor->TakeDamage(Damage, FDamageEvent{}, OwnerPawn->GetController(), this);
			UE_LOG(LogRifleWeapon, Display, TEXT("Damage was applied to: %s"), *HitActor->GetName());
		}
	}
	else
	{
		UE_LOG(LogRifleWeapon, Warning, TEXT("Trace didn't hit anyone"));
	}
	ReduceAmmo();
	LogAmmo();
	UKismetSystemLibrary::DrawDebugArrow(this, TraceStart, TraceEnd, 5.0f, FLinearColor::Red, 5.0f, 1.0f);
	Client_Recoil(OwnerPawn, true);
}

void ARifleWeapon::Client_Recoil_Implementation(APawn* ApplyTo, bool IsGoingUp)
{
	if (IsGoingUp) 
	{
		Recoil(ApplyTo);
	}
	else
	{
		Redo_Recoil(ApplyTo);
	}
}
