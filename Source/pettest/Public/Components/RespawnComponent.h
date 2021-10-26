// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCoreTypes.h"
#include "Components/ActorComponent.h"
#include "RespawnComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PETTEST_API URespawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URespawnComponent();


	FOnPawnRespawn OnPawnRespawn;

	void Respawn(int32 RespawnTime);

	int32 GetRespawnCountDown() const { return RespawnCountDown; }
	bool IsRespawnInProgress() const { return bIsRespawnInProgress; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FTimerHandle RespawnTimerHandle;

	UPROPERTY(Replicated)
	int32 RespawnCountDown = 0;

	UPROPERTY(Replicated)
	bool bIsRespawnInProgress = false;

	UFUNCTION(Client, Unreliable)
	void ProcessOnPawnRespawn();

	void RespawnTimerUpdate();
};
