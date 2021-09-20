// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameModeBase.h"
#include "CharacterController.h"
#include "PlayerCharacter.h"

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ACharacterController::StaticClass();
}
