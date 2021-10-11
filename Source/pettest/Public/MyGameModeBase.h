// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

UCLASS()
class PETTEST_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMyGameModeBase();
	
	void Killed(AController* KillerController, AController* VictimController);
	void RespawnRequest(AController* Controller);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	int32 RespawnTime = 5.0f;

private:
	void StartRespawn(AController* Controller);
};
