// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "MakeSandwich/MakeSandwichState.h"

#include "MakeSandwich/MakeSandwichPlayerState.h"
#include "MakeSandwich/Entity/SandwichIngredient.h"

void AMakeSandwichState::StoreIngredients(AMakeSandwichPlayerState* const Player)
{
	auto& Ingredients = Player->GetIngredients();
	const auto Team = Player->GetTeam();
	for (auto& x : Ingredients)
		MulticastSetIngredients(x.Key, TeamIngredients[Team].FindOrAdd(x.Key) += x.Value, Team);
	
}

void AMakeSandwichState::MulticastSetIngredients_Implementation(UClass* const Ingredient, const uint8 Num, const uint8 Team)
{
	TeamIngredients[Team].Add(Ingredient, Num);
}
