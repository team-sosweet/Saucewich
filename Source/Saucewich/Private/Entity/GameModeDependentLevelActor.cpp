// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Entity/GameModeDependentLevelActor.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

void AGameModeDependentLevelActor::BeginPlay()
{
	if (GameModeClass)
	{
		if (const auto World = GetWorld())
		{
			if (const auto GS = World->GetGameState())
			{
				if (const auto GmCls = GS->GetDefaultGameMode())
				{
					if (!GmCls->IsA(GameModeClass))
					{
						Destroy();
						return;
					}
				}
			}
		}
	}
	
	Super::BeginPlay();
}
