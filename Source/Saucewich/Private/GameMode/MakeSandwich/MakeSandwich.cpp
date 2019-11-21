// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "MakeSandwich.h"

#include "EngineUtils.h"
#include "TimerManager.h"

#include "Entity/PickupSpawnVolume.h"

#define LOCTEXT_NAMESPACE ""

void AMakeSandwich::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (PerkSpawnInterval > 0 && PerkClasses.Num() > 0)
	{
		for (const auto SpawnVolume : TActorRange<APickupSpawnVolume>{GetWorld()})
			PerkSpawnVolumes.Add(SpawnVolume);

		if (PerkSpawnVolumes.Num() > 0)
			GetWorldTimerManager().SetTimer(PerkSpawnTimer, this, &AMakeSandwich::SpawnPerk, PerkSpawnInterval, true);
	}
}

void AMakeSandwich::SpawnPerk() const
{
	PrintMessage(LOCTEXT("SpecialIngredientSpawned", "Ư����ᰡ ��򰡿� ��Ÿ�����!"), EMsgType::Center);
	PerkSpawnVolumes[FMath::RandHelper(PerkSpawnVolumes.Num())]->Spawn(PerkClasses[FMath::RandHelper(PerkClasses.Num())]);
}
