// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/BasePickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasePickup, All, All)

ABasePickup::ABasePickup()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.02f;

	bReplicates = true;
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
	InitialLocation = PickupMesh->GetRelativeLocation();
	GenerateRotationYaw();
}

void ABasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActive) return;
	HandleMovement();
}

void ABasePickup::HandleMovement()
{
	if (!GetWorld()) return;

	PickupMesh->AddLocalRotation(FRotator(0.0f, RotationYaw, 0.0f));

	FVector CurrentLocation = PickupMesh->GetRelativeLocation();
	CurrentLocation.Z = InitialLocation.Z + Amplitude * FMath::Sin(Frequency * GetWorld()->GetTimeSeconds());
	PickupMesh->AddLocalOffset(CurrentLocation);
}

void ABasePickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePickup, bIsActive);
	DOREPLIFETIME_CONDITION(ABasePickup, RotationYaw, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ABasePickup, InitialLocation, COND_InitialOnly);
}

void ABasePickup::PickupWasTaken(AActor* PickupActor) 
{
	if (!PickupActor)
	{
		UE_LOG(LogBasePickup, Warning, TEXT("Somebody sent nullptr to 'PickupWasTaken' method"));
		return;
	}
	else if (!HasAuthority()) return;

	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, TEXT("ChangeBehaviour"), true);

	GivePickupTo(PickupActor);
	ChangeBehaviour(false);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, TimerDel, RespawnTime, false);
}

void ABasePickup::ChangeBehaviour(bool IsActive)
{
	if (!HasAuthority()) return;

	bIsActive = IsActive;
	SetActorHiddenInGame(!IsActive);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, IsActive ? ECR_Overlap : ECR_Ignore);
}

void ABasePickup::GivePickupTo(AActor* PickupActor) {}

void ABasePickup::GenerateRotationYaw() 
{
	if (!HasAuthority()) return;

	const auto Direction = FMath::RandBool() ? 1.0f : -1.0f;
	RotationYaw = FMath::RandRange(1.0f, 2.0f) * Direction;
	ForceNetUpdate();
}
