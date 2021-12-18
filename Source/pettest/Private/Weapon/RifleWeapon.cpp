// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RifleWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"

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
	SetFXActive(false);
	/*const auto OwnerPawn = Cast<APawn>(GetOwner());
	check(OwnerPawn);
	Client_Recoil(OwnerPawn, false);*/
}

void ARifleWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//
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
	CollisionParams.bReturnPhysicalMaterial = true;
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
	PlayImpactFX(HitResult);
	SpawnTraceFX(WeaponMesh->GetSocketLocation(MuzzleSocketName), TraceEnd);
	OnTraceAppeared.Execute(TraceEnd);
	//UKismetSystemLibrary::DrawDebugArrow(this, TraceStart, TraceEnd, 5.0f, FLinearColor::Red, 5.0f, 1.0f);
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

void ARifleWeapon::PlayImpactFX_Implementation(const FHitResult& Hit)
{
	if (IsRunningDedicatedServer()) return;

	UNiagaraSystem* NiagaraEffect = ImpactEffect;
	if (Hit.PhysMaterial.IsValid())
	{
		const auto PhysMat = Hit.PhysMaterial.Get();
		if (ImpactDataMap.Contains(PhysMat))
		{
			NiagaraEffect = ImpactDataMap[PhysMat];
		}
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, NiagaraEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
}

void ARifleWeapon::SpawnTraceFX_Implementation(const FVector& TraceStart, const FVector& TraceEnd)
{
	if (IsRunningDedicatedServer() || (GetInstigatorController() && GetInstigatorController()->IsLocalPlayerController())) return;

	const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, TraceStart);
	if (TraceFXComponent) {
		TraceFXComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd);
	}
}

void ARifleWeapon::SetFXActive(bool IsActive)
{
	if (!HasAuthority()) return;
	
	// Повторяющиеся звуки выстрелов
}
