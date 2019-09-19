// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "MakeSandwich.h"

#include "EngineUtils.h"
#include "TimerManager.h"

#include "Entity/PickupSpawnVolume.h"

void AMakeSandwich::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (PerkSpawnInterval > 0 && PerkClasses.Num() > 0)
	{
		for (TActorIterator<APickupSpawnVolume> It{GetWorld()}; It; ++It)
			PerkSpawnVolumes.Add(*It);

		if (PerkSpawnVolumes.Num() > 0)
			GetWorldTimerManager().SetTimer(PerkSpawnTimer, this, &AMakeSandwich::SpawnPerk, PerkSpawnInterval, true);
	}
}

void AMakeSandwich::SpawnPerk() const
{
	PrintMessage("SpecialIngredientSpawned");
	PerkSpawnVolumes[FMath::RandHelper(PerkSpawnVolumes.Num())]->Spawn(PerkClasses[FMath::RandHelper(PerkClasses.Num())]);
}
