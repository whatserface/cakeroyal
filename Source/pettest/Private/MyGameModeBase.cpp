// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameModeBase.h"
#include "Player/CharacterController.h"
#include "Player/PlayerCharacter.h"
#include "Player/CharacterController.h"
#include "UI/GameHUD.h"
#include "Components/RespawnComponent.h"
#include "Components/WeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogMyGameModeBase, All, All)

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ACharacterController::StaticClass();
	HUDClass = AGameHUD::StaticClass();
}

void AMyGameModeBase::StartRespawn(AController* Controller)
{
	const auto Component = Controller->GetComponentByClass(URespawnComponent::StaticClass());
	const auto RespawnComponent = Cast<URespawnComponent>(Component);
	if (!RespawnComponent) { UE_LOG(LogTemp, Display, TEXT("Starts Respawning")); return; }

	RespawnComponent->Respawn(RespawnTime);
}

void AMyGameModeBase::Killed(AController* KillerController, AController* VictimController)
{
	if (!VictimController)
	{
		UE_LOG(LogMyGameModeBase, Warning, TEXT("Victim controller was nullptr"));
		return;
	}
	StartRespawn(VictimController);
}

bool AMyGameModeBase::RespawnRequest(AController* Controller)
{
	if (!Controller) return false;

	if (APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, FindPlayerStart(Controller)->GetTransform()))
	{
		Controller->Possess(Pawn);
		return true;
	}

	UE_LOG(LogMyGameModeBase, Warning, TEXT("Pawn couldn't be spawn while respawning"));
	return false;
}
