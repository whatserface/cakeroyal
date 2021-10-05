// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameModeBase.h"
#include "Player/CharacterController.h"
#include "Player/PlayerCharacter.h"
#include "UI/GameHUD.h"

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ACharacterController::StaticClass();
	HUDClass = AGameHUD::StaticClass();
}
