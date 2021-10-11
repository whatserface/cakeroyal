// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RespawnComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PETTEST_API URespawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URespawnComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Respawn(int32 RespawnTime);
private:
	FTimerHandle RespawnTimerHandle;

	UPROPERTY(Replicated)
	int32 RespawnCountDown = 0;

	void RespawnTimerUpdate();
};
