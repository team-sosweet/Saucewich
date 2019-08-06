// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameInstance.h"
#include "Engine/World.h"
#include "ActorPool.h"

USaucewichGameInstance::USaucewichGameInstance()
	:ActorPoolClass{AActorPool::StaticClass()}
{
	Sensitivity = 0.5f;
	A = B = 1.0f;
}

AActorPool* USaucewichGameInstance::GetActorPool()
{
	if (!ActorPool) ActorPool = static_cast<AActorPool*>(GetWorld()->SpawnActor(ActorPoolClass));
	return ActorPool;
}

float USaucewichGameInstance::GetSensitivity() const
{
	return (A * Sensitivity) + (A * B);
}

void USaucewichGameInstance::SetRevision(float NewA, float NewB)
{
	A = NewA; B = NewB;
}