// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GameMode/MakeSandwich/Entity/SandwichIngredient.h"

#include "GameFramework/Pawn.h"

#include "Saucewich.h"
#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"
#include "Player/TpsCharacter.h"

void ASandwichIngredient::BePickedUp(ATpsCharacter* const By)
{
	static_cast<AMakeSandwichPlayerState*>(By->GetPlayerState())->PickupIngredient(GetClass());
	Super::BePickedUp(By);
}

void ASandwichIngredient::StartPickUp(ATpsCharacter* const By)
{
	Super::StartPickUp(By);
	auto& Picking = static_cast<AMakeSandwichPlayerState*>(By->GetPlayerState())->PickingUp;
	LOG_ASSERT(!Picking);
	Picking = this;
}

void ASandwichIngredient::CancelPickUp(ATpsCharacter* const By)
{
	Super::CancelPickUp(By);
	auto& Picking = static_cast<AMakeSandwichPlayerState*>(By->GetPlayerState())->PickingUp;
	LOG_ASSERT(Picking == this);
	Picking = nullptr;
}

bool ASandwichIngredient::CanPickedUp(const ATpsCharacter* const By) const
{
	const auto Player = static_cast<const AMakeSandwichPlayerState*>(By->GetPlayerState());
	return (!Player->PickingUp || Player->PickingUp == this) && Player->CanPickupIngredient();
}

bool ASandwichIngredient::CanEverPickedUp(const ATpsCharacter* const By) const
{
	return By->GetPlayerState<AMakeSandwichPlayerState>() != nullptr;
}
