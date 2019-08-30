// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "MakeSandwich/MakeSandwichState.h"

#include "MakeSandwich/MakeSandwichPlayerState.h"
#include "MakeSandwich/Entity/SandwichIngredient.h"

void AMakeSandwichState::StoreIngredients(AMakeSandwichPlayerState* const Player)
{
	const auto Team = Player->GetTeam();
	auto& TeamIngredients = GetTeamIngredients(Team);
	
	for (auto& x : Player->GetIngredients())
		MulticastSetIngredients(x.Key, TeamIngredients.FindOrAdd(x.Key) += x.Value, Team);

	auto Min = TNumericLimits<uint8>::Max();
	for (const auto& x : TeamIngredients) if (x.Value < Min) Min = x.Value;
	if (Min > 0)
	{
		for (auto& x : TeamIngredients) x.Value -= Min;
		SetTeamScore(Team, GetTeamScore(Team) + Min);
	}
}

void AMakeSandwichState::MulticastSetIngredients_Implementation(UClass* const Ingredient, const uint8 Num, const uint8 Team)
{
	if (!HasAuthority()) GetTeamIngredients(Team).Add(Ingredient, Num);
}
