// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Entity/GameModeDependentLevelActor.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

void AGameModeDependentLevelActor::BeginPlay()
{
	if (GameModeClass && !GetWorld()->GetGameState()->GetDefaultGameMode()->IsA(GameModeClass))
	{
		Destroy();
		return;
	}
	
	Super::BeginPlay();
}
