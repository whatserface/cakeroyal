// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CharacterCameraManager.h"
#include "Player/PlayerCharacter.h"

ACharacterCameraManager::ACharacterCameraManager()
{
	ViewPitchMin = -87.0f;
	ViewPitchMax = 87.0f;
	bAlwaysApplyModifiers = true;
}

void ACharacterCameraManager::BeginPlay()
{
	Super::BeginPlay();
	MyPawn = PCOwner ? Cast<APlayerCharacter>(PCOwner->GetPawn()) : nullptr;
}

void ACharacterCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);
	if (!PCOwner || !MyPawn || MyPawn->IsPendingKill()) return;

	if (PCOwner && !MyPawn)
	{
		MyPawn = PCOwner ? Cast<APlayerCharacter>(PCOwner->GetPawn()) : nullptr;
		return;
	}
	
	MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation());
}