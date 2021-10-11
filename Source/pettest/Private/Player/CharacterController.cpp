// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CharacterController.h"
#include "Components/RespawnComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/PlayerState.h"

ACharacterController::ACharacterController()
{
	RespawnComponent = CreateDefaultSubobject<URespawnComponent>("RespawnComponent");
	OnNewPawn.AddUObject(this, &ACharacterController::FireDelegate);
}

void ACharacterController::FireDelegate_Implementation(APawn* NewPawn)
{
	OnClientNewPawn.Broadcast(NewPawn);
}

void ACharacterController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	
	if (GetStateName() == NAME_Spectating)
	{
		AutoManageActiveCameraTarget(GetSpectatorPawn());
	}
}


void ACharacterController::StartSpectating()
{
	if (!HasAuthority()) return;

	PlayerState->SetIsSpectator(true);
	ChangeState(NAME_Spectating);
	Client_StartSpectating();
}

void ACharacterController::StartPlaying()
{
	if (!HasAuthority()) return;

	PlayerState->SetIsSpectator(false);
	ChangeState(NAME_Playing);
	ClientGotoState(NAME_Playing);
}

void ACharacterController::Client_StartSpectating_Implementation()
{
	if (PlayerCameraManager)
		SetSpawnLocation(PlayerCameraManager->GetCameraLocation());

	ChangeState(NAME_Spectating);
}
