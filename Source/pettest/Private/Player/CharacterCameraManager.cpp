// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CharacterCameraManager.h"
#include "Player/PlayerCharacter.h"
#include "Components/RespawnComponent.h"

ACharacterCameraManager::ACharacterCameraManager()
{
	ViewPitchMin = -87.0f;
	ViewPitchMax = 87.0f;
	bAlwaysApplyModifiers = true;
}

void ACharacterCameraManager::BeginPlay()
{
	Super::BeginPlay();
	UpdatePawn(nullptr);
	if (!PCOwner) return;
	const auto RespawnComponent = PCOwner->FindComponentByClass<URespawnComponent>();
	if (RespawnComponent) RespawnComponent->OnPawnRespawn.AddUObject(this, &ACharacterCameraManager::UpdatePawn);
}

void ACharacterCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

	if (!PCOwner || (MyPawn && MyPawn->IsPendingKill()))
	{
		return;
	}
	if (PCOwner && !MyPawn)
	{
		UpdatePawn(nullptr);
		return;
	}
	MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation());
}

void ACharacterCameraManager::UpdatePawn(APawn* NewPawn)
{
	if (NewPawn)
	{
		MyPawn = Cast<APlayerCharacter>(NewPawn);
		return;
	}
	MyPawn = PCOwner ? Cast<APlayerCharacter>(PCOwner->GetPawn()) : nullptr;
}
