// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "PickupSpawner.h"

#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "Entity/ActorPool.h"
#include "Entity/Pickup.h"
#include "GameMode/SaucewichGameMode.h"
#include "GameMode/SaucewichGameState.h"
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
	SetSpawnTimer();
}

float APickupSpawner::GetSpawnInterval() const
{
	if (SpawnIntervalOverride > 0) return SpawnIntervalOverride;

	if (const auto GS = GetWorld()->GetGameState())
		if (const auto DefGm = GS->GetDefaultGameMode<ASaucewichGameMode>())
			return DefGm->GetPickupSpawnInterval();

	return 0;
}

float APickupSpawner::GetSpawnInterval(const AGameStateBase* const GS) const
{
	if (SpawnIntervalOverride > 0) return SpawnIntervalOverride;

	if (const auto DefGm = GS->GetDefaultGameMode<ASaucewichGameMode>())
		return DefGm->GetPickupSpawnInterval();

	return 0;
}

float APickupSpawner::GetRemainingSpawnTime() const
{
	if (HasAuthority())
		return FMath::Max(GetWorldTimerManager().GetTimerRemaining(SpawnTimer), 0.f);

	if (const auto GS = GetWorld()->GetGameState())
		return FMath::Max(GetSpawnInterval(GS) - (GS->GetServerWorldTimeSeconds() - TimerStartTime), 0.f);

	return 0;
}

void APickupSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickupSpawner, TimerStartTime);
}

void APickupSpawner::Spawn()
{
	if (!HasAuthority()) return;
	
	auto&& Transform = Body->GetComponentTransform();
	
	FActorSpawnParameters Parameters;
	Parameters.Owner = this;

	const auto Pool = AActorPool::Get(this);
	if (const auto Pickup = Pool->Spawn<APickup>(*Class, Transform, Parameters))
	{
		Pickup->bSpawnedFromSpawner = true;
	}
}

void APickupSpawner::SetSpawnTimer()
{
	if (!HasAuthority()) return;

	if (const auto GI = GetWorld()->GetGameInstance<USaucewichGameInstance>())
	{
		GI->SafeGameState([this](ASaucewichGameState* const GS)
		{
			TimerStartTime = GS->GetServerWorldTimeSeconds();
			const auto Interval = GetSpawnInterval(GS);
			if (Interval > 0)
			{
				GetWorldTimerManager().SetTimer(SpawnTimer, this, &APickupSpawner::Spawn, Interval);
			}
		});
	}
}
