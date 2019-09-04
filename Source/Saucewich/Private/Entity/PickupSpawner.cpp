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
	RootComponent = Body;
	Body->BodyInstance.SetCollisionProfileNameDeferred("NoCollision");
}

void APickupSpawner::PickedUp()
{
	SetSpawnTimer();
}

float APickupSpawner::GetRemainingSpawnTime() const
{
	return FMath::Max(GetWorldTimerManager().GetTimerRemaining(SpawnTimer), 0.f);
}

void APickupSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (IsPendingKill()) return;
	
	SetSpawnTimer();
}

void APickupSpawner::Spawn()
{
	auto&& Transform = Body->GetComponentTransform();
	
	FActorSpawnParameters Parameters;
	Parameters.Owner = this;

	const auto Pool = GetGameInstance<USaucewichGameInstance>()->GetActorPool();
	if (const auto Pickup = static_cast<APickup*>(Pool->Spawn(*Class, Transform, Parameters)))
	{
		Pickup->bSpawnedFromSpawner = true;
	}
}

void APickupSpawner::SetSpawnTimer()
{
	if (!IsNetMode(NM_Client))
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &APickupSpawner::Spawn, SpawnInterval);
}
