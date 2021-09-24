// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "PlayerCharacter.h"
#include "RifleWeapon.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All)

UWeaponComponent::UWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    SpawnTPPWeapon();
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOG(LogWeaponComponent, Display, TEXT("End play reason: %s"), *StaticEnum<EEndPlayReason::Type>()->GetNameStringByValue(EndPlayReason));

    const auto Detachment = FDetachmentTransformRules::KeepWorldTransform;
    if (TPPWeapon) 
    {
        TPPWeapon->DetachFromActor(Detachment);
        TPPWeapon->Destroy();
        UE_LOG(LogWeaponComponent, Display, TEXT("TPP Weapon successfully destroyed"));
    }
    else { UE_LOG(LogWeaponComponent, Warning, TEXT("When destroying TPP Weapon was nullptr")); }
    if (FPPWeapon)
    {
        FPPWeapon->DetachFromActor(Detachment);
        FPPWeapon->Destroy();
        UE_LOG(LogWeaponComponent, Display, TEXT("FPP Weapon successfully destroyed"));
    }

    Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UWeaponComponent, TPPWeapon);
}

void UWeaponComponent::SpawnTPPWeapon_Implementation()
{
    if (!GetWorld() || !GetOwner() || !GetOwner()->HasAuthority()) return;

    const auto Player = Cast<APlayerCharacter>(GetOwner());
    if (!Player) { UE_LOG(LogWeaponComponent, Warning, TEXT("Cast was failed")); return; }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Player;
    SpawnParams.Instigator = Player;
    if (!TPPWeapon)
    {
        const auto TWeapon = GetWorld()->SpawnActor<AThirdPersonWeapon>(TPPWeaponClass, SpawnParams);
        if (!TWeapon) { UE_LOG(LogWeaponComponent, Warning, TEXT("TPP Weapon Actor couldn't be spawned")); return; }
    
        TPPWeapon = TWeapon;
    }
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, false);
    TPPWeapon->AttachToComponent(Player->GetOuterMesh(), TransformRules, WeaponSocketName);
    SpawnFPPWeapon();
}
 
void UWeaponComponent::SpawnFPPWeapon_Implementation()
{
    if (!GetOwner()) return;
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player) { UE_LOG(LogWeaponComponent, Warning, TEXT("Cast was failed")); return; }
    if (!Player->IsLocallyControlled()) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Player;
    SpawnParams.Instigator = Player;
    const auto FWeapon = GetWorld()->SpawnActor<AFirstPersonWeapon>(TPPWeapon->GetWeaponInfo().FPPWeaponClass, SpawnParams);
    if (!FWeapon) { UE_LOG(LogWeaponComponent, Warning, TEXT("FPP Weapon Actor couldn't be spawned")); return; }

    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, false);
    FWeapon->AttachToComponent(Player->GetMesh(), TransformRules, WeaponSocketName);
    FPPWeapon = FWeapon;
}

void UWeaponComponent::StartFire_Implementation()
{
    if (!TPPWeapon) { UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon's pointer is null")); return; }

    TPPWeapon->StartFire();
}

void UWeaponComponent::StopFire()
{
    if (!TPPWeapon) { UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon's pointer is null")); return; }

    //Weapon->StopFire();
}
