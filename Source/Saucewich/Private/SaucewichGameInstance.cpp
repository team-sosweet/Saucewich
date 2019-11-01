// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameInstance.h"

#include "Engine/World.h"

#include "GameMode/SaucewichGameState.h"

ASaucewichGameState* USaucewichGameInstance::GetGameState() const
{
	return GetWorld()->GetGameState<ASaucewichGameState>();
}
