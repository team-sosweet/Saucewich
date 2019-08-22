// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "MakeSandwichPlayerState.h"
#include "Entity/ActorPool.h"
#include "GameMode/MakeSandwich/Entity/SandwichIngredient.h"
#include "SaucewichGameInstance.h"

void AMakeSandwichPlayerState::MulticastPickupIngredient_Implementation(const TSubclassOf<ASandwichIngredient> Class)
{
	++Ingredients.FindOrAdd(Class);
}

void AMakeSandwichPlayerState::PickupIngredient(const TSubclassOf<ASandwichIngredient> Class)
{
	if (!CanPickupIngredient()) return;
	MulticastPickupIngredient(Class);
}

uint8 AMakeSandwichPlayerState::GetNumIngredients() const
{
	uint8 Num = 0;
	for (auto&& Ingredient : Ingredients)
	{
		Num += Ingredient.Value;
	}
	return Num;
}

bool AMakeSandwichPlayerState::CanPickupIngredient() const
{
	return GetNumIngredients() < MaxIngredients;
}

void AMakeSandwichPlayerState::OnDeath()
{
	if (const auto GI = GetGameInstance<USaucewichGameInstance>())
	{
		for (auto&& Ingredient : Ingredients)
		{
			 GI->GetActorPool()->Spawn(
				 Ingredient.Key, GetPawn()->GetRootComponent()->GetComponentTransform()
			 );
		}
	}
	Ingredients.Reset();
}