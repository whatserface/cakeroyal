// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "CharacterController.h"
#include "HealthComponent.h"
#include "Components/InputComponent.h"
#include "MyCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjInit)
	: Super(ObjInit.SetDefaultSubobjectClass<UMyCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->SetIsReplicated(true);

	OuterMesh = CreateDefaultSubobject<USkeletalMeshComponent>("CharacterMesh1");
	OuterMesh->SetupAttachment(RootComponent);
	OuterMesh->SetOwnerNoSee(true);
	OuterMesh->bCastHiddenShadow = true;
	OuterMesh->SetIsReplicated(true);

	GetMesh()->SetOnlyOwnerSee(true);
	GetMesh()->SetCastShadow(false);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(HealthComponent)
		HealthComponent->OnDeath.AddUObject(this, &APlayerCharacter::Server_OnDeath);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnAround", this, &APlayerCharacter::TurnAround);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &APlayerCharacter::Run);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Released, this, &APlayerCharacter::StopRun);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, IsMovingForward);
	DOREPLIFETIME(APlayerCharacter, WantsToRun);
}

void APlayerCharacter::MoveForward(float Value)
{
	SetbIsMovingForward(Value > 0.0f);
	if (Value == 0.0f) return;

	AddMovementInput(GetActorForwardVector(), Value);
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value == 0.0f) return;

	SetbIsMovingForward(false);
	AddMovementInput(GetActorRightVector(), Value);
}

void APlayerCharacter::TurnAround(float Value)
{
	if (Value == 0.0f) return;

	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	if (Value == 0.0f) return;

	AddControllerPitchInput(Value);
}

bool APlayerCharacter::IsRunning() const
{
	return IsMovingForward && WantsToRun && !GetVelocity().IsZero();
}

void APlayerCharacter::Run()
{
	SetbWantsToRun(true);
}

void APlayerCharacter::StopRun()
{
	SetbWantsToRun(false);
}

void APlayerCharacter::SetbIsMovingForward_Implementation(bool Value)
{
	if (GetLocalRole() == ROLE_Authority) IsMovingForward = Value;
}

void APlayerCharacter::SetbWantsToRun_Implementation(bool Value)
{
	if (GetLocalRole() == ROLE_Authority) WantsToRun = Value;
}


void APlayerCharacter::Server_OnDeath_Implementation()
{
	SetLifeSpan(LifeSpan);

	Multicast_Ragdoll();

	if (GetLocalRole() == ROLE_Authority) {
		GetCharacterMovement()->DisableMovement();
		GetMesh()->SetVisibility(false, true);
		const auto CharController = Cast<ACharacterController>(Controller);
		if (!CharController) return;

		CharController->StartSpectating();
	}
}

void APlayerCharacter::Multicast_Ragdoll_Implementation()
{
	if (!IsRunningDedicatedServer())
	{
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		OuterMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		OuterMesh->SetSimulatePhysics(true);
	}
}

float APlayerCharacter::GetMovementDirection() const
{
	if (GetVelocity().IsZero()) return 0.0f;
	const auto VelocityNormal = GetVelocity().GetSafeNormal();
	const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
	const auto Degrees = FMath::RadiansToDegrees(AngleBetween);
	const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);
	return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}
