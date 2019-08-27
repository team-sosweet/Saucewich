// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GameMode/MakeSandwich/Entity/SandwichIngredient.h"

#include "GameFramework/Pawn.h"

#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"

void ASandwichIngredient::BePickedUp(AActor* const By)
{
	static_cast<AMakeSandwichPlayerState*>(static_cast<APawn*>(By)->GetPlayerState())->PickupIngredient(GetClass());
	Super::BePickedUp(By);
}

bool ASandwichIngredient::CanPickedUp(const AActor* By) const
{
	if (const auto Pawn = Cast<APawn>(By))
		if (const auto Player = Cast<AMakeSandwichPlayerState>(Pawn->GetPlayerState()))
			return Player->CanPickupIngredient();
	return false;
}
