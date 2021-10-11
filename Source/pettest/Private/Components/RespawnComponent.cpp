// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RespawnComponent.h"
#include "MyGameModeBase.h"
#include "Net/UnrealNetwork.h"

URespawnComponent::URespawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URespawnComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URespawnComponent, RespawnCountDown);
}

void URespawnComponent::Respawn_Implementation(int32 RespawnTime)
{
	if (!GetWorld()) return;

	RespawnCountDown = RespawnTime;
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &URespawnComponent::RespawnTimerUpdate, 1.0f, true);
}

void URespawnComponent::RespawnTimerUpdate()
{
	UE_LOG(LogTemp, Display, TEXT("updaTE. RespawnCountDown: %i"), RespawnCountDown);
	if(GetOwnerRole() == ROLE_Authority && --RespawnCountDown == 0)
	{
		if (!GetWorld()) return;

		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
		const auto GameMode = GetWorld()->GetAuthGameMode<AMyGameModeBase>();
		if (!GameMode) return;

		GameMode->RespawnRequest(Cast<AController>(GetOwner()));
	}
}
