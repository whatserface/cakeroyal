// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "MyGameModeBase.h"
#include "Player/PlayerCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All)

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHealth(MaxHealth);
	MyPawn = Cast<APlayerCharacter>(GetOwner());
	if (!MyPawn) return;
	
	MyPawn->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnTakeAnyDamage);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UHealthComponent, MyPawn);
	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, Armor);
}

void UHealthComponent::OnTakeAnyDamage_Implementation(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || IsDead() || !GetWorld() || !MyPawn) return;
	if (GetWorld()->GetTimerManager().IsTimerActive(HealTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(HealTimer);
	}
	
	if (Armor > 0.0f) {
		UE_LOG(LogTemp, Display, TEXT("Armor: %.2f"), Armor);
		SetArmor(Armor - Damage);
	}
	else {
		UE_LOG(LogTemp, Display, TEXT("Health: %.2f"), Health);
		SetHealth(Health - Damage);
	}

	if (IsDead())
	{
		Killed(InstigatedBy);
		MyPawn->ForceNetUpdate();
		Client_InvokeOnDeath();
		OnDeath.Broadcast();
	}
	else if (Health < HealThreshold)
	{
		GetWorld()->GetTimerManager().SetTimer(HealTimer, this, &UHealthComponent::Heal, HealUpdateTime, true, HealDelay);
		FiniteHP = Health + HPToHeal;
	}
}

void UHealthComponent::Killed(AController* KilledBy)
{
	if (!GetWorld() || GetOwnerRole() != ROLE_Authority || !MyPawn) return;

	const auto GameMode = GetWorld()->GetAuthGameMode<AMyGameModeBase>();
	if (!GameMode) return;

	const auto VictimController = MyPawn->Controller;
	if (!VictimController)
	{
		UE_LOG(LogTemp, Warning, TEXT("IN HEALTH, victim controller is nullptr"));
		return;
	}
	GameMode->Killed(KilledBy, VictimController);
}

void UHealthComponent::Heal()
{
	if (GetOwnerRole() != ROLE_Authority) return;
	
	SetHealth(Health + HealModifier);
	if (Health >= FiniteHP && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HealTimer);
	}
}

void UHealthComponent::SetHealth(float NewHealth)
{
	if (GetOwnerRole() != ROLE_Authority) return;

	Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	//OnHealthChanged.Broadcast(Health/*, HealthDelta*/);
	Client_InvokeOnHealthChanged(Health);
}

void UHealthComponent::SetArmor(float NewArmor)
{
	if (GetOwnerRole() != ROLE_Authority) return;

	if ((Armor - NewArmor) > Armor)
	{
		UE_LOG(LogTemp, Display, TEXT("Setting health from armor: %.2f"), Health + NewArmor);
		SetHealth(Health + NewArmor);
	}
	Armor = FMath::Clamp(NewArmor, 0.0f, MaxArmor);
	UE_LOG(LogTemp, Display, TEXT("Set armor: %.2f"), Armor);
	OnArmorChanged.Broadcast(Armor);
	Client_InvokeOnArmorChanged(Armor);
}

bool UHealthComponent::Client_InvokeOnHealthChanged_Validate(float NewHealth)
{
	return NewHealth >= 0.0f && NewHealth <= MaxHealth;
}

void UHealthComponent::Client_InvokeOnHealthChanged_Implementation(float NewHealth)
{
	OnHealthChanged.Broadcast(NewHealth);
}

bool UHealthComponent::Client_InvokeOnArmorChanged_Validate(float NewArmor)
{
	return NewArmor >= 0.0f && NewArmor <= MaxArmor;
}

void UHealthComponent::Client_InvokeOnArmorChanged_Implementation(float NewArmor)
{
	OnArmorChanged.Broadcast(NewArmor);
}

bool UHealthComponent::Client_InvokeOnDeath_Validate()
{
	return true;//IsDead();
}

void UHealthComponent::Client_InvokeOnDeath_Implementation()
{
	OnDeath.Broadcast();
}

void UHealthComponent::TryToAddHP(float HP)
{
	SetHealth(Health + HP);
}

void UHealthComponent::TryToAddArmor(float AP)
{
	SetArmor(Armor + AP);
}
