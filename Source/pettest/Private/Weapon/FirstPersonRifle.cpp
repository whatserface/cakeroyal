// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/FirstPersonRifle.h"
#include "Components/WeaponComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void AFirstPersonRifle::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner()) return;

	const auto WeaponComponent = GetOwner()->FindComponentByClass<UWeaponComponent>();
	if (!WeaponComponent) return;

	WeaponComponent->OnTraceAppeared.BindUFunction(this, TEXT("PlayTraceFX"));
}

void AFirstPersonRifle::PlayTraceFX_Implementation(FVector TraceEnd)
{
	const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, WeaponMesh->GetSocketLocation(MuzzleSocketName));
	if (TraceFXComponent) {
		TraceFXComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("TraceFXComponent is nullptr!"));
	}
}