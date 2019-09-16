// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "PickupSpawner.h"

#include "Components/SphereComponent.h"
#include "TimerManager.h"

#include "Entity/ActorPool.h"
#include "Entity/Pickup.h"
#include "SaucewichGameInstance.h"

APickupSpawner::APickupSpawner()
	:Body{CreateDefaultSubobject<USphereComponent>("Body")}
{
	bReplicates = true;
	RootComponent = Body;
	Body->BodyInstance.SetCollisionProfileNameDeferred("NoCollision");
}

void APickupSpawner::PickedUp()
{
	if (HasAuthority()) MulticastSetSpawnTimer();
}

float APickupSpawner::GetRemainingSpawnTime() const
{
	return FMath::Max(GetWorldTimerManager().GetTimerRemaining(SpawnTimer), 0.f);
}

void APickupSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (IsPendingKill()) return;

	if (HasAuthority()) MulticastSetSpawnTimer();
}

void APickupSpawner::Spawn()
{
	if (!HasAuthority()) return;
	
	auto&& Transform = Body->GetComponentTransform();
	
	FActorSpawnParameters Parameters;
	Parameters.Owner = this;

	const auto Pool = GetGameInstance<USaucewichGameInstance>()->GetActorPool();
	if (const auto Pickup = static_cast<APickup*>(Pool->Spawn(*Class, Transform, Parameters)))
	{
		Pickup->bSpawnedFromSpawner = true;
	}
}

void APickupSpawner::MulticastSetSpawnTimer_Implementation()
{
	if (HasAuthority()) GetWorldTimerManager().SetTimer(SpawnTimer, this, &APickupSpawner::Spawn, SpawnInterval);
	else GetWorldTimerManager().SetTimer(SpawnTimer, SpawnInterval, false);
}
