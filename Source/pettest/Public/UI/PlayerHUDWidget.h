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

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	bool GetAmmoPercent(float& AmmoPercent) const;

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
	void OnNewPawn(APawn* NewPawn);
	void OnHealthChanged(float NewHealth);
	void OnArmorChanged(float NewArmor);
	void OnPawnDeath();
	void OnReload();

	float MaxArmor;
	float MaxHealth;
	
	float LastHP;
	bool bHasJustSpawned = true;
};
