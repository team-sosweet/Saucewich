// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "ActorPoolComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAUCEWICH_API UActorPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActorPoolComponent() = default;

	template <class T>
	T* GetActor(UClass* Class, const FTransform& Transform = FTransform{}, const FActorSpawnParameters& SpawnParam = FActorSpawnParameters{})
	{
		for (AActor* const Actor : ActorPool)
		{
			T* const t = static_cast<T*>(Actor);
			if (!t->IsUsing())
			{
				t->SetActorLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());
				t->SetUsing(true);
				return t;
			}
		}
		T* const SpawnedActor = GetWorld()->SpawnActor<T>(Class, Transform, SpawnParam);
		if (SpawnedActor)
		{
			ActorPool.Add(SpawnedActor);
		}
		return SpawnedActor;
	}

private:
	TArray<AActor*> ActorPool;
};
