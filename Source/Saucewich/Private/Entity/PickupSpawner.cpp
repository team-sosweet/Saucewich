// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "PickupSpawner.h"

#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "Entity/ActorPool.h"
#include "Entity/Pickup.h"
#include "Online/SaucewichGameMode.h"
#include "Online/SaucewichGameState.h"
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
	if (HasAuthority())
	{
		bSpawnTimerFlag = !bSpawnTimerFlag;
		SetSpawnTimer();
	}
}

float APickupSpawner::GetSpawnInterval() const
{
	if (SpawnIntervalOverride > 0) return SpawnIntervalOverride;

	if (const auto GS = GetWorld()->GetGameState())
		if (const auto DefGm = GS->GetDefaultGameMode<ASaucewichGameMode>())
			return DefGm->GetPickupSpawnInterval();

	return 0;
}

float APickupSpawner::GetRemainingSpawnTime() const
{
	return FMath::Max(GetWorldTimerManager().GetTimerRemaining(SpawnTimer), 0.f);
}

void APickupSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (IsPendingKill()) return;

	if (HasAuthority())
	{
		bSpawnTimerFlag = !bSpawnTimerFlag;
		SetSpawnTimer();
	}
}

void APickupSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickupSpawner, bSpawnTimerFlag);
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

void APickupSpawner::SetSpawnTimer()
{
	GetWorld()->GetGameInstance<USaucewichGameInstance>()->SafeGameState([this](auto)
	{
		const auto Interval = GetSpawnInterval();
		if (Interval > 0)
		{
			if (HasAuthority()) GetWorldTimerManager().SetTimer(SpawnTimer, this, &APickupSpawner::Spawn, Interval);
			else GetWorldTimerManager().SetTimer(SpawnTimer, Interval, false);
		}
	});
}
