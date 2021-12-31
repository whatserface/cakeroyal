// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/HealthComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/RespawnComponent.h"
#include "Player/CharacterController.h"
#include "UI/DeathWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerHUDWidget, All, All)

void UPlayerHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!GetOwningPlayer()) return;
	bHasJustSpawned = true;
	const auto RespawnComponent = GetOwningPlayer()->FindComponentByClass<URespawnComponent>();
	if (!RespawnComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Respawn Component was nullptr"));
		return;
	}
	RespawnComponent->OnPawnRespawn.AddUObject(this, &UPlayerHUDWidget::OnNewPawn);
	OnNewPawn(GetOwningPlayerPawn());
}

void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	ManageInterpolations(DeltaTime);
}

void UPlayerHUDWidget::ManageInterpolations(float DeltaTime)
{
	if (bShouldInterpolateHealth && HealthProgressBar) {
		InterpolateHealthFrom = FMath::FInterpTo(InterpolateHealthFrom, InterpolateHealthTo, DeltaTime, 10.0f);
		HealthProgressBar->SetPercent(InterpolateHealthFrom / MaxHealth);

		if (FMath::IsNearlyEqual(InterpolateHealthFrom, InterpolateHealthTo, 0.01f))
		{
			HealthProgressBar->SetPercent(InterpolateHealthTo / MaxHealth);
			bShouldInterpolateHealth = false;
		}
	}
	if (bShouldInterpolateArmor && ArmorProgressBar) {
		InterpolateArmorFrom = FMath::FInterpTo(InterpolateArmorFrom, InterpolateArmorTo, DeltaTime, 5.f);
		UE_LOG(LogTemp, Display, TEXT("Interpolating Armor: %.2f"), InterpolateArmorFrom);
		ArmorProgressBar->SetPercent(InterpolateArmorFrom / MaxArmor);

		if (FMath::IsNearlyEqual(InterpolateArmorFrom, InterpolateArmorTo, 0.01f)) 
		{
			ArmorProgressBar->SetPercent(InterpolateArmorTo / MaxArmor);
			bShouldInterpolateArmor = false;
		}
	}
}

void UPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
	if (!NewPawn || !GetWorld()) return;
	MainHUD->SetVisibility(ESlateVisibility::Visible);
	DeathWidget->SetVisibility(ESlateVisibility::Collapsed);
	const auto HealthComponent = NewPawn->FindComponentByClass<UHealthComponent>();
	const auto WeaponComponent = NewPawn->FindComponentByClass<UWeaponComponent>();
	if (!HealthComponent || !WeaponComponent)
	{
		UE_LOG(LogPlayerHUDWidget, Warning, TEXT("One of components is nullptr"));
		return;
	}
	if (!HealthComponent->OnHealthChanged.IsBoundToObject(this))
	{
		HealthComponent->OnHealthChanged.AddUObject(this, &UPlayerHUDWidget::OnHealthChanged);
		HealthComponent->OnArmorChanged.AddUObject(this, &UPlayerHUDWidget::OnArmorChanged);
		if (MaxHealth == 0.0f) {
			MaxHealth = HealthComponent->GetMaxHealth();
		}
		if (MaxArmor == 0.0f) {
			MaxArmor = HealthComponent->GetMaxArmor();
		}
		OnHealthChanged(MaxHealth);
		HealthProgressBar->SetPercent(1.0f);
		UE_LOG(LogPlayerHUDWidget, Display, TEXT("Health Changed"));
	}
	if (!HealthComponent->OnDeath.IsBoundToObject(this))
	{
		HealthComponent->OnDeath.AddUObject(this, &UPlayerHUDWidget::OnPawnDeath);
	}
	if (!WeaponComponent->OnAmmoChanged.IsBoundToObject(this))
	{
		WeaponComponent->OnAmmoChanged.BindUObject(this, &UPlayerHUDWidget::OnAmmoChanged);
		UE_LOG(LogTemp, Display, TEXT("Binded to ammo changed delegate, running %s"), NewPawn->IsLocallyControlled() ? TEXT("locally") : TEXT("remotely"));
		if (MaxAmmo == -1) {
			FTimerDelegate WeaponDelegate;
			WeaponDelegate.BindUFunction(this, TEXT("WriteFromWeaponComponent"), NewPawn);
			GetWorld()->GetTimerManager().SetTimer(WeaponTimer, WeaponDelegate, 0.1f, true);
		}
	}
}

void UPlayerHUDWidget::OnPawnDeath()
{
	UE_LOG(LogTemp, Display, TEXT("SOMEBODY WAS KILLED"));
	MainHUD->SetVisibility(ESlateVisibility::Collapsed);
	bHasJustSpawned = true;
	DeathWidget->SetVisibility(ESlateVisibility::Visible);
	AmmoProgressBar->SetPercent(0.0f);
}

void UPlayerHUDWidget::WriteFromWeaponComponent(APawn* Pawn)
{
	const auto WeaponComponent = Pawn ? Pawn->FindComponentByClass<UWeaponComponent>() : nullptr;
	if (!WeaponComponent) return;

	MaxAmmo = WeaponComponent->GetMaxAmmo();
	if (MaxAmmo != 0) GetWorld()->GetTimerManager().ClearTimer(WeaponTimer);
}

void UPlayerHUDWidget::OnHealthChanged(float NewHealth)
{
	if (NewHealth < 0.0f)
	{ 
		UE_LOG(LogTemp, Display, TEXT("Failed. NewHealth: %.2f < 0.0f or %.2f == %.2f"), NewHealth, LastHP, NewHealth);
		bShouldInterpolateHealth = false;
		return;
	}
	
	UE_LOG(LogTemp, Display, TEXT("Current HP: %.2f, Last HP was %.2f"), LastHP, NewHealth);
	if (!bHasJustSpawned) {
		InterpolateHealthFrom = LastHP;
		InterpolateHealthTo = NewHealth;
		bShouldInterpolateHealth = true;

		if (!IsAnyAnimationPlaying() && NewHealth != LastHP)
		{
			PlayAnimation(NewHealth > LastHP ? HealthPickupAnimation : HealthReduceAnimation);
		}
	}

	bHasJustSpawned = false;
	LastHP = NewHealth;
}

void UPlayerHUDWidget::OnArmorChanged(float NewArmor)
{
	if (NewArmor < 0.0f)
	{
		bShouldInterpolateArmor = false;
		return;
	}
	
	if (NewArmor != LastArmor)
	{
		InterpolateArmorFrom = LastArmor;
		InterpolateArmorTo = NewArmor;
		bShouldInterpolateArmor = true;
	}
	UE_LOG(LogTemp, Display, TEXT("New armor: %.2f"), NewArmor);
	if (NewArmor == MaxArmor)
	{
		PlayAnimation(ArmorPickupAnimation);
	}
	LastArmor = NewArmor;
}

void UPlayerHUDWidget::OnAmmoChanged(int32 NewAmmo)
{
	if (NewAmmo < 0) return;

	UE_LOG(LogTemp, Display, TEXT("AMmo: %i"), NewAmmo);

	const float AmmoPercent = NewAmmo != 0 ? (float)(MaxAmmo - NewAmmo) / (float)(MaxAmmo) : 1.0f;
	AmmoProgressBar->SetPercent(AmmoPercent);
	if (NewAmmo == 0 && !IsAnimationPlaying(ReloadAnimation))
	{
		PlayAnimation(ReloadAnimation);
	}
}
