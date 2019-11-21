// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "MakeSandwich/MakeSandwichState.h"

#include "MakeSandwich/MakeSandwichPlayerState.h"
#include "MakeSandwich/Entity/SandwichIngredient.h"

void AMakeSandwichState::StoreIngredients(AMakeSandwichPlayerState* const Player)
{
	const auto Team = Player->GetTeam();
	auto& TeamIngredients = GetTeamIngredients(Team);
	
	for (auto& x : Player->GetIngredients())
		TeamIngredients.FindOrAdd(x.Key) += x.Value;

	auto bHasEveryIngredients = true;
	for (const auto& x : SandwichIngredients)
	{
		if (!TeamIngredients.Contains(x))
		{
			bHasEveryIngredients = false;
			break;
		}
	}
	if (bHasEveryIngredients)
	{
		auto Min = TNumericLimits<uint8>::Max();
		for (const auto& x : TeamIngredients) if (x.Value < Min) Min = x.Value;
		for (auto& x : TeamIngredients) x.Value -= Min;
		SetTeamScore(Team, GetTeamScore(Team) + Min);
	}

	TArray<TSubclassOf<ASandwichIngredient>> Ingredients;
	TArray<uint8> Num;
	for (const auto& x : TeamIngredients)
	{
		Ingredients.Add(x.Key);
		Num.Add(x.Value);
	}
	MulticastSetIngredients(Team, MoveTemp(Ingredients), MoveTemp(Num));
}

void AMakeSandwichState::MulticastSetIngredients_Implementation(const uint8 Team, const TArray<TSubclassOf<ASandwichIngredient>>& Ingredients, const TArray<uint8>& Num)
{
	if (!HasAuthority())
	{
		auto& TeamIngredients = GetTeamIngredients(Team);
		for (auto i = 0; i < Num.Num(); ++i)
		{
			TeamIngredients.Add(Ingredients[i], Num[i]);
		}
	}
}
