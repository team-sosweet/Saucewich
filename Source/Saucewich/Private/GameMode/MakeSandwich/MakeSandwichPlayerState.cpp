// Copyright 2019 Othereum. All Rights Reserved.

#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"

#include "Engine/World.h"

#include "Entity/ActorPool.h"
#include "GameMode/MakeSandwich/MakeSandwichState.h"
#include "GameMode/MakeSandwich/Entity/SandwichIngredient.h"
#include "SaucewichInstance.h"
#include "Names.h"

using FIngMap = TMap<TSubclassOf<ASandwichIngredient>, uint8>;

FIngredients::FIngredients(AMakeSandwichPlayerState* const Owner)
	:Owner{Owner}
{
}

void FIngredients::Reset()
{
	Ingredients.Reset();
	OnModify({});
}

void FIngredients::Add(const TSubclassOf<ASandwichIngredient> Ing)
{
	++Ingredients.FindOrAdd(Ing);
	OnModify(Ing);
}

void FIngredients::OnModify(const TSubclassOf<ASandwichIngredient> NewIng) const
{
	check(Owner);
	Owner->BroadcastIngredientChanged(NewIng);
}

AMakeSandwichPlayerState::AMakeSandwichPlayerState()
	:Ingredients{this}
{
}

void AMakeSandwichPlayerState::PickupIngredient(const TSubclassOf<ASandwichIngredient> Class)
{
	if (HasAuthority())
	{
		AddScore(NAME("PickupIngredient"));
		MulticastPickupIngredient(Class);
	}
}

void AMakeSandwichPlayerState::MulticastPickupIngredient_Implementation(const TSubclassOf<ASandwichIngredient> Class)
{
	Ingredients.Add(Class);
	OnPickupIngredient();
}

void AMakeSandwichPlayerState::PutIngredientsInFridge()
{
	if (Ingredients.Get().Num() <= 0) return;

	const auto GameInstance = GetWorld()->GetGameInstanceChecked<USaucewichInstance>();
	const auto GameState = CastChecked<AMakeSandwichState>(GetWorld()->GetGameState());

	static const FName ScoreName = TEXT("PutIngredients");
	const auto NumIngredients = GetNumIngredients();
	const auto ScorePer = GameInstance->GetScoreData(ScoreName).Score;
	
	AddScore(ScoreName, NumIngredients * ScorePer);
	SetObjective(GetObjective() + NumIngredients);
	GameState->StoreIngredients(this);

	MulticastResetIngredients();
}

void AMakeSandwichPlayerState::BroadcastIngredientChanged(const TSubclassOf<ASandwichIngredient> NewIng) const
{
	OnIngredientChanged.Broadcast(NewIng);
	OnIngChangedNative.Broadcast(NewIng);
}

void AMakeSandwichPlayerState::MulticastResetIngredients_Implementation()
{
	Ingredients.Reset();
	OnPutIngredients();
}

uint8 AMakeSandwichPlayerState::GetNumIngredients() const
{
	uint8 Num = 0;
	for (auto&& Ingredient : Ingredients.Get())
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
			for (auto&& Ingredient : Ingredients.Get())
				for (auto i = 0; i < Ingredient.Value; ++i)
					AActorPool::Get(this)->Spawn(Ingredient.Key, Transform);
		}
	}
	Ingredients.Reset();
}
