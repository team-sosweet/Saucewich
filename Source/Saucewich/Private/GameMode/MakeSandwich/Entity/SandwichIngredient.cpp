// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "GameMode/MakeSandwich/Entity/SandwichIngredient.h"

#include "GameFramework/Pawn.h"

#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"
#include "Player/TpsCharacter.h"

void ASandwichIngredient::OnPickedUp_Implementation(ATpsCharacter* const By)
{
	static_cast<AMakeSandwichPlayerState*>(By->GetPlayerState())->PickupIngredient(GetClass());
}

void ASandwichIngredient::StartPickUp_Implementation(ATpsCharacter* const By)
{
	Super::StartPickUp_Implementation(By);
	auto& Picking = static_cast<AMakeSandwichPlayerState*>(By->GetPlayerState())->PickingUp;
	Picking = this;
}

void ASandwichIngredient::CancelPickUp_Implementation(ATpsCharacter* const By)
{
	Super::CancelPickUp_Implementation(By);
	auto& Picking = static_cast<AMakeSandwichPlayerState*>(By->GetPlayerState())->PickingUp;
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
