// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RifleWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WeaponComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


DEFINE_LOG_CATEGORY_STATIC(LogRifleWeapon, All, All)

ARifleWeapon::ARifleWeapon() : Super() 
{
	Tags.Add("Player");
}

void ARifleWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	MyCharacter = Cast<ACharacter>(GetOwner());
}

void ARifleWeapon::StartFire()
{
	if (!HasAuthority()) return;

	GetWorldTimerManager().SetTimer(ShootingTimer, this, &ARifleWeapon::MakeShot, WeaponInfo.ShootingRate, true);
	MakeShot();
	InitFX();
}

void ARifleWeapon::StopFire()
{
	if (!GetOwner() || !HasAuthority()) return;
	
	bHasPassedFireRateDelay = false;
	GetWorldTimerManager().ClearTimer(ShootingTimer);
	SetFXActive(false);
	GetWorldTimerManager().SetTimer(FireRateControllerTimer, this, &ARifleWeapon::ControlFireRateDelay, WeaponInfo.ShootingRate, false);
	/*const auto MyCharacter = Cast<APawn>(GetOwner());
	check(MyCharacter);
	Client_Recoil(MyCharacter, false);*/
}

void ARifleWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//
}

void ARifleWeapon::MakeShot()
{
	UE_LOG(LogTemp, Display, TEXT("Delay has %sbeen passed"), bHasPassedFireRateDelay ? TEXT(" ") : TEXT("not "));
	if (!CanShoot() && !WeaponComponent->CanShoot() && HasAuthority())
	{
		StopFire();
		return;
	} 

	if (!MyCharacter || !MyCharacter->GetCharacterMovement()) return;
	FVector TraceStart, TraceEnd;
	FRotator EyesRotation;
	MyCharacter->GetActorEyesViewPoint(TraceStart, EyesRotation);
	const float HalfRad = FMath::DegreesToRadians(MyCharacter->GetCharacterMovement()->IsFalling() ? BulletSpreadAloft : BulletSpreadDefault);
	const FVector ShootDirection = FMath::VRandCone(EyesRotation.Vector(), HalfRad);
	TraceEnd = TraceStart + ShootDirection * ShootLength;
	FCollisionQueryParams CollisionParams;
	TArray<const AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MyCharacter);
	ActorsToIgnore.Add(this);
	CollisionParams.AddIgnoredActors(ActorsToIgnore);
	CollisionParams.bReturnPhysicalMaterial = true;
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams))
	{
		TraceEnd = HitResult.ImpactPoint;
		if (const auto HitActor = HitResult.GetActor()) 
		{
			HitActor->TakeDamage(Damage, FDamageEvent{}, MyCharacter->GetController(), this);
			UE_LOG(LogRifleWeapon, Display, TEXT("Damage was applied to: %s"), *HitActor->GetName());
		}
	}
	ReduceAmmo();
	PlayImpactFX(HitResult);
	SpawnTraceFX(WeaponMesh->GetSocketLocation(MuzzleSocketName), TraceEnd);
	OnTraceAppeared.Execute(TraceEnd);
	//UKismetSystemLibrary::DrawDebugArrow(this, TraceStart, TraceEnd, 5.0f, FLinearColor::Red, 5.0f, 1.0f);
	Client_Recoil(MyCharacter, true);
}

void ARifleWeapon::ControlFireRateDelay()
{
	if (HasAuthority()) bHasPassedFireRateDelay = true;
}

bool ARifleWeapon::CanShoot()
{
	return Super::CanShoot() && bHasPassedFireRateDelay;
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
	if (IsRunningDedicatedServer() || !GetOwner() || UGameplayStatics::GetPlayerController(this, 1) == Hit.GetActor()) return;

	FImpactData ImpactData = DefaultImpactData;
	if (Hit.PhysMaterial.IsValid())
	{
		const auto PhysMat = Hit.PhysMaterial.Get();
		if (ImpactDataMap.Contains(PhysMat))
		{
			ImpactData = ImpactDataMap[PhysMat];
		}
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactData.NiagaraEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	UGameplayStatics::PlaySoundAtLocation(this, ImpactData.ImpactSound, Hit.ImpactPoint);
	UE_LOG(LogTemp, Display, TEXT("Playing sound"));
}

void ARifleWeapon::SpawnTraceFX_Implementation(const FVector& TraceStart, const FVector& TraceEnd)
{
	if (IsRunningDedicatedServer() || (GetInstigatorController() && GetInstigatorController()->IsLocalPlayerController())) return;

	const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, TraceStart);
	if (TraceFXComponent) {
		TraceFXComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd);
	}
}

void ARifleWeapon::InitFX_Implementation()
{
	if (IsRunningDedicatedServer()) return;

	if (!FireAudioComponent)
	{
		FireAudioComponent = UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, MuzzleSocketName, WeaponMesh->GetSocketLocation(MuzzleSocketName), EAttachLocation::SnapToTarget, false, 1.0f, 1.0f, 0,
																  nullptr, nullptr, false);
		UE_LOG(LogTemp, Display, TEXT("Spawning fire comp"));
	}
	SetFXActive(true);
}

void ARifleWeapon::SetFXActive_Implementation(bool IsActive)
{
	if (IsRunningDedicatedServer()) return;
	
	if (FireAudioComponent)
	{
		IsActive ? FireAudioComponent->Play() : FireAudioComponent->Stop();
	}
}
