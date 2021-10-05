// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCoreTypes.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PETTEST_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	FOnDeath OnDeath;
	FOnHealthChangedSignature OnHealthChanged;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool IsDead() const { return FMath::IsNearlyZero(Health); }
	bool IsHealthFull() const { return Health == MaxHealth; }
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
	void TryToAddHP(float HP);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ClampMin = "0"))
	float HealUpdateTime = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ClampMin = "0"))
	float HealDelay = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ClampMin = "0"))
	float HealModifier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ClampMin = "0"))
	float HealThreshold = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ClampMin = "0"))
	float HPToAdd = 20.0f;

	virtual void BeginPlay() override;

private:
	FTimerHandle HealTimer;

	UPROPERTY(Replicated)
	float Health = 0.0f;

	float FiniteHP;

	UFUNCTION(Server, Reliable)
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Client, Unreliable, WithValidation)
	void Client_InvokeOnHealthChanged(float NewHealth);

	void Heal();
	void SetHealth(float NewHealth);
};
