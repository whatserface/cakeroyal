// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirstPersonWeapon.h"
#include "FirstPersonRifle.generated.h"

class UNiagaraSystem;

UCLASS()
class PETTEST_API AFirstPersonRifle : public AFirstPersonWeapon
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Unreliable, Category = "VFX")
	void PlayTraceFX(FVector TraceEnd);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* TraceFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	FString TraceTargetName = "LaserEnd";

	virtual void BeginPlay() override;
};
