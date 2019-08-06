// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ObjectSpawner.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "GameFramework/GameModeBase.h"

void AObjectSpawner::BeginPlay()
{
	Super::BeginPlay();

	const auto Gm = GetWorld()->GetAuthGameMode();
	if (Gm && Gm->GetClass() == GameMode)
	{
		FActorSpawnParameters Parameters;
		Parameters.Owner = this;
		GetWorld()->SpawnActor(ActorToSpawn, &GetRootComponent()->GetComponentTransform(), Parameters);
	}
}
