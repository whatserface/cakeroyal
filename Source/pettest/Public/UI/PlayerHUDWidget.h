// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UProgressBar;
class UCanvasPanel;
class UDeathWidget;

UCLASS()
class PETTEST_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MainHUD;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ArmorProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* AmmoProgressBar;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ReloadAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ArmorPickupAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HealthPickupAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HealthReduceAnimation;

	UPROPERTY(meta = (BindWidget))
	UDeathWidget* DeathWidget;

	virtual void NativeOnInitialized() override;

private:
	FTimerHandle WeaponTimer;

	float MaxArmor;
	float MaxHealth;
	int32 MaxAmmo = -1;
	
	float LastHP;
	bool bHasJustSpawned = true;

	UFUNCTION()
	void WriteFromWeaponComponent(APawn* Pawn);
	
	UFUNCTION()
	void OnAmmoChanged(int32 NewAmmo);

	void OnPawnDeath();
	void OnNewPawn(APawn* NewPawn);
	void OnHealthChanged(float NewHealth);
	void OnArmorChanged(float NewArmor);
};
