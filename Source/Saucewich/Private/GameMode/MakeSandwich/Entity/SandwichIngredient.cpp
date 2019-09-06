// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GameMode/MakeSandwich/Entity/SandwichIngredient.h"

#include "GameFramework/Pawn.h"

#include "Saucewich.h"
#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"
#include "Player/TpsCharacter.h"

void ASandwichIngredient::BePickedUp_Implementation(ATpsCharacter* const By)
{
	static_cast<AMakeSandwichPlayerState*>(By->GetPlayerState())->PickupIngredient(GetClass());
	Super::BePickedUp_Implementation(By);
}

void ASandwichIngredient::StartPickUp_Implementation(ATpsCharacter* const By)
{
	Super::StartPickUp_Implementation(By);
	auto& Picking = static_cast<AMakeSandwichPlayerState*>(By->GetPlayerState())->PickingUp;
	GUARANTEE(!Picking);
	Picking = this;
}

void ASandwichIngredient::CancelPickUp_Implementation(ATpsCharacter* const By)
{
	Super::CancelPickUp_Implementation(By);
	auto& Picking = static_cast<AMakeSandwichPlayerState*>(By->GetPlayerState())->PickingUp;
	GUARANTEE(Picking == this);
	Picking = nullptr;
}

bool ASandwichIngredient::CanPickedUp_Implementation(const ATpsCharacter* const By) const
{
	const auto Player = static_cast<const AMakeSandwichPlayerState*>(By->GetPlayerState());
	return (!Player->PickingUp || Player->PickingUp == this) && Player->CanPickupIngredient();
}

bool ASandwichIngredient::CanEverPickedUp_Implementation(const ATpsCharacter* const By) const
{
	return By->GetPlayerState<AMakeSandwichPlayerState>() != nullptr;
}
