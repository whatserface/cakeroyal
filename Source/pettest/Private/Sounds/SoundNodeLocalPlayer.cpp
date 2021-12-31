// Fill out your copyright notice in the Description page of Project Settings.

#include "Sounds/SoundNodeLocalPlayer.h"
#include "pettest.h"
#include "SoundDefinitions.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "LocalPlayer"

void USoundNodeLocalPlayer::ParseNodes(FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstances)
{
	// The accesses to the Pawn will be unsafe once we thread audio
	check(IsInGameThread());

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(ActiveSound.GetWorld(), "Player", OutActors);
	AActor* SoundOwner = nullptr;
	for (auto Actor: OutActors)
	{
		if (Actor->GetName() == ActiveSound.GetOwnerName() && Actor->GetUniqueID() == ActiveSound.GetOwnerID())
		{
			SoundOwner = Actor;
			break;
		}
	}
	if (SoundOwner == nullptr)
	{
		if (ChildNodes[1])
		{
			ChildNodes[1]->ParseNodes(AudioDevice, GetNodeWaveInstanceHash(NodeWaveInstanceHash, ChildNodes[1], 1), ActiveSound, ParseParams, WaveInstances); 
		}
		return;
	}

	APlayerController* PCOwner = Cast<APlayerController>(SoundOwner);
	APawn* PawnOwner = PCOwner ? PCOwner->GetPawn() : Cast<APawn>(SoundOwner);
	
	// If SoundOwner is weapon, then the pawn we're looking for is the owner of that weapon actor
	if (!PawnOwner) 
	{
		PCOwner = SoundOwner->GetOwner() ? Cast<APlayerController>(SoundOwner->GetOwner()) : nullptr;
		PawnOwner = PCOwner ? PCOwner->GetPawn() : 
			SoundOwner->GetOwner() ? Cast<APawn>(SoundOwner->GetOwner()) : nullptr;
	}

	const bool bIsLocallyControlled = PawnOwner && PawnOwner->IsLocallyControlled() && Cast<APlayerController>(PawnOwner->Controller);
	const int32 PlayIndex = bIsLocallyControlled ? 0 : 1;
	if (PlayIndex < ChildNodes.Num() && ChildNodes[PlayIndex])
	{
		ChildNodes[PlayIndex]->ParseNodes(AudioDevice, GetNodeWaveInstanceHash(NodeWaveInstanceHash, ChildNodes[PlayIndex], PlayIndex), ActiveSound, ParseParams, WaveInstances);
	}
}

void USoundNodeLocalPlayer::CreateStartingConnectors()
{
	InsertChildNode(ChildNodes.Num());
	InsertChildNode(ChildNodes.Num());
}

#if WITH_EDITOR
FText USoundNodeLocalPlayer::GetInputPinName(int32 PinIndex) const
{
	return (PinIndex == 0) ? LOCTEXT("LocalPlayerLabel", "Local") : LOCTEXT("RemotePlayerLabel", "Remote");
}
#endif

int32 USoundNodeLocalPlayer::GetMaxChildNodes() const
{
	return 2;
}

int32 USoundNodeLocalPlayer::GetMinChildNodes() const
{
	return 2;
}

#undef LOCTEXT_NAMESPACE