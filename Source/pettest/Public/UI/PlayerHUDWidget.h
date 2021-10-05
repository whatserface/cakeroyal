// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UProgressBar;

UCLASS()
class PETTEST_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* AmmoProgressBar;

	virtual void NativeOnInitialized() override;

private:
	void OnHealthChanged(float NewHealth);
	void OnAmmoChanged(int32 ClipAmmo, int32 CurrentAmmo);

	float CurrentHealth;
	float MaxHealth;
};
