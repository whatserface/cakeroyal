// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/HealthComponent.h"
#include "Components/WeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerHUDWidget, All, All)

void UPlayerHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (GetOwningPlayerPawn())
	{
		if (HealthProgressBar)
		{
			const auto HealthComponent = GetOwningPlayerPawn()->FindComponentByClass<UHealthComponent>();
			if (HealthComponent)
			{
				HealthComponent->OnHealthChanged.AddUObject(this, &UPlayerHUDWidget::OnHealthChanged);
				MaxHealth = HealthComponent->GetMaxHealth();
			}
		}
		if (AmmoProgressBar)
		{
			const auto WeaponComponent = GetOwningPlayer()->FindComponentByClass<UWeaponComponent>();
			if (WeaponComponent)
			{
				UE_LOG(LogPlayerHUDWidget, Display, TEXT("delegate was set"));
				/*if (WeaponComponent->GetOnAmmoChangedDelegate())
				{
					WeaponComponent->GetOnAmmoChangedDelegate()->AddUObject(this, &UPlayerHUDWidget::OnAmmoChanged);
					UE_LOG(LogPlayerHUDWidget, Display, TEXT("delegate was set"));
				}
				else
				{
					UE_LOG(LogPlayerHUDWidget, Warning, TEXT("delegate ptr was null"));
				}*/
			}
			else
			{
				UE_LOG(LogPlayerHUDWidget, Warning, TEXT("delegate wasn't set"));
			}
		}
	}
}

void UPlayerHUDWidget::OnHealthChanged(float NewHealth)
{
	if(NewHealth >= 0.0f)
	{
		HealthProgressBar->SetPercent(NewHealth / MaxHealth);
	}
}

void UPlayerHUDWidget::OnAmmoChanged(int32 ClipAmmo, int32 CurrentAmmo)
{
	if(CurrentAmmo > ClipAmmo)
	{
		checkNoEntry();
	}
	if(ClipAmmo == 0)
	{
		UE_LOG(LogPlayerHUDWidget, Display, TEXT("needs reload"));
		//reload animation
	}
	else
	{
		const float AmmoDelta = ClipAmmo == CurrentAmmo ? 0.0f : ClipAmmo / (ClipAmmo - CurrentAmmo);
		UE_LOG(LogPlayerHUDWidget, Display, TEXT("Setting percent...: %f"), AmmoDelta);
		AmmoProgressBar->SetPercent(AmmoDelta);
	}
}
