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
    SpawnWeapon();
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOG(LogWeaponComponent, Display, TEXT("End play reason: %s"), *StaticEnum<EEndPlayReason::Type>()->GetNameStringByValue(EndPlayReason));
    if (!ObservedWeapons.DestroyAndDetach(ObservedWeapons.FPPWeapon)) { UE_LOG(LogWeaponComponent, Warning, TEXT("Destroying and detachment of FPP Weapon wasn't successful")); }
    if (!ObservedWeapons.DestroyAndDetach(ObservedWeapons.TPPWeapon)) { UE_LOG(LogWeaponComponent, Warning, TEXT("Destroying and detachment of TPP Weapon wasn't successful")); }
    Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UWeaponComponent, ObservedWeapons);
    DOREPLIFETIME(UWeaponComponent, Weapon);
}
void UWeaponComponent::SpawnWeapon_Implementation()
{
    if (!GetWorld() || !GetOwner() || !GetOwner()->HasAuthority()) return;

    const auto Player = Cast<APlayerCharacter>(GetOwner());
    if (!Player) { UE_LOG(LogWeaponComponent, Warning, TEXT("Cast was failed")); return; }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Player;
    SpawnParams.Instigator = Player;
    const auto FWeapon = GetWorld()->SpawnActor<ARifleWeapon>(WeaponClass, SpawnParams);
    if(!FWeapon) { UE_LOG(LogWeaponComponent, Warning, TEXT("FPP Weapon Actor couldn't be spawned")); return; }

    ObservedWeapons.FPPWeapon = FWeapon;
    Weapon = FWeapon;
    const auto TWeapon = GetWorld()->SpawnActor<ARifleWeapon>(WeaponClass, SpawnParams);
    if (!TWeapon) { UE_LOG(LogWeaponComponent, Warning, TEXT("TPP Weapon Actor couldn't be spawned")); return; }

    ObservedWeapons.TPPWeapon = TWeapon;
    FWeapon->AttachToPlayer(WeaponSocketName, true);
    TWeapon->AttachToPlayer(WeaponSocketName, false);

    UE_LOG(LogWeaponComponent, Display, TEXT("Spawn was successful"));
}
 
void UWeaponComponent::StartFire_Implementation()
{
    if (!Weapon) { UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon's pointer is null")); return; }

    UE_LOG(LogWeaponComponent, Display, TEXT("FIRED"));
    Weapon->StartFire();
}

void UWeaponComponent::StopFire()
{
    if (!Weapon) return;

    Weapon->StopFire();
}
