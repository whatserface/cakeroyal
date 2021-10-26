// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DeathWidget.h"
#include "Components/RespawnComponent.h"

bool UDeathWidget::GetRespawnTime(int32& CountDownTime) const
{
	const auto RespawnComponent = GetOwningPlayer()->FindComponentByClass<URespawnComponent>();
	if (!RespawnComponent || !RespawnComponent->IsRespawnInProgress()) return false;

	CountDownTime = RespawnComponent->GetRespawnCountDown();
	return true;
}
