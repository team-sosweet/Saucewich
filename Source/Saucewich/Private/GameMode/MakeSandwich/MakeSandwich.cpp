// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "MakeSandwich.h"

#include "GameMode/MakeSandwich/MakeSandwichState.h"

bool AMakeSandwich::ReadyToEndMatch_Implementation()
{
	if (const auto GS = GetGameState<ASaucewichGameState>())
	{
		return GS->GetRemainingRoundSeconds() <= 0;
	}
	return false;
}
