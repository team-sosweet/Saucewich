// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Entity/ActorPool.h"
#include "Engine/World.h"
#include "Entity/PoolActor.h"
#include "SaucewichInstance.h"

const FActorSpawnParameters AActorPool::DefaultParameters;

AActorPool* AActorPool::Get(const UObject* const WorldContextObject)
{
	return Get(WorldContextObject->GetWorld());
}

AActorPool* AActorPool::Get(const UWorld* const World)
{
	return Get(World->GetGameInstanceChecked<USaucewichInstance>());
}

AActorPool* AActorPool::Get(const USaucewichInstance* const SaucewichInstance)
{
	return SaucewichInstance->GetActorPool();
}

APoolActor* AActorPool::Spawn(const TSubclassOf<APoolActor> Class, const FTransform& Transform, const FActorSpawnParameters& SpawnParameters)
{
	check(Class);

	if (const auto PoolPtr = Pool.Find(Class))
	{
		while (PoolPtr->Num() > 0)
		{
			if (const auto Actor = PoolPtr->Pop().Get())
			{
				Actor->SetOwner(SpawnParameters.Owner);
				Actor->SetInstigator(SpawnParameters.Instigator);
				Actor->SetActorTransform(Transform);
				Actor->Activate();
				return Actor;
			}
		}
	}

	if (const auto Actor = GetWorld()->SpawnActor<APoolActor>(Class, Transform, SpawnParameters))
	{
		Actor->Activate();
		return Actor;
	}

	return nullptr;
}

void AActorPool::Release(APoolActor* const Actor)
{
	check(IsValidLowLevel());
	const auto Class = Actor->GetClass();
	Pool.FindOrAdd(Class).Add(Actor);
}
