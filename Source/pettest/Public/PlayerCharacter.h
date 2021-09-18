// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UWeaponComponent;
class UHealthComponent;
class FObjectInitializer;
class USkeletalMeshComponent;

UCLASS()
class PETTEST_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjInit);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	USkeletalMeshComponent* GetLocalMesh();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetMovementDirection() const;

	USkeletalMeshComponent* GetOuterMesh() { return OuterMesh; }
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* OuterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Death")
	float LastLifeSpan = 2.5f;

	virtual void BeginPlay() override;

	UFUNCTION(Server, Unreliable, Category = "Death")
	void Server_OnDeath();

	UFUNCTION(NetMulticast, Unreliable, Category = "Death")
	void Multicast_Ragdoll();

private:
	UPROPERTY(Replicated)
	bool WantsToRun = false;

	UPROPERTY(Replicated)
	bool IsMovingForward = false;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void Run();
	void StopRun();

	UFUNCTION(Server, Unreliable, Category = "Movement")
	void SetbIsMovingForward(bool Value);

	UFUNCTION(Server, Unreliable, Category = "Movement")
	void SetbWantsToRun(bool Value);


	void TurnAround(float Value);
	void LookUp(float Value);
};
