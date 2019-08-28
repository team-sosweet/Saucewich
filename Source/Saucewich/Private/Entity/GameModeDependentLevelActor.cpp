// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Entity/GameModeDependentLevelActor.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

void AGameModeDependentLevelActor::BeginPlay()
{
	if (GameStateClass && !GetWorld()->GetGameState()->IsA(GameStateClass))
	{
		Destroy();
		return;
	}
	
	Super::BeginPlay();
}
