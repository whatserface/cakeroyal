// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterController.h"
#include "PlayerCharacter.h"
#include "MyGameModeBase.h"

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ACharacterController::StaticClass();
}
