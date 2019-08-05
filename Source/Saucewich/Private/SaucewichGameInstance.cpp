// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameInstance.h"
#include "Engine/World.h"
#include "ActorPool.h"

USaucewichGameInstance::USaucewichGameInstance()
	:ActorPoolClass{AActorPool::StaticClass()}
{
}

AActorPool* USaucewichGameInstance::GetActorPool()
{
	if (!ActorPool)
		ActorPool = static_cast<AActorPool*>(GetWorld()->SpawnActor(ActorPoolClass));
	return ActorPool;
}
