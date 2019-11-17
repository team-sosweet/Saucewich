// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichInstance.h"
#include "Engine/World.h"
#include "ActorPool.h"
#include "SauceMarker.h"

template <class T>
static T* Get(T*& Ptr, UClass* const Class, UWorld* const World)
{
	if (!IsValid(Ptr)) Ptr = World->SpawnActor<T>(Class);
	return Ptr;
}

template <class T>
static T* Get(T*& Ptr, UWorld* const World)
{
	return Get(Ptr, T::StaticClass(), World);
}

AActorPool* USaucewichInstance::GetActorPool() const
{
	return Get(ActorPool, GetWorld());
}

ASauceMarker* USaucewichInstance::GetSauceMarker() const
{
	return Get(SauceMarker, SauceMarkerClass.LoadSynchronous(), GetWorld());
}
