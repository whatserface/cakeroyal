// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FirstPersonWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class PETTEST_API AFirstPersonWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AFirstPersonWeapon();


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* WeaponMesh;
};
