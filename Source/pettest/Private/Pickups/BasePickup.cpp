// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/BasePickup.h"

ABasePickup::ABasePickup()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABasePickup::BeginPlay()
{
	Super::BeginPlay();
	
}
