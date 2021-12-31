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

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
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


	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void ShrinkHealthBar(float NewHealth, float LastHealth);

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

private:
	FTimerHandle WeaponTimer;

	float MaxArmor;
	float MaxHealth;
	int32 MaxAmmo = -1;
	
	bool bShouldInterpolateHealth = false;
	float InterpolateHealthFrom;
	float InterpolateHealthTo;

	bool bShouldInterpolateArmor = false;
	float InterpolateArmorFrom;
	float InterpolateArmorTo;

	float LastHP;
	float LastArmor;
	bool bHasJustSpawned = true;

	UFUNCTION()
	void WriteFromWeaponComponent(APawn* Pawn);
	
	UFUNCTION()
	void OnAmmoChanged(int32 NewAmmo);

	void ManageInterpolations(float DeltaTime);
	void OnPawnDeath();
	void OnNewPawn(APawn* NewPawn);
	void OnHealthChanged(float NewHealth);
	void OnArmorChanged(float NewArmor);
};
