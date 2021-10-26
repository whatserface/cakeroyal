// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePickup.generated.h"

UCLASS(Abstract, Blueprintable)
class PETTEST_API ABasePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	ABasePickup();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup")
	class UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	float RespawnTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Amplitude = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Frequency = 2.0f;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void PickupWasTaken(AActor* PickupActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void GivePickupTo(AActor* PickupActor);

	virtual void BeginPlay() override;

private:
	FTimerHandle RespawnTimerHandle;

	UPROPERTY(Replicated)
	float RotationYaw = 0.0f;

	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	bool bIsActive = true;

	UFUNCTION()
	void ChangeBehaviour(bool IsActive);
	
	void HandleMovement();
	void GenerateRotationYaw();
};
