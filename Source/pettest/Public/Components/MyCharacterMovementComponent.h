// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacterMovementComponent.generated.h"

class FObjectInitializer;
class APlayerCharacter;

UCLASS()
class PETTEST_API UMyCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UMyCharacterMovementComponent(const FObjectInitializer& ObjInit);
	
	virtual float GetMaxSpeed() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetRunModifier(float NewModifier);
	float GetDefaultRunModifier() const { return RunModifier; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "1.5", ClampMax = "12"))
	float RunModifier = 1.8f;

	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient, Replicated)
	APlayerCharacter* OwningPlayer;
};
