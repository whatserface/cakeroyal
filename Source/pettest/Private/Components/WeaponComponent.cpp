// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponComponent.h"
#include "Player/PlayerCharacter.h"
#include "Weapon/RifleWeapon.h"
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

    if (GetOwnerRole() == ROLE_Authority)
    {
        MyPawn = Cast<APlayerCharacter>(GetOwner());
        check(MyPawn);
    }
    SpawnTPPWeapon();
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOG(LogWeaponComponent, Display, TEXT("End play reason: %s"), *StaticEnum<EEndPlayReason::Type>()->GetNameStringByValue(EndPlayReason));
    //UE_LOG(LogWeaponComponent, Display, TEXT("End play reason: %s"), *UEnum::GetValueAsString(EndPlayReason));

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
    MyPawn = nullptr;

    Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(UWeaponComponent, MyPawn);
    DOREPLIFETIME(UWeaponComponent, TPPWeapon);
}

void UWeaponComponent::SpawnTPPWeapon_Implementation()
{
    if (!GetWorld() || !MyPawn || !MyPawn->HasAuthority()) return;

    UE_LOG(LogWeaponComponent, Display, TEXT("Spawning TPP Weapon"));
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = MyPawn;
    SpawnParams.Instigator = MyPawn;
    if (!TPPWeapon)
    {
        const auto TWeapon = GetWorld()->SpawnActor<AThirdPersonWeapon>(TPPWeaponClass, SpawnParams);
        if (!TWeapon) { UE_LOG(LogWeaponComponent, Warning, TEXT("TPP Weapon Actor couldn't be spawned")); return; }
    
        TPPWeapon = TWeapon;
    }
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, false);
    TPPWeapon->AttachToComponent(MyPawn->GetMesh(), TransformRules, WeaponSocketName);
    FTimerHandle TestTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TestTimerHandle, this, &UWeaponComponent::SpawnFPPWeapon, 0.5f, false);
}
 
void UWeaponComponent::SpawnFPPWeapon_Implementation()
{
    if (!MyPawn->IsLocallyControlled()) {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Pawn isn't locally controlled"));
        return;
    }
    UE_LOG(LogWeaponComponent, Display, TEXT("Spawning FPP Weapon"));
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = MyPawn;
    SpawnParams.Instigator = MyPawn;
    const auto FWeapon = GetWorld()->SpawnActor<AFirstPersonWeapon>(TPPWeapon->GetWeaponInfo().FPPWeaponClass, SpawnParams);
    if (!FWeapon) 
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("FPP Weapon Actor couldn't be spawned"));
        return;
    }
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, false);
    FWeapon->AttachToComponent(MyPawn->GetInnerMesh(), TransformRules, WeaponSocketName);
    FPPWeapon = FWeapon;
    UE_LOG(LogWeaponComponent, Display, TEXT("FPP Weapon spawn was succeeded"));
}

void UWeaponComponent::StartFire()
{
    if (!TPPWeapon || !MyPawn)
    { 
        UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon or pawn's pointer is null"));
        return;
    }

    TPPWeapon->StartFire();
}

void UWeaponComponent::StopFire()
{
    if (!TPPWeapon)
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon's pointer is null")); 
        return; 
    }

    TPPWeapon->StopFire();
}

void UWeaponComponent::Reload_Implementation()
{
    if(!TPPWeapon)
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon's pointer is null"));
        return;
    }
    
    PlayReloadAnim();
    TPPWeapon->Reload();
}

void UWeaponComponent::PlayReloadAnim_Implementation()
{
    if (!IsRunningDedicatedServer() && MyPawn)
    {
        if (MyPawn->IsLocallyControlled())
        {
            MyPawn->PlayAnimMontageFPP(ReloadMontageFPP);
        }
        else
        {
            MyPawn->PlayAnimMontage(ReloadMontageTPP);
        }
    }
}

void UWeaponComponent::PlayAnimMontage(UAnimMontage* Animation)
{
    //
}

bool UWeaponComponent::GetAmmoPercent(float& OutAmmoPercent)
{
    if (!TPPWeapon || !MyPawn || !MyPawn->IsLocallyControlled()) return false;

    TPPWeapon->GetWeaponBullets(OutAmmoPercent);
    return true;
}
