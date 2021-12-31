// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/BasePickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasePickup, All, All)

ABasePickup::ABasePickup()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.02f;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	SetReplicateMovement(false);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	CollisionComponent->SetSphereRadius(51.0f);
	CollisionComponent->SetIsReplicated(true);
	RootComponent = CollisionComponent;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMeshComponent");
	PickupMesh->SetupAttachment(RootComponent);
}

void ABasePickup::BeginPlay()
{
	Super::BeginPlay();
	GenerateRotationYaw();
}

void ABasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive) HandleMovement();
}

void ABasePickup::HandleMovement()
{
	PickupMesh->AddLocalRotation(FRotator(0.0f, RotationYaw, 0.0f));
	
	FVector CurrentLocation = PickupMesh->GetRelativeLocation();
	CurrentLocation.Z = Amplitude * FMath::Sin(Frequency * GetWorld()->GetTimeSeconds());
	PickupMesh->SetRelativeLocation(CurrentLocation);
}

void ABasePickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePickup, bIsActive);
	DOREPLIFETIME_CONDITION(ABasePickup, RotationYaw, COND_InitialOnly);
}

void ABasePickup::PickupWasTaken(AActor* PickupActor) 
{
	if (!PickupActor)
	{
		UE_LOG(LogBasePickup, Warning, TEXT("Somebody sent nullptr to 'PickupWasTaken' method"));
		return;
	}
	else if (!HasAuthority()) return;

	SetOwner(PickupActor);
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, TEXT("ChangeBehaviour"), true);

	Client_PlaySound();
	GivePickupTo(PickupActor);
	ChangeBehaviour(false);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, TimerDel, RespawnTime, false);
}

void ABasePickup::Client_PlaySound_Implementation()
{
	UE_LOG(LogTemp, Display, TEXT("played sound"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
}

void ABasePickup::GivePickupTo_Implementation(AActor* PickupActor)
{
	UE_LOG(LogBasePickup, Warning, TEXT("Overlapped pickup doesn't have implementation of Give Pickup To method"));
}

void ABasePickup::ChangeBehaviour(bool IsActive)
{
	if (!HasAuthority()) return;

	bIsActive = IsActive;
	SetActorHiddenInGame(!IsActive);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, IsActive ? ECR_Overlap : ECR_Ignore);
}

void ABasePickup::GenerateRotationYaw() 
{
	if (!HasAuthority()) return;

	const auto Direction = FMath::RandBool() ? 1.0f : -1.0f;
	RotationYaw = FMath::RandRange(1.0f, 2.0f) * Direction;
	ForceNetUpdate();
}
