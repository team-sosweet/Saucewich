// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Entity/ActorPool.h"

#include "Engine/World.h"

#include "Entity/PoolActor.h"

const FActorSpawnParameters AActorPool::DefaultParameters;

AActorPool* AActorPool::Get(const UObject* const WorldContext)
{
	static TWeakObjectPtr<AActorPool> ActorPoolPtr;
	auto ActorPool = ActorPoolPtr.Get();
	if (ActorPool) return ActorPool;
	
	check(WorldContext);
	const auto World = WorldContext->GetWorld();
	check(World);
	ActorPool = World->SpawnActor<AActorPool>();
	check(ActorPool);
	ActorPoolPtr = ActorPool;
	return ActorPool;
}

APoolActor* AActorPool::Spawn(const TSubclassOf<APoolActor> Class, const FTransform& Transform, const FActorSpawnParameters& SpawnParameters)
{
	if (!Class) return nullptr;

	if (const auto PoolPtr = Pool.Find(Class))
	{
		while (PoolPtr->Num() > 0)
		{
			if (const auto Actor = PoolPtr->Pop().Get())
			{
				Actor->SetOwner(SpawnParameters.Owner);
				Actor->Instigator = SpawnParameters.Instigator;
				Actor->SetActorTransform(Transform);
				Actor->Activate();
				return Actor;
			}
		}
	}

	if (const auto Actor = static_cast<APoolActor*>(GetWorld()->SpawnActor(Class, &Transform, SpawnParameters)))
	{
		Actor->Activate();
		return Actor;
	}

	return nullptr;
}

void AActorPool::Release(APoolActor* const Actor)
{
	if (!IsValidLowLevelFast(false)) return;
	const auto Class = Actor->GetClass();
	Pool.FindOrAdd(Class).Add(Actor);
}
