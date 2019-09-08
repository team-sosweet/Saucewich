// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/GameModeDependentLevelActor.h"
#include "PickupSpawnVolume.generated.h"

UCLASS()
class SAUCEWICH_API APickupSpawnVolume final : public AGameModeDependentLevelActor
{
	GENERATED_BODY()

public:
	APickupSpawnVolume();
	void Spawn(TSubclassOf<class APickup> Class) const;

private:	
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Volume;
};
