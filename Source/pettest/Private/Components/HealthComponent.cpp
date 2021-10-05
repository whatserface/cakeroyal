// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All)

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	AActor* ComponentOwner = GetOwner();
	if(ComponentOwner)
	{
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnTakeAnyDamage);
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
}

void UHealthComponent::OnTakeAnyDamage_Implementation(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || IsDead() || !GetWorld()) return;
	if (GetWorld()->GetTimerManager().IsTimerActive(HealTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(HealTimer);
	}
	SetHealth(Health - Damage);
	if (IsDead())
	{
		OnDeath.Broadcast();
	}
	else if (Health < HealThreshold)
	{
		GetWorld()->GetTimerManager().SetTimer(HealTimer, this, &UHealthComponent::Heal, HealUpdateTime, true, HealDelay);
		FiniteHP = Health + HPToAdd;
	}
}

void UHealthComponent::Heal()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		SetHealth(Health + HealModifier);
		if (Health >= FiniteHP && GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(HealTimer);
		}
	}
}

void UHealthComponent::SetHealth(float NewHealth)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
		//const float HealthDelta = Health - NewHealth;
		OnHealthChanged.Broadcast(Health/*, HealthDelta*/);
		Client_InvokeOnHealthChanged(Health);
	}
}

bool UHealthComponent::Client_InvokeOnHealthChanged_Validate(float NewHealth)
{
	return NewHealth >= 0.0f && NewHealth <= MaxHealth;
}

void UHealthComponent::Client_InvokeOnHealthChanged_Implementation(float NewHealth)
{
	OnHealthChanged.Broadcast(NewHealth);
}

void UHealthComponent::TryToAddHP(float HP)
{
	SetHealth(Health + HP);
}
