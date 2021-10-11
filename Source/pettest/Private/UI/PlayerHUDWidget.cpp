// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/HealthComponent.h"
#include "Components/WeaponComponent.h"
#include "Player/CharacterController.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerHUDWidget, All, All)

void UPlayerHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (GetOwningPlayer())
	{
		const auto Controller = Cast<ACharacterController>(GetOwningPlayer());
		if (!Controller) return;

		Controller->OnClientNewPawn.AddUObject(this, &UPlayerHUDWidget::OnNewPawn);
		OnNewPawn(GetOwningPlayerPawn());
	}
}

void UPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
	UE_LOG(LogPlayerHUDWidget, Display, TEXT("On new pawn present"));
	if (!NewPawn) 
	{
		UE_LOG(LogPlayerHUDWidget, Warning, TEXT("pawn is nullptr"));
	}
	if (!NewPawn || !GetWorld() || !HealthProgressBar) return;
	UE_LOG(LogPlayerHUDWidget, Display, TEXT("We got here!"));
	const auto HealthComponent = NewPawn->FindComponentByClass<UHealthComponent>();
	if (HealthComponent)
	{
		if (!HealthComponent->OnHealthChanged.IsBoundToObject(this)) 
		{
			HealthComponent->OnHealthChanged.AddUObject(this, &UPlayerHUDWidget::OnHealthChanged);
			if (MaxHealth == 0.0f)
			{
				MaxHealth = HealthComponent->GetMaxHealth();
			}
			OnHealthChanged(MaxHealth);
			UE_LOG(LogPlayerHUDWidget, Display, TEXT("Health Changed"));
		}
		if (!HealthComponent->OnDeath.IsBoundToObject(this)) 
		{
			HealthComponent->OnDeath.AddUObject(this, &UPlayerHUDWidget::OnPawnDeath); 
		}
	}
	else
	{
		UE_LOG(LogPlayerHUDWidget, Warning, TEXT("Health component is nullptr"));
	}
}

void UPlayerHUDWidget::OnPawnDeath()
{
	//
}

bool UPlayerHUDWidget::GetAmmoPercent(float& AmmoPercent) const
{
	const auto WeaponComponent = GetOwningPlayerPawn() ? GetOwningPlayerPawn()->FindComponentByClass<UWeaponComponent>() : nullptr;
	if (!WeaponComponent) return false;

	return WeaponComponent->GetAmmoPercent(AmmoPercent);
}

void UPlayerHUDWidget::OnHealthChanged(float NewHealth)
{
	if(NewHealth >= 0.0f)
	{
		HealthProgressBar->SetPercent(NewHealth / MaxHealth);
		UE_LOG(LogPlayerHUDWidget, Display, TEXT("Percent set: %.2f"), NewHealth);
	}
}
