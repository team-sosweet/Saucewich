// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "MakeSandwichPlayerState.h"

#include "Engine/World.h"

#include "Entity/ActorPool.h"
#include "GameMode/MakeSandwich/MakeSandwichState.h"
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

void AMakeSandwichPlayerState::PutIngredientsInFridge()
{
	if (Ingredients.Num() <= 0) return;
	
	if (const auto GS = GetWorld()->GetGameState<AMakeSandwichState>())
		GS->StoreIngredients(this);

	MulticastResetIngredients();
}

void AMakeSandwichPlayerState::MulticastResetIngredients_Implementation()
{
	Ingredients.Reset();
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
	DropIngredients();
}

void AMakeSandwichPlayerState::DropIngredients()
{	
	if (HasAuthority())
	{
		const auto GI = GetGameInstance<USaucewichGameInstance>();
		if (!GI) return;

		auto&& Transform = GetPawn()->GetRootComponent()->GetComponentTransform();
		for (auto&& Ingredient : Ingredients)
			for (auto i = 0; i < Ingredient.Value; ++i)
				GI->GetActorPool()->Spawn(Ingredient.Key, Transform);
	}
	Ingredients.Reset();
}
