// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ActorPool.generated.h"

class APoolActor;
class USaucewichInstance;

UCLASS(NotBlueprintable, NotPlaceable)
class SAUCEWICH_API AActorPool final : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta=(DisplayName="Get Actor Pool", WorldContext=WorldContextObject))
	static AActorPool* Get(const UObject* WorldContextObject);
	static AActorPool* Get(const UWorld* World);
	static AActorPool* Get(const USaucewichInstance* SaucewichInstance);
	
	APoolActor* Spawn(TSubclassOf<APoolActor> Class, const FTransform& Transform = FTransform::Identity, const struct FActorSpawnParameters& SpawnParameters = DefaultParameters);

	template <class T>
	T* Spawn(const TSubclassOf<T> Class, const FTransform& Transform = FTransform::Identity, const FActorSpawnParameters& SpawnParameters = DefaultParameters)
	{
		static_assert(TIsDerivedFrom<T, APoolActor>::IsDerived, "T must be derived from APoolActor");
		return CastChecked<T>(Spawn(TSubclassOf<APoolActor>{Class}, Transform, SpawnParameters), ECastCheckedType::NullAllowed);
	}

	template <class T>
	T* Spawn(const FTransform& Transform = FTransform::Identity, const FActorSpawnParameters& SpawnParameters = DefaultParameters)
	{
		static_assert(TIsDerivedFrom<T, APoolActor>::IsDerived, "T must be derived from APoolActor");
		return CastChecked<T>(Spawn(T::StaticClass(), Transform, SpawnParameters), ECastCheckedType::NullAllowed);
	}

	void Release(APoolActor* Actor);

private:
	static const FActorSpawnParameters DefaultParameters;
	TMap<TSubclassOf<APoolActor>, TArray<TWeakObjectPtr<APoolActor>>> Pool;
};
