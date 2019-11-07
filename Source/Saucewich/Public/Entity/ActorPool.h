// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ActorPool.generated.h"

class APoolActor;

UCLASS(NotBlueprintable)
class SAUCEWICH_API AActorPool : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta=(DisplayName="Get Actor Pool", WorldContext="WorldContext"))
	static AActorPool* Get(const UObject* WorldContext);
	
	APoolActor* Spawn(TSubclassOf<APoolActor> Class, const FTransform& Transform = FTransform::Identity, const struct FActorSpawnParameters& SpawnParameters = DefaultParameters);

	template <class T>
	T* Spawn(const TSubclassOf<T> Class, const FTransform& Transform = FTransform::Identity, const FActorSpawnParameters& SpawnParameters = DefaultParameters)
	{
		static_assert(TIsDerivedFrom<T, APoolActor>::IsDerived, "T must be derived from APoolActor");
		return Cast<T>(Spawn(*Class, Transform, SpawnParameters));
	}

	template <class T>
	T* Spawn(const FTransform& Transform = FTransform::Identity, const FActorSpawnParameters& SpawnParameters = DefaultParameters)
	{
		static_assert(TIsDerivedFrom<T, APoolActor>::IsDerived, "T must be derived from APoolActor");
		return static_cast<T*>(Spawn(T::StaticClass(), Transform, SpawnParameters));
	}

	void Release(APoolActor* Actor);

private:
	static const FActorSpawnParameters DefaultParameters;
	TMap<TSubclassOf<APoolActor>, TArray<TWeakObjectPtr<APoolActor>>> Pool;
};
