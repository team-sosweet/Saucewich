// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "MakeSandwich.h"

#include "EngineUtils.h"
#include "TimerManager.h"

#include "Saucewich.h"
#include "Entity/PickupSpawnVolume.h"
#include "GameMode/MakeSandwich/MakeSandwichState.h"

void AMakeSandwich::BeginPlay()
{
	Super::BeginPlay();

	if (PerkSpawnInterval > 0 && PerkClasses.Num() > 0)
	{
		for (TActorIterator<APickupSpawnVolume> It{GetWorld()}; It; ++It)
			PerkSpawnVolumes.Add(*It);

		if (PerkSpawnVolumes.Num() > 0)
			GetWorldTimerManager().SetTimer(PerkSpawnTimer, this, &AMakeSandwich::SpawnPerk, PerkSpawnInterval, true);
	}
}

bool AMakeSandwich::ReadyToEndMatch_Implementation()
{
	if (const auto GS = GetGameState<ASaucewichGameState>())
	{
		return GS->GetRemainingRoundSeconds() <= 0;
	}
	return false;
}

void AMakeSandwich::SpawnPerk() const
{
	PrintMessage("SpecialIngredientSpawned");
	PerkSpawnVolumes[FMath::RandHelper(PerkSpawnVolumes.Num())]->Spawn(PerkClasses[FMath::RandHelper(PerkClasses.Num())]);
}
