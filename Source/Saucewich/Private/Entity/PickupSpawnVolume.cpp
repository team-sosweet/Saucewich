// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "PickupSpawnVolume.h"

#include "Components/BoxComponent.h"
#include "TimerManager.h"

#include "Entity/ActorPool.h"
#include "Entity/Pickup.h"
#include "SaucewichGameInstance.h"

APickupSpawnVolume::APickupSpawnVolume()
	:Volume{CreateDefaultSubobject<UBoxComponent>("Volume")}
{
	RootComponent = Volume;
	Volume->BodyInstance.SetCollisionProfileNameDeferred("NoCollision");
}

void APickupSpawnVolume::Spawn(const TSubclassOf<class APickup> Class) const
{
	const auto Extent = Volume->GetScaledBoxExtent();
	const FTransform Transform{GetActorLocation() + FMath::RandPointInBox({-Extent, Extent})};
	GetGameInstance<USaucewichGameInstance>()->GetActorPool()->Spawn(Class, Transform);
}
