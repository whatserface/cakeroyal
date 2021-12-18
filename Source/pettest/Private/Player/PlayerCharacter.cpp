// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "Player/CharacterController.h"
#include "Components/InputComponent.h"
#include "Components/MyCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/HealthComponent.h"
#include "Components/WeaponComponent.h"
#include "MyGameModeBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerCharacter, All, All)

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjInit)
	: Super(ObjInit.SetDefaultSubobjectClass<UMyCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->SetIsReplicated(true);
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>("WeaponComponent");
	CustomMovementComponent = Cast<UMyCharacterMovementComponent>(GetCharacterMovement());
	WeaponComponent->SetIsReplicated(true);

	InnerMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FPPMesh");
	InnerMesh->SetupAttachment(RootComponent);
	InnerMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	InnerMesh->SetOnlyOwnerSee(true);
	InnerMesh->SetCastShadow(false);

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("FPPMesh")));
	DefMesh = FRotationTranslationMatrix(DefMesh1P->GetRelativeRotation(), DefMesh1P->GetRelativeLocation());
	UpdateMeshes();

	check(HealthComponent);
	HealthComponent->OnDeath.AddUObject(this, &APlayerCharacter::Server_OnDeath);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnAround", this, &APlayerCharacter::TurnAround);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlayerCharacter::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &APlayerCharacter::StopRun);
	PlayerInputComponent->BindAction("OnMoveRight", IE_Pressed, this, &APlayerCharacter::OnMoveRightPressed);
	PlayerInputComponent->BindAction("OnMoveRight", IE_Released, this, &APlayerCharacter::OnMoveRightReleased);
	PlayerInputComponent->BindAction("OnMoveLeft", IE_Released, this, &APlayerCharacter::OnMoveRightReleased);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &APlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &APlayerCharacter::StopFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::Reload);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, IsMovingForward);
	DOREPLIFETIME(APlayerCharacter, WantsToRun);
	DOREPLIFETIME(APlayerCharacter, bCanRun);
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((Value > 0.0f) != IsMovingForward)
	{
		SetbIsMovingForward(Value > 0.0f);
	}

	if (Value == 0.0f) return;

	AddMovementInput(GetActorForwardVector(), Value);
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value == 0.0f) return;

	//SetbIsMovingForward(false);
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
	return bCanRun && IsMovingForward && WantsToRun && !GetVelocity().IsZero();
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
	IsMovingForward = Value;
}

void APlayerCharacter::SetCanRun(bool CanRun)
{
	if (HasAuthority()) bCanRun = CanRun;
}

void APlayerCharacter::OnMoveRightPressed_Implementation()
{
	CustomMovementComponent->SetRunModifier(SiteRunModifier);
}

void APlayerCharacter::OnMoveRightReleased_Implementation()
{
	CustomMovementComponent->SetRunModifier(RunModifier);
}

void APlayerCharacter::SetbWantsToRun_Implementation(bool Value)
{
	WantsToRun = Value;
}

void APlayerCharacter::Server_OnDeath_Implementation()
{
	SetLifeSpan(LastLifeSpan);
	WeaponComponent->StopFire();

	UE_LOG(LogPlayerCharacter, Display, TEXT("Player: %s will die in a few seconds"), *GetName());
	Multicast_Ragdoll();

	GetCharacterMovement()->DisableMovement();
	InnerMesh->SetVisibility(false, true);

	const auto CharController = Cast<ACharacterController>(Controller);
	if (!CharController) return;

	CharController->StartSpectating();
}

void APlayerCharacter::Multicast_Ragdoll_Implementation()
{
	if (IsRunningDedicatedServer()) return;

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);
	UpdateMeshes();
}

void APlayerCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	const FRotator RotCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);
	const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocation) * LocalToWorld.Inverse();
	const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
	const FMatrix MeshRelativeToCamera = DefMesh * LeveledCameraLS.Inverse();
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

	InnerMesh->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());
}

void APlayerCharacter::UpdateMeshes()
{
	InnerMesh->VisibilityBasedAnimTickOption = IsLocallyControlled() ? EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones : EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	GetMesh()->VisibilityBasedAnimTickOption = IsLocallyControlled() ? EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered : EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void APlayerCharacter::StartFire()
{
	if (!ShootMontageFPP) {
		UE_LOG(LogPlayerCharacter, Warning, TEXT("Shoot montage is nullptr!"));
	}

	if (WeaponComponent && WeaponComponent->CanShoot() && InnerMesh->GetAnimInstance() && //
		!InnerMesh->GetAnimInstance()->Montage_IsPlaying(ReloadMontageFPP))
	{
		if (ShootMontageFPP && !GetWorldTimerManager().IsTimerActive(ClientShootingTimer)) {
			ClientShootingTimerDel.BindUFunction(this, TEXT("PlayAnimMontageFPP"), ShootMontageFPP);
			GetWorldTimerManager().SetTimer(ClientShootingTimer, ClientShootingTimerDel, ShootMontageFPP->GetPlayLength(), true);
			PlayAnimMontageFPP(ShootMontageFPP);
		}
		WeaponComponent->StartFire();
	}
}

void APlayerCharacter::StopFire()
{
	if (!WeaponComponent) return;
	WeaponComponent->StopFire();

	if (GetWorldTimerManager().IsTimerActive(ClientShootingTimer)) {
		GetWorldTimerManager().ClearTimer(ClientShootingTimer);
	}
}

void APlayerCharacter::Reload()
{
	StopFire();
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([this]() {
		PlayAnimMontageFPP(ReloadMontageFPP);
		WeaponComponent->Reload();
	});
	FTimerHandle TempHandle;
	GetWorldTimerManager().SetTimer(TempHandle, TimerCallback, 0.1f, false);
}

void APlayerCharacter::PlayAnimMontageFPP(UAnimMontage* MontageToPlay)
{
	if (!InnerMesh || !InnerMesh->AnimScriptInstance || !MontageToPlay) {
		UE_LOG(LogPlayerCharacter, Warning, TEXT("In 'PlayAnimMontageFPP' some nullptr occured"));
		return;
	}
	const float Length = InnerMesh->AnimScriptInstance->Montage_Play(MontageToPlay);
	UE_LOG(LogTemp, Display, TEXT("Anim montage playing for: %.2f secs"), Length);

	for (FAnimMontageInstance* montage : InnerMesh->AnimScriptInstance->MontageInstances)
	{
		if (!montage || !montage->Montage) {
			continue;
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *montage->Montage->GetName());
	}
}
