// Fill out your copyright notice in the Description page of Project Settings.


#include "RifleWeapon.h"

// Sets default values
ARifleWeapon::ARifleWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ARifleWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}
void ARifleWeapon::StartFire_Implementation() 
{
	//
}

void ARifleWeapon::StopFire_Implementation()
{
	//
}
