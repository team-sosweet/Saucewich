// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GameMode/MakeSandwich/Entity/SandwichIngredient.h"

#include "GameFramework/Pawn.h"

#include "Saucewich.h"
#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"

void ASandwichIngredient::BePickedUp(AActor* const By)
{
	static_cast<AMakeSandwichPlayerState*>(static_cast<APawn*>(By)->GetPlayerState())->PickupIngredient(GetClass());
	Super::BePickedUp(By);
}

void ASandwichIngredient::StartPickUp(AActor* const By)
{
	auto& Picking = static_cast<AMakeSandwichPlayerState*>(static_cast<APawn*>(By)->GetPlayerState())->PickingUp;
	LOG_ASSERT(!Picking);
	Picking = this;
}

void ASandwichIngredient::CancelPickUp(AActor* const By)
{
	auto& Picking = static_cast<AMakeSandwichPlayerState*>(static_cast<APawn*>(By)->GetPlayerState())->PickingUp;
	LOG_ASSERT(Picking == this);
	Picking = nullptr;
}

bool ASandwichIngredient::CanPickedUp(const AActor* const By) const
{
	const auto Player = static_cast<const AMakeSandwichPlayerState*>(static_cast<const APawn*>(By)->GetPlayerState());
	return (!Player->PickingUp || Player->PickingUp == this) && Player->CanPickupIngredient();
}

bool ASandwichIngredient::CanEverPickedUp(const AActor* const By) const
{
	if (const auto Pawn = Cast<APawn>(By))
		return Pawn->GetPlayerState()->IsA<AMakeSandwichPlayerState>();
	return false;
}
