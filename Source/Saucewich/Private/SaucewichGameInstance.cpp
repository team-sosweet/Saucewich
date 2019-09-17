// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameInstance.h"

#include "Engine/World.h"
#include "TimerManager.h"

#include "ActorPool.h"
#include "SaucewichGameState.h"

DEFINE_LOG_CATEGORY_STATIC(LogSaucewichGameInstance, Log, All)

AActorPool* USaucewichGameInstance::GetActorPool()
{
	if (!IsValid(ActorPool)) ActorPool = GetWorld()->SpawnActor<AActorPool>();
	return ActorPool;
}

ASaucewichGameState* USaucewichGameInstance::GetGameState() const
{
	return GetWorld()->GetGameState<ASaucewichGameState>();
}

float USaucewichGameInstance::GetSensitivity() const
{
	return CorrectionValue * Sensitivity + CorrectionValue * .5f;
}
