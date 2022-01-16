// Fill out your copyright notice in the Description page of Project Settings.

#include "Sounds/SoundNodeLocalPlayer.h"
#include "pettest.h"
#include "SoundDefinitions.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "LocalPlayer"

TMap<uint32, bool> USoundNodeLocalPlayer::LocallyControlledActorCache;

void USoundNodeLocalPlayer::ParseNodes(FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstances)
{
	bool bLocallyControlled = false;
	if (bool* LocallyControlledPtr = LocallyControlledActorCache.Find(ActiveSound.GetOwnerID()))
	{
		bLocallyControlled = *LocallyControlledPtr;
	}

	const int32 PlayIndex = bLocallyControlled ? 0 : 1;

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