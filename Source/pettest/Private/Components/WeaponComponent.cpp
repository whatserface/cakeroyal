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
    if (!TPPWeapon) return;

    if (!TPPWeapon->OnAmmoChanged.IsBound()) {
        TPPWeapon->OnAmmoChanged.BindUObject(this, &UWeaponComponent::AmmoChanged);
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
        const bool bWasDestroyed = TPPWeapon->Destroy();
        UE_LOG(LogWeaponComponent, Display, TEXT("TPP Weapon %s destroyed"), bWasDestroyed ? TEXT("successfully ") : TEXT("couldn't be "));
    }
    else { UE_LOG(LogWeaponComponent, Warning, TEXT("When destroying TPP Weapon was nullptr")); }
    if (FPPWeapon)
    {
        FPPWeapon->DetachFromActor(Detachment);
        const bool bWasDestroyed = FPPWeapon->Destroy();
        UE_LOG(LogWeaponComponent, Display, TEXT("FPP Weapon %s destroyed"), bWasDestroyed ? TEXT("successfully ") : TEXT("couldn't be "));
    }
    MyPawn = nullptr;

    Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(UWeaponComponent, TPPWeapon);
    DOREPLIFETIME_CONDITION(UWeaponComponent, MyPawn, COND_InitialOnly);
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
    GetOwner()->ForceNetUpdate();

    TPPWeapon->OnTraceAppeared.BindUFunction(this, TEXT("TraceAppeared"));

    FTimerHandle TestTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TestTimerHandle, this, &UWeaponComponent::SpawnFPPWeapon, 0.5f, false);
}

void UWeaponComponent::SpawnFPPWeapon_Implementation()
{
    if (!MyPawn || !MyPawn->IsLocallyControlled()) {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Pawn isn't locally controlled"));
        return;
    }
    // assigning delegates to client, because they don't replicate
    if (!TPPWeapon->OnAmmoChanged.IsBound()) {
        TPPWeapon->OnAmmoChanged.BindUObject(this, &UWeaponComponent::AmmoChanged);
    }
    if (!TPPWeapon->OnTraceAppeared.IsBound()) {
        TPPWeapon->OnTraceAppeared.BindUFunction(this, TEXT("TraceAppeared"));
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
    if (!CanShoot()) return;
    
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
    if (!CanReload()) return;
    
    bReloadAnimInProgress = true;

    MyPawn->SetCanRun(false);

    PlayReloadAnim();
}

bool UWeaponComponent::CanShoot() const
{
    return TPPWeapon && TPPWeapon->CanShoot() && !bReloadAnimInProgress && MyPawn && !MyPawn->IsRunning();
}

void UWeaponComponent::PlayReloadAnim_Implementation()
{
    if (IsRunningDedicatedServer() || !MyPawn) return;

    if (!ReloadMontageTPP)
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("One of reload montage assets wasn't set"));
        return;
    }
    if (!MyPawn->IsLocallyControlled())
    {
        MyPawn->PlayAnimMontage(ReloadMontageTPP);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Stopping fire from comp"));
        OnAmmoChanged.Execute(0);
    }
}

int32 UWeaponComponent::GetMaxAmmo() const
{
    return TPPWeapon ? TPPWeapon->GetWeaponInfo().Bullets : -1;
}

float UWeaponComponent::GetFireRate() const
{
    return TPPWeapon ? TPPWeapon->GetWeaponInfo().ShootingRate : -1.0f;
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
    if (GetOwnerRole() != ROLE_Authority  || !TPPWeapon || !MyPawn || !(MeshComp == MyPawn->GetInnerMesh() || MeshComp == MyPawn->GetMesh())) return;

    MyPawn->SetCanRun(true);
    UE_LOG(LogTemp, Display, TEXT("Reload finished on %s"), (GetNetMode() == ROLE_AutonomousProxy) ? TEXT("owning client") : TEXT("different client"));
    TPPWeapon->Reload();
    bReloadAnimInProgress = false;
}

bool UWeaponComponent::CanReload() const
{
    if (GetOwnerRole() < ROLE_AutonomousProxy)
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Running CanReload method not on server or local controlled client"));
        return false;
    }
    return MyPawn && !MyPawn->IsPendingKill() && !bReloadAnimInProgress && TPPWeapon && TPPWeapon->CanReload();
}

void UWeaponComponent::AmmoChanged(int32 NewAmmo)
{
    if (!MyPawn || !MyPawn->IsLocallyControlled()) return;
    UE_LOG(LogTemp, Display, TEXT("Delegate Invoked in component! Bullets: %i, it %s"), NewAmmo, (GetOwnerRole() == ROLE_Authority) ? TEXT("Ran on server") : TEXT("didn't run on server"));

    OnAmmoChanged.ExecuteIfBound(NewAmmo);
    if (NewAmmo == 0 && !bReloadAnimInProgress)
    {
        MyPawn->Reload();
    }
}

void UWeaponComponent::TraceAppeared_Implementation(FVector TraceEnd)
{
    OnTraceAppeared.ExecuteIfBound(TraceEnd);
}
