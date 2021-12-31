// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCoreTypes.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class APlayerCharacter;
class UCameraShakeBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PETTEST_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	FOnDeath OnDeath;
	FOnShieldChangedSignature OnHealthChanged;
	FOnShieldChangedSignature OnArmorChanged;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool IsDead() const { return FMath::IsNearlyZero(Health); }
	float GetMaxHealth() const { return MaxHealth; }
	float GetMaxArmor() const { return MaxArmor; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsHealthFull() const { return Health == MaxHealth; }
	
	UFUNCTION(BlueprintCallable, Category  = "Armor")
	bool IsArmorFull() const { return Armor == MaxArmor; }
	
	UFUNCTION(BlueprintCallable)
	void TryToAddHP(float HP);

	UFUNCTION(BlueprintCallable)
	void TryToAddArmor(float AP);

protected:
	UPROPERTY(Transient, Replicated)
	class APlayerCharacter* MyPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TSubclassOf<UCameraShakeBase> DamageCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Armor", meta = (ClampMin = "0"))
	float MaxArmor = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ClampMin = "0"))
	float HealUpdateTime = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ClampMin = "0"))
	float HealDelay = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ClampMin = "0"))
	float HealModifier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ClampMin = "0"))
	float HealThreshold = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ClampMin = "0"))
	float HPToHeal = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX", meta = (ClampMin = "0"))
	float ArmorCameraShakeScale = 0.4f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX", meta = (ClampMin = "0"))
	float HealthCameraShakeScale = 1.0f;

	virtual void BeginPlay() override;

private:
	FTimerHandle HealTimer;

	UPROPERTY(Replicated)
	float Health = 0.0f;

	UPROPERTY(Replicated)
	float Armor = 0.0f;

	float FiniteHP;

	UFUNCTION(Server, Reliable)
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Client, Unreliable, WithValidation)
	void Client_InvokeOnHealthChanged(float NewHealth);

	UFUNCTION(Client, Unreliable, WithValidation)
	void Client_InvokeOnArmorChanged(float NewArmor);

	UFUNCTION(Client, Unreliable, WithValidation)
	void Client_InvokeOnDeath();

	void Heal();
	void SetHealth(float NewHealth);
	void SetArmor(float NewArmor);
	void Killed(AController* KilledBy);
};
