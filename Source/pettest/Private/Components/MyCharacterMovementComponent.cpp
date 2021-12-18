// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MyCharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerCharacter.h"

UMyCharacterMovementComponent::UMyCharacterMovementComponent(const FObjectInitializer& ObjInit) : Super(ObjInit) {}

void UMyCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningPlayer = Cast<APlayerCharacter>(GetOwner());
}

float UMyCharacterMovementComponent::GetMaxSpeed() const
{
	const float MaxSpeed = Super::GetMaxSpeed();
	return OwningPlayer && OwningPlayer->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}

void UMyCharacterMovementComponent::SetRunModifier(float NewModifier)
{
	if (GetOwnerRole() == ROLE_Authority) RunModifier = NewModifier;
}

void UMyCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMyCharacterMovementComponent, OwningPlayer);
	DOREPLIFETIME(UMyCharacterMovementComponent, RunModifier);
}

