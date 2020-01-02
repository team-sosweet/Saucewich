// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "PickupSpawnVolume.generated.h"

UCLASS()
class SAUCEWICH_API APickupSpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	APickupSpawnVolume();
	void Spawn(TSubclassOf<class APickup> Class) const;

private:	
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Volume;
};
