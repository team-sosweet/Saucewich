// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "MakeSandwichPlayerState.h"

#include "Engine/World.h"

#include "Entity/ActorPool.h"
#include "GameMode/MakeSandwich/MakeSandwichState.h"
#include "GameMode/MakeSandwich/Entity/SandwichIngredient.h"
#include "SaucewichGameInstance.h"

void AMakeSandwichPlayerState::PickupIngredient(const TSubclassOf<ASandwichIngredient> Class)
{
	if (HasAuthority())
	{
		AddScore("PickupIngredient");
		MulticastPickupIngredient(Class);
	}
}

void AMakeSandwichPlayerState::MulticastPickupIngredient_Implementation(const TSubclassOf<ASandwichIngredient> Class)
{
	++Ingredients.FindOrAdd(Class);
	OnPickupIngredient();
}

void AMakeSandwichPlayerState::PutIngredientsInFridge()
{
	if (Ingredients.Num() <= 0) return;

	if (const auto GS = GetWorld()->GetGameState<AMakeSandwichState>())
	{
		static const FName ScoreName = "PutIngredients";
		const auto NumIngredients = GetNumIngredients();
		AddScore(ScoreName, NumIngredients * GS->GetScoreData(ScoreName).Score);
		SetObjective(GetObjective() + NumIngredients);
		GS->StoreIngredients(this);
	}

	MulticastResetIngredients();
}

void AMakeSandwichPlayerState::MulticastResetIngredients_Implementation()
{
	Ingredients.Reset();
	OnPutIngredients();
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

void AMakeSandwichPlayerState::Reset()
{
	Super::Reset();
	Ingredients.Reset();
}

void AMakeSandwichPlayerState::OnDeath()
{
	Super::OnDeath();
	DropIngredients();
}

void AMakeSandwichPlayerState::OnCharDestroyed()
{
	DropIngredients();
}

void AMakeSandwichPlayerState::DropIngredients()
{	
	if (HasAuthority())
	{
		if (const auto Pawn = GetPawn())
		{
			auto&& Transform = Pawn->GetRootComponent()->GetComponentTransform();
			for (auto&& Ingredient : Ingredients)
				for (auto i = 0; i < Ingredient.Value; ++i)
					AActorPool::Get(this)->Spawn(Ingredient.Key, Transform);
		}
	}
	Ingredients.Reset();
}
