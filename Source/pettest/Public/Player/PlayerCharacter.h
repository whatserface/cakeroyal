// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UHealthComponent;
class FObjectInitializer;
class USkeletalMeshComponent;
class AThirdPersonWeapon;
class AFirstPersonWeapon;
class UWeaponComponent;

UCLASS()
class PETTEST_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjInit);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetMovementDirection() const;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void PlayAnimMontageFPP(UAnimMontage* MontageToPlay);

	void SetCanRun(bool CanRun);
	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation);
	USkeletalMeshComponent* GetInnerMesh() { return InnerMesh; }
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* InnerMesh;

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
	
	UPROPERTY(Replicated)
	bool bCanRun = true;

	UFUNCTION(Server, Unreliable, Category = "Movement")
	void SetbIsMovingForward(bool Value);

	UFUNCTION(Server, Unreliable, Category = "Movement")
	void SetbWantsToRun(bool Value);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAround(float Value);
	void LookUp(float Value);

	void Run();
	void StopRun();
	FRotator DefaultFPPRotation;
	FVector DefaultFPPLocation;
};