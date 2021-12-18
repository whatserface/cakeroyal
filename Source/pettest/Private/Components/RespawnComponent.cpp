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
	DOREPLIFETIME_CONDITION(URespawnComponent, bIsRespawnInProgress, COND_OwnerOnly);
}

void URespawnComponent::Respawn(int32 RespawnTime)
{
	if (GetOwnerRole() != ROLE_Authority || !GetWorld()) return;

	RespawnCountDown = RespawnTime;
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &URespawnComponent::RespawnTimerUpdate, 1.0f, true);
	bIsRespawnInProgress = true;
}

void URespawnComponent::RespawnTimerUpdate()
{
	UE_LOG(LogTemp, Display, TEXT("update. RespawnCountDown: %i"), RespawnCountDown);
	if (GetOwnerRole() == ROLE_Authority && --RespawnCountDown == 0)
	{
		if (!GetWorld()) return;

		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
		const auto GameMode = GetWorld()->GetAuthGameMode<AMyGameModeBase>();
		if (!GameMode) return;

		if (GameMode->RespawnRequest(Cast<AController>(GetOwner())))
		{
			FTimerHandle TempHandle;
			GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &URespawnComponent::ProcessOnPawnRespawn, 0.2f, false);
			bIsRespawnInProgress = false;
		}
	}
}

int32 URespawnComponent::GetRespawnCountDown() const
{
	return RespawnCountDown;
}

void URespawnComponent::ProcessOnPawnRespawn_Implementation()
{
	const auto Controller = Cast<AController>(GetOwner());
	if (!Controller || !Controller->GetPawn()) return;

	OnPawnRespawn.Broadcast(Controller->GetPawn());
}
