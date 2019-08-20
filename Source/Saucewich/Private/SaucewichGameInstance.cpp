// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameInstance.h"

#include "Engine/World.h"
#include "TimerManager.h"

#include "ActorPool.h"
#include "SaucewichGameState.h"

DEFINE_LOG_CATEGORY_STATIC(LogSaucewichGameInstance, Log, All)

USaucewichGameInstance::USaucewichGameInstance()
	:ActorPoolClass{AActorPool::StaticClass()}
{
}

AActorPool* USaucewichGameInstance::GetActorPool()
{
	if (!ActorPool) ActorPool = static_cast<AActorPool*>(GetWorld()->SpawnActor(ActorPoolClass));
	return ActorPool;
}

ASaucewichGameState* USaucewichGameInstance::GetGameState() const
{
	return GetWorld()->GetGameState<ASaucewichGameState>();
}

void USaucewichGameInstance::CheckGameState()
{
	if (const auto GS = GetWorld()->GetGameState())
	{
		if (const auto SaucewichGS = Cast<ASaucewichGameState>(GS))
		{
			OnGameStateReady.Broadcast(SaucewichGS);
			OnGameStateReady.Clear();
		}
		else
		{
			UE_LOG(LogSaucewichGameInstance, Error, TEXT("Failed to cast game state to SaucewichGameState"));
		}
	}
	else
	{
		NotifyWhenGameStateReady();
	}
}

void USaucewichGameInstance::NotifyWhenGameStateReady()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &USaucewichGameInstance::CheckGameState);
}
