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

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	bool GetAmmoPercent(float& AmmoPercent) const;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* AmmoProgressBar;

	virtual void NativeOnInitialized() override;

private:
	void OnNewPawn(APawn* NewPawn);
	void OnHealthChanged(float NewHealth);
	void OnPawnDeath();
	//void OnAmmoChanged(); по-любому понадобиться, когда придётся менять видимость виджету/играть анимацию какую-нибудь

	float CurrentHealth;
	float MaxHealth;
};
