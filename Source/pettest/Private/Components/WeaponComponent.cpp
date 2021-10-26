// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponComponent.h"
#include "Player/PlayerCharacter.h"
#include "Weapon/RifleWeapon.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "ReloadAnimNotify.h"
 
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
        SpawnTPPWeapon();
        InitAnimations();
    }
    if (TPPWeapon)
    {
        TPPWeapon->OnReload.BindUFunction(this, TEXT("Reload"));
    }
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
    DOREPLIFETIME_CONDITION(UWeaponComponent, bReloadAnimInProgress, COND_OwnerOnly);
}

void UWeaponComponent::SpawnTPPWeapon()
{
    if (!GetWorld() || GetOwnerRole() != ROLE_Authority || !MyPawn) return;

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
    GetWorld()->GetTimerManager().SetTimer(TestTimerHandle, this, &UWeaponComponent::SpawnFPPWeapon, 0.3f, false);
}
 
void UWeaponComponent::SpawnFPPWeapon_Implementation()
{
    if (!MyPawn->IsLocallyControlled()) {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Pawn isn't locally controlled"));
        return;
    }
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
}

void UWeaponComponent::StartFire_Implementation()
{
    if (!TPPWeapon || !MyPawn)
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon or pawn's pointer is null"));
        return;
    }
    if (bReloadAnimInProgress) return;
    
    TPPWeapon->StartFire();
}

void UWeaponComponent::StopFire_Implementation()
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
    if (!TPPWeapon)
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Weapon's pointer is null"));
        return;
    }
    if (!CanReload() || !MyPawn || MyPawn->IsPendingKill()) return;

    MyPawn->SetCanRun(false);
    bReloadAnimInProgress = true;
    PlayReloadAnim();
}

bool UWeaponComponent::CanShoot() const
{
    return TPPWeapon && !TPPWeapon->IsAmmoEmpty() && !bReloadAnimInProgress && MyPawn && !MyPawn->IsRunning();
}

void UWeaponComponent::PlayReloadAnim_Implementation()
{
    if (!IsRunningDedicatedServer() && MyPawn)
    {
        if (!ReloadMontageFPP || !ReloadMontageTPP)
        {
            UE_LOG(LogWeaponComponent, Warning, TEXT("One of reload montage assets wasn't set"));
            return;
        }
        if (MyPawn->IsLocallyControlled() && MyPawn->GetInnerMesh() && MyPawn->GetInnerMesh()->GetAnimInstance())
        {
            OnReload.Execute();
            MyPawn->PlayAnimMontageFPP(ReloadMontageFPP);
            UE_LOG(LogTemp, Display, TEXT("TPP WEAPON: %s"), *TPPWeapon->GetName());
            //const float length = MyPawn->GetInnerMesh()->GetAnimInstance()->Montage_Play(ReloadMontageFPP, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, false);
            //UE_LOG(LogTemp, Display, TEXT("Playing animation for %.2f seconds"), length);
        }
        else
        {
            MyPawn->PlayAnimMontage(ReloadMontageTPP);
        }
    }
}

bool UWeaponComponent::GetAmmoPercent(float& OutAmmoPercent)
{
    if (!TPPWeapon || !MyPawn || !MyPawn->IsLocallyControlled()) return false;

    TPPWeapon->GetWeaponBullets(OutAmmoPercent);
    return true;
}

void UWeaponComponent::InitAnimations()
{
    if (!ReloadMontageFPP || !ReloadMontageTPP) return;

    auto NotifyEvents = ReloadMontageFPP->Notifies;
    for (FAnimNotifyEvent NotifyEvent : NotifyEvents)
    {
        auto ReloadFinishedNotify = Cast<UReloadAnimNotify>(NotifyEvent.Notify);
        if (ReloadFinishedNotify)
        {
            ReloadFinishedNotify->OnNotified.AddUObject(this, &UWeaponComponent::OnReloadFinished);
            break;
        }
    }
    NotifyEvents = ReloadMontageTPP->Notifies;
    for (FAnimNotifyEvent NotifyEvent : NotifyEvents)
    {
        auto ReloadFinishedNotify = Cast<UReloadAnimNotify>(NotifyEvent.Notify);
        if (ReloadFinishedNotify)
        {
            ReloadFinishedNotify->OnNotified.AddUObject(this, &UWeaponComponent::OnReloadFinished);
            break;
        }
    }
}

void UWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComp)
{
    UE_LOG(LogTemp, Display, TEXT("Almost reloaded!"));
    if (GetOwnerRole() != ROLE_Authority  || !TPPWeapon || !MyPawn || !(MeshComp == MyPawn->GetInnerMesh() || MeshComp == MyPawn->GetMesh())) return;

    MyPawn->SetCanRun(true);
    UE_LOG(LogTemp, Display, TEXT("Reload finished"));
    TPPWeapon->Reload();
    bReloadAnimInProgress = false;
}

bool UWeaponComponent::CanReload() const
{
    if (GetOwnerRole() != ROLE_Authority)
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Running CanReload method not on server"));
        return false;
    }
    return !bReloadAnimInProgress && TPPWeapon && TPPWeapon->CanReload();
}
