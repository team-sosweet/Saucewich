// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GameMode/MakeSandwich/Entity/SandwichIngredient.h"

#include "TimerManager.h"

#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"
#include "Player/TpsCharacter.h"

static AMakeSandwichPlayerState* GetPlayer(AActor* const Actor)
{
	if (const auto Pawn = Cast<APawn>(Actor))
		return Pawn->GetPlayerState<AMakeSandwichPlayerState>();
	return nullptr;
}

void ASandwichIngredient::NotifyActorBeginOverlap(AActor* const OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto Player = GetPlayer(OtherActor);
	if (!Player) return;

	if (PickupTime <= 0)
	{
		BePickedUp(Player);
	}
	else
	{
		auto Pickup = [this, Player]
		{
			if (Player->CanPickupIngredient())
			{
				BePickedUp(Player);
			}
			else
			{
				PickupTimers.Remove(Player);
			}
		};

		GetWorldTimerManager().SetTimer(PickupTimers.Add(Player), Pickup, PickupTime, false);
	}
}

void ASandwichIngredient::NotifyActorEndOverlap(AActor* const OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	ClearTimer(GetPlayer(OtherActor));
}

void ASandwichIngredient::OnReleased()
{
	Super::OnReleased();
	PickupTimers.Reset();
}

void ASandwichIngredient::BePickedUp(AMakeSandwichPlayerState* const Player)
{
	Player->PickupIngredient(GetClass());
	Release(true);
}

void ASandwichIngredient::ClearTimer(AMakeSandwichPlayerState* const Player)
{
	if (!Player) return;

	FTimerHandle Timer;
	if (PickupTimers.RemoveAndCopyValue(Player, Timer))
	{
		GetWorldTimerManager().ClearTimer(Timer);
	}
}
