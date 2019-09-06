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

USaucewichGameInstance::~USaucewichGameInstance()
{
	if (IsDedicatedServerInstance())
	{
		// TODO: 매치 서버에게 이 게임 서버가 종료되었음을 알림
	}
}

AActorPool* USaucewichGameInstance::GetActorPool()
{
	if (!IsValid(ActorPool)) ActorPool = static_cast<AActorPool*>(GetWorld()->SpawnActor(ActorPoolClass));
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
