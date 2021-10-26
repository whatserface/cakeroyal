// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RifleWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRifleWeapon, All, All)

ARifleWeapon::ARifleWeapon() : Super() {}

void ARifleWeapon::StartFire()
{
	if (!HasAuthority()) return;

	GetWorldTimerManager().SetTimer(ShootingTimer, this, &ARifleWeapon::MakeShot, ShootingRate, true);
	MakeShot();
}

void ARifleWeapon::StopFire()
{
	if (!GetOwner() || !HasAuthority()) return;
	
	GetWorldTimerManager().ClearTimer(ShootingTimer);
	/*const auto OwnerPawn = Cast<APawn>(GetOwner());
	check(OwnerPawn);
	Client_Recoil(OwnerPawn, false);*/
}

void ARifleWeapon::MakeShot()
{
	if (!CanShoot())
	{
		StopFire();
		return;
	} 

	const auto OwnerPawn = Cast<ACharacter>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->GetCharacterMovement()) return;
	FVector TraceStart, TraceEnd;
	FRotator EyesRotation;
	OwnerPawn->GetActorEyesViewPoint(TraceStart, EyesRotation);
	const float HalfRad = OwnerPawn->GetCharacterMovement()->IsFalling() ? FMath::DegreesToRadians(BulletSpreadAloft) : FMath::DegreesToRadians(BulletSpreadDefault);
	const FVector ShootDirection = FMath::VRandCone(EyesRotation.Vector(), HalfRad);
	TraceEnd = TraceStart + ShootDirection * ShootLength;
	FCollisionQueryParams CollisionParams;
	TArray<const AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerPawn);
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
